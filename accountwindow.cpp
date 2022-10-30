#include "accountwindow.h"
#include "changepasswordwindow.h"
#include "conversationwindow.h"
#include "editaccountwindow.h"
#include "editcontactinfowindow.h"
#include "mainwindow.h"
#include "qdatetime.h"
#include "qsqlerror.h"
#include "qsqlfield.h"
#include "qsqlquery.h"
#include "qsqlrecord.h"
#include "ui_accountwindow.h"
#include <QCloseEvent>

AccountWindow::AccountWindow(QWidget *parent, int accountId) :
    QMainWindow(parent),
    ui(new Ui::AccountWindow)
{
    ui->setupUi(this);
    this->accountId = accountId;

    this->contactsListModel = new QSqlQueryModel();
    this->blackListModel = new QSqlQueryModel();
    this->conversationWindows = new QHash<int, ConversationWindow*>;

    updateContactList();
    updateBlackList();
    updateAccountName();

    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(contactsListViewContextMenu(QPoint)));
    connect(ui->blackList, SIGNAL(customContextMenuRequested(QPoint)), SLOT(blackListViewContextMenu(QPoint)));
}

void AccountWindow::updateAccountName(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database");
    if (!db.open())
       {
          qDebug() << "Error: connection with database failed";
       }
       else
       {
          qDebug() << "Database: connection ok";
       }

    QSqlQuery query;
    query.prepare("SELECT * FROM Accounts WHERE ID = :accountId");
    query.bindValue(":accountId", accountId);
    if(query.exec()){
        qDebug() << "Succesful";
        query.first();
        auto accountName = query.value(2).toString();
        ui->accountNameLabel->setText(tr("Account") + ": " + accountName);
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }

    db.close();
}

void AccountWindow::updateContactList()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database");
    if (!db.open())
       {
          qDebug() << "Error: connection with database failed";
       }
       else
       {
          qDebug() << "Database: connection ok";
       }

    QSqlQuery query;
    query.prepare("SELECT ac.ContactName, c.Id, a.Name, bl.AccountId , bl.ContactId  FROM Accounts a INNER JOIN Accounts_Contacts ac ON a.ID = ac.AccountId INNER JOIN Contacts c  ON ac.ContactId = c.Id LEFT JOIN BlackList bl ON c.Id = bl.ContactId AND a.ID = bl.AccountId  WHERE bl.AccountId IS NULL AND bl.ContactId IS NULL AND a.ID = :accountId;");
    query.bindValue(":accountId", accountId);
    if(query.exec()){
        qDebug() << "Succesful";
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }

    contactsListModel->setQuery(query);
    ui->listView->setModel(contactsListModel);
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(contactsListViewContextMenu(QPoint)));

    db.close();
}

void AccountWindow::updateBlackList()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database");
    if (!db.open())
       {
          qDebug() << "Error: connection with database failed";
       }
       else
       {
          qDebug() << "Database: connection ok";
       }

    QSqlQuery blackListQuery;
    blackListQuery.prepare("SELECT ac.ContactName, c.Id, a.Name FROM Accounts a INNER JOIN Accounts_Contacts ac ON a.ID = ac.AccountId INNER JOIN Contacts c  ON ac.ContactId = c.Id INNER JOIN BlackList bl ON c.Id = bl.ContactId AND a.ID = bl.AccountId  WHERE a.ID = :accountId;");
    blackListQuery.bindValue(":accountId", accountId);
    if(blackListQuery.exec()){
        qDebug() << "Succesful";
    } else{
        qDebug() << "Error with query" << blackListQuery.lastError().text();
    }
    blackListModel->setQuery(blackListQuery);
    ui->blackList->setModel(blackListModel);
    ui->blackList->setContextMenuPolicy(Qt::CustomContextMenu);
    db.close();
}

void AccountWindow::contactsListViewContextMenu(const QPoint &pos)
{
    QPoint globalpos = ui->listView->mapToGlobal(pos);

    QMenu menuBeyondItem;

    QMenu menuForItem;
    QAction* action_editElement = menuForItem.addAction(tr("Go to conversation"), this, SLOT(goToConversation()));
    menuForItem.addAction(tr("Add account to blacklist"), this, SLOT(addAccountToBlackList()));
    menuForItem.addAction(tr("Change account name"), this, SLOT(changeAccountName()));

    auto pointedItem = ui->listView->indexAt(pos);
    pointedItem.isValid();

    QAction* selectedAction;
    if(pointedItem.isValid()) {
        selectedAction = menuForItem.exec(globalpos);
        if(selectedAction) {
            if(selectedAction == action_editElement) {
                qDebug() << "Edit";
            }
        }
    }
}

void AccountWindow::changeAccountName(){
    auto indexes = ui->listView->selectionModel()->selectedIndexes();
    auto indexNumber = indexes.front().row();
    auto contactId = contactsListModel->record(indexNumber).field(1).value().toInt();
    auto editContactInfoWindow = new EditContactInfoWindow(this, accountId, contactId);
    editContactInfoWindow->setModal(true);
    editContactInfoWindow->show();
}

void AccountWindow::changeAccountNameFromBlackList(){
    auto indexes = ui->blackList->selectionModel()->selectedIndexes();
    auto indexNumber = indexes.front().row();
    auto contactId = blackListModel->record(indexNumber).field(1).value().toInt();
    auto editContactInfoWindow = new EditContactInfoWindow(this, accountId, contactId);
    editContactInfoWindow->setModal(true);
    editContactInfoWindow->show();
}

