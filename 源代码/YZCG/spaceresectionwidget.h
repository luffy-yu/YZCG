#ifndef SPACERESECTIONWIDGET_H
#define SPACERESECTIONWIDGET_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 空间后方交会界面
  */

#include "Headers.h"
#include "mylineedit.h"
#include "utilities.h"
#include "algorithms.h"

class SpaceResectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpaceResectionWidget(QWidget *parent = 0);
    
signals:
    /**
     * @brief requestData 请求数据
     * @param parent 父对象
     * @param requester 请求者对象名
     * @param row 行数
     * @param col 列数
     */
    void requestData(QWidget *parent,QString requester,int row,int col);
public slots:
    /**
     * @brief requestDataSlot 请求数据
     * @param requester 请求者对象名
     * @param row 行数
     * @param col 列数
     */
    void requestDataSlot(QString requester,int row,int col);
    /**
     * @brief replyDataSlot 返回数据
     * @param parent 父对象
     * @param target 目标对象名
     * @param list 字符串
     * @param row 行数
     * @param col 列数
     */
    void replyDataSlot(QWidget *parent,QString target,QStringList list,int row,int col);
    /**
     * @brief chooseGPDataBtnSlot 选择地面控制点数据
     */
    void chooseGPDataBtnSlot();
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
    QLabel *methodLabel;
    QComboBox *methodBox;
    QLabel *x0Label;//x0
    QLabel *y0Label;//y0
    QLabel *fLabel;//f
    QLabel *sLabel;//像片比例尺分母
    QLabel *pLabel;//限差

    MyLineEdit *x0LineEdit;
    MyLineEdit *y0LineEdit;
    MyLineEdit *fLineEdit;
    MyLineEdit *sLineEdit;
    MyLineEdit *pLineEdit;

    QPushButton *chooseCPDataBtn;//CP:Control Point
    QTableWidget *CPTable;

    QPushButton *calculateBtn;//计算

    QTableWidget *matrixTable;//旋转矩阵
    QTableWidget *exteriorTable;//外方位元素

    QStringList CPTableHeader;//控制点表头
    QVBoxLayout *mainLayout;
};

#endif // SPACERESECTIONWIDGET_H
