#include "mainwindow.h"
#include "ui_mainwindow.h"


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

    // Create Executive Member Vector

    ui->stackedWidget_main->setCurrentIndex(HOME); // setting default indices
    ui->stackedWidget_sales->setCurrentIndex(SALES_DAILY);
    ui->stackedWidget_admin->setCurrentIndex(ADMIN_MEMBER);

    ui->tableWidget_membership->hide();

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

    ui->label_total_revune->setVisible(false);

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
        qDebug() << "MEMEBER_PURCHASE_AMOUT_TEST: " << MEMEBER_PURCHASE_AMOUT_TEST;
        #if MEMEBER_PURCHASE_AMOUT_TEST
            const int VERIFY_SIZE = 15; //The number of members being verified

            /* Unit testing parallel arrays
             *
             * These three parallel are for my unit test only,
             */
            int verifyId[VERIFY_SIZE] = {12121, 12345, 12897, 12899, 16161, 31311,
                                         35647, 44444, 56723, 61616, 67890, 67899,
                                         77777, 88888, 96309};

            QString verifyName[VERIFY_SIZE] = {"Harry Havealotsofkids", "Sally Shopper", "Fred Frugal",
                                               "Johnny Shopper", "Benjamin BusinessShopper", "Sam Single",
                                               "Linda Livesalone", "Larry Largefamily", "Sue Shoparound",
                                               "Sally SuperShopper", "Betty Buysalot", "BulkClub Bob",
                                               "Sam Spendstomuch", "Carrie CaresAboutMoney", "Mary IsHappy"};

            double verifyPurchase[VERIFY_SIZE] = {226.75, 9452.839999999997, 0, 5201.32, 0, 79.8, 63.17, 0,
                                                  14.950000000000001, 5813.4, 129.8, 269.59, 4700.969999999999,
                                                  1823.1200000000001, 0};

            bool passUnitTest = true;  //This is a bool value that keeps track of whether or not this feature passes
                                       //its unit test
        #endif

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

       #if MEMEBER_PURCHASE_AMOUT_TEST
           const int RESULT_WIDTH = 50; //The width of printing out the results

           QString result = ""; //The string to be qDebug out
           QString member;      //A QString storing one member

           result.append(QString("Expected").leftJustified(RESULT_WIDTH, QChar(' ')));
           result.append(QString("Actual").leftJustified(RESULT_WIDTH, QChar(' ')));

           qDebug() << result << endl;
       #endif

       qDebug() << tableData->rowCount();
       for (int index = 0; index < tableData->rowCount(); index++)
       {

          totalRevune += tableData->record(index).value("Revune").toDouble();

           #if MEMEBER_PURCHASE_AMOUT_TEST
               member = tableData->record(index).value("memberID").toString();
               member.append(" " + tableData->record(index).value("name").toString());
               member.append(" " + tableData->record(index).value("revune").toString());

               result = member.leftJustified(RESULT_WIDTH, QChar(' '));

               member = QString::number(verifyId[index]);
               member.append(" " + verifyName[index]);
               member.append(" " + QString::number(verifyPurchase[index], 'd'));

               result.append(member.leftJustified(RESULT_WIDTH, QChar(' ')));

               qDebug() << result << endl;

               if (tableData->record(index).value("memberID").toInt()    != verifyId[index]   &&
                   tableData->record(index).value("name").toString()     != verifyName[index] &&
                   tableData->record(index).value("revune").toDouble()   != verifyPurchase[index])
               {
                   passUnitTest = false;
               }
           #endif
       }

       ui->label_total_revune->setText(QString("Total Revune: $").append(QString::number(totalRevune, 'f', 2)));
       ui->label_total_revune->setVisible(true);

        #if MEMEBER_PURCHASE_AMOUT_TEST
            assert(passUnitTest);
        #endif
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
    tempMemberAdd.executiveStatus = ui->lineEdit_admin_membersubmission_executive->text();
    tempMemberAdd.expirationDate = ui->lineEdit_admin_membersubmission_date->text();

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
    ui->tableWidget_membership->hide();
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
    ui->tableWidget_membership->show();
    ui->gridWidget_membership_expire->hide();

    // wipe existing data on table
    ui->tableWidget_membership->setRowCount(0);
    executiveMemberIDList.clear();
    executiveAr.clear();
    regularMemberIDList.clear();
    regularAr.clear();
    upgradeCount = 0;
    downgradeCount = 0;
    upgradeIndex = 0;
    downgradeIndex = 0;

    QSqlQuery query;
    query.prepare("select memberID from members where memberType='Regular'");

    // Execute Query
    if(query.exec())
    {
        // iterate through and pull ids
        while(query.next())
        {
            regularMemberIDList.insert(upgradeIndex, query.value(0).toString());
            upgradeIndex++;
        }

        // DEBUG: print list
        for(upgradeIndex = 0; upgradeIndex < regularMemberIDList.size(); upgradeIndex++)
        {
            qDebug() << regularMemberIDList[upgradeIndex];
        }
    }
    else // if unsuccessful, print error
    {
        qDebug() << query.lastError().text();
    }

    // use executiveMemberID to pull purchase data from db into vector
    query.prepare("SELECT members.memberID, members.name, sum(purchases.qty * products.price) "
                  "FROM members, purchases, products "
                  "WHERE members.memberID = purchases.memberID "
                  "AND purchases.productID = products.productID "
                  "AND members.memberID = :memberID");

    // If vector empty
    if(regularAr.empty())
    {
        // Iterate through ID list, calling each member's purchases
        for(upgradeIndex = 0; upgradeIndex < regularMemberIDList.size(); upgradeIndex++)
        {
            query.bindValue(":memberID", regularMemberIDList[upgradeIndex]);

            // Execute Query
            if(query.exec())
            {
                // Iterate through query data and pull purchase information into vector
                while(query.next())
                {
                    if(query.value(0).toString() != "")
                    {
                        // Copy into temp object
                        regTemp.memberID = query.value(0).toString();
                        regTemp.name = query.value(1).toString();
                        regTemp.amountSpent = query.value(2).toString();

                        // Add object to vector
                        regularAr.append(regTemp);
                    }
                }
            }
            else // if unsuccessful, print error
            {
                qDebug() << query.lastError().text();
            }
        }
    }

   // Print entire vector
   for(upgradeIndex = 0; upgradeIndex < regularAr.count(); upgradeIndex++)
   {
       qDebug() << "PRINTING PERSON #" << upgradeIndex + 1;
       qDebug() << regularAr[upgradeIndex].memberID;
       qDebug() << regularAr[upgradeIndex].name;
       qDebug() << regularAr[upgradeIndex].amountSpent;
   }

   // Initializing tableWidget
   enum { MEMBERSHIP_NUMBER, MEMBER_NAME, AMT_SPENT, REBATE_AMOUNT };
   QStringList columns;
   ui->tableWidget_membership->setColumnCount(4);
   columns << "Membership Number" << "Member Name" << "Amount Spent" << "Rebate Amount";
   ui->tableWidget_membership->setHorizontalHeaderLabels(columns);


   float rebateAmount = 0.0;            // member's rebate received
   const float REBATE_RATE = 0.02;      // rebate rate for calculation
   const float REBATE_MIN = 65.0;       // minimum rebate needed for exec member

   // loop through purchases to collect all people. add to tableWidget if <65
   if(upgradeCount == 0)
   {
       for(upgradeIndex = 0; upgradeIndex < regularAr.count(); upgradeIndex++)
       {
           // Calculate estimated rebate
           rebateAmount = regularAr[upgradeIndex].amountSpent.toFloat() * REBATE_RATE;

           // If rebateAmount under rebateMin, add to recommendations
           if(rebateAmount > REBATE_MIN)
           {
               ui->tableWidget_membership->insertRow(upgradeCount);
               ui->tableWidget_membership->setItem(upgradeCount, MEMBERSHIP_NUMBER,
                                                   new QTableWidgetItem(regularAr[upgradeIndex].memberID));
               ui->tableWidget_membership->setItem(upgradeCount, MEMBER_NAME,
                                                   new QTableWidgetItem(regularAr[upgradeIndex].name));
               ui->tableWidget_membership->setItem(upgradeCount, AMT_SPENT,
                                                   new QTableWidgetItem(QString::number(regularAr[upgradeIndex].amountSpent.toFloat(), 'f', 2)));
               ui->tableWidget_membership->setItem(upgradeCount, REBATE_AMOUNT,
                                                   new QTableWidgetItem(QString::number(rebateAmount, 'f', 2)));
               upgradeCount++;
           }
       }
   }

   // Set label to display number of recommendations
   QString labelText = "Number of recommended membership upgrades: " + QString::number(upgradeCount);
   ui->label_membership_recommendation_status->setText(labelText);
}

