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

    this->parent = parent;

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
    sortItemModel = new QSqlTableModel;

    // Set default indices
    ui->stackedWidget_main->setCurrentIndex(HOME); 
    ui->stackedWidget_sales->setCurrentIndex(SALES_DAILY);
    ui->stackedWidget_admin->setCurrentIndex(ADMIN_MEMBER);

    // Ensure user is logged out when program starts
    setPermissions(NONE);
    ui->label_home_warning->setText("");

    // Initialize point of sale page
    InitializePosTable();
    ui->comboBox_pos_qty->setEnabled(false);
    ui->comboBox_pos_itemlist->setEnabled(false);
    ui->pushButton_pos_purchase->setEnabled(false);

    // Initialize sales page
    InitializeSalesTableView();
    ui->label_total_revenue->setVisible(false);
    ui->label_sales_searchmembererrormessage->setVisible(false);

    // Initializes membership page
    ui->tableWidget_membership->hide();
    ui->gridWidget_membership_expire->hide();

    // Initializes admin page
    ui->pushButton_admin_confirmdeletemember->setEnabled(false);
    ui->gridWidget_admin_memberdatafields->hide();
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->gridWidget_admin_itemdatafields->hide();
    ui->gridWidget_admin_confirmdeleteitem->hide();

    // Prepare line edit integer validations for 'Add Member' feature
    idCheck = new QIntValidator;
    idCheck->setRange(0,100000);
    ui->lineEdit_admin_membersubmission_id->setValidator(idCheck);

    // Initialize 'Add Member' feature
    memberModel = nullptr;
    ui->label_admin_membersubmission_nameID_warning->hide();

    // Initialize 'Edit Member' and 'Delete Member' features
    ui->pushButton_admin_editmember->hide();
    ui->pushButton_admin_deletemember->setEnabled(false);
  
    ui->label_admin_products_errormessage->setVisible(false);

    itemModel = nullptr;

    // Initialize administrator item submission line edits
    // Validators
    QIntValidator    *productIdValidator;    //The validator for product id
    QDoubleValidator *productPriceValidator; //The validator for product price

        // ID
    productIdValidator = new QIntValidator;
    productIdValidator->setTop(1000);
    ui->lineEdit_admin_itemsubmission_id->setValidator(productIdValidator);

        // Price
    productPriceValidator = new QDoubleValidator;
    productPriceValidator->setDecimals(2);
    productIdValidator->setBottom(0.01);
    ui->lineEdit_admin_itemsubmission_price->setValidator(productPriceValidator);
    ui->lineEdit_admin_itemsubmission_price->setMaxLength(6);

        // Name
    ui->lineEdit_admin_itemsubmission_name->setMaxLength(50);

    // Initialize positional placeholders
    qDebug() << "feature: " << database->driver()->hasFeature(QSqlDriver::PositionalPlaceholders);

    // Hides the total revenue for the sort by item feature
    ui->label_inventorysales->hide();
}

//copy constructor
MainWindow::MainWindow(const MainWindow &otherWindow)
    : QMainWindow(otherWindow.parent)
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

    sortItemModel = new QSqlTableModel;


    // setting default indices
    ui->stackedWidget_main->setCurrentIndex(HOME);

    ui->stackedWidget_sales->setCurrentIndex(SALES_DAILY);
    ui->stackedWidget_admin->setCurrentIndex(ADMIN_MEMBER);

    //ensures the user is logged out when the program starts
    setPermissions(NONE);
    ui->label_home_warning->setText("");

    //initializes pos page
    ui->comboBox_pos_qty->setEnabled(false);
    ui->comboBox_pos_itemlist->setEnabled(false);
    ui->pushButton_pos_purchase->setEnabled(false);

    //initializes sales page
    InitializeSalesTableView();
    ui->label_total_revenue->setVisible(false);
    ui->label_sales_searchmembererrormessage->setVisible(false);


    //initializes membership page
    ui->tableWidget_membership->hide();
    ui->gridWidget_membership_expire->hide();

    //initializes admin page
    ui->pushButton_admin_confirmdeletemember->setEnabled(false);
    ui->gridWidget_admin_memberdatafields->hide();
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->gridWidget_admin_itemdatafields->hide();
    ui->gridWidget_admin_confirmdeleteitem->hide();

    ui->pushButton_membership_upgrades->setEnabled(false);
    ui->pushButton_membership_downgrades->setEnabled(false);



    //preparing the line edit integer validations for add member
    //ui->lineEdit_admin_membersubmission_name->setValidator(new QRegExpValidator( QRegExp("[A-Za-z]{0,255}"), this ));
    idCheck = new QIntValidator;
    idCheck->setRange(0,100000);
    ui->lineEdit_admin_membersubmission_id->setValidator(idCheck);

    //sets up add member
    memberModel = nullptr;
    ui->label_admin_membersubmission_nameID_warning->hide();

    ui->pushButton_admin_editmember->hide();
    ui->pushButton_admin_deletemember->setEnabled(false);

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

    //hides the total revenue for the sort by item feature
    ui->label_inventorysales->hide();
}

// MainWindow Destructor
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

/*----Window Navigation----*/

// Navigates to home page and logs user out
void MainWindow::on_pushButton_home_clicked()
{
    //updates the dropdown menu in search by item
    MainWindow::on_pushButton_sales_searchitem_clicked();
    ui->stackedWidget_main->setCurrentIndex(HOME);
    setPermissions(PermissionLevel::NONE); //logs user out

    ui->pushButton_membership_upgrades->setEnabled(false);
    ui->pushButton_membership_downgrades->setEnabled(false);
}

// Navigates to POS page
void MainWindow::on_pushButton_POS_clicked()
{
    ui->stackedWidget_main->setCurrentIndex(POS);
    InitializePosTable();
}

// Navigates to sales page
void MainWindow::on_pushButton_sales_clicked()
{
    //updates the dropdown menu in search by item
    MainWindow::on_pushButton_sales_searchitem_clicked();
    ui->stackedWidget_main->setCurrentIndex(SALES);
    ui->radioButton_sales_byday_executive->setEnabled(false);
    ui->radioButton_sales_byday_regular->setEnabled(false);
    ui->radioButton_sales_byday_both->setEnabled(false);

    ui->gridWidget_sales_byday_labels->hide();
}

// Navigates to membership page
void MainWindow::on_pushButton_members_clicked()
{
    ui->stackedWidget_main->setCurrentIndex(MEMBER);
}

// Navigates to administrator page
void MainWindow::on_pushButton_admin_clicked()
{
    ui->stackedWidget_main->setCurrentIndex(ADMIN);
    ui->pushButton_membership_upgrades->setEnabled(true);
    ui->pushButton_membership_downgrades->setEnabled(true);
}

    /*----Sales Page push buttons----*/

// Navigates to daily sales tab
void MainWindow::on_pushButton_sales_daily_clicked()
{
    ui->stackedWidget_sales->setCurrentIndex(SALES_DAILY);
}

