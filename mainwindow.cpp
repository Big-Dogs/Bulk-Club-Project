#include "mainwindow.h"
#include "ui_mainwindow.h"





// Window Initialization
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Variables
    QString databasePath = Database::findDatabase("db.db");

    ui->setupUi(this);

    // Instantiate database
    if (databasePath != "NOT FOUND")
    {
        this->database = new Database(databasePath, "QSQLITE");
    }
    else
    {
        throw databasePath;
    }

    qDebug() << "Current Path: " << QDir::currentPath();

    formatPrice = new MoneyDelegate;


    InitializePosTable();


    // Create Executive Member Vector



    ui->stackedWidget_main->setCurrentIndex(HOME); // setting default indices
    ui->stackedWidget_sales->setCurrentIndex(SALES_DAILY);
    ui->stackedWidget_admin->setCurrentIndex(ADMIN_MEMBER);



    ui->tableWidget_membership->hide();

    ui->pushButton_sales->setEnabled(false); // hiding and greying stuff
    ui->pushButton_members->setEnabled(false);
    ui->pushButton_admin->setEnabled(false);

    ui->comboBox_pos_qty->setEnabled(false);
    ui->pushButton_pos_purchase->setEnabled(false);

    ui->label_home_warning->hide();

    index = 0;
    setPermissions(index);

    ui->pushButton_admin_confirmdeleteitem->setEnabled(false);
    ui->pushButton_admin_confirmdeletemember->setEnabled(false);

    ui->gridWidget_admin_memberdatafields->hide();
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->gridWidget_admin_itemdatafields->hide();
    ui->gridWidget_admin_confirmdeleteitem->hide();

    ui->gridWidget_membership_expire->hide();

    ui->label_total_revune->setVisible(false);
    ui->label_sales_searchmembererrormessage->setVisible(false);
    ui->comboBox_sales_manymembersfound->setVisible(false);

    qDebug() << "feature: " << database->driver()->hasFeature(QSqlDriver::PositionalPlaceholders);

}


MainWindow::~MainWindow()
{
    delete ui;

    delete database;

    delete formatPrice;
}

/*----Testing Permissions----*/
void MainWindow::setPermissions(int permission)
{
    if(permission == 0)
    {
        ui->pushButton_POS->setEnabled(false);
        ui->pushButton_sales->setEnabled(false);
        ui->pushButton_members->setEnabled(false);
        ui->pushButton_admin->setEnabled(false);
    }
    else if (permission == 1)
    {
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(false);
        ui->pushButton_members->setEnabled(false);
        ui->pushButton_admin->setEnabled(false);
    }
    else if (permission == 2)
    {
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(true);
        ui->pushButton_members->setEnabled(true);
        ui->pushButton_admin->setEnabled(false);
    }
    else if (permission == 3)
    {
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(true);
        ui->pushButton_members->setEnabled(true);
        ui->pushButton_admin->setEnabled(true);
    }
}

void MainWindow::on_pushButton_employeepermissions_clicked()
{
    index = 1;
    setPermissions(index);
}

void MainWindow::on_pushButton_managerpermissions_clicked()
{
    index = 2;
    setPermissions(index);
}

void MainWindow::on_pushButton_adminpermissions_clicked()
{
    index = 3;
    setPermissions(index);
}


/*----Window Navigation----*/
void MainWindow::on_pushButton_home_clicked() // home page
{
    ui->stackedWidget_main->setCurrentIndex(HOME);
    index = 0;
    setPermissions(index);
}

