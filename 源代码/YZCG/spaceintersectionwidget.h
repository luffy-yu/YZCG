#ifndef SPACEINTERSECTIONWIDGET_H
#define SPACEINTERSECTIONWIDGET_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 空间前方交会界面
  */

#include "Headers.h"
#include "mylineedit.h"
#include "utilities.h"
#include "algorithms.h"

class SpaceIntersectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpaceIntersectionWidget(QWidget *parent = 0);
    
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
     * @brief chooseSPBtnSlot 选择同名像点数据
     */
    void chooseSPBtnSlot();
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
    QVBoxLayout *mainLayout;
    QLabel *methodLabel;
    QComboBox *methodBox;

    QLabel *x0Label;
    QLabel *y0Label;
    QLabel *fLabel;
    MyLineEdit *x0LineEdit;
    MyLineEdit *y0LineEdit;
    MyLineEdit *fLineEdit;

    QLabel *LXsLabel;
    QLabel *LYsLabel;
    QLabel *LZsLabel;
    QLabel *LphiLabel;
    QLabel *LomegaLabel;
    QLabel *LkappaLabel;
    MyLineEdit *LXsLineEdit;
    MyLineEdit *LYsLineEdit;
    MyLineEdit *LZsLineEdit;
    MyLineEdit *LphiLineEdit;
    MyLineEdit *LomegaLineEdit;
    MyLineEdit *LkappaLineEdit;

    QLabel *RXsLabel;
    QLabel *RYsLabel;
    QLabel *RZsLabel;
    QLabel *RphiLabel;
    QLabel *RomegaLabel;
    QLabel *RkappaLabel;
    MyLineEdit *RXsLineEdit;
    MyLineEdit *RYsLineEdit;
    MyLineEdit *RZsLineEdit;
    MyLineEdit *RphiLineEdit;
    MyLineEdit *RomegaLineEdit;
    MyLineEdit *RkappaLineEdit;

    QPushButton *chooseSPBtn;//SP : same point
    QPushButton *calculateBtn;
    QTableWidget *SPTable;
    QTableWidget *resultTable;

    QStringList SPTableHeader;
    QStringList resultTableHeader;
};

#endif // SPACEINTERSECTIONWIDGET_H
