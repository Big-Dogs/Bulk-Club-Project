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

QString Database::findDatabase(QString fileName)
{
    QDir    databaseSearch; //The QDir object used to file the database file
    QString databasePath;   //The path of the database file, equals NOT FOUND
                            //if database file could not be found

    databaseSearch = QDir::currentPath();

    databaseSearch.cdUp();

    bool projectDirectoryFound = databaseSearch.cd("Bulk-Club-Project");

    if (projectDirectoryFound)
    {
        if (databaseSearch.exists(fileName))
        {
            databasePath = databaseSearch.filePath(fileName);
        }
        else
        {
            databasePath = "NOT FOUND";

            qDebug() << "Error: The database file could not be found.";
        }
    }
    else
    {
        databasePath = "NOT FOUND";

        qDebug() << "Error: The project directory could not be found.";
    }

    return databasePath;
}


// Get list of regular member IDs
QStringList Database::GetRegularMemberIDList()
{
    QStringList regularMemberIDList;
    int index = 0;
    QSqlQuery query;
    query.prepare("select memberID from members where memberType='Regular'");

    // Execute Query
    if(query.exec())
    {
        // iterate through and pull ids
        while(query.next())
        {
            regularMemberIDList.insert(index, query.value(0).toString());
            index++;
        }

        // DEBUG: print list
        for(int index = 0; index < regularMemberIDList.size(); index++)
        {
            qDebug() << regularMemberIDList[index];
        }
    }
    else // if unsuccessful, print error
    {
        qDebug() << query.lastError().text();
    }

    return regularMemberIDList;
}

// Get vector of member purchase listings
QVector<Database::Member> Database::GetRegularMemberPurchases(QStringList regularIDList)
{
    QSqlQuery query;
    QVector<Member> regularPurchaseAr;
    Member temp;

    // use executiveMemberID to pull purchase data from db into vector
    query.prepare("SELECT members.memberID, members.name, sum(purchases.qty * products.price) "
                  "FROM members, purchases, products "
                  "WHERE members.memberID = purchases.memberID "
                  "AND purchases.productID = products.productID "
                  "AND members.memberID = :memberID");

    // If vector empty
    if(regularPurchaseAr.empty())
    {
        // Iterate through ID list, calling each member's purchases
        for(int index = 0; index < regularIDList.size(); index++)
        {
            query.bindValue(":memberID", regularIDList[index]);

            // Execute Query
            if(query.exec())
            {
                // Iterate through query data and pull purchase information into vector
                while(query.next())
                {
                    if(query.value(0).toString() != "")
                    {
                        // Copy into temp object
                        temp.memberID = query.value(0).toString();
                        temp.name = query.value(1).toString();
                        temp.amountSpent = query.value(2).toString();

                        // Add object to vector
                        regularPurchaseAr.append(temp);
                    }
                }
            }
            else // if unsuccessful, print error
            {
                qDebug() << query.lastError().text();
            }
        }
    }

   // Print entire vector
   for(int index = 0; index < regularPurchaseAr.count(); index++)
   {
       qDebug() << "PRINTING PERSON #" << index + 1;
       qDebug() << regularPurchaseAr[index].memberID;
       qDebug() << regularPurchaseAr[index].name;
       qDebug() << regularPurchaseAr[index].amountSpent;
   }

   return regularPurchaseAr;
}

// Get list of executive member IDs
QStringList Database::GetExecutiveMemberIDList()
{
    QSqlQuery query;
    QStringList executiveIDList;
    int index = 0;

    query.prepare("select memberID from members where memberType='Executive'");

    // Execute Query
    if(query.exec())
    {
        // iterate through and pull ids
        while(query.next())
        {
            executiveIDList.insert(index, query.value(0).toString());
            index++;
        }

        qDebug() << "PRINTING EXECUTIVE MEMBER ID LIST: \n";
        // DEBUG: print list
        for(int index = 0; index < executiveIDList.size(); index++)
        {
            qDebug() << "EXECUTIVE ID #" << index + 1 << ": "
                     << executiveIDList[index];
        }
    }
    else // if unsuccessful, print error
    {
        qDebug() << query.lastError().text();
    }

    return executiveIDList;
}

