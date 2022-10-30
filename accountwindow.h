#ifndef ACCOUNTWINDOW_H
#define ACCOUNTWINDOW_H

#include "conversationwindow.h"
#include "qsqlquerymodel.h"
#include <QMainWindow>
#include <QHash>

namespace Ui {
class AccountWindow;
}

class AccountWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AccountWindow(QWidget *parent = nullptr, int accountId = - 1);
    ~AccountWindow();
    void updateAccountInformation();
    QHash<int, ConversationWindow*>* conversationWindows;
    void updateContactList();
    void updateBlackList();

private:
    Ui::AccountWindow *ui;
    int accountId;
    QSqlQueryModel* contactsListModel;
    QSqlQueryModel* blackListModel;
    void closeEvent(QCloseEvent *event);
    void updateAccountName();

private slots:
    void contactsListViewContextMenu(const QPoint &pos);
    void blackListViewContextMenu(const QPoint &pos);
    void goToConversation();
    void changeAccountName();
    void changeAccountNameFromBlackList();
    void addAccountToBlackList();
    void removeContactFromBlackList();
    void onClosedConversationWindow();
    void on_actionExit_to_main_menu_triggered();
    void on_actionChange_account_name_triggered();
    void on_actionChange_account_password_triggered();
};

#endif // ACCOUNTWINDOW_H
