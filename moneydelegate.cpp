#include "moneydelegate.h"
#include <QDebug>
#include <QRect>
#include <QPoint>

MoneyDelegate::MoneyDelegate(QObject *parent)
              :QStyledItemDelegate(parent)
{
    //nothing special in constructor
}


void MoneyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    double  value; //The value being displayed in the table
    QString text;  //The text will be displayed in the table

    //retrieving value from the table
    value = index.model()->data(index, Qt::DisplayRole).toDouble();

    text = "$ ";
    text.append(QString::number(value, 'f', 2));

    //translated is used here to print the data in the middle of
    //its item veritically
    painter->drawText(option.rect.translated(0, 5), text);
}
