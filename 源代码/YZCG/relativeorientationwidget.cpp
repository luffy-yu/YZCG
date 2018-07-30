#include "relativeorientationwidget.h"

RelativeOrientationWidget::RelativeOrientationWidget(QWidget *parent) :
    QWidget(parent)
{
    initUI();
    setObjectName("RelativeOrientation");
}

void RelativeOrientationWidget::requestDataSlot(QString requester, int row, int col)
{
    emit requestData(this,requester,row,col);
}

void RelativeOrientationWidget::replyDataSlot(QWidget *parent, QString target, QStringList list, int row, int col)
{
    if(parent != this)
        return;
    if(col == 1){
        MyLineEdit *edit = findChild<MyLineEdit*>(target);
        edit->setText(list.at(0));
    }else{
        QTableWidget *table = findChild<QTableWidget*>(target);
        table->setRowCount(row);

        for(int i = 0;i < row;i++){
            for(int j = 0;j < col;j++){
                QTableWidgetItem *item = new QTableWidgetItem(list.at(i * col + j));
                table->setItem(i,j,item);
//                table->item(i,j)->setData(Qt::DisplayRole,list.at(i * col + j));
            }
        }
    }
}

void RelativeOrientationWidget::chooseSPBtnSlot()
{
    emit requestData(this,SPTable->objectName(),6,5);
}

void RelativeOrientationWidget::setElementsTableHeader(int index)
{
    if(index == 0){
        elementsTable->setHorizontalHeaderLabels(elementsTableHeader1);
    }else{
        elementsTable->setHorizontalHeaderLabels(elementsTableHeader2);
    }
}

void RelativeOrientationWidget::calculateBtnSlot()
{
    calculateBtn->setDisabled(true);

    RelativeOrientationStruct ros;

    const int pointcount = SPTable->rowCount();

    double *interior = new double[3];
    interior[0] = x0LineEdit->text().toDouble();
    interior[1] = y0LineEdit->text().toDouble();
    interior[2] = fLineEdit->text().toDouble();

    double *m = new double(mLineEdit->text().toDouble());
    double *limit = new double(pLineEdit->text().toDouble());

    double *samepoint = new double[SPTable->rowCount() * 4];
    for(int i = 0;i < SPTable->rowCount();i++)
    {
        for(int j = 1;j < SPTable->columnCount();j++)
        {
            samepoint[i * 4 + j - 1] = SPTable->item(i,j)->text().toDouble();
        }
    }

    double *result = new double[5];
    double *precision = new double[5];
    double *modelpos = new double[SPTable->rowCount() * 3];

    ros.pointcount = pointcount;
    ros.interior = interior;
    ros.m = m;
    ros.limit = limit;
    ros.samepoint = samepoint;
    ros.result = result;
    ros.precision = precision;
    ros.modelpos = modelpos;


    Utilities::fillTableItem(elementsTable,2,5);
    Utilities::fillTableItem(resultTable,SPTable->rowCount(),4);

    if(methodBox->currentIndex() == 0){
        //单独法
        if(runRelativeOrientationS(ros)){
            for(int i = 0;i < 5;i++){
                //elementsTable->setItem(0,i,new QTableWidgetItem(tr("%1").arg(ros.result[i])));
                //elementsTable->setItem(1,i,new QTableWidgetItem(tr("± %1").arg(ros.precision[i])));

                elementsTable->item(0,i)->setData(Qt::DisplayRole,tr("%1").arg(ros.result[i]));
                elementsTable->item(1,i)->setData(Qt::DisplayRole,tr("± %1").arg(ros.precision[i]));
            }
            for(int i = 0;i < resultTable->rowCount();i++)
            {
                //resultTable->setItem(i,0,new QTableWidgetItem(SPTable->item(i,0)->text()));
                resultTable->item(i,0)->setData(Qt::DisplayRole,SPTable->item(i,0)->text());

                for(int j = 1;j < resultTable->columnCount();j++)
                {
                    //resultTable->setItem(i,j,new QTableWidgetItem(tr("%1").arg(ros.modelpos[i * 3 + j - 1])));
                    resultTable->item(i,j)->setData(Qt::DisplayRole,ros.modelpos[i * 3 + j - 1]);
                }
            }
        }
    }else{
        //连续法
        if(runRelativeOrientationC(ros)){
            for(int i = 0;i < 5;i++){
                //elementsTable->setItem(0,i,new QTableWidgetItem(tr("%1").arg(ros.result[i])));
                //elementsTable->setItem(1,i,new QTableWidgetItem(tr("± %1").arg(ros.precision[i])));

                elementsTable->item(0,i)->setData(Qt::DisplayRole,tr("%1").arg(ros.result[i]));
                elementsTable->item(1,i)->setData(Qt::DisplayRole,tr("± %1").arg(ros.precision[i]));
            }
            for(int i = 0;i < resultTable->rowCount();i++)
            {
                //resultTable->setItem(i,0,new QTableWidgetItem(SPTable->item(i,0)->text()));
                resultTable->item(i,0)->setData(Qt::DisplayRole,SPTable->item(i,0)->text());

                for(int j = 1;j < resultTable->columnCount();j++)
                {
                    //resultTable->setItem(i,j,new QTableWidgetItem(tr("%1").arg(ros.modelpos[i * 3 + j - 1])));
                    resultTable->item(i,j)->setData(Qt::DisplayRole,ros.modelpos[i * 3 + j - 1]);
                }
            }
        }
    }

    releaseRelativeOrientationStruct(ros);

    calculateBtn->setEnabled(true);
}

