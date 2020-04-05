#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Instantiate database
    this->database = new Database("db.db", "QSQLITE");

    ui->stackedWidget_main->setCurrentIndex(HOME); // setting default indices
    ui->stackedWidget_sales->setCurrentIndex(SALES_DAILY);
    ui->stackedWidget_admin->setCurrentIndex(ADMIN_MEMBER);


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
    ui->stackedWidget_main->setCurrentIndex(HOME);
}

void MainWindow::on_pushButton_POS_clicked() // POS page
{
    ui->stackedWidget_main->setCurrentIndex(POS);
}

void MainWindow::on_pushButton_sales_clicked() // sales page
{
    ui->stackedWidget_main->setCurrentIndex(SALES);
}
    void MainWindow::on_pushButton_sales_daily_clicked() // daily sales report
    {
        ui->stackedWidget_sales->setCurrentIndex(SALES_DAILY);
    }

    void MainWindow::on_pushButton_sales_sortmember_clicked() // sales by member
    {
        ui->stackedWidget_sales->setCurrentIndex(SALES_SORT_MEMBER);
    }

    void MainWindow::on_pushButton_sales_sortitem_clicked() // sales by item
    {
        ui->stackedWidget_sales->setCurrentIndex(SALES_SORT_ITEM);
    }

    void MainWindow::on_pushButton_sales_searchmember_clicked() // search by member
    {
        ui->stackedWidget_sales->setCurrentIndex(SALES_SEARCH_MEMBER);
    }

    void MainWindow::on_pushButton_sales_searchitem_clicked() // search by item
    {
        // Move to proper page
        ui->stackedWidget_sales->setCurrentIndex(SALES_SEARCH_ITEM);

        // Make sure list is empty
        if(productList.empty())
        {
            // Create list of all products in database
            QSqlQuery query;
            query.prepare("SELECT name FROM products");

            // If query executes successfully
            if(query.exec())
            {
                while(query.next())
                {
                    productList << query.value(0).toString();
                }

                // Output list to console
                for(int index = 0; index < productList.count(); index++)
                {
                    qDebug() << productList[index];
                }
            }
            else // Else print error
            {
                qDebug() << query.lastError().text();
            }
        }
        // Initialize textcompleter for searching
        QLineEdit* inputField = ui->lineEdit_sales_searchitem;
        TextCompleter(productList, inputField);
    }

void MainWindow::on_pushButton_members_clicked() // membership page
{
    ui->stackedWidget_main->setCurrentIndex(MEMBER);
}

void MainWindow::on_pushButton_admin_clicked() // administrator tools
{
    ui->stackedWidget_main->setCurrentIndex(ADMIN);
}
    void MainWindow::on_pushButton_admin_member_clicked() // adding/deleting members
    {
        ui->stackedWidget_admin->setCurrentIndex(ADMIN_MEMBER);
    }

    void MainWindow::on_pushButton_admin_inventory_clicked() // adding/deleting inventory
    {
        ui->stackedWidget_admin->setCurrentIndex(ADMIN_ITEM);
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
    // Populate dropdown menu if empty
    if(ui->comboBox_membership_expire->count() == 0)
    {
        ui->comboBox_membership_expire->addItem("January");
        ui->comboBox_membership_expire->addItem("February");
        ui->comboBox_membership_expire->addItem("March");
        ui->comboBox_membership_expire->addItem("April");
        ui->comboBox_membership_expire->addItem("May");
        ui->comboBox_membership_expire->addItem("June");
        ui->comboBox_membership_expire->addItem("July");
        ui->comboBox_membership_expire->addItem("August");
        ui->comboBox_membership_expire->addItem("September");
        ui->comboBox_membership_expire->addItem("October");
        ui->comboBox_membership_expire->addItem("November");
        ui->comboBox_membership_expire->addItem("December");
    }

    // Initialize tableView_membership using MembershipTableModel
    membershipModel = new MembershipTableModel(this, database);
    membershipModel->InitializeTable();
    ui->tableView_membership->setModel(membershipModel);

    // Hide numerical vertical header
    ui->tableView_membership->verticalHeader()->setVisible(false);
    // Make fields uneditable
    ui->tableView_membership->setEditTriggers(QTableView::NoEditTriggers);
    // Show it
    ui->gridWidget_membership_expire->show();
}

