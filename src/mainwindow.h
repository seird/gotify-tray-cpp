#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QIcon>
#include <QItemSelection>

#include "statuswidget.h"
#include "messageitemmodel.h"
#include "applicationitemmodel.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(MessageItemModel * messageItemModel, ApplicationItemModel * applicationItemModel, ApplicationProxyModel * applicationProxyModel, QWidget *parent = nullptr);
    ~MainWindow();
    void bringToFront();
    void enableButtons();
    void disableButtons();
    void enableApplications(bool select=true);
    void disableApplications();
    void setActive();
    void setConnecting();
    void setError();
    void storeWindowState();
    void restoreWindowState();
    void setIcons();
    QModelIndex selectedApplication();
    bool eventFilter(QObject * watched, QEvent * event);

signals:
    void refresh();
    void deleteAll(ApplicationItem * applicationItem);
    void deleteMessage(MessageItem * item);
    void applicationChanged(ApplicationItem * item);
    void hidden();
    void activated();

private slots:
    void displayMessageWidgets(const QModelIndex &parent, int first, int last);
    void currentChangedCallback(const QModelIndex &current, const QModelIndex &previous);
    void refreshCallback();
    void deleteAllCallback();

protected:
    void closeEvent(QCloseEvent *event);

private:
    MessageItemModel * messageItemModel;
    ApplicationItemModel * applicationItemModel;
    ApplicationProxyModel * applicationProxyModel;
    Ui::MainWindow * ui;
    
    void connectComponents();
};


#endif // MAINWINDOW_H
