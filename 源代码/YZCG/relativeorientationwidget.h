#ifndef RELATIVEORIENTATIONWIDGET_H
#define RELATIVEORIENTATIONWIDGET_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 像点坐标量测界面
  */

#include "Headers.h"
#include "mylineedit.h"
#include "utilities.h"
#include "algorithms.h"


class RelativeOrientationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RelativeOrientationWidget(QWidget *parent = 0);
    
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
     * @brief chooseSPBtnSlot 选择同名像点
     */
    void chooseSPBtnSlot();
    /**
     * @brief setElementsTableHeader 设置相对定向元素表头
     * @param index
     */
    void setElementsTableHeader(int index);
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
    QLabel *x0Label;
    QLabel *y0Label;
    QLabel *fLabel;
    QLabel *mLabel;//像片比例尺分母
    QLabel *pLabel;//限差
    QComboBox *methodBox;
    MyLineEdit *x0LineEdit;
    MyLineEdit *y0LineEdit;
    MyLineEdit *fLineEdit;
    MyLineEdit *mLineEdit;
    MyLineEdit *pLineEdit;
    //相对方位元素
    //单独法结果φ1 κ1 φ2 ω2 κ2
    //连续法结果BY BZ φ2 ω2 κ2
    QTableWidget *elementsTable;

    QPushButton *chooseSPBtn;
    QPushButton *calculateBtn;

    QTableWidget *SPTable;
    QTableWidget *resultTable;

    QVBoxLayout *mainLayout;
    QStringList elementsTableHeader1;
    QStringList elementsTableHeader2;
    QStringList SPTableHeader;
    QStringList resultTableHeader;
};

#endif // RELATIVEORIENTATIONWIDGET_H