void MainWindow::on_pushButton_POS_clicked() // POS page
{
    ui->stackedWidget_main->setCurrentIndex(POS);



    QStringList items = this->database->getNames();

    qDebug() << items.length() << " items";
    //item number combo box
    if(ui->comboBox_pos_itemlist->count() == 0)
    {
        for (int i = 0; i < items.length(); i++)
        {
            ui->comboBox_pos_itemlist->addItem(items.at(i));
        }
    }

    if(ui->comboBox_pos_qty->count() == 0)
    {
        for (int i = 1; i <= 10; i++)
        {
            ui->comboBox_pos_qty->addItem(QString::number(i));
        }
    }
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
        //Constant
            const int ID_COLUMN     = 0; //The column number for the member's id number
            const int NAME_COLUMN   = 1; //The column number for the member's name
            const int REVUNE_COLUMN = 2; //The column number for the member's revune
        //Variables
        QSqlQuery       query;         //The query object use to exucute the query for tableData (easier to check for errors)
        QSqlQueryModel *tableData;     //A point to the query model storing the data for the table

        double totalRevune = 0.00; //The total revune across all members

        ui->stackedWidget_sales->setCurrentIndex(SALES_SORT_MEMBER);

       bool queryError = query.exec("SELECT members.memberID, members.name, sum(products.price * purchases.qty) AS revune FROM members "
                                    "LEFT OUTER JOIN purchases ON purchases.memberID=members.memberID "
                                    "LEFT OUTER JOIN products ON purchases.productID=products.productID "
                                    "GROUP BY members.memberID "
                                    "ORDER BY members.memberID");

       if (!queryError)
       {
           qDebug() << query.lastError().text();

           throw query.lastError();
       }
       else
       {
           qDebug() << "successful";
       }

       tableData = new QSqlQueryModel();

       tableData->setQuery(query);

       tableData->setHeaderData(ID_COLUMN, Qt::Horizontal, tr("ID"));
       tableData->setHeaderData(NAME_COLUMN, Qt::Horizontal, tr("Name"));
       tableData->setHeaderData(REVUNE_COLUMN, Qt::Horizontal, tr("Revune"));

       ui->tableView_sales_sortmember->verticalHeader()->setVisible(false);

       ui->tableView_sales_sortmember->setModel(tableData);

       ui->tableView_sales_sortmember->setItemDelegateForColumn(REVUNE_COLUMN, formatPrice);

       ui->tableView_sales_sortmember->resizeColumnToContents(NAME_COLUMN);

       qDebug() << tableData->rowCount();
       for (int index = 0; index < tableData->rowCount(); index++)
       {

          totalRevune += tableData->record(index).value("Revune").toDouble();
       }

       ui->label_total_revune->setText(QString("Total Revune: $").append(QString::number(totalRevune, 'f', 2)));
       ui->label_total_revune->setVisible(true);
    }

    void MainWindow::on_pushButton_sales_sortitem_clicked() // sales by item
    {
        ui->stackedWidget_sales->setCurrentIndex(SALES_SORT_ITEM);
    }

    void MainWindow::on_pushButton_sales_searchmember_clicked() // search by member
    {
        //Constants
        const int ID   = 0; //The consant corresponding the id column from the query result
        const int NAME = 1; //The constant corresponding to the name column from the query result
        //Variables
        QSqlQuery   retrieveMemberData; //The query object use to retrieve the member's name and id
        QStringList memberIdName;       //A QStringList container the id and name of all the members
                                        //basically a member in the list has a QString for their id
                                        //then following QString is their name

        ui->stackedWidget_sales->setCurrentIndex(SALES_SEARCH_MEMBER);

        bool retrieveDataError = retrieveMemberData.exec("SELECT memberId, name FROM members");

        if (!retrieveDataError)
        {
            qDebug() << retrieveMemberData.lastError().text();

            throw retrieveMemberData.lastError();
        }

        while (retrieveMemberData.next())
        {
            memberIdName << retrieveMemberData.value(ID).toString();
            memberIdName << retrieveMemberData.value(NAME).toString();
        }

        TextCompleter(memberIdName, ui->lineEdit_sales_searchmember);
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


        QSqlQueryModel *model = new QSqlQueryModel();
        QSqlQuery * query = new QSqlQuery;
        query->prepare("select * from members");

        if(!query->exec())
        {
            qDebug() << query->lastError().text();
        }
        else
        {
            model->setQuery(*query);
            ui->tableView_admin_members->setModel(model);
        }
        delete query;
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

     MainWindow::on_pushButton_admin_member_clicked();
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

    QString memberType = "Regular";

    TempMember tempMemberAdd;


    tempMemberAdd.id = ui->lineEdit_admin_membersubmission_id->text();
    tempMemberAdd.name = ui->lineEdit_admin_membersubmission_name->text();
    //tempMemberAdd.executiveStatus = ui->lineEdit_admin_membersubmission_executive->text();
    //tempMemberAdd.expirationDate = ui->lineEdit_admin_membersubmission_date->text();

    if(tempMemberAdd.executiveStatus == "executive")
        memberType = "Executive";

    QSqlQuery query;

    int renewalPrice;

    if(memberType == "Executive")
        renewalPrice = 120;
    else
        renewalPrice = 60;

    query.prepare("INSERT INTO members "
                  "(memberID, name, "
                  "memberType, expireDate,"
                  "renewalPrice) VALUES(?,?,?,?,?)");

    query.addBindValue(tempMemberAdd.id);
    query.addBindValue(tempMemberAdd.name);
    query.addBindValue(memberType);
    query.addBindValue(tempMemberAdd.expirationDate);
    query.addBindValue(QString::number(renewalPrice));

    if(!query.exec())
        qDebug() << "Member failed to save";

     MainWindow::on_pushButton_admin_member_clicked();
     MainWindow::ClearMemberFields();
}

