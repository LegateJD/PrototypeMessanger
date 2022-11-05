#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "accountwindow.h"
#include "conversationwindow.h"
#include "createaccountwindow.h"
#include "login.h"
#include "qstringlistmodel.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QListWidgetItem>
#include <QListView>
#include <QMessageBox>
#include <QAction>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>
#include <QActionGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    accountsListModel = new QSqlQueryModel();
    contactsListModel = new QSqlQueryModel();
    updateAccountList();
    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(accountsListViewContextMenu(QPoint)));
    this->createLanguageMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::accountsListViewContextMenu(const QPoint &pos)
{
    QPoint globalpos = ui->listView->mapToGlobal(pos);

    QMenu menuBeyondItem;
    QAction* action_addElement = menuBeyondItem.addAction("Add");

    QMenu menuForItem;
    QAction* action_editElement = menuForItem.addAction(tr("Sign In"), this, SLOT(onListViewTrigger()));
    menuForItem.addAction(tr("Delete account"), this, SLOT(onListViewDelete()));

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

void MainWindow::onListViewDelete()
{
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, tr("Account removal"), tr("Delete this account?"),
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        QModelIndexList indexes = ui->listView->selectionModel()->selectedIndexes();
        auto indexNumber = indexes.front().row();
        auto accountId = accountsListModel->record(indexNumber).field(1).value().toInt();
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
        query.prepare("PRAGMA foreign_keys = ON;");
        query.exec();
        query.prepare("DELETE FROM Accounts WHERE ID = :accountId;");
        query.bindValue(":accountId", accountId);
        if(query.exec()){
            qDebug() << "Succesful";
            updateAccountList();
            query.prepare("DELETE FROM Contacts WHERE ID NOT IN (SELECT ContactId FROM Accounts_Contacts);");
            query.exec();
        } else{
            qDebug() << "Error with query" << query.lastError().text();
        }
      } else {
        qDebug() << "Yes was *not* clicked";
      }
}

void MainWindow::onListViewTrigger()
{
    qDebug() << "Succesful";
    QModelIndexList indexes = ui->listView->selectionModel()->selectedIndexes();
    auto indexNumber = indexes.front().row();
    auto accountId = accountsListModel->record(indexNumber).field(1).value().toInt();
    Login* login = new Login(this, accountId);
    connect(login, SIGNAL(loginCompletedSignal(int)), SLOT(loginCompleted(int)));
    login->setModal(true);
    login->show();
}

void MainWindow::loginCompleted(int accountId)
{
    AccountWindow* accountWindow = new AccountWindow(this, accountId);
    accountWindow->show();
    this->hide();
}

void MainWindow::on_listView_customContextMenuRequested(const QPoint &pos)
{
}


void MainWindow::on_actionCreate_account_triggered()
{
    CreateAccountWindow* createAccountWindow = new CreateAccountWindow(this);
    connect(createAccountWindow, SIGNAL(accountCreatedSignal()), SLOT(updateAccountList()));
    createAccountWindow->setModal(true);
    createAccountWindow->show();
}

void MainWindow::createLanguageMenu(void) {
 langGroup = new QActionGroup(ui->menuLanguage);
 connect(ui->menuLanguage, SIGNAL (triggered(QAction *)), this, SLOT (slotLanguageChanged(QAction *)));

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
 query.prepare("SELECT Value FROM Settings WHERE Key = 'Language';");
 query.exec();
 qDebug() << "Succesful";
 query.first();
 auto defaultLocale = query.value(0).toString();

 //QString defaultLocale = QLocale::system().name();
 //defaultLocale.truncate(defaultLocale.lastIndexOf('_'));
 m_langPath = QApplication::applicationDirPath();
 QDir dir(m_langPath);
 QStringList fileNames = dir.entryList(QStringList("prototype_*.qm"));

 for (int i = 0; i < fileNames.size(); ++i) {
  QString locale;
  locale = fileNames[i];
  locale.truncate(locale.lastIndexOf('.'));
  locale.remove(0, locale.indexOf('_') + 1);

  QString lang = QLocale::languageToString(QLocale(locale).language());
  QByteArray ba = lang.toLocal8Bit();
  const char *c_str2 = ba.data();

  QAction *action = new QAction(tr(c_str2), this);
  action->setCheckable(true);
  action->setData(locale);

  langGroup->addAction(action);
  ui->menuLanguage->addAction(action);

  // set default translators and language checked
  if (defaultLocale == locale) {
   action->setChecked(true);
  }
 }
}

void MainWindow::updateLanguages()
{
    foreach(auto action, langGroup->actions()){
        auto locale = action->data().toString();
        QString lang = QLocale::languageToString(QLocale(locale).language());
        QByteArray ba = lang.toLocal8Bit();
        const char *c_str2 = ba.data();
        action->setText(tr(c_str2));
    }
}

void MainWindow::slotLanguageChanged(QAction* action)
{
 if(0 != action) {
  loadLanguage(action->data().toString());
  setWindowIcon(action->icon());
  updateLanguages();
 }
}

void switchTranslator(QTranslator& translator, const QString& filename) {
 qApp->removeTranslator(&translator);

 // load the new translator
QString path = QApplication::applicationDirPath();
 if(translator.load(path + "/" + filename))
  qApp->installTranslator(&translator);
}

void MainWindow::loadLanguage(const QString& rLanguage) {
 if(m_currLang != rLanguage) {
  m_currLang = rLanguage;
  QLocale locale = QLocale(m_currLang);
  QLocale::setDefault(locale);
  QString languageName = QLocale::languageToString(locale.language());
  switchTranslator(m_translator, QString("prototype_%1.qm").arg(rLanguage));
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
  query.prepare("UPDATE Settings SET Value = :lang WHERE Key = 'Language'");
  query.bindValue(":lang", rLanguage);
  if(query.exec()){
      qDebug() << "Succesful";
  } else{
      qDebug() << "Error with query" << query.lastError().text();
  }
  db.close();
 }
}

void MainWindow::changeEvent(QEvent* event) {
 if(0 != event) {
  switch(event->type()) {
   case QEvent::LanguageChange:
      ui->retranslateUi(this);
    break;
   case QEvent::LocaleChange:
   {
    QString locale = QLocale::system().name();
    locale.truncate(locale.lastIndexOf('_'));
    loadLanguage(locale);
   }
   break;
  }
 }
 QMainWindow::changeEvent(event);
}

void MainWindow::updateAccountList() {
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
    query.prepare("SELECT Name, ID FROM Accounts");
    if(query.exec()){
        qDebug() << "Succesful";
    } else{
        qDebug() << "Error with query" << query.lastError().text();
    }

    accountsListModel->setQuery(query);
    ui->listView->setModel(accountsListModel);
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    db.close();
}
