#include "mainwindow.h"

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
    for (const QString &locale : uiLanguages) {
            const QString baseName = "prototype_" + QLocale(locale).name();
            if (translator.load(path + "/" + baseName + ".qm")) {
                a.installTranslator(&translator);
                break;
            }
        }
    MainWindow w;
    w.show();
    return a.exec();
}
