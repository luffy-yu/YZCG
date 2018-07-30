#include "datacontainerwidget.h"

DataContainerWidget::DataContainerWidget(QWidget *parent) :
    QWidget(parent)
{
    treeView = new QTreeView;
    treeModel = new QStandardItemModel;
    rootItem = treeModel->invisibleRootItem();

    treeModel->setColumnCount(4);
    treeModel->setHorizontalHeaderItem(0,new QStandardItem(tr("数据")));
    treeModel->setHorizontalHeaderItem(1,new QStandardItem(tr("大小")));
    treeModel->setHorizontalHeaderItem(2,new QStandardItem(tr("路径")));
    treeModel->setHorizontalHeaderItem(3,new QStandardItem(tr("描述")));

    treeView->setModel(treeModel);
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(treeView);

    setLayout(layout);

    sizeVisible = true;
    pathVisible = true;
    descriptionVisible = true;
}

QStandardItemModel *DataContainerWidget::getModel()
{
    return treeModel;
}

void DataContainerWidget::addDataSlot(const QStringList &list, const int &row, const int &col, const QString &filename, const QString &description)
{
#ifdef QT_DEBUG
    qDebug()<<list<<row<<col<<filename<<description;
#endif
    QList<QStandardItem *>items;
    items << new QStandardItem(list.join(","));
    items << new QStandardItem(tr("%1*%2").arg(row).arg(col));
    items << new QStandardItem(filename);
    items << new QStandardItem(description);

    rootItem->appendRow(items);
}

void DataContainerWidget::contextMenuEvent(QContextMenuEvent */*event*/)
{
    QMenu *menu = new QMenu(this);
    QAction *editAction = new QAction(tr("编辑"),menu);
    QAction *removeAction = new QAction(tr("移除"),menu);
    QAction *removeAllAction = new QAction(tr("移除所有"),menu);
    QAction *previewAction = new QAction(tr("预览"),menu);
    QAction *saveAction = new QAction(tr("保存"),menu);
    QAction *saveAllAction = new QAction(tr("保存所有"),menu);
    QAction *openAction = new QAction(tr("打开"),menu);
    QAction *addDescriptionAction = new QAction(tr("添加描述"),menu);

    QAction *sizeAction = new QAction(tr("显示大小"),menu);
    QAction *pathAction = new QAction(tr("显示路径"),menu);
    QAction *descriptionAction = new QAction(tr("显示描述"),menu);

    sizeAction->setCheckable(true);
    pathAction->setCheckable(true);
    descriptionAction->setCheckable(true);

    menu->addAction(editAction);
    menu->addSeparator();
    menu->addAction(removeAction);
    menu->addAction(removeAllAction);
    menu->addSeparator();
    menu->addAction(previewAction);
    menu->addSeparator();
    menu->addAction(saveAction);
    menu->addAction(saveAllAction);
    menu->addSeparator();
    menu->addAction(openAction);
    menu->addSeparator();
    menu->addAction(addDescriptionAction);
    menu->addSeparator();

    menu->addAction(sizeAction);
    menu->addAction(pathAction);
    menu->addAction(descriptionAction);

    sizeAction->setChecked(sizeVisible);
    pathAction->setChecked(pathVisible);
    descriptionAction->setChecked(descriptionVisible);

    connect(editAction,SIGNAL(triggered()),this,SLOT(editSlot()));
    connect(removeAction,SIGNAL(triggered()),this,SLOT(removeSlot()));
    connect(removeAllAction,SIGNAL(triggered()),this,SLOT(removeAllSlot()));
    connect(previewAction,SIGNAL(triggered()),this,SLOT(previewSlot()));
    connect(saveAction,SIGNAL(triggered()),this,SLOT(saveSlot()));
    connect(saveAllAction,SIGNAL(triggered()),this,SLOT(saveAllSlot()));
    connect(openAction,SIGNAL(triggered()),this,SLOT(openSlot()));
    connect(addDescriptionAction,SIGNAL(triggered()),this,SLOT(addDescriptionSlot()));

    connect(sizeAction,SIGNAL(triggered()),this,SLOT(showSize()));
    connect(pathAction,SIGNAL(triggered()),this,SLOT(showPath()));
    connect(descriptionAction,SIGNAL(triggered()),this,SLOT(showDescription()));

    menu->exec(QCursor::pos());
    delete menu;
}

