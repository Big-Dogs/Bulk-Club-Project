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

    ui->pushButton_admin_confirmdeletemember->setEnabled(false);

    ui->gridWidget_admin_memberdatafields->hide();
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->gridWidget_admin_itemdatafields->hide();
    ui->gridWidget_admin_confirmdeleteitem->hide();

    ui->gridWidget_membership_expire->hide();

    ui->label_total_revune->setVisible(false);
    ui->label_sales_searchmembererrormessage->setVisible(false);
    ui->comboBox_sales_manymembersfound->setVisible(false);

    ui->label_admin_products_errormessage->setVisible(false);

    itemModel = nullptr;

    //Setting up admin item submission line edits
    //Validators
    QIntValidator    *productIdValidator;    //The validator for product id
    QDoubleValidator *productPriceValidator; //The validator for product price

    //id
    productIdValidator = new QIntValidator;
    productIdValidator->setBottom(1);
    ui->lineEdit_admin_itemsubmission_id->setValidator(productIdValidator);

    //price
    productPriceValidator = new QDoubleValidator;
    productPriceValidator->setDecimals(2);
    productIdValidator->setBottom(0.01);
    ui->lineEdit_admin_itemsubmission_price->setValidator(productPriceValidator);

    qDebug() << "feature: " << database->driver()->hasFeature(QSqlDriver::PositionalPlaceholders);

}


MainWindow::~MainWindow()
{
    delete ui;

    delete database;

    delete formatPrice;

    if (itemModel != nullptr)
    {
        delete itemModel;
    }

   delete ui->lineEdit_admin_itemsubmission_id->validator();

   delete ui->lineEdit_admin_itemsubmission_price->validator();
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
        //Constant
        const int PRODUCT_ID_COLUMN    = 0; //The column for the product id
        const int PRODUCT_NAME_COLUMN  = 1; //The column for the product name
        const int PRODUCT_PRICE_COLUMN = 2; //The column for the procdut price

        //Hidding error message
        ui->label_admin_products_errormessage->setVisible(false);

        ui->stackedWidget_admin->setCurrentIndex(ADMIN_ITEM);
        ui->gridWidget_admin_itemdatafields->setVisible(false);

        //set up model
        if (itemModel != nullptr)
        {
            delete itemModel;
        }
        itemModel = new QSqlTableModel;

        itemModel->setTable("products");

        itemModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

        itemModel->setSort(PRODUCT_NAME_COLUMN, Qt::AscendingOrder);

        itemModel->setHeaderData(PRODUCT_ID_COLUMN, Qt::Horizontal, QVariant("Product ID"));

        itemModel->setHeaderData(PRODUCT_NAME_COLUMN, Qt::Horizontal, QVariant("Product Name"));

        itemModel->setHeaderData(PRODUCT_PRICE_COLUMN, Qt::Horizontal, QVariant("Price"));

        itemModel->select();

        //set up view
        ui->tableView_admin_inventory->setModel(itemModel);

        ui->tableView_admin_inventory->setItemDelegateForColumn(PRODUCT_PRICE_COLUMN, formatPrice);

        ui->tableView_admin_inventory->resizeColumnToContents(PRODUCT_NAME_COLUMN);

        ui->tableView_admin_inventory->setEditTriggers(QAbstractItemView::AnyKeyPressed);
      
        ui->tableView_admin_inventory->setSelectionMode(QAbstractItemView::SingleSelection);

        ui->tableView_admin_inventory->setSelectionBehavior(QAbstractItemView::SelectRows);

        ui->tableView_admin_inventory->setFocusPolicy(Qt::NoFocus);

        //connecting to dataChanged and currentChanged
        QObject::connect(itemModel, &QSqlTableModel::dataChanged, this, &MainWindow::on_tableModel_dataChanged);
        QObject::connect(ui->tableView_admin_inventory, &QTableView::selectRow, this, &MainWindow::on_tableView_item_currentChanged);


        //Disabling edit and delete functionality since no item is selected
        ui->pushButton_admin_edititem->setEnabled(false);
        ui->pushButton_admin_deleteitem->setEnabled(false);
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
    ui->pushButton_admin_itemsubmission_submit->setVisible(true);
    ui->pushButton_admin_itemsubmission_cancel->setVisible(true);

    //setting up line edit for input
    ui->lineEdit_admin_itemsubmission_id->setReadOnly(false);
    ui->lineEdit_admin_itemsubmission_name->setReadOnly(false);
    ui->lineEdit_admin_itemsubmission_price->setReadOnly(false);

    ui->lineEdit_admin_itemsubmission_id->setText(QString());
    ui->lineEdit_admin_itemsubmission_name->setText(QString());
    ui->lineEdit_admin_itemsubmission_price->setText(QString());

    itemModel->insertRows(itemModel->rowCount(), /*Count: */ 1); //inserting 1 row at bottom

    //selecting an abitary column from the row just inserted
    ui->tableView_admin_inventory->setCurrentIndex(itemModel->index(itemModel->rowCount() - 1, 0));
}

