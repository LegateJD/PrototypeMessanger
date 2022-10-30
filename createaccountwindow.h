#ifndef CREATEACCOUNTWINDOW_H
#define CREATEACCOUNTWINDOW_H

#include <QDialog>

namespace Ui {
class CreateAccountWindow;
}

class CreateAccountWindow : public QDialog
{
    Q_OBJECT

signals:
    void accountCreatedSignal();

public:
    explicit CreateAccountWindow(QWidget *parent = nullptr);
    ~CreateAccountWindow();

private slots:
    void on_registerButton_clicked();

private:
    Ui::CreateAccountWindow *ui;
};

#endif // CREATEACCOUNTWINDOW_H
