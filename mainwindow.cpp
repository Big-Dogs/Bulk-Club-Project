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
    ui->stackedWidget_main->setCurrentIndex(HOME);
    setPermissions(PermissionLevel::NONE); //logs user out
}

// Navigates to POS page
void MainWindow::on_pushButton_POS_clicked()
{
    ui->stackedWidget_main->setCurrentIndex(POS);
}

// Navigates to sales page
void MainWindow::on_pushButton_sales_clicked()
{
    ui->stackedWidget_main->setCurrentIndex(SALES);
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
    QSqlQueryModel *dailySalesModel = new QSqlQueryModel; // Create new model
    QSqlQuery query; // Query to gather data

    //Retrieve purchases sorted by date
    query.prepare("SELECT purchases.datePurchased, purchases.memberID, "
                                  "products.name, products.price, purchases.qty, products.price * purchases.qty "
                                  "FROM purchases join products "
                                  "ON (products.productID = purchases.productID) "
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
    dailySalesModel->setHeaderData(DAILY_ID, Qt::Horizontal, tr("Member ID"));
    dailySalesModel->setHeaderData(DAILY_ITEM, Qt::Horizontal, tr("Item"));
    dailySalesModel->setHeaderData(DAILY_PRICE, Qt::Horizontal, tr("Price"));
    dailySalesModel->setHeaderData(DAILY_QTY, Qt::Horizontal, tr("Qty"));
    dailySalesModel->setHeaderData(DAILY_TOTAL, Qt::Horizontal, tr("Total"));

    // Resize tableview columns
    ui->tableView_sales_daily->resizeColumnToContents(2);

    // Initialize tableView_sales_daily using querymodel
    ui->tableView_sales_daily->setModel(dailySalesModel);
    dailySalesModel->setHeaderData(DAILY_DATE, Qt::Horizontal, tr("Date"));
    dailySalesModel->setHeaderData(DAILY_ID, Qt::Horizontal, tr("Member ID"));
    dailySalesModel->setHeaderData(DAILY_ITEM, Qt::Horizontal, tr("Item"));
    dailySalesModel->setHeaderData(DAILY_PRICE, Qt::Horizontal, tr("Price"));
    dailySalesModel->setHeaderData(DAILY_QTY, Qt::Horizontal, tr("Qty"));
    dailySalesModel->setHeaderData(DAILY_TOTAL, Qt::Horizontal, tr("Total"));

    // Hide numerical vertical header
    ui->tableView_sales_daily->verticalHeader()->setVisible(false);

    // Make fields uneditable
    ui->tableView_sales_daily->setEditTriggers(QTableView::NoEditTriggers);
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

    bool queryError = query.exec("SELECT members.memberID, members.name, sum(products.price * purchases.qty) AS Revenue FROM members "
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
                    " sum(products.price * purchases.qty)"
                    "FROM products LEFT OUTER JOIN purchases "
                    "ON products.productID = purchases.productID "
                    "GROUP BY products.productID")))
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
    ui->tableView_sales_sortitem->sortByColumn(ITEM_PRICE, Qt::AscendingOrder);
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
    retrieveData.prepare("SELECT members.memberID, members.name, sum(products.price * purchases.qty) AS revenue FROM members "
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

    //hides all warnings
    ui->label_admin_membersubmission_nameID_warning->hide();

    //used to add the member to the database
    QSqlQuery query;

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
    tempMemberAdd.name = normalizeCapitalization(ui->lineEdit_admin_membersubmission_name->text());

    //sets the executive status based off of the member based off of the radio button
    qDebug() << tempMemberAdd.name;
    if(ui->radioButton_admin_member->isChecked())
        tempMemberAdd.executiveStatus = "Executive";
    else
        tempMemberAdd.executiveStatus = "Regular";

    //sets the expiration date of the member based on the current date
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


    //checks to make sure that the id and name fields contain at least one character.
    //If either of them dont, a warning message pops up that tells the user what
    //to do
    if(tempMemberAdd.name != "")
    {
        if(tempMemberAdd.id != "")
        {
            //inserts the member into the database
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
                //sets the correct warning messages
                ui->label_admin_membersubmission_nameID_warning->setText(
                            "WARNING: Please enter a unique name and ID");
                ui->label_admin_membersubmission_nameID_warning->show();

                qDebug() << query.lastError().text();
                qDebug() << "name == __ | ID != __  ";
            }
            else
            {
                //clears out all of the member fields, hides the add member widgets, and warnings
                MainWindow::on_pushButton_admin_member_clicked();
                MainWindow::ClearMemberFields();

                //hides warning message, and hides member text fields
                ui->gridWidget_admin_memberdatafields->hide();
                ui->label_admin_membersubmission_nameID_warning->hide();
            }
        }
        else
        {
            //sets the correct warning messages
            ui->label_admin_membersubmission_nameID_warning->setText(
                        "WARNING: Please enter a unique name and ID");
            ui->label_admin_membersubmission_nameID_warning->show();

            qDebug() << query.lastError().text();
            qDebug() << "name != __ | ID == __  ";
        }
    }
    else
    {
        //sets the correct warning messages
        ui->label_admin_membersubmission_nameID_warning->setText(
                    "WARNING: Please enter a unique name and ID");
        ui->label_admin_membersubmission_nameID_warning->show();

        qDebug() << query.lastError().text();
        qDebug() << "name == __ | ID != __  ";
    }
}

