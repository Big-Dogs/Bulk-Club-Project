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
    ui->comboBox_pos_itemlist->setEnabled(false);
    ui->pushButton_pos_purchase->setEnabled(false);


    ui->label_home_warning->hide();

    setPermissions(NONE);

    ui->pushButton_admin_confirmdeletemember->setEnabled(false);

    ui->gridWidget_admin_memberdatafields->hide();
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->gridWidget_admin_itemdatafields->hide();
    ui->gridWidget_admin_confirmdeleteitem->hide();

    ui->gridWidget_membership_expire->hide();

    ui->label_total_revune->setVisible(false);
    ui->label_sales_searchmembererrormessage->setVisible(false);
    ui->comboBox_sales_manymembersfound->setVisible(false);


    //preparing the line edit integer validations for add member
    idCheck = new QIntValidator;
    idCheck->setRange(0,100000);
    ui->lineEdit_admin_membersubmission_id->setValidator(idCheck);
    monthCheck = new QIntValidator;
    monthCheck->setRange(0,12);
    ui->lineEdit_admin_membersubmission_month->setValidator(monthCheck);
    dayCheck = new QIntValidator;
    dayCheck->setTop(31);
    dayCheck->setBottom(1);
    ui->lineEdit_admin_membersubmission_day->setValidator(dayCheck);
    yearCheck = new QIntValidator;
    yearCheck->setRange(1890,2021);
    ui->lineEdit_admin_membersubmission_year->setValidator(yearCheck);

    //sets up add member
    memberModel = nullptr;
    ui->lineEdit_admin_membersubmission_year->hide();
    ui->label_admin_slash1->hide();
    ui->lineEdit_admin_membersubmission_day->hide();
    ui->label_admin_slash2->hide();
    ui->lineEdit_admin_membersubmission_month->hide();
    ui->label_admin_membersubmission_date->hide();
    ui->label_admin_membersubmission_date_warning->hide();
    ui->label_admin_membersubmission_id_warning->hide();
    ui->label_admin_membersubmission_name_warning->hide();
    ui->label_admin_membersubmission_nameID_warning->hide();

    InitializeSalesTableView(); //initializes daily sales report
      
    ui->label_admin_products_errormessage->setVisible(false);

    itemModel = nullptr;

    //Setting up admin item submission line edits
    //Validators
    QIntValidator    *productIdValidator;    //The validator for product id
    QDoubleValidator *productPriceValidator; //The validator for product price

    //id
    productIdValidator = new QIntValidator;
    productIdValidator->setTop(1000);
    ui->lineEdit_admin_itemsubmission_id->setValidator(productIdValidator);

    //price
    productPriceValidator = new QDoubleValidator;
    productPriceValidator->setDecimals(2);
    productIdValidator->setBottom(0.01);
    ui->lineEdit_admin_itemsubmission_price->setValidator(productPriceValidator);
    ui->lineEdit_admin_itemsubmission_price->setMaxLength(6);

    //name
    ui->lineEdit_admin_itemsubmission_name->setMaxLength(50);


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


    if(permission == NONE)
    {
        ui->pushButton_POS->setEnabled(false);
        ui->pushButton_sales->setEnabled(false);
        ui->pushButton_members->setEnabled(false);
        ui->pushButton_admin->setEnabled(false);
        ui->stackedWidget_main->setCurrentIndex(HOME);
    }
    else if (permission == EMPLOYEE)
    {
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(false);
        ui->pushButton_members->setEnabled(false);
        ui->pushButton_admin->setEnabled(false);
        ui->stackedWidget_main->setCurrentIndex(POS);
    }
    else if (permission == MANAGER)
    {
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(true);
        ui->pushButton_members->setEnabled(true);
        ui->pushButton_admin->setEnabled(false);
        ui->stackedWidget_main->setCurrentIndex(SALES);
    }
    else if (permission == ADMINISTRATOR)
    {
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(true);
        ui->pushButton_members->setEnabled(true);
        ui->pushButton_admin->setEnabled(true);
        ui->stackedWidget_main->setCurrentIndex(ADMIN);
    }
}

void MainWindow::on_pushButton_employeepermissions_clicked()
{
    setPermissions(EMPLOYEE);
}

void MainWindow::on_pushButton_managerpermissions_clicked()
{
    setPermissions(MANAGER);
}

void MainWindow::on_pushButton_adminpermissions_clicked()
{
    setPermissions(ADMINISTRATOR);
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

}


void MainWindow::on_pushButton_sales_clicked() // sales page
{
    ui->stackedWidget_main->setCurrentIndex(SALES);
}

