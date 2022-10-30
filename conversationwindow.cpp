#include "conversationwindow.h"
#include "accountwindow.h"
#include "messagelabel.h"
#include "qevent.h"
#include "qmessagebox.h"
#include "qsqldatabase.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "qsqlrecord.h"
#include "qtimer.h"
#include "ui_conversationwindow.h"
#include <QSqlQueryModel>
#include <QLabel>
#include <QScrollBar>
#include <QDateTime>

ConversationWindow::ConversationWindow(QWidget *parent, int accountId, int contactId, QString accountName, QString contactName) :
    QDialog(parent),
    ui(new Ui::ConversationWindow)
{
    ui->setupUi(this);
    this->accountId = accountId;
    this->contactId = contactId;
    this->accountName = accountName;
    this->contactName = contactName;
    this->boxLayout = new QVBoxLayout(this);
    updateContactInfo();
    boxLayout->setAlignment(Qt::AlignBottom);

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

    auto accountsListModel = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("Select m.Id , m.Message_Text , m.Created_At , m.IsSentMessage , m.Accounts_ContactsAccountsID  , m.Accounts_ContactsContactsID  FROM Messages m INNER JOIN Accounts_Contacts ac ON m.Accounts_ContactsAccountsID = ac.AccountId  AND m.Accounts_ContactsContactsID = ac.ContactId  WHERE ac.AccountId  = :accountId AND ac.ContactId  = :contactId;");
    query.bindValue(":accountId", accountId);
    query.bindValue(":contactId", contactId);
    if(query.exec()){
        qDebug() << "Succesful";
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }

    accountsListModel->setQuery(query);
    auto count = accountsListModel->rowCount();


    for(int i = 0; i < accountsListModel->rowCount(); ++i)
    {
        auto frame = new QFrame(this);
        auto messageLayout = new QVBoxLayout(this);
        auto messageId = accountsListModel->record(i).value(0).toInt();
        auto messageText = accountsListModel->record(i).value(1).toString();
        auto date = accountsListModel->record(i).value(2).toString();
        auto isSent = accountsListModel->record(i).value(3).toInt();
        auto messageLabel = new MessageLabel(messageId);
        messageLabel->setParent(frame);
        QDateTime qdateTime = QDateTime::fromString(date, "yyyy-MM-dd HH:mm:ss");
        qdateTime.setTimeSpec(Qt::UTC);
        auto dateLabel = new QLabel(qdateTime.toLocalTime().toString("yyyy-MM-dd HH:mm:ss"), frame);
        messageLabel->setText(messageText);
        frame->setMaximumHeight(60);
        if(isSent == 1) {
          messageLabel->setText(this->accountName + ": " + messageText);
          messageLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
          dateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
          frame->setStyleSheet("QFrame { background-color : #19919c; color : white; }");
        } else{
            messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            messageLabel->setText(this->contactName + ": " + messageText);
            dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            frame->setStyleSheet("QFrame { background-color : #199c52; color : white; }");
        }
        frame->setContextMenuPolicy(Qt::ActionsContextMenu);
        auto action = frame->addAction(tr("Delete message"), this, SLOT(onMessageDeleted()));
        QVariant v = QVariant::fromValue(messageLabel);
        action->setData(v);
        messageLayout->addWidget(messageLabel, Qt::AlignRight);
        messageLayout->addWidget(dateLabel);
        frame->setLayout(messageLayout);
        boxLayout->addWidget(frame);
    }

    ui->scrollAreaWidgetContents->setLayout(boxLayout);
    QTimer::singleShot(200, [this]{
           ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
       });
}

void ConversationWindow::updateContactInfo(){
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
    query.prepare("SELECT * FROM Contacts c INNER JOIN Accounts_Contacts ac ON c.Id = ac.ContactId WHERE ac.ContactId = :contactId AND ac.AccountId = :accountId;");
    query.bindValue(":contactId", contactId);
    query.bindValue(":accountId", accountId);
    if(query.exec()){
        qDebug() << "Succesful";
        query.first();
        auto contactName = query.value(4).toString();
        auto contactUserId = query.value(2).toString();
        ui->contactNameLabel->setText(tr("Contact name") + ": " + contactName);
        ui->contactUserIdLabel->setText(tr("Contact Global User Id") + ": " + contactUserId);

        qDebug() << "Error with query" << query.lastError().text();
    }

    db.close();
}


ConversationWindow::~ConversationWindow()
{
    delete ui;
}

void ConversationWindow::onMessageDeleted()
{
    auto action = static_cast<QAction*>(sender());
    auto variant = action->data();
    auto message = variant.value<MessageLabel*>();
    auto messageId = message->messageId;
    auto frame = static_cast<QFrame*>(message->parent());
    //boxLayout->removeWidget(message);

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

    auto accountsListModel = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("Delete From Messages WHERE Id = :messageId;");
    query.bindValue(":messageId", messageId);
    if(query.exec()){
        qDebug() << "Succesful";
        boxLayout->removeWidget(frame);
        frame->close();
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }

    db.close();
}

void ConversationWindow::on_pushButton_clicked()
{
    auto messageText = ui->textEdit->toPlainText();

    if(messageText.isEmpty()){
        return;
    }

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

    query.prepare("INSERT INTO Messages (Message_Text,Created_At,IsSentMessage,Accounts_ContactsAccountsID,Accounts_ContactsContactsID) VALUES (:messageText,:dateTime,1,:accountId,:contactId);");
    query.bindValue(":messageText", messageText);
    query.bindValue(":accountId", accountId);
    query.bindValue(":contactId", contactId);
    query.bindValue(":dateTime", dateTime.toString("yyyy-MM-dd HH:mm:ss"));
    if(query.exec()){
        auto frame = new QFrame(this);
        auto messageLayout = new QVBoxLayout(this);
        auto dateLabel = new QLabel(dateTime.toLocalTime().toString("yyyy-MM-dd HH:mm:ss"), frame);
        qDebug() << "Succesful";
        query.prepare("SELECT last_insert_rowid();");
        query.exec();
        query.first();
        auto lastId = query.value(0).toInt();
        auto messageLabel = new MessageLabel(lastId);
        messageLabel->setText(messageText);
        messageLabel->setText(this->accountName + ": " + messageText);
        frame->setStyleSheet("QFrame { background-color : #19919c; color : white; }");
        messageLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        dateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        frame->setContextMenuPolicy(Qt::ActionsContextMenu);
        auto action = frame->addAction(tr("Delete message"), this, SLOT(onMessageDeleted()));
        QVariant v = QVariant::fromValue(messageLabel);
        action->setData(v);
        frame->setMaximumHeight(60);
        messageLayout->addWidget(messageLabel, Qt::AlignRight);
        messageLayout->addWidget(dateLabel);
        frame->setLayout(messageLayout);
        boxLayout->addWidget(frame);
        ui->textEdit->setText("");
        QTimer::singleShot(100, [this]{
               ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
           });
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }
    db.close();
}

void ConversationWindow::closeEvent(QCloseEvent *event)
{
    auto accountWindow = static_cast<AccountWindow*>(this->parent());
    accountWindow->conversationWindows->remove(this->contactId);
    event->accept();
}

