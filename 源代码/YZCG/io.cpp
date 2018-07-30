#include "io.h"

bool IO::save2xmlFile(QWidget *widget, QString filename)
{
    //QString windowname = MdiArea->activeSubWindow()->objectName();
    //MyWidget *widget = static_cast<MyWidget *>(MdiArea->activeSubWindow()->widget());

    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement root = doc.createElement(widget->objectName());
    doc.appendChild(root);
    //QComboBox
    QList<QComboBox *> allcombobox = widget->findChildren<QComboBox *>();
    if(allcombobox.size() != 0){
        QDomElement typecom = doc.createElement("QComboBox");
        root.appendChild(typecom);
        foreach (QComboBox *box, allcombobox) {
            QDomElement child = doc.createElement(box->objectName());
            child.setAttribute("Text",box->currentText());
            child.setAttribute("Index",box->currentIndex());
            typecom.appendChild(child);
        }
    }
    //MyLineEdit
    QList<MyLineEdit *> alllineedit = widget->findChildren<MyLineEdit *>();
    if(alllineedit.size() != 0){
        QDomElement typeline = doc.createElement("MyLineEdit");
        root.appendChild(typeline);
        foreach (MyLineEdit *lineedit, alllineedit) {
            QDomElement child = doc.createElement(lineedit->objectName());
            child.setAttribute("Value",lineedit->text());
            typeline.appendChild(child);
        }
    }
    //TableWidget
    QList<QTableWidget *> alltable = widget->findChildren<QTableWidget *>();
    if(alltable.size() != 0){
        QDomElement typetable = doc.createElement("QTableWidget");
        root.appendChild(typetable);
        foreach (QTableWidget *table, alltable) {
            if(table->rowCount() == 0){
                QDomElement child = doc.createElement(table->objectName());
                child.setAttribute("HasData",tr("No"));
                typetable.appendChild(child);
            } else {
                QDomElement child = doc.createElement(table->objectName());
                child.setAttribute("HasData",tr("Yes"));
                for(int i = 0;i < table->rowCount();i++){
                    for(int j = 0;j < table->columnCount();j++){
                        QDomElement tableitem = doc.createElement("Data");
                        tableitem.setAttribute("Position",tr("(%1,%2)").arg(i).arg(j));
                        tableitem.setAttribute("Value",table->item(i,j)->text());
                        child.appendChild(tableitem);
                    }
                }
                typetable.appendChild(child);
            }
        }
    }
    //Create DataTime
    QDomElement fileinfo = doc.createElement("FileInfo");
    fileinfo.setAttribute("CreatedDate",QDate::currentDate().toString("yyyy.MM.dd"));
    fileinfo.setAttribute("CreatedTime",QTime::currentTime().toString("hh:mm:ss.zzz"));
    root.appendChild(fileinfo);

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        qDebug()<<"Failed open to writing";
    }else{
        QTextStream stream(&file);
        doc.save(stream,4);
        file.close();
        QMessageBox::information(NULL,tr("提示"),tr("保存成功"),QMessageBox::Yes);
    }

    return true;
}

bool IO::readFromxmlFile(QString filename, MainWindow *window,QMdiArea *area)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(NULL,tr("错误"),tr("打开失败!"),QMessageBox::Yes);
        return false;
    }
    QDomDocument doc;
    if(!doc.setContent(&file)){
        file.close();
        return false;
    }
    file.close();
    //    //XML说明
    //    QDomNode firstNode = doc.firstChild();
    //    qDebug() << firstNode.nodeName() << firstNode.nodeValue();
    //根节点 处理方式
    QDomElement root = doc.documentElement();
    //qDebug()<<root.nodeName();
    //首先生成窗口，然后填充数据。
    if(!createSubWindow(window,root.nodeName()))
        return false;

    QWidget *widget = static_cast<QWidget *>(area->activeSubWindow()->widget());

    QDomNodeList rootnodelist = root.childNodes();
    bool *ok = new bool;
    for(int i = 0;i < rootnodelist.size();i++){
        QDomElement element = rootnodelist.at(i).toElement();
        if(element.tagName() == "QComboBox"){
            QDomNodeList boxlist = element.childNodes();
            for(int j = 0;j < boxlist.size();j++){
                QDomElement boxelement = boxlist.at(j).toElement();
                QComboBox *combobox = widget->findChild<QComboBox *>(boxelement.tagName());
                combobox->setCurrentIndex(boxelement.attribute("Index").toInt(ok));
            }
        }
        if(element.tagName() == "MyLineEdit"){
            QDomNodeList editlist = element.childNodes();
            for(int j = 0;j < editlist.size();j++){
                QDomElement editelement = editlist.at(j).toElement();
                MyLineEdit *lineedit = widget->findChild<MyLineEdit *>(editelement.tagName());
                lineedit->setText(editelement.attribute("Value"));
            }
        }
        if(element.tagName() == "QTableWidget"){
            QDomNodeList tablelist = element.childNodes();
            for(int j = 0;j < tablelist.size();j++){
                QDomElement tableelement = tablelist.at(j).toElement();
                QTableWidget *table = widget->findChild<QTableWidget *>(tableelement.tagName());
                if(!tableelement.hasChildNodes()){
                    for(int k = 0; k < table->rowCount();k++){
                        for(int l = 0;l < table->columnCount();l++){
                            table->setItem(k,l,new QTableWidgetItem(""));
                            //table->item(k,l)->setData(Qt::DisplayRole,"");
                        }
                    }
                } else {
                    QDomNodeList tablenodelist = tableelement.childNodes();
                    table->setRowCount(tablenodelist.size() / table->columnCount());
                    QString string;
                    for(int m = 0;m < tablenodelist.size();m++){
                        QDomElement tabledata = tablenodelist.at(m).toElement();
                        string.clear();
                        string.append(tabledata.attribute("Position"));
                        string = string.remove("(");
                        string = string.remove(")");
                        table->setItem(string.split(",").at(0).toInt(ok),string.split(",").at(1).toInt(ok),new QTableWidgetItem(tabledata.attribute("Value")));
                        //table->item(string.split(",").at(0).toInt(ok),string.split(",").at(1).toInt(ok))->setData(Qt::DisplayRole,tabledata.attribute("Value"));
                    }
                }
            }
        }
        if(element.tagName() == "FileInfo"){
            QMessageBox::information(NULL,tr("关于"),tr("文件创建于 %1 %2。").arg(element.attribute("CreatedDate")).arg(element.attribute("CreatedTime")),QMessageBox::Yes);
        }
    }
    file.close();

    return true;
}

bool IO::createSubWindow(MainWindow *window, QString windowname)
{
    return window->createSubWindow(windowname);
}
