#include "spaceintersectionwidget.h"

SpaceIntersectionWidget::SpaceIntersectionWidget(QWidget *parent) :
    QWidget(parent)
{
    initUI();
    setObjectName("SpaceIntersection");
}

void SpaceIntersectionWidget::requestDataSlot(QString requester, int row, int col)
{
   emit requestData(this,requester,row,col);
}

void SpaceIntersectionWidget::replyDataSlot(QWidget *parent, QString target, QStringList list, int row, int col)
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

void SpaceIntersectionWidget::chooseSPBtnSlot()
{
    emit requestData(this,SPTable->objectName(),1,5);
}

void SpaceIntersectionWidget::calculateBtnSlot()
{
    calculateBtn->setDisabled(true);

    IntersectionStruct is;
    const int pointcount = SPTable->rowCount();
    double *interior = new double[3];
    interior[0] = x0LineEdit->text().toDouble();
    interior[1] = y0LineEdit->text().toDouble();
    interior[2] = fLineEdit->text().toDouble();

    double *samepoint = new double[SPTable->rowCount() * 4];
    for(int i = 0;i < SPTable->rowCount();i++){
        for(int j = 1;j < 5;j++){
            samepoint[i * 4 + j - 1] = SPTable->item(i,j)->text().toDouble();
        }
    }

    double *leftexterior = new double[6];

    leftexterior[0] = LXsLineEdit->text().toDouble();
    leftexterior[1] = LYsLineEdit->text().toDouble();
    leftexterior[2] = LZsLineEdit->text().toDouble();
    leftexterior[3] = LphiLineEdit->text().toDouble();
    leftexterior[4] = LomegaLineEdit->text().toDouble();
    leftexterior[5] = LkappaLineEdit->text().toDouble();

    double *rightexterior = new double[6];

    rightexterior[0] = RXsLineEdit->text().toDouble();
    rightexterior[1] = RYsLineEdit->text().toDouble();
    rightexterior[2] = RZsLineEdit->text().toDouble();
    rightexterior[3] = RphiLineEdit->text().toDouble();
    rightexterior[4] = RomegaLineEdit->text().toDouble();
    rightexterior[5] = RkappaLineEdit->text().toDouble();

    double *result = new double[SPTable->rowCount() * 3];

    is.pointcount = pointcount;
    is.interior = interior;
    is.samepoint = samepoint;
    is.leftexterior = leftexterior;
    is.rightexterior = rightexterior;
    is.result = result;

    Utilities::fillTableItem(resultTable,SPTable->rowCount(),4);

    if(methodBox->currentIndex() == 0){
        //平差法
        if(runIntersectionAdj(is)){
            for(int i = 0;i < SPTable->rowCount();i++)
            {
                //resultTable->setItem(i,0,new QTableWidgetItem(SPTable->item(i,0)->text()));
                resultTable->item(i,0)->setData(Qt::DisplayRole,SPTable->item(i,0)->text());
                for(int j = 1;j < 4;j++)
                {
                    //resultTable->setItem(i,j,new QTableWidgetItem(tr("%1").arg(is.result[i * 3 + j - 1])));
                    resultTable->item(i,j)->setData(Qt::DisplayRole,is.result[i * 3 + j - 1]);
                }
            }
        }
    }else{
        //投影系数法
        if(runIntersectionPro(is)){
            for(int i = 0;i < SPTable->rowCount();i++)
            {
                //resultTable->setItem(i,0,new QTableWidgetItem(SPTable->item(i,0)->text()));
                resultTable->item(i,0)->setData(Qt::DisplayRole,SPTable->item(i,0)->text());
                for(int j = 1;j < 4;j++)
                {
                    //resultTable->setItem(i,j,new QTableWidgetItem(tr("%1").arg(is.result[i * 3 + j - 1])));
                    resultTable->item(i,j)->setData(Qt::DisplayRole,is.result[i * 3 + j - 1]);
                }
            }
        }
    }
    releaseIntersectionStruct(is);

    calculateBtn->setEnabled(true);
}

