#ifndef EDITCONTACTINFOWINDOW_H
#define EDITCONTACTINFOWINDOW_H

#include <QDialog>

namespace Ui {
class EditContactInfoWindow;
}

class EditContactInfoWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EditContactInfoWindow(QWidget *parent = nullptr, int accountId = -1, int contactId = -1);
    ~EditContactInfoWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::EditContactInfoWindow *ui;
    int accountId;
    int contactId;
};

#endif // EDITCONTACTINFOWINDOW_H
