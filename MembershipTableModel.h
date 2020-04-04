#ifndef MEMBERSHIPTABLEMODEL_H
#define MEMBERSHIPTABLEMODEL_H

#include "Database.h"
#include <QSqlTableModel>
#include <QTableWidgetItem>
#include <QObject>

class MembershipTableModel : public QSqlTableModel
{
public:
    enum TableFields
    {
        ID,
        NAME,
        MEMBERSHIP_TYPE,
        EXPIRE_DATE,
        RENEWAL_COST,
    };

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

    MembershipTableModel(QObject *parent, Database *db);

    void InitializeTable();
};

#endif // MEMBERSHIPTABLEMODEL_H
