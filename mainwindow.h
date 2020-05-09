#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>
#include <QCompleter>
#include <QSqlDriver>
#include "Database.h"
#include "MembershipTableModel.h"
#include "debugflags.h"
#include "moneydelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:


    /*----Menu Navigation----*/

        /*----POS page----*/
    /*!
     * \brief on_pushButton_POS_clicked : Method to switch current page to POS
     */
    void on_pushButton_POS_clicked();

        /*!
         * \brief on_pushButton_pos_purchase_clicked : Method to confirm purchase and add it to the database
         */
        void on_pushButton_pos_purchase_clicked();

        /*!
         * \brief on_comboBox_pos_itemlist_activated : Method to select the item for the upcoming purchase
         */
        void on_comboBox_pos_itemlist_activated(int index);

        /*!
         * \brief on_comboBox_pos_qty_activated : Method to select the quantity for the upcoming purchase
         */
        void on_comboBox_pos_qty_activated(int index);

        /*!
         * \brief on_comboBox_pos_memberlist_activated : Method to select the member id for the upcoming purchase
         */
        void on_comboBox_pos_memberlist_activated(int index);

        /*!
         * \brief on_comboBox_pos_memberlist_currentIndexChanged : Method to clear the receipts page when a new
         * member is selected
         */
        void on_comboBox_pos_memberlist_currentIndexChanged(int index);


        /*----Home page----*/
    /*!
     * \brief on_pushButton_home_clicked : Method to switch current page to home and log the user out
     */
    void on_pushButton_home_clicked();

        /*!
         * \brief on_pushButton_home_login_clicked : Method to attempt to log the user in
         */
        void on_pushButton_home_login_clicked();

        /*----Sales page----*/
    /*!
     * \brief on_pushButton_sales_clicked : Method to switch current page to sales
     */
    void on_pushButton_sales_clicked();

        /*!
         * \brief on_pushButton_sales_daily_clicked : Method to open daily sales tab
         */
        void on_pushButton_sales_daily_clicked();

            /*!
             * \brief on_pushButton_sale_byday_clicked : method to filter sales by day date purchased
             */
            void on_pushButton_sale_byday_clicked();

        void on_pushButton_sales_sortmember_clicked();

        void on_pushButton_sales_sortitem_clicked();

        void on_pushButton_sales_searchmember_clicked();

            void on_pushButton_sales_searchmemberconfirm_clicked();

        void on_pushButton_sales_searchitem_clicked();

            void on_pushButton_sales_searchitemconfirm_clicked();

         /*----Membership page----*/
    /*!
     * \brief on_pushButton_members_clicked : Method to switch current page to members
     */
    void on_pushButton_members_clicked();

        void on_pushButton_membership_rebates_clicked();

        void on_pushButton_membership_expiration_clicked();

        void on_pushButton_membership_upgrades_clicked();

        void on_pushButton_membership_downgrades_clicked();

        void on_pushButton_membership_expire_clicked();

        /*----Admin page----*/
    /*!
     * \brief on_pushButton_admin_clicked : Method to switch current page to admin
     */
    void on_pushButton_admin_clicked();

        void on_pushButton_admin_member_clicked();

            void on_pushButton_admin_addmember_clicked();

            void on_pushButton_admin_editmember_clicked();

            void on_pushButton_admin_deletemember_clicked();

            void on_pushButton_admin_membersubmission_submit_clicked();

            void on_pushButton_admin_membersubmission_cancel_clicked();

            void on_pushButton_admin_confirmdeletemember_clicked();

            void on_pushButton_admin_canceldeletemember_clicked();

            void on_tableView_admin_members_doubleClicked(const QModelIndex &index);

            void ClearMemberFields();

        void on_pushButton_admin_inventory_clicked();

            void on_pushButton_admin_additem_clicked();

            void on_pushButton_admin_edititem_clicked();

            void on_pushButton_admin_deleteitem_clicked();

            void on_pushButton_admin_itemsubmission_submit_clicked();

            void on_pushButton_admin_itemsubmission_cancel_clicked();

            void on_pushButton_admin_confirmdeleteitem_clicked();

            void on_pushButton_admin_canceldeleteitem_clicked();

            void on_tableView_admin_inventory_doubleClicked(const QModelIndex &index);



    // Autocomplete text searches
    void TextCompleter(QStringList products, QLineEdit *inputField);


    void on_tableView_admin_members_clicked(const QModelIndex &index);

    void on_stackedWidget_admin_currentChanged(int arg1);

    void on_stackedWidget_admin_widgetRemoved(int index);

    void on_stackedWidget_main_currentChanged(int arg1);

    void on_stackedWidget_sales_currentChanged(int arg1);

    void on_tableView_admin_inventory_activated(const QModelIndex &index);

    void on_lineEdit_admin_itemsubmission_id_textEdited(const QString &arg1);

    void on_lineEdit_admin_itemsubmission_name_textEdited(const QString &arg1);

    void on_lineEdit_admin_itemsubmission_price_textEdited(const QString &arg1);

    void on_tableView_admin_inventory_clicked(const QModelIndex &index);

    /* This is a custom slot that is connected to the dataChanged
     * signal for itemModel when the inventory management button is
     * clicked. It is used to update the data in the item submission
     * line edits when the user changes the data in the table.
     */
    void on_tableModel_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                   const QVector<int> &roles);

    void on_tableView_item_currentChanged(int row);

    void on_tableView_admin_inventory_pressed(const QModelIndex &index);