void MainWindow::on_pushButton_admin_membersubmission_cancel_clicked() // cancels submission for adding/editing
{
    ui->gridWidget_admin_memberdatafields->hide();
    ui->pushButton_admin_deletemember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);

    ui->label_admin_membersubmission_nameID_warning->hide();

    //clears out the member information line edits
    MainWindow::ClearMemberFields();
}

void MainWindow::on_pushButton_admin_confirmdeletemember_clicked() // confirms delete member
{
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->pushButton_admin_deletemember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);

    //removes the row at the currently selected index based on member id.
    deleteMemberIndex = deleteMemberIndex.sibling(deleteMemberIndex.row(), memberModel->fieldIndex("memberID"));
    if(memberModel->removeRow(deleteMemberIndex.row()))
    {
        qDebug() << "The member failed to delete. " << memberModel->lastError().text();
    }
    if(!(memberModel->submitAll()))
    {
        qDebug() << "\nFailed to submit the deleteion request\n";
    }
}

void MainWindow::on_pushButton_admin_canceldeletemember_clicked() // cancels delete member
{
    ui->gridWidget_admin_confirmdeletemember->hide();
    ui->pushButton_admin_deletemember->setEnabled(false);
    ui->pushButton_admin_addmember->setEnabled(true);
    ui->pushButton_admin_editmember->setEnabled(true);
}

void MainWindow::on_tableView_admin_members_doubleClicked(const QModelIndex &index) // double click admin member table
{
    ui->pushButton_admin_confirmdeletemember->setEnabled(true);
    // set text for label_admin_confirmdeletemember and change initial value to empty
}

    /*----Inventory Page----*/
//The button that initialize the add item feature
void MainWindow::on_pushButton_admin_additem_clicked() //add item button
{
    if (!itemModel->isDirty())
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
    }//end if (!itemModel->isDirty())
}

//The button that initialize the edit item feature
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

//The button that initialize the delete item feature
void MainWindow::on_pushButton_admin_deleteitem_clicked() // delete item button
{
    //Variables
    QString     confirmDeleteMessage; //The message display to confirm the deletion
    QModelIndex deleteProduct;        //The index of the product being deleted

    if (!itemModel->isDirty())
    {
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
    }//end if (!item->itemModel->isDirty())
}

//Submit the changes to itemModel from add or edit item to the database
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

//Reverts the changes made to itemModel by add or edit item
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

//Submit the changes made to itemModel by delete item to the database
void MainWindow::on_pushButton_admin_confirmdeleteitem_clicked() // confirms delete
{
    ui->gridWidget_admin_confirmdeleteitem->hide();
    ui->gridWidget_admin_itemdatafields->setVisible(false);
    ui->pushButton_admin_deleteitem->setEnabled(false); //no item will be selected
    ui->pushButton_admin_additem->setEnabled(true);

    ui->label_admin_products_errormessage->setVisible(false);

    itemModel->submitAll();
}

//Revert changes made to itemModel by delete item
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

