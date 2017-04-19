#include "setting.h"
#include "ui_setting.h"

Setting::Setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);

    isClickTrue = false;
    ADBPath = "";

    //信号与槽连接
    QObject::connect(ui->cbFontSize,SIGNAL(currentIndexChanged(int)),this,SLOT(fontSizeChangedSlot()));
    QObject::connect(ui->cbFontType,SIGNAL(currentIndexChanged(int)),this,SLOT(fontTypeChangedSlot()));
    QObject::connect(ui->btnReset,SIGNAL(clicked()),this,SLOT(resetSlot()));
    QObject::connect(ui->btnOK,SIGNAL(clicked()),this,SLOT(btnOKClickedSlot()));
    QObject::connect(ui->btnCancel,SIGNAL(clicked()),this,SLOT(close()));
    QObject::connect(ui->cbCodeType,SIGNAL(currentIndexChanged(int)),this,SLOT(codeTypeChangedSlot()));
    QObject::connect(ui->btnSelectPath,SIGNAL(clicked()),this,SLOT(selectPathSlot()));
}

Setting::~Setting()
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
bool Setting::getIsClickTrue()
{
    return isClickTrue;
}


/*
 * 函数名称：    getFontType()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    获取字体类型
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回字体类型
*/
QString Setting::getFontType()
{
    return fontType;
}


/*
 * 函数名称：    getFontSize()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    获取字体大小
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回字体大小
*/
int Setting::getFontSize()
{
    return fontSize;
}


/*
 * 函数名称：    getCodeType()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    获取编码类型
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回编码类型
*/
QString Setting::getCodeType()
{
    return codeType;
}


/*
 * 函数名称：    getADBPath()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    获取ADB工具的路径
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回ADB工具的路径
*/
QString Setting::getADBPath()
{
    return ADBPath;
}


/*
 * 函数名称：    transParams()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    向对话框传入参数
 * 输入参数：    fontType:字体类型   fontSize:字体大小   codeType:编码类型
 * 输出参数：    无
 * 返回值：      无
*/
void Setting::transParams(QString fontType, int fontSize, QString codeType,QString ADBPath)
{
    this->fontType = fontType;
    this->fontSize = fontSize;
    this->codeType = codeType;
    this->ADBPath = ADBPath;

    int index = -1;
    index = ui->cbFontType->findText(fontType);
    index = (index == -1) ? 0 : index;
    ui->cbFontType->setCurrentIndex(index);
    
    index = ui->cbFontSize->findText(QString::number(fontSize));
    index = (index == -1) ? 0 : index;
    ui->cbFontSize->setCurrentIndex(index);
    
    ui->labelPreview->setFont(QFont(fontType,fontSize,QFont::Normal));
    
    index = ui->cbCodeType->findText(codeType);
    index = (index == -1) ? 82 : index;
    ui->cbCodeType->setCurrentIndex(index);
    
    ui->etADBPath->setText(ADBPath);
}


/*
 * 函数名称：    btnOKClickedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    确认按钮点击槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void Setting::btnOKClickedSlot()
{
    isClickTrue = true;
    this->close();
}


/*
 * 函数名称：    fontTypeChangedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    字体类型选择框内容变化槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void Setting::fontTypeChangedSlot()
{
    updatePreviewFont();
}


/*
 * 函数名称：    fontSizeChangedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    字体大小选择框内容变化槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void Setting::fontSizeChangedSlot()
{
    updatePreviewFont();
}


/*
 * 函数名称：    resetSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    重置按钮点击槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void Setting::resetSlot()
{
    if (ui->tabWidget->currentWidget() == ui->font) {
        ui->cbFontType->setCurrentIndex(82);
        ui->cbFontSize->setCurrentIndex(2);
    } else if (ui->tabWidget->currentWidget() == ui->codeType) {
        ui->cbCodeType->setCurrentIndex(0);
    }
}


/*
 * 函数名称：    codeTypeChangedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    编码类型选择框内容变化槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void Setting::codeTypeChangedSlot()
{
    codeType = ui->cbCodeType->currentText();
}


/*
 * 函数名称：    selectPathSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    选择ADB工具路径的槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void Setting::selectPathSlot()
{
    QString path = QFileDialog::getOpenFileName(this,"请选择ADB工具路径",
                  "C:/Users/hexiaolong/Desktop","(*.exe)");

    if (!path.isEmpty()) {
        QString name = "/" + path.split("/")[path.split("/").length() - 1];
        path.remove(name);
        ui->etADBPath->setText(path);
        ADBPath = path;
    }
}


/*
 * 函数名称：    updatePreviewFont()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    更新预览字体样式
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void Setting::updatePreviewFont()
{
    fontType = ui->cbFontType->currentText();
    fontSize = ui->cbFontSize->currentText().toInt();
    QFont font = QFont(fontType,fontSize,QFont::Normal);
    ui->labelPreview->setFont(font);
}

