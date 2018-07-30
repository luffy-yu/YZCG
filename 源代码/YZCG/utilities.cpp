#include "utilities.h"

void Utilities::setTableAttributes(QTableWidget *table)
{
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    table->setAlternatingRowColors(true);
    for(int i = 0;i < table->rowCount();i++){
        for(int j = 0;j < table->columnCount();j++){
            table->setItem(i,j,new QTableWidgetItem(""));
            //table->item(i,j)->setData(Qt::DisplayRole,"");
        }
    }
}

QStringList Utilities::tableData2StringList(QTableWidget *table)
{
    QStringList list;
    for(int i = 0;i < table->rowCount();i++)
    {
        for(int j = 0;j < table->columnCount();j++)
        {
            list.append(table->item(i,j)->text());
        }
    }
    return list;
}

void Utilities::fillTableItem(QTableWidget *table, int row, int col)
{
    table->setRowCount(row);
    table->setColumnCount(col);

    for(int i = 0;i < row;i++){
        for(int j = 0;j < col;j++){
            if(!table->item(i,j)){
                table->setItem(i,j,new QTableWidgetItem(""));
            }
        }
    }
}
