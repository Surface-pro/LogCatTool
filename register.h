#ifndef REGISTER_H
#define REGISTER_H

#include <QDialog>
#include <QMessageBox>
#include <QDateTime>
#include "security.h"


namespace Ui {
class Register;
}

class Register : public QDialog
{
    Q_OBJECT
    
public:
    explicit Register(QWidget *parent = 0);
    ~Register();
    
public:
    bool getIsClickTrue();      //退出时是否点击了确认
    QString getRegisterNum();   //获取注册码
    
private:
    QString getCpuId();                 //获取CPU的ID
    QString getDiskId();                //获取硬盘ID
    QString getMachineNum();            //获取机器码
    
private slots:
    void btnOKClickedSlot();        //确认按钮槽函数
    void getMachineNumSlot();       //获取机器码槽函数
    
private:
    Ui::Register *ui;
    
    bool isClickTrue;   //是否点击了确认键
    QString registerNum;
};

#endif // REGISTER_H
