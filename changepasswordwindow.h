#ifndef CHANGEPASSWORDWINDOW_H
#define CHANGEPASSWORDWINDOW_H

#include <QDialog>

namespace Ui {
class ChangePasswordWindow;
}

class ChangePasswordWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePasswordWindow(QWidget *parent = nullptr, int accountId = - 1);
    ~ChangePasswordWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ChangePasswordWindow *ui;
    int accountId;
};

#endif // CHANGEPASSWORDWINDOW_H