void AccountWindow::blackListViewContextMenu(const QPoint &pos)
{
    QPoint globalpos = ui->blackList->mapToGlobal(pos);

    QMenu menuBeyondItem;

    QMenu menuForItem;
    QAction* action_editElement = menuForItem.addAction(tr("Remove from blacklist"), this, SLOT(removeContactFromBlackList()));
    menuForItem.addAction(tr("Change account name"), this, SLOT(changeAccountNameFromBlackList()));

    auto pointedItem = ui->blackList->indexAt(pos);
    pointedItem.isValid();

    QAction* selectedAction;
    if(pointedItem.isValid()) {
        selectedAction = menuForItem.exec(globalpos);
        if(selectedAction) {
            if(selectedAction == action_editElement) {
                qDebug() << "Edit";
            }
        }
    }
}

void AccountWindow::removeContactFromBlackList()
{
    QModelIndexList indexes = ui->blackList->selectionModel()->selectedIndexes();
    auto indexNumber = indexes.front().row();
    auto contactId = blackListModel->record(indexNumber).field(1).value().toInt();
    auto contactName = blackListModel->record(indexNumber).field(0).value().toString();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database");
    if (!db.open())
       {
          qDebug() << "Error: connection with database failed";
       }
       else
       {
          qDebug() << "Database: connection ok";
       }

    QSqlQuery query;
    query.prepare("DELETE FROM BlackList WHERE AccountId = :accountId AND ContactId = :contactId;");
    query.bindValue(":accountId", accountId);
    query.bindValue(":contactId", contactId);
    if(query.exec()){
        qDebug() << "Succesful";
        updateContactList();
        updateBlackList();
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }

    db.close();
}

void AccountWindow::goToConversation()
{
    QModelIndexList indexes = ui->listView->selectionModel()->selectedIndexes();
    auto indexNumber = indexes.front().row();
    auto contactId = contactsListModel->record(indexNumber).field(1).value().toInt();
    auto contactName = contactsListModel->record(indexNumber).field(0).value().toString();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database");
    if (!db.open())
       {
          qDebug() << "Error: connection with database failed";
       }
       else
       {
          qDebug() << "Database: connection ok";
       }

    QSqlQuery query;
    query.prepare("SELECT ac.ContactName, c.Id, a.Name FROM Accounts a INNER JOIN Accounts_Contacts ac ON a.ID = ac.AccountId INNER JOIN Contacts c ON ac.ContactId = c.Id WHERE a.ID = :accountId;");
    query.bindValue(":accountId", accountId);
    if(query.exec()){
        qDebug() << "Succesful";
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }

    contactsListModel->setQuery(query);
    ui->listView->setModel(contactsListModel);
    auto currentAccountName = contactsListModel->record(0).field(2).value().toString();
    db.close();

    if(conversationWindows->contains(contactId)){
        //(*conversationWindows)[contactId]->raise();
    }
    else{
        auto conversationWindow = new ConversationWindow(this, this->accountId, contactId, contactName, currentAccountName);
        conversationWindow->show();
        connect(conversationWindow, &QWidget::destroyed, this, &AccountWindow::onClosedConversationWindow);
        conversationWindow->setAttribute(Qt::WA_DeleteOnClose);
        conversationWindows->insert(contactId, conversationWindow);
    }
}

void AccountWindow::addAccountToBlackList()
{
    QModelIndexList indexes = ui->listView->selectionModel()->selectedIndexes();
    auto indexNumber = indexes.front().row();
    auto contactId = contactsListModel->record(indexNumber).field(1).value().toInt();
    auto contactName = contactsListModel->record(indexNumber).field(0).value().toString();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database");
    if (!db.open())
       {
          qDebug() << "Error: connection with database failed";
       }
       else
       {
          qDebug() << "Database: connection ok";
       }

    QSqlQuery query;
    auto dateTime = QDateTime::currentDateTimeUtc();
    query.prepare("INSERT INTO BlackList (AccountId, ContactId, CreatedAt) VALUES (:accountId, :contactId, :date);");
    query.bindValue(":contactId", contactId);
    query.bindValue(":accountId", this->accountId);
    query.bindValue(":date", dateTime.toString("yyyy-MM-dd HH:mm:ss"));
    if(query.exec()){
        qDebug() << "Succesful";
        updateContactList();
        updateBlackList();
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }

    db.close();
}

AccountWindow::~AccountWindow()
{
    delete ui;
}

void AccountWindow::on_actionExit_to_main_menu_triggered()
{
    foreach (ConversationWindow* value, *conversationWindows){
        if(value->isVisible()){
            value->hide();
        }
    }

    this->hide();
    this->parentWidget()->show();
}


void AccountWindow::on_actionChange_account_name_triggered()
{
    auto editAccountWindow = new EditAccountWindow(this, accountId);
    editAccountWindow->setModal(true);
    editAccountWindow->show();
}


void AccountWindow::on_actionChange_account_password_triggered()
{
    auto changePasswordWindow = new ChangePasswordWindow(this, accountId);
    changePasswordWindow->setModal(true);
    changePasswordWindow->show();
}

void AccountWindow::updateAccountInformation()
{
    updateAccountName();
    auto accountWindow = static_cast<MainWindow*>(this->parent());
    accountWindow->updateAccountList();
}

void AccountWindow::onClosedConversationWindow()
{
    auto accountWindow = static_cast<ConversationWindow*>(sender());
    //accountWindow->updateAccountList();
}

void AccountWindow::closeEvent(QCloseEvent *event)
{
    foreach (ConversationWindow* value, *conversationWindows){
        if(value->isVisible()){
            value->hide();
        }
    }
    auto accountWindow = static_cast<MainWindow*>(this->parent());
    accountWindow->show();
    event->accept();
}

