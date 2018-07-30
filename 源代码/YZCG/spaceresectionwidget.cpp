#include "spaceresectionwidget.h"

SpaceResectionWidget::SpaceResectionWidget(QWidget *parent) :
    QWidget(parent)
{
    initUI();
    setObjectName("SpaceResection");
}

void SpaceResectionWidget::requestDataSlot(QString requester, int row, int col)
{
    emit requestData(this,requester,row,col);
}

void SpaceResectionWidget::replyDataSlot(QWidget *parent, QString target, QStringList list, int row, int col)
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

void SpaceResectionWidget::chooseGPDataBtnSlot()
{
    emit requestData(this,CPTable->objectName(),4,5);
}

void SpaceResectionWidget::calculateBtnSlot()
{
    calculateBtn->setDisabled(true);

    ResectionStruct rs;

    const int pointcount = CPTable->rowCount();
    double *interior = new double[3];
    interior[0] = x0LineEdit->text().toDouble();
    interior[1] = y0LineEdit->text().toDouble();
    interior[2] = fLineEdit->text().toDouble();

    double *scale = new double(sLineEdit->text().toDouble());
    double *limit = new double(fLineEdit->text().toDouble());
    double *pointpos = new double[CPTable->rowCount() * CPTable->columnCount()];
    for(int i = 0;i < CPTable->rowCount();i++)
    {
        for(int j = 0; j < CPTable->columnCount();j++)
        {
            pointpos[i * CPTable->columnCount() + j] = CPTable->item(i,j)->text().toDouble();
        }
    }

    double *rotatematrix = new double[9];
    double *exterior = new double[6];
    double *m0 = new double(0.0);
    double *matrix = new double[36];
    double *precision = new double[6];

    rs.pointcount = pointcount;
    rs.interior = interior;
    rs.scale = scale;
    rs.limit = limit;
    rs.pointpos = pointpos;
    rs.rotatematrix = rotatematrix;
    rs.exterior = exterior;
    rs.m0 = m0;
    rs.matrix = matrix;
    rs.precision = precision;

    Utilities::fillTableItem(matrixTable,3,3);
    Utilities::fillTableItem(exteriorTable,6,1);

    if(methodBox->currentIndex() == 0){
        //欧拉角
        if(runResectionEuler(rs)){
            for(int i = 0;i < 3;i++){
                for(int j = 0;j < 3;j++){
                    //matrixTable->setItem(i,j,new QTableWidgetItem(tr("%1").arg(rs.rotatematrix[i * 3 + j])));
                    matrixTable->item(i,j)->setData(Qt::DisplayRole,rs.rotatematrix[i * 3 + j]);
                }
            }

//            exteriorTable->setItem(0,0,new QTableWidgetItem(tr("Xs = %1 ± %2 m").arg(rs.exterior[0]).arg(rs.precision[0])));
//            exteriorTable->setItem(1,0,new QTableWidgetItem(tr("Ys = %1 ± %2 m").arg(rs.exterior[1]).arg(rs.precision[1])));
//            exteriorTable->setItem(2,0,new QTableWidgetItem(tr("Zs = %1 ± %2 m").arg(rs.exterior[2]).arg(rs.precision[2])));
//            exteriorTable->setItem(3,0,new QTableWidgetItem(tr(" φ = %1 ± %2 rad").arg(rs.exterior[3]).arg(rs.precision[3])));
//            exteriorTable->setItem(4,0,new QTableWidgetItem(tr(" ω = %1 ± %2 rad").arg(rs.exterior[4]).arg(rs.precision[4])));
//            exteriorTable->setItem(5,0,new QTableWidgetItem(tr(" κ = %1 ± %2 rad").arg(rs.exterior[5]).arg(rs.precision[5])));

            exteriorTable->item(0,0)->setData(Qt::DisplayRole,tr("Xs = %1 ± %2 m").arg(rs.exterior[0]).arg(rs.precision[0]));
            exteriorTable->item(1,0)->setData(Qt::DisplayRole,tr("Ys = %1 ± %2 m").arg(rs.exterior[1]).arg(rs.precision[1]));
            exteriorTable->item(2,0)->setData(Qt::DisplayRole,tr("Zs = %1 ± %2 m").arg(rs.exterior[2]).arg(rs.precision[2]));
            exteriorTable->item(3,0)->setData(Qt::DisplayRole,tr(" φ = %1 ± %2 rad").arg(rs.exterior[3]).arg(rs.precision[3]));
            exteriorTable->item(4,0)->setData(Qt::DisplayRole,tr(" ω = %1 ± %2 rad").arg(rs.exterior[4]).arg(rs.precision[4]));
            exteriorTable->item(5,0)->setData(Qt::DisplayRole,tr(" κ = %1 ± %2 rad").arg(rs.exterior[5]).arg(rs.precision[5]));
        }
    }else{
        if(runResectionQuaternion(rs)){
            for(int i = 0;i < 3;i++){
                for(int j = 0;j < 3;j++){
                    //matrixTable->setItem(i,j,new QTableWidgetItem(tr("%1").arg(rs.rotatematrix[i * 3 + j])));
                    matrixTable->item(i,j)->setData(Qt::DisplayRole,rs.rotatematrix[i * 3 + j]);
                }
            }

//            exteriorTable->setItem(0,0,new QTableWidgetItem(tr("Xs = %1 ± %2 m").arg(rs.exterior[0]).arg(rs.precision[0])));
//            exteriorTable->setItem(1,0,new QTableWidgetItem(tr("Ys = %1 ± %2 m").arg(rs.exterior[1]).arg(rs.precision[1])));
//            exteriorTable->setItem(2,0,new QTableWidgetItem(tr("Zs = %1 ± %2 m").arg(rs.exterior[2]).arg(rs.precision[2])));
//            exteriorTable->setItem(3,0,new QTableWidgetItem(tr(" φ = %1 ± %2 rad").arg(rs.exterior[3]).arg(rs.precision[3])));
//            exteriorTable->setItem(4,0,new QTableWidgetItem(tr(" ω = %1 ± %2 rad").arg(rs.exterior[4]).arg(rs.precision[4])));
//            exteriorTable->setItem(5,0,new QTableWidgetItem(tr(" κ = %1 ± %2 rad").arg(rs.exterior[5]).arg(rs.precision[5])));

            exteriorTable->item(0,0)->setData(Qt::DisplayRole,tr("Xs = %1 ± %2 m").arg(rs.exterior[0]).arg(rs.precision[0]));
            exteriorTable->item(1,0)->setData(Qt::DisplayRole,tr("Ys = %1 ± %2 m").arg(rs.exterior[1]).arg(rs.precision[1]));
            exteriorTable->item(2,0)->setData(Qt::DisplayRole,tr("Zs = %1 ± %2 m").arg(rs.exterior[2]).arg(rs.precision[2]));
            exteriorTable->item(3,0)->setData(Qt::DisplayRole,tr(" φ = %1 ± %2 rad").arg(rs.exterior[3]).arg(rs.precision[3]));
            exteriorTable->item(4,0)->setData(Qt::DisplayRole,tr(" ω = %1 ± %2 rad").arg(rs.exterior[4]).arg(rs.precision[4]));
            exteriorTable->item(5,0)->setData(Qt::DisplayRole,tr(" κ = %1 ± %2 rad").arg(rs.exterior[5]).arg(rs.precision[5]));

        }
    }

    releaseResectionStruct(rs);

    calculateBtn->setEnabled(true);
}

