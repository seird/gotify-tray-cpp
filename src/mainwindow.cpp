#include "mainwindow.h"
#include "settings.h"
#include "ui_mainwindow.h"
#include "messagewidget.h"
#include "cache.h"
#include "utils.h"

#include <QtCore>
#include <QApplication>
#include <QMessageBox>



MainWindow::MainWindow(MessageItemModel * messageItemModel, ApplicationItemModel * applicationItemModel, ApplicationProxyModel * applicationProxyModel, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(qApp->applicationName());

    this->messageItemModel = messageItemModel;
    ui->listView_messages->setModel(messageItemModel);

    this->applicationItemModel = applicationItemModel;
    this->applicationProxyModel = applicationProxyModel;
    ui->listView_applications->setModel(applicationProxyModel);

    // Do not expand the applications listview when resizing
    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);

    // Do not collapse the message list
    ui->splitter->setCollapsible(1, false);

    setFonts();
    setIcons();
    restoreWindowState();
    connectComponents();

    installEventFilter(this);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::connectComponents()
{
    connect(messageItemModel, &MessageItemModel::rowsInserted, this, &MainWindow::displayMessageWidgets);
    connect(ui->listView_applications->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::currentChangedCallback);
    connect(settings, &Settings::fontChanged, this, &MainWindow::setFonts);
    connect(settings, &Settings::sizeChanged, this, &MainWindow::setIcons);
    connect(settings, &Settings::showPriorityChanged, this, &MainWindow::showPriority);
}


void MainWindow::setFonts()
{
    ui->label_application->setFont(settings->selectedApplicationFont());

    QFont font = settings->applicationFont();
    for (int r=0; r<applicationItemModel->rowCount(); ++r)
        applicationItemModel->item(r)->setFont(font);

    for (int r=0; r<messageItemModel->rowCount(); ++r) {
        MessageWidget * messageWidget = static_cast<MessageWidget *>(ui->listView_messages->indexWidget(messageItemModel->index(r, 0)));
        messageWidget->setFonts();
    }
}


void MainWindow::setIcons()
{
    QString theme = Utils::getTheme();
    ui->pb_refresh->setIcon(QIcon("://res/themes/" + theme + "/refresh.svg"));
    ui->pb_delete_all->setIcon(QIcon("://res/themes/" + theme + "/trashcan.svg"));

    QSize labelSize = settings->statusLabelSize();
    ui->statusWidget->setFixedSize(labelSize);
    ui->statusWidget->refresh();

    QSize buttonSize = settings->mainButtonSize();
    ui->pb_refresh->setFixedSize(buttonSize);
    ui->pb_delete_all->setFixedSize(buttonSize);
    ui->pb_refresh->setIconSize(0.7*buttonSize);
    ui->pb_delete_all->setIconSize(0.9*buttonSize);

    ui->listView_applications->setIconSize(settings->applicationIconSize());

    for (int r=0; r<messageItemModel->rowCount(); ++r) {
        MessageWidget * messageWidget = static_cast<MessageWidget *>(ui->listView_messages->indexWidget(messageItemModel->index(r, 0)));
        messageWidget->setIcons();
    }
}


void MainWindow::showPriority(bool enabled)
{
    for (int r=0; r<messageItemModel->rowCount(); ++r) {
        MessageWidget * messageWidget = static_cast<MessageWidget *>(ui->listView_messages->indexWidget(messageItemModel->index(r, 0)));
        messageWidget->showPriority(enabled);
    }
}


QModelIndex MainWindow::selectedApplication()
{
    return ui->listView_applications->selectionModel()->currentIndex();
}


void MainWindow::bringToFront()
{
    ensurePolished();
    show();
    setWindowState((windowState() & ~Qt::WindowState::WindowMinimized) | Qt::WindowState::WindowActive);
    activateWindow();
    raise();
}


void MainWindow::enableButtons()
{
    ui->pb_delete_all->setEnabled(true);
    ui->pb_refresh->setEnabled(true);
}


void MainWindow::disableButtons()
{
    ui->pb_delete_all->setDisabled(true);
    ui->pb_refresh->setDisabled(true);
}


void MainWindow::enableApplications(bool select)
{
    ui->listView_applications->setEnabled(true);
    ui->listView_applications->setFocus();
    if (select)
        ui->listView_applications->setCurrentIndex(applicationProxyModel->index(0, 0));
}


void MainWindow::disableApplications()
{
    ui->listView_applications->clearSelection();
    ui->listView_applications->setDisabled(true);
}


void MainWindow::setActive()
{
    ui->statusWidget->setActive();
}


void MainWindow::setConnecting()
{
    ui->statusWidget->setConnecting();
}


void MainWindow::setError()
{
    ui->statusWidget->setError();
}


void MainWindow::displayMessageWidgets(const QModelIndex &parent, int first, int last)
{
    QString theme = Utils::getTheme();
    QApplication * app = qApp;

    for (int i=first; i<=last; ++i) {
        QModelIndex index = messageItemModel->index(i, 0, parent);
        if (!index.isValid())
            continue;
        MessageItem * item = messageItemModel->itemFromIndex(index);
        MessageWidget * messageWidget = new MessageWidget(item, QIcon(cache->getFile(item->appId())), ui->listView_messages);
        connect(messageWidget, &MessageWidget::deletionRequested, this, [this, item]{emit deleteMessage(item);});
        ui->listView_messages->setIndexWidget(index, messageWidget);

        app->processEvents();
    }
}


void MainWindow::currentChangedCallback(const QModelIndex &current, const QModelIndex &previous)
{
    ApplicationItem * item = applicationItemModel->itemFromIndex(applicationProxyModel->mapToSource(current));
    if (item) {
        ui->label_application->setText(item->text());
        emit applicationChanged(item);
    }
}


void MainWindow::refreshCallback()
{
    emit refresh();
}


void MainWindow::deleteAllCallback()
{
    if (!messageItemModel->rowCount())
        return;

    ApplicationItem * item = applicationItemModel->itemFromIndex(applicationProxyModel->mapToSource(selectedApplication()));

    QString text = item->allMessages() ? "Delete ALL messages?" : "Delete all '" + item->name() + "' messages?";
    if (QMessageBox::warning(this, "Are you sure?", text,
                             QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel,
                             QMessageBox::StandardButton::Cancel)
        != QMessageBox::StandardButton::Ok) {
        return;
    }

    emit deleteAll(item);
}


void MainWindow::storeWindowState()
{
    settings->setWindowGeometry(saveGeometry());
    settings->setWindowState(saveState());
    settings->setSplitterState(ui->splitter->saveState());
}

void MainWindow::restoreWindowState()
{
    restoreGeometry(settings->windowGeometry());
    restoreState(settings->windowState());
    ui->splitter->restoreState(settings->splitterState());
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
    emit hidden();
}


bool MainWindow::eventFilter(QObject * watched, QEvent * event)
{
    switch (event->type()) {
    case QEvent::Type::WindowActivate:
        emit activated();
        break;
    default:
        break;
    }

    return QMainWindow::eventFilter(watched, event);
}
