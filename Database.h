#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <QSqlDatabase>
#include <QVector>

class Database : public QSqlDatabase
{
public:

    // For use in 'Recommend Upgrades/Downgrades' features
    struct Member // Object to hold member purchase information
    {
        QString memberID;
        QString name;
        QString amountSpent;
    };

    // Constructor
    Database(QString path, QString driver);

    // Finding the database file
    static QString findDatabase(QString fileName);

    // Get list of regular member IDs
    QStringList GetRegularMemberIDList();

    // Get vector of member purchase listings
    QVector<Member> GetRegularMemberPurchases(QStringList regularIDList);

    // Get list of executive member IDs
    QStringList GetExecutiveMemberIDList();

    // Get vector of member purchase listings
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

    // Destructor
    ~Database();

private:
    Member temp;
};

#endif // DATABASE_H
