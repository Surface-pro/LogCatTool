#include "security.h"


/*
 *  该类为单例模式设计方式，保证整个软件只存在一个实例对象
 * 
 * 该文件所实现的功能称为“安全管理器”；
 * 该文件实现的目的：
 *      主要是为了当该软件共享到网络上时，可以隐藏不希望他人看到的细节；
 *      
*/



//初始化静态变量
Security* Security::mInstance = new Security();
QMutex* Security::mMutex = new QMutex;


/*
 * 函数名称：    getInstance()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    获取该实例（单例模式）
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回实例
*/
Security* Security::getInstance()
{
    //此处双重判断并加锁，可有效的避免多线程中同时访问造成创建多个实例
    if (mInstance == NULL) {
        mMutex->lock();     //加锁
        if (mInstance == NULL) {
            mInstance = new Security;
        }
        mMutex->unlock();   //解锁
    }
    
    return mInstance;
}


/*
 * 函数名称：    Security(QObject *parent)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    构造函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回实例
*/
Security::Security(QObject *parent) :
    QObject(parent)
{
    isRegisterSuccess = false;
    
    
}


/*
 * 函数名称：    isRegisted()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    获取软件是否注册
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      注册了返回true，否则返回false
*/
bool Security::isRegisted()
{
    return isRegisterSuccess;
}


/*
 * 函数名称：    setRegisted(bool isRegister)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    设置软件注册状态
 * 输入参数：    isRegister:是否注册了
 * 输出参数：    无
 * 返回值：      无
*/
void Security::setRegisted(bool isRegister)
{
    isRegisterSuccess = isRegister;
}


/*
 * 函数名称：    checkRegisterNum(QString num)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    检查软件注册码是否符合规范
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      符合返回true，否则返回false
*/
bool Security::checkRegisterNum(QString num)
{
    if(num.length() != 19) {
        return false;
    }
    
    QStringList list = num.split("-");
    int size = list.size();
    if (size != 4) {
        return false;
    }
    QString tempStr;
    QChar tempChar;
    for (int i = 0; i < size; ++i) {
        tempStr = list.at(i);
        if (tempStr.length() != 4) {
            return false;
        } else {
            for (int j = 0; j < 4; ++j) {
                tempChar = tempStr.at(i);
                if ((tempChar >= '0') && (tempChar <= '9')) {
                    break;
                } else if ((tempChar >= 'A') && (tempChar <= 'Z')) {
                    break;
                }else {
                    return false;
                }
            }
        }
    }
    
    QString realNum = generateRegisterNum(getMachineNum());
    //qDebug()<<"inputNum=" + num;
    //qDebug()<<"realNum=" + realNum;
    //QMessageBox::information(0,"警告","注册码为：" + realNum);
    if ((num != realNum) || (realNum == "")) {
        return false;
    }
    
    return true;
}


/*
 * 函数名称：    getCpuId()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    获取CPU的ID
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回CPU的ID
*/
QString Security::getCpuId()
{
    QString cpuID = "";
    unsigned int cpuInfo[4];
    QString temp;
    
    __cpuid(1,cpuInfo[0],cpuInfo[1],cpuInfo[2],cpuInfo[3]);
    for (int i = 0; i < 4; ++i) {
        temp = QString::number(cpuInfo[i],16).toUpper();
        while (temp.length() != 8) {
            temp = "0" + temp;
        }
        cpuID += temp;
    }
    //qDebug()<<"【cpuID】=" + cpuID;
    return cpuID;
}


/*
 * 函数名称：    getDiskId()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.20
 * 函数功能：    获取硬盘ID
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回硬盘的ID
*/
QString Security::getDiskId()
{
    DWORD VolumeSerialNumber;
    GetVolumeInformationA("C:\\",NULL,NULL,
                         &VolumeSerialNumber,NULL,NULL,
                         NULL,NULL);
    QString diskId = QString::number(VolumeSerialNumber,16).toUpper();
    while (diskId.length() != 8) {
        diskId = "0" + diskId;
    }
    //qDebug()<<"【diskID】=" + diskId;    
    return diskId;
}


/*
 * 函数名称：    getMachineNum()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.20
 * 函数功能：    获取机器码
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回机器码
*/
QString Security::getMachineNum()
{
    QString machineNum = "";
    QString cpuId = getCpuId();
    QString diskId = getDiskId();
    
    cpuId = 
    machineNum = cpuId.mid(0,8) + cpuId.mid(16,8) + diskId + cpuId.mid(24,8);
    //qDebug()<<"machineNum=" + machineNum;
    
    return machineNum;
}


/*
 * 函数名称：    generateRegisterNum(QString machineNum)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    由机器码生成注册码，实现原理：不可逆加密算法
 * 输入参数：    machineNum:机器码
 * 输出参数：    无
 * 返回值：      返回注册码
*/
QString Security::generateRegisterNum(QString machineNum)
{
    if (machineNum.length() != 32) {
        return "";
    }
    
    QString registerNum = "";
    for (int i = 0; i < 16; ++i) {
        ushort tempNum1 = machineNum.at(i).unicode();
        ushort tempNum2 =  machineNum.at(i + 5).unicode();
        ushort tempNum3 = + machineNum.at(i + 16).unicode();
        
        int tempNum = (int)(pow(tempNum1 - tempNum2,2) / 40 + abs(tempNum2 - tempNum3) 
                            + sqrt(abs(tempNum1 - tempNum3)));
        
        QString tempStr = QString::number(tempNum,16).toUpper();
        registerNum += tempStr.at(tempStr.length() - 1);
        if ((i != 15) && (((i + 1) % 4) == 0)) {
            registerNum += "-";
        }
    }
    
    return registerNum;
}

