#include "moneydelegate.h"
#include <QDebug>
#include <QRect>
#include <QPoint>

// Constructor
MoneyDelegate::MoneyDelegate(QObject *parent)
              :QStyledItemDelegate(parent) {}

// Overwritten paint function that changes the format of the data that is rendered
void MoneyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    double  value; // Value being displayed in the table
    QString text;  // Text will be displayed in the table

    // Retrieve value from the table
    value = index.model()->data(index, Qt::DisplayRole).toDouble();

    text = "$ ";
    text.append(QString::number(value, 'f', 2));

    // "Translated" is used here to print the data in the middle of its item vertically
    painter->drawText(option.rect.translated(0, 5), text);
}
