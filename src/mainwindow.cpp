#include "mainwindow.h"
#include "settings.h"
#include "ui_mainwindow.h"
#include "messagewidget.h"
#include "cache.h"
#include "utils.h"

#include <QtCore>
#include <QApplication>
#include <QMessageBox>

#include <rapidfuzz/fuzz.hpp>


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
    ui->le_search->setEnabled(true);
}


void MainWindow::disableButtons()
{
    ui->pb_delete_all->setDisabled(true);
    ui->pb_refresh->setDisabled(true);
    ui->le_search->setDisabled(true);
}

void MainWindow::clearSearchField()
{
    ui->le_search->clear();
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

void MainWindow::searchMessages(QString query)
{
    RequestHandler *requestHandler = RequestHandler::getInstance();
    GotifyModel::Messages gotifyMessageList(requestHandler->getMessagesArray());
    if (query.isEmpty())
    {
        messageItemModel->clear();
        for (GotifyModel::Message *message : gotifyMessageList.messages)
        {
            messageItemModel->appendMessage(message);
        }
        return;
    }
    // this score cutoff is arbitrary and can be adjusted. score range is [0,100]
    const double score_cutoff = 10.0;
    // using std strings and remove all special characters like emojis
    std::string processed_query_string = query.toLower().toLatin1().constData();
    processed_query_string.erase(std::remove(processed_query_string.begin(), processed_query_string.end(), '?'),
                                 processed_query_string.end());
    ApplicationItem *currentApplicationItem =
        applicationItemModel->itemFromIndex(applicationProxyModel->mapToSource(selectedApplication()));
    int currentAppId = currentApplicationItem->id();
    std::vector<std::pair<GotifyModel::Message *, double>> results;
    rapidfuzz::fuzz::CachedRatio<QString> scorer(processed_query_string);
    QList<GotifyModel::Message *> messagesList = gotifyMessageList.messages;
    // search
    for (GotifyModel::Message *message : gotifyMessageList.messages)
    {
        if (currentAppId != message->appId && currentAppId != 0)
            continue;
        QString messageText = message->title.toLower() + " " + message->message.toLower();
        std::string processed_message_string = messageText.toLatin1().constData();
        processed_message_string.erase(
            std::remove(processed_message_string.begin(), processed_message_string.end(), '?'),
            processed_message_string.end());
        double score = scorer.similarity(processed_message_string, score_cutoff);
        if (messageText.toLatin1().indexOf(query.toLower().toLatin1()) != -1)
        {
            score += 100.0;
        }
        if (score >= score_cutoff)
        {
            results.emplace_back(std::make_pair(message, score));
        }
    }
    // sort results by score and then by message id
    std::sort(results.begin(), results.end(), [](const auto &a, const auto &b) {
        if (a.second == b.second)
        {
            return a.first->id > b.first->id;
        }
        return a.second > b.second;
    });
    // clear and apply the new model. take only the first 10 results
    messageItemModel->clear();
    int count = 0;
    for (const auto &result : results)
    {
        if (count >= 10)
            break;
        messageItemModel->appendMessage(result.first);
        count++;
    }
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