// Filter purchases based on date of purchases
void MainWindow::on_pushButton_sale_byday_clicked()
{
    double dailyRevenue = 0;
    int numberExecutive = 0;
    int numberRegular = 0;

    QSqlQuery nameQuery;
    // query to retrieve purchases sorted by date
    nameQuery.prepare("SELECT members.memberType AS status, COUNT(DISTINCT(members.name)) AS numMembers "
                      "FROM members, purchases, products WHERE members.memberID = purchases.memberID "
                      "AND products.productID = purchases.productID "
                      "AND purchases.datePurchased = ? "
                      "GROUP BY members.memberType");
    nameQuery.bindValue(0, ui->comboBox_sales_byday->currentText());
    // if successful set the query mode
    if (nameQuery.exec())
    {
        // Copy into int values
        while (nameQuery.next())
        {
            if (nameQuery.value("status").toString() == "Executive")
            {
                numberExecutive = nameQuery.value("numMembers").toInt();
            }
            else
            {
                numberRegular = nameQuery.value("numMembers").toInt();
            }
        }

        qDebug() << "Executives: " << numberExecutive;
        qDebug() << "Regular: " << numberRegular;
    }
    // otherwise print a warning message
    else
    {
        qDebug() << nameQuery.lastError().text();
    }


    QSqlQueryModel *dailySalesModel = new QSqlQueryModel; // Create new model
    QSqlQuery query; // Query to gather data
    // query to retrieve purchases sorted by date
    query.prepare("SELECT purchases.datePurchased, members.name, members.memberType, "
                                  "products.name, products.price, purchases.qty, products.price * purchases.qty * 1.0775 AS Total "
                                  "FROM purchases "
                                  "JOIN products ON (products.productID = purchases.productID) "
                                  "JOIN members ON (members.memberID = purchases.memberID) "
                                  "WHERE datePurchased = ?");

    // Configure combo box
    qDebug() << "combo box: " << ui->comboBox_sales_byday->currentText();
    query.bindValue(0, ui->comboBox_sales_byday->currentText());

    // If successful, set the query model
    if (query.exec())
    {

        dailySalesModel->setQuery(query);
    }
    // Else print error message
    else
    {
        qDebug() << query.lastError().text();
    }

    // Initialize tableView_sales_daily using querymodel
    ui->tableView_sales_daily->setModel(dailySalesModel);
    dailySalesModel->setHeaderData(DAILY_DATE, Qt::Horizontal, tr("Date"));
    dailySalesModel->setHeaderData(DAILY_NAME, Qt::Horizontal, tr("Member Name"));
    dailySalesModel->setHeaderData(DAILY_STATUS, Qt::Horizontal, tr("Member Status"));
    dailySalesModel->setHeaderData(DAILY_ITEM, Qt::Horizontal, tr("Item"));
    dailySalesModel->setHeaderData(DAILY_PRICE, Qt::Horizontal, tr("Price"));
    dailySalesModel->setHeaderData(DAILY_QTY, Qt::Horizontal, tr("Qty"));
    dailySalesModel->setHeaderData(DAILY_TOTAL, Qt::Horizontal, tr("Total"));

    // Resize tableview columns
    ui->tableView_sales_daily->resizeColumnToContents(DAILY_NAME);

    // Hide numerical vertical header
    ui->tableView_sales_daily->verticalHeader()->setVisible(false);

    // Make fields uneditable
    ui->tableView_sales_daily->setEditTriggers(QTableView::NoEditTriggers);

    ui->radioButton_sales_byday_executive->setEnabled(true);
    ui->radioButton_sales_byday_regular->setEnabled(true);
    ui->radioButton_sales_byday_both->setEnabled(true);
    ui->radioButton_sales_byday_both->click();

    // Add up daily revenue
    for (int index = 0; index < dailySalesModel->rowCount(); index++)
    {
       dailyRevenue += dailySalesModel->record(index).value("Total").toDouble();
    }

    ui->label_sales_byday_dailyrevenue->setText(QString("$").append(QString::number(dailyRevenue, 'f', 2)));
    ui->label_sales_byday_numberofexecutive->setText(QString::number(numberExecutive));
    ui->label_sales_byday_numberofregular->setText(QString::number(numberRegular));

    ui->gridWidget_sales_byday_labels->show();
}

void MainWindow::on_radioButton_sales_byday_executive_clicked() // filters daily sales report by executive members
{
    for (int i = 0; i < ui->tableView_sales_daily->model()->rowCount(); i++)
    {
        if (ui->tableView_sales_daily->model()->index(i, DAILY_STATUS).data().toString() == "Regular")
        {
            ui->tableView_sales_daily->setRowHidden(i, true);
        }
        else
            ui->tableView_sales_daily->setRowHidden(i, false);
    }
}

void MainWindow::on_radioButton_sales_byday_regular_clicked() // filters daily sales report by regular members
{
    for (int i = 0; i < ui->tableView_sales_daily->model()->rowCount(); i++)
    {
        if (ui->tableView_sales_daily->model()->index(i, DAILY_STATUS).data().toString() == "Executive")
        {
            ui->tableView_sales_daily->setRowHidden(i, true);
        }
        else
            ui->tableView_sales_daily->setRowHidden(i, false);
    }
}

void MainWindow::on_radioButton_sales_byday_both_clicked() // filters daily sales report by both member types
{
    for (int i = 0; i < ui->tableView_sales_daily->model()->rowCount(); i++)
    {
        ui->tableView_sales_daily->setRowHidden(i, false);
    }
}

//The button to print out the revenue of each member
void MainWindow::on_pushButton_sales_sortmember_clicked() // sales by member
{
    //Constants
    const int ID_COLUMN      = 0; //The column number for the member's id number
    const int NAME_COLUMN    = 1; //The column number for the member's name
    const int REVENUE_COLUMN = 2; //The column number for the member's revenue

    //Variables
    QSqlQuery       query;         //The query object use to exucute the query for tableData (easier to check for errors)
    QSqlQueryModel *tableData;     //A point to the query model storing the data for the table

    double totalRevenue = 0.00; //The total revenue across all members

    ui->stackedWidget_sales->setCurrentIndex(SALES_SORT_MEMBER);

    bool queryError = query.exec("SELECT members.memberID, members.name, sum(products.price * purchases.qty) * 1.0775 AS Revenue FROM members "
                                 "LEFT OUTER JOIN purchases ON purchases.memberID=members.memberID "
                                 "LEFT OUTER JOIN products ON purchases.productID=products.productID "
                                 "GROUP BY members.memberID "
                                 "ORDER BY members.memberID");

    // If error, output error message
    if (!queryError)
    {
        qDebug() << query.lastError().text();
        throw query.lastError();
    }
    else // Else, output success
    {
        qDebug() << "successful";
    }

    // Initialize and configure tablemodel
    tableData = new QSqlQueryModel();
    tableData->setQuery(query);
    tableData->setHeaderData(ID_COLUMN, Qt::Horizontal, tr("ID"));
    tableData->setHeaderData(NAME_COLUMN, Qt::Horizontal, tr("Name"));
    tableData->setHeaderData(REVENUE_COLUMN, Qt::Horizontal, tr("Revenue"));

    // Configure tableview
    ui->tableView_sales_sortmember->verticalHeader()->setVisible(false);
    ui->tableView_sales_sortmember->setModel(tableData);
    ui->tableView_sales_sortmember->setItemDelegateForColumn(REVENUE_COLUMN, formatPrice);
    ui->tableView_sales_sortmember->resizeColumnToContents(NAME_COLUMN);

    // Debug: Output rowcount
    qDebug() << tableData->rowCount();

    // Sum total revenue
    for (int index = 0; index < tableData->rowCount(); index++)
    {
        totalRevenue += tableData->record(index).value("Revenue").toDouble();
    }

    // List total revenue
    ui->label_total_revenue->setText(QString("Total Revenue: $").append(QString::number(totalRevenue, 'f', 2)));
    ui->label_total_revenue->setVisible(true);
}

