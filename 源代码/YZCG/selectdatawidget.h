#ifndef SELECTDATAWIDGET_H
#define SELECTDATAWIDGET_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 选择数据的界面
  */

#include "Headers.h"

class SelectDataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SelectDataWidget(QWidget *parent = 0);
    /**
     * @brief setListViewModel 设置Model
     * @param model
     */
    void setListViewModel(QStandardItemModel *model);
signals:
    /**
     * @brief replyData 返回数据
     * @param parent 父对象
     * @param target 目标对象名
     * @param list 字符串
     * @param row 行数
     * @param col 列数
     */
    void replyData(QWidget *parent,QString target,QStringList list,int row,int col);
public slots:
    /**
     * @brief listViewClicked 点击事件
     * @param index
     */
    void listViewClicked(QModelIndex index);
    /**
     * @brief listViewDoubleClicked 双击事件
     * @param index
     */
    void listViewDoubleClicked(QModelIndex index);
    /**
     * @brief oneviewClicked 单选视图点击事件
     * @param index
     */
    void oneviewClicked(QModelIndex index);
    /**
     * @brief requestDataSlot 请求数据
     * @param parent 父对象
     * @param requester 请求者对象名
     * @param row 行数
     * @param col 列数
     */
    void requestDataSlot(QWidget *parent,QString requester,int row,int col);
private:
    //list列表
    QListView *listView;
    //单个数据列表
    QListView *oneView;
    QVBoxLayout *layout;

    QStringList listData;
    QStringListModel *oneViewModel;
    //
    QWidget *requesterParent;
    QString requesterName;
    int requestRow;
    int requestCol;
};

#endif // SELECTDATAWIDGET_H
