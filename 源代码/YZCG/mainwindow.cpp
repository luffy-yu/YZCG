#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
    //DockWidget - ExplorerWidget
    explorer = new ExplorerWidget;
    QDockWidget *leftDock = new QDockWidget(tr("文件系统"),this);
    leftDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    leftDock->setWidget(explorer);
    addDockWidget(Qt::LeftDockWidgetArea,leftDock);
    viewMenu->addAction(leftDock->toggleViewAction());
    //DockWidget - DataContainer
    dataContainer = new DataContainerWidget;
    QDockWidget *rightDock = new QDockWidget(tr("可选数据"),this);
    rightDock->setAllowedAreas(Qt::RightDockWidgetArea);
    rightDock->setWidget(dataContainer);
    addDockWidget(Qt::RightDockWidgetArea,rightDock);
    viewMenu->addAction(rightDock->toggleViewAction());

    mdiArea = new QMdiArea(this);
    setCentralWidget(mdiArea);

    connect(explorer,SIGNAL(requestViewFile(QString)),this,SLOT(viewFileSlot(QString)));
    connect(explorer,SIGNAL(requestEditFile(QString)),this,SLOT(editFileSlot(QString)));
    connect(explorer,SIGNAL(requestOpenProject(QString)),this,SLOT(openProjectSlot(QString)));
    connect(explorer,SIGNAL(requestOpenData(QString)),dataContainer,SLOT(openDataSlot(QString)));

    selectData = new SelectDataWidget;
    selectData->hide();
    selectData->setListViewModel(dataContainer->getModel());
}

bool MainWindow::createSubWindow(QString windowname)
{
    if(windowname == "SpaceResection"){
        resectionSlot();
    }
    if(windowname == "SpaceIntersection"){
        interSectionSlot();
    }
    if(windowname == "RelativeOrientation"){
        relativeSlot();
    }
    if(windowname == "AbsoluteOrientation"){
        absoluteSlot();
    }
    return true;
}

void MainWindow::openProjectSlot()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("打开..."),"",tr("YZCG Project(*.ycp)"));
    if(!filename.isEmpty()){
        IO::readFromxmlFile(filename,this,mdiArea);
    }
    else{
        return;
    }
}

void MainWindow::saveProjectSlot()
{
    if (!mdiArea->subWindowList().size())
        return;
    QString filename = QFileDialog::getSaveFileName(this,tr("保存..."),"",tr("YZCG Project(*.ycp)"));
    if (!filename.isEmpty()){
        QWidget *widget = static_cast<QWidget *>(mdiArea->activeSubWindow()->widget());
        IO::save2xmlFile(widget,filename);
    }
}

void MainWindow::exitSlot()
{
    close();
}

void MainWindow::resectionSlot()
{
    SpaceResectionWidget *widget = new SpaceResectionWidget;

    connect(widget,SIGNAL(requestData(QWidget*,QString,int,int)),selectData,SLOT(requestDataSlot(QWidget*,QString,int,int)));
    connect(selectData,SIGNAL(replyData(QWidget*,QString,QStringList,int,int)),widget,SLOT(replyDataSlot(QWidget*,QString,QStringList,int,int)));
    createSubWindow(widget,"SpaceResection",tr("空间后方交会"));
}

void MainWindow::interSectionSlot()
{
    SpaceIntersectionWidget *widget = new SpaceIntersectionWidget;

    connect(widget,SIGNAL(requestData(QWidget*,QString,int,int)),selectData,SLOT(requestDataSlot(QWidget*,QString,int,int)));
    connect(selectData,SIGNAL(replyData(QWidget*,QString,QStringList,int,int)),widget,SLOT(replyDataSlot(QWidget*,QString,QStringList,int,int)));
    createSubWindow(widget,"SpaceIntersection",tr("空间前方交会"));
}

void MainWindow::relativeSlot()
{
    RelativeOrientationWidget *widget = new RelativeOrientationWidget;

    connect(widget,SIGNAL(requestData(QWidget*,QString,int,int)),selectData,SLOT(requestDataSlot(QWidget*,QString,int,int)));
    connect(selectData,SIGNAL(replyData(QWidget*,QString,QStringList,int,int)),widget,SLOT(replyDataSlot(QWidget*,QString,QStringList,int,int)));
    createSubWindow(widget,"RelativeOrientation",tr("相对定向"));
}

void MainWindow::absoluteSlot()
{
    AbsoluteOrientationWidget *widget = new AbsoluteOrientationWidget;

    connect(widget,SIGNAL(requestData(QWidget*,QString,int,int)),selectData,SLOT(requestDataSlot(QWidget*,QString,int,int)));
    connect(selectData,SIGNAL(replyData(QWidget*,QString,QStringList,int,int)),widget,SLOT(replyDataSlot(QWidget*,QString,QStringList,int,int)));
    createSubWindow(widget,"AbsoluteOrientation",tr("绝对定向"));
}

void MainWindow::pointLocationSlot()
{
    PointLocation *pl = new PointLocation(this);
    pl->setWindowTitle(tr("像点坐标量测 - YZCG "));
    connect(pl->returnmView(),SIGNAL(addData(QStringList,int,int,QString,QString)),dataContainer,SLOT(addDataSlot(QStringList,int,int,QString,QString)));
    pl->show();
}

