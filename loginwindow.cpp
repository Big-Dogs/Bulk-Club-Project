#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_pushButton_login_clicked() //! Checking Permissions
{
    QString username = ui->lineEdit_username->text();
    QString password = ui->lineEdit_password->text();
    QString warning = "Invalid username or password. Please try again.";

    if (username == "employee" &&
        password == "test")
    {
        index = 0;
        close();
    }
    else if (username == "manager" &&
        password == "test")
    {
        index = 1;
        close();
    }
    else if (username == "admin" &&
        password == "test")
    {
        index = 2;
        close();
    }
    else
    {
        ui->label_warning->setText(warning);
    }

}

int LoginWindow::getIndex() //! Retrieves permission level
{
    return index;
}