private:
    /* This function capitalize the first letter of
     * every word and set all other letters to lower
     * case.
     */
    QString normalizeCapitalization(QString text);

    /* restrictSelectToRow
     *      Restricts the user ability to select a tableview item to
     *      only the items in the same row of selectedRow
     *
     * Precondition:
     *      pass in const QModelIndex &
     *          const QModelIndex &selectedRow - An item in the row the user is allow
     *                                           to select items from. It is
     *                                           intendended that you pass in the current
     *                                           index from you tableview
     *
     *  Postcondition:
     *      The user can only selects items in the same row as selectedRow
     */
    void restrictSelectToRow(const QModelIndex &selectedRow);

    Ui::MainWindow *ui;
    int index = 0; // Testing Permissions
    MembershipTableModel *membershipModel; // Membership Table View Configuration
    Database *database; // Pointer to database

    QSqlTableModel *itemModel; //The table model use to display the products table

    MoneyDelegate *formatPrice; //A pointer to the delegate used to format
                                //items in table for money

    // For use in 'Search Sales by Product Name' feature
    QStringList productList; // List of all products in database
    QString salesReportProduct; // Product manager wishes to view for sales report



    //For use in the admin member management functions
    QSqlTableModel *memberModel;
    QModelIndex deleteMemberIndex;

    enum memberTableHeaders
    {
        memberID,
        name,
        membershipType,
        expirationDate,
        membershipCost
    };

    //For use in 'Display Member Rebates' feature
    //this is a comment
    struct ExecutiveMemberRebate
    {
        QString memberID;
        QString name;
        QString amountSpent;
        QString rebate;
    };

    //For use in 'Sort item' feature

    QSqlQueryModel *sortItemModel;
    enum sortItemHeaders
    {
        ITEM_ID,
        ITEM_NAME,
        ITEM_PRICE
    };

    //For use in 'Add Customer' feature
    struct TempMember
    {
        QString id;
        QString name;
        QString executiveStatus;
        QString expirationDate;
    };

    // For use in POS page
    QString posItemName; // name of item
    int posMemberID; // member id
    int posItem; // item id
    int posQty; // item quantity
    double posPrice; // item price
    double posTotal; // total of price * qty
    int receiptRow; // index of row in receipt table


    // For use in upgrade/downgrade features
    float rebateAmount = 0.0; // member's rebate received
    int downgradeCount = 0; // number of downgrade recommendations on QStringList
    int upgradeCount = 0; // number of downgrade recommendations on QStringList
    const int TABLE_WIDGET_COLS = 4; // number of columns on tablewidget
    const float REBATE_RATE = 0.02;      // rebate rate for calculation
    const float REBATE_MIN = 65.0;       // minimum rebate needed for exec member
    QStringList tableWidgetColumnNames = {
        "ID",
        "Name",
        "Amount Spent",
        "Rebate Amount"
    };


    /*!
     * \brief The Pages enum is used to track which main page the user is on
     */
    enum Pages
    {
        HOME,
        POS,
        SALES,
        MEMBER,
        ADMIN,
    };

    /*!
     * \brief The SalesPages enum is used to track which sales tab the user is on
     */
    enum SalesPages
    {
        SALES_DAILY,
        SALES_SORT_MEMBER,
        SALES_SORT_ITEM,
        SALES_SEARCH_MEMBER,
        SALES_SEARCH_ITEM,
    };

    /*!
     * \brief The Pages enum is used to track which admin tab the user is on
     */
    enum AdminPages
    {
        ADMIN_MEMBER,
        ADMIN_ITEM,
    };


    QModelIndex currentProcessIndex; //The current index being processed


    // Enum to keep track of upgrade/downgrade feature columns
    enum MembershipTableWidgetColumns
    {
        MEMBERSHIP_NUMBER,
        MEMBER_NAME,
        AMT_SPENT,
        REBATE_AMOUNT
    };



    /*!
     * \brief The DailySales enum is used to track columns on the daily sales table
     */
    enum DailySales
    {
        DAILY_DATE,
        DAILY_ID,
        DAILY_ITEM,
        DAILY_PRICE,
        DAILY_QTY,
        DAILY_TOTAL,
    };

    /*!
     * \brief The PermissionLevel enum is used to track login credentials
     */
    enum PermissionLevel
    {
        NONE,
        EMPLOYEE,
        MANAGER,
        ADMINISTRATOR,
    };


    // Helper Function Prototypes

        // Reset all values in membership table widget
    void InitializeMembershipTableWidget();

        // Print suggested upgrades report
    void PrintUpgradeReport(QVector<Database::Member> regularMemberPurchases);

        // Print suggested downgrades report
    void PrintDowngradeReport(QVector<Database::Member> executiveMemberPurchases);

    /*!
     * \brief SetPermissions : Method for changing the user's permission level
     * \param permission : An integer holding the index that will set the user's permission level
     */
    void setPermissions(int permission);

    /*!
     * \brief InitializeSalesTableView : Method for filling the daily sales combo box and initializing
     * the daily sales report table
     */
    void InitializeSalesTableView();

    /*!
     * \brief InitializePosTable : Method for filling the POS page combo boxes and initializing
     * the POS receipts table
     */
    void InitializePosTable();

    /*!
     * \brief printReceipt : method for populating the POS receipts table
     */
    void printReceipt();




};
#endif // MAINWINDOW_H