/*----Sales Window push buttons----*/
    void MainWindow::on_pushButton_sales_daily_clicked() // daily sales report
    {
        ui->stackedWidget_sales->setCurrentIndex(SALES_DAILY);
        InitializeSalesTableView();
    }

    void MainWindow::on_pushButton_sale_byday_clicked()
    {        
        QSqlQueryModel *dailySalesModel = new QSqlQueryModel;
        QSqlQuery query;
        // Filter expiration by month
        query.prepare("SELECT purchases.datePurchased, purchases.memberID, "
                                      "products.name, products.price, purchases.qty, products.price * purchases.qty "
                                      "FROM purchases join products "
                                      "ON (products.productID = purchases.productID) "
                                      "WHERE datePurchased = ?");
        qDebug() << "combo box: " << ui->comboBox_sales_byday->currentText();
        query.bindValue(0, ui->comboBox_sales_byday->currentText());

        if (query.exec())
        {
            dailySalesModel->setQuery(query);

        }
        else
        {
            qDebug() << query.lastError().text();

        }

        // Initialize tableView_sales_daily using querymodel

        ui->tableView_sales_daily->setModel(dailySalesModel);
        dailySalesModel->setHeaderData(DAILY_DATE, Qt::Horizontal, tr("Date"));
        dailySalesModel->setHeaderData(DAILY_ID, Qt::Horizontal, tr("Member ID"));
        dailySalesModel->setHeaderData(DAILY_ITEM, Qt::Horizontal, tr("Item"));
        dailySalesModel->setHeaderData(DAILY_PRICE, Qt::Horizontal, tr("Price"));
        dailySalesModel->setHeaderData(DAILY_QTY, Qt::Horizontal, tr("Qty"));
        dailySalesModel->setHeaderData(DAILY_TOTAL, Qt::Horizontal, tr("Total"));

        ui->tableView_sales_daily->resizeColumnToContents(2);

        // Hide numerical vertical header
        ui->tableView_sales_daily->verticalHeader()->setVisible(false);
        // Make fields uneditable
        ui->tableView_membership->setEditTriggers(QTableView::NoEditTriggers);

        

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

        //set up model
        if (memberModel != nullptr)
        {
            delete memberModel;
        }
        enum MembershipTableWidgetColumns{
            ID_COLUMN,
            NAME_COLUMN,
            MEMBERSHIP_TYPE_COLUMN,
            EXPIRATION_DATE_COLUMN,
            RENEWAL_PRICE_COLUMN
        };

        memberModel = new QSqlTableModel;
        memberModel->setTable("members");

        memberModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        memberModel->setSort(NAME_COLUMN, Qt::AscendingOrder);
        memberModel->setHeaderData(ID_COLUMN, Qt::Horizontal, QVariant("Member ID"));
        memberModel->setHeaderData(NAME_COLUMN, Qt::Horizontal, QVariant("Name"));
        memberModel->setHeaderData(MEMBERSHIP_TYPE_COLUMN, Qt::Horizontal, QVariant("Membership Type"));
        memberModel->setHeaderData(EXPIRATION_DATE_COLUMN, Qt::Horizontal, QVariant("Expiration Date"));
        memberModel->setHeaderData(RENEWAL_PRICE_COLUMN, Qt::Horizontal, QVariant("Renewal Cost"));
        memberModel->select();


        //set up view
        ui->tableView_admin_members->setModel(memberModel);
        ui->tableView_admin_members->setItemDelegateForColumn(RENEWAL_PRICE_COLUMN, formatPrice);
        ui->tableView_admin_members->resizeColumnToContents(NAME_COLUMN);
        ui->tableView_admin_members->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableView_admin_members->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView_admin_members->setFocusPolicy(Qt::NoFocus);
        ui->tableView_admin_members->setWordWrap(false);

//        QSqlQueryModel *model = new QSqlQueryModel();
//        QSqlQuery * query = new QSqlQuery;
//        query->prepare("select * from members");

//        if(!query->exec())
//        {
//            qDebug() << query->lastError().text();
//        }
//        else
//        {
//            model->setQuery(*query);
//            ui->tableView_admin_members->setModel(model);
//        }
//        delete query;
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

        ui->tableView_admin_inventory->setWordWrap(false);

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

     //preparing the line edits to have a max character amount
     ui->lineEdit_admin_membersubmission_month->setMaxLength(2);
     ui->lineEdit_admin_membersubmission_day->setMaxLength(2);
     ui->lineEdit_admin_membersubmission_year->setMaxLength(4);


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

    ui->pushButton_admin_deletemember->setEnabled(true);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);

    //Created strings that are used to assign a date to the new member
    QString thisYear = QDate::currentDate().toString("yyyy");
    QString thisMonth = QDate::currentDate().toString("M");
    QString thisDay = QDate::currentDate().toString("dd");

    //Takes care of memberships that start on the 29th of feb
    if(QDate::isLeapYear(thisYear.toInt()) && QDate::currentDate().toString("M/dd") == "2/29")
        thisDay = QString::number(thisDay.toInt() - 1);
    QString nextYear = QString::number(thisYear.toInt() + 1);

    TempMember tempMemberAdd;

    //Populates the tempMember struct instance with the new member information.
    tempMemberAdd.id = ui->lineEdit_admin_membersubmission_id->text();

    //Formats the name to make sure the first letter is always capitalized, and the rest are lower case
    tempMemberAdd.name = ui->lineEdit_admin_membersubmission_name->text();
    QString tempName = tempMemberAdd.name;
    tempName = tempName.toLower();
    tempName[0] = tempMemberAdd.name[0].toUpper();
    tempMemberAdd.name = tempName;


       //     QString(QChar::fromLatin1(firstLetterName);

    qDebug() << tempMemberAdd.name;
    if(ui->radioButton_admin_member->isChecked())
        tempMemberAdd.executiveStatus = "Executive";
    else
        tempMemberAdd.executiveStatus = "Regular";

    tempMemberAdd.expMonth = thisMonth;
    tempMemberAdd.expDay = thisDay;
    tempMemberAdd.expYear = nextYear;

    //creates the expiration date string
    tempMemberAdd.expDate = tempMemberAdd.expMonth + "/"
                          + tempMemberAdd.expDay   + "/"
                          + tempMemberAdd.expYear;

    //sets the renewal price for the member based on executive status
    int renewalPrice = 0;
    if(tempMemberAdd.executiveStatus == "Executive")
        renewalPrice = 120;
    else if(tempMemberAdd.executiveStatus == "Regular")
        renewalPrice = 60;

    if(tempMemberAdd.name != "")
    {
        if(tempMemberAdd.id != "")
        {
            //inserts the member into the database
            QSqlQuery query;
            query.prepare("INSERT INTO members "
                          "(memberID, name, "
                          "memberType, expireDate,"
                          "renewalPrice) VALUES(?,?,?,?,?)");

            query.addBindValue(tempMemberAdd.id);
            query.addBindValue(tempMemberAdd.name);
            query.addBindValue(tempMemberAdd.executiveStatus);
            query.addBindValue(tempMemberAdd.expDate);
            query.addBindValue(QString::number(renewalPrice));

            if(!query.exec())
            {
                qDebug() << "Member failed to save" << memberModel->lastError();
                ui->label_admin_membersubmission_nameID_warning->show();

            }
            else
            {
                //clears out all of the member fields, hides the add member widgets, and warnings
                MainWindow::on_pushButton_admin_member_clicked();
                MainWindow::ClearMemberFields();
                ui->gridWidget_admin_memberdatafields->hide();
                ui->label_admin_membersubmission_date_warning->hide();
                ui->label_admin_membersubmission_id_warning->hide();
                ui->label_admin_membersubmission_name_warning->hide();
                ui->label_admin_membersubmission_nameID_warning->hide();
                qDebug() << tempMemberAdd.expYear.toInt(),
                            tempMemberAdd.expMonth.toInt(),
                            tempMemberAdd.expDay.toInt();
            }
        }
        else
        {   //sets the correct warning messages
            ui->label_admin_membersubmission_id_warning->show();
            ui->label_admin_membersubmission_name_warning->hide();
            qDebug() << itemModel->lastError().text();
        }
    }
    else
    {   //sets the correct warning messages
        ui->label_admin_membersubmission_name_warning->show();

        if(tempMemberAdd.id == "")
            ui->label_admin_membersubmission_id_warning->show();
    }
}

