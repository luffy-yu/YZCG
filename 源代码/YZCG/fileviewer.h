#ifndef FILEVIEWER_H
#define FILEVIEWER_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 文本浏览器界面
  */

#include "Headers.h"

class FileViewer : public QWidget
{
    Q_OBJECT
public:
    explicit FileViewer(QWidget *parent = 0,QString filename = QString());
private:
    QTextBrowser *browser;
};

#endif // FILEVIEWER_H