void MainWindow::on_pushButton_admin_membersubmission_cancel_clicked() // cancels submission for adding/editing
{
    ui->gridWidget_admin_memberdatafields->hide();
    ui->pushButton_admin_deletemember->setEnabled(true);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);

    MainWindow::ClearMemberFields();
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
    ui->tableWidget_membership->hide();


    //clears the information from other vertical stackwidget tabs
    ui->tableWidget_membership->setRowCount(0);
    upgradeCount = 0;
    downgradeCount = 0;


    ExecutiveMemberRebate tempMember;
    int memberIndex = 0;
    QStringList memberIDList;

    QSqlQuery query;
    query.prepare("SELECT memberID FROM MEMBERS WHERE memberType='Executive'");

    // Execute Query
    if(query.exec())
    {
        // iterate through and pull ids
        while(query.next())
        {

            memberIDList.insert(memberIndex, query.value(0).toString());
            memberIndex++;
        }

        // DEBUG: print list
        for(int i = 0; i < memberIDList.size(); i++)
        {
            qDebug() << memberIDList[i];
        }
    }
    else
    {
        qDebug() << query.lastError().text();
    }

    query.prepare("SELECT members.memberID, members.name, "
                  "sum(purchases.qty * products.price) "
                  "FROM members, purchases, products "
                  "WHERE members.memberID = purchases.memberID "
                  "AND purchases.productID = products.productID "
                  "AND members.memberID = :memberID");

    QVector<ExecutiveMemberRebate> memberList;

    // If member list is empty
    if(memberList.empty())
    {
        // Iterate through ID list, calling each member's purchases
        for(int i = 0; i < memberIDList.size(); i++)
        {
            query.bindValue(":memberID", memberIDList[i]);

            // Execute Query
            if(query.exec())
            {
                // Iterate through query data and pull purchase information into vector
                while(query.next())
                {
                    if(query.value(0).toString() != "")
                    {
                        // Copy into temp object
                        tempMember.memberID = query.value(0).toString();
                        tempMember.name = query.value(1).toString();
                        tempMember.amountSpent = query.value(2).toString();
                        tempMember.rebate = QString::number((tempMember.amountSpent.toFloat()) * .02);
                        // Add object to member list
                        memberList.append(tempMember);
                    }
                }
            }
            else // if unsuccessful, print error
            {
                qDebug() << query.lastError().text();
            }
        }
    }
    //printing the list of executive members
    for(int i = 0; i < memberList.size(); i++)
    {
        qDebug() << memberList[i].name << " " << memberList[i].memberID << " "
                 << memberList[i].amountSpent << " " << memberList[i].rebate << "\n";
    }


    QStringList tableColumns;
    ui->tableWidget_membership->setColumnCount(3);
    tableColumns << "ID Number" << "Name" << "Rebate";
    ui->tableWidget_membership->setHorizontalHeaderLabels(tableColumns);


        //populates the tableWidget_membership
        for(int execMember = 0; execMember < memberList.count(); execMember++)
        {
            ui->tableWidget_membership->insertRow(execMember);
            ui->tableWidget_membership->setItem(execMember, 0,
                                                new QTableWidgetItem(memberList[execMember].memberID));
            ui->tableWidget_membership->setItem(execMember, 1,
                                                new QTableWidgetItem(memberList[execMember].name));
            ui->tableWidget_membership->setItem(execMember, 2,
                                                new QTableWidgetItem("$" + QString::number(memberList[execMember].rebate.toFloat(), 'f', 2)));
        }
        //shows the tableWidget_membership
        ui->tableWidget_membership->show();

        //outputs the nuber of rows in tableWidget_membership
        qDebug() << ui->tableWidget_membership->rowCount();

        //gets the total of all of the executive member's rebates.
        float totalAllRebates;
        for(int i = 0; i < memberList.size(); i++)
        {
            totalAllRebates += memberList[i].rebate.toFloat();
        }
        QString labelText = "Total of all rebates: $" + QString::number(totalAllRebates);
        ui->label_membership_recommendation_status->setText(labelText);


}

