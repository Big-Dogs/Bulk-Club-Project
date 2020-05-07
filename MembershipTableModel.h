#ifndef MEMBERSHIPTABLEMODEL_H
#define MEMBERSHIPTABLEMODEL_H

#include "Database.h"
#include <QSqlTableModel>
#include <QTableWidgetItem>
#include <QObject>

/*!
 * \brief MembershipTableModel class used to configure tableview
 * used for membership table
 */
class MembershipTableModel : public QSqlTableModel
{
public:
    /*!
     * \brief TableFields enum is used to track columns on membership table
     */
    enum TableFields
    {
        ID,
        NAME,
        MEMBERSHIP_TYPE,
        EXPIRE_DATE,
        RENEWAL_COST,
    };
    /*!
     * \brief Months enum is used to keep track of calendar months. To be used
     * by 'Expiring memberships' feature
     */
    enum Months
    {
        JANUARY,
        FEBRUARY,
        MARCH,
        APRIL,
        MAY,
        JUNE,
        JULY,
        AUGUST,
        SEPTEMBER,
        OCTOBER,
        NOVEMBER,
        DECEMBER,
    };

    /*!
     * \brief MembershipTableModel Constructor
     * \param parent : pointer to object from which this object is inherited
     * \param db : pointer to the database used in instantiation
     */
    MembershipTableModel(QObject *parent, Database *db);

    /*!
     * \brief InitializeTable method is used to reset and initialize default
     * values used for membership table view
     */
    void InitializeTable();
};

#endif // MEMBERSHIPTABLEMODEL_H
