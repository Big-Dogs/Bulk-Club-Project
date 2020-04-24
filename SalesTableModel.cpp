#include "SalesTableModel.h"


SalesTableModel::SalesTableModel(QObject *parent, Database *db)
                    : QSqlTableModel(parent, *db) {}

void SalesTableModel::InitializeSalesTable()
{
    // Select 'purchases' table
    this->setTable("purchases");

    // Configure tableView columns
    this->setHeaderData(MEMBER_ID, Qt::Horizontal, tr("Member ID"));
    this->setHeaderData(PRODUCT_ID, Qt::Horizontal, tr("Product No."));
    this->setHeaderData(DATE_PURCHASED, Qt::Horizontal, tr("Date"));
    this->setHeaderData(QTY, Qt::Horizontal, tr("Qty"));

    this->select();
}
