#include "editcontactinfowindow.h"
#include "accountwindow.h"
#include "qmessagebox.h"
#include "qsqldatabase.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_editcontactinfowindow.h"

EditContactInfoWindow::EditContactInfoWindow(QWidget *parent, int accountId, int contactId) :
    QDialog(parent),
    ui(new Ui::EditContactInfoWindow)
{
    ui->setupUi(this);
    this->accountId = accountId;
    this->contactId = contactId;
}

EditContactInfoWindow::~EditContactInfoWindow()
{
    delete ui;
}

void EditContactInfoWindow::on_pushButton_clicked()
{
    auto newContactName = ui->newContactNameLine->text();
    if(newContactName.isEmpty()){
        QMessageBox msgBox;
        msgBox.setText(tr("Contact name cannot be empty"));
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
    query.prepare("SELECT COUNT(*) FROM Accounts_Contacts WHERE AccountId = :accountId AND UPPER(ContactName) = UPPER(:contactName);");
    query.bindValue(":accountId", accountId);
    query.bindValue(":contactId", contactId);
    query.bindValue(":contactName", newContactName);
    if(query.exec()){
        qDebug() << "Succesful";
        query.first();
        auto count = query.value(0).toInt();
        if(count > 0){
            QMessageBox::critical(this, tr("Validation error"), tr("Contact name is already taken"));
            return;
        }
    } else{
        qDebug() << "Error with query" << query.lastError().text();
        return;
    }

    query.prepare("UPDATE Accounts_Contacts SET ContactName=:contactName WHERE AccountId = :accountId AND ContactId = :contactId;");
    query.bindValue(":accountId", accountId);
    query.bindValue(":contactName", newContactName);
    query.bindValue(":contactId", contactId);
    if(query.exec()){
        qDebug() << "Succesful";
        auto accountWindow = static_cast<AccountWindow*>(this->parent());
        accountWindow->updateContactList();
        accountWindow->updateBlackList();
        close();
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }
}

