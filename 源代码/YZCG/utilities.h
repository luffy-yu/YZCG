#ifndef UTILITIES_H
#define UTILITIES_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 工具类
  */

#ifndef QTABLEWIDGET_H
#include <QTableWidget>
#endif

#ifndef QHEADERVIEW_H
#include <QHeaderView>
#endif

#ifdef QT_DEBUG
#include <QDebug>
#endif

class Utilities
{
public:
    /**
     * @brief setTableAttributes 设置表格属性
     * @param table
     */
    void static setTableAttributes(QTableWidget *table);
    /**
     * @brief tableData2StringList 表格数据转换为stringList
     * @param table
     * @return
     */
    QStringList static tableData2StringList(QTableWidget *table);
    /**
     * @brief fillTableItem 用""填充table的单元格
     * @param table
     * @param row 目标行数
     * @param col 目标列数
     */
    void static fillTableItem(QTableWidget *table,int row,int col);
};

#endif // UTILITIES_H
