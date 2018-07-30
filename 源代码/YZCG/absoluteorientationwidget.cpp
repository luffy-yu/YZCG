#include "absoluteorientationwidget.h"

AbsoluteOrientationWidget::AbsoluteOrientationWidget(QWidget *parent) :
    QWidget(parent)
{
    initUI();
    setObjectName("AbsoluteOrientation");
}

void AbsoluteOrientationWidget::requestDataSlot(QString requester, int row, int col)
{
    emit requestData(this,requester,row,col);
}

void AbsoluteOrientationWidget::replyDataSlot(QWidget *parent, QString target, QStringList list, int row, int col)
{
    if(parent != this)
        return;
    if(col == 1){
        MyLineEdit *edit = findChild<MyLineEdit*>(target);
#ifdef QT_DEBUG
        qDebug()<<list.at(0);
#endif
        edit->setText(list.at(0));
    }else{
        QTableWidget *table = findChild<QTableWidget*>(target);
        table->setRowCount(row);

        for(int i = 0;i < row;i++){
            for(int j = 0;j < col;j++){
                QTableWidgetItem *item = new QTableWidgetItem(list.at(i * col + j));
                table->setItem(i,j,item);
                //table->item(i,j)->setData(Qt::DisplayRole,list.at(i * col + j));
            }
        }
    }
}

void AbsoluteOrientationWidget::chooseMDBtnSlot()
{
    emit requestData(this,MDTable->objectName(),7,4);
}

void AbsoluteOrientationWidget::chooseGDBtnslot()
{
    emit requestData(this,GDTable->objectName(),7,4);
}

void AbsoluteOrientationWidget::calculateBtnSlot()
{
    calculateBtn->setDisabled(true);
    AbsoluteOrientationStruct aos;

    const int pointcount = MDTable->rowCount();

    double *limit = new double(pLineEdit->text().toDouble());
    double *modelpos = new double[MDTable->rowCount() * 3];
    double *controlpos = new double[MDTable->rowCount() * 3];
    double *result = new double[7];
    double *precision = new double[7];
    double *groundpos = new double[MDTable->rowCount() * 3];
    double *deltaxyz = new double[MDTable->rowCount() * 3];

    for(int i = 0;i < MDTable->rowCount();i++){
        for(int j = 1;j < MDTable->columnCount();j++){
            modelpos[i * 3 + j - 1] = MDTable->item(i,j)->text().toDouble();
            controlpos[i * 3 + j - 1] = GDTable->item(i,j)->text().toDouble();
        }
    }

    aos.pointcount = pointcount;
    aos.limit = limit;
    aos.modelpos = modelpos;
    aos.controlpos = controlpos;
    aos.result = result;
    aos.precision = precision;
    aos.groundpos = groundpos;
    aos.deltaxyz = deltaxyz;

    Utilities::fillTableItem(absoluteTable,2,7);
    Utilities::fillTableItem(resultTable,pointcount,7);

    if(runAbsoluteOrientation(aos)){
        for(int i = 0;i < 7;i++){
            //absoluteTable->setItem(0,i,new QTableWidgetItem(tr("%1").arg(aos.result[i])));
            //absoluteTable->setItem(1,i,new QTableWidgetItem(tr("± %1").arg(aos.precision[i])));

            absoluteTable->item(0,i)->setData(Qt::DisplayRole,tr("%1").arg(aos.result[i]));
            absoluteTable->item(1,i)->setData(Qt::DisplayRole,tr("± %1").arg(aos.precision[i]));
        }

        for(int i = 0;i < pointcount;i++)
        {
            //resultTable->setItem(i,0,new QTableWidgetItem(MDTable->item(i,0)->text()));
            resultTable->item(i,0)->setData(Qt::DisplayRole,MDTable->item(i,0)->text());
            for(int j = 1;j < 4;j++)
            {
                //resultTable->setItem(i,j,new QTableWidgetItem(tr("%1").arg(aos.groundpos[3 * i + j - 1])));
                //resultTable->setItem(i,j + 3,new QTableWidgetItem(tr("%1").arg(aos.deltaxyz[3 * i + j - 1])));

                resultTable->item(i,j)->setData(Qt::DisplayRole,aos.groundpos[3 * i + j - 1]);
                resultTable->item(i,j + 3)->setData(Qt::DisplayRole,aos.deltaxyz[3 * i + j - 1]);
            }
        }
    }

    releaseAbsoluteOrientationStruct(aos);
    calculateBtn->setEnabled(true);
}

