#include "register.h"
#include "ui_register.h"

Register::Register(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    
    isClickTrue = false;
    registerNum = "";
    
    QObject::connect(ui->btnMachine,SIGNAL(clicked()),this,SLOT(getMachineNumSlot()));
    QObject::connect(ui->btnOK,SIGNAL(clicked()),this,SLOT(btnOKClickedSlot()));
    QObject::connect(ui->btnCancel,SIGNAL(clicked()),this,SLOT(close()));
}

Register::~Register()
{
    delete ui;
}

/*
 * 函数名称：    getIsClickTrue()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    获取是否点击了确定
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      点击了确定返回true，否则返回false
*/
bool Register::getIsClickTrue()
{
    return isClickTrue;
}


/*
 * 函数名称：    getRegisterNum()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    获取注册码
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回注册码
*/
QString Register::getRegisterNum()
{
    return registerNum;
}


/*
 * 函数名称：    btnOKClickedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    确认按钮槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void Register::btnOKClickedSlot()
{
    QString num = ui->etRegister->text();
    Security *security = Security::getInstance();
    if (security->checkRegisterNum(num)) {
        QMessageBox::information(this,"提示","注册成功！");
        registerNum = num;
        isClickTrue = true;
        this->close();
    } else {
        QMessageBox::information(this,"提示","注册码无效，注册失败！");
        registerNum = "";
    }
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
QString Register::getCpuId()
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
QString Register::getDiskId()
{
    DWORD VolumeSerialNumber;
    GetVolumeInformationA("C:\\",NULL,NULL,
                         &VolumeSerialNumber,NULL,NULL,
                         NULL,NULL);
    QString diskId = QString::number(VolumeSerialNumber,16).toUpper();
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
QString Register::getMachineNum()
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
 * 函数名称：    getMachineNumSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.20
 * 函数功能：    获取机器码槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void Register::getMachineNumSlot()
{
    QString num = getMachineNum();
    ui->etMachine->setText((num == "") ? "获取失败" : num);
}