void MainWindow::on_pushButton_membership_expiration_clicked() // member expiration list
{
    ui->tableWidget_membership->hide();
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
    ui->tableWidget_membership->show();
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
    // Reset all values
    InitializeMembershipTableWidget();

    // Get Regular Member ID List
    QStringList regularMemberIDList = database->GetRegularMemberIDList();

    // Get Regular Member Purchases
    QVector<Database::Member> regularMemberPurchases =
            database->GetRegularMemberPurchases(regularMemberIDList);

    // Print Suggested Upgrade Report
    PrintUpgradeReport(regularMemberPurchases);
}

void MainWindow::on_pushButton_membership_downgrades_clicked() // member downgrades list
{
    // Reset all values
    InitializeMembershipTableWidget();

    // Get Executive Member ID List
    QStringList executiveMemberIDList = database->GetExecutiveMemberIDList();

    // Get Regular Member Purchases
    QVector<Database::Member> executiveMemberPurchases =
            database->GetExecutiveMemberPurchases(executiveMemberIDList);

    // Print Suggested Downgrade Report
    PrintDowngradeReport(executiveMemberPurchases);
}

/*----POS Page push buttons----*/
void MainWindow::on_pushButton_pos_purchase_clicked() // purchase button
{

    ui->comboBox_pos_itemlist->setCurrentIndex(0);
    ui->comboBox_pos_qty->setCurrentIndex(0);
    ui->comboBox_pos_qty->setEnabled(false);
    ui->label_pos_price->setText("");

    QTableWidgetItem *item = new QTableWidgetItem;
    QTableWidgetItem *price = new QTableWidgetItem;
    QTableWidgetItem *qty = new QTableWidgetItem;
    QTableWidgetItem *total = new QTableWidgetItem;

    item->setText(posItemName);
    price->setText(QString::number(posPrice));
    qty->setText(QString::number(posQty));
    total->setText(QString::number(posTotal));

    ui->tableWidget_pos_receipts->insertRow(receiptRow);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 0, item);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 1, price);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 2, qty);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 3, total);
    receiptRow++;

    ui->pushButton_pos_purchase->setEnabled(false);

}

void MainWindow::on_comboBox_pos_itemlist_activated(int index)
{
    posItem = index+1;
    posItemName = ui->comboBox_pos_itemlist->currentText();
    ui->comboBox_pos_qty->setEnabled(true);

}
void MainWindow::on_comboBox_pos_qty_activated(int index)
{
    posQty = index + 1;
    posPrice = this->database->getPrice(posItem);
    posTotal = posPrice * posQty;
    ui->label_pos_price->setText(QString::number(posTotal));
    ui->pushButton_pos_purchase->setEnabled(true);
}


