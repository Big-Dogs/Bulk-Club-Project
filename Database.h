#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <QSqlDatabase>
#include <QVector>


/*!
 * \brief The Database class is a wrapper for QSqlDatabase
 */
class Database : public QSqlDatabase
{
public:

    // Member information Struct
    /*!
     * \struct Member
     * \brief Object to hold member purchase information. For use in
     * 'Recommend Upgrades/Downgrades' features
     */
    struct Member // Object to hold member purchase information
    {
        QString memberID;
        QString name;
        QString amountSpent;
    };

    /*!
     * \brief Database - Constructor
     * \param path Path to SQL Database file
     * \param driver QString identifier for the specific flavor of SQL
     * we are using.
     */
    Database(QString path, QString driver);

    /*!
     * \brief findDatabase : Method used for finding the database file
     * \param fileName : a string representing the name of database file to be
     * found
     * \return
     */
    static QString findDatabase(QString fileName);

    /*!
     * \brief GetRegularMemberIDList : Method for returning a list of member
     * IDs of type 'Regular'
     * \return regularIDList : A QStringList of member IDs
     */
    QStringList GetRegularMemberIDList();

    /*!
     * \brief GetRegularMemberPurchases : Method for returning member purchases
     * of 'Regular' members
     * \param regularIDList : A QStringList of member IDs
     * \return regularPurchaseAr : A vector of 'Regular'-type members' purchases
     */
    QVector<Member> GetRegularMemberPurchases(QStringList regularIDList);

    /*!
     * \brief GetExecutiveMemberIDList : Method for returning a list of member
     * IDs of type 'Executive'
     * \return executiveIDList : A QStringList of member ID's
     */
    QStringList GetExecutiveMemberIDList();

    /*!
     * \brief GetExecutiveMemberPurchases
     * \param executiveIDList : A QStringList of member IDs
     * \return executivePurchaseAr : A vector 'Executive'-type members'
     * purchases
     */
    QVector<Member> GetExecutiveMemberPurchases(QStringList executiveIDList);



    // ---------- START SUGGESTED PROTOTYPES (by weston) ---------- //

    // Add Member
    bool AddCustomer(QString name, QString isExecutive, QString expireDate);

    // Edit Member Data
    bool UpdateCustomer(QString name, QString isExecutive, QString expireDate);

    // Remove Member
    bool DeleteCustomer(QString memberID);

    // Add Product
    bool AddProduct(QString name, QString price);

    // Edit Product Data
    bool UpdateProduct(QString name, QString price);

    // Remove Product
    bool DeleteProduct(QString productID);

    // ---------- END SUGGESTED PROTOTYPES (by weston) ---------- //

    // ---------- START PROTOTYPES (by noah) ---------- //
    double getPrice(QString item);

    int getItem(QString item);

    QStringList getNames();

    QStringList getPOSMembers();

    void addPurchase(int memberID, int productID, QString datePurchased, int qty);
   
    int checkLogin(QString username, QString password);
    // ---------- END PROTOTYPES (by noah) ---------- //

    ~Database();

private:
    Member temp; /// placeholder for Member-type operations
};

#endif // DATABASE_H
