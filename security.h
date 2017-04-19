#ifndef SECURITY_H
#define SECURITY_H

#include <QObject>
#include <QDebug>
#include <QMutex>
#include <QMessageBox>
#include "cpuid.h"
#include <windows.h>
#include <math.h>


class Security : public QObject
{
    Q_OBJECT
private:
    explicit Security(QObject *parent = 0); //构造函数私有化，防止外部直接创建该对象
    
signals:
    
public slots:
    
public:
    static Security* getInstance();     //获取该实例
    
public:
    bool isRegisted();                  //获取软件是否注册
    bool checkRegisterNum(QString num); //检查软件注册码是否符合规范
    void setRegisted(bool isRegister);  //设置软件注册状态
    
private:
    QString getCpuId();                 //获取CPU的ID
    QString getDiskId();                //获取硬盘ID
    QString getMachineNum();            //获取机器码
    QString generateRegisterNum(QString machineNum);//由机器码生成注册码
    
private:
    static Security *mInstance;     //实例
    static QMutex *mMutex;          //线程锁
    bool isRegisterSuccess;         //标志软件是否注册
    
};

#endif // SECURITY_H
