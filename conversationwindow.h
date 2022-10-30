#ifndef CONVERSATIONWINDOW_H
#define CONVERSATIONWINDOW_H

#include "qboxlayout.h"
#include <QDialog>

namespace Ui {
class ConversationWindow;
}

class ConversationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConversationWindow(QWidget *parent = nullptr, int accountId = -1, int contactId = -1, QString accountName = nullptr, QString contactName = nullptr);
    ~ConversationWindow();

private slots:
    void on_pushButton_clicked();
    void onMessageDeleted();

private:
    Ui::ConversationWindow *ui;
    int accountId;
    int contactId;
    QString accountName;
    QString contactName;
    QVBoxLayout* boxLayout;
    void closeEvent(QCloseEvent *event);
    void updateContactInfo();
};

#endif // CONVERSATIONWINDOW_H
