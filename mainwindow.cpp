#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget_main->setCurrentIndex(home); // setting default indices
    ui->stackedWidget_sales->setCurrentIndex(salesdaily);
    ui->stackedWidget_admin->setCurrentIndex(adminmember);


    ui->pushButton_sales->setEnabled(false); // hiding and greying stuff
    ui->pushButton_members->setEnabled(false);
    ui->pushButton_admin->setEnabled(false);

    ui->pushButton_admin_confirmdeleteitem->setEnabled(false);
    ui->pushButton_admin_confirmdeletemember->setEnabled(false);

    ui->gridWidget_admin_memberdatafields->hide();
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->gridWidget_admin_itemdatafields->hide();
    ui->gridWidget_admin_confirmdeleteitem->hide();

    ui->gridWidget_membership_expire->hide();

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

void MainWindow::on_pushButton_employeepermissions_clicked()
{
    index = 0;
    setPermissions(index);
}

void MainWindow::on_pushButton_managerpermissions_clicked()
{
    index = 1;
    setPermissions(index);
}

void MainWindow::on_pushButton_adminpermissions_clicked()
{
    index = 2;
    setPermissions(index);
}


/*----Window Navigation----*/
void MainWindow::on_pushButton_home_clicked() // home page
{
    ui->stackedWidget_main->setCurrentIndex(home);
}

void MainWindow::on_pushButton_POS_clicked() // POS page
{
    ui->stackedWidget_main->setCurrentIndex(pos);
}

void MainWindow::on_pushButton_sales_clicked() // sales page
{
    ui->stackedWidget_main->setCurrentIndex(sales);
}
    void MainWindow::on_pushButton_sales_daily_clicked() // daily sales report
    {
        ui->stackedWidget_sales->setCurrentIndex(salesdaily);
    }

    void MainWindow::on_pushButton_sales_sortmember_clicked() // sales by member
    {
        ui->stackedWidget_sales->setCurrentIndex(salessortmember);
    }

    void MainWindow::on_pushButton_sales_sortitem_clicked() // sales by item
    {
        ui->stackedWidget_sales->setCurrentIndex(salessortitem);
    }

    void MainWindow::on_pushButton_sales_searchmember_clicked() // search by member
    {
        ui->stackedWidget_sales->setCurrentIndex(salessearchmember);
    }

    void MainWindow::on_pushButton_sales_searchitem_clicked() // search by item
    {
        ui->stackedWidget_sales->setCurrentIndex(salessearchitem);
    }

void MainWindow::on_pushButton_members_clicked() // membership page
{
    ui->stackedWidget_main->setCurrentIndex(member);
}

void MainWindow::on_pushButton_admin_clicked() // administrator tools
{
    ui->stackedWidget_main->setCurrentIndex(admin);
}
    void MainWindow::on_pushButton_admin_member_clicked() // adding/deleting members
    {
        ui->stackedWidget_admin->setCurrentIndex(adminmember);
    }

    void MainWindow::on_pushButton_admin_inventory_clicked() // adding/deleting inventory
    {
        ui->stackedWidget_admin->setCurrentIndex(adminitem);
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
    ui->gridWidget_admin_confirmdeletemember->show();
    ui->pushButton_admin_editmember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(false);
}

void MainWindow::on_pushButton_admin_membersubmission_submit_clicked() // submit button for adding/editing
{
    ui->gridWidget_admin_memberdatafields->hide();
    ui->pushButton_admin_deletemember->setEnabled(true);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);
}

void MainWindow::on_pushButton_admin_membersubmission_cancel_clicked() // cancels submission for adding/editing
{
    ui->gridWidget_admin_memberdatafields->hide();
    ui->pushButton_admin_deletemember->setEnabled(true);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);
}
void MainWindow::on_pushButton_admin_confirmdeletemember_clicked() // confirms delete member
{
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->pushButton_admin_deletemember->setEnabled(true);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);
}

