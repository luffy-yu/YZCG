#include "tableeditwidget.h"

TableEditWidget::TableEditWidget()
{
    initUI();
}

TableEditWidget::TableEditWidget(QString fileName)
{
    initUI();
    fillTable(fileName);
}

TableEditWidget::TableEditWidget(QStringList list, int rows, int cols, QString filename)
{
    initUI();
    contentList = list;
    fillTable(contentList,rows,cols);
    fileFullPath = filename;
}

void TableEditWidget::createSlot()
{
    fileFullPath = "";
    table->clearContents();
    table->setRowCount(1);
    table->setColumnCount(1);
    QTableWidgetItem *item = new QTableWidgetItem("");
    table->setItem(0,0,item);

    //table->item(0,0)->setData(Qt::DisplayRole,"");
}

void TableEditWidget::openSlot()
{
    QString filename = QFileDialog::getOpenFileName(this,"","","");
    if(filename.isEmpty())
        return;
    fileFullPath = filename;

    fillTable(fileFullPath);
}
/*
 *导入功能:
 *按列数不变的方式导入，如果原来列数为0，则将列数设置为1之后再执行导入。
 */
void TableEditWidget::importSlot()
{
    QString filename = QFileDialog::getOpenFileName(this,"","","");
    if(filename.isEmpty())
        return;
    if(!openFile(filename))
        return;
    int cols = table->columnCount() == 0 ? 1 : table->columnCount();
    int rows = contentList.size() % cols == 0 ? (contentList.size() / cols) : (contentList.size() / cols + 1);

    int result = QMessageBox::question(this,tr("提示"),tr("是否从第一行开始填入数据？"),QMessageBox::Yes,QMessageBox::No);

    table->setColumnCount(cols);
    QString string;
    if(result == QMessageBox::Yes){
        table->setRowCount(rows);

        for(int i = 0;i < rows;i++){
            for(int j = 0;j < cols;j++){
                if(i * cols + j < contentList.size()){
                    string = contentList[i * cols + j];
                }else{
                    string = "";
                }
                QTableWidgetItem *item = new QTableWidgetItem(string);
                table->setItem(i,j,item);
//                table->item(i,j)->setData(Qt::DisplayRole,string);
            }
        }
    }else if(result == QMessageBox::No){
        int oldRows = table->rowCount();
        table->setRowCount(rows + table->rowCount());
        for(int i = 0;i < rows;i++){
            for(int j = 0;j < cols;j++){
                if(i * cols + j < contentList.size()){
                    string = contentList[i * cols + j];
                }else{
                    string = "";
                }
                QTableWidgetItem *item = new QTableWidgetItem(string);
                table->setItem(oldRows + i,j,item);
//                table->item(oldRows + i,j)->setData(Qt::DisplayRole,string);
            }
        }
    }else
        return;
}

//先检查，通过后导入到可选数据窗口
void TableEditWidget::confirmSlot()
{
    if(checkAllIsNumber()){
        //QMessageBox::information(this,tr("提示"),tr("检查通过！"),QMessageBox::Ok);
        if(!fileFullPath.isEmpty()){
            saveFile(fileFullPath);
        }
        emit addData2DataContainer(Utilities::tableData2StringList(table),table->rowCount(),table->columnCount(),fileFullPath,"");

        close();

    }else{
        QMessageBox::critical(this,tr("错误"),tr("检查到非法数据，请修正！"),QMessageBox::Yes);
    }
}

void TableEditWidget::cancelSlot()
{
    close();
}

void TableEditWidget::saveSlot()
{
    if(!fileFullPath.isEmpty()){
        saveFile(fileFullPath);
        QMessageBox::information(this,tr("提示"),tr("保存成功！"),QMessageBox::Ok);
    }else{
        saveAsSlot();
    }
}

void TableEditWidget::saveAsSlot()
{
    QString filename = QFileDialog::getSaveFileName(this,"","","*.txt");
    if(!filename.isEmpty()){
        fileFullPath = filename;
        saveFile(fileFullPath);
        QMessageBox::information(this,tr("提示"),tr("成功保存为: %1。").arg(filename),QMessageBox::Ok);
    }
}