void DataContainerWidget::editSlot()
{
    if(treeView->currentIndex().isValid()){
        bool ok;
        int row = treeView->currentIndex().row();
        TableEditWidget *editor = new TableEditWidget(treeModel->item(row,0)->text().split(","),
                                                     treeModel->item(row,1)->text().split("*").at(0).toInt(&ok),
                                                     treeModel->item(row,1)->text().split("*").at(1).toInt(&ok),
                                                     treeModel->item(row,2)->text());
        connect(editor,SIGNAL(addData2DataContainer(QStringList,int,int,QString,QString)),this,SLOT(updateRowSlot(QStringList,int,int,QString)));

        editor->show();
    }
}

void DataContainerWidget::removeSlot()
{
    if(treeView->currentIndex().isValid())
        treeModel->removeRow(treeView->currentIndex().row());
}

void DataContainerWidget::removeAllSlot()
{
    if(treeView->currentIndex().isValid())
        treeModel->removeRows(0,treeModel->rowCount());
}

void DataContainerWidget::previewSlot()
{
    if (treeView->currentIndex().isValid()) {
        int row = treeView->currentIndex().row();
        bool ok;
        createPreview(treeModel->item(row,0)->text().split(","),
                      treeModel->item(row,1)->text().split("*").at(0).toInt(&ok),
                      treeModel->item(row,1)->text().split("*").at(1).toInt(&ok),
                      treeModel->item(row,2)->text());
    }
}

void DataContainerWidget::saveSlot()
{
    if (treeView->currentIndex().isValid()) {
        QString fileName = QFileDialog::getSaveFileName(this,tr("保存..."),"",tr("YZCG Data(*.ycd)"));
        if (!fileName.isEmpty()) {
            QStringList *stringList = new QStringList;
            for(int i = 0;i < 4;i++){
                stringList->append(treeModel->item(treeView->currentIndex().row(),i)->text());//数据 大小 文件路径 描述
            }
            QList<QString> *list = new QList<QString>;
            list->append(stringList->join(";"));
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                QDataStream out(&file);
                // Write a header with a "magic number" and a version
                out << (quint32)0xCCCCCCCC;
                out << (qint32)1;
                out.setVersion(QDataStream::Qt_4_8);
                //Write the data
                out << *list;
                file.close();
                QMessageBox::information(this,tr("提示"),tr("保存成功"),QMessageBox::Ok);
            }
            delete stringList;
            delete list;
        }
    }
}

void DataContainerWidget::saveAllSlot()
{
    if (treeView->currentIndex().isValid()) {
        QString fileName = QFileDialog::getSaveFileName(this,tr("保存..."),"",tr("YZCG Data(*.ycd)"));
        if (!fileName.isEmpty()) {
            int row = treeModel->rowCount();
            QList<QString> *list = new QList<QString>();
            for(int i = 0;i < row;i++){
                QStringList *stringList = new QStringList;
                for(int j = 0;j < 4;j++){
                    stringList->append(treeModel->item(i,j)->text());//数据 大小 文件路径 描述
                }
                list->append(stringList->join(";"));
                delete stringList;
            }
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                QDataStream out(&file);
                out << (quint32)0xCCCCCCCC;
                out << row;//行数
                out.setVersion(QDataStream::Qt_4_8);
                //Write the data
                out << *list;
                file.close();
                QMessageBox::information(this,tr("提示"),tr("保存成功"),QMessageBox::Ok);
            }
            delete list;
        }
    }

}