void MainWindow::on_pushButton_admin_membersubmission_cancel_clicked() // cancels submission for adding/editing
{
    ui->gridWidget_admin_memberdatafields->hide();
    ui->pushButton_admin_deletemember->setEnabled(true);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);

    MainWindow::ClearMemberFields();
    ui->label_admin_membersubmission_date_warning->hide();
    ui->label_admin_membersubmission_id_warning->hide();
    ui->label_admin_membersubmission_name_warning->hide();
    ui->label_admin_membersubmission_nameID_warning->hide();
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
    ui->pushButton_admin_itemsubmission_submit->setVisible(true);
    ui->pushButton_admin_itemsubmission_cancel->setVisible(true);

    //setting up line edit for input
    ui->lineEdit_admin_itemsubmission_id->setReadOnly(false);
    ui->lineEdit_admin_itemsubmission_name->setReadOnly(false);
    ui->lineEdit_admin_itemsubmission_price->setReadOnly(false);
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
            ui->pushButton_admin_deleteitem->setEnabled(false); //no item is selected
            ui->pushButton_admin_additem->setEnabled(true);
            ui->pushButton_admin_edititem->setEnabled(false); //no item is selected

            //making sure no items are selected
            ui->tableView_admin_inventory->clearSelection();

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
    ui->pushButton_admin_edititem->setEnabled(false); //no item is selected

    //making sure no items are selected
    ui->tableView_admin_inventory->clearSelection();

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

        //resets the table
        on_pushButton_admin_member_clicked();

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

    printReceipt();

