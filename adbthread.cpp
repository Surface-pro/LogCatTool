#include "adbthread.h"

ADBThread::ADBThread(QObject *parent) :
    QThread(parent)
{
    //初始化结果存储区
    result.clear();
    result.append("");
    result.append("");
}

ADBThread::~ADBThread()
{
    requestInterruption();
    quit();
    wait();
}


/*
 * 函数名称：    transParams(QString adbPath, QString cmd)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.14
 * 函数功能：    向线程传入参数，必须在线程启动前传入
 * 输入参数：    adbPath:ADB路径       cmd:要执行的命令
 * 输出参数：    无
 * 返回值：      无
*/
void ADBThread::transParams(QString adbPath, QString cmd)
{
    this->adbPath = adbPath;
    this->cmd = cmd;
}


/*
 * 函数名称：    getResult()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.14
 * 函数功能：    获取线程执行的结果，包含有2个元素，其中[0]表示执行结果，[1]表示执行发生错误的信息
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回执行的结果
*/
QStringList ADBThread::getResult()
{
    return result;
}


//void ADBThread::run()
//{
//    qDebug()<<"ADBThread Run : "<<QThread::currentThreadId()<<"......";
//    int value = 0;
//    while (!isInterruptionRequested() && (value < 100)) {
//        msleep(50);
//        value++;
        
//        emit resultReady(value);
//    }
//    qDebug()<<"ADBThread Done !";
//    emit done();
//}


/*
 * 函数名称：    run()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.14
 * 函数功能：    重写线程的run()函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void ADBThread::run()
{
    //qDebug()<<"ADBThread Run : "<<QThread::currentThreadId()<<"......";
    
    result = execWindowsCmd(adbPath,cmd);   //执行ADB命令
    
    //qDebug()<<"result[0]=" + result.at(0) + "\nresult[1]=" + result.at(1);
    //qDebug()<<"ADBThread Done !";
    
    emit done();    //发送线程执行完毕信号
}


/*
 * 函数名称：    execWindowsCmd(QString adbPath,QString cmd)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.14
 * 函数功能：    执行CMD命令
 * 输入参数：    adbPath:ADB工具路径     cmd:要执行的命令
 * 输出参数：    无
 * 返回值：      返回字符串列表，包含两个元素，其中[0]:表示执行结果返回； [1]:表示执行错误返回
*/
QStringList ADBThread::execWindowsCmd(QString adbPath,QString cmd)
{
    QStringList result;
    bool isDispWaitDialog = false;

    if (adbPath == "") {
        result.append("");
        result.append("未设置ADB路径");
        return result;
    }

    QProcess process(0);
    process.setWorkingDirectory(adbPath);
    process.start("cmd",QStringList()<<"/c"<<cmd);
    //如果该命令在300ms以内还未执行完，则将该标志位置位，表示需要显示等待对话框，以提示用户
    isDispWaitDialog = !process.waitForFinished(300);
    
    if (isDispWaitDialog) {
        emit showDialog();  //发送显示等待对话框信号
    }
    process.waitForStarted();
    process.waitForFinished(5000);
    if (isDispWaitDialog) {
        emit closeDialog(); //发送关闭等待对话框信号
    }

    QString out = QString::fromUtf8(process.readAllStandardOutput());
    QString err = QString::fromUtf8(process.readAllStandardError());

    process.close();
    process.kill();

    out = out.trimmed();
    err = err.trimmed();

    result.append(out);
    result.append(err);

    return result;
}


