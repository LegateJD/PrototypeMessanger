#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qsqlquerymodel.h"
#include "qtranslator.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void loginCompleted(int account);
    void updateAccountList();

protected:
  void changeEvent(QEvent*);

private slots:
    void on_listView_customContextMenuRequested(const QPoint &pos);
    void accountsListViewContextMenu(const QPoint &pos);
    void onListViewTrigger();
    void onListViewDelete();
    void on_actionCreate_account_triggered();
    void slotLanguageChanged(QAction* action);

private:
    Ui::MainWindow *ui;
    QSqlQueryModel* accountsListModel;
    QSqlQueryModel* contactsListModel;
    int currentAccountId;
    QString currentAccountName;
    void createLanguageMenu(void);
    void updateLanguages();
    void loadLanguage(const QString& rLanguage);
    QString m_langPath;
    QString m_currLang;
    QTranslator m_translator;
    QActionGroup* langGroup;
};
#endif // MAINWINDOW_H
