#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setPermissions(int);

private slots:

    /*----Testing Permissions----*/
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();


    /*----Menu Navigation----*/
    void on_pushButton_POS_clicked();

    void on_pushButton_home_clicked();

    void on_pushButton_sales_clicked();

    void on_pushButton_members_clicked();

    void on_pushButton_admin_clicked();

    void on_pushButton_sales_daily_clicked();

    void on_pushButton_sales_member_clicked();

    void on_pushButton_sales_item_clicked();

    void on_pushButton_admin_member_clicked();

    void on_pushButton_admin_inventory_clicked();

private:
    Ui::MainWindow *ui;
    int index = 0; // Testing Permissions
};
#endif // MAINWINDOW_H
