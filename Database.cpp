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

// Finds database in filesystem
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
    QStringList regularMemberIDList; // List of member ID's
    int index = 0; // Index for list traversal
    QSqlQuery query; // Query to gather data
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
    QSqlQuery query; // Query to gather data
    QVector<Member> regularPurchaseAr; // Array of 'regular' member purchases
    Member temp; // Temp object used for copying

    // Use regularMemberID to pull purchase data from db into vector
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
    QSqlQuery query; // Query to gather data
    QStringList executiveIDList; // List of executive member ID's
    int index = 0; // Index for traversing list

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

// Get vector of executive member purchase listings
QVector<Database::Member> Database::GetExecutiveMemberPurchases(QStringList executiveIDList)
{
    QSqlQuery query; // Query to gathering data
    QVector<Member> executivePurchaseAr; // Array of 'executive' member purchases
    Member temp; // Temporary object used for copying

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

// Gets price of item requested
double Database::GetPrice(QString item)
{
    double itemPrice = 0; // Price of item

    QSqlQuery query; // Query to gather data


    // Finds item being purchased
       // Query database to get price of selected item
    query.prepare("select price from products where name = ?");
    // If it does match
        // Return price of item
    query.bindValue(0, item);
    if(query.exec())
    {
        while(query.next())
        {
            itemPrice = query.value(0).toDouble();
        }

        qDebug() << "item price: " << itemPrice;
    }
    // Else
         // Display errormessage
    else // If unsuccessful, print error
    {
        qDebug() << "no match found";
    }

    return itemPrice;
}

// Get Product ID of product
int Database::GetItem(QString item)
{
    int productID = 0; // ID of product requested

    QSqlQuery query; // Query to gather data

    // Finds item being purchased
       // Query database to get product id of selected item
    query.prepare("select productID from products where name = ?");
    // If it does match
        // Return price of item
    query.bindValue(0, item);

    if(query.exec())
    {
        while(query.next())
        {
            productID = query.value(0).toInt();
        }

        qDebug() << "item price: " << productID;
    }
    // Else
         // Display errormessage
    else //  If unsuccessful, print error
    {
        qDebug() << "no match found";
    }

    return productID;
}

// Get list of product names
QStringList Database::GetNames()
{
    QStringList itemNames; // List of product names
    int index = 0; // Index for traversing list
    QSqlQuery query; // Query to gather data

    // Retrieves item names
       // Query database to match item name to item number
    query.prepare("select name from products");
    // If it does match
        // Return names
    if(query.exec())
    {
        while(query.next())
        {
            itemNames.insert(index, query.value(0).toString());
            index++;
        }
    }
    // Else
         // Display errormessage
    else // If unsuccessful, print error
    {
        qDebug() << "no match found";
    }

    return itemNames;
}

// Get a list of member IDs
QStringList Database::GetPOSMembers()
{
    QStringList memberIDs; // List of member ID's
    int index = 0; // Index for traversing list
    QSqlQuery query; // Query to gather data

    // Retrieves member IDs
       // Query database to get member IDs
    query.prepare("select memberID from members");
    // If it does match
        // Return names
    if(query.exec())
    {
        while(query.next())
        {
            memberIDs.insert(index, query.value(0).toString());
            index++;
        }
    }
    // Else
         // Display errormessage
    else // If unsuccessful, print error
    {
        qDebug() << "no match found";
    }

    return memberIDs;
}

// Adds a purchase to the database
void Database::AddPurchase(int memberID, int productID, QString datePurchased, int qty)
{
    QSqlQuery query; // Query to gather data

    //Add a purchase
       // Prepare database to insert a purchase into the
       // Purchases table
    query.prepare("INSERT INTO purchases "
                  "(memberID, productID, "
                  "datePurchased, qty)"
                  "VALUES(?,?,?,?)");

    // Insert into purchases table
    query.addBindValue(memberID);
    query.addBindValue(productID);
    query.addBindValue(datePurchased);
    query.addBindValue(qty);

    // Else
         // Display errormessage
    if(!query.exec())
        qDebug() << "Purchase could not be added.";
}

// Validates user credentials
int Database::CheckLogin(QString username, QString password)
{
    int permissionLevel = 0; // Initial user permission level
    QSqlQuery query; // Query to gather data

    // Checks if username and password are valid
       // Query database to see if username and password match any items
    query.prepare("select isAdmin, isManager from users where username = '"+username+"' and password = '"+password+"'");

    // If it does match
        // Verify the permission level of valid user
        // Set permission level
    if(query.exec())
    {
        while(query.next())
        {
            // Add 1 for customer permissions
            permissionLevel = query.value(0).toInt() + query.value(1).toInt() + 1;
        }

        qDebug() << " permission level: " << permissionLevel;
    }
    // Else
         // Display errormessage
    else // If unsuccessful, print error
    {
        qDebug() << "no match found";
    }

    return permissionLevel;
}

// Destructor
Database::~Database() {}