void AbsoluteOrientationWidget::initUI()
{
    pLabel = new QLabel(tr("限差"),this);
    pLineEdit = new MyLineEdit(this,"pLineEdit");
    chooseMDBtn = new QPushButton(tr("选择数据"),this);
    chooseGDBtn = new QPushButton(tr("选择数据"),this);

    connect(pLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(chooseMDBtn,SIGNAL(clicked()),this,SLOT(chooseMDBtnSlot()));
    connect(chooseGDBtn,SIGNAL(clicked()),this,SLOT(chooseGDBtnslot()));

    MDTableHeader << tr("点号") << tr("Xp") << tr("Yp") << tr("Zp");
    GDTableHeader << tr("点号") << tr("Xt") << tr("Yt") << tr("Zt");
    absoluteHeader << tr("ΔX") << tr("ΔY") << tr("ΔZ") << tr("λ") << tr("Φ") << tr("Ω") << tr("Κ");
    resultHeader << tr("点号") << tr("X") << tr("Y") << tr("Z") << tr("ΔX") << tr("ΔY") << tr("ΔZ");

    MDTable = new QTableWidget(this);
    MDTable->setObjectName("MDTable");
    MDTable->setColumnCount(4);
    Utilities::setTableAttributes(MDTable);
    MDTable->setHorizontalHeaderLabels(MDTableHeader);

    GDTable = new QTableWidget(this);
    GDTable->setObjectName("GDTable");
    GDTable->setColumnCount(4);
    Utilities::setTableAttributes(GDTable);
    GDTable->setHorizontalHeaderLabels(GDTableHeader);

    calculateBtn = new QPushButton(tr("计算"),this);
    connect(calculateBtn,SIGNAL(clicked()),this,SLOT(calculateBtnSlot()));

    absoluteTable = new QTableWidget(2,7,this);
    absoluteTable->setObjectName("absoluteTable");
    absoluteTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    absoluteTable->verticalHeader()->hide();
    absoluteTable->setRowHeight(0,30);
    absoluteTable->setMaximumHeight(100);
    absoluteTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    absoluteTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    absoluteTable->setHorizontalHeaderLabels(absoluteHeader);

    resultTable = new QTableWidget(this);
    resultTable->setObjectName("resultTable");
    resultTable->setColumnCount(7);
    Utilities::setTableAttributes(resultTable);
    resultTable->setHorizontalHeaderLabels(resultHeader);

    mainLayout = new QVBoxLayout;

    QGroupBox *paraBox = new QGroupBox(tr("基本参数"));
    QHBoxLayout *paraLayout = new QHBoxLayout;

    paraLayout->addWidget(pLabel);
    paraLayout->addWidget(pLineEdit);
    paraBox->setLayout(paraLayout);

    mainLayout->addWidget(paraBox);

    QGroupBox *pointsBox = new QGroupBox(tr("控制坐标"));
    QHBoxLayout *pointsLayout = new QHBoxLayout;

    QGroupBox *modelBox = new QGroupBox(tr("模型坐标"));
    QVBoxLayout *modelLayout = new QVBoxLayout;

    modelLayout->addWidget(chooseMDBtn);
    modelLayout->addWidget(MDTable);
    modelBox->setLayout(modelLayout);

    pointsLayout->addWidget(modelBox);

    QGroupBox *groundBox = new QGroupBox(tr("地面测量坐标"));
    QVBoxLayout *groundLayout = new QVBoxLayout;
    groundLayout->addWidget(chooseGDBtn);
    groundLayout->addWidget(GDTable);
    groundBox->setLayout(groundLayout);

    pointsLayout->addWidget(groundBox);
    pointsBox->setLayout(pointsLayout);

    mainLayout->addWidget(pointsBox);

    mainLayout->addWidget(calculateBtn);

    QGroupBox *absoluteBox = new QGroupBox(tr("绝对定向"));
    QVBoxLayout *absoluteLayout = new QVBoxLayout;

    QGroupBox *elementsBox = new QGroupBox(tr("绝对方位元素"));
    QVBoxLayout *elementsLayout = new QVBoxLayout;


    elementsLayout->addWidget(absoluteTable);
    elementsBox->setLayout(elementsLayout);

    absoluteLayout->addWidget(elementsBox);

    QGroupBox *resultBox = new QGroupBox(tr("地面摄测坐标"));
    QVBoxLayout *resultLayout = new QVBoxLayout;

    resultLayout->addWidget(resultTable);
    resultBox->setLayout(resultLayout);

    absoluteLayout->addWidget(resultBox);
    absoluteBox->setLayout(absoluteLayout);

    mainLayout->addWidget(absoluteBox);

    setLayout(mainLayout);
}
