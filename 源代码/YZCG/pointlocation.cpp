#include "pointlocation.h"

PointLocation::PointLocation(QWidget *parent) :
    QMainWindow(parent)
{
    setAttribute(Qt::WA_QuitOnClose);
    setFixedSize(802,624);
    openImageAction = new QAction(QObject::tr("打开图像"),this);
    connect(openImageAction,SIGNAL(triggered()),this,SLOT(openImageSlot()));
    closeImageAction = new QAction(tr("关闭图像"),this);
    connect(closeImageAction,SIGNAL(triggered()),this,SLOT(closeImageSlot()));
    resetImageAction = new QAction(tr("重置图像"),this);
    connect(resetImageAction,SIGNAL(triggered()),this,SLOT(resetImageSlot()));
    showDockAction = new QAction(tr("显示列表"),this);
    connect(showDockAction,SIGNAL(triggered()),this,SLOT(showDockSlot()));

    menuBar()->setVisible(false);
    toolBar = new QToolBar(this);
    toolBar->setMovable(false);

    toolBar->addAction(openImageAction);
    toolBar->addAction(closeImageAction);
    toolBar->addAction(resetImageAction);
    toolBar->addAction(showDockAction);

    addToolBar(toolBar);

    scene = new QGraphicsScene(0,0,800,600);
    scene->setBackgroundBrush(Qt::gray);

    mView = new MainView;
    mView->setScene(scene);
    mView->copyScene(scene);

    mView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    hView = new HawkeyeView;
    connect(this,SIGNAL(openImage(QString)),hView,SLOT(openImage(QString)));
    connect(this,SIGNAL(closeImage()),hView,SLOT(closeImage()));
    connect(this,SIGNAL(resetImage()),hView,SLOT(resetImage()));

    connect(hView,SIGNAL(openImage(bool,QString)),mView,SLOT(openImage(bool,QString)));
    connect(hView,SIGNAL(blueImageRectF(QPolygonF)),mView,SLOT(showBlueRectF(QPolygonF)));
    proxyWidget = scene->addWidget(hView);

    proxyWidget->setPos(10,10);
    proxyWidget->setZValue(2.0);

    setCentralWidget(mView);
    show();

    mView->installEventFilter(mView);
    setMouseTracking(true);
}

MainView *PointLocation::returnmView()
{
    return mView;
}

void PointLocation::openImageSlot()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("打开图像"),"",tr("*.jpg;;*.png;;*.gif;;*.tif;;*.*"));
    if (!filename.isEmpty()) {
        emit openImage(filename);
    }
}

void PointLocation::closeImageSlot()
{
    emit closeImage();
}

void PointLocation::resetImageSlot()
{
    emit resetImage();
}

void PointLocation::showDockSlot()
{
    mView->showDockSlot();
}

void PointLocation::closeEvent(QCloseEvent *event)
{
    mView->~QGraphicsView();
    hView->~QGraphicsView();

    QMainWindow::closeEvent(event);
}
