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

    // ---------- START PROTOTYPES (by noah) ---------- //
    /*!
     * \brief GetPrice : Method for returning the cost of a product
     * in the database
     * \param item : A QString holding the product name
     * \return itemPrice : A double containing the cost of the product
     */
    double GetPrice(QString item);

    /*!
     * \brief GetItem : Method for returning the product ID of a product
     * in the database
     * \param item : A QString holding the product name
     * \return ProductID : An integer containing product ID
     */
    int GetItem(QString item);

    /*!
     * \brief GetNames : Method for returning a list of product names
     * \return itemNames : A QStringList of product names
     */
    QStringList GetNames();

    /*!
     * \brief GetPOSMembers : Method for returning a list of member IDs
     * \return memberIDs : A QStringList of member IDs
     */
    QStringList GetPOSMembers();

    /*!
     * \brief AddPurchase : Method for adding a purchase to the database
     * \param memberID : An integer containing the ID of the
     * member making the purchase
     * \param productID : An integer containing the ID of the
     * product being purchased
     * \param datePurchased : A QString containing the current date
     * \param qty : An int containing the number of items being purchased
     */
    void AddPurchase(int memberID, int productID, QString datePurchased, int qty);
   
    /*!
     * \brief CheckLogin : Method for comparing user login to the database
     * \param username : A QString holding the entered username
     * \param password : A QString holding the entered password
     * \return permissionLevel : An integer signifying the user's credentials
     */
    int CheckLogin(QString username, QString password);
    // ---------- END PROTOTYPES (by noah) ---------- //

    /*!
     * \brief ~Database() Destructor
     */
    ~Database();

private:
    Member temp; /// placeholder for Member-type operations
};

#endif // DATABASE_H