void MainWindow::on_pushButton_admin_canceldeletemember_clicked() // cancels delete member
{
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->pushButton_admin_deletemember->setEnabled(true);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);
}

void MainWindow::on_tableView_admin_members_doubleClicked(const QModelIndex &index) // double click admin member table
{
    ui->pushButton_admin_confirmdeletemember->setEnabled(true);
    // set text for label_admin_confirmdeletemember and change initial value to empty
}

    /*----Inventory Page----*/
void MainWindow::on_pushButton_admin_additem_clicked() //add item button
{
    ui->gridWidget_admin_itemdatafields->show();
    ui->pushButton_admin_deleteitem->setEnabled(false);
    ui->pushButton_admin_edititem->setEnabled(false);
}

void MainWindow::on_pushButton_admin_edititem_clicked() // edit item button
{
    ui->gridWidget_admin_itemdatafields->show();
    ui->pushButton_admin_deleteitem->setEnabled(false);
    ui->pushButton_admin_additem->setEnabled(false);
}

void MainWindow::on_pushButton_admin_deleteitem_clicked() // delete item button
{
    ui->gridWidget_admin_confirmdeleteitem->show();
    ui->pushButton_admin_edititem->setEnabled(false);
    ui->pushButton_admin_additem->setEnabled(false);
}

void MainWindow::on_pushButton_admin_itemsubmission_submit_clicked() //confirms add/edit
{
    ui->gridWidget_admin_itemdatafields->hide();
    ui->pushButton_admin_deleteitem->setEnabled(true);
    ui->pushButton_admin_additem->setEnabled(true);
    ui->pushButton_admin_edititem->setEnabled(true);
}

void MainWindow::on_pushButton_admin_itemsubmission_cancel_clicked() // cancels add/edit
{
    ui->gridWidget_admin_itemdatafields->hide();
    ui->pushButton_admin_deleteitem->setEnabled(true);
    ui->pushButton_admin_additem->setEnabled(true);
    ui->pushButton_admin_edititem->setEnabled(true);
}

void MainWindow::on_pushButton_admin_confirmdeleteitem_clicked() // confirms delete
{
    ui->gridWidget_admin_confirmdeleteitem->hide();
    ui->pushButton_admin_deleteitem->setEnabled(true);
    ui->pushButton_admin_additem->setEnabled(true);
    ui->pushButton_admin_edititem->setEnabled(true);
}

void MainWindow::on_pushButton_admin_canceldeleteitem_clicked() // cancels delete
{
    ui->gridWidget_admin_confirmdeleteitem->hide();
    ui->pushButton_admin_deleteitem->setEnabled(true);
    ui->pushButton_admin_additem->setEnabled(true);
    ui->pushButton_admin_edititem->setEnabled(true);
}

void MainWindow::on_tableView_admin_inventory_doubleClicked(const QModelIndex &index) // double click admin inventory table
{
    ui->pushButton_admin_confirmdeleteitem->setEnabled(true);
    // set text for label_admin_confirmdeleteitem and change initial value to empty
}

/*----Membership Window push buttons----*/
void MainWindow::on_pushButton_membership_rebates_clicked() // member rebates list
{
    ui->gridWidget_membership_expire->hide();
}

void MainWindow::on_pushButton_membership_expiration_clicked() // member expiration list
{
    ui->gridWidget_membership_expire->show();
}

void MainWindow::on_pushButton_membership_upgrades_clicked() // member upgrades list
{
    ui->gridWidget_membership_expire->hide();
}

void MainWindow::on_pushButton_membership_downgrades_clicked() // member downgrades list
{
    ui->gridWidget_membership_expire->hide();
}

/*----POS Page push buttons----*/
void MainWindow::on_pushButton_pos_purchase_clicked() // purchase button
{

}


/*----Sales Page push buttons----*/
void MainWindow::on_pushButton_sales_searchmemberconfirm_clicked() // search member button
{

}

void MainWindow::on_pushButton_sales_searchitemconfirm_clicked() // search item button
{

}