void TableEditWidget::contextMenuEvent(QContextMenuEvent */*event*/)
{
#ifdef QT_DEBUG
    qDebug()<<"Table Edit ContextMenu";
#endif

    QMenu *menu = new QMenu(this);

    QAction *addRowAction = new QAction(tr("插入行"),menu);
    QAction *addColAction = new QAction(tr("插入列"),menu);

    QAction *delRowAction = new QAction(tr("删除行"),menu);
    QAction *delColAction = new QAction(tr("删除列"),menu);

    QAction *delSelectAction = new QAction(tr("删除选择"),menu);

    QAction *unSelectAction = new QAction(tr("取消选择"),menu);
    QAction *selectAllAction = new QAction(tr("全选"),menu);
    QAction *SetRowColAction = new QAction(tr("设置"),menu);

    QAction *resetAction = new QAction(tr("重置"),menu);


    menu->addAction(addRowAction);
    menu->addAction(addColAction);
    menu->addSeparator();
    menu->addAction(delRowAction);
    menu->addAction(delColAction);
    menu->addSeparator();
    menu->addAction(delSelectAction);
    menu->addSeparator();
    menu->addAction(unSelectAction);
    menu->addSeparator();
    menu->addAction(selectAllAction);
    menu->addSeparator();
    menu->addAction(SetRowColAction);
    menu->addSeparator();
    menu->addAction(resetAction);

    connect(addRowAction,SIGNAL(triggered()),this,SLOT(addRowSlot()));
    connect(addColAction,SIGNAL(triggered()),this,SLOT(addColSlot()));
    connect(delRowAction,SIGNAL(triggered()),this,SLOT(delRowSlot()));
    connect(delColAction,SIGNAL(triggered()),this,SLOT(delColSlot()));
    connect(delSelectAction,SIGNAL(triggered()),this,SLOT(delSelectSlot()));
    connect(unSelectAction,SIGNAL(triggered()),this,SLOT(unSelectSlot()));
    connect(selectAllAction,SIGNAL(triggered()),this,SLOT(selectAllSlot()));
    connect(SetRowColAction,SIGNAL(triggered()),this,SLOT(setRowColSlot()));
    connect(resetAction,SIGNAL(triggered()),this,SLOT(resetSlot()));

    menu->exec(QCursor::pos());
    delete menu;
}

void TableEditWidget::addRowSlot()
{
    bool ok;
    int i = QInputDialog::getInt(this,tr("插入行"),tr("数目:"),1,1,100,1,&ok);

    if(ok)
    {
        int begin = table->currentRow();
        for(int j = 0; j < i;j++)
        {
            table->insertRow(table->currentRow());
        }
        for(int m = begin;m < begin + i;m++)
        {
            for(int n = 0;n < table->columnCount();n++)
            {
                QTableWidgetItem *item = new QTableWidgetItem("");
                table->setItem(m,n,item);

//                table->item(m,n)->setData(Qt::DisplayRole,"");
            }
        }
    }
}

void TableEditWidget::addColSlot()
{
    bool ok;
    int i = QInputDialog::getInt(this,tr("插入列"),tr("数目:"),1,1,100,1,&ok);

    if(ok)
    {
        int begin = table->currentColumn();
        for(int j = 0; j < i;j++)
        {
            table->insertColumn(table->currentColumn());
        }
        for(int m = 0;m < table->rowCount();m++)
        {
            for(int n = begin;n < begin + i;n++)
            {
                QTableWidgetItem *item = new QTableWidgetItem("");
                table->setItem(m,n,item);

//                table->item(m,n)->setData(Qt::DisplayRole,"");
            }
        }
    }

}

void TableEditWidget::delRowSlot()
{
    bool ok;
    int i = QInputDialog::getInt(this,tr("删除行"),tr("数目:"),1,1,table->rowCount(),1,&ok);
    if(ok)
    {
        for(int j = 0; j < i;j++)
        {
            table->removeRow(table->currentRow());
        }
    }
}

void TableEditWidget::delColSlot()
{
    bool ok;
    int i = QInputDialog::getInt(this,tr("删除列"),tr("数目:"),1,1,table->columnCount(),1,&ok);
    if(ok)
    {
        for(int j = 0; j < i;j++)
        {
            table->removeColumn(table->currentColumn());
        }
    }
}

void TableEditWidget::delSelectSlot()
{
    foreach (QTableWidgetItem *i, table->selectedItems()) {
        i->setData(Qt::DisplayRole,QString(""));
    }
}

void TableEditWidget::unSelectSlot()
{
    table->clearSelection();
}

void TableEditWidget::selectAllSlot()
{
    table->selectAll();
}

void TableEditWidget::setRowColSlot()
{
    bool ok;
    QString rc = QInputDialog::getText(this,tr("设置行列数"),tr("行数*列数："),QLineEdit::Normal,tr("%1*%2").arg(table->rowCount()).arg(table->columnCount()),&ok);
    if(ok && !rc.isEmpty())
    {
        bool okr;
        bool okc;
        int row = rc.simplified().split("*").at(0).toInt(&okr);
        int col = rc.simplified().split("*").at(1).toInt(&okc);
        if(okr && okc)
        {
            table->setRowCount(row);
            table->setColumnCount(col);
        }
        else
            return;
    }
}

