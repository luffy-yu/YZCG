#ifndef IO_H
#define IO_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief I/O模块
  */

#include "Headers.h"
#include "mylineedit.h"
#include "mainwindow.h"

class MainWindow;
class IO : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief save2xmlFile 保存到xml文件
     * @param widget 窗口
     * @param filename xml文件名
     * @return
     */
    bool static save2xmlFile(QWidget *widget, QString filename);
    /**
     * @brief readFromxmlFile 读取xml文件创建窗口
     * @param filename xml文件名
     * @param window 主窗口
     * @param area MDI区域
     * @return
     */
    bool static readFromxmlFile(QString filename, MainWindow *window, QMdiArea *area);
private:
    /**
     * @brief createSubWindow 创建子窗口
     * @param window 主窗口
     * @param windowname 子窗口窗口名
     * @return
     */
    bool static createSubWindow(MainWindow *window,QString windowname);
};

#endif // IO_H
