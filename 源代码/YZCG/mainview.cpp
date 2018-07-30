#include "mainview.h"

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief ZValue:
  * 0.0 : pixmapItem
  * 0.5 : pointItem
  * 1.0 : circleItem
  */
MainView::MainView(QWidget *parent) :
    QGraphicsView(parent)
{
    setMouseTracking(true);

    hasImage = new bool(false);

    model = new QStandardItemModel;
    view = new QTreeView;

    rootItem = model->invisibleRootItem();

    model->setColumnCount(2);
    model->setHorizontalHeaderItem(0,new QStandardItem(tr("点名")));
    model->setHorizontalHeaderItem(1,new QStandardItem(tr("像素坐标")));

    view->setModel(model);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(view,SIGNAL(clicked(QModelIndex)),this,SLOT(itemClicked(QModelIndex)));
    connect(view,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(itemDoubleClicked(QModelIndex)));

    dock = new QDockWidget(this);
    dock->setWidget(view);
    dock->setFloating(true);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);

    isRemove = new bool(false);

    itemMap = new QMap<QGraphicsEllipseItem *,QPointF>();

    circleItem = new QGraphicsEllipseItem(-5,-5,10,10);

    circleItem->setPen(QPen(Qt::red));

    circleItem->setZValue(1.0);

    circleItem->setVisible(false);
}

void MainView::showBlueRectF(const QPolygonF &rect)
{
    circleItem->setVisible(false);
    //pixmap item
    QGraphicsItem *pixmapitem = NULL;
    foreach (QGraphicsItem *item, scene->items()) {
        if (item->zValue() == 0.0) {
            pixmapitem = item;
            QPointF leftTop = rect.first();
            QPointF rightTop = rect.at(1);
            qreal factor = this->width() / (rightTop.x() - leftTop.x());
            item->setScale(factor);
            QPointF distance = mapToScene(0.0,0.0) - item->mapToScene(leftTop);
            item->moveBy(distance.x(),distance.y());
        }
    }
    //set the point item pos
    foreach (QGraphicsEllipseItem *item, itemMap->keys()) {
        item->setPos(pixmapitem->mapToScene(itemMap->value(item)));
    }
}

void MainView::openImage(bool open, const QString &filename)
{
    circleItem->setVisible(false);

    *hasImage = open;
    if (open) {
        pixMap = QPixmap(filename);
        QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixMap);
        pixmapItem->setZValue(0.0);
        scene->addItem(pixmapItem);
    } else {
        foreach (QGraphicsItem *item, scene->items()) {
            if ((item->zValue() == 0.0) || (item->zValue() == 0.5))
                delete item;
        }
    }
    rootItem->removeRows(0,rootItem->rowCount());

    itemMap->clear();
}

void MainView::showDockSlot()
{
    dock->show();
}

void MainView::removeSlot(bool checked)
{
    *isRemove = checked;
    if(checked){
        dock->setVisible(true);
        dock->activateWindow();
    }
}

void MainView::removeAllSlot()
{
    circleItem->setVisible(false);

    foreach (QGraphicsEllipseItem *item, itemMap->keys()) {
        scene->removeItem(item);
        itemMap->remove(item);
        delete item;
    }

    rootItem->removeRows(0,rootItem->rowCount());
}

void MainView::exportToSlot()
{
    circleItem->setVisible(false);
    int *row = new int(rootItem->rowCount());
    QStringList *list = new QStringList;
    for (int i = 0;i < (*row);i++) {
        list->append(model->item(i,0)->text());
        list->append(model->item(i,1)->text().split(",").at(0));
        list->append(model->item(i,1)->text().split(",").at(1));
    }
    emit addData(*list,*row,3,"",tr("像点坐标量测"));
    delete row;
    delete list;
}

void MainView::itemDoubleClicked(const QModelIndex &index)
{
    if (*isRemove) {
        int *row = new int(index.row());
        foreach (QGraphicsEllipseItem *item, itemMap->keys()) {
            if(QString(tr("%1,%2").arg(itemMap->value(item).x()).arg(itemMap->value(item).y())) == (model->item(index.row(),1)->text())){
                scene->removeItem(item);
                itemMap->remove(item);
                delete item;
            }
        }

        rootItem->removeRow(*row);

        //*isRemove = false;
        delete row;

        circleItem->setVisible(false);
    } else {
        bool *ok = new bool;
        QString name = QInputDialog::getText(this,tr("修改点名"),tr("点名"),QLineEdit::Normal,model->item(index.row(),0)->text(),ok);
        if (*ok) {
            model->setItem(index.row(),0,new QStandardItem(name));
            //model->item(index.row(),0)->setData(name,Qt::DisplayRole);
        }
    }
}

