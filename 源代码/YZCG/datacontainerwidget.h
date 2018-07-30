#ifndef DATACONTAINERWIDGET_H
#define DATACONTAINERWIDGET_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 可用数据界面
  */

#include "Headers.h"
#include "tableeditwidget.h"

class DataContainerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataContainerWidget(QWidget *parent = 0);
    /**
     * @brief getModel 返回model共选择数据窗口界面使用
     * @return
     */
    QStandardItemModel *getModel();
public slots:
    /**
     * @brief addDataSlot 添加数据
     * @param list 字符串
     * @param row 行数
     * @param col 列数
     * @param filename 文件路径
     * @param description 描述
     */
    void addDataSlot(const QStringList &list,const int &row,const int &col,const QString &filename,const QString &description);
    /**
     * @brief contextMenuEvent 右键菜单
     * @param event
     */
    void contextMenuEvent(QContextMenuEvent *event);
    /**
     * @brief editSlot 编辑
     */
    void editSlot();
    /**
     * @brief removeSlot 移除
     */
    void removeSlot();
    /**
     * @brief removeAllSlot 移除所有
     */
    void removeAllSlot();
    /**
     * @brief previewSlot 预览
     */
    void previewSlot();
    /**
     * @brief saveSlot 保存
     */
    void saveSlot();
    /**
     * @brief saveAllSlot 保存所有
     */
    void saveAllSlot();
    /**
     * @brief openSlot 打开
     */
    void openSlot();
    /**
     * @brief addDescriptionSlot 添加描述
     */
    void addDescriptionSlot();
    /**
     * @brief updateRowSlot 更新编辑的数据
     * @param list 字符串
     * @param row 行数
     * @param col 列数
     * @param filename 文件路径
     */
    void updateRowSlot(const QStringList &list,const int &row,const int &col,const QString &filename);
    /**
     * @brief openDataSlot 打开数据，即ycd文件
     * @param filepath 文件路径
     */
    void openDataSlot(const QString &filepath);
    /**
     * @brief showSize 显示大小
     */
    void showSize();
    /**
     * @brief showPath 显示路径
     */
    void showPath();
    /**
     * @brief showDescription 显示描述
     */
    void showDescription();
protected:
    /**
     * @brief createPreview 创建预览
     * @param list 字符串
     * @param row 行数
     * @param col 列数
     * @param filename 文件路径
     */
    void createPreview(const QStringList &list,const int &row,const int &col,const QString &filename);
    /**
     * @brief openYcdFile 打开ycd文件
     * @param filename 文件名
     */
    void openYcdFile(const QString &filename);
private:
    QTreeView *treeView;
    QHeaderView *headView;
    QStandardItemModel *treeModel;
    QStandardItem *rootItem;
    bool sizeVisible;
    bool pathVisible;
    bool descriptionVisible;
};

#endif // DATACONTAINERWIDGET_H
