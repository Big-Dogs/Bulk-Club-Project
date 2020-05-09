#ifndef MONEYDELEGATE_H
#define MONEYDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class MoneyDelegate : public QStyledItemDelegate
{
public:
    /*!
     * \brief MoneyDelegate
     *      A defualt constructor for the MoneyDelegate class. It does nothing but call
     *      the constructor for QStyledItemDelegate using parent
     *
     * \param parent - A pointer the parent of the delagate
     */
    MoneyDelegate(QObject *parent = nullptr);

    /*!
     * \brief paint
     *      The overwritten paint function, used to change the formatting of the text it
     *      is painting.
     *
     * Precondition
     *      This delegate should only be used on table items displaying some form of money
     *
     * Postcondition
     *      The money amount is rendered to 2 decimal places and a dollar sign before the number
     *
     * \param painter - The painter that renders the data stores in index
     *
     * \param option - The QStyleOptionViewItem that helps set up and format the rendering, such
     *                 picking the correct spot on the screen to render the text
     *
     * \param index - The QModelIndex of the item being rendering in the table view
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // MONEYDELEGATE_H
