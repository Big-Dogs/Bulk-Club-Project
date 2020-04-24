#ifndef SALESTABLEMODEL_H
#define SALESTABLEMODEL_H

#include "Database.h"
#include <QSqlTableModel>
#include <QTableWidgetItem>
#include <QObject>

class SalesTableModel : public QSqlTableModel
{
public:
    enum TableFields
    {
        MEMBER_ID,
        PRODUCT_ID,
        DATE_PURCHASED,
        QTY,
    };

    enum Days
    {
        TWELFTH,
        THIRTEENTH,
        FOURTEENTH,
        FIFTEENTH,
        SIXTEENTH,
        SEVENTEENTH,
        EIGHTEENTH,
    };

    SalesTableModel(QObject *parent, Database *db);

    void InitializeSalesTable();
};


#endif // SALESTABLEMODEL_H
