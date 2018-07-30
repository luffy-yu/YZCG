#ifndef ABSOLUTEORIENTATIONWIDGET_H
#define ABSOLUTEORIENTATIONWIDGET_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 绝对定向的界面
  */

#include "Headers.h"
#include "mylineedit.h"
#include "utilities.h"
#include "algorithms.h"

class AbsoluteOrientationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AbsoluteOrientationWidget(QWidget *parent = 0);
    
signals:
    /**
     * @brief requestData 请求数据
     * @param parent 父窗口
     * @param requester 请求者对象名
     * @param row 数据行数
     * @param col 数据列数
     */
    void requestData(QWidget *parent,QString requester,int row,int col);
public slots:
    /**
     * @brief requestDataSlot 请求数据
     * @param requester 请求者对象名
     * @param row 数据行数
     * @param col 数据列数
     */
    void requestDataSlot(QString requester,int row,int col);
    /**
     * @brief replyDataSlot 返回数据
     * @param parent 父窗口
     * @param target 目标对象名
     * @param list 字符串
     * @param row 行数
     * @param col 列数
     */
    void replyDataSlot(QWidget *parent,QString target,QStringList list,int row,int col);
    /**
     * @brief chooseMDBtnSlot 选择模型数据
     */
    void chooseMDBtnSlot();
    /**
     * @brief chooseGDBtnslot 选择地面数据
     */
    void chooseGDBtnslot();
    /**
     * @brief calculateBtnSlot 计算
     */
    void calculateBtnSlot();
protected:
    /**
     * @brief initUI 初始化界面
     */
    void initUI();
private:
    QLabel *pLabel;//精度
    MyLineEdit *pLineEdit;
    QPushButton *chooseMDBtn;//MD : model data 模型数据
    QPushButton *chooseGDBtn;//GD : ground data 地面测量数据
    QTableWidget *MDTable;
    QTableWidget *GDTable;

    QPushButton *calculateBtn;

    QTableWidget *absoluteTable;//绝对定向元素
    QTableWidget *resultTable;//绝对定向结果

    QStringList MDTableHeader;
    QStringList GDTableHeader;
    QStringList absoluteHeader;
    QStringList resultHeader;
    QVBoxLayout *mainLayout;
};

#endif // ABSOLUTEORIENTATIONWIDGET_H
