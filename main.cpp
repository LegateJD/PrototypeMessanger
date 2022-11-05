#include "mainwindow.h"
#include "qsqlquery.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString path = QApplication::applicationDirPath();

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    /*for (const QString &locale : uiLanguages) {
        const QString baseName = "prototype_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }*/

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
    auto language = query.value(0).toString();
    if(!language.isEmpty()){
        const QString baseName = "prototype_" + QLocale(language).name();
        if (translator.load(path + "/" + baseName + ".qm")) {
            a.installTranslator(&translator);
        }
    }
    else{
        for (const QString &locale : uiLanguages) {
                const QString baseName = "prototype_" + QLocale(locale).name();
                if (translator.load(path + "/" + baseName + ".qm")) {
                    a.installTranslator(&translator);
                    break;
                }
            }
    }

    MainWindow w;
    w.show();
    return a.exec();
}