void MainWindow::on_pushButton_admin_edititem_clicked() // edit item button
{
    ui->gridWidget_admin_itemdatafields->show();
    ui->pushButton_admin_deleteitem->setEnabled(false);
    ui->pushButton_admin_additem->setEnabled(false);
}

void MainWindow::on_pushButton_admin_deleteitem_clicked() // delete item button
{
    //Variables
    QString     confirmDeleteMessage; //The message display to confirm the deletion
    QModelIndex deleteProduct;        //The index of the product being deleted

    currentProcessIndex = ui->tableView_admin_inventory->currentIndex();

    ui->gridWidget_admin_confirmdeleteitem->show();
    ui->pushButton_admin_edititem->setEnabled(false);
    ui->pushButton_admin_additem->setEnabled(false);

    deleteProduct = ui->tableView_admin_inventory->currentIndex();

    deleteProduct = deleteProduct.sibling(deleteProduct.row(), itemModel->fieldIndex("name"));

    confirmDeleteMessage = "Delete ";
    confirmDeleteMessage.append(deleteProduct.data().toString());
    confirmDeleteMessage.append('?');
    ui->label_admin_confirmdeleteitem->setText(confirmDeleteMessage);

    bool deleteError = itemModel->removeRow(deleteProduct.row());

    if (!deleteError)
    {
        qDebug() << itemModel->lastError().text();
    }
}

void MainWindow::on_pushButton_admin_itemsubmission_submit_clicked() //confirms add/edit
{
    //Constant
    const int PRODUCT_ID_COLUMN    = 0; //The column for the product id
    const int PRODUCT_NAME_COLUMN  = 1; //The column for the product name
    const int PRODUCT_PRICE_COLUMN = 2; //The column for the procdut price

    //Variable
    bool inputIsValid = true;  //A bool that is true if the user input
                               //is valid and false otherwise
    QModelIndex input;         //The model index of the input currently
                               //being validated
    QString errorMessage = ""; //The QString containing the message that is
                               //displayed if the input is not valid

    input = ui->tableView_admin_inventory->currentIndex();

    //Validating product id
    input = input.model()->index(input.row(), PRODUCT_ID_COLUMN);

    if (input.data().toInt() < 0)
    {
        errorMessage.append("Please enter a positive value for the id \n");

        inputIsValid = false;
    }

    //Validating product price
    input = input.model()->index(input.row(), PRODUCT_PRICE_COLUMN);

    if (input.data().toDouble() <= 0.0)
    {
        errorMessage.append("Please enter a positive value for the price \n");

        inputIsValid = false;
    }

    //Validating product name
    input = input.model()->index(input.row(), PRODUCT_NAME_COLUMN);

    if (input.data().toString() == QString())
    {
        errorMessage.append(("Please enter a product name \n"));

        inputIsValid = false;
    }

    if (inputIsValid)
    {
        qDebug() << input.data().toString();
        itemModel->setData(input, QVariant(normalizeCapitalization(input.data().toString())));
        qDebug() << input.data().toString();
      
        bool submitError = itemModel->submitAll();

        if (!submitError)
        {
            qDebug() << itemModel->lastError().text();

            if (itemModel->lastError().text() == "UNIQUE constraint failed: products.name Unable to fetch row")
            {
                ui->label_admin_products_errormessage->setText("Product name isn't unique, please enter \n"
                                                               "another name");

                ui->label_admin_products_errormessage->setVisible(true);
            }

            if (itemModel->lastError().text() == "UNIQUE constraint failed: products.productID Unable to fetch row")
            {
                ui->label_admin_products_errormessage->setText("Product id isn't unique, please enter \n"
                                                               "another id");

                ui->label_admin_products_errormessage->setVisible(true);
            }
        }
        else
        {
            ui->gridWidget_admin_itemdatafields->hide();
            ui->pushButton_admin_deleteitem->setEnabled(true);
            ui->pushButton_admin_additem->setEnabled(true);
            ui->pushButton_admin_edititem->setEnabled(true);

            //Clearing line edits
            ui->lineEdit_admin_itemsubmission_id->setText(QString());
            ui->lineEdit_admin_itemsubmission_name->setText(QString());
            ui->lineEdit_admin_itemsubmission_price->setText(QString());

            itemModel->select();

            //Hidding error message
            ui->label_admin_products_errormessage->setVisible(false);
        }
    }
    else
    {
        ui->label_admin_products_errormessage->setText(errorMessage);

        ui->label_admin_products_errormessage->setVisible(true);
    }
}

