#ifndef TABLEEDITWIDGET_H
#define TABLEEDITWIDGET_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 表格数据编辑界面
  */

#include "Headers.h"
#include "utilities.h"

class TableEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TableEditWidget();
    /**
     * @brief TableEditWidget
     * @param fileName 文件名
     */
    explicit TableEditWidget(QString fileName);
    /**
     * @brief TableEditWidget
     * @param list 字符串
     * @param rows 行数
     * @param cols 列数
     * @param filename 文件名
     */
    explicit TableEditWidget(QStringList list,int rows,int cols,QString filename);
signals:
    /**
     * @brief addData2DataContainer 添加数据到 可用数据 窗口
     * @param list 字符串（数据）
     * @param row 行数
     * @param col 列数
     * @param filename 文件名
     * @param description 描述
     */
    void addData2DataContainer(const QStringList &list,const int &row,const int &col,const QString &filename,const QString &description);
public slots:
    /**
     * @brief createSlot 创建
     */
    void createSlot();
    /**
     * @brief openSlot 打开
     */
    void openSlot();
    /**
     * @brief importSlot 导入
     */
    void importSlot();
    /**
     * @brief confirmSlot 确认
     */
    void confirmSlot();
    /**
     * @brief cancelSlot 取消
     */
    void cancelSlot();
    /**
     * @brief saveSlot 保存
     */
    void saveSlot();
    /**
     * @brief saveAsSlot 另存
     */
    void saveAsSlot();
    /**
     * @brief contextMenuEvent 右键菜单
     * @param event
     */
    void contextMenuEvent(QContextMenuEvent *event);
    /**
     * @brief addRowSlot 添加一行
     */
    void addRowSlot();
    /**
     * @brief addColSlot 添加一列
     */
    void addColSlot();
    /**
     * @brief delRowSlot 删除一行
     */
    void delRowSlot();
    /**
     * @brief delColSlot 删除一列
     */
    void delColSlot();
    /**
     * @brief delSelectSlot 删除所选
     */
    void delSelectSlot();
    /**
     * @brief unSelectSlot 取消选择
     */
    void unSelectSlot();
    /**
     * @brief selectAllSlot 选择所有
     */
    void selectAllSlot();
    /**
     * @brief setRowColSlot 设置行列数
     */
    void setRowColSlot();
    /**
     * @brief resetSlot 重置
     */
    void resetSlot();
protected:
    /**
     * @brief initUI 初始化界面
     */
    void initUI();
    /**
     * @brief openFile 打开文件
     * @param fileName 文件名
     * @return
     */
    bool openFile(const QString &fileName);
    /**
     * @brief saveFile 保存文件
     * @param fileName 文件名
     */
    void saveFile(const QString &fileName);
    /**
     * @brief checkAllIsNumber 检查是不是都是数字
     * @return
     */
    bool checkAllIsNumber();
    /**
     * @brief fillTable 用文件填充表格
     * @param fileName 文件名
     */
    void fillTable(const QString &fileName);
    /**
     * @brief fillTable 用字符串填充表格
     * @param list 字符串
     * @param rows 行数
     * @param cols 列数
     */
    void fillTable(const QStringList &list,int rows,int cols);
private:
    QTableWidget *table;
    QPushButton *createBtn;//新建
    QPushButton *openBtn;//打开
    QPushButton *importBtn;//导入
    QPushButton *confirmBtn;//确定
    QPushButton *cancelBtn;//取消
    QPushButton *saveBtn;//保存
    QPushButton *saveAsBtn;//另存

    int rowCount;
    int colCount;
    int fileLine;
    QString fileFullPath;
    QStringList contentList;
    bool allIsNumber;
    bool editable;
};

#endif // TABLEEDITWIDGET_H