void MainWindow::on_pushButton_membership_downgrades_clicked() // member downgrades list
{
    ui->tableWidget_membership->show();
    ui->gridWidget_membership_expire->hide();

    // wipe existing data on table
    ui->tableWidget_membership->clear();
    ui->tableWidget_membership->setRowCount(0);
    executiveMemberIDList.clear();
    executiveAr.clear();
    regularMemberIDList.clear();
    regularAr.clear();
    upgradeCount = 0;
    downgradeCount = 0;
    upgradeIndex = 0;
    downgradeIndex = 0;

    QSqlQuery query;
    query.prepare("select memberID from members where memberType='Executive'");

    // Execute Query
    if(query.exec())
    {
        // iterate through and pull ids
        while(query.next())
        {
            executiveMemberIDList.insert(downgradeIndex, query.value(0).toString());
            downgradeIndex++;
        }

        // DEBUG: print list
        for(downgradeIndex = 0; downgradeIndex < executiveMemberIDList.size(); downgradeIndex++)
        {
            qDebug() << executiveMemberIDList[downgradeIndex];
        }
    }
    else // if unsuccessful, print error
    {
        qDebug() << query.lastError().text();
    }

    // use executiveMemberID to pull purchase data from db into vector
    query.prepare("SELECT members.memberID, members.name, sum(purchases.qty * products.price) "
                  "FROM members, purchases, products "
                  "WHERE members.memberID = purchases.memberID "
                  "AND purchases.productID = products.productID "
                  "AND members.memberID = :memberID");

    // If vector empty
    if(executiveAr.empty())
    {
        // Iterate through ID list, calling each member's purchases
        for(downgradeIndex = 0; downgradeIndex < executiveMemberIDList.size(); downgradeIndex++)
        {
            query.bindValue(":memberID", executiveMemberIDList[downgradeIndex]);

            // Execute Query
            if(query.exec())
            {
                // Iterate through query data and pull purchase information into vector
                while(query.next())
                {
                    if(query.value(0).toString() != "")
                    {
                        // Copy into temp object
                        execTemp.memberID = query.value(0).toString();
                        execTemp.name = query.value(1).toString();
                        execTemp.amountSpent = query.value(2).toString();

                        // Add object to vector
                        executiveAr.append(execTemp);
                    }
                }
            }
            else // if unsuccessful, print error
            {
                qDebug() << query.lastError().text();
            }
        }
    }

   // Print entire vector
   for(downgradeIndex = 0; downgradeIndex < executiveAr.count(); downgradeIndex++)
   {
       qDebug() << "PRINTING PERSON #" << downgradeIndex + 1;
       qDebug() << executiveAr[downgradeIndex].memberID;
       qDebug() << executiveAr[downgradeIndex].name;
       qDebug() << executiveAr[downgradeIndex].amountSpent;
   }

   // Initializing tableWidget
   enum { MEMBERSHIP_NUMBER, MEMBER_NAME, AMT_SPENT, REBATE_AMOUNT };
   QStringList columns;
   ui->tableWidget_membership->setColumnCount(4);
   columns << "Membership Number" << "Member Name" << "Amount Spent" << "Rebate Amount";
   ui->tableWidget_membership->setHorizontalHeaderLabels(columns);


   float rebateAmount = 0.0;            // member's rebate received
   const float REBATE_RATE = 0.02;      // rebate rate for calculation
   const float REBATE_MIN = 65.0;       // minimum rebate needed for exec member

   // loop through purchases to collect all people. add to tableWidget if <65
   if(downgradeCount == 0)
   {
       for(downgradeIndex = 0; downgradeIndex < executiveAr.count(); downgradeIndex++)
       {
           // Calculate estimated rebate
           rebateAmount = executiveAr[downgradeIndex].amountSpent.toFloat() * REBATE_RATE;

           // If rebateAmount under rebateMin, add to recommendations
           if(rebateAmount < REBATE_MIN)
           {
               ui->tableWidget_membership->insertRow(downgradeCount);
               ui->tableWidget_membership->setItem(downgradeCount, MEMBERSHIP_NUMBER,
                                                   new QTableWidgetItem(executiveAr[downgradeIndex].memberID));
               ui->tableWidget_membership->setItem(downgradeCount, MEMBER_NAME,
                                                   new QTableWidgetItem(executiveAr[downgradeIndex].name));
               ui->tableWidget_membership->setItem(downgradeCount, AMT_SPENT,
                                                   new QTableWidgetItem(QString::number(executiveAr[downgradeIndex].amountSpent.toFloat(), 'f', 2)));
               ui->tableWidget_membership->setItem(downgradeCount, REBATE_AMOUNT,
                                                   new QTableWidgetItem(QString::number(rebateAmount, 'f', 2)));
               downgradeCount++;
           }
       }
   }

   // Set label to display number of recommendations
   QString labelText = "Number of recommended membership downgrades: " + QString::number(downgradeCount);
   ui->label_membership_recommendation_status->setText(labelText);
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