// Displays report of sales sorted by item name
void MainWindow::on_pushButton_sales_sortitem_clicked()
{
    // Set index of sales reports
    ui->stackedWidget_sales->setCurrentIndex(SALES_SORT_ITEM);

    // Initialize model
    if (sortItemModel != nullptr)
    {
        delete sortItemModel;
    }


    QSqlQuery query; // Query to gather data

    // Execute Query
        // If unsuccessful, print error
    if(!(query.exec("SELECT products.productID, products.name,"
                    " sum(products.price * purchases.qty) * 1.0775 "
                    "FROM products LEFT OUTER JOIN purchases "
                    "ON products.productID = purchases.productID "
                    "GROUP BY products.productID "
                    "ORDER BY products.name")))
    {
        qDebug() << query.lastError().text();
    }

    // Configure model
    sortItemModel = new QSqlQueryModel;
    sortItemModel->setQuery(query);
    sortItemModel->sort(ITEM_PRICE, Qt::AscendingOrder);

    sortItemModel->setHeaderData(ITEM_ID, Qt::Horizontal, QVariant("ID"));
    sortItemModel->setHeaderData(ITEM_NAME, Qt::Horizontal, QVariant("Product Name"));
    sortItemModel->setHeaderData(ITEM_PRICE, Qt::Horizontal, QVariant("Revenue"));

    // Configure view
    ui->tableView_sales_sortitem->setModel(sortItemModel);
    ui->tableView_sales_sortitem->resizeColumnToContents(ITEM_NAME);
    ui->tableView_sales_sortitem->verticalHeader()->setVisible(false);
    ui->tableView_sales_sortitem->setItemDelegateForColumn(ITEM_PRICE, formatPrice);
    ui->tableView_sales_sortitem->setEditTriggers(QAbstractItemView::AnyKeyPressed);
    ui->tableView_sales_sortitem->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_sales_sortitem->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_sales_sortitem->setFocusPolicy(Qt::NoFocus);
    ui->tableView_sales_sortitem->setWordWrap(false);
    ui->tableView_sales_sortitem->setSortingEnabled(true);
    ui->tableView_sales_sortitem->sortByColumn(ITEM_NAME, Qt::AscendingOrder);
    ui->tableView_sales_sortitem->show();

    QString revenueMessage; // Message displayed to user
    revenueMessage = "Total Revenue: $";
    double totalRevenue = 0.0; // Total revenue to be displayed

    // Sum total revenue
    for (int index = 0; index < sortItemModel->rowCount(); index++)
    {
       totalRevenue = totalRevenue + sortItemModel->record(index).value(ITEM_PRICE).toDouble();
    }

    // Add total to 'total revenue' string
    revenueMessage = revenueMessage.append(QString::number(totalRevenue, 'f', 2));

    // Configure view
    ui->label_inventorysales->show();
    ui->label_inventorysales->setText(revenueMessage);
}

// Search for a specific member and print total revenue generated by member
void MainWindow::on_pushButton_sales_searchmember_clicked()
{
    //Constants
    const int ID   = 0; //The consant corresponding the id column from the query result
    const int NAME = 1; //The constant corresponding to the name column from the query result

    //Variables
    QSqlQuery   retrieveMemberData; //The query object use to retrieve the member's name and id
    QStringList memberIdName;       //A QStringList container the id and name of all the members
                                    //basically a member in the list has a QString for their id
                                    //then following QString is their name

    // Navigate to page
    ui->stackedWidget_sales->setCurrentIndex(SALES_SEARCH_MEMBER);

    // Define error parameters
    bool retrieveDataError = retrieveMemberData.exec("SELECT memberId, name FROM members");

    // If query unsuccessful, print error and throw exception
    if (!retrieveDataError)
    {
        qDebug() << retrieveMemberData.lastError().text();
        throw retrieveMemberData.lastError();
    }

    // Populate member data string list
    while (retrieveMemberData.next())
    {
        memberIdName << retrieveMemberData.value(ID).toString();
        memberIdName << retrieveMemberData.value(NAME).toString();
    }

    // Populate completer
    TextCompleter(memberIdName, ui->lineEdit_sales_searchmember);
}

