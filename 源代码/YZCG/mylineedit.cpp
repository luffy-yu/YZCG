#include "mylineedit.h"

MyLineEdit::MyLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

MyLineEdit::MyLineEdit(QWidget *parent, QString objName, bool enabled)
{
    setParent(parent);
    setObjectName(objName);
    setEnabled(enabled);
    setValidator(new QDoubleValidator(parent));
}

void MyLineEdit::chooseDataSlot()
{
    emit requestData(this->objectName(),1,1);
}

//void MyLineEdit::inputSlot()
//{
//    clearFocus();
//    bool ok;
//    double number = QInputDialog::getDouble(this,tr("手动输入"),tr("请输入:"),0,-2147483647,2147483647,1,&ok);
//    if(ok)
//        this->setText(QString::number(number));
//}

void MyLineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
}

void MyLineEdit::mouseDoubleClickEvent(QMouseEvent */*event*/)
{
    QMenu *menu = new QMenu;
    QAction *chooseDataAction = new QAction(tr("选择数据"),menu);
    //QAction *inputActioin = new QAction(tr("手动输入"),menu);
    menu->addAction(chooseDataAction);
    //menu->addAction(inputActioin);

    connect(chooseDataAction,SIGNAL(triggered()),this,SLOT(chooseDataSlot()));
    //connect(inputActioin,SIGNAL(triggered()),this,SLOT(inputSlot()));

    menu->exec(QCursor::pos());

    delete menu;
    clearFocus();
}
