#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <QSqlDatabase>

class Database : public QSqlDatabase
{
public:
    // Constructor
    Database(QString path, QString driver);

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


    // PUT ALL YALL'S QUERIES DOWN HERE. IF IT'S PURELY A TABLEVIEW QUERY TO
    // DISPLAY DATA THEN YOU DONT NEED TO RUN QUERIES HERE

    // Destructor
    ~Database();
};

#endif // DATABASE_H