// Search by product name and display revenue generated by product
void MainWindow::on_pushButton_sales_searchitem_clicked()
{
    // Move to proper page
    ui->stackedWidget_sales->setCurrentIndex(SALES_SEARCH_ITEM);
    productList.clear();
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

// Search database for requested member from lineEdit_sales_searchmember and print to tableView
void MainWindow::on_pushButton_sales_searchmemberconfirm_clicked()
{
    //Constant
    const int ID_COLUMN     = 0; //The column number for the member's id number
    const int NAME_COLUMN   = 1; //The column number for the member's name
    const int REVENUNE_COLUMN = 2; //The column number for the member's revenue

    //Variables
    QString        memberFound;   //The QString store member that is found, it
                                  //eiter stores the member's name or id
    QSqlQuery      retrieveData;  //The query object that retrieves the data from
                                  //the database
    QSqlQueryModel *tableData;    //The model that store the data to be displayed in
                                  //the table view
    QString         comboBoxText; //A QString to do intermediates steps on the text
                                  //for the many members found comboBox

    // Initialize error message
    ui->label_sales_searchmembererrormessage->setVisible(false);

    memberFound = ui->lineEdit_sales_searchmember->text();

    //Prep query
    retrieveData.prepare("SELECT members.memberID, members.name, sum(products.price * purchases.qty) * 1.0775 AS revenue FROM members "
                         "LEFT OUTER JOIN purchases ON purchases.memberID=members.memberID "
                         "LEFT OUTER JOIN products ON purchases.productID=products.productID "
                         "WHERE members.memberID=? OR members.name=?");

    // Bind safe values
    retrieveData.bindValue(0, memberFound);
    retrieveData.bindValue(1, memberFound);

    // Set error parameters
    bool queryError = retrieveData.exec();

    // If query is unsuccessful, print error and throw exception
    if (!queryError)
    {
        qDebug() << retrieveData.lastError().text();

        throw retrieveData.lastError();
    }

    // Initialize and configure model
    tableData = new QSqlQueryModel;
    tableData->setQuery(retrieveData);
    tableData->setHeaderData(ID_COLUMN, Qt::Horizontal, QVariant("Member ID"));
    tableData->setHeaderData(NAME_COLUMN, Qt::Horizontal, QVariant("Member Name"));
    tableData->setHeaderData(REVENUNE_COLUMN, Qt::Horizontal, QVariant("Revenue"));

    qDebug() << "check";

    // Configure view
    ui->tableView_sales_searchmember->setModel(tableData);
    ui->tableView_sales_searchmember->verticalHeader()->setVisible(false);

    if (tableData->record(0).value("memberId").isNull()) //no members found
    {
        ui->label_sales_searchmembererrormessage->setText("Error: That member was not found");
        ui->label_sales_searchmembererrormessage->setVisible(true);

        // Clear table view
        tableData->clear();
    }
    else //exactly one member found
    {
        ui->tableView_sales_searchmember->setItemDelegateForColumn(2, formatPrice);
    }
}

// Button to confirm that this item should be searched for
void MainWindow::on_pushButton_sales_searchitemconfirm_clicked()
{
    //Constant
    const int TOTAL_REVENUE_INDEX = 2; //The column index for total revenue

    // Pull desired item to be placed into query
        salesReportProduct = ui->lineEdit_sales_searchitem->text();

        // Output item to terminal for testing purposes
        qDebug() << salesReportProduct;

        // Declare and intialize query and query model to pull relevant information
        QSqlQueryModel *model = new QSqlQueryModel;
        QSqlQuery query;




        // Prep extremely complex query
        query.prepare("select products.name, sum(purchases.qty), sum(purchases.qty) * products.price * 1.0775 "
                      "from products, purchases "
                      "where products.name=:name "
                      "and products.productID = purchases.productID;");

        // Bind variable to query
        query.bindValue(":name", salesReportProduct);
        qDebug() <<"query test 2:"<< query.isNull("name");
        qDebug() <<"query test 1:"<< query.value(1).toInt();
        if(query.exec())
        {
        qDebug() <<"query test integer:"<< query.value(1).toInt();

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
        query.first();
        if(query.value(0).toString() == "")
        {


            query.prepare("SELECT DISTINCT products.name, 0, 0 FROM products, purchases "
                           "WHERE products.name=? ;"
                          );


            // Bind variable to query
            query.bindValue(0, salesReportProduct);

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

        }
        // Hide row numbers
        ui->tableView_sales_searchitem->verticalHeader()->setVisible(false);
        // Configure tableView with model
        ui->tableView_sales_searchitem->setModel(model);
        //Format total revenue to a money amount
        ui->tableView_sales_searchitem->setItemDelegateForColumn(TOTAL_REVENUE_INDEX, formatPrice);

}

    /*----Admin Page push buttons----*/

// Navigate to "Manage Membership" administrator interface
void MainWindow::on_pushButton_admin_member_clicked()
{
    // Initialize gridwidget
    ui->stackedWidget_admin->setCurrentIndex(ADMIN_MEMBER);
    ui->pushButton_admin_editmember->setEnabled(false);
    ui->pushButton_admin_deletemember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->gridWidget_admin_memberdatafields->hide();
    ui->gridWidget_admin_confirmdeletemember->hide();

    if(memberModel != nullptr)
    {
        delete memberModel;
    }

    // Initialize model
    memberModel = new QSqlTableModel;
    memberModel->setTable("members");
    memberModel->setSort(name, Qt::AscendingOrder);

    // Enum to configure tablewidget columns
    enum MembershipTableWidgetColumns
    {
        ID_COLUMN,
        NAME_COLUMN,
        MEMBERSHIP_TYPE_COLUMN,
        EXPIRATION_DATE_COLUMN,
        RENEWAL_PRICE_COLUMN
    };

    // Configure model
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

    //Configure view
    ui->tableView_admin_members->setModel(memberModel);
    ui->tableView_admin_members->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_admin_members->setItemDelegateForColumn(RENEWAL_PRICE_COLUMN, formatPrice);
    ui->tableView_admin_members->resizeColumnToContents(NAME_COLUMN);
    ui->tableView_admin_members->resizeColumnToContents(MEMBERSHIP_TYPE_COLUMN);
    ui->tableView_admin_members->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_admin_members->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_admin_members->setFocusPolicy(Qt::NoFocus);
    ui->tableView_admin_members->setWordWrap(false);
}


//Button to navigate to administrator "Manage Items" interface
void MainWindow::on_pushButton_admin_inventory_clicked()
{
    //Constant
    const int PRODUCT_ID_COLUMN    = 0; //The column for the product id
    const int PRODUCT_NAME_COLUMN  = 1; //The column for the product name
    const int PRODUCT_PRICE_COLUMN = 2; //The column for the procdut price

    // Reset "Add Member" Page
    ui->gridWidget_admin_memberdatafields->hide();
    ui->pushButton_admin_deletemember->setEnabled(false);
    ui->gridWidget_admin_confirmdeletemember->hide();

    //Hide error message
    ui->label_admin_products_errormessage->setVisible(false);

    // Navigate to page
    ui->stackedWidget_admin->setCurrentIndex(ADMIN_ITEM);
    ui->gridWidget_admin_itemdatafields->setVisible(false);

    // Initialize model
    if (itemModel != nullptr)
    {
        delete itemModel;
    }
    itemModel = new QSqlTableModel;

    // Configure model
    itemModel->setTable("products");
    itemModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    itemModel->setSort(PRODUCT_NAME_COLUMN, Qt::AscendingOrder);
    itemModel->setHeaderData(PRODUCT_ID_COLUMN, Qt::Horizontal, QVariant("Product ID"));
    itemModel->setHeaderData(PRODUCT_NAME_COLUMN, Qt::Horizontal, QVariant("Product Name"));
    itemModel->setHeaderData(PRODUCT_PRICE_COLUMN, Qt::Horizontal, QVariant("Price"));
    itemModel->select();

    // Configure view
    ui->tableView_admin_inventory->setModel(itemModel);
    ui->tableView_admin_inventory->setItemDelegateForColumn(PRODUCT_PRICE_COLUMN, formatPrice);
    ui->tableView_admin_inventory->resizeColumnToContents(PRODUCT_NAME_COLUMN);
    ui->tableView_admin_inventory->setEditTriggers(QAbstractItemView::AnyKeyPressed);
    ui->tableView_admin_inventory->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_admin_inventory->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_admin_inventory->setFocusPolicy(Qt::NoFocus);
    ui->tableView_admin_inventory->setWordWrap(false);

    // Connect dataChanged and currentChanged
    QObject::connect(itemModel, &QSqlTableModel::dataChanged, this, &MainWindow::on_tableModel_dataChanged);

    // Disable edit and delete functionality since no item is selected
    ui->pushButton_admin_edititem->setEnabled(false);
    ui->pushButton_admin_deleteitem->setEnabled(false);
}

// Navigate to "Add Member" interface
void MainWindow::on_pushButton_admin_addmember_clicked()
{
    MainWindow::on_pushButton_admin_member_clicked();
    ui->gridWidget_admin_memberdatafields->show();
    ui->pushButton_admin_editmember->setEnabled(false);
    ui->pushButton_admin_deletemember->setEnabled(false);

    // Set maximum character value
    ui->lineEdit_admin_membersubmission_name->setMaxLength(50);
}

// Navigate to "Edit Member" interface
void MainWindow::on_pushButton_admin_editmember_clicked()
{
    ui->gridWidget_admin_memberdatafields->show();
    ui->pushButton_admin_deletemember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(false);
}

// Nagivate to "Delete Member" interface
void MainWindow::on_pushButton_admin_deletemember_clicked()
{
    ui->gridWidget_admin_confirmdeletemember->show();
    ui->pushButton_admin_confirmdeletemember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(false);

    // Obtain index of currently selected row on tableview
    deleteMemberIndex = ui->tableView_admin_members->currentIndex();
    deleteMemberIndex = deleteMemberIndex.sibling(deleteMemberIndex.row(), memberModel->fieldIndex("name"));

    // Set "Delete" button data
    QString confirmDelete;
    confirmDelete = "Delete ";
    confirmDelete.append(deleteMemberIndex.data().toString());
    confirmDelete.append('?');
    ui->label_admin_confirmdeletemember->setText(confirmDelete);
}

// Confirm "Add Member" or "Edit Member" user selection
void MainWindow::on_pushButton_admin_membersubmission_submit_clicked()
{

    ui->pushButton_admin_deletemember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);

    // Hide warnings
    ui->label_admin_membersubmission_nameID_warning->hide();

    // Query to gather data
    QSqlQuery query;

    // Strings used to assign a date to the new member
    QString thisYear = QDate::currentDate().toString("yyyy");
    QString thisMonth = QDate::currentDate().toString("M");
    QString thisDay = QDate::currentDate().toString("dd");

    // Normalize leap year data
    if(QDate::isLeapYear(thisYear.toInt()) && QDate::currentDate().toString("M/dd") == "2/29")
        thisDay = QString::number(thisDay.toInt() - 1);
    QString nextYear = QString::number(thisYear.toInt() + 1);

    TempMember tempMemberAdd;

    // Populate tempMember struct instance with new member information.
    tempMemberAdd.id = ui->lineEdit_admin_membersubmission_id->text();

    // Formats name to ensure first letter is always capitalized, and the rest are lower case
    tempMemberAdd.name = normalizeCapitalization(ui->lineEdit_admin_membersubmission_name->text());

    // Sets executive status based off of the member based off of the radio button
    qDebug() << tempMemberAdd.name;
    if(ui->radioButton_admin_member->isChecked())
        tempMemberAdd.executiveStatus = "Executive";
    else
        tempMemberAdd.executiveStatus = "Regular";

    // Set expiration date of the member based on current date
    tempMemberAdd.expMonth = thisMonth;
    tempMemberAdd.expDay = thisDay;
    tempMemberAdd.expYear = nextYear;

    // Creates expiration date string
    tempMemberAdd.expDate = tempMemberAdd.expMonth + "/"
                          + tempMemberAdd.expDay   + "/"
                          + tempMemberAdd.expYear;

    // Set renewal price for member based on executive status
    int renewalPrice = 0;
    if(tempMemberAdd.executiveStatus == "Executive")
        renewalPrice = 120;
    else if(tempMemberAdd.executiveStatus == "Regular")
        renewalPrice = 60;

    // Ensure that the id and name fields contain at least one character.
    // If either of them dont, print warning message with instructions
    if(tempMemberAdd.name != "")
    {

        if(tempMemberAdd.id != "")
        {
            // Inserts member into database //

            // Prepare query
            query.prepare("INSERT INTO members "
                          "(memberID, name, "
                          "memberType, expireDate,"
                          "renewalPrice) VALUES(?,?,?,?,?)");

            // Prepare safe values
            query.addBindValue(tempMemberAdd.id);
            query.addBindValue(tempMemberAdd.name);
            query.addBindValue(tempMemberAdd.executiveStatus);
            query.addBindValue(tempMemberAdd.expDate);
            query.addBindValue(QString::number(renewalPrice));

            // If query fails, print error message
            if(!query.exec())
            {
                qDebug() << "Member failed to save" << memberModel->lastError();
                //sets the correct warning messages
                ui->label_admin_membersubmission_nameID_warning->setText(
                            "WARNING: Please enter a unique name and ID");
                ui->label_admin_membersubmission_nameID_warning->show();

                qDebug() << query.lastError().text();
                qDebug() << "name == __ | ID != __  ";
            }
            else
            {
                // Clear member fields, hide add member widgets, and warnings
                MainWindow::on_pushButton_admin_member_clicked();
                MainWindow::ClearMemberFields();

                // Hide warning message, and member text fields
                ui->gridWidget_admin_memberdatafields->hide();
                ui->label_admin_membersubmission_nameID_warning->hide();
            }
        }
        else
        {
            // Set correct warning messages
            ui->label_admin_membersubmission_nameID_warning->setText(
                        "WARNING: Please enter a unique name and ID");
            ui->label_admin_membersubmission_nameID_warning->show();

            qDebug() << query.lastError().text();
            qDebug() << "name != __ | ID == __  ";
        }
    }
    else
    {
        // Set correct warning messages
        ui->label_admin_membersubmission_nameID_warning->setText(
                    "WARNING: Please enter a unique name and ID");
        ui->label_admin_membersubmission_nameID_warning->show();

        qDebug() << query.lastError().text();
        qDebug() << "name == __ | ID != __  ";
    }
    MainWindow::on_pushButton_sales_searchitem_clicked();
}

// Cancel decision to add or edit member
void MainWindow::on_pushButton_admin_membersubmission_cancel_clicked()
{
    ui->gridWidget_admin_memberdatafields->hide();
    ui->pushButton_admin_deletemember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);

    ui->label_admin_membersubmission_nameID_warning->hide();

    // Clear member information line edits
    MainWindow::ClearMemberFields();
}

