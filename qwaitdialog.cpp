#include "qwaitdialog.h"



/*
 * 函数名称：    QWaitDialog(QWidget *parent,QString title)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.14
 * 函数功能：    构造函数
 * 输入参数：    parent:父控件      title:提示标题
 * 输出参数：    无
 * 返回值：      无
*/
QWaitDialog::QWaitDialog(QWidget *parent,QString title) :
    QDialog(parent)
{
    setWindowOpacity(0.8);      //设置窗口透明度
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);   //设置窗口无边框
    setStyleSheet("background-color:transparent;");         //设置样式表
    
    /*  实现原理：最外层垂直布局中仅包含一个QFrame背景控件，然后QFrame中包含一个水平布局，
     *          水平布局中从左至右依次是gif动画、标题，动画为转圈的等待，标题为等待的提示信息；
     *  -------------------------------------
     *  | --------  ----------------------- |
     *  | | gif  |  |                     | |
     *  | | 动画  |  |       标题           | |
     *  | |      |  |                     | |
     *  | --------  ----------------------- |
     *  -------------------------------------
    */
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);
    
    background = new QFrame;
    background->setStyleSheet("background-color:rgb(255,255,255);border-radius:10px;");//30,30,30 border-radius:10px;
    
    QHBoxLayout *hLayout = new QHBoxLayout(background);
    hLayout->setSpacing(10);
    
    label = new QLabel;
    label->setStyleSheet("background-color:transparent;");
    label->setFixedWidth(40);
    label->setFixedHeight(40);
    label->setScaledContents(true);
    
    movie = new QMovie(":/new/image/wait.gif");//工程中资源文件中gif动画的路径
    label->setMovie(movie);
    
    tip_label = new QLabel;
    tip_label->setText(title);
    tip_label->setStyleSheet("color:black;background-color:transparent;font: 14pt \"Courier New\";");
    
    hLayout->addWidget(label);
    hLayout->addWidget(tip_label);
    vLayout->addWidget(background);
    
    movie->start();
}


/*
 * 函数名称：    ~QWaitDialog()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.14
 * 函数功能：    析构函数，释放相应的资源
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
QWaitDialog::~QWaitDialog()
{
    delete label;
    delete movie;
    delete tip_label;
    delete background;
}
