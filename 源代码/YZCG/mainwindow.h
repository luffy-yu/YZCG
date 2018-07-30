#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 系统主界面
  */

#include "Headers.h"
#include "explorerwidget.h"
#include "datacontainerwidget.h"
#include "spaceresectionwidget.h"
#include "spaceintersectionwidget.h"
#include "relativeorientationwidget.h"
#include "absoluteorientationwidget.h"
#include "tableeditwidget.h"

#include "selectdatawidget.h"

#include "io.h"
#include "pointlocation.h"

#include "fileviewer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    /**
     * @brief createSubWindow 创建子窗口
     * @param windowname 子窗口标题
     * @return
     */
    bool createSubWindow(QString windowname);

private slots:
    /**
     * @brief openProjectSlot 打开
     */
    void openProjectSlot();
    /**
     * @brief saveProjectSlot 保存
     */
    void saveProjectSlot();
    /**
     * @brief exitSlot 退出
     */
    void exitSlot();
    /**
     * @brief resectionSlot 空间后方交会
     */
    void resectionSlot();
    /**
     * @brief interSectionSlot 空间前方交会
     */
    void interSectionSlot();
    /**
     * @brief relativeSlot 相对定向
     */
    void relativeSlot();
    /**
     * @brief absoluteSlot 绝对定向
     */
    void absoluteSlot();
    /**
     * @brief pointLocationSlot 像点坐标量测
     */
    void pointLocationSlot();
    /**
     * @brief aboutSlot 关于
     */
    void aboutSlot();
    /**
     * @brief aboutQtSlot 关于Qt
     */
    void aboutQtSlot();
    /**
     * @brief viewFileSlot 浏览文件
     * @param filePath 文件路径
     */
    void viewFileSlot(const QString &filePath);
    /**
     * @brief editFileSlot 编辑文件
     * @param filePath 文件路径
     */
    void editFileSlot(const QString &filePath);
    /**
     * @brief openProjectSlot 打开项目
     * @param filepath 文件路径
     */
    void openProjectSlot(const QString &filepath);
    /**
     * @brief updateLabel 更新当前时间
     */
    void updateLabel();
    /**
     * @brief openUrl 打开网页
     * @param url 链接地址
     */
    void openUrl(QString url);
protected:
    /**
     * @brief initUI 初始化界面
     */
    void initUI();
    /**
     * @brief createSubWindow 创建子窗口
     * @param widget 界面
     * @param objName 对象名称
     * @param title 标题
     */
    void createSubWindow(QWidget *widget,const QString &objName,const QString &title);
private:
    //文件菜单
    QMenu *fileMenu;
    //空间交会菜单
    QMenu *spaceMenu;
    //定向菜单
    QMenu *orientationMenu;
    //工具菜单，包含像点坐标量测
    QMenu *toolMenu;
    //窗口菜单
    QMenu *viewMenu;
    //帮助菜单
    QMenu *helpMenu;

    //打开工程
    QAction *openProjectAction;
    //保存工程
    QAction *saveProjectAction;
    QAction *exitAction;
    //空间后方交会
    QAction *resectionAction;
    //空间前方交会
    QAction *interSectionAction;
    //相对定向
    QAction *relativeAction;
    //绝对定向
    QAction *absoluteAction;
    //像点坐标量测
    QAction *pointLocationAction;
    //关于
    QAction *aboutAction;
    //关于Qt
    QAction *aboutQtAction;
    //mdiArea
    QMdiArea *mdiArea;

    ExplorerWidget *explorer;//文件系统
    DataContainerWidget *dataContainer;//可用数据

    SelectDataWidget *selectData;//选择数据

    //状态栏
    QStatusBar *bar;
    //作者
    QLabel *authorLabel;
    //当前时间
    QLabel *dateTimeLabel;
    //定时器，用于更新时间。
    QTimer *timer;
};

#endif // MAINWINDOW_H
