#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget_main->setCurrentIndex(0);
    ui->stackedWidget_sales->setCurrentIndex(0);
    ui->stackedWidget_admin->setCurrentIndex(0);


    ui->pushButton_sales->setEnabled(false);
    ui->pushButton_members->setEnabled(false);
    ui->pushButton_admin->setEnabled(false);


    ui->gridWidget_admin_memberdatafields->hide();
    ui->gridWidget_admin_itemdatafields->hide();

}


MainWindow::~MainWindow()
{
    delete ui;
}

/*----Testing Permissions----*/
void MainWindow::setPermissions(int permission)
{
    if(permission == 0)
    {
        ui->pushButton_sales->setEnabled(false);
        ui->pushButton_members->setEnabled(false);
        ui->pushButton_admin->setEnabled(false);
    }
    else if (permission == 1)
    {
        ui->pushButton_sales->setEnabled(true);
        ui->pushButton_members->setEnabled(true);
        ui->pushButton_admin->setEnabled(false);
    }
    else if (permission == 2)
    {
        ui->pushButton_sales->setEnabled(true);
        ui->pushButton_members->setEnabled(true);
        ui->pushButton_admin->setEnabled(true);
    }
}

void MainWindow::on_pushButton_clicked()
{
    index = 0;
    setPermissions(index);
}

void MainWindow::on_pushButton_2_clicked()
{
    index = 1;
    setPermissions(index);
}

void MainWindow::on_pushButton_3_clicked()
{
    index = 2;
    setPermissions(index);
}


/*----Window Navigation----*/
void MainWindow::on_pushButton_home_clicked() // home page
{
    ui->stackedWidget_main->setCurrentIndex(0);
}

void MainWindow::on_pushButton_POS_clicked() // POS page
{
    ui->stackedWidget_main->setCurrentIndex(1);
}

void MainWindow::on_pushButton_sales_clicked() // sales page
{
    ui->stackedWidget_main->setCurrentIndex(2);
}
    void MainWindow::on_pushButton_sales_daily_clicked() // daily sales report
    {
        ui->stackedWidget_sales->setCurrentIndex(0);
    }

    void MainWindow::on_pushButton_sales_member_clicked() // sales by member
    {
        ui->stackedWidget_sales->setCurrentIndex(1);
    }

    void MainWindow::on_pushButton_sales_item_clicked() // sales by item
    {
        ui->stackedWidget_sales->setCurrentIndex(2);
    }

void MainWindow::on_pushButton_members_clicked() // membership page
{
    ui->stackedWidget_main->setCurrentIndex(3);
}

void MainWindow::on_pushButton_admin_clicked() // administrator tools
{
    ui->stackedWidget_main->setCurrentIndex(4);
}
    void MainWindow::on_pushButton_admin_member_clicked() // adding/deleting members
    {
        ui->stackedWidget_admin->setCurrentIndex(0);
    }

    void MainWindow::on_pushButton_admin_inventory_clicked() // adding/deleting inventory
    {
        ui->stackedWidget_admin->setCurrentIndex(1);
    }



/*----Admin Window push buttons----*/
    /*----Member Page----*/
void MainWindow::on_pushButton_admin_addmember_clicked() // add member button
{
    ui->gridWidget_admin_memberdatafields->show();
    ui->pushButton_admin_editmember->setEnabled(false);
    ui->pushButton_admin_deletemember->setEnabled(false);
}

void MainWindow::on_pushButton_admin_editmember_clicked() // edit member button
{
    ui->gridWidget_admin_memberdatafields->show();
    ui->pushButton_admin_deletemember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(false);
}

void MainWindow::on_pushButton_admin_deletemember_clicked() // delete member button
{

}

void MainWindow::on_pushButton_admin_membersubmission_clicked() // submit button for adding/editing
{
    ui->gridWidget_admin_memberdatafields->hide();
    ui->pushButton_admin_deletemember->setEnabled(true);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);
}

    /*----Inventory Page----*/
void MainWindow::on_pushButton_admin_additem_clicked() //add item button
{
    ui->gridWidget_admin_itemdatafields->show();
    ui->pushButton_admin_deleteitem->setEnabled(false);
}

void MainWindow::on_pushButton_admin_deleteitem_clicked()
{

}

void MainWindow::on_pushButton_admin_itemsubmission_clicked()
{
    ui->gridWidget_admin_itemdatafields->hide();
    ui->pushButton_admin_deleteitem->setEnabled(true);
}

/*----Membership Window push buttons----*/
void MainWindow::on_pushButton_membership_rebates_clicked()
{

}

void MainWindow::on_pushButton_membership_expiration_clicked()
{

}

void MainWindow::on_pushButton_membership_upgrades_clicked()
{

}

void MainWindow::on_pushButton_membership_downgrades_clicked()
{

}

/*----POS Page push buttons----*/
void MainWindow::on_pushButton_pos_purchase_clicked()
{

}


