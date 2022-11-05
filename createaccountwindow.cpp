#include "createaccountwindow.h"
#include "qdatetime.h"
#include "qmessagebox.h"
#include "qsqldatabase.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_createaccountwindow.h"
#include <QUuid>
#include <QCryptographicHash>

CreateAccountWindow::CreateAccountWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateAccountWindow)
{
    ui->setupUi(this);
}

CreateAccountWindow::~CreateAccountWindow()
{
    delete ui;
}

void CreateAccountWindow::on_registerButton_clicked()
{
    auto userName = ui->userNameLineEdit->text();
    auto password = ui->passwordLineEdit->text();

    if(userName.isEmpty()){
        QMessageBox msgBox;
        msgBox.setText(tr("Account name cannot be empty"));
        msgBox.exec();
        return;
    }

    if(password.isEmpty()){
        QMessageBox msgBox;
        msgBox.setText(tr("Password cannot be empty"));
        msgBox.exec();
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
    query.prepare("SELECT COUNT(*) FROM Accounts WHERE UPPER(Name) = UPPER(:accountName)");
    query.bindValue(":accountName", userName);
    if(query.exec()){
        qDebug() << "Succesful";
        query.first();
        auto count = query.value(0).toInt();
        if(count > 0){
            QMessageBox::critical(this, tr("Validation error"), tr("Account name is already taken"));
            return;
        }
    } else{
        qDebug() << "Error with query" << query.lastError().text();
        return;
    }

    auto userUUID = QUuid::createUuid().toString();
    auto userId = QString(QCryptographicHash::hash(userUUID.toUtf8(),QCryptographicHash::Md5).toHex());
    auto passwordHash = QString(QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha3_512).toHex());
    auto dateTime = QDateTime::currentDateTimeUtc();
    query.prepare("INSERT INTO Accounts (User_Id,Name,Password,Created_At) VALUES (:userId, :userName, :password,:date);");
    query.bindValue(":userId", userId);
    query.bindValue(":password", passwordHash);
    query.bindValue(":userName", userName);
    query.bindValue(":date", dateTime.toString("yyyy-MM-dd HH:mm:ss"));
    if(query.exec()){
        qDebug() << "Succesful";
        query.prepare("SELECT last_insert_rowid();");
        query.exec();
        query.first();
        auto lastId = query.value(0).toInt();
        query.prepare("INSERT INTO Keys (Public_Key,Private_Key,Created_At,AccountId) VALUES ('asffffffffffffffffffklhjkjghkjshdkjghsdkg','hfdhdfghhfhfdhfffffffffffffklhjkjghkjshdkjghsdkg',:date,:accountId);");
        query.bindValue(":date", dateTime.toString("yyyy-MM-dd HH:mm:ss"));
        query.bindValue(":accountId", lastId);
        query.exec();
        emit accountCreatedSignal();
        hide();
    } else{
        qDebug() << "Error with query" << query.lastError().text();
        QMessageBox msgBox;
        msgBox.setText(tr("Incorrect password"));
        msgBox.exec();
    }
}