void TableEditWidget::resetSlot()
{
    rowCount = fileLine;
    colCount = contentList.size() / fileLine;

    table->clearContents();

    table->setRowCount(rowCount);
    table->setColumnCount(colCount);

    for(int i = 0;i < rowCount;i++)
    {
        for(int j = 0;j < colCount;j++)
        {
            QTableWidgetItem *fill = new QTableWidgetItem(contentList[i * colCount + j]);
            table->setItem(i,j,fill);
//            table->item(i,j)->setData(Qt::DisplayRole,contentList[i * colCount + j]);
        }
    }
}

void TableEditWidget::initUI()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    table = new QTableWidget(this);
    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    createBtn = new QPushButton(tr("新建"),this);
    openBtn = new QPushButton(tr("打开"),this);
    importBtn = new QPushButton(tr("导入"),this);
    confirmBtn = new QPushButton(tr("确定"),this);
    cancelBtn = new QPushButton(tr("取消"),this);
    saveBtn = new QPushButton(tr("保存"),this);
    saveAsBtn = new QPushButton(tr("另存"),this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(table);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(createBtn);
    btnLayout->addWidget(openBtn);
    btnLayout->addWidget(importBtn);
    btnLayout->addWidget(confirmBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(saveAsBtn);

    layout->addLayout(btnLayout);
    setLayout(layout);

    connect(createBtn,SIGNAL(clicked()),this,SLOT(createSlot()));
    connect(openBtn,SIGNAL(clicked()),this,SLOT(openSlot()));
    connect(importBtn,SIGNAL(clicked()),this,SLOT(importSlot()));
    connect(confirmBtn,SIGNAL(clicked()),this,SLOT(confirmSlot()));
    connect(cancelBtn,SIGNAL(clicked()),this,SLOT(cancelSlot()));
    connect(saveBtn,SIGNAL(clicked()),this,SLOT(saveSlot()));
    connect(saveAsBtn,SIGNAL(clicked()),this,SLOT(saveAsSlot()));
}

bool TableEditWidget::openFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this,tr("错误"),tr("打开文件失败！"),QMessageBox::Ok);
        return false;
    }
    QString string;
    fileLine = 0;
    contentList.clear();
    QTextStream in(&file);
    while(!in.atEnd()){
        string = in.readLine().simplified();
        if(!string.isEmpty()){
            contentList.append(string.split(" "));
            fileLine ++;
        }
    }
    file.close();
    return true;
}

void TableEditWidget::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return;
    QTextStream out(&file);
    out.setFieldWidth(15);
    out.setFieldAlignment(QTextStream::AlignRight);
    for(int i = 0;i < table->rowCount();i++){
        for(int j = 0;j < table->columnCount();j++){
            out << table->item(i,j)->text();
        }
        out << '\n';
    }
    out.flush();
    file.close();
}

bool TableEditWidget::checkAllIsNumber()
{
    allIsNumber = true;
    bool ok;
    double d;
    for(int i = 0;i < table->rowCount();i++){
        for(int j = 0;j < table->columnCount();j++){
            d = table->item(i,j)->text().toDouble(&ok);
            if(!ok){
                allIsNumber = false;
                break;
            }
        }
    }
    return allIsNumber;
}

void TableEditWidget::fillTable(const QString &fileName)
{
    fileFullPath = fileName;
    if(!openFile(fileFullPath)){
        return;
    }
    rowCount = fileLine;
    colCount =  contentList.size() / fileLine;

    table->setRowCount(rowCount);
    table->setColumnCount(colCount);

    for(int i = 0;i < rowCount;i++){
        for(int j = 0;j < colCount;j++){
            QTableWidgetItem *item = new QTableWidgetItem(contentList[i * colCount + j]);
            table->setItem(i,j,item);

//            table->item(i,j)->setData(Qt::DisplayRole,contentList[i * colCount + j]);
        }
    }
}

void TableEditWidget::fillTable(const QStringList &list, int rows, int cols)
{
    table->setRowCount(rows);
    table->setColumnCount(cols);

    for(int i = 0;i < rows;i++){
        for(int j = 0;j < cols;j++){
            QTableWidgetItem *item = new QTableWidgetItem(list[i * cols + j]);
            table->setItem(i,j,item);

//            table->item(i,j)->setData(Qt::DisplayRole,list[i * cols + j]);
        }
    }
}