void RelativeOrientationWidget::initUI()
{
    mainLayout = new QVBoxLayout;

    methodLabel = new QLabel(tr("像对"),this);
    x0Label = new QLabel(tr("<h2>x<sub>0</sub></h2>"),this);
    y0Label = new QLabel(tr("<h2>y<sub>0</sub></h2>"),this);
    fLabel = new QLabel(tr("<h2>f</h2>"),this);
    mLabel = new QLabel(tr("像片比例尺分母"),this);
    pLabel = new QLabel(tr("限差"),this);

    methodBox = new QComboBox(this);
    methodBox->setObjectName("methodBox");
    methodBox->addItem(tr("单独像对"));
    methodBox->addItem(tr("连续像对"));
    connect(methodBox,SIGNAL(activated(int)),this,SLOT(setElementsTableHeader(int)));

    x0LineEdit = new MyLineEdit(this,"x0LineEdit");
    y0LineEdit = new MyLineEdit(this,"y0LineEdit");
    fLineEdit = new MyLineEdit(this,"fLineEdit");
    mLineEdit = new MyLineEdit(this,"mLineEdit");
    pLineEdit = new MyLineEdit(this,"pLineEdit");

    connect(x0LineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(y0LineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(fLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(mLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(pLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));

    elementsTable = new QTableWidget(2,5,this);
    elementsTable->setObjectName("elementsTable");
    elementsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    elementsTable->verticalHeader()->hide();
    elementsTable->setRowHeight(0,30);
    elementsTable->setMaximumHeight(100);
    elementsTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    elementsTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    elementsTableHeader1 << tr("φ1") << tr("κ1") << tr("φ2") << tr("ω2") << tr("κ2");
    elementsTableHeader2 << tr("BY") << tr("BZ") << tr("φ2") << tr("ω2") << tr("κ2");
    elementsTable->setHorizontalHeaderLabels(elementsTableHeader1);

    chooseSPBtn = new QPushButton(tr("选择数据"),this);
    connect(chooseSPBtn,SIGNAL(clicked()),this,SLOT(chooseSPBtnSlot()));
    calculateBtn = new QPushButton(tr("计算"),this);
    connect(calculateBtn,SIGNAL(clicked()),this,SLOT(calculateBtnSlot()));

    SPTable = new QTableWidget(this);
    SPTable->setObjectName("SPTable");
    SPTable->setColumnCount(5);
    Utilities::setTableAttributes(SPTable);
    SPTableHeader << tr("点号") << tr("x1") << tr("y1") << tr("x2") << tr("y2");
    SPTable->setHorizontalHeaderLabels(SPTableHeader);

    resultTable = new QTableWidget(this);
    resultTable->setObjectName("resultTable");
    resultTable->setColumnCount(4);
    Utilities::setTableAttributes(resultTable);
    resultTableHeader << tr("点号") << tr("Xs2") << tr("Ys2") << tr("Zs2");
    resultTable->setHorizontalHeaderLabels(resultTableHeader);

    QGroupBox *parasBox = new QGroupBox(tr("基本参数"));
    QGridLayout *parasLayout = new QGridLayout;

    parasLayout->addWidget(methodLabel,0,0,1,1,Qt::AlignLeft);
    parasLayout->addWidget(x0Label,0,1,1,1,Qt::AlignLeft);
    parasLayout->addWidget(y0Label,0,2,1,1,Qt::AlignLeft);
    parasLayout->addWidget(fLabel,0,3,1,1,Qt::AlignLeft);
    parasLayout->addWidget(mLabel,0,4,1,1,Qt::AlignLeft);
    parasLayout->addWidget(pLabel,0,5,1,1,Qt::AlignLeft);

    parasLayout->addWidget(methodBox,1,0,1,1);
    parasLayout->addWidget(x0LineEdit,1,1,1,1);
    parasLayout->addWidget(y0LineEdit,1,2,1,1);
    parasLayout->addWidget(fLineEdit,1,3,1,1);
    parasLayout->addWidget(mLineEdit,1,4,1,1);
    parasLayout->addWidget(pLineEdit,1,5,1,1);

    parasBox->setLayout(parasLayout);
    mainLayout->addWidget(parasBox);

    QGroupBox *SPBox = new QGroupBox(tr("同名像点坐标"));
    QVBoxLayout *SPLayout = new QVBoxLayout;

    SPLayout->addWidget(chooseSPBtn);
    SPLayout->addWidget(SPTable);
    SPBox->setLayout(SPLayout);
    mainLayout->addWidget(SPBox);

    mainLayout->addWidget(calculateBtn);

    QGroupBox *relativeBox = new QGroupBox(tr("相对定向"));
    QVBoxLayout *relativeLayout = new QVBoxLayout;

    QGroupBox *elementsBox = new QGroupBox(tr("相对方位元素"));
    QVBoxLayout *elementsLayout = new QVBoxLayout;

    elementsLayout->addWidget(elementsTable);
    elementsBox->setLayout(elementsLayout);

    relativeLayout->addWidget(elementsBox);

    QGroupBox *resultBox = new QGroupBox(tr("模型点坐标"));
    QVBoxLayout *resultLayout = new QVBoxLayout;

    resultLayout->addWidget(resultTable);
    resultBox->setLayout(resultLayout);

    relativeLayout->addWidget(resultBox);
    relativeBox->setLayout(relativeLayout);

    mainLayout->addWidget(relativeBox);

    setLayout(mainLayout);
}