void DataContainerWidget::openSlot()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("打开..."),"",tr("YZCG Data(*.ycd)"));
    if (!filename.isEmpty()) {
        openYcdFile(filename);
    }
}

void DataContainerWidget::addDescriptionSlot()
{
    if (treeView->currentIndex().isValid()) {
        bool ok;
        int row = treeView->currentIndex().row();
        QString string = QInputDialog::getText(this,tr("添加描述"),tr("请输入要添加的描述："),QLineEdit::Normal,treeModel->item(row,3)->text(),&ok);
        if(ok)
        {
            //treeModel->setItem(row,3,new QStandardItem(string));
            treeModel->item(row,3)->setData(string,Qt::DisplayRole);
        }
    }
}

void DataContainerWidget::updateRowSlot(const QStringList &list, const int &row, const int &col, const QString &filename)
{
#ifdef QT_DEBUG
    qDebug()<<"update row slot:\n"<<list<<row<<col<<filename;
#endif
    int index = treeView->currentIndex().row();
//    treeModel->setItem(index,0,new QStandardItem(list.join(",")));
//    treeModel->setItem(index,1,new QStandardItem(tr("%1*%2").arg(row).arg(col)));
//    treeModel->setItem(index,2,new QStandardItem(filename));

    treeModel->item(index,0)->setData(list.join(","),Qt::DisplayRole);
    treeModel->item(index,1)->setData(tr("%1*%2").arg(row).arg(col),Qt::DisplayRole);
    treeModel->item(index,2)->setData(filename,Qt::DisplayRole);
}

void DataContainerWidget::openDataSlot(const QString &filepath)
{
    openYcdFile(filepath);
}

void DataContainerWidget::showSize()
{
    sizeVisible = !sizeVisible;
    treeView->setColumnHidden(1,!sizeVisible);
}

void DataContainerWidget::showPath()
{
    pathVisible = !pathVisible;
    treeView->setColumnHidden(2,!pathVisible);
}

void DataContainerWidget::showDescription()
{
    descriptionVisible = !descriptionVisible;
    treeView->setColumnHidden(3,!descriptionVisible);
}

void DataContainerWidget::createPreview(const QStringList &list, const int &row, const int &col, const QString &filename)
{
    QTableWidget *table= new QTableWidget;
    table->setAttribute(Qt::WA_DeleteOnClose);
    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowMinMaxButtonsHint);
    table->setMinimumSize(600,200);
    table->setWindowTitle(filename);
    table->setRowCount(row);
    table->setColumnCount(col);

    for(int i = 0;i < row;i++)
    {
        for(int j = 0;j < col;j++)
        {
            QTableWidgetItem *fill = new QTableWidgetItem(list[i * col + j]);
            table->setItem(i,j,fill);
            //table->item(i,j)->setData(Qt::DisplayRole,list[i * col + j]);
        }
    }

    table->setWindowModality(Qt::ApplicationModal);
    table->show();
}

void DataContainerWidget::openYcdFile(const QString &filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly)){
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_4_8);
        quint32 magic;
        in >> magic;
        if (magic != 0xCCCCCCCC)
            QMessageBox::critical(this,tr("错误"),tr("未识别的文件!"),QMessageBox::Yes);
        qint32 row;
        in >> row;
        QList<QString> *list = new QList<QString>();
        in >> *list;
        file.close();

        QString string,col0,col1,col2,col3;
        for(int i = 0;i < row;i++){
            string = list->at(i);
            {
                col0 = string.split(";").at(0);
                col1 = string.split(";").at(1);
                col2 = string.split(";").at(2);
                col3 = string.split(";").at(3);

                QList<QStandardItem *> items;
                items << new QStandardItem(col0);
                items << new QStandardItem(col1);
                items << new QStandardItem(col2);
                items << new QStandardItem(col3);
                rootItem->appendRow(items);
            }
        }
        delete list;
    }
}