void MainWindow::on_pushButton_admin_itemsubmission_cancel_clicked() // cancels add/edit
{
    ui->gridWidget_admin_itemdatafields->hide();
    ui->pushButton_admin_additem->setEnabled(true);

    //Clearing line edits
    ui->lineEdit_admin_itemsubmission_id->setText(QString());
    ui->lineEdit_admin_itemsubmission_name->setText(QString());
    ui->lineEdit_admin_itemsubmission_price->setText(QString());

    //Removing unsubmitted row
    itemModel->revertAll();

    //Hidding error message
    ui->label_admin_products_errormessage->setVisible(false);
}

void MainWindow::on_pushButton_admin_confirmdeleteitem_clicked() // confirms delete
{
    ui->gridWidget_admin_confirmdeleteitem->hide();
    ui->gridWidget_admin_itemdatafields->setVisible(false);
    ui->pushButton_admin_deleteitem->setEnabled(false); //no item will be selected
    ui->pushButton_admin_additem->setEnabled(true);

    ui->label_admin_products_errormessage->setVisible(false);

    itemModel->submitAll();
}

void MainWindow::on_pushButton_admin_canceldeleteitem_clicked() // cancels delete
{
    ui->gridWidget_admin_confirmdeleteitem->hide();
    ui->gridWidget_admin_itemdatafields->setVisible(false);
    ui->pushButton_admin_deleteitem->setEnabled(false); //no item will be selected
    ui->pushButton_admin_additem->setEnabled(true);

    ui->label_admin_products_errormessage->setVisible(false);

    itemModel->revertAll();

    itemModel->select();
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
    executiveMemberIDList.clear();
    executiveAr.clear();
    regularMemberIDList.clear();
    regularAr.clear();
    upgradeCount = 0;
    downgradeCount = 0;
    upgradeIndex = 0;
    downgradeIndex = 0;


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
    ui->lineEdit_admin_membersubmission_executive->clear();
    ui->lineEdit_admin_membersubmission_date->clear();
}

void MainWindow::on_stackedWidget_admin_currentChanged(int arg1)
{

}

/* I inserted this slot and as a result if I remove it will
 * generate a compile time error
 *
 * if you know how to properly remove please do
 */
void MainWindow::on_stackedWidget_admin_widgetRemoved(int index)
{

}

void MainWindow::on_stackedWidget_main_currentChanged(int arg1)
{

}

void MainWindow::on_stackedWidget_sales_currentChanged(int arg1)
{

}

void MainWindow::on_tableView_admin_inventory_activated(const QModelIndex &index)
{
    //Variables
    QModelIndex     idIndex;       //The index at the id column in the same row as index
    QModelIndexList previousIndex; //A QModelIndex to store the previous index if necessary

    //checking if table model is clean or dirty
    //dirty means data is being added, updated, or deleted
    if (!itemModel->isDirty())
    {

    }
    else
    {
        //Data is being added, updated, or deleted
        //The button that is enable tells me which one it is

        if (ui->pushButton_admin_additem->isEnabled())
        {
            qDebug() << "Detects add item";
            if (index.row() != itemModel->rowCount() - 1)
            {
                //display error message
                ui->label_admin_products_errormessage->setText("Warning, The item you are adding is not saved. \n "
                                                               "Please either submit or cancel the addition of this item before moving on.");
                ui->label_admin_products_errormessage->setVisible(true);

                ui->tableView_admin_inventory->setCurrentIndex(itemModel->index(itemModel->rowCount() - 1, itemModel->fieldIndex("productID")));
            }
        }

        if (ui->pushButton_admin_edititem->isEnabled())
        {

        }

        if (ui->pushButton_admin_deleteitem->isEnabled());
        {
            idIndex = index.sibling(index.row(), itemModel->fieldIndex("productID"));
            if (idIndex.data().toString() == ui->label_admin_itemsubmission_id->text())
            {
                //display error message
                ui->label_admin_products_errormessage->setText("Warning, Please confirm or cancel the current deletion \n "
                                                               "before editing or adding another product.");

                ui->label_admin_products_errormessage->setVisible(true);

                previousIndex = itemModel->match(idIndex, Qt::EditRole, QVariant(ui->label_admin_itemsubmission_id->text()));

                if (!previousIndex.isEmpty())
                {
                    ui->tableView_admin_inventory->setCurrentIndex(previousIndex.first());
                }
                else
                {
                    qDebug() << "Something went wrong";
                }
            }//end if (index.data().toString() == ui->label_admin_itemsubmission_id->text())
        }//end if (ui->pushButton_admin_deleteitem->isEnabled());
    }
}

