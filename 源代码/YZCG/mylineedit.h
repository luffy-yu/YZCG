#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 自定义输入框
  */

#include "Headers.h"

class MyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit MyLineEdit(QWidget *parent = 0);
    /**
     * @brief MyLineEdit
     * @param parent 父对象
     * @param objName 对象名
     * @param enabled 是否可用
     */
    explicit MyLineEdit(QWidget *parent,QString objName,bool enabled = true);
    
signals:
    /**
     * @brief requestData 请求数据
     * @param name 对象名
     * @param row 行数
     * @param col 列数
     */
    void requestData(QString name,int row,int col);
public slots:
    /**
     * @brief chooseDataSlot 选择数据
     */
    void chooseDataSlot();
//    /**
//     * @brief inputSlot 手动输入
//     */
//    void inputSlot();
protected:
    /**
     * @brief focusInEvent 获得焦点事件
     * @param event
     */
    void focusInEvent(QFocusEvent *event);
    /**
     * @brief mouseDoubleClickEvent 鼠标双击事件
     * @param event
     */
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // MYLINEEDIT_H
