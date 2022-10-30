#ifndef EDITACCOUNTWINDOW_H
#define EDITACCOUNTWINDOW_H

#include <QDialog>

namespace Ui {
class EditAccountWindow;
}

class EditAccountWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EditAccountWindow(QWidget *parent = nullptr, int accountId = -1);
    ~EditAccountWindow();
    void updateAccountInformation();

private slots:
    void on_pushButton_clicked();

private:
    Ui::EditAccountWindow *ui;
    int accountId;
};

#endif // EDITACCOUNTWINDOW_H