//Keeping the table model updated with what is new the line edits
void MainWindow::on_lineEdit_admin_itemsubmission_id_textEdited(const QString &productId)
{
    //Constant
    const int PRODUCT_ID_COLUMN = 0; //The column for the product id

    //Variables
    QModelIndex productIdIndex; //The model index of the product id for the selected row

    productIdIndex = ui->tableView_admin_inventory->currentIndex();

    //making sure the product id row is selected
    productIdIndex = productIdIndex.model()->index(productIdIndex.row(), PRODUCT_ID_COLUMN);

    qDebug() << QVariant(productId).toInt();
    if (QVariant(productId).isNull())
    {
        qDebug() << "null";
    }
    else
    {
        qDebug() << "not null";
    }
    itemModel->setData(productIdIndex, QVariant(productId));
}

void MainWindow::on_lineEdit_admin_itemsubmission_name_textEdited(const QString &productName)
{
    //Constant
    const int PRODUCT_NAME_COLUMN = 1; //The column for the product price

    //Variables
    QModelIndex productNameIndex; //The model index of the product name for the selected row

    qDebug() << "retrieving index selected";
    productNameIndex = ui->tableView_admin_inventory->currentIndex();

    qDebug() << "correcting index selected";
    //making sure the product id row is selected
    productNameIndex = productNameIndex.model()->index(productNameIndex.row(), PRODUCT_NAME_COLUMN);

    if (QVariant(productName).isNull())
    {
        qDebug() << "null";
    }
    else
    {
        qDebug() << "not null";
    }
    itemModel->setData(productNameIndex, QVariant(productName));
}

void MainWindow::on_lineEdit_admin_itemsubmission_price_textEdited(const QString &productPrice)
{
    //Constant
    const int PRODUCT_PRICE_COLUMN = 2; //The column for the product price

    //Variables
    QModelIndex productPriceIndex; //The model index of the product price for the selected row

    productPriceIndex = ui->tableView_admin_inventory->currentIndex();

    //making sure the product id row is selected
    productPriceIndex = productPriceIndex.model()->index(productPriceIndex.row(), PRODUCT_PRICE_COLUMN);

    itemModel->setData(productPriceIndex, QVariant(productPrice));
}

