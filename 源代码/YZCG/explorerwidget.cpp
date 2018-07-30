#include "explorerwidget.h"

ExplorerWidget::ExplorerWidget(QWidget *parent) :
    QWidget(parent)
{
    model = new QFileSystemModel;
    view = new QTreeView;

    model->setRootPath(QDir::homePath());
    view->setAnimated(true);
    view->setSortingEnabled(true);
    view->setAlternatingRowColors(true);
    view->setAutoScroll(true);

    view->setModel(model);

    //0 - Name
    //1 - Size
    //2 - Type
    //3 - Data Modified
    view->setColumnHidden(1,true);
    view->setColumnHidden(2,true);
    view->setColumnHidden(3,true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(view);
    setLayout(layout);
}

void ExplorerWidget::contextMenuEvent(QContextMenuEvent */*event*/)
{
#ifdef QT_DEBUG
    qDebug()<<"ExplorerWidget contextMenu";
#endif
    if(QFileInfo(model->filePath(view->currentIndex())).isDir()){
        QMenu *menu = new QMenu(this);
        QAction *expandAction = new QAction(tr("展开"),menu);
        QAction *collapseAction = new QAction(tr("收起"),menu);
        menu->addAction(expandAction);
        menu->addSeparator();
        menu->addAction(collapseAction);

        connect(expandAction,SIGNAL(triggered()),this,SLOT(expandSlot()));
        connect(collapseAction,SIGNAL(triggered()),this,SLOT(collapseSlot()));

        menu->exec(QCursor::pos());
        delete menu;
    }else if(QFileInfo(model->filePath(view->currentIndex())).isFile()){
        QMenu *menu = new QMenu(this);

        QString filename = model->fileName(view->currentIndex());

        QRegExp rx1("*.ycd");
        QRegExp rx2("*.ycp");
        rx1.setPatternSyntax(QRegExp::Wildcard);
        rx2.setPatternSyntax(QRegExp::Wildcard);

        if(rx1.exactMatch(filename)){
            QAction *openAction = new QAction(tr("打开"),menu);
            menu->addAction(openAction);
            connect(openAction,SIGNAL(triggered()),this,SLOT(openDataSlot()));
        }else if(rx2.exactMatch(filename)){
            QAction *openAction = new QAction(tr("打开"),menu);
            menu->addAction(openAction);
            connect(openAction,SIGNAL(triggered()),this,SLOT(openProjectSlot()));
        }else{
            QAction *viewAction = new QAction(tr("浏览"),menu);
            QAction *editAction = new QAction(tr("编辑"),menu);

            menu->addAction(viewAction);
            menu->addSeparator();
            menu->addAction(editAction);

            connect(viewAction,SIGNAL(triggered()),this,SLOT(viewSlot()));
            connect(editAction,SIGNAL(triggered()),this,SLOT(editSlot()));
        }
        menu->exec(QCursor::pos());
        delete menu;
    }else
        return;
}

void ExplorerWidget::expandSlot()
{
    view->expand(view->currentIndex());
}

void ExplorerWidget::collapseSlot()
{
    view->collapse(view->currentIndex());
}

void ExplorerWidget::viewSlot()
{
    emit requestViewFile(model->filePath(view->currentIndex()));
}

void ExplorerWidget::editSlot()
{
    emit requestEditFile(model->filePath(view->currentIndex()));
}

void ExplorerWidget::openProjectSlot()
{
    openProjectOrData(true,model->filePath(view->currentIndex()));
}

void ExplorerWidget::openDataSlot()
{
    openProjectOrData(false,model->filePath(view->currentIndex()));
}

void ExplorerWidget::openProjectOrData(bool isProject, QString filename)
{
    if(isProject){
        emit requestOpenProject(filename);
    }else{
        emit requestOpenData(filename);
    }
}
