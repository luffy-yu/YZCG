#include "hawkeyeview.h"

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief ZValue:
  * 0.0 : pixmapItem
  * 1.0 : hLine
  * 2.0 : vLine
  * 3.0 : rect
  */

HawkeyeView::HawkeyeView(QWidget *parent) :
    QGraphicsView(parent)
{
    setAttribute(Qt::WA_QuitOnClose);
    setFixedSize(200,150);

    setAcceptDrops(true);

    setRenderHints(QPainter::SmoothPixmapTransform);

    scene = new QGraphicsScene(-1,-1,200,150);
    setScene(scene);
    scene->setBackgroundBrush(Qt::black);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    factor = new qreal(1.0);

    hLine.setP1(QPoint(0,75));
    hLine.setP2(QPoint(200,75));

    vLine.setP1(QPoint(100,0));
    vLine.setP2(QPoint(100,150));

    rect.setRect(80,60,40,30);

    isClose = new bool(true);
}

void HawkeyeView::reset()
{
    foreach (QGraphicsItem *item, scene->items()) {
        delete item;
    }
    scale(1 / (*factor),1 / (*factor));

    *factor = 1.0;

    QGraphicsLineItem *hLineItem = new QGraphicsLineItem(hLine);
    hLineItem->setPen(QPen(Qt::red));
    scene->addItem(hLineItem);

    hLineItem->setZValue(1.0);

    QGraphicsLineItem *vLineItem = new QGraphicsLineItem(vLine);
    vLineItem->setPen(QPen(Qt::red));
    scene->addItem(vLineItem);

    vLineItem->setZValue(2.0);

    QGraphicsRectItem *rectItem = new QGraphicsRectItem(rect);
    rectItem->setPen(QPen(Qt::blue));
    scene->addItem(rectItem);

    rectItem->setZValue(3.0);
}

void HawkeyeView::openImage(const QString &filename)
{
    emit openImage(false);
    reset();
    pixMap = QPixmap(filename);
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixMap);
    pixmapItem->setZValue(0.0);
    scene->addItem(pixmapItem);

    *isClose = false;

    QGraphicsItem *rectItem = NULL;
    foreach (QGraphicsItem *item, scene->items()) {
        if (item->zValue() == 3.0)
            rectItem = item;
    }
    emit openImage(true,filename);
    emit blueImageRectF(pixmapItem->mapFromScene(rectItem->mapToScene(rectItem->boundingRect())));
}

void HawkeyeView::closeImage()
{
    foreach (QGraphicsItem *item, scene->items()) {
        if(item->zValue() == 0.0)
            delete item;
    }

    *isClose = true;
    emit openImage(false);
}

void HawkeyeView::resetImage()
{
    if (! *isClose) {
        reset();
        QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixMap);
        pixmapItem->setZValue(0.0);
        scene->addItem(pixmapItem);
        QGraphicsItem *rectItem = NULL;
        foreach (QGraphicsItem *item, scene->items()) {
            if (item->zValue() == 3.0)
                rectItem = item;
        }
        emit blueImageRectF(pixmapItem->mapFromScene(rectItem->mapToScene(rectItem->boundingRect())));
    }
}