void SpaceResectionWidget::initUI()
{
    methodLabel = new QLabel(tr("方法"),this);
    methodBox = new QComboBox(this);
    methodBox->setObjectName("methodBox");
    methodBox->addItem(tr("欧拉角"));
    methodBox->addItem(tr("单位四元数"));

    x0Label = new QLabel(tr("<h2>x<sub>0</sub></h2>"),this);
    y0Label = new QLabel(tr("<h2>y<sub>0</sub></h2>"),this);
    fLabel = new QLabel(tr("<h2>f</h2>"),this);
    sLabel = new QLabel(tr("像片比例尺分母"),this);
    pLabel = new QLabel(tr("限差"),this);

    x0LineEdit = new MyLineEdit(this,"x0LineEdit");
    y0LineEdit = new MyLineEdit(this,"y0LineEdit");
    fLineEdit = new MyLineEdit(this,"fLineEdit");
    sLineEdit = new MyLineEdit(this,"sLineEdit");
    pLineEdit = new MyLineEdit(this,"pLineEdit");


    connect(x0LineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(y0LineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(fLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(sLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(pLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));

    chooseCPDataBtn = new QPushButton(tr("选择数据"),this);

    CPTable = new QTableWidget(this);
        CPTable->setObjectName("CPTable");
    CPTable->setColumnCount(5);
    Utilities::setTableAttributes(CPTable);
    CPTableHeader << "x(mm)" << "y(mm)" << "X(m)" << "Y(m)" << "Z(m)";
    CPTable->setHorizontalHeaderLabels(CPTableHeader);

    matrixTable = new QTableWidget(3,3,this);
    matrixTable->setObjectName("matrixTable");
    matrixTable->horizontalHeader()->hide();
    matrixTable->verticalHeader()->hide();
    matrixTable->setShowGrid(false);
    Utilities::setTableAttributes(matrixTable);
    matrixTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);

    exteriorTable = new QTableWidget(6,1,this);
    exteriorTable->setObjectName("exteriorTable");
    exteriorTable->horizontalHeader()->hide();
    exteriorTable->verticalHeader()->hide();
    exteriorTable->setShowGrid(false);
    Utilities::setTableAttributes(exteriorTable);
    exteriorTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);

    calculateBtn = new QPushButton(tr("计算"),this);
    connect(calculateBtn,SIGNAL(clicked()),this,SLOT(calculateBtnSlot()));
    mainLayout = new QVBoxLayout;

    QGroupBox *parameterBox = new QGroupBox(tr("基本参数"));
    QGridLayout *parameterLayout = new QGridLayout;
    parameterLayout->addWidget(methodLabel,0,0,1,1,Qt::AlignLeft);
    parameterLayout->addWidget(x0Label,0,1,1,1,Qt::AlignLeft);
    parameterLayout->addWidget(y0Label,0,2,1,1,Qt::AlignLeft);
    parameterLayout->addWidget(fLabel,0,3,1,1,Qt::AlignLeft);
    parameterLayout->addWidget(sLabel,0,4,1,1,Qt::AlignLeft);
    parameterLayout->addWidget(pLabel,0,5,1,1,Qt::AlignLeft);

    parameterLayout->addWidget(methodBox,1,0,1,1);
    parameterLayout->addWidget(x0LineEdit,1,1,1,1);
    parameterLayout->addWidget(y0LineEdit,1,2,1,1);
    parameterLayout->addWidget(fLineEdit,1,3,1,1);
    parameterLayout->addWidget(sLineEdit,1,4,1,1);
    parameterLayout->addWidget(pLineEdit,1,5,1,1);

    parameterBox->setLayout(parameterLayout);
    mainLayout->addWidget(parameterBox);

    QGroupBox *CPBox = new QGroupBox(tr("控制点数据"));
    QVBoxLayout *CPLayout = new QVBoxLayout;
    CPLayout->addWidget(chooseCPDataBtn);
    CPLayout->addWidget(CPTable);
    CPBox->setLayout(CPLayout);
    mainLayout->addWidget(CPBox);

    mainLayout->addWidget(calculateBtn);

    QGroupBox *outPutBox = new QGroupBox(tr("空间后方交会"));
    QHBoxLayout *outPutLayout = new QHBoxLayout;

    QGroupBox *matrixBox = new QGroupBox(tr("旋转矩阵"));
    QVBoxLayout *matrixLayout = new QVBoxLayout;
    matrixLayout->addWidget(matrixTable);
    matrixBox->setLayout(matrixLayout);

    QGroupBox *exteriorBox = new QGroupBox(tr("外方位元素"));
    QVBoxLayout *exteriorLayout = new QVBoxLayout;
    exteriorLayout->addWidget(exteriorTable);
    exteriorBox->setLayout(exteriorLayout);

    outPutLayout->addWidget(matrixBox);
    outPutLayout->addWidget(exteriorBox);

    outPutBox->setLayout(outPutLayout);

    mainLayout->addWidget(outPutBox);

    setLayout(mainLayout);

    connect(chooseCPDataBtn,SIGNAL(clicked()),this,SLOT(chooseGPDataBtnSlot()));
}
