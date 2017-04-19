#include "connectadb.h"
#include "ui_connectadb.h"

ConnectADB::ConnectADB(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectADB)
{
    ui->setupUi(this);

    isClickTrue = false;
    selectedDevice = "";
    mRadios = new QList<QRadioButton*>;

    QObject::connect(ui->btnOK,SIGNAL(clicked()),this,SLOT(btnOKClickedSlot()));
    QObject::connect(ui->btnCancel,SIGNAL(clicked()),this,SLOT(close()));
}

ConnectADB::~ConnectADB()
{
    delete ui;
}


/*
 * 函数名称：    transParams()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    向该对话框传入参数
 * 输入参数：    devices:所有ADB设备表
 * 输出参数：    无
 * 返回值：      无
*/
void ConnectADB::transParams(QStringList devices)
{
    mDevices = devices;
    
    //根据设备表手动创建选择控件
    for (int i = 0; i < mDevices.size(); ++i) {
        QRadioButton *btnRadio = new QRadioButton;
        btnRadio->setText(mDevices.at(i));
        mRadios->append(btnRadio);
        ui->container->addWidget(btnRadio);
    }

}


/*
 * 函数名称：    getIsClickTrue()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    获取退出时是否点击了确认
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      点击了确认返回true，否则返回false
*/
bool ConnectADB::getIsClickTrue()
{
    return isClickTrue;
}


/*
 * 函数名称：    getDeviceName()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    获取选中的ADB设备名
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回选中的ADB设备名
*/
QString ConnectADB::getDeviceName()
{
    return selectedDevice;
}


/*
 * 函数名称：    btnOKClickedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    确认按钮槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void ConnectADB::btnOKClickedSlot()
{
    //查找是哪个设备被选中了
    QString name = "";
    for (int i = 0; i < mRadios->size(); ++i) {
        QRadioButton *btnRadio = mRadios->at(i);
        if (btnRadio->isChecked()) {
            name = btnRadio->text();
        }
    }

    if (name != "") {
        selectedDevice = name;
        isClickTrue = true;
        this->close();
    } else {
        QMessageBox::information(this,"警告","请选择一个设备！");
    }
}
