#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>
#include <QCompleter>
#include <QSqlDriver>
#include "Database.h"
#include "MembershipTableModel.h"
#include "moneydelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/*!
 * \brief The MainWindow class handles user interface and serves as the primary
 * container for the entire program
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief MainWindow::MainWindow() Default Constructor
     * \param parent QWidget parent object
     */
    MainWindow(QWidget *parent = nullptr);

    /*!
     * \brief MainWindow
     *      The copy constructor for the main window
     *
     * Precondition
     *      none
     *
     * Postcondition
     *      initialize any needed set up before the program start
     *
     * \param otherWindow - The main window is being copied
     */
    MainWindow(const MainWindow &otherWindow);

    /*!
     * \brief ~MainWindow() Destructor
     */
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
         * \param index : unused
         */
        void on_comboBox_pos_itemlist_activated(int index);

        /*!
         * \brief on_comboBox_pos_qty_activated : Method to select the quantity for the upcoming purchase
         * \param index : unused
         */
        void on_comboBox_pos_qty_activated(int index);

        /*!
         * \brief on_comboBox_pos_memberlist_activated : Method to select the member id for the upcoming purchase
         * \param index : unused
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

            /*!
             * \brief on_radioButton_sales_byday_executive_clicked : Method to filter sales results by executive member
             */
            void on_radioButton_sales_byday_executive_clicked();

            /*!
             * \brief on_radioButton_sales_byday_regular_clicked : Method to filter sales results by regular member
             */
            void on_radioButton_sales_byday_regular_clicked();

            /*!
             * \brief on_radioButton_sales_byday_executive_clicked : Method to filter sales results by both member types
             */
            void on_radioButton_sales_byday_both_clicked();

         /*!
         * \brief on_pushButton_sales_sortmember_clicked -
         *       A slot that is connected to an signal emitted when the sort by member
         *       button on the sales page is clicked
         *
         * Precondition
         *      login as manager and nagivate to the sales page
         *
         * Postcondition
         *      A table is displayed presenting all the members in the database with
         *      their id, name, and revenue. A label displaying total revenue is displayed
         *      beneath the table.
         */
        void on_pushButton_sales_sortmember_clicked();

        /*!
         * \brief on_pushButton_sales_sortitem_clicked : Button allows user to
         * activate "Sort by Item" feature
         */
        void on_pushButton_sales_sortitem_clicked();

        /*!
         * \brief on_pushButton_sales_searchmember_clicked -
         *       A slot that is connected to an signal emitted when the search by member
         *       button on the sales page is clicked
         *
         * Precondition -
         *      login as manager and nagivate to the sales page
         *
         * Postcondition -
         *      A search grid is displayed providing the user with a QLineEdit to type in
         *      either the name or id of the member they wish to search for. In addition,
         *      the user is also provided with a search button to search the database based
         *      on their input into the QLineEdit.
         */
        void on_pushButton_sales_searchmember_clicked();

             /*!
             * \brief on_pushButton_sales_searchmemberconfirm_clicked -
             *       A slot that is connected to an signal emitted when the search button for
             *       the search by member feature on the sales page is clicked
             *
             * Precondition -
             *      click on the search by member button on the sales page and input the wanted
             *      member's name or id in the QLineEdit
             *
             * Postcondition -
             *      The text the user inputted into the lineEdit_sales_searchMembers is used to
             *      search the database for the wanted member. If the member is found then the member's
             *      id, name, and revenue is displayed in the table. If the member is not found, then
             *      an error message is displayed below the search button informing the user that the
             *      member was not found.
             */
            void on_pushButton_sales_searchmemberconfirm_clicked();

        /*!
         * \brief on_pushButton_sales_searchitem_clicked : Button allows
         * user to activate "Search by Item" feature
         */
        void on_pushButton_sales_searchitem_clicked();

            /*!
             * \brief on_pushButton_sales_searchitemconfirm_clicked : Button
             * allows user to confirm selection for item they would like to
             * search for
             */
            void on_pushButton_sales_searchitemconfirm_clicked();

         /*----Membership page----*/
    /*!
     * \brief on_pushButton_members_clicked : Method to switch current page to members
     */
    void on_pushButton_members_clicked();

        /*!
         * \brief on_pushButton_membership_rebates_clicked : Button allows user
         * to display membership rebate amounts
         */
        void on_pushButton_membership_rebates_clicked();

        /*!
         * \brief on_pushButton_membership_expiration_clicked : Button allows
         * user to enter member expiration report interface
         */
        void on_pushButton_membership_expiration_clicked();

            /*!
             * \brief on_pushButton_membership_expire_clicked : Button will
             * generate membership expiration report based on month selected
             */
            void on_pushButton_membership_expire_clicked();

        /*!
         * \brief MainWindow::on_pushButton_membership_upgrades_clicked : Method
         * will display which members should upgrade their membership based on
         * rebate amount earned through purchases
         */
        void on_pushButton_membership_upgrades_clicked();

        /*!
         * \brief MainWindow::on_pushButton_membership_downgrades_clicked : Method
         * will display which members should downgrade their membership based on
         * insufficient rebate amount earned through purchases
         */
        void on_pushButton_membership_downgrades_clicked();

        /*----Admin page----*/
    /*!
     * \brief on_pushButton_admin_clicked : Method to switch current page to admin
     */
    void on_pushButton_admin_clicked();

        /*!
         * \brief on_pushButton_admin_member_clicked : Button allows user to
         * enter membership administration interface
         */
        void on_pushButton_admin_member_clicked();

            /*!
             * \brief on_pushButton_admin_addmember_clicked : Button allows
             * user to enter "Add Member" interface
             */
            void on_pushButton_admin_addmember_clicked();

            /*!
             * \brief on_pushButton_admin_editmember_clicked : Button allows
             * user to enter "Edit Member" interface
             */
            void on_pushButton_admin_editmember_clicked();

            /*!
             * \brief on_pushButton_admin_deletemember_clicked : Button allows
             * user to enter "Delete Member" interface
             */
            void on_pushButton_admin_deletemember_clicked();

            /*!
             * \brief on_pushButton_admin_membersubmission_submit_clicked :
             * Button will confirm user's choice
             */
            void on_pushButton_admin_membersubmission_submit_clicked();

            /*!
             * \brief on_pushButton_admin_membersubmission_cancel_clicked :
             * Button will cancel user's choice
             */
            void on_pushButton_admin_membersubmission_cancel_clicked();

            /*!
             * \brief on_pushButton_admin_confirmdeletemember_clicked :
             * Button will confirm user's choice to delete member
             */
            void on_pushButton_admin_confirmdeletemember_clicked();

            /*!
             * \brief on_pushButton_admin_canceldeletemember_clicked :
             * Button will cancel user's choice to delete member
             */
            void on_pushButton_admin_canceldeletemember_clicked();

            /*!
             * \brief on_tableView_admin_members_doubleClicked : Allows
             *  user to select specific cells of table
             * \param index : Cell selected by user
             */
            void on_tableView_admin_members_doubleClicked(const QModelIndex &index);

            /*!
             * \brief ClearMemberFields : Method will reinitialize fields to
             * blank state
             */
            void ClearMemberFields();

        /*!
         * \brief on_pushButton_admin_inventory_clicked -
         *       A slot that is connected to an signal emitted when the manage inventory button
         *       on the admin page is clicked
         *
         * Precondition -
         *      login as manager and nagivate to the admin page
         *
         * Postcondition -
         *      The QSqlTableModel that itemModel points to is created and connected to the products
         *      table in the database. itemModel is then displayed in the table view and the item management
         *      system functionality buttons (add item, edit item, and delete item) are displayed. The edit item
         *      and delete item buttons are disabled because no item is selected yet.
         */
        void on_pushButton_admin_inventory_clicked();

            /*!
             * \brief on_pushButton_admin_additem_clicked
             *       A slot that is connected to an signal emitted when the add item button
             *       for the item management system is clicked
             *
             * Precondition -
             *      Access the item management system by clicking on manage inventory on the
             *      admin page
             *
             * Postcondition -
             *      If the item field line edits are not currently displayed, then that are displayed.
             *      The item field line edits are clear of any content and a new row is inserted into
             *      the bottom of itemModel, and selected. The item field line edits read only property
             *      are turned off to allow user input and the submit and cancel buttons for add and edit
             *      items are displayed. The delete item and edit item buttons are disabled to prevent over
             *      lapping functionality.
             */
            void on_pushButton_admin_additem_clicked();

            /*!
             * \brief on_pushButton_admin_edititem_clicked
             *       A slot that is connected to an signal emitted when the edit item button
             *       for the item management system is clicked
             *
             * Precondition -
             *      Access the item management system by clicking on manage inventory on the
             *      admin page, and select a product from the table
             *
             * Postcondition -
             *      The item field line edits read only property are turned off to allow user input and
             *      the submit and cancel buttons for add and edit items are displayed. The delete item
             *      and add item buttons are disabled to prevent overlapping functionality.
             */
            void on_pushButton_admin_edititem_clicked();

            /*!
             * \brief on_pushButton_admin_deleteitem_clicked
             *       A slot that is connected to an signal emitted when the delete item button
             *       for the item management system is clicked
             *
             * Precondition -
             *      Access the item management system by clicking on manage inventory on the
             *      admin page, and select a product from the table
             *
             * Postcondition -
             *      The current row selected is removed from itemModel (not from the database), and the
             *      delete confirmation prompt is displayed. With the delete confirmation prompt its
             *      corresponding confirm and cancel buttons are also displayed to allow the user to
             *      either confirm or cancel the deletion.
             */
            void on_pushButton_admin_deleteitem_clicked();

            /*!
             * \brief on_pushButton_admin_itemsubmission_submit_clicked
             *       A slot that is connected to an signal emitted when the itemsubmission submit item
             *       button for the item management system is clicked
             *
             * Precondition -
             *      Initialize the add item or edit item features by pressing their corresponding buttons
             *      in the item management system. Fill out the item field line edits with the accurate
             *      data for the item.
             *
             * Postcondition -
             *      If the item data the user inputted has an error in it, then an apporiate error message
             *      is displayed. If the item data the user inputted does not have an error, then the changes
             *      made to itemModel are submitted to the database and window is return the state it was in
             *      immediately after clicking manage inventory.
             */
            void on_pushButton_admin_itemsubmission_submit_clicked();

            /*!
             * \brief on_pushButton_admin_itemsubmission_cancel_clicked
             *       A slot that is connected to an signal emitted when the itemsubmission cancel item
             *       button for the item management system is clicked
             *
             * Precondition -
             *      Initialize the add item or edit item features by pressing their corresponding buttons
             *      in the item management system.
             *
             * Postcondition -
             *      All changes made to itemModel are reverted and the window is return to the state it was
             *      in immediately after clicking on manage inventory.
             */
            void on_pushButton_admin_itemsubmission_cancel_clicked();

            /*!
             * \brief on_pushButton_admin_confirmdeleteitem_clicked
             *       A slot that is connected to an signal emitted when the confirm delete item
             *       button for the item management system is clicked
             *
             * Precondition -
             *      Initialize the delete item feature by pressing its corresponding button
             *      in the item management system.
             *
             * Postcondition -
             *      The deletion is submitted to the database, and the window is return the state it was in
             *      immediately after clicking on manage inventory.
             */
            void on_pushButton_admin_confirmdeleteitem_clicked();

            /*!
             * \brief on_pushButton_admin_canceldeleteitem_clicked
             *       A slot that is connected to an signal emitted when the cancel delete item
             *       button for the item management system is clicked
             *
             * Precondition -
             *      Initialize the delete item feature by pressing its corresponding button
             *      in the item management system.
             *
             * Postcondition -
             *      The deletion is reverted, and the window is return the state it was in
             *      immediately after clicking on manage inventory.
             */
            void on_pushButton_admin_canceldeleteitem_clicked();

    /**
    * @brief MainWindow::TextCompleter
    * Continuously searches list of products as the user types what they want
    * to find.
    * @param names the list of school names currently in database
    * @param field QLineEdit form that function is being used on
    */
    void TextCompleter(QStringList products, QLineEdit *inputField);

    /*!
     * \brief on_tableView_admin_members_clicked : Method allows user to
     * directly select cells within the admin tableview
     * \param index : Cell selected by user
     */
    void on_tableView_admin_members_clicked(const QModelIndex &index);

    /*!
     * \brief on_lineEdit_admin_itemsubmission_id_textEdited
     *      A slot that is connect to a signal emitted when the user changes the text in the
     *      item id line edit.
     *
     * Precondition
     *      The user has initialized either the add item or edit item features by clicking on their
     *      corresponding buttons.
     *
     * Postcondition
     *      The data in the id field of the selected row is updated with productId
     *
     * \param productId - The updated QString displayed in the item id line edit
     */
    void on_lineEdit_admin_itemsubmission_id_textEdited(const QString &productId);

    /*!
     * \brief on_lineEdit_admin_itemsubmission_name_textEdited
     *      A slot that is connect to a signal emitted when the user changes the text in the
     *      item name line edit.
     *
     * Precondition
     *      The user has initialized either the add item or edit item features by clicking on their
     *      corresponding buttons.
     *
     * Postcondition
     *      The data in the name field of the selected row is updated with productName
     *
     * \param productName - The updated QString displayed in the item name line edit
     */
    void on_lineEdit_admin_itemsubmission_name_textEdited(const QString &productName);

    /*!
     * \brief on_lineEdit_admin_itemsubmission_price_textEdited
     *      A slot that is connect to a signal emitted when the user changes the text in the
     *      item price line edit.
     *
     * Precondition
     *      The user has initialized either the add item or edit item features by clicking on their
     *      corresponding buttons.
     *
     * Postcondition
     *      The data in the name field of the selected row is updated with productPrice
     *
     * \param productPrice - The updated QString displayed in the item price line edit
     */
    void on_lineEdit_admin_itemsubmission_price_textEdited(const QString &productPrice);

    /*!
     * \brief on_tableView_admin_inventory_clicked
     *      A slot that is connected to a signal emitted when the user clicks on tableView_admin_inventory
     *
     * Precondition
     *      The user accessed the item management system by clicking on the management inventory button in
     *      the admin page
     *
     * Postcondition
     *      If itemModel is clean then all that happens is that the item field line edits are displayed (if
     *      not displayed before) with the data of the newly selected item. If itemModel is dirty and index
     *      lies in a row that was not previously selected then an error message is displayed telling the
     *      user to submit or cancel changes before selecting an item, and the previously seleted item is
     *      reselected.
     *
     * \param index - The QModelIndex of the field the user clicked on
     */
    void on_tableView_admin_inventory_clicked(const QModelIndex &index);

    /*!
     * \brief on_tableModel_dataChanged
     *      This is a custom slot that is connected to the dataChanged
     *      signal for itemModel when the inventory management button is
     *      clicked. It is used to update the data in the item submission
     *      line edits when the user changes the data in the table.
     *
     * Precondition
     *      Modified the data in itemModel by typing directly into tableView_admin_inventory
     *
     * Postcondition
     *      The text in the item field line edits is updated to reflect the changes made in
     *      itemModel
     *
     * \param topLeft - The QModelIndex of the first item changed
     *
     * \param bottomRight - The QModelIndex of the last item changed (for all purposes in this
     *                      program, it is the same as topLeft so it is not used)
     *
     * \param roles - A QVector containing all of the roles of the QModelIndices that was changed.
     *                It is unused in this program.
     */
    void on_tableModel_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                   const QVector<int> &roles);