// Confirm decision to delete member
void MainWindow::on_pushButton_admin_confirmdeletemember_clicked()
{
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->pushButton_admin_deletemember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);

    // Removes row at currently selected index based on member id.
    deleteMemberIndex = deleteMemberIndex.sibling(deleteMemberIndex.row(), memberModel->fieldIndex("memberID"));
    if(memberModel->removeRow(deleteMemberIndex.row()))
    {
        qDebug() << "The member failed to delete. " << memberModel->lastError().text();
    }
    if(!(memberModel->submitAll()))
    {
        qDebug() << "\nFailed to submit the deleteion request\n";
    }
    MainWindow::on_pushButton_sales_searchitem_clicked();
}

// Cancel decision to delete member
void MainWindow::on_pushButton_admin_canceldeletemember_clicked()
{
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->pushButton_admin_deletemember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);
}

// Determines action connected to double-clicking 'admin member table'
void MainWindow::on_tableView_admin_members_doubleClicked(const QModelIndex &index)
{
    ui->pushButton_admin_confirmdeletemember->setEnabled(true);
}

    /*----Inventory Page----*/

// Navigate to "Add Item" interface
void MainWindow::on_pushButton_admin_additem_clicked()
{
    if (!itemModel->isDirty())
    {
        // Initialize view
        ui->gridWidget_admin_itemdatafields->show();
        ui->pushButton_admin_deleteitem->setEnabled(false);
        ui->pushButton_admin_edititem->setEnabled(false);
        ui->pushButton_admin_itemsubmission_submit->setVisible(true);
        ui->pushButton_admin_itemsubmission_cancel->setVisible(true);

        // Initialize line edit
        ui->lineEdit_admin_itemsubmission_id->setReadOnly(false);
        ui->lineEdit_admin_itemsubmission_name->setReadOnly(false);
        ui->lineEdit_admin_itemsubmission_price->setReadOnly(false);
        ui->lineEdit_admin_itemsubmission_id->setText(QString());
        ui->lineEdit_admin_itemsubmission_name->setText(QString());
        ui->lineEdit_admin_itemsubmission_price->setText(QString());

        // Configure model
        itemModel->insertRows(itemModel->rowCount(), /*Count: */ 1);

        // Selecting an abitary column from the row just inserted
        ui->tableView_admin_inventory->setCurrentIndex(itemModel->index(itemModel->rowCount() - 1, 0));
    }//end if (!itemModel->isDirty())
}

// Navigate to "Edit Item" interface
void MainWindow::on_pushButton_admin_edititem_clicked()
{
    // Initialize view
    ui->gridWidget_admin_itemdatafields->show();
    ui->pushButton_admin_deleteitem->setEnabled(false);
    ui->pushButton_admin_additem->setEnabled(false);
    ui->pushButton_admin_itemsubmission_submit->setVisible(true);
    ui->pushButton_admin_itemsubmission_cancel->setVisible(true);

    // Initialize line edit for input
    ui->lineEdit_admin_itemsubmission_id->setReadOnly(false);
    ui->lineEdit_admin_itemsubmission_name->setReadOnly(false);
    ui->lineEdit_admin_itemsubmission_price->setReadOnly(false);
}

