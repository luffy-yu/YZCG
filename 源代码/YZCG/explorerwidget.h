#ifndef EXPLORERWIDGET_H
#define EXPLORERWIDGET_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 文件系统界面
  */

#include "Headers.h"

class ExplorerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExplorerWidget(QWidget *parent = 0);
    
signals:
    /**
     * @brief requestViewFile 请求浏览文件
     * @param filePath 文件路径
     */
    void requestViewFile(const QString &filePath);
    /**
     * @brief requestEditFile 请求编辑文件
     * @param filePath 文件路径
     */
    void requestEditFile(const QString &filePath);
    /**
     * @brief requestOpenProject 打开工程文件
     * @param filePath 文件路径
     */
    void requestOpenProject(const QString &filePath);
    /**
     * @brief requestOpenData 打开数据
     * @param filepath 文件路径
     */
    void requestOpenData(const QString &filepath);
public slots:
    /**
     * @brief contextMenuEvent 右键菜单
     * @param event
     */
    void contextMenuEvent(QContextMenuEvent *event);
    /**
     * @brief expandSlot 展开
     */
    void expandSlot();
    /**
     * @brief collapseSlot 收起
     */
    void collapseSlot();
    /**
     * @brief viewSlot 浏览
     */
    void viewSlot();
    /**
     * @brief editSlot 编辑
     */
    void editSlot();
    /**
     * @brief openProjectSlot 打开项目
     */
    void openProjectSlot();
    /**
     * @brief openDataSlot 打开数据
     */
    void openDataSlot();
protected:
    /**
     * @brief openProjectOrData 打开项目或者数据
     * @param isProject true为项目，false为数据
     * @param filename 文件名
     */
    void openProjectOrData(bool isProject,QString filename);
private:
    QFileSystemModel *model;
    QTreeView *view;
};

#endif // EXPLORERWIDGET_H
