#include "login.h"
#include "qmessagebox.h"
#include "qsqldatabase.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_login.h"
#include <QCryptographicHash>

Login::Login(QWidget *parent, int accountId) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->accountId = accountId;
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButton_clicked()
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
    QString passwordHash = QString(QCryptographicHash::hash(ui->passwordLine->text().toUtf8(),QCryptographicHash::Sha3_512).toHex());
    query.prepare("SELECT Count(*) FROM Accounts WHERE Password == :hash");
    query.bindValue(":hash", passwordHash);
    qDebug() << "Text" << ui->passwordLine->text();
    if(query.exec()){
        query.first();
        int count = query.value(0).toInt();
        if(count > 0){
            hide();
            db.close();
            emit loginCompletedSignal(this->accountId);
        } else{
            QMessageBox msgBox;
            msgBox.setText(tr("Incorrect password"));
            msgBox.exec();
        }
        qDebug() << "Succesful" << count;
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }

    db.close();
}

