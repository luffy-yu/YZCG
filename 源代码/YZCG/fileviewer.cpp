#include "fileviewer.h"

FileViewer::FileViewer(QWidget *parent, QString filename) :
    QWidget(parent)
{
    setWindowModality(Qt::ApplicationModal);

    browser = new QTextBrowser(this);
    browser->setMinimumSize(600,400);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(browser);

    setLayout(layout);

    setWindowTitle(tr("%1 - %2").arg(filename).arg("YZCG"));

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
#ifdef QT_DEBUG
        qDebug()<<"fileviewer open file failed";
#endif
        return;
    }
    browser->setText(tr(file.readAll()));
    file.close();
}