void SpaceIntersectionWidget::initUI()
{
    mainLayout = new QVBoxLayout;

    methodLabel = new QLabel(tr("方法"),this);
    methodBox = new QComboBox(this);
    methodBox->setObjectName("methodBox");
    methodBox->addItem(tr("平差法"));
    methodBox->addItem(tr("投影系数法"));

    x0Label = new QLabel(tr("<h2>x<sub>0</sub></h2>"),this);
    y0Label = new QLabel(tr("<h2>y<sub>0</sub></h2>"),this);
    fLabel = new QLabel(tr("<h2>f</h2>"),this);

    x0LineEdit = new MyLineEdit(this,"x0LineEdit");
    y0LineEdit = new MyLineEdit(this,"y0LineEdit");
    fLineEdit = new MyLineEdit(this,"fLineEdit");

    connect(x0LineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(y0LineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(fLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));

    LXsLabel = new QLabel(tr("Xs"),this);
    LYsLabel = new QLabel(tr("Ys"),this);
    LZsLabel = new QLabel(tr("Zs"),this);
    LphiLabel = new QLabel(tr("φ"),this);
    LomegaLabel = new QLabel(tr("ω"),this);
    LkappaLabel = new QLabel(tr("κ"),this);

    LXsLineEdit = new MyLineEdit(this,"LXsLineEdit");
    LYsLineEdit = new MyLineEdit(this,"LYsLineEdit");
    LZsLineEdit = new MyLineEdit(this,"LZsLineEdit");
    LphiLineEdit = new MyLineEdit(this,"LphiLineEdit");
    LomegaLineEdit = new MyLineEdit(this,"LomegaLineEdit");
    LkappaLineEdit = new MyLineEdit(this,"LkappaLineEdit");

    connect(LXsLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(LYsLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(LZsLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(LphiLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(LomegaLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(LkappaLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));

    RXsLabel = new QLabel(tr("Xs"),this);
    RYsLabel = new QLabel(tr("Ys"),this);
    RZsLabel = new QLabel(tr("Zs"),this);
    RphiLabel = new QLabel(tr("φ"),this);
    RomegaLabel = new QLabel(tr("ω"),this);
    RkappaLabel = new QLabel(tr("κ"),this);

    RXsLineEdit = new MyLineEdit(this,"RXsLineEdit");
    RYsLineEdit = new MyLineEdit(this,"RYsLineEdit");
    RZsLineEdit = new MyLineEdit(this,"RZsLineEdit");
    RphiLineEdit = new MyLineEdit(this,"RphiLineEdit");
    RomegaLineEdit = new MyLineEdit(this,"RomegaLineEdit");
    RkappaLineEdit = new MyLineEdit(this,"RkappaLineEdit");

    connect(RXsLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(RYsLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(RZsLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(RphiLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(RomegaLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));
    connect(RkappaLineEdit,SIGNAL(requestData(QString,int,int)),this,SLOT(requestDataSlot(QString,int,int)));

    chooseSPBtn = new QPushButton(tr("选择数据"),this);
    connect(chooseSPBtn,SIGNAL(clicked()),this,SLOT(chooseSPBtnSlot()));

    calculateBtn = new QPushButton(tr("计算"),this);
    connect(calculateBtn,SIGNAL(clicked()),this,SLOT(calculateBtnSlot()));
    SPTable = new QTableWidget(this);
    SPTable->setObjectName("SPTable");
    SPTable->setColumnCount(5);
    resultTable = new QTableWidget(this);
    resultTable->setObjectName("resultTable");
    resultTable->setColumnCount(4);

    Utilities::setTableAttributes(SPTable);
    SPTableHeader << tr("点号") << "x1" << "y1" << "x2" << "y2";
    SPTable->setHorizontalHeaderLabels(SPTableHeader);

    Utilities::setTableAttributes(resultTable);
    resultTableHeader << tr("点号") << "X" << "Y" << "Z";
    resultTable->setHorizontalHeaderLabels(resultTableHeader);

    QGroupBox *parasBox = new QGroupBox(tr("基本参数"));
    QGridLayout *parasLayout = new QGridLayout;

    parasLayout->addWidget(methodLabel,0,0,1,1,Qt::AlignLeft);
    parasLayout->addWidget(x0Label,0,1,1,1,Qt::AlignLeft);
    parasLayout->addWidget(y0Label,0,2,1,1,Qt::AlignLeft);
    parasLayout->addWidget(fLabel,0,3,1,1,Qt::AlignLeft);

    parasLayout->addWidget(methodBox,1,0,1,1);
    parasLayout->addWidget(x0LineEdit,1,1,1,1);
    parasLayout->addWidget(y0LineEdit,1,2,1,1);
    parasLayout->addWidget(fLineEdit,1,3,1,1);

    parasBox->setLayout(parasLayout);

    mainLayout->addWidget(parasBox);

    QGroupBox *exteriorsBox = new QGroupBox(tr("外方位元素"));
    QVBoxLayout *exteriorLayout = new QVBoxLayout;

    QGroupBox *leftBox = new QGroupBox(tr("左片"));
    QGridLayout *leftLayout = new QGridLayout;

    QGroupBox *rightBox = new QGroupBox(tr("右片"));
    QGridLayout *rightLayout = new QGridLayout;

    leftLayout->addWidget(LXsLabel,0,0,1,1,Qt::AlignLeft);
    leftLayout->addWidget(LYsLabel,0,1,1,1,Qt::AlignLeft);
    leftLayout->addWidget(LZsLabel,0,2,1,1,Qt::AlignLeft);
    leftLayout->addWidget(LphiLabel,0,3,1,1,Qt::AlignLeft);
    leftLayout->addWidget(LomegaLabel,0,4,1,1,Qt::AlignLeft);
    leftLayout->addWidget(LkappaLabel,0,5,1,1,Qt::AlignLeft);

    leftLayout->addWidget(LXsLineEdit,1,0,1,1);
    leftLayout->addWidget(LYsLineEdit,1,1,1,1);
    leftLayout->addWidget(LZsLineEdit,1,2,1,1);
    leftLayout->addWidget(LphiLineEdit,1,3,1,1);
    leftLayout->addWidget(LomegaLineEdit,1,4,1,1);
    leftLayout->addWidget(LkappaLineEdit,1,5,1,1);

    leftBox->setLayout(leftLayout);
    exteriorLayout->addWidget(leftBox);

    rightLayout->addWidget(RXsLabel,0,0,1,1,Qt::AlignLeft);
    rightLayout->addWidget(RYsLabel,0,1,1,1,Qt::AlignLeft);
    rightLayout->addWidget(RZsLabel,0,2,1,1,Qt::AlignLeft);
    rightLayout->addWidget(RphiLabel,0,3,1,1,Qt::AlignLeft);
    rightLayout->addWidget(RomegaLabel,0,4,1,1,Qt::AlignLeft);
    rightLayout->addWidget(RkappaLabel,0,5,1,1,Qt::AlignLeft);

    rightLayout->addWidget(RXsLineEdit,1,0,1,1);
    rightLayout->addWidget(RYsLineEdit,1,1,1,1);
    rightLayout->addWidget(RZsLineEdit,1,2,1,1);
    rightLayout->addWidget(RphiLineEdit,1,3,1,1);
    rightLayout->addWidget(RomegaLineEdit,1,4,1,1);
    rightLayout->addWidget(RkappaLineEdit,1,5,1,1);

    rightBox->setLayout(rightLayout);
    exteriorLayout->addWidget(rightBox);

    exteriorsBox->setLayout(exteriorLayout);

    mainLayout->addWidget(exteriorsBox);

    QGroupBox *pointResultBox = new QGroupBox;
    QHBoxLayout *pointResultLayout = new QHBoxLayout;

    QGroupBox *SPBox = new QGroupBox(tr("同名像点坐标"));
    QVBoxLayout *SPLayout = new QVBoxLayout;

    SPLayout->addWidget(chooseSPBtn);
    SPLayout->addWidget(SPTable);
    SPBox->setLayout(SPLayout);

    pointResultLayout->addWidget(SPBox);

    QGroupBox *resultBox = new QGroupBox(tr("空间前方交会"));
    QVBoxLayout *resultLayout = new QVBoxLayout;
    resultLayout->addWidget(calculateBtn);
    resultLayout->addWidget(resultTable);

    resultBox->setLayout(resultLayout);

    pointResultLayout->addWidget(resultBox);
    pointResultBox->setLayout(pointResultLayout);

    mainLayout->addWidget(pointResultBox);

    setLayout(mainLayout);
}
