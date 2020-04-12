#ifndef MONEYDELEGATE_H
#define MONEYDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class MoneyDelegate : public QStyledItemDelegate
{
public:
    MoneyDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // MONEYDELEGATE_H