// Navigate to "Delete Item" interface
void MainWindow::on_pushButton_admin_deleteitem_clicked()
{
    //Variables
    QString     confirmDeleteMessage; //The message display to confirm the deletion
    QModelIndex deleteProduct;        //The index of the product being deleted

    if (!itemModel->isDirty())
    {
        // Initialize View
        currentProcessIndex = ui->tableView_admin_inventory->currentIndex();
        ui->gridWidget_admin_confirmdeleteitem->show();
        ui->pushButton_admin_edititem->setEnabled(false);
        ui->pushButton_admin_additem->setEnabled(false);

        deleteProduct = ui->tableView_admin_inventory->currentIndex();
        deleteProduct = deleteProduct.sibling(deleteProduct.row(), itemModel->fieldIndex("name"));

        // Populate pushbutton text
        confirmDeleteMessage = "Delete ";
        confirmDeleteMessage.append(deleteProduct.data().toString());
        confirmDeleteMessage.append('?');
        ui->label_admin_confirmdeleteitem->setText(confirmDeleteMessage);

        // Set error parameters
        bool deleteError = itemModel->removeRow(deleteProduct.row());

        if (!deleteError)
        {
            qDebug() << itemModel->lastError().text();
        }
    }//end if (!item->itemModel->isDirty())
}

// Submit decision to add or edit item in database
void MainWindow::on_pushButton_admin_itemsubmission_submit_clicked()
{
    //Constants
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

    // Validate product id
    input = input.model()->index(input.row(), PRODUCT_ID_COLUMN);

    // If product ID entered is less than zero, output error
    if (input.data().toInt() < 0)
    {
        errorMessage.append("Please enter a positive value for the id \n");

        inputIsValid = false;
    }

    // Validate product price
    input = input.model()->index(input.row(), PRODUCT_PRICE_COLUMN);

    // If product ID entered is less than or equal to zero, output error
    if (input.data().toDouble() <= 0.0)
    {
        errorMessage.append("Please enter a positive value for the price \n");

        inputIsValid = false;
    }

    // Validate product name
    input = input.model()->index(input.row(), PRODUCT_NAME_COLUMN);

    // If no name, output error
    if (input.data().toString() == QString())
    {
        errorMessage.append(("Please enter a product name \n"));

        inputIsValid = false;
    }

    // If all inputs valid, add item to database
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

            //Hide error message
            ui->label_admin_products_errormessage->setVisible(false);
        }
    }
    else
    {
        ui->label_admin_products_errormessage->setText(errorMessage);
        ui->label_admin_products_errormessage->setVisible(true);
    }
    //updates the drop down in search by item
    MainWindow::on_pushButton_sales_searchitem_clicked();
}

// Cancel decision to add or edit item to/in database
void MainWindow::on_pushButton_admin_itemsubmission_cancel_clicked()
{
    // Initialize view
    ui->gridWidget_admin_itemdatafields->hide();
    ui->pushButton_admin_additem->setEnabled(true);
    ui->pushButton_admin_edititem->setEnabled(false);
    ui->tableView_admin_inventory->clearSelection();

    // Initialize line edits
    ui->lineEdit_admin_itemsubmission_id->setText(QString());
    ui->lineEdit_admin_itemsubmission_name->setText(QString());
    ui->lineEdit_admin_itemsubmission_price->setText(QString());

    // Remove unsubmitted row
    itemModel->revertAll();

    //Hide error message
    ui->label_admin_products_errormessage->setVisible(false);
}

// Confirm deletion of item from database
void MainWindow::on_pushButton_admin_confirmdeleteitem_clicked()
{
    // Initialize view
    ui->gridWidget_admin_confirmdeleteitem->hide();
    ui->gridWidget_admin_itemdatafields->setVisible(false);
    ui->pushButton_admin_deleteitem->setEnabled(false); //no item will be selected
    ui->pushButton_admin_additem->setEnabled(true);

    // Hide error message
    ui->label_admin_products_errormessage->setVisible(false);

    // Configure model
    itemModel->submitAll();

    //updates the drop down in search by item
    MainWindow::on_pushButton_sales_searchitem_clicked();
}

// Cancel request to delete item
void MainWindow::on_pushButton_admin_canceldeleteitem_clicked()
{
    // Initialize View
    ui->gridWidget_admin_confirmdeleteitem->hide();
    ui->gridWidget_admin_itemdatafields->setVisible(false);
    ui->pushButton_admin_deleteitem->setEnabled(false); //no item will be selected
    ui->pushButton_admin_additem->setEnabled(true);
    ui->label_admin_products_errormessage->setVisible(false);

    // Configure model
    itemModel->revertAll();
    itemModel->select();
}

/*----Membership Page push buttons----*/

