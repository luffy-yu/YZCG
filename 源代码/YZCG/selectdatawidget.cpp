#include "selectdatawidget.h"

SelectDataWidget::SelectDataWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowModality(Qt::ApplicationModal);

    listView = new QListView(this);
    oneView = new QListView(this);

    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    oneView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    listView->setAlternatingRowColors(true);
    oneView->setAlternatingRowColors(true);

    layout = new QVBoxLayout;
    layout->addWidget(listView);
    layout->addWidget(oneView);

    setLayout(layout);

    connect(listView,SIGNAL(clicked(QModelIndex)),this,SLOT(listViewClicked(QModelIndex)));
    connect(listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(listViewDoubleClicked(QModelIndex)));
    connect(oneView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(oneviewClicked(QModelIndex)));

    oneViewModel = new QStringListModel(this);

    oneView->hide();
    oneView->setModel(oneViewModel);
}

void SelectDataWidget::setListViewModel(QStandardItemModel *model)
{
    listView->setModel(model);
    listView->setModelColumn(0);
}

void SelectDataWidget::listViewClicked(QModelIndex index)
{
    QStandardItemModel *model = (QStandardItemModel*)listView->model();
    bool ok;
    int row = model->item(index.row(),1)->text().split("*").at(0).toInt(&ok);
    int col = model->item(index.row(),1)->text().split("*").at(1).toInt(&ok);

    if(requestCol == 1){
        if(row * col > 1){
            listData = model->item(index.row(),0)->text().split(",");
            oneViewModel->setStringList(listData);
            oneView->show();
        }else{
            oneView->hide();
        }
    }else{
        oneView->hide();
    }
}

void SelectDataWidget::listViewDoubleClicked(QModelIndex index)
{
    QStandardItemModel *model = (QStandardItemModel*)listView->model();
    bool ok;
    int row = model->item(index.row(),1)->text().split("*").at(0).toInt(&ok);
    int col = model->item(index.row(),1)->text().split("*").at(1).toInt(&ok);

    if((requestCol == 1)){
        if(row * col == 1){
            emit replyData(requesterParent,requesterName,
                            model->item(index.row(),0)->text().split(","),
                           1,1);
            close();
        }else{
            return;
        }
    }else{
        if((row >= requestRow) && (col == requestCol)){
            emit replyData(requesterParent,requesterName,
                           model->item(index.row(),0)->text().split(","),
                           row,col);
            close();
        }else{
            QMessageBox::information(this,tr("提示"),tr("所选数据不符合要求。（要求：行数>=%1，列数 = %2）") .arg(requestRow).arg(requestCol),QMessageBox::Ok);
        }
    }
}

void SelectDataWidget::oneviewClicked(QModelIndex index)
{
#ifdef QT_DEBUG
    qDebug()<<oneViewModel->data(index,Qt::DisplayRole).toString();
#endif
    emit replyData(requesterParent,requesterName,
                   QStringList(oneViewModel->data(index,Qt::DisplayRole).toString()),requestRow,requestCol);
    close();
}

void SelectDataWidget::requestDataSlot(QWidget *parent, QString requester, int row, int col)
{
    show();
    requesterParent = parent;
    requesterName = requester;
    requestRow = row;
    requestCol = col;

#ifdef QT_DEBUG
    qDebug()<<"SelectdataWidget:";
    qDebug()<<parent;
    qDebug()<<requester;
    qDebug()<<row;
    qDebug()<<col;
#endif
}