void MainWindow::on_tableView_admin_inventory_clicked(const QModelIndex &index)
{
    //Constant
    const int PRODUCT_ID_COLUMN    = 0; //The column for the product id
    const int PRODUCT_NAME_COLUMN  = 1; //The column for the product name
    const int PRODUCT_PRICE_COLUMN = 2; //The column for the product price

    //Variables
    QModelIndex productData; //The QModelIndex use to reflect the product
                             //data currently being processed
    QModelIndexList previousIndex; //A QModelIndex to store the previous index if necessary


    //checking if table model is clean or dirty
    //dirty means data is being added, updated, or deleted
    if (!itemModel->isDirty())
    {
        //Displaying data of selected item
        ui->gridWidget_admin_itemdatafields->setVisible(true);
        ui->pushButton_admin_itemsubmission_submit->setVisible(false);
        ui->pushButton_admin_itemsubmission_cancel->setVisible(false);

        productData = index.sibling(index.row(), PRODUCT_ID_COLUMN);
        ui->lineEdit_admin_itemsubmission_id->setText(productData.data().toString());

        productData = index.sibling(index.row(), PRODUCT_NAME_COLUMN);
        ui->lineEdit_admin_itemsubmission_name->setText(productData.data().toString());

        productData = index.sibling(index.row(), PRODUCT_PRICE_COLUMN);
        ui->lineEdit_admin_itemsubmission_price->setText(productData.data().toString());

        //making data uneditable
        ui->lineEdit_admin_itemsubmission_id->setReadOnly(true);
        ui->lineEdit_admin_itemsubmission_name->setReadOnly(true);
        ui->lineEdit_admin_itemsubmission_price->setReadOnly(true);

        //enable edit and delete functionality
        ui->pushButton_admin_edititem->setEnabled(true);
        ui->pushButton_admin_deleteitem->setEnabled(true);
    }
    else
    {
        qDebug() << "detected as dirty";
        //Data is being added, updated, or deleted
        //The button that is enable tells me which one it is

        if (ui->pushButton_admin_additem->isEnabled())
        {
            if (index.row() != itemModel->rowCount() - 1)
            {
                //display error message
                ui->label_admin_products_errormessage->setText("Warning, The item you are adding is not saved. \n "
                                                               "Please either submit or cancel the addition of \n "
                                                               "this item before moving on.");
                ui->label_admin_products_errormessage->setVisible(true);

                ui->tableView_admin_inventory->setCurrentIndex(itemModel->index(itemModel->rowCount() - 1, itemModel->fieldIndex("productID")));
            }
        }

        if (ui->pushButton_admin_edititem->isEnabled())
        {

        }

        if (ui->pushButton_admin_deleteitem->isEnabled())
        {
            qDebug() << "detected deletion";
            productData = index.sibling(index.row(), itemModel->fieldIndex("productID"));

            if (productData != currentProcessIndex.sibling(currentProcessIndex.row(), itemModel->fieldIndex("productID")))
            {
                //display error message
                ui->label_admin_products_errormessage->setText("Warning, Please confirm or cancel the current \n"
                                                               "deletion before editing or adding another \n"
                                                               "product.");

                ui->label_admin_products_errormessage->setVisible(true);

                ui->tableView_admin_inventory->setCurrentIndex(currentProcessIndex);

            }//end if (index.data().toString() == ui->label_admin_itemsubmission_id->text())
        }//end if (ui->pushButton_admin_deleteitem->isEnabled());
    }
}

QString MainWindow::normalizeCapitalization(QString text)
{
    //Vairables
    QString::Iterator textIterator;   //An iterator over text
    bool              newWord = true; //A bool value true if processing a new word

    text = text.toLower();

    textIterator = text.begin();

    for (textIterator = text.begin(); textIterator != text.end(); ++textIterator)
    {
         if (newWord && textIterator->isLetter())
         {
            *textIterator = textIterator->toUpper();

             newWord = false;
         }

         if (textIterator->isSpace())
         {
             newWord = true;
         }
    }

    return text;
}

void MainWindow::on_tableModel_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                               const QVector<int> &roles)
{
    //Constant
    const int PRODUCT_ID_COlUMN    = 0; //The column for product id
    const int PRODUCT_NAME_COLUMN  = 1; //The column for product name
    const int PRODCUT_PRICE_COLUMN = 2; //The column for product price
    //Variables
    QModelIndex selectedRow; //The model index in the selected row

    selectedRow = ui->tableView_admin_inventory->currentIndex();
    selectedRow = selectedRow.model()->index(selectedRow.row(), PRODUCT_ID_COlUMN);

    if (topLeft == selectedRow)
    {
        ui->lineEdit_admin_itemsubmission_id->setText(selectedRow.data().toString());
    }
    else
    {
        selectedRow = selectedRow.model()->index(selectedRow.row(), PRODUCT_NAME_COLUMN);

        if (topLeft == selectedRow)
        {
            ui->lineEdit_admin_itemsubmission_name->setText(selectedRow.data().toString());
        }
        else
        {
            selectedRow = selectedRow.model()->index(selectedRow.row(), PRODCUT_PRICE_COLUMN);

            ui->lineEdit_admin_itemsubmission_price->setText(selectedRow.data().toString());
        }
    }
}

void MainWindow::on_tableView_admin_inventory_pressed(const QModelIndex &index)
{
    qDebug() << "pressed";
}

void MainWindow::on_tableView_item_currentChanged(int row)
{
    qDebug() << "current changed";
}

//void restrictSelectToRow(const QModelIndex &selectedRow)
//{
//    //Variable
//    QAbstractItemModel *model = selectedRow.model(); //The model selectedRow exist in

//    for (int columnIndex = 0; columnIndex < model->columnCount(); columnIndex++)
//    {
//        for (int rowIndex = 0; rowIndex < model->rowCount(); rowIndex++)
//        {
//            //skipping selectedRow's row
//            if (rowIndex != selectedRow.row())
//            {
//                model
//            }
//        }
//    }
//}