// Navigate to "Membership Rebates" feature interface
void MainWindow::on_pushButton_membership_rebates_clicked()
{
    // Initialize view
    ui->gridWidget_membership_expire->hide();
    ui->tableWidget_membership->hide();

    ui->label_membershippage->setText("Rebates Page");

    //clears the information from other vertical stackwidget tabs
    ui->tableWidget_membership->setRowCount(0);
    upgradeCount = 0; // Number of upgrades recommended
    downgradeCount = 0; // Number of downgrades recommended
    ExecutiveMemberRebate tempMember; // Temporary object used for copying
    QStringList memberIDList = database->GetExecutiveMemberIDList(); // Get list of member ID's
    QSqlQuery query; // Query used to gather data
    QVector<ExecutiveMemberRebate> memberList; // Vector of member purchases

    // Prepare query
    query.prepare("SELECT members.memberID, members.name, "
                  "sum(purchases.qty * products.price) "
                  "FROM members, purchases, products "
                  "WHERE members.memberID = purchases.memberID "
                  "AND purchases.productID = products.productID "
                  "AND members.memberID = :memberID");

    // Iterate through ID list, calling each member's purchases
    for(int i = 0; i < memberIDList.size(); i++)
    {
        // Bind safe values
        query.bindValue(":memberID", memberIDList[i]);

        // Execute Query
        if(query.exec())
        {
            // Iterate through query data and pull purchase information into vector
            while(query.next())
            {
                // If query results are empty
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

    // Get members who made no purchases
    query.prepare("SELECT DISTINCT members.memberID, members.name, 0 , 0 "
                  "FROM members, purchases "
                  "WHERE members.memberType = 'Executive' "
                  "AND members.memberID NOT IN "
                  "(SELECT memberID from purchases)" );

    // Execute query
    if(query.exec())
    {
        while(query.next())
        {
            // If query results in blank data
            if(query.value(0).toString() != "")
            {
                // Copy into temp object
                tempMember.memberID = query.value(0).toString();
                tempMember.name = query.value(1).toString();
                tempMember.amountSpent = 0.f;
                tempMember.rebate = 0.f;

                // Add object to vector
                memberList.append(tempMember);
            }
        }
    }
    else // if unsuccessful, print error
    {
        qDebug() << query.lastError().text();
    }

    // Print list of executive members
    for(int i = 0; i < memberList.size(); i++)
    {
        qDebug() << memberList[i].name << " " << memberList[i].memberID << " "
                 << memberList[i].amountSpent << " " << memberList[i].rebate << "\n";
    }

    QStringList tableColumns; // Liast of column names
    ui->tableWidget_membership->setColumnCount(3); // Configure row count
    tableColumns << "ID" << "Name" << "Rebate"; // Assign column names

    // Configure view
    ui->tableWidget_membership->setHorizontalHeaderLabels(tableColumns);

    // Populate tableWidget_membership
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

    // Display view
    ui->tableWidget_membership->show();

    // Output number of rows in tableWidget_membership
    qDebug() << ui->tableWidget_membership->rowCount();

    // Sum executive member's rebates.
    float totalAllRebates;
    for(int i = 0; i < memberList.size(); i++)
    {
        totalAllRebates += memberList[i].rebate.toFloat();
    }
  
    // Print rebates
    QString labelText = "Total of all rebates: $" + QString::number(totalAllRebates, 'f', 2);
    ui->label_membership_recommendation_status->setText(labelText);


    // Reset table
    on_pushButton_admin_member_clicked();

}

// Determine which memberships are expiring based on month requested
void MainWindow::on_pushButton_membership_expiration_clicked()
{
    ui->label_membershippage->setText("Expiration Page");
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

    // Configure view
    ui->tableView_membership->setModel(membershipModel);
    ui->tableView_membership->resizeColumnsToContents();
    ui->tableView_membership->verticalHeader()->setVisible(false);
    ui->tableView_membership->setEditTriggers(QTableView::NoEditTriggers);

    // Show it
    ui->gridWidget_membership_expire->show();
}

// Confirm entry of expiration month selected and show results
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
     ui->gridWidget_membership_expire->show();
}

// Navigate to membership upgrade recommendations
void MainWindow::on_pushButton_membership_upgrades_clicked()
{
    ui->label_membershippage->setText("Recommended Upgrades Page");
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

// Navigate to membership downgrade recommendations
void MainWindow::on_pushButton_membership_downgrades_clicked() // member downgrades list
{
    ui->label_membershippage->setText("Recommended Downgrades Page");
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

// Set member in POS
void MainWindow::on_comboBox_pos_memberlist_activated(int index)
{
    posMemberID = ui->comboBox_pos_memberlist->currentText().toInt();
    ui->comboBox_pos_itemlist->setEnabled(true); // enables the item selection
}

// Clear receipt page in POS if other member is selected
void MainWindow::on_comboBox_pos_memberlist_currentIndexChanged(int index)
{
    ui->tableWidget_pos_receipts->clear();

    QStringList TableHeader;
    ui->tableWidget_pos_receipts->setColumnCount(5);
    TableHeader << "Member ID" << "Item" << "Price" << "qty" << "Total";
    ui->tableWidget_pos_receipts->setHorizontalHeaderLabels(TableHeader);
    ui->tableWidget_pos_receipts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_pos_receipts->setShowGrid(false);
    ui->tableWidget_pos_receipts->verticalHeader()->setVisible(false);
    ui->tableWidget_pos_receipts->setColumnWidth(1, 300);
    receiptRow = 0;
}

// Set item in POS
void MainWindow::on_comboBox_pos_itemlist_activated(int index)
{
    posItemName = ui->comboBox_pos_itemlist->currentText();
    posItem = this->database->GetItem(posItemName);
    ui->comboBox_pos_qty->setEnabled(true); // enables the quantity selection
}

// Set quantity of product in POS
void MainWindow::on_comboBox_pos_qty_activated(int index)
{
    posQty = index + 1;
    posPrice = this->database->GetPrice(posItemName);
    posTotal = posPrice * posQty * 1.0775; // calculates total
    ui->label_pos_price->setText(QString::number(posTotal, 'f', 2));
    ui->pushButton_pos_purchase->setEnabled(true); // enables the purchase button
}

// Confirm purchase
void MainWindow::on_pushButton_pos_purchase_clicked()
{
    // Print receipt
    printReceipt();

    // Add purchase data to database
    this->database->AddPurchase(posMemberID, posItem, QDate::currentDate().toString("M/dd/yyyy"), posQty);

    // Reset purchase data fields
    ui->comboBox_pos_itemlist->setCurrentIndex(0);
    ui->comboBox_pos_qty->setCurrentIndex(0);
    ui->comboBox_pos_qty->setEnabled(false);
    ui->label_pos_price->setText("");
    ui->pushButton_pos_purchase->setEnabled(false);
    ui->comboBox_pos_itemlist->setEnabled(false);

    // Update daily sales options with new purchase
    InitializeSalesTableView();
}

// Autocomplete for 'Search by Member/Item' features
void MainWindow::TextCompleter(QStringList products, QLineEdit *inputField)
{
    QCompleter *completer = new QCompleter(products, inputField);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    inputField->setCompleter(completer);
}

// Clears member fields in admin "Member" interface
void MainWindow::ClearMemberFields()
{
    ui->lineEdit_admin_membersubmission_id->clear();
    ui->lineEdit_admin_membersubmission_name->clear();
    ui->radioButton_admin_member->setChecked(false);
}

/*----Home Page push buttons----*/

// Attempt user login
void MainWindow::on_pushButton_home_login_clicked()
{
    // Initialize text
    ui->label_home_warning->setText("");

    // Insert text
    QString username = ui->lineEdit_home_username->text();
    QString password = ui->lineEdit_home_password->text();

    // Check database for matching credentials
    int permissionIndex = this->database->CheckLogin(username, password);

    // If credentials match, set permission to matching level
    if (permissionIndex > NONE)
    {
        setPermissions(permissionIndex);
    }
    else // if not, display warning message
    {
        ui->label_home_warning->setText("Invalid username or password. Please try again.");
        ui->label_home_warning->setStyleSheet("color: red");
    }

    // Clear fields
    ui->lineEdit_home_username->clear();
    ui->lineEdit_home_password->clear();
}


    // Helper Function Definitions //

// Use login credentials to control the user's access to program features
void MainWindow::setPermissions(int permission)
{
    switch(permission)
    {
    case PermissionLevel::NONE:                 // Login failed
        ui->pushButton_POS->setEnabled(false);
        ui->pushButton_sales->setEnabled(false);
        ui->pushButton_members->setEnabled(false);
        ui->pushButton_admin->setEnabled(false);
        ui->stackedWidget_main->setCurrentIndex(HOME);
        break;
    case PermissionLevel::EMPLOYEE:             // Employee login enables the POS page
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(false);
        ui->pushButton_members->setEnabled(false);
        ui->pushButton_admin->setEnabled(false);
        on_pushButton_POS_clicked();
        break;
    case PermissionLevel::MANAGER:              // Manager login enables POS, sales, and membership pages
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(true);
        ui->pushButton_members->setEnabled(true);
        ui->pushButton_admin->setEnabled(false);
        on_pushButton_sales_clicked();
        break;
    case PermissionLevel::ADMINISTRATOR:        // Administrator login enables all pages
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(true);
        ui->pushButton_members->setEnabled(true);
        ui->pushButton_admin->setEnabled(true);
        on_pushButton_admin_clicked();
        break;
    }
}

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
    ui->tableWidget_membership->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_membership->resizeColumnsToContents();
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

// Fill the daily sales combo box and intialize view
void MainWindow::InitializeSalesTableView()
{
    // Clear data
    ui->comboBox_sales_byday->clear();
    QSqlQuery query; // Query to retrieve data

    // Prep query
    query.prepare("select datePurchased from purchases group by datePurchased");

    // If successful, fill the sales by day combo box with retrieved items
    if(query.exec())
    {
       while (query.next())
       {
           ui->comboBox_sales_byday->addItem(query.value("datePurchased").toString());
       }
    }
    // If not successful, print an error message
    else
    {
        qDebug() << query.lastError().text();
    }

    // Hide numerical vertical header
    ui->tableView_sales_daily->verticalHeader()->setVisible(false);
    // Make fields uneditable
    ui->tableView_sales_daily->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_sales_daily->setItemDelegateForColumn(DAILY_TOTAL, formatPrice);
    ui->tableView_sales_daily->setItemDelegateForColumn(DAILY_PRICE, formatPrice);
}

// Fill POS page combo boxes and initialize receipts table
void MainWindow::InitializePosTable()
{
    QStringList TableHeader; // List of header names
    ui->tableWidget_pos_receipts->setColumnCount(5); // Set column count
    TableHeader << "Member ID" << "Item" << "Price" << "qty" << "Total"; // Populate header name list

    // Initialize view
    ui->tableWidget_pos_receipts->setHorizontalHeaderLabels(TableHeader);
    ui->tableWidget_pos_receipts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_pos_receipts->setShowGrid(false);
    ui->tableWidget_pos_receipts->verticalHeader()->setVisible(false);
    ui->tableWidget_pos_receipts->setColumnWidth(1, 300);

    // Initialize row number
    receiptRow = 0;

    // Clearing combobox before repopulating
    ui->comboBox_pos_memberlist->clear();
    ui->comboBox_pos_itemlist->clear();

    // Initialize member ID combo box
    QStringList members = this->database->GetPOSMembers();

    // Populate item number combo box
    if(ui->comboBox_pos_memberlist->count() == 0)
    {
        for (int i = 0; i < members.length(); i++)
        {
            ui->comboBox_pos_memberlist->addItem(members.at(i));
        }
    }

    //initializing items combo box
    QStringList items = this->database->GetNames();

    //item number combo box
    if(ui->comboBox_pos_itemlist->count() == 0)
    {
        for (int i = 0; i < items.length(); i++)
        {
            ui->comboBox_pos_itemlist->addItem(items.at(i));
        }
    }

    // Initialize quantity in combo box
    if(ui->comboBox_pos_qty->count() == 0)
    {
        for (int i = 1; i <= 10; i++)
        {
            ui->comboBox_pos_qty->addItem(QString::number(i));
        }
    }
}

// Prints receipt table in POS page
void MainWindow::printReceipt()
{
    // Create all objects
    QTableWidgetItem *member = new QTableWidgetItem;
    QTableWidgetItem *item = new QTableWidgetItem;
    QTableWidgetItem *price = new QTableWidgetItem;
    QTableWidgetItem *qty = new QTableWidgetItem;
    QTableWidgetItem *total = new QTableWidgetItem;

    // Populate objects
    member->setText(QString::number(posMemberID));
    item->setText(posItemName);
    price->setText(QString::number(posPrice));
    qty->setText(QString::number(posQty));
    total->setText(QString::number(posTotal, 'f', 2));

    // Populate receipt
    ui->tableWidget_pos_receipts->insertRow(receiptRow);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 0, member);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 1, item);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 2, price);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 3, qty);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 4, total);
    receiptRow++;
}