void MainWindow::on_pushButton_membership_expire_clicked()
{
    // Filter expiration by month
    switch(ui->comboBox_membership_expire->currentIndex())
    {
        case MembershipTableModel::JANUARY   : membershipModel->setFilter("substr(expireDate, 0, 3) = '01'");
                                               break;
        case MembershipTableModel::FEBRUARY  : membershipModel->setFilter("substr(expireDate, 0, 3) = '02'");
                                               break;
        case MembershipTableModel::MARCH     : membershipModel->setFilter("substr(expireDate, 0, 3) = '03'");
                                               break;
        case MembershipTableModel::APRIL     : membershipModel->setFilter("substr(expireDate, 0, 3) = '04'");
                                               break;
        case MembershipTableModel::MAY       : membershipModel->setFilter("substr(expireDate, 0, 3) = '05'");
                                               break;
        case MembershipTableModel::JUNE      : membershipModel->setFilter("substr(expireDate, 0, 3) = '06'");
                                               break;
        case MembershipTableModel::JULY      : membershipModel->setFilter("substr(expireDate, 0, 3) = '07'");
                                               break;
        case MembershipTableModel::AUGUST    : membershipModel->setFilter("substr(expireDate, 0, 3) = '08'");
                                               break;
        case MembershipTableModel::SEPTEMBER : membershipModel->setFilter("substr(expireDate, 0, 3) = '09'");
                                               break;
        case MembershipTableModel::OCTOBER   : membershipModel->setFilter("substr(expireDate, 0, 3) = '10'");
                                               break;
        case MembershipTableModel::NOVEMBER  : membershipModel->setFilter("substr(expireDate, 0, 3) = '11'");
                                               break;
        case MembershipTableModel::DECEMBER  : membershipModel->setFilter("substr(expireDate, 0, 3) = '12'");
                                               break;
    }

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
    // Pull desired item to be placed into query
    salesReportProduct = ui->lineEdit_sales_searchitem->text();

    // Output item to terminal for testing purposes
    qDebug() << salesReportProduct;

    // Declare and intialize query and query model to pull relevant information
    QSqlQueryModel *model = new QSqlQueryModel;
    QSqlQuery query;

    // Prep extremely complex query
    query.prepare("select products.name, sum(purchases.qty), sum(purchases.qty) * products.price "
                  "from products, purchases "
                  "where products.name=:name "
                  "and products.productID = purchases.productID;");

    // Bind variable to query
    query.bindValue(":name", salesReportProduct);

    if(query.exec())
    {
        // Insert query into model
        model->setQuery(query);

        // Configure headers to reflect relevant descriptors
        model->setHeaderData(0, Qt::Horizontal, tr("Product Name"));
        model->setHeaderData(1, Qt::Horizontal, tr("Quantity Sold"));
        model->setHeaderData(2, Qt::Horizontal, tr("Total Revenue"));
    }
    else // if unsuccessful, print error
    {
        qDebug() << query.lastError().text();
    }

    // Hide row numbers
    ui->tableView_sales_searchitem->verticalHeader()->setVisible(false);
    // Configure tableView with model
    ui->tableView_sales_searchitem->setModel(model);
}

void MainWindow::on_pushButton_sale_byday_clicked()
{

}

/**
 * @brief MainWindow::TextCompleter
 * Continuously searches list of products as the user types what they want
 * to find.
 * @param names the list of school names currently in database
 * @param field QLineEdit form that function is being used on
 */
void MainWindow::TextCompleter(QStringList products, QLineEdit *inputField)
{
    QCompleter *completer = new QCompleter(products, inputField);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    inputField->setCompleter(completer);
}