void MainView::itemClicked(const QModelIndex &index)
{
    if(!(*hasImage))
        return;
    if(!index.isValid())
        return;

    QGraphicsItem *pixmapItem = NULL;
    foreach (QGraphicsItem *item, scene->items()) {
        if(item->zValue() == 0.0)
            pixmapItem = item;
    }

    foreach (QGraphicsEllipseItem *item, itemMap->keys()) {
        if(QString(tr("%1,%2").arg(itemMap->value(item).x()).arg(itemMap->value(item).y())) == (model->item(index.row(),1)->text())){

            QPoint move = QPoint(this->width() / 2,this->height() / 2) - mapFromScene(pixmapItem->mapToScene(itemMap->value(item)));

            pixmapItem->moveBy(move.x(),move.y());

            moveEllipseItem();

            circleItem->setPos(pixmapItem->mapToScene(itemMap->value(item)));

            circleItem->setVisible(true);
        }
    }
}

bool MainView::eventFilter(QObject */*obj*/, QEvent *event)
{
    if (*hasImage) {
        if (event->type() == QEvent::MouseButtonPress){
            QMouseEvent *ev = static_cast<QMouseEvent *>(event);
            if ((!pointInmView(ev->pos())) && (ev->button() == Qt::LeftButton )){

                *isRemove = false;

                circleItem->setVisible(false);
                //添加点
                static int count = 0;

                QGraphicsItem *pixmapitem = NULL;
                foreach (QGraphicsItem *item, scene->items()) {
                    if (item->zValue() == 0.0) {
                        pixmapitem = item;
                    }
                }

                QGraphicsEllipseItem *item = new QGraphicsEllipseItem(0,0,1,1);
                QPointF move = mapToScene(ev->pos()) - item->mapToScene(QPointF(0.0,0.0));
                item->moveBy(move.x(),move.y());
                item->setPen(QPen(Qt::blue));
                item->setZValue(0.5);

                bool *ok = new bool;
                QString name = QInputDialog::getText(this,tr("请输入点名"),tr("点名"),QLineEdit::Normal,tr("P%1").arg(count),ok);
                if (! *ok){
                    event->ignore();
                    return true;
                }

                scene->addItem(item);
                QPointF pos = pixmapitem->mapFromScene(mapToScene(ev->pos()));

                itemMap->insert(item,pos);

                addItem(name,pos);

                delete ok;

                count++;
            }
        }
        return true;
    }
    else {
        event->ignore();
        return false;
    }
}

void MainView::contextMenuEvent(QContextMenuEvent */*event*/)
{
    QMenu *menu = new QMenu;
    QAction *remove = new QAction(tr("删除"),menu);
    QAction *removeAll = new QAction(tr("删除所有"),menu);
    QAction *showDock = new QAction(tr("显示列表"),menu);
    QAction *exportTo = new QAction(tr("导出"),menu);
    remove->setCheckable(true);

    menu->addAction(remove);
    menu->addAction(removeAll);
    menu->addSeparator();
    menu->addAction(showDock);
    menu->addSeparator();
    menu->addAction(exportTo);

    showDock->setEnabled(!dock->isVisible());
    remove->setEnabled(rootItem->rowCount() > 0);

    if(remove->isEnabled()){
        if(*isRemove){
            remove->setChecked(true);
        }else{
            remove->setChecked(false);
        }
    }

    removeAll->setEnabled(rootItem->rowCount() > 0);
    exportTo->setEnabled(rootItem->rowCount() > 0);

    connect(remove,SIGNAL(triggered(bool)),this,SLOT(removeSlot(bool)));
    connect(removeAll,SIGNAL(triggered()),this,SLOT(removeAllSlot()));
    connect(showDock,SIGNAL(triggered()),this,SLOT(showDockSlot()));
    connect(exportTo,SIGNAL(triggered()),this,SLOT(exportToSlot()));

    menu->exec(QCursor::pos());
    delete menu;
}

void MainView::moveEllipseItem()
{
    //set the point item pos
    QGraphicsItem *pixmapitem = NULL;
    foreach (QGraphicsItem *item, scene->items()) {
        if (item->zValue() == 0.0) {
            pixmapitem = item;
        }
    }
    foreach (QGraphicsEllipseItem *item, itemMap->keys()) {
        item->setPos(pixmapitem->mapToScene(itemMap->value(item)));
    }
}

void MainView::copyScene(QGraphicsScene *scene)
{
    this->scene = scene;
    this->scene->addItem(circleItem);
}

bool MainView::pointInmView(const QPointF &point)
{
    if ((point.x() > 10)
            &&(point.x() <= 210)
            &&(point.y() > 10)
            &&(point.y() <= 160))
        return true;
    else
        return false;
}

void MainView::addItem(const QString &name, const QPointF &pos)
{
    QList<QStandardItem *> item;
    item << new QStandardItem(name);
    item << new QStandardItem(tr("%1,%2").arg(pos.x()).arg(pos.y()));
    rootItem->appendRow(item);
}
