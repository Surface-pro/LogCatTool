#ifndef CONNECTADB_H
#define CONNECTADB_H

#include <QDialog>
#include <QStringList>
#include <QRadioButton>
#include <QMessageBox>
#include <QList>

namespace Ui {
class ConnectADB;
}

class ConnectADB : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectADB(QWidget *parent = 0);
    ~ConnectADB();

public:
    void transParams(QStringList devices);   //向该对话框传入参数
    bool getIsClickTrue();      //退出时是否点击了确认
    QString getDeviceName();    //获取选中的ADB设备名

private slots:
    void btnOKClickedSlot();        //确认按钮槽函数

private:
    Ui::ConnectADB *ui;

    bool isClickTrue;               //是否点击了确认键
    QString selectedDevice;         //选中的ADB设备名

    QStringList mDevices;           //所有ADB设备表
    QList<QRadioButton*> *mRadios;  //选择控件表
};

#endif // CONNECTADB_H
