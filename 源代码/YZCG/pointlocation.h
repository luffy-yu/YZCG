#ifndef POINTLOCATION_H
#define POINTLOCATION_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 像点坐标量测 - 主窗口
  */

#include "Headers.h"
#include "mainview.h"
#include "hawkeyeview.h"

class PointLocation : public QMainWindow
{
    Q_OBJECT
public:
    explicit PointLocation(QWidget *parent = 0);
    /**
     * @brief returnmView 返回主视图的指针
     * @return
     */
    MainView *returnmView();
signals:
    /**
     * @brief openImage 打开图像
     * @param filename 文件名
     */
    void openImage(const QString &filename);
    /**
     * @brief closeImage 关闭图像
     */
    void closeImage();
    /**
     * @brief resetImage 重置图像
     */
    void resetImage();
public slots:
    /**
     * @brief openImageSlot 打开图像
     */
    void openImageSlot();
    /**
     * @brief closeImageSlot 关闭图像
     */
    void closeImageSlot();
    /**
     * @brief resetImageSlot 重置图像
     */
    void resetImageSlot();
    /**
     * @brief showDockSlot 显示列表
     */
    void showDockSlot();
protected:
    /**
     * @brief closeEvent 关闭事件
     * @param event
     */
    void closeEvent(QCloseEvent *event);
private:
    QGraphicsScene *scene;//场景
    MainView *mView;//主视图
    HawkeyeView *hView;//鹰眼视图
    QGraphicsProxyWidget *proxyWidget;//代理widget
    QToolBar *toolBar;//工具栏
    QAction *openImageAction;//打开图像
    QAction *closeImageAction;//关闭图像
    QAction *resetImageAction;//重置图像
    QAction *showDockAction;//显示列表
};

#endif // POINTLOCATION_H