void MainWindow::aboutSlot()
{
    QMessageBox::about(this,tr("关于"),tr("<h2>作者:于留传<br>"
                                        "指导老师:王仁礼<br>"
                                        "学校:山东科技大学<br>"
                                        "学号:201101180723<br>"
                                        "班级:遥感科学与技术2011级1班<br>"
                                        "Last Modified:2015-06-15 10:29</h2>"));
}

void MainWindow::aboutQtSlot()
{
    qApp->aboutQt();
}

void MainWindow::viewFileSlot(const QString &filePath)
{
#ifdef QT_DEBUG
    qDebug()<<"view File Slot";
#endif
    FileViewer *viewer = new FileViewer(NULL,filePath);
    viewer->show();
}

void MainWindow::editFileSlot(const QString &filePath)
{
#ifdef QT_DEBUG
    qDebug()<<"edit File Slot";
    qDebug()<<"File Type:" + QFileInfo(QFile(filePath)).suffix();
#endif
    if(QFileInfo(QFile(filePath)).suffix() != "txt"){
        QMessageBox::critical(this,tr("错误"),tr("不支持该类型文件的编辑！"),QMessageBox::Ok);
        return;
    }
    TableEditWidget *widget = new TableEditWidget(filePath);
    connect(widget,SIGNAL(addData2DataContainer(QStringList,int,int,QString,QString)),dataContainer,SLOT(addDataSlot(QStringList,int,int,QString,QString)));

    widget->show();
}

void MainWindow::openProjectSlot(const QString &filepath)
{
    IO::readFromxmlFile(filepath,this,mdiArea);
}

void MainWindow::updateLabel()
{
    dateTimeLabel->setText(tr("<h3>%1</h3>").arg(QDateTime::currentDateTime().toString("yyyy-MM-d hh:mm:ss")));
}

void MainWindow::openUrl(QString url)
{
    QDesktopServices::openUrl(url);
}

void MainWindow::initUI()
{
    //menus
    fileMenu = new QMenu(tr("文件"),this);
    spaceMenu = new QMenu(tr("交会"),this);
    orientationMenu  = new QMenu(tr("定向"),this);
    toolMenu = new QMenu(tr("工具"),this);
    viewMenu = new QMenu(tr("窗口"),this);
    helpMenu = new QMenu(tr("帮助"),this);
    //actions
    openProjectAction = new QAction(tr("打开"),this);
    saveProjectAction = new QAction(tr("保存"),this);
    exitAction = new QAction(tr("退出"),this);
    resectionAction = new QAction(tr("空间后方交会"),this);
    interSectionAction = new QAction(tr("空间前方交会"),this);
    relativeAction = new QAction(tr("相对定向"),this);
    absoluteAction = new QAction(tr("绝对定向"),this);
    pointLocationAction = new QAction(tr("像点坐标量测"),this);
    aboutAction = new QAction(tr("关于"),this);
    aboutQtAction = new QAction(tr("关于Qt"),this);

    fileMenu->addAction(openProjectAction);
    fileMenu->addAction(saveProjectAction);
    fileMenu->addAction(exitAction);

    spaceMenu->addAction(resectionAction);
    spaceMenu->addAction(interSectionAction);

    orientationMenu->addAction(relativeAction);
    orientationMenu->addAction(absoluteAction);

    toolMenu->addAction(pointLocationAction);

    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(spaceMenu);
    menuBar()->addMenu(orientationMenu);
    menuBar()->addMenu(toolMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);

    connect(openProjectAction,SIGNAL(triggered()),this,SLOT(openProjectSlot()));
    connect(saveProjectAction,SIGNAL(triggered()),this,SLOT(saveProjectSlot()));
    connect(exitAction,SIGNAL(triggered()),this,SLOT(exitSlot()));
    connect(resectionAction,SIGNAL(triggered()),this,SLOT(resectionSlot()));
    connect(interSectionAction,SIGNAL(triggered()),this,SLOT(interSectionSlot()));
    connect(relativeAction,SIGNAL(triggered()),this,SLOT(relativeSlot()));
    connect(absoluteAction,SIGNAL(triggered()),this,SLOT(absoluteSlot()));
    connect(pointLocationAction,SIGNAL(triggered()),this,SLOT(pointLocationSlot()));
    connect(aboutAction,SIGNAL(triggered()),this,SLOT(aboutSlot()));
    connect(aboutQtAction,SIGNAL(triggered()),this,SLOT(aboutQtSlot()));

    //状态栏
    bar = new QStatusBar(this);

    authorLabel = new QLabel(tr("<style> a {text-decoration: none} </style><a href=\"http://user.qzone.qq.com/1096050539\"><h3><font color = blue>Author:yuliuchuan@RS2011.SDUST</font></h3></a>"),this);
    connect(authorLabel,SIGNAL(linkActivated(QString)),this,SLOT(openUrl(QString)));

    bar->addPermanentWidget(authorLabel,0);

    dateTimeLabel = new QLabel(tr("<h3>%1</h3>").arg(QDateTime::currentDateTime().toString("yyyy-MM-d hh:mm:ss")),this);

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateLabel()));
    timer->start(1000);
    bar->addWidget(dateTimeLabel,0);
    this->setStatusBar(bar);
}

void MainWindow::createSubWindow(QWidget *widget, const QString &objName, const QString &title)
{
    QMdiSubWindow *subWindow = new QMdiSubWindow(this);
    subWindow->setObjectName(objName);
    subWindow->setWindowTitle(title);
    subWindow->setWidget(widget);
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    mdiArea->addSubWindow(subWindow);

    subWindow->show();
}
