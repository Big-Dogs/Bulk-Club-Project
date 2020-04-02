#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    int getIndex(); //Retrieves Permission level

private slots:
    void on_pushButton_login_clicked(); // Checking Permissions

private:
    Ui::LoginWindow *ui;
    int index = 0; // Permission Level
};

#endif // LOGINWINDOW_H
