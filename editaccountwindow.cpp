#include "editaccountwindow.h"
#include "accountwindow.h"
#include "qsqldatabase.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_editaccountwindow.h"
#include "qmessagebox.h"

EditAccountWindow::EditAccountWindow(QWidget *parent, int accountId) :
    QDialog(parent),
    ui(new Ui::EditAccountWindow)
{
    ui->setupUi(this);
    this->accountId = accountId;
}

EditAccountWindow::~EditAccountWindow()
{
    delete ui;
}

void EditAccountWindow::on_pushButton_clicked()
{
    auto changedAccountName = ui->accountNameLine->text();

    if(changedAccountName.isEmpty()){
        QMessageBox::information(this, tr("Validation error"), tr("Account name cannot be empty"));
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
    query.prepare("SELECT COUNT(*) FROM Accounts WHERE UPPER(Name) = UPPER(:accountName);");
    query.bindValue(":accountName", changedAccountName);
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

    query.prepare("UPDATE Accounts SET Name=:accountName WHERE ID=:accountId;");
    query.bindValue(":accountId", accountId);
    query.bindValue(":accountName", changedAccountName);
    if(query.exec()){
        qDebug() << "Succesful";
        updateAccountInformation();
        hide();
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }
}

void EditAccountWindow::updateAccountInformation()
{
    auto accountWindow = static_cast<AccountWindow*>(this->parent());
    accountWindow->updateAccountInformation();
}