/*----Sales Page push buttons----*/
void MainWindow::on_pushButton_sales_searchmemberconfirm_clicked() // search member button
{
    //Constant
    const int ID_COLUMN     = 0; //The column number for the member's id number
    const int NAME_COLUMN   = 1; //The column number for the member's name
    const int REVENUNE_COLUMN = 2; //The column number for the member's revune

    //Variables
    QString        memberFound;   //The QString store member that is found, it
                                  //eiter stores the member's name or id
    QSqlQuery      retrieveData;  //The query object that retrieves the data from
                                  //the database
    QSqlQueryModel *tableData;    //The model that store the data to be displayed in
                                  //the table view
    QString         comboBoxText; //A QString to do intermediates steps on the text
                                  //for the many members found comboBox

    //seting error widgets to be invisible
    ui->label_sales_searchmembererrormessage->setVisible(false);
    ui->comboBox_sales_manymembersfound->setVisible(false);

    memberFound = ui->lineEdit_sales_searchmember->text();

    //I know positional placeholders are terrible but I just feel better using something that is actually part of the SQL driver, sorry
    retrieveData.prepare("SELECT members.memberID, members.name, sum(products.price * purchases.qty) AS revune FROM members "
                         "LEFT OUTER JOIN purchases ON purchases.memberID=members.memberID "
                         "LEFT OUTER JOIN products ON purchases.productID=products.productID "
                         "WHERE members.memberID=? OR members.name=?");

    retrieveData.bindValue(0, memberFound);
    retrieveData.bindValue(1, memberFound);

    bool queryError = retrieveData.exec();

    if (!queryError)
    {
        qDebug() << retrieveData.lastError().text();

        throw retrieveData.lastError();
    }

    tableData = new QSqlQueryModel;

    tableData->setQuery(retrieveData);

    tableData->setHeaderData(ID_COLUMN, Qt::Horizontal, QVariant("Member ID"));
    tableData->setHeaderData(NAME_COLUMN, Qt::Horizontal, QVariant("Member Name"));
    tableData->setHeaderData(REVENUNE_COLUMN, Qt::Horizontal, QVariant("Revenune"));

    qDebug() << "check";

    if (tableData->rowCount() == 1)
    {
        ui->tableView_sales_searchmember->setModel(tableData);
        ui->tableView_sales_searchmember->verticalHeader()->setVisible(false);
//        ui->tableView_sales_searchmember->resizeColumnsToContents(); Messes up with the delegate, will work on in a later sprint

        if (tableData->record(0).value("memberId").isNull()) //no members found
        {
            ui->label_sales_searchmembererrormessage->setText("Error: That member was not found");
            ui->label_sales_searchmembererrormessage->setVisible(true);

            //clearing table view
            tableData->clear();
        }
        else //exactly one member found
        {
            ui->tableView_sales_searchmember->setItemDelegateForColumn(2, formatPrice);
        }
    }
    else //more than one member found
    {
        ui->label_sales_searchmembererrormessage->setText("Error: Many members found, please choose the one you were looking for");
        ui->label_sales_searchmembererrormessage->setVisible(true);

        for (int index = 0; index < tableData->rowCount(); index++)
        {
            comboBoxText = tableData->record(index).value("memberId").toString().append(" ");

            comboBoxText.append(tableData->record(index).value("name").toString());

            ui->comboBox_sales_manymembersfound->addItem(comboBoxText);
        }

        ui->comboBox_sales_manymembersfound->setVisible(true);
    }
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
void MainWindow::ClearMemberFields()
{
    ui->lineEdit_admin_membersubmission_id->clear();
    ui->lineEdit_admin_membersubmission_name->clear();
    //ui->lineEdit_admin_membersubmission_executive->clear();
    //ui->lineEdit_admin_membersubmission_date->clear();
}


/*----Home Page push buttons----*/
void MainWindow::on_pushButton_home_login_clicked()
{
    QString username = ui->lineEdit_home_username->text();
    QString password = ui->lineEdit_home_password->text();

    ui->label_home_warning->hide();

    if (username == "employee" &&
        password == "test")
    {
        index = 1;
        setPermissions(index);
        ui->stackedWidget_main->setCurrentIndex(POS);
    }
    else if (username == "manager" &&
        password == "test")
    {
        index = 2;
        setPermissions(index);
        ui->stackedWidget_main->setCurrentIndex(SALES);
    }
    else if (username == "admin" &&
        password == "test")
    {
        index = 3;
        setPermissions(index);
        ui->stackedWidget_main->setCurrentIndex(ADMIN);
    }
    else
    {
        ui->label_home_warning->show();
    }
}

// Helper Function Definitions
    // Reset all values in membership table widget
void MainWindow::InitializeMembershipTableWidget()
{
    ui->tableWidget_membership->show();
    ui->gridWidget_membership_expire->hide();
    ui->tableWidget_membership->setRowCount(0);
    upgradeCount = 0;
    downgradeCount = 0;
    rebateAmount = 0.0; // member's rebate received
    ui->tableWidget_membership->setColumnCount(TABLE_WIDGET_COLS);
    ui->tableWidget_membership->setHorizontalHeaderLabels(tableWidgetColumnNames);
}

    // Print suggested upgrades report
void MainWindow::PrintUpgradeReport(QVector<Database::Member> regularMemberPurchases)
{
    // loop through purchases to collect all people. add to tableWidget if <65
    for(int index = 0; index < regularMemberPurchases.count(); index++)
    {
        // Calculate estimated rebate
        rebateAmount = regularMemberPurchases[index].amountSpent.toFloat() * REBATE_RATE;

        // If rebateAmount under rebateMin, add to recommendations
        if(rebateAmount > REBATE_MIN)
        {
            ui->tableWidget_membership->insertRow(upgradeCount);
            ui->tableWidget_membership->setItem(upgradeCount, MEMBERSHIP_NUMBER,
                                                new QTableWidgetItem(regularMemberPurchases[index].memberID));
            ui->tableWidget_membership->setItem(upgradeCount, MEMBER_NAME,
                                                new QTableWidgetItem(regularMemberPurchases[index].name));
            ui->tableWidget_membership->setItem(upgradeCount, AMT_SPENT,
                                                new QTableWidgetItem(QString::number(regularMemberPurchases[index].amountSpent.toFloat(), 'f', 2)));
            ui->tableWidget_membership->setItem(upgradeCount, REBATE_AMOUNT,
                                                new QTableWidgetItem(QString::number(rebateAmount, 'f', 2)));
            upgradeCount++;
        }
    }

    // Set label to display number of recommendations
    QString labelText = "Number of recommended membership upgrades: " + QString::number(upgradeCount);
    ui->label_membership_recommendation_status->setText(labelText);
}

    // Print suggested downgrades report
void MainWindow::PrintDowngradeReport(QVector<Database::Member> executiveMemberPurchases)
{
    // loop through purchases to collect all people. add to tableWidget if <65
    for(int index = 0; index < executiveMemberPurchases.count(); index++)
    {
        // Calculate estimated rebate
        rebateAmount = executiveMemberPurchases[index].amountSpent.toFloat() * REBATE_RATE;

        // If rebateAmount under rebateMin, add to recommendations
        if(rebateAmount < REBATE_MIN)
        {
            ui->tableWidget_membership->insertRow(downgradeCount);
            ui->tableWidget_membership->setItem(downgradeCount, MEMBERSHIP_NUMBER,
                                                new QTableWidgetItem(executiveMemberPurchases[index].memberID));
            ui->tableWidget_membership->setItem(downgradeCount, MEMBER_NAME,
                                                new QTableWidgetItem(executiveMemberPurchases[index].name));
            ui->tableWidget_membership->setItem(downgradeCount, AMT_SPENT,
                                                new QTableWidgetItem(QString::number(executiveMemberPurchases[index].amountSpent.toFloat(), 'f', 2)));
            ui->tableWidget_membership->setItem(downgradeCount, REBATE_AMOUNT,
                                                new QTableWidgetItem(QString::number(rebateAmount, 'f', 2)));
            downgradeCount++;
        }
    }

    // Set label to display number of recommendations
    QString labelText = "Number of recommended membership downgrades: " + QString::number(downgradeCount);
    ui->label_membership_recommendation_status->setText(labelText);

}

//initialize pos table
void MainWindow::InitializePosTable()
{
    QStringList TableHeader;
    ui->tableWidget_pos_receipts->setColumnCount(4);
    TableHeader << "Item" << "Price" << "qty" << "Total";
    ui->tableWidget_pos_receipts->setHorizontalHeaderLabels(TableHeader);
    ui->tableWidget_pos_receipts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_pos_receipts->setShowGrid(false);
    ui->tableWidget_pos_receipts->verticalHeader()->setVisible(false);
    ui->tableWidget_pos_receipts->setColumnWidth(0, 300);
    receiptRow = 0;
}

