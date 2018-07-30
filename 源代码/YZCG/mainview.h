#ifndef MAINVIEW_H
#define MAINVIEW_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 像点坐标量测 - 主窗口
  */

#include "Headers.h"
#include <QMap>

class MainView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MainView(QWidget *parent = 0);
    /**
     * @brief copyScene 复制场景指针
     * @param scene 场景指针
     */
    void copyScene(QGraphicsScene *scene);

signals:
    /**
     * @brief addData 添加数据
     * @param list 字符串
     * @param row 行数
     * @param col 列数
     * @param filename 文件名
     * @param description 描述
     */
    void addData(const QStringList list, const int row, const int col, const QString filename, const QString description);
public slots:
    /**
     * @brief showBlueRectF 显示蓝色区域
     * @param rect 蓝色区域
     */
    void showBlueRectF(const QPolygonF &rect);
    /**
     * @brief openImage 打开图像
     * @param open true为打开，false为关闭
     * @param filename 文件名
     */
    void openImage(bool open,const QString &filename = QString());
    /**
     * @brief showDockSlot 显示列表
     */
    void showDockSlot();
    /**
     * @brief removeSlot 移除
     */
    void removeSlot(bool checked);
    /**
     * @brief removeAllSlot 移除所有
     */
    void removeAllSlot();
    /**
     * @brief exportToSlot 导出
     */
    void exportToSlot();
    /**
     * @brief itemClicked item单击事件
     * @param index 索引
     */
    void itemClicked(const QModelIndex &index);

    void itemDoubleClicked(const QModelIndex &index);
protected:
    /**
     * @brief eventFilter 事件过滤器
     * @param event
     * @return
     */
    bool eventFilter(QObject *, QEvent *event);
    /**
     * @brief contextMenuEvent 右键菜单
     * @param event
     */
    void contextMenuEvent(QContextMenuEvent *event);

    void moveEllipseItem();
private:
    QPixmap pixMap;//图像
    QGraphicsScene *scene;//场景
    QMap<QGraphicsEllipseItem *,QPointF> *itemMap;//(点，位置)
    QStandardItemModel *model;//模型
    QDockWidget *dock;
    QTreeView *view;
    QStandardItem *rootItem;
    bool *hasImage;//是否有图像
    bool *isRemove;//是否移动
    QGraphicsEllipseItem *circleItem;//用于标记选择点
private:
    /**
     * @brief pointInmView 点是否在鹰眼视图里
     * @param point 点
     * @return 在，返回true，否则返回false
     */
    bool pointInmView(const QPointF &point);
    /**
     * @brief addItem 添加点
     * @param name 名称
     * @param pos 位置
     */
    void addItem(const QString &name, const QPointF &pos);
};
#endif // MAINVIEW_H