void HawkeyeView::wheelEvent(QWheelEvent *event)
{
    if (! *isClose) {
        QGraphicsItem *pixmapItem = NULL;
        QGraphicsItem *hLineItem = NULL;
        QGraphicsItem *vLineItem = NULL;
        QGraphicsItem *rectItem = NULL;
        foreach (QGraphicsItem *item, scene->items()) {
            switch (int(item->zValue())) {
            case 0:
                pixmapItem = item;
                break;
            case 1:
                hLineItem = item;
                break;
            case 2:
                vLineItem = item;
                break;
            case 3:
                rectItem = item;
                break;
            default:
                break;
            }
        }
        //Line
        QPoint hOriginLeftTop = mapFromScene(hLineItem->mapToScene(QPointF(0.0,0.0)));//变换前左上角位置
        QPoint hOriginDis = QPoint(0,0) - hOriginLeftTop;//变换前距离左上角的距离
        qreal originFactor = 1 / (*factor);//变化前 放缩比例
        //rect
        QPoint rOriginLeftTop = mapFromScene(rectItem->mapToScene(QPointF(0.0,0.0)));
        QPoint rOriginDis = QPoint(0,0) - rOriginLeftTop;
        //pixmap
        if (event->delta() > 0) {
            QPoint originLeftTop = mapFromScene(pixmapItem->mapToScene(QPointF(0.0,0.0)));//左上角点
            QPoint originDis = event->pos() - originLeftTop;//距离左上角的距离

            scale(1.25,1.25);
            (*factor) *= 1.25;
            QPoint LeftTop = mapFromScene(pixmapItem->mapToScene(QPointF(0.0,0.0)));//变换后左上角点

            QPointF neededPos = event->pos() - originDis * 1.25;//需要在的位置

            QPointF move = neededPos - LeftTop;//移动的距离
            pixmapItem->moveBy(move.x() / (*factor),move.y() / (*factor));
        }
        else {
            QPoint originLeftTop = mapFromScene(pixmapItem->mapToScene(QPointF(0.0,0.0)));//左上角点
            QPoint originDis = event->pos() - originLeftTop;//距离左上角的距离

            scale(0.8,0.8);
            (*factor) *= 0.8;
            QPoint LeftTop = mapFromScene(pixmapItem->mapToScene(QPointF(0.0,0.0)));//变换后左上角点

            QPointF neededPos = event->pos() - originDis * 0.8;//需要在的位置

            QPointF move = neededPos - LeftTop;//移动的距离
            pixmapItem->moveBy(move.x() / (*factor),move.y() / (*factor));
        }
        //Line
        hLineItem->setScale(1 / (*factor));
        vLineItem->setScale(1 / (*factor));
        qreal lastFactor = 1 / (*factor);//变换后放缩比例
        QPoint hLeftTop = mapFromScene(hLineItem->mapToScene(QPointF(0.0,0.0)));//变换后左上角位置
        QPointF hNeedLine = QPointF(0.0,0.0) - hOriginDis * (lastFactor / originFactor);//变换后需要在的位置
        QPointF hLineMove = hNeedLine - hLeftTop;//需要移动的距离
        hLineItem->moveBy(hLineMove.x() / (*factor),hLineMove.y() / (*factor));
        vLineItem->moveBy(hLineMove.x() / (*factor),hLineMove.y() / (*factor));
        //rect
        rectItem->setScale(1 / *factor);
        QPoint rLeftTop = mapFromScene(rectItem->mapToScene(QPointF(0.0,0.0)));
        QPointF rNeedRect = QPointF(0.0,0.0) - rOriginDis * (lastFactor / originFactor);
        QPointF rectMove = rNeedRect - rLeftTop;
        rectItem->moveBy(rectMove.x() / (*factor),rectMove.y() / (*factor));
        //
        emit blueImageRectF(pixmapItem->mapFromScene(rectItem->mapToScene(rectItem->boundingRect())));
    }
}

void HawkeyeView::dragEnterEvent(QDragEnterEvent *event)
{
    if (! *isClose) {
        if (event->mimeData()->hasFormat("eventPos")) {
            if(event->source() == this) {
                event->setDropAction(Qt::MoveAction);
                event->accept();
            } else {
                event->acceptProposedAction();
            }
        } else {
            event->ignore();
        }
    }
}

void HawkeyeView::dragMoveEvent(QDragMoveEvent *event)
{
    if (! *isClose) {
        if (event->mimeData()->hasFormat("eventPos")) {
            if(event->source() == this) {
                event->setDropAction(Qt::MoveAction);
                event->accept();
            } else {
                event->acceptProposedAction();
            }
        } else {
            event->ignore();
        }
    }
}

void HawkeyeView::dropEvent(QDropEvent *event)
{
    if (! *isClose) {
        if (event->mimeData()->hasFormat("eventPos")) {
            QGraphicsItem *pixmapItem = NULL;
            QGraphicsItem *rectItem = NULL;
            foreach (QGraphicsItem *item, scene->items()) {
                if (item->zValue() == 0.0)
                    pixmapItem = item;
                if (item->zValue() == 3.0)
                    rectItem = item;
            }
            QByteArray itemData = event->mimeData()->data("eventPos");
            QDataStream dataStream(&itemData,QIODevice::ReadOnly);

            QPoint offset;

            dataStream >> offset;

            QPointF move = event->pos() - offset;

            pixmapItem->moveBy(move.x() / (*factor),move.y() / (*factor));
            if (event->source() == this) {
                event->setDropAction(Qt::MoveAction);
                event->accept();
            } else {
                event->acceptProposedAction();
            }
            emit blueImageRectF(pixmapItem->mapFromScene(rectItem->mapToScene(rectItem->boundingRect())));

        } else {
            event->ignore();
        }
    }
}

void HawkeyeView::mousePressEvent(QMouseEvent *event)
{
    if (! *isClose) {
        QGraphicsItem *pixmapItem = NULL;
        foreach (QGraphicsItem *item, scene->items()) {
            if (item->zValue() == 0.0)
                pixmapItem = item;
        }
        QByteArray dragData;

        QDataStream dataStream(&dragData,QIODevice::WriteOnly);

        dataStream << event->pos();

        QMimeData *mimeData = new QMimeData;

        mimeData->setData("eventPos",dragData);

        QPixmap pixmap = pixMap.scaled(pixMap.width() * (*factor),pixMap.height() * (*factor));

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);

        drag->setHotSpot(event->pos() - mapFromScene(pixmapItem->mapToScene(QPointF(0.0,0.0))));

        if (drag->exec(Qt::CopyAction | Qt::MoveAction,Qt::CopyAction) == Qt::MoveAction) {
            event->accept();
        } else {
            event->ignore();
        }
    }
}
