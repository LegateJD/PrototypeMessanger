#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr, int accountId = -1);
    ~Login();

private slots:
    void on_pushButton_clicked();

signals:
    void loginCompletedSignal(int account);

private:
    Ui::Login *ui;
    int accountId;
};

#endif // LOGIN_H