private:
    /*!
     * \brief normalizeCapitalization
     *     This function capitalize the first letter of
     *     every word and set all other letters to lower
     *     case.
     *
     * Precondition
     *      none
     *
     * Postcondition
     *      Every letter in text is lower cased, then the first letter of every word in
     *      text is upper cased. The result is returned
     *
     * \param text - The QString the modify the capitalization for
     *
     * \return - text after modifying text's capitalization
     */
    QString normalizeCapitalization(QString text);

    Ui::MainWindow *ui;     /// Pointer to user interface object
    QWidget *parent;        ///Pointer to the parent object, used in copy constructor
    int index = 0;          /// Testing Permissions
    MembershipTableModel *membershipModel; /// Pointer to Membership Table View Configuration
    Database *database;     /// Pointer to database object

    QSqlTableModel *itemModel; /// Pointer to table model used to display the products table

    MoneyDelegate *formatPrice; /// Pointer to delegate used to format money-related items in table

    // For use in 'Search Sales by Product Name' feature
    QStringList productList;    /// List of all products in database
    QString salesReportProduct; /// Specific product requested by sales manager

    //For use in the admin member management functions
    QSqlTableModel *memberModel;    /// Model used to configure admin tableview
    QModelIndex deleteMemberIndex;  /// Index of member to delete from database

    /// Enum used to configure membership table headers
    enum memberTableHeaders
    {
        memberID,
        name,
        membershipType,
        expirationDate,
        membershipCost
    };

    /// Struct used to store member data for use in 'Display Member Rebates' feature
    struct ExecutiveMemberRebate
    {
        QString memberID;
        QString name;
        QString amountSpent;
        QString rebate;
    };

    /// Enum used to configure table headers for 'Sort by Item' feature
    QSqlQueryModel *sortItemModel;
    enum sortItemHeaders
    {
        ITEM_ID,
        ITEM_NAME,
        ITEM_PRICE
    };

    /// Struct used to store member data for use in 'Add Customer' feature
    struct TempMember
    {
        QString id;
        QString name;
        QString executiveStatus;
        QString expMonth;
        QString expDay;
        QString expYear;
        QString expDate;
    };

    // For use in 'Add Member' feature
    QIntValidator *idCheck;     /// Validates member ID

    // For use in POS page
    QString posItemName;    /// Name of item purchased
    int posMemberID;        /// ID of member making purchase
    int posItem;            /// ID of product being purchased
    int posQty;             /// Quantity of items purchased
    double posPrice;        /// Price of item purchased
    double posTotal;        /// Total of price * quantity
    int receiptRow;         /// Index of row in receipt table

    // For use in upgrade/downgrade features
    float rebateAmount = 0.0;           /// Amount member will receive (rebate)
    int downgradeCount = 0;             /// Number of downgrade recommendations on QStringList
    int upgradeCount = 0;               /// Number of downgrade recommendations on QStringList
    const int TABLE_WIDGET_COLS = 4;    /// Number of columns on tablewidget
    const float REBATE_RATE = 0.02;     /// Rebate rate used for calculation
    const float REBATE_MIN = 65.0;      /// Minimum rebate needed to make executive membership worth the price
    QStringList tableWidgetColumnNames = {
        "ID",
        "Name",
        "Amount Spent",
        "Rebate Amount"
    }; /// List of names used for tablewidget in 'Recommend Upgrade/Downgrade' features

    /*!
     * \brief Enum used to track which main page the user is on
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
     * \brief Enum used to track which sales tab the user is on
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
     * \brief Enum used to track which admin tab the user is on
     */
    enum AdminPages
    {
        ADMIN_MEMBER,
        ADMIN_ITEM,
    };

    QModelIndex currentProcessIndex; /// The current index being processed

    /// Enum to keep track of upgrade/downgrade feature columns
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
        DAILY_NAME,
        DAILY_STATUS,
        DAILY_ITEM,
        DAILY_PRICE,
        DAILY_QTY,
        DAILY_TOTAL,
    };

    /*!
     * \brief Enum used to track login credentials
     */
    enum PermissionLevel
    {
        NONE,
        EMPLOYEE,
        MANAGER,
        ADMINISTRATOR,
    };

    // Helper Function Prototypes //

    /*!
     * \brief InitializeMembershipTableWidget : Method resets values for
     * membership tablewidget
     */
    void InitializeMembershipTableWidget();

    /*!
     * \brief MainWindow::PrintUpgradeReport : Function will populate membership
     * table view to be shown to user in "Recommend Upgrades" feature
     * \param regularMemberPurchases : Vector of regular members' purchases
     */
    void PrintUpgradeReport(QVector<Database::Member> regularMemberPurchases);

    /*!
     * \brief MainWindow::PrintDowngradeReport : Function will populate membership
     * table view to be shown to user in "Recommend Downgrades" feature
     * \param executiveMemberPurchases : Vector of executive members' purchases
     */
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