// Get vector of member purchase listings
QVector<Database::Member> Database::GetExecutiveMemberPurchases(QStringList executiveIDList)
{
    QSqlQuery query;
    QVector<Member> executivePurchaseAr;
    Member temp;

    // use executiveMemberID to pull purchase data from db into vector
    query.prepare("SELECT members.memberID, members.name, sum(purchases.qty * products.price) "
                  "FROM members, purchases, products "
                  "WHERE members.memberID = purchases.memberID "
                  "AND purchases.productID = products.productID "
                  "AND members.memberID = :memberID"
                  );

    // Iterate through ID list, calling each member's purchases
    for(int index = 0; index < executiveIDList.size(); index++)
    {
        query.bindValue(":memberID", executiveIDList[index]);

        // Execute Query
        if(query.exec())
        {
            // Iterate through query data and pull purchase information into vector
            while(query.next())
            {
                if(query.value(0).toString() != "")
                {
                    // Copy into temp object
                    temp.memberID = query.value(0).toString();
                    temp.name = query.value(1).toString();
                    temp.amountSpent = query.value(2).toString();

                    // Add object to vector
                    executivePurchaseAr.append(temp);
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
                temp.memberID = query.value(0).toString();
                temp.name = query.value(1).toString();
                temp.amountSpent = query.value(2).toString();

                // Add object to vector
                executivePurchaseAr.append(temp);
            }
        }
    }
    else // if unsuccessful, print error
    {
        qDebug() << query.lastError().text();
    }

   // Print entire vector
   for(int index = 0; index < executivePurchaseAr.count(); index++)
   {
       qDebug() << "PRINTING PERSON #" << index + 1;
       qDebug() << executivePurchaseAr[index].memberID;
       qDebug() << executivePurchaseAr[index].name;
       qDebug() << executivePurchaseAr[index].amountSpent;
   }

   return executivePurchaseAr;
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

double Database::getPrice(int item)
{
    double purchaseAmt = 0;

    QSqlQuery query;

    QString thisItem = QString::number(item);
    //finds item being purchased
       //query database to get price of selected item
    query.prepare("select price from products where productId = '" + thisItem + "'");
    //if it does match
        //return price of item
    if(query.exec())
    {

        while(query.next())
        {
            purchaseAmt = query.value(0).toDouble();
        }

        qDebug() << "item price: " << purchaseAmt;
    }
    //else
         //display errormessage
    else // if unsuccessful, print error
    {
        qDebug() << "no match found";
    }

    return purchaseAmt;
}

QStringList Database::getNames()
{
    QStringList itemNames;
    int index = 0;

    QSqlQuery query;

    //retrieves item names
       //query database to match item name to item number
    query.prepare("select name from products");
    //if it does match
        //return names
    if(query.exec())
    {
        while(query.next())
        {
            itemNames.insert(index, query.value(0).toString());
            index++;
        }
    }
    //else
         //display errormessage
    else // if unsuccessful, print error
    {
        qDebug() << "no match found";
    }

    return itemNames;
}

QStringList Database::getPOSMembers()
{
    QStringList memberNames;
    int index = 0;

    QSqlQuery query;

    //retrieves item names
       //query database to match item name to item number
    query.prepare("select memberID from members");
    //if it does match
        //return names
    if(query.exec())
    {
        while(query.next())
        {
            memberNames.insert(index, query.value(0).toString());
            index++;
        }
    }
    //else
         //display errormessage
    else // if unsuccessful, print error
    {
        qDebug() << "no match found";
    }

    return memberNames;
}

void Database::addPurchase(int memberID, int productID, QString datePurchased, int qty)
{

    QSqlQuery query;

    query.prepare("INSERT INTO purchases "
                  "(memberID, productID, "
                  "datePurchased, qty)"
                  "VALUES(?,?,?,?)");

    query.addBindValue(memberID);
    query.addBindValue(productID);
    query.addBindValue(datePurchased);
    query.addBindValue(qty);

    if(!query.exec())
        qDebug() << "Purchase could not be added.";
}

// Destructor
Database::~Database() {}
