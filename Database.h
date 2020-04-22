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

    // Destructor
    ~Database();

private:
    Member temp;
};

#endif // DATABASE_H