/*----Membership Page push buttons----*/
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
    QStringList memberIDList = database->GetExecutiveMemberIDList();

    QSqlQuery query;
    QVector<ExecutiveMemberRebate> memberList;


    query.prepare("SELECT members.memberID, members.name, "
                  "sum(purchases.qty * products.price) "
                  "FROM members, purchases, products "
                  "WHERE members.memberID = purchases.memberID "
                  "AND purchases.productID = products.productID "
                  "AND members.memberID = :memberID");


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

    // Get members who made no purchases
    query.prepare("SELECT DISTINCT members.memberID, members.name, 0 , 0 "
                  "FROM members, purchases "
                  "WHERE members.memberType = 'Executive' "
                  "AND members.memberID NOT IN "
                  "(SELECT memberID from purchases)" );

    if(query.exec())
    {
        while(query.next())
        {
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


    //printing the list of executive members
    for(int i = 0; i < memberList.size(); i++)
    {
        qDebug() << memberList[i].name << " " << memberList[i].memberID << " "
                 << memberList[i].amountSpent << " " << memberList[i].rebate << "\n";
    }


    QStringList tableColumns;
    ui->tableWidget_membership->setColumnCount(3);
    tableColumns << "ID" << "Name" << "Rebate";
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

/*!
 * \brief MainWindow::on_pushButton_membership_expiration_clicked : Method
 * allows user to determine which memberships are expiring based on month
 * entered. Intended to be used with ...expire_clicked, the confirmation button.
 */
void MainWindow::on_pushButton_membership_expiration_clicked()
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
    // Resize Columns
    ui->tableView_membership->resizeColumnsToContents();
    // Hide numerical vertical header
    ui->tableView_membership->verticalHeader()->setVisible(false);
    // Make fields uneditable
    ui->tableView_membership->setEditTriggers(QTableView::NoEditTriggers);
    // Show it
    ui->gridWidget_membership_expire->show();
}

/*!
 * \brief MainWindow::on_pushButton_membership_expire_clicked: Method attached
 * to button to confirm entry of expiration month selected. Month selected will
 * determine which members to show (based on expiration date)
 */
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

// Button brings user to membership upgrade recommendations
void MainWindow::on_pushButton_membership_upgrades_clicked()
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

// Button brings user to membership downgrade recommendations
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
void MainWindow::on_comboBox_pos_memberlist_activated(int index) // sets member for upcoming purchase
{
    posMemberID = ui->comboBox_pos_memberlist->currentText().toInt();
    ui->comboBox_pos_itemlist->setEnabled(true); // enables the item selection
}

void MainWindow::on_comboBox_pos_memberlist_currentIndexChanged(int index) // clears the receipt page when another member is selected
{
    ui->tableWidget_pos_receipts->clear();
    InitializePosTable();
}

void MainWindow::on_comboBox_pos_itemlist_activated(int index) // sets item for upcoming purchase
{
    posItemName = ui->comboBox_pos_itemlist->currentText();
    posItem = this->database->GetItem(posItemName);
    ui->comboBox_pos_qty->setEnabled(true); // enables the quantity selection
}

void MainWindow::on_comboBox_pos_qty_activated(int index) // sets quantity for upcoming purchase
{
    posQty = index + 1;
    posPrice = this->database->GetPrice(posItemName);
    posTotal = posPrice * posQty; // calculates total
    ui->label_pos_price->setText(QString::number(posTotal, 'f', 2));
    ui->pushButton_pos_purchase->setEnabled(true); // enables the purchase button
}

void MainWindow::on_pushButton_pos_purchase_clicked() // purchase button
{

    printReceipt(); // prints receipt

    this->database->AddPurchase(posMemberID, posItem, QDate::currentDate().toString("M/dd/yyyy"), posQty); // adds purchase to database

    //resets purchase data fields
    ui->comboBox_pos_itemlist->setCurrentIndex(0);
    ui->comboBox_pos_qty->setCurrentIndex(0);
    ui->comboBox_pos_qty->setEnabled(false);
    ui->label_pos_price->setText("");
    ui->pushButton_pos_purchase->setEnabled(false);
    ui->comboBox_pos_itemlist->setEnabled(false);

    InitializeSalesTableView(); // updates the daily sales options to include current purchase
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
}

/*----Home Page push buttons----*/
void MainWindow::on_pushButton_home_login_clicked() // attempts to log the user in
{
    ui->label_home_warning->setText("");

    QString username = ui->lineEdit_home_username->text();
    QString password = ui->lineEdit_home_password->text();

    int permissionIndex = this->database->CheckLogin(username, password); //checks database for matching credentials

    if (permissionIndex > NONE) // if there is a match, set the permission level to matching index
    {
        setPermissions(permissionIndex);

    }
    else // if not, display warning message
    {
        ui->label_home_warning->setText("Invalid username or password. Please try again.");
        ui->label_home_warning->setStyleSheet("color: red");
    }

    ui->lineEdit_home_username->clear();
    ui->lineEdit_home_password->clear();
}


// Helper Function Definitions

void MainWindow::setPermissions(int permission) //uses login credentials to control the user's access to program features
{
    switch(permission)
    {
    case PermissionLevel::NONE:                 //login failed
        ui->pushButton_POS->setEnabled(false);
        ui->pushButton_sales->setEnabled(false);
        ui->pushButton_members->setEnabled(false);
        ui->pushButton_admin->setEnabled(false);
        ui->stackedWidget_main->setCurrentIndex(HOME);
        break;
    case PermissionLevel::EMPLOYEE:             //employee login enables the POS page
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(false);
        ui->pushButton_members->setEnabled(false);
        ui->pushButton_admin->setEnabled(false);
        ui->stackedWidget_main->setCurrentIndex(POS);
        break;
    case PermissionLevel::MANAGER:              //manager login enables POS, sales, and membership pages
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(true);
        ui->pushButton_members->setEnabled(true);
        ui->pushButton_admin->setEnabled(false);
        ui->stackedWidget_main->setCurrentIndex(SALES);
        break;
    case PermissionLevel::ADMINISTRATOR:        //administrator login enables all pages
        ui->pushButton_POS->setEnabled(true);
        ui->pushButton_sales->setEnabled(true);
        ui->pushButton_members->setEnabled(true);
        ui->pushButton_admin->setEnabled(true);
        ui->stackedWidget_main->setCurrentIndex(ADMIN);
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

void MainWindow::InitializeSalesTableView() // fills the daily sales combo box and sets up the tableview
{

    ui->comboBox_sales_byday->clear();
    QSqlQuery query;
    //query the database to get every purchase date
    query.prepare("select datePurchased from purchases group by datePurchased");
    //if successful, fill the sales by day combo box with retrieves items
    if(query.exec())
    {
       while (query.next())
       {
           ui->comboBox_sales_byday->addItem(query.value("datePurchased").toString());
       }
    }
    //if not successful, print an error message
    else
    {
        qDebug() << query.lastError().text();
    }


    // Hide numerical vertical header
    ui->tableView_sales_daily->verticalHeader()->setVisible(false);
    // Make fields uneditable
    ui->tableView_sales_daily->setEditTriggers(QTableView::NoEditTriggers);
}

void MainWindow::InitializePosTable() // fills the POS page combo boxes and initializes the receipts table
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

    //initializing member id combo box
    QStringList members = this->database->GetPOSMembers();
    qDebug() << members.length() << " items";
    //item number combo box
    if(ui->comboBox_pos_memberlist->count() == 0)
    {
        for (int i = 0; i < members.length(); i++)
        {
            ui->comboBox_pos_memberlist->addItem(members.at(i));
        }
    }

    //initializing items combo box
    QStringList items = this->database->GetNames();
    qDebug() << items.length() << " items";
    //item number combo box
    if(ui->comboBox_pos_itemlist->count() == 0)
    {
        for (int i = 0; i < items.length(); i++)
        {
            ui->comboBox_pos_itemlist->addItem(items.at(i));
        }
    }

    //initializing quantity combo box
    if(ui->comboBox_pos_qty->count() == 0)
    {
        for (int i = 1; i <= 10; i++)
        {
            ui->comboBox_pos_qty->addItem(QString::number(i));
        }
    }
}

void MainWindow::printReceipt() // prints the receipt table in the pos page
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
    total->setText(QString::number(posTotal, 'f', 2));

    ui->tableWidget_pos_receipts->insertRow(receiptRow);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 0, member);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 1, item);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 2, price);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 3, qty);
    ui->tableWidget_pos_receipts->setItem(receiptRow, 4, total);
    receiptRow++;

}

void MainWindow::on_tableView_admin_members_clicked(const QModelIndex &index)
{
    if(ui->gridWidget_admin_memberdatafields->isHidden())
    {
        ui->pushButton_admin_deletemember->setEnabled(true);
        ui->pushButton_admin_editmember->setEnabled(false);
        ui->pushButton_admin_addmember->setEnabled(true);
    }
}


//Keeping the id column in the table model updated with what is new the line edit
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

//Keeping the name column in the table model updated with what is new the line edit
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

//Keeping the price column in the table model updated with what is new the line edit
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

//Fills in line edit with selected data and prevent user from selecting new data when change itemModel
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

//Upper case the first letter in every word and lower case the rest
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

//Update line edits with data changed in the tableview
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
