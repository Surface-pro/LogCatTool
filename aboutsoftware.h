#ifndef ABOUTSOFTWARE_H
#define ABOUTSOFTWARE_H

#include <QDialog>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QList>
#include <QDebug>
#include <QMessageBox>
#include "softwareinfo.h"
#include "security.h"

namespace Ui {
class aboutSoftware;
}

class aboutSoftware : public QDialog
{
    Q_OBJECT
    
public:
    explicit aboutSoftware(QWidget *parent = 0);
    ~aboutSoftware();
    
private slots:
    void expandSoftwareFuncSlot();      //展开或隐藏软件功能简介信息槽函数
    void expandUpdateInfoSlot();        //展开或隐藏更新日志详细信息槽函数
    
private:
    Ui::aboutSoftware *ui;
    
    QList<QPushButton*> *updateBtnMore; //更新日志索引后的所有按钮集合
    QList<QWidget*> *updateWidgets;     //更新日志详细信息控件集合
    
    Security *mSecurity;                //软件安全管理器
};

#endif // ABOUTSOFTWARE_H