//    QDate::currentDate().toString("m/dd/yyyy");

    int addPurchaseID = posMemberID;

    int purchaseQty = posQty;

    this->database->addPurchase(addPurchaseID, posItem, QDate::currentDate().toString("M/dd/yyyy"), purchaseQty);

    ui->pushButton_pos_purchase->setEnabled(false);
    ui->comboBox_pos_itemlist->setEnabled(false);
}

void MainWindow::on_comboBox_pos_memberlist_activated(int index)
{
    posMemberID = ui->comboBox_pos_memberlist->currentText().toInt();
    ui->comboBox_pos_itemlist->setEnabled(true);
}

void MainWindow::on_comboBox_pos_itemlist_activated(int index)
{
    posItemName = ui->comboBox_pos_itemlist->currentText();
    posItem = this->database->getItem(posItemName);
    ui->comboBox_pos_qty->setEnabled(true);

}
void MainWindow::on_comboBox_pos_qty_activated(int index)
{
    posQty = index + 1;
    posPrice = this->database->getPrice(posItemName);
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
    ui->radioButton_admin_member->setChecked(false);
    ui->lineEdit_admin_membersubmission_month->clear();
    ui->lineEdit_admin_membersubmission_day->clear();
     ui->lineEdit_admin_membersubmission_year->clear();
}


/*----Home Page push buttons----*/
void MainWindow::on_pushButton_home_login_clicked()
{
    ui->label_home_warning->hide();

    QString username = ui->lineEdit_home_username->text();
    QString password = ui->lineEdit_home_password->text();

    int permissionIndex = this->database->checkLogin(username, password);

    if (permissionIndex > NONE)
    {
        setPermissions(permissionIndex);

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



void MainWindow::InitializeSalesTableView()
{

    ui->comboBox_sales_byday->clear();
    QSqlQuery query;
    query.prepare("select datePurchased from purchases group by datePurchased");
    //sales by day combo box
    if(query.exec())
    {
       while (query.next())
       {
           ui->comboBox_sales_byday->addItem(query.value("datePurchased").toString());
       }
    }
    else
    {
        qDebug() << query.lastError().text();
    }


    // Hide numerical vertical header
    ui->tableView_sales_daily->verticalHeader()->setVisible(false);
    // Make fields uneditable
    ui->tableView_membership->setEditTriggers(QTableView::NoEditTriggers);
}

//initialize pos table
void MainWindow::InitializePosTable()
{
    QStringList TableHeader;
    ui->tableWidget_pos_receipts->setColumnCount(5);
    TableHeader << "Member ID" << "Item" << "Price" << "qty" << "Total";
    ui->tableWidget_pos_receipts->setHorizontalHeaderLabels(TableHeader);
    ui->tableWidget_pos_receipts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_pos_receipts->setShowGrid(false);
    ui->tableWidget_pos_receipts->verticalHeader()->setVisible(false);
    ui->tableWidget_pos_receipts->setColumnWidth(1, 300);
    receiptRow = 0;

    //initializing combo boxes
    QStringList members = this->database->getPOSMembers();
    qDebug() << members.length() << " items";
    //item number combo box
    if(ui->comboBox_pos_memberlist->count() == 0)
    {
        for (int i = 0; i < members.length(); i++)
        {
            ui->comboBox_pos_memberlist->addItem(members.at(i));
        }
    }
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

void MainWindow::printReceipt()
{
    QTableWidgetItem *member = new QTableWidgetItem;
    QTableWidgetItem *item = new QTableWidgetItem;
    QTableWidgetItem *price = new QTableWidgetItem;
    QTableWidgetItem *qty = new QTableWidgetItem;
    QTableWidgetItem *total = new QTableWidgetItem;

    member->setText(QString::number(posMemberID));
    item->setText(posItemName);
    price->setText(QString::number(posPrice));
    qty->setText(QString::number(posQty));
    total->setText(QString::number(posTotal));

    ui->tableWidget_pos_receipts->insertRow(receiptRow);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 0, member);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 1, item);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 2, price);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 3, qty);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 4, total);
    receiptRow++;

}





void MainWindow::on_comboBox_pos_memberlist_currentIndexChanged(int index)
{
    ui->tableWidget_pos_receipts->clear();
    InitializePosTable();
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

//
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
