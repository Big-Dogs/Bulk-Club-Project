#include "Database.h"
#include <QtDebug>

// Constructor
Database::Database(QString path, QString driver)
        :QSqlDatabase(addDatabase(driver))
{
    // Set Hostname to Local
    setHostName("localhost");
    setDatabaseName(path);

    // Check if open
    if(open())
    {
        qDebug() << "Database opened successfully";
    }
    else
    {
        qDebug() << this->lastError().text();
    }
}

// Add Member
bool Database::AddCustomer(QString name, QString isExecutive, QString expireDate) { return false; }

// Edit Member Data
bool Database::UpdateCustomer(QString name, QString isExecutive, QString expireDate) { return false; }

// Remove Member
bool Database::DeleteCustomer(QString memberID) { return false; }

// Add Product
bool Database::AddProduct(QString name, QString price) { return false; }

// Edit Product Data
bool Database::UpdateProduct(QString name, QString price) { return false; }

// Remove Product
bool Database::DeleteProduct(QString productID) { return false; }

// PUT ALL YALL'S QUERIES DOWN HERE. IF IT'S PURELY A TABLEVIEW QUERY TO
// DISPLAY DATA THEN YOU DONT NEED TO RUN QUERIES HERE

// Destructor
Database::~Database() {}
