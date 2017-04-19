#ifndef ADBTHREAD_H
#define ADBTHREAD_H

#include <QThread>
#include <QDebug>
#include <QProcess>
#include <QMessageBox>

class ADBThread : public QThread
{
    Q_OBJECT
public:
    explicit ADBThread(QObject *parent = 0);
    ~ADBThread();
    
signals:
    void showDialog();      //显示等待对话框信号
    void closeDialog();     //关闭等待对话框信号
    void done();            //线程执行完毕信号
    
public slots:
    
public:
    //向线程传入参数，必须在线程启动前传入
    void transParams(QString adbPath,QString cmd);
    
    //获取线程执行的结果，包含有2个元素，其中[0]表示执行结果，[1]表示执行发生错误的信息
    QStringList getResult();
    
protected:
    void run();             //重写线程的run()函数
    
private:
    QStringList execWindowsCmd(QString adbPath,QString cmd);    //执行CMD命令
    
private:
    QString adbPath;        //ADB路径
    QString cmd;            //命令
    QStringList result;     //执行结果
    
};

#endif // ADBTHREAD_H
