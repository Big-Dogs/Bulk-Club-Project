#include "MembershipTableModel.h"


MembershipTableModel::MembershipTableModel(QObject *parent, Database *db)
                    : QSqlTableModel(parent, *db) {}

void MembershipTableModel::InitializeTable()
{
    // Select 'members' table
    this->setTable("members");

    // Configure tableView columns
    this->setHeaderData(ID, Qt::Horizontal, tr("Member ID"));
    this->setHeaderData(NAME, Qt::Horizontal, tr("Member Name"));
    this->setHeaderData(EXPIRE_DATE, Qt::Horizontal, tr("Expiration Date"));
    this->setHeaderData(MEMBERSHIP_TYPE, Qt::Horizontal, tr("Membership Type"));
    this->setHeaderData(RENEWAL_COST, Qt::Horizontal, tr("Renewal Cost"));

    this->select();
}
