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

    void setPermissions(int);

private slots:

    /*----Testing Permissions----*/
    void on_pushButton_employeepermissions_clicked();

    void on_pushButton_managerpermissions_clicked();

    void on_pushButton_adminpermissions_clicked();


    /*----Menu Navigation----*/

        /*----POS page----*/
    void on_pushButton_POS_clicked();

        void on_pushButton_pos_purchase_clicked();

        /*----Home page----*/
    void on_pushButton_home_clicked();

        /*----Sales page----*/
    void on_pushButton_sales_clicked();

        void on_pushButton_sales_daily_clicked();

        void on_pushButton_sales_sortmember_clicked();

        void on_pushButton_sales_sortitem_clicked();

        void on_pushButton_sales_searchmember_clicked();

            void on_pushButton_sales_searchmemberconfirm_clicked();

        void on_pushButton_sales_searchitem_clicked();

            void on_pushButton_sales_searchitemconfirm_clicked();

         /*----Membership page----*/
    void on_pushButton_members_clicked();

        void on_pushButton_membership_rebates_clicked();

        void on_pushButton_membership_expiration_clicked();

        void on_pushButton_membership_upgrades_clicked();

        void on_pushButton_membership_downgrades_clicked();

        /*----Admin page----*/
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

        void on_pushButton_admin_inventory_clicked();

            void on_pushButton_admin_additem_clicked();

            void on_pushButton_admin_edititem_clicked();

            void on_pushButton_admin_deleteitem_clicked();

            void on_pushButton_admin_itemsubmission_submit_clicked();

            void on_pushButton_admin_itemsubmission_cancel_clicked();

            void on_pushButton_admin_confirmdeleteitem_clicked();

            void on_pushButton_admin_canceldeleteitem_clicked();

            void on_tableView_admin_inventory_doubleClicked(const QModelIndex &index);





            void on_pushButton_sale_byday_clicked();

            void on_pushButton_membership_expire_clicked();

            void ClearMemberFields();
    // Autocomplete text searches
    void TextCompleter(QStringList products, QLineEdit *inputField);

    void on_pushButton_home_login_clicked();

    void on_comboBox_pos_itemlist_activated(int index);

    void on_comboBox_pos_qty_activated(int index);

    void on_comboBox_pos_memberlist_activated(int index);

private:
    Ui::MainWindow *ui;
    int index = 0; // Testing Permissions
    MembershipTableModel *membershipModel; // Membership Table View Configuration
    Database *database; // Pointer to database

    MoneyDelegate *formatPrice; //A pointer to the delegate used to format
                                //items in table for money

    // For use in 'Search Sales by Product Name' feature
    QStringList productList; // List of all products in database
    QString salesReportProduct; // Product manager wishes to view for sales report


    //For use in 'Display Member Rebates' feature
    //this is a comment
    struct ExecutiveMemberRebate
    {
        QString memberID;
        QString name;
        QString amountSpent;
        QString rebate;
    };


    //For use in 'Add Customer' feature
    struct TempMember
    {
        QString id;
        QString name;
        QString executiveStatus;
        QString expirationDate;
    };


    // For use in upgrade/downgrade features
    float rebateAmount = 0.0; // member's rebate received
    int downgradeCount = 0; // number of downgrade recommendations on QStringList
    int upgradeCount = 0; // number of downgrade recommendations on QStringList
    const int TABLE_WIDGET_COLS = 4; // number of columns on tablewidget
    const float REBATE_RATE = 0.02;      // rebate rate for calculation
    const float REBATE_MIN = 65.0;       // minimum rebate needed for exec member
    QStringList tableWidgetColumnNames = {
        "Membership Number",
        "Member Name",
        "Amount Spent",
        "Rebate Amount"
    };


    // For use in POS page
    QString posItemName;
    QString posDate;
    int posMemberID;
    int posItem;
    int posQty;
    double posPrice;
    double posTotal;
    int receiptRow;


    // Enum to keep track of all program pages
    enum Pages
    {
        HOME,
        POS,
        SALES,
        MEMBER,
        ADMIN,
    };

    // Enum to keep track of manager reports pages
    enum SalesPages
    {
        SALES_DAILY,
        SALES_SORT_MEMBER,
        SALES_SORT_ITEM,
        SALES_SEARCH_MEMBER,
        SALES_SEARCH_ITEM,
    };

    // Enum to keep track of administrator interface pages
    enum AdminPages
    {
        ADMIN_MEMBER,
        ADMIN_ITEM,
    };

    // Enum to keep track of upgrade/downgrade feature columns
    enum MembershipTableWidgetColumns
    {
        MEMBERSHIP_NUMBER,
        MEMBER_NAME,
        AMT_SPENT,
        REBATE_AMOUNT
    };




    // Helper Function Prototypes

        // Reset all values in membership table widget
    void InitializeMembershipTableWidget();

        // Print suggested upgrades report
    void PrintUpgradeReport(QVector<Database::Member> regularMemberPurchases);

        // Print suggested downgrades report
    void PrintDowngradeReport(QVector<Database::Member> executiveMemberPurchases);

        //initialize pos table
    void InitializePosTable();

        //prints a receipt in pos page
    void printReceipt();



};
#endif // MAINWINDOW_H
