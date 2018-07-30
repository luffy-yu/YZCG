#ifndef HAWKEYEVIEW_H
#define HAWKEYEVIEW_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 像点坐标量测 - 鹰眼窗口
  */

#include "Headers.h"

class HawkeyeView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit HawkeyeView(QWidget *parent = 0);

signals:
    /**
     * @brief blueImageRectF 显示蓝色区域的图像
     * @param rect
     */
    void blueImageRectF(const QPolygonF &rect);
    /**
     * @brief openImage 打开图像
     * @param open true为打开，false为关闭
     * @param filename 文件名
     */
    void openImage(bool open,QString filename = QString());

public slots:
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
protected:
    /**
     * @brief mousePressEvent 鼠标按下事件
     * @param event
     */
    void mousePressEvent(QMouseEvent *event);
    /**
     * @brief wheelEvent 鼠标中键滑动事件
     * @param event
     */
    void wheelEvent(QWheelEvent *event);
    /**
     * @brief dragEnterEvent 拖动进入事件
     * @param event
     */
    void dragEnterEvent(QDragEnterEvent *event);
    /**
     * @brief dragMoveEvent 拖动事件
     * @param event
     */
    void dragMoveEvent(QDragMoveEvent *event);
    /**
     * @brief dropEvent 拖动释放事件
     * @param event
     */
    void dropEvent(QDropEvent *event);
    /**
     * @brief reset 重置
     */
    void reset();
private:
    QPixmap pixMap;//图像
    QGraphicsScene *scene;//场景
    QRectF rect;//蓝色区域
    QLineF hLine;//水平线
    QLineF vLine;//竖直线
    qreal *factor;//缩放比例
    bool *isClose;//是否关闭图像
};

#endif // HAWKEYEVIEW_H
