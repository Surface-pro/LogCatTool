#include "aboutsoftware.h"
#include "ui_aboutsoftware.h"

aboutSoftware::aboutSoftware(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutSoftware)
{
    ui->setupUi(this);
    mSecurity = Security::getInstance();
    
    //初始化相关变量
    updateWidgets = new QList<QWidget*>;
    updateBtnMore = new QList<QPushButton*>;
    QFont font = QFont("楷体",14,QFont::Normal);
    QVBoxLayout *vLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    QLabel *contentLabel = new QLabel;
    
    
    //布局当前版本信息栏
    QString text = SoftwareInfo::getCurVersionInfo();
    contentLabel->setText(text);
    contentLabel->setFont(font);
    contentLabel->setStyleSheet("background-color:rgb(200,200,200);");
    vLayout->addWidget(contentLabel);
    ui->widgetCurVersion->setLayout(vLayout);
    
    
    //布局软件功能简介栏
    vLayout = new QVBoxLayout;
    contentLabel = new QLabel;
    contentLabel->setText(SoftwareInfo::getSoftwareFuncInfo());
    contentLabel->setFont(font);
    contentLabel->setStyleSheet("background-color:rgb(200,200,200);");
    vLayout->addWidget(contentLabel);
    ui->widgetSoftwareFunc->setLayout(vLayout);
    ui->widgetSoftwareFunc->hide();
    QObject::connect(ui->btnFuncMore,SIGNAL(clicked()),this,SLOT(expandSoftwareFuncSlot()));
    
    
    //布局更新日志信息栏
    vLayout = new QVBoxLayout;
    vLayout->setSpacing(0);
    QStringList updateInfo = SoftwareInfo::getUpdateInfo();
    QStringList updateInfoDetails = SoftwareInfo::getUpdateInfoDetails();
    int size = updateInfo.size();
    updateBtnMore->clear();
    updateWidgets->clear();
    for (int i = 0; i < size; ++i) {
        //布局索引行
        hLayout = new QHBoxLayout;
        QLabel *label = new QLabel(updateInfo.at(i));
        label->setFont(font);
        QPushButton *btnMore = new QPushButton("查看细节");
        btnMore->setFont(font);
        QObject::connect(btnMore,SIGNAL(clicked()),this,SLOT(expandUpdateInfoSlot()));
        
        hLayout->addWidget(label);
        hLayout->addStretch();
        hLayout->addWidget(btnMore);
        
        //布局详细信息内容
        QWidget *widget = new QWidget;
        QVBoxLayout *vTempLayout = new QVBoxLayout;
        contentLabel = new QLabel;
        contentLabel->setText(updateInfoDetails.at(i));
        contentLabel->setFont(font);
        contentLabel->setStyleSheet("background-color:rgb(200,200,200);");
        vTempLayout->addWidget(contentLabel);
        widget->setLayout(vTempLayout);
        widget->hide();
        
        vLayout->addLayout(hLayout);
        vLayout->addWidget(widget);
        
        //将生成的控件添加到对应的集合中
        updateBtnMore->append(btnMore);
        updateWidgets->append(widget);
    }
    ui->widgetUpdateInfo->setLayout(vLayout);
    
    adjustSize();   //自动调整窗口大小
}

aboutSoftware::~aboutSoftware()
{
    delete ui;
}


/*
 * 函数名称：    expandSoftwareFuncSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.16
 * 函数功能：    展开或隐藏软件功能简介信息槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void aboutSoftware::expandSoftwareFuncSlot()
{
    if (ui->widgetSoftwareFunc->isHidden()) {
        ui->widgetSoftwareFunc->show();
    } else {
        ui->widgetSoftwareFunc->hide();
    }
}


/*
 * 函数名称：    expandUpdateInfoSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.16
 * 函数功能：    展开或隐藏更新日志详细信息槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void aboutSoftware::expandUpdateInfoSlot()
{
    /*  
     * 判断软件是否注册，只有注册了才能查看软件更新日志的详细信息，
     * 因为其中包含了该软件部分重要功能的实现原理，此功能的添加主要
     * 是为了当该软件共享到网络上时，避免其他人查看细节部分；
    */
    if (!mSecurity->isRegisted()) {
        QMessageBox::information(this,"警告","软件未注册，无法查看此信息！");
        return;
    }
    
    //先寻找点击的是哪个按钮
    int btnIndex = -1;
    QWidget *widget = this->focusWidget();
    int num = updateBtnMore->size();
    for (int i = 0; i < num; ++i) {
        if (widget == updateBtnMore->at(i)) {
            btnIndex = i;
            break;
        }
    }
    if (btnIndex == -1) {
        return;
    }
    
    //然后将其他更新日志详细信息隐藏
    for (int i = 0; i < num; ++i) {
        if (i == btnIndex) {
            continue;
        }
        
        widget = updateWidgets->at(i);
        if (!widget->isHidden()) {
            widget->hide();
        }
    }
    
    //最后隐藏或显示所点击的对应的更新日志详细信息
    widget = updateWidgets->at(btnIndex);
    if (widget->isHidden()) {
        widget->show();
    } else {
        widget->hide();
    }
}