// Determine actions taken when admin tableview is activated
void MainWindow::on_tableView_admin_members_clicked(const QModelIndex &index)
{
    // Activate pushbuttons
    if(ui->gridWidget_admin_memberdatafields->isHidden())
    {
        ui->pushButton_admin_deletemember->setEnabled(true);
        ui->pushButton_admin_editmember->setEnabled(false);
        ui->pushButton_admin_addmember->setEnabled(true);
    }
}


// Update ID column in table model with new line edit information
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

// Update name column in table model with new line edit information
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

// Update price column in table model with new line edit information
void MainWindow::on_lineEdit_admin_itemsubmission_price_textEdited(const QString &productPrice)
{
    //Constant
    const int PRODUCT_PRICE_COLUMN = 2; // Column for the product price

    //Variables
    QModelIndex productPriceIndex; // Model index of the product price for the selected row

    productPriceIndex = ui->tableView_admin_inventory->currentIndex();

    // Ensure product id row is selected
    productPriceIndex = productPriceIndex.model()->index(productPriceIndex.row(), PRODUCT_PRICE_COLUMN);

    itemModel->setData(productPriceIndex, QVariant(productPrice));
}

// Populate line edit with selected data and prevent user from selecting new data when changing itemModel
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


    // Check if table model is clean or dirty. Dirty means data is being added, updated, or deleted
    if (!itemModel->isDirty())
    {
        // Display selected item data
        ui->gridWidget_admin_itemdatafields->setVisible(true);
        ui->pushButton_admin_itemsubmission_submit->setVisible(false);
        ui->pushButton_admin_itemsubmission_cancel->setVisible(false);

        productData = index.sibling(index.row(), PRODUCT_ID_COLUMN);
        ui->lineEdit_admin_itemsubmission_id->setText(productData.data().toString());

        productData = index.sibling(index.row(), PRODUCT_NAME_COLUMN);
        ui->lineEdit_admin_itemsubmission_name->setText(productData.data().toString());

        productData = index.sibling(index.row(), PRODUCT_PRICE_COLUMN);
        ui->lineEdit_admin_itemsubmission_price->setText(productData.data().toString());

        // Make data uneditable
        ui->lineEdit_admin_itemsubmission_id->setReadOnly(true);
        ui->lineEdit_admin_itemsubmission_name->setReadOnly(true);
        ui->lineEdit_admin_itemsubmission_price->setReadOnly(true);

        // Enable edit and delete functionality
        ui->pushButton_admin_edititem->setEnabled(true);
        ui->pushButton_admin_deleteitem->setEnabled(true);
    }
    else
    {
        qDebug() << "detected as dirty";

        // Detect unwanted data
        if (ui->pushButton_admin_additem->isEnabled())
        {
            if (index.row() != itemModel->rowCount() - 1)
            {
                // Display error message
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
                //  Display error message
                ui->label_admin_products_errormessage->setText("Warning, Please confirm or cancel the current \n"
                                                               "deletion before editing or adding another \n"
                                                               "product.");

                ui->label_admin_products_errormessage->setVisible(true);
                ui->tableView_admin_inventory->setCurrentIndex(currentProcessIndex);

            }//end if (index.data().toString() == ui->label_admin_itemsubmission_id->text())
        }//end if (ui->pushButton_admin_deleteitem->isEnabled());
    }
}

// Uppercase first letter in every word and lowercase the rest
QString MainWindow::normalizeCapitalization(QString text)
{
    //Vairables
    QString::Iterator textIterator;   //An iterator over text
    bool              newWord = true; //A bool value true if processing a new word

    text = text.toLower();
    textIterator = text.begin();

    // Iterate through word
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

// Update tableview line edits with changed data
void MainWindow::on_tableModel_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                               const QVector<int> &roles)
{
    //Constant
    const int PRODUCT_ID_COlUMN    = 0; // Column for product id
    const int PRODUCT_NAME_COLUMN  = 1; // Column for product name
    const int PRODCUT_PRICE_COLUMN = 2; // Column for product price

    //Variables
    QModelIndex selectedRow; // Model index in the selected row

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
