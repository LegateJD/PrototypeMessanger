#include "changepasswordwindow.h"
#include "qmessagebox.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_changepasswordwindow.h"
#include <QCryptographicHash>

ChangePasswordWindow::ChangePasswordWindow(QWidget *parent, int accountId) :
    QDialog(parent),
    ui(new Ui::ChangePasswordWindow)
{
    ui->setupUi(this);
    this->accountId = accountId;
}

ChangePasswordWindow::~ChangePasswordWindow()
{
    delete ui;
}

void ChangePasswordWindow::on_pushButton_clicked()
{
    auto oldPassword = ui->actualPasswordLine->text();
    auto firstPassword = ui->firstPasswordLine->text();
    auto secondPassword = ui->secondPasswordLine->text();

    if(oldPassword.isEmpty()){
        QMessageBox::information(this, tr("Validation error"), tr("Password cannot be empty"));
        return;
    }

    if(firstPassword.isEmpty()){
        QMessageBox::information(this, tr("Validation error"), tr("Password cannot be empty"));
        return;
    }

    if(secondPassword.isEmpty()){
        QMessageBox::information(this, tr("Validation error"), tr("Password cannot be empty"));
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

    QString oldPasswordHash = QString(QCryptographicHash::hash(oldPassword.toUtf8(),QCryptographicHash::Sha3_512).toHex());
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Accounts WHERE ID = :accountId AND Password = :password");
    query.bindValue(":password", oldPasswordHash);
    query.bindValue(":accountId", accountId);
    if(query.exec()){
        qDebug() << "Succesful";
        query.first();
        auto count = query.value(0).toInt();
        if(count < 1){
            QMessageBox::critical(this, tr("Validation error"), tr("Incorrect  old password"));
            return;
        }
    } else{
        qDebug() << "Error with query" << query.lastError().text();
        return;
    }

    if(firstPassword == secondPassword){
        QSqlQuery query;
        QString passwordHash = QString(QCryptographicHash::hash(firstPassword.toUtf8(),QCryptographicHash::Sha3_512).toHex());
        query.prepare("UPDATE Accounts SET Password=:password WHERE ID=:accountId;");
        query.bindValue(":password", passwordHash);
        query.bindValue(":accountId", accountId);
        if(query.exec()){
            hide();
            qDebug() << "Succesful";
        } else{
            qDebug() << "Error with query" << query.lastError().text();
        }
    }
    else{
        QMessageBox::critical(this, tr("Validation error"), tr("Incorrect  Passwords"));
    }

    db.close();
}

