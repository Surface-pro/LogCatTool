#include "mainwindow.h"
#include "ui_mainwindow.h"



#define     ON                          1       //开
#define     OFF                         0       //关
#define     DEBUG_SWITCH                ON      //打印信息输出开关

#define     DEFAULT_TAG                 "All messages"          //默认的日志Tag，即显示所有信息
#define     ADB_ERR_RESULT              "'adb' "                //ADB返回的错误信息
#define     MAX_CACHE_LINES             1000                    //日志文件最大缓存数据行数
#define     LOG_CACHE_FILE_PATH         "/cache/logCache.txt"   //日志缓存文件路径
#define     SETTING_FILE_PATH           "/config/setting.cfg"   //软件的全局配置文件

#define     WORK_MODE_OFFLINE           "  离线查看模式  "        //工作模式--离线查看模式
#define     WORK_MODE_ONLINE            "  实时终端模式  "        //工作模式--实时终端模式
#define     NO_ADB_DEVICE               "  ADB设备未连接  "       //无ADB设备

#define     MAX_SAVE_HISTORY_NUM        10                      //最大文件打开历史记录数量
#define     DEFAULT_HISTORY_ACTION      "清空历史记录"            //清空文件打开历史记录Action动作名称





/*
 * 函数名称：    MainWindow(QWidget *parent)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.1.20
 * 函数功能：    构造函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //软件UI属性配置
    ui->setupUi(this);                      //装在UI文件
    setWindowTitle("Android日志分析工具");    //设置软件标题
    setWindowState(Qt::WindowMaximized);    //设置软件启动时为最大化窗口

    
    //初始化软件运行环境
    initEnvironment();
    

    //设置信号与槽的连接
    QObject::connect(ui->menuBar,SIGNAL(triggered(QAction*)),this,SLOT(trigerMenuSlot(QAction*)));
    QObject::connect(ui->mainToolBar,SIGNAL(actionTriggered(QAction*)),this,SLOT(trigerToolSlot(QAction*)));
    
    QObject::connect(ui->cbLevel,SIGNAL(currentIndexChanged(int)),this,SLOT(logLevelChangedSlot()));
    QObject::connect(ui->etSearch,SIGNAL(textChanged(QString)),this,SLOT(searchEditChangedSlot()));
    QObject::connect(ui->swTimeFilter,SIGNAL(clicked()),this,SLOT(swTimeFilterChangedSlot()));
    QObject::connect(ui->swTime,SIGNAL(clicked()),this,SLOT(swMessageFilterChangedSlot()));
    QObject::connect(ui->swLevel,SIGNAL(clicked()),this,SLOT(swMessageFilterChangedSlot()));
    QObject::connect(ui->swPID,SIGNAL(clicked()),this,SLOT(swMessageFilterChangedSlot()));
    QObject::connect(ui->swTag,SIGNAL(clicked()),this,SLOT(swMessageFilterChangedSlot()));
    QObject::connect(ui->swHead,SIGNAL(clicked()),this,SLOT(swMessageFilterChangedSlot()));
    QObject::connect(ui->history,SIGNAL(triggered(QAction*)),this,SLOT(trigerHistorySlot(QAction*)));
    
    isConnectTimeFilter(true);  //建立时间过滤器的信号与槽的连接
    isConnectScroll(true);      //建立内容显示区的垂直滚动条的信号与槽的连接
    
}


/*
 * 函数名称：    ~MainWindow()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.1.20
 * 函数功能：    析构函数，用于释放软件所占用的资源
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
MainWindow::~MainWindow()
{
    delete ui;
}


/*
 * 函数名称：    loadSoftwareSetting()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.16
 * 函数功能：    加载软件的设置文件
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::loadSoftwareSetting(QString path)
{
    //创建配置文件解析器
    mSettingParse = new ParseSettingFile(this,path);
    //从解析器中获取解析后的参数
    mCurFontType = mSettingParse->getFontType();
    mCurFontSize = mSettingParse->getFontSize();
    mCurCodeType = mSettingParse->getCodeType();
    ADBPath = mSettingParse->getADBPath();
    mRegisterNum = mSettingParse->getRegisterNum();
    mHistory = mSettingParse->getHistory();
    
    Security *security = Security::getInstance();
    if (security->checkRegisterNum(mRegisterNum)) {
        security->setRegisted(true);
    } else {
        security->setRegisted(false);
    }
    
    updateHistoryAction(mHistory);
}


/*
 * 函数名称：    initEnvironment()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.14
 * 函数功能：    初始化相关变量
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::initEnvironment()
{
    mLogCacheFilePath = QCoreApplication::applicationDirPath() + LOG_CACHE_FILE_PATH;
    logCurTime("当前日志缓存路径：" + mLogCacheFilePath);
    
    mSettingFilePath = QCoreApplication::applicationDirPath() + SETTING_FILE_PATH;
    logCurTime("当前全局设置文件路径：" + mSettingFilePath);
    
    //加载软件的设置文件，并设置相应的参数
    loadSoftwareSetting(mSettingFilePath);
    
    //初始化文件打开历史记录
    if (ui->history->actions().size() > 2) {
        ui->history->setEnabled(true);
    } else {
        ui->history->setEnabled(false);
    }
    
    //创建软件底部状态栏的标签
    fileLabel = new QLabel;
    adbDeviceLabel = new QLabel;
    workModeLabel = new QLabel;
    
    //样式表的设置
    ui->statusBar->setStyleSheet("QStatusBar::item{border:0px;}"
                                 "QStatusBar{background-color:rgb(54,54,54);}");
    fileLabel->setStyleSheet("background-color:rgb(30,30,30);color:rgb(200,200,200);");
    adbDeviceLabel->setStyleSheet("background-color:rgb(30,30,30);color:rgb(200,200,200);");
    workModeLabel->setStyleSheet("background-color:rgb(30,30,30);color:rgb(200,200,200);");
    
    fileLabel->setMaximumWidth(1000);
    
    ui->statusBar->addWidget(workModeLabel);
    ui->statusBar->addWidget(adbDeviceLabel);
    ui->statusBar->addWidget(fileLabel);
    
    //软件启动时默认为“离线查看模式”
    adbDeviceLabel->setText(NO_ADB_DEVICE);
    workModeLabel->setText(WORK_MODE_OFFLINE);

    //时间信息过滤器默认失能
    ui->swTimeFilter->setChecked(false);

    //获取UI控件对象
    lwFilter = ui->lwFilter;
    lwContent = ui->lwContent;
    labelLine = ui->labelLine;
    labelTime = ui->labelTime; 
    labelLevel = ui->labelLevel;
    labelPID = ui->labelPID;
    labelTag = ui->labelTag;
    labelText = ui->labelText;

    //相关变量初始化
    isDispTime = true;
    isDispLevel = true;
    isDispPID = true;
    isDispTag = true;
    isDispHead = true;
    logFileSize = 0;
    mCurLogFilePath = "";
    cmdProcess = NULL;
    mCurADBDeviceStatus = false;
    isTerminalMode = false;
    mLastScrollValue = 0;
    isCheckedUpdateFile = false;
    mReConnecCnt = 0;
    isLoadingFile = false;
    isScaningADBDevice = false;
    mGotoLine = "";
    mAllLines = 0;
    mCurLines = 0;
    mCurOptDialog = NULL;
    mEventLoop = new QEventLoop;
    mCurRange = new ItemsRange(this,0,0,0,0,0);
    
    ui->verticalScrollBar->setHidden(true);

    eventLoopTimer = new QTimer(this);
    QObject::connect(eventLoopTimer,SIGNAL(timeout()),this,SLOT(eventLoopTimeoutSlot()));
    
    fileTimer = new QTimer(this);
    QObject::connect(fileTimer,SIGNAL(timeout()),this,SLOT(fileTimeoutSlot()));

    statusTimer = new QTimer(this);
    QObject::connect(statusTimer,SIGNAL(timeout()),this,SLOT(statusTimeoutSlot()));
    
    clearAllDataList();      //初始化全局数据表

    //设置文本字体样式
    QFont font = QFont(mCurFontType,mCurFontSize,QFont::Normal);
    lwFilter->setFont(font);
    lwFilter->setStyleSheet("QListWidget{color:#FFC5C5C5;}"
                            "QListWidget::Item:selected{background:#80ABABAB;}"
                            "QListWidget::Item:hover{background:#40ABABAB;}");

    lwContent->setFont(font);
    lwContent->setStyleSheet("QListWidget::Item:selected{background:#80ABABAB;}"
                             "QListWidget::Item:hover{background:#40ABABAB;}");

    labelLine->setFont(font);
    labelTime->setFont(font);
    labelLevel->setFont(font);
    labelPID->setFont(font);
    labelTag->setFont(font);
    labelText->setFont(font);
    
}





/*******************************************************************************************
 *                                    普通槽函数区域
 * *****************************************************************************************/

/*
 * 函数名称：    trigerMenuSlot(QAction* action)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    软件菜单栏触发槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::trigerMenuSlot(QAction* action)
{
    //打开文件动作
    if (action == ui->actionOpen) {
        openFileAction();
    }
    //关闭文件动作
    else if (action == ui->actionClose) {
        closeFileAction();
    }
    //设置动作
    else if (action == ui->actionSetting) {
        setingAction();
    }
    //关于软件动作
    else if (action == ui->actionAbout) {
        aboutAction();
    } 
    //退出软件
    else if (action == ui->actionExit) {
        close();
    }
    //跳转到行
    else if (action == ui->actionGotoLine) {
        gotoLineAction();
    }
    //软件注册
    else if (action == ui->actionRegister) {
        registerSolftware();
    }
}


/*
 * 函数名称：    trigerToolSlot(QAction *action)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.15
 * 函数功能：    工具栏触发槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::trigerToolSlot(QAction *action)
{
    //重载文件动作
    if (action == ui->actionRefresh) {
        refreshLogFileAction();
    }
    //ADB连接动作
    else if (action == ui->actionConnectADB) {
        connectADBAction();
    }
    //工作模式切换动作
    else if (action == ui->actionTerminal) {
        clickTerminalAction();
    }
}


/*
 * 函数名称：    trigerHistorySlot(QAction *action)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.16
 * 函数功能：    文件打开历史记录触发槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::trigerHistorySlot(QAction *action)
{
    //如果是清空历史记录
    if (action->text() == DEFAULT_HISTORY_ACTION) {
        QList<QAction*> list = ui->history->actions();
        QMenu *history = ui->history;
        int size = list.size();
        
        for (int i = 0; i < (size - 1); ++i) {
            history->removeAction(list.at(i));
        }
        
        history->setEnabled(false);
    } 
    //否则为打开选中的文件
    else {
        QString path = action->text();
        //如果选中的文件还未打开
        if (path != mCurLogFilePath) {
            loadFile(path);
        } else {
            QMessageBox::information(this,"警告","该文件已经打开！");
        }
    }
}


/*
 * 函数名称：    logLevelChangedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    日志等级发生了改变
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::logLevelChangedSlot()
{
    dispContainSearchString(ui->etSearch->text());
}


/*
 * 函数名称：    filterChangedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    当前选中的过滤器发生了变化
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::filterChangedSlot()
{
    dispContainSearchString(ui->etSearch->text());
    
    //如果当前是实时终端模式，切换后显示内容的最后一行，以便达到显示最新内容的目的
    if (isTerminalMode) {
        ui->verticalScrollBar->setValue(ui->verticalScrollBar->maximum());
        lwContent->setCurrentRow(lwContent->count() - 1);
    }
}


/*
 * 函数名称：    searchEditChangedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    日志搜索栏发生了改变
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::searchEditChangedSlot()
{
    dispContainSearchString(ui->etSearch->text());
}


/*
 * 函数名称：    swTimeFilterChangedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    时间过滤开关变化槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::swTimeFilterChangedSlot()
{
    //如果使能了时间过滤器
    if (ui->swTimeFilter->isChecked()) {

        //从控件中读取时间信息并进行格式化重构
        mStartTime = ui->cbStartMonth->currentText() + "-" + ui->cbStartDay->currentText()
                + " " + ui->cbStartHour->currentText() + ":" + ui->cbStartMinute->currentText()
                + ":" + ui->cbStartSecond->currentText() + ".000";

        mStopTime = ui->cbStopMonth->currentText() + "-" + ui->cbStopDay->currentText()
                + " " + ui->cbStopHour->currentText() + ":" + ui->cbStopMinute->currentText()
                + ":" + ui->cbStopSecond->currentText() + ".000";

        //如果起始时间大于终止时间
        if (QString::compare(mStartTime,mStopTime) > 0) {
            ui->swTimeFilter->setChecked(false);
            QMessageBox::information(this,"警告","时间设置错误，应满足【起始时间 <= 终止时间】！");
            return;
        }
    }
    //如果失能了时间过滤器
    else {
        //若当前数据表为空则初始化为固定值，否则设置为整个时间区域
        if (mCurTimes.isEmpty()) {
            mStartTime = "01-01 00:00:00.000";
            mStopTime = "01-01 00:00:00.000";
        } else {
            mStartTime = mCurTimes.at(0);
            mStopTime = mCurTimes.at(mCurLines - 1);
        }

        //更新时间信息到控件显示
        updateTimeStringToWidget(mStartTime,mStopTime);
    }

    //刷新显示
    if (!mCurTimes.isEmpty()) {
        dispContainSearchString(ui->etSearch->text());
    }
}


/*
 * 函数名称：    timeFilterChangedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    时间过滤选择框变化槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::timeFilterChangedSlot()
{
    if (ui->swTimeFilter->isChecked()) {

        //从控件中读取时间信息并进行格式化重构
        mStartTime = ui->cbStartMonth->currentText() + "-" + ui->cbStartDay->currentText()
                + " " + ui->cbStartHour->currentText() + ":" + ui->cbStartMinute->currentText()
                + ":" + ui->cbStartSecond->currentText() + ".000";

        mStopTime = ui->cbStopMonth->currentText() + "-" + ui->cbStopDay->currentText()
                + " " + ui->cbStopHour->currentText() + ":" + ui->cbStopMinute->currentText()
                + ":" + ui->cbStopSecond->currentText() + ".000";

        //如果起始时间大于终止时间
        if (QString::compare(mStartTime,mStopTime) > 0) {
            ui->swTimeFilter->setChecked(false);
            QMessageBox::information(this,"警告","时间设置错误，应满足【起始时间 <= 终止时间】！");
            return;
        }

        //刷新显示
        if (!mCurTimes.isEmpty()) {
            dispContainSearchString(ui->etSearch->text());
        }
    }
}


/*
 * 函数名称：    swMessageFilterChangedSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    信息过滤开关变化槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::swMessageFilterChangedSlot()
{
    isDispTime = !ui->swTime->isChecked();
    isDispLevel = !ui->swLevel->isChecked();
    isDispPID = !ui->swPID->isChecked();
    isDispTag = !ui->swTag->isChecked();
    isDispHead = !ui->swHead->isChecked();

    //根据信息过滤器的开关状态设置是否显示对应的标签
    ui->labelTime->setVisible(isDispTime);
    ui->labelLevel->setVisible(isDispLevel);
    ui->labelPID->setVisible(isDispPID);
    ui->labelTag->setVisible(isDispTag);

    //刷新显示
    dispContainSearchString(ui->etSearch->text());
}


/*
 * 函数名称：    threadDoneSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.14
 * 函数功能：    执行ADB命令线程完毕槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::threadDoneSlot()
{
    //logCurTime("线程执行完毕");
    mEventLoop->exit();
}


/*
 * 函数名称：    showDialogSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.14
 * 函数功能：    显示等待对话框槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::showDialogSlot()
{
    //logCurTime("显示对话框");
    QString title;
    if (mCurADBDeviceStatus) {
        title = "正在重连ADB服务器....";
    } else {
        title = "正在启动ADB服务器....";
    }
    mReConnecCnt++;
     
    mWaitDialog = new QWaitDialog(this,title);
    mWaitDialog->exec();
}


/*
 * 函数名称：    closeDialogSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.14
 * 函数功能：    关闭等待对话框槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::closeDialogSlot()
{
    //logCurTime("关闭对话框");
    mWaitDialog->close();
}


/*
 * 函数名称：    gotoLineEditTextChangedSLot(QString text)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.16
 * 函数功能：    跳转到行输入编辑框当前文本发生变化触发槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::gotoLineEditTextChangedSLot(QString text)
{
    mGotoLine = text;
}


/*
 * 函数名称：    gotoLineSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.16
 * 函数功能：    跳转到指定行槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::gotoLineSlot()
{
    QString text = mGotoLine;
    if (text.isEmpty()) {
        mCurOptDialog->close();
        return;
    }
    
    int length = text.length();
    
    QChar temp;
    for (int i = 0; i < length; ++i) {
        temp = text.at(i);
        if ((temp < '0') || (temp > '9')) {
            QMessageBox::information(this,"警告","行号无效，必须为纯数字！");
            return;
        }
    }
    
    int line = text.toInt();
    if ((line < 1) || (line > mCurLines)) {
        QMessageBox::information(this,"警告","行号无效，不在指定的范围内！");
        return;
    }
    mCurOptDialog->close();
    
    //更新当前显示缓冲区范围
    int visibleFirst = line - mCurRange->getPageItemNum() / 2;
    int first = visibleFirst - 1;
    mCurRange = new ItemsRange(this,first,visibleFirst,mCurRange->getCount(),
                               mCurRange->getPageItemNum(),mCurLines);
    
    dispAreaData(mCurRange,0);  //显示缓冲区的数据
    
    //将焦点定位到跳转的行
    int count = mCurRange->getCount();
    int bitNum = QString::number(mCurLines).length();
    for (int i = 0; i < count; ++i) {
        if (line == lwContent->item(i)->text().mid(0,bitNum).toInt()) {
            lwContent->setCurrentRow(i);
            break;
        }
    }
}


/*
 * 函数名称：    selfVerticalScrollSlot(int value)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.10
 * 函数功能：    QListWidget自带的垂直滚动条值变化槽函数
 *             实现机制：通过控制自带的滚动条，来达到自定义滚动条控制显示内容的目的，
 *                      具体逻辑的实现较复杂，此处不做详细介绍；
 * 输入参数：    value:滚动条当前的值
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::selfVerticalScrollSlot(int value)
{
    //logCurTime("内部滑动：value=" + QString::number(value));
    int direction = 0;
    int step = value - mLastScrollValue;
    //与上一次的值作比较，判断滑动的方向，1表示向下滑动，-1表示向上滑动，0表示未滑动
    if (value > mLastScrollValue) {
        direction = 1;
    } else if (value < mLastScrollValue) {
        direction = -1;
    }
    
    //logCurTime("由" + QString::number(mLastScrollValue) + " >> " + QString::number(value));
    //如果是向下滑动
    if (direction == 1) {
        int first = mCurRange->getFirst();
        int visibleFirst = mCurRange->getVisibleFirst();
        int count = mCurRange->getCount();
        if ((visibleFirst != 0) 
                && (mCurRange->getLast() != (mCurLines - 1))) {
            first += step;
        } else {
            mLastScrollValue = value;
        }
        visibleFirst += step;
        mCurRange = new ItemsRange(this,first,visibleFirst,count,mCurRange->getPageItemNum(),mCurLines);
        dispAreaData(mCurRange,direction);
    } 
    //如果是向上滑动
    else if (direction == -1) {
        int first = mCurRange->getFirst();
        int visibleFirst = mCurRange->getVisibleFirst();
        int count = mCurRange->getCount();
        if ((first != 0) 
                && (mCurRange->getLast() != (mCurLines - 1))) {
            first += step;
        } else if (mCurRange->getLast() == (mCurLines - 1)) {
            if ((visibleFirst + step) <= first) {
                first += step;
                if ((value == 0) && (first != 0)) {
                    mLastScrollValue = visibleFirst + step - first;
                }
            } else {
                mLastScrollValue = value;
            }
        } else {
            mLastScrollValue = value;
        }
        visibleFirst += step;
        mCurRange = new ItemsRange(this,first,visibleFirst,count,mCurRange->getPageItemNum(),mCurLines);
        dispAreaData(mCurRange,direction);
    } 
    //如果未滑动
    else if (value == 0) {
        mLastScrollValue = 3;
        lwContent->verticalScrollBar()->setValue(1);
    }
    
}


/*
 * 函数名称：    verticalScrollSlot(int value)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.10
 * 函数功能：    自定义的垂直滚动条值变化槽函数
 * 输入参数：    value:滚动条当前的值
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::verticalScrollSlot(int value)
{
    //logCurTime("外部滑动：value=" + QString::number(value));
    int visibleFirst = value;
    int first = (visibleFirst > 0) ? (visibleFirst - 1) : visibleFirst;
    int count = mCurRange->getCount();
    mCurRange = new ItemsRange(this,first,visibleFirst,count,mCurRange->getPageItemNum(),mCurLines);
    dispAreaData(mCurRange,0);
}





/*******************************************************************************************
 *                                   定时器槽函数区域
 * *****************************************************************************************/

/*
 * 函数名称：    fileTimeoutSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    日志缓存文件大小监控定时器槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::fileTimeoutSlot()
{
    int size = logFileSize;
    QFile *file = new QFile(mLogCacheFilePath);
    if (file->exists()) {
        size = file->size();
    }
    file->close();

    
    //如果文件大小发生了变化，则重新加载该文件，以显示最新的文件内容，达到实时显示的目的
    if (logFileSize != size) {
        //logCurTime("logFileSize=" + QString::number(logFileSize) + ", size=" + QString::number(size));
        logFileSize = size;
        
        //记录更新文件内容前的相关显示参数
        int oldFilterRow = lwFilter->currentRow();
        int oldContentRow = lwContent->currentRow();
        int oldFilterScrollPosition = lwFilter->verticalScrollBar()->value();
        bool isMax = (ui->verticalScrollBar->value() == ui->verticalScrollBar->maximum()) ? true : false;
        oldFilterRow = (oldFilterRow > 0) ? oldFilterRow : 0;
        oldContentRow = (oldContentRow > 0) ? oldContentRow : 0;
        oldFilterScrollPosition = (oldFilterScrollPosition > 0) ? oldFilterScrollPosition : 0;
        
        //更新文件内容
        updateFile(mLogCacheFilePath);
        
        //恢复更新文件前的相关显示参数
        if(oldFilterRow >= lwFilter->count()) {
            oldFilterRow = lwFilter->count() - 1;
            oldFilterRow = (oldFilterRow > 0) ? oldFilterRow : 0;
        }
        lwFilter->setCurrentRow(oldFilterRow);
        
        int max = lwFilter->verticalScrollBar()->maximum();
        if(oldFilterScrollPosition > max) {
            oldFilterScrollPosition = max;
        }
        lwFilter->verticalScrollBar()->setValue(oldFilterScrollPosition);
        
        //如果更新前显示的是最后一行，则表示实时显示最新内容；否则当前显示内容不变
        if (isMax) {
            ui->verticalScrollBar->setValue(ui->verticalScrollBar->maximum());
            lwContent->setCurrentRow(lwContent->count() - 1);
        } else {
            if(oldContentRow >= lwContent->count()) {
                oldContentRow = lwContent->count() - 1;
                oldContentRow = (oldContentRow > 0) ? oldContentRow : 0;
            }
            lwContent->setCurrentRow(oldContentRow);
        }
    }
}


/*
 * 函数名称：    statusTimeoutSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    ADB设备连接状态监控定时器槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::statusTimeoutSlot()
{
    QString info = "";
    //如果当前ADB设备连接成功了
    if (isADBConnectedSuccess(mCurADBDevice)) {
        //如果上一次是未连接状态，且当前是实时终端模式，则表示是重连接，
        //需要通过手动模拟再次进入实时终端模式，以达到自动恢复显示的目的
        if(!mCurADBDeviceStatus && isTerminalMode) {
            isTerminalMode = false;
            clickTerminalAction();    //执行该函数之前mCurADBDeviceStatus必须为false
        }
        mCurADBDeviceStatus = true;
        info = "连接成功";
        mReConnecCnt = 0;           //如果连接成功了，则清空重连计数器
    } else {
        mCurADBDeviceStatus = false;
        fileTimer->stop();
        //判断该设备是否还存在
        if (!isExistDevice(mCurADBDevice)) {
            info = "设备丢失";
        } else {
            info = "连接断开";
        }
        
        //如果重连3次均失败，则退出ADB环境
        if (mReConnecCnt >= 3) {
            mReConnecCnt = 0;
            QMessageBox::information(this,"警告","3次重连失败，即将退出ADB环境！");
            
            mCurADBDeviceStatus = false;    //更新当前ADB设备的连接状态
            statusTimer->stop();            //停止状态监控定时器
            mCurADBDevice = "";             //清空当前设备名
            adbDeviceLabel->clear();        //清空软件底部状态栏的显示
            
            //如果当前处于实时终端模式
            if (isTerminalMode) {
                isTerminalMode = false;     //退出实时终端模式
                isCheckedUpdateFile = false;
                closeFileAction();          //关闭当前加载的文件
                clearLogCacheFile();        //清空日志缓存文件
                workModeLabel->setText(WORK_MODE_OFFLINE);
            }
        }
    }
    
    //更新软件底部状态栏的显示
    adbDeviceLabel->setText("  【当前ADB设备】： " + mCurADBDevice + "  " + info + "  ");
}


/*
 * 函数名称：    eventLoopTimeoutSlot()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.15
 * 函数功能：    主线程的事件循环控制定时器槽函数，仅执行一次
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::eventLoopTimeoutSlot()
{
    mEventLoop->exit();         //退出事件阻塞，使主线程恢复执行
    eventLoopTimer->stop();     //关闭该定时器，保证单次执行即可
}





/*******************************************************************************************
 *                                    Action动作函数区域
 * *****************************************************************************************/

/*
 * 函数名称：    setingAction()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    设置菜单点击动作
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::setingAction()
{
    //创建设置对话框并显示
    Setting *dialog = new Setting;
    dialog->transParams(mCurFontType,mCurFontSize,mCurCodeType,ADBPath);
    dialog->exec();

    //如果点击了确定按钮，则设置属性生效
    if (dialog->getIsClickTrue()) {
        QStringList paramList;  //创建存储参数表
        
        bool isNeedUpdateFont = false;
        //判断当前字体是否有变化
        if (mCurFontType != dialog->getFontType()) {
            isNeedUpdateFont = true;
            mCurFontType = dialog->getFontType();
            paramList.append(mSettingParse->createParamsItem(FONT_TYPE,mCurFontType));
        }
        
        //判断当前字体大小是否有变化
        if (mCurFontSize != dialog->getFontSize()) {
            isNeedUpdateFont = true;
            mCurFontSize = dialog->getFontSize();
            paramList.append(mSettingParse->createParamsItem(FONT_SIZE,QString::number(mCurFontSize)));
        }

        //判断是否需要更新字体
        if (isNeedUpdateFont) {
            QFont font = QFont(mCurFontType,mCurFontSize,QFont::Normal);
            lwFilter->setFont(font);
            lwContent->setFont(font);
    
            labelTime->setFont(font);
            labelLevel->setFont(font);
            labelText->setFont(font);
            labelTag->setFont(font);
    
            autoAdjustTitleLabel();//自动调节标题标签的位置
        }

        //判断当前ADB路径是否有变化
        if (ADBPath != dialog->getADBPath()) {
            ADBPath = dialog->getADBPath();
            paramList.append(mSettingParse->createParamsItem(ADB_PATH,ADBPath));
        }

        //如果编码格式发生了变化则重新加载当前日志文件
        if (mCurCodeType != dialog->getCodeType()) {
            mCurCodeType = dialog->getCodeType();
            paramList.append(mSettingParse->createParamsItem(CODE_TYPE,mCurCodeType));
            if (!mCurLogFilePath.isEmpty()) {
                refreshLogFileAction();
            }
        }
        
        //将新的设置更新到设置文件中
        bool isSaveSuccess = mSettingParse->saveParamsToFile(mSettingFilePath,paramList);
        if (isSaveSuccess) {
            logCurTime("保存配置参数成功！");
        } else {
            logCurTime("保存配置参数失败！");
        }
    }
}


/*
 * 函数名称：    openFileAction()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    打开文件菜单点击动作
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::openFileAction()
{  
    //打开后缀为*.txt的日志文件
    QString path = QFileDialog::getOpenFileName(this,"请选择LogCat文件",
                  "C:/Users/hexiaolong/Desktop","(*.txt)");
    //logCurTime("path=" + path);

    //如果选择某一路径后点击了确定按钮
    if (!path.isEmpty()) {
        loadFile(path); //加载该文件
    }
}


/*
 * 函数名称：    closeFileAction()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    关闭文件菜单点击动作，释放相应的资源
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::closeFileAction()
{
    ui->verticalScrollBar->setHidden(true);
    ui->etSearch->clear();
    ui->cbLevel->setCurrentIndex(0);
    lwContent->clear();
    ui->swTimeFilter->setChecked(false);
    mCurLogFilePath = "";
    fileLabel->clear();
    clearAllDataList();
}


/*
 * 函数名称：    aboutAction()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.6
 * 函数功能：    关于软件动作
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::aboutAction()
{
    aboutSoftware *dialog = new aboutSoftware;
    dialog->exec();
}


/*
 * 函数名称：    refreshLogFileAction()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    重载当前日志文件槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::refreshLogFileAction()
{
    if (mCurLogFilePath.isEmpty()) {
        QMessageBox::information(this,"警告","当前无文件打开，无法重载！");
        return;
    }
    
    loadFile(mCurLogFilePath);
}


/*
 * 函数名称：    clickTerminalAction()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    终端模式开关槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::clickTerminalAction()
{
    //如果当前是实时终端模式
    if (isTerminalMode) {
        //如果确认退出实时终端模式
        if (QMessageBox::Ok == QMessageBox::information(this,"警告","是否要退出终端模式？"
                                                        ,QMessageBox::Ok,QMessageBox::Cancel)) {
            //workModeLabel->setText("\t\t正在进入离线查看模式....");
            isTerminalMode = false; //更新当前工作模式状态
            isCheckedUpdateFile = false;
            closeFileAction();      //关闭加载的日志缓存文件
            clearLogCacheFile();    //清空日志缓存文件
            workModeLabel->setText(WORK_MODE_OFFLINE);
        }
    } 
    //如果当前是离线查看模式
    else {
        //ADBPath = "D:/ruanjain/工具类/ADB调试";
        //如果设置了ADB工具路径
        if (ADBPath != "") {
            //当前是否连接了ADB设备
            if (mCurADBDevice != "") {
                //先判断日志缓存路径是否存在，若不存在则立即创建该路径
                if (!isDirExist(mLogCacheFilePath,true)) {
                    QMessageBox::information(this,"警告","创建日志缓存文件失败，无法进入实时终端模式！");
                    return;
                }
                
                isTerminalMode = true;  //更新当前工作模式状态
                //workModeLabel->setText("\t\t正在进入实时终端模式....");
                
                //创建CMD进程，用于将Android日志输出到指定文件
                cmdProcess = new QProcess(this);
                cmdProcess->setProcessChannelMode(QProcess::MergedChannels);
                cmdProcess->setWorkingDirectory(ADBPath);   //必须设置正确的进程工作路径，否则将找不到ADB工具
    
                //如果已经存在日志缓存文件，则获取其大小，否则该大小默认为0
                QFile *file = new QFile(mLogCacheFilePath);
                if (file->exists()) {
                    logFileSize = file->size();
                } else {
                    logFileSize = 0;
                }
                file->close();
    
                //logCurTime("开始清空缓存...");
                //清空日志输出的缓存
                QString cmd = "adb -s " + mCurADBDevice + " logcat -c";
                //此处if的目的是：当显示过程中ADB设备连接突然断开或丢失后再次重连时，不需要清空缓存
                if (mCurADBDeviceStatus) {
                    cmdProcess->start("cmd",QStringList()<<"/c"<<cmd);
                    cmdProcess->waitForStarted();
                    cmdProcess->waitForFinished();
                }
    
                //logCurTime("清空缓存完毕");
                //开始重新输出日志到指定文件
//                cmd = "adb -s " + mCurADBDevice + " logcat -b main -v time -s TableView>"
//                        + mLogCacheFilePath;
                cmd = "adb -s " + mCurADBDevice + " logcat -b main -v time>"
                        + mLogCacheFilePath;
                cmdProcess->reset();
                cmdProcess->start("cmd",QStringList()<<"/c"<<cmd);
                
                isCheckedUpdateFile = false;
                closeFileAction();
                
                //启动日志缓存文件大小监控定时器（10ms检测一次是否有变化）
                fileTimer->start(10);
                workModeLabel->setText(WORK_MODE_ONLINE);
                //logCurTime("启动了定时器");
            } else {
                QMessageBox::information(this,"警告","请先选择一个ADB设备！");
            }
        } else {
            QMessageBox::information(this,"警告","未设置ADB工具路径，请检查！");
        }
    }
}


/*
 * 函数名称：    connectADBAction()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    连接或断开ADB设备槽函数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::connectADBAction()
{
    //如果当前ADB设备是连接状态
    if (mCurADBDeviceStatus) {
        //如果确认断开当前ADB连接
        if (QMessageBox::Ok == QMessageBox::information(this,"警告","是否要断开当前ADB连接？"
                                                        ,QMessageBox::Ok,QMessageBox::Cancel)) {
            mCurADBDeviceStatus = false;    //更新当前ADB设备的连接状态
            statusTimer->stop();            //停止状态监控定时器
            execWindowsCmd(ADBPath,"adb kill-server");  //关闭ADB服务器
            mCurADBDevice = "";             //清空当前设备名
            adbDeviceLabel->clear();        //清空软件底部状态栏的显示
            
            //如果当前处于实时终端模式
            if (isTerminalMode) {
                isTerminalMode = false;     //退出实时终端模式
                isCheckedUpdateFile = false;
                closeFileAction();          //关闭当前加载的文件
                clearLogCacheFile();        //清空日志缓存文件
                workModeLabel->setText(WORK_MODE_OFFLINE);
            }
        }
    } 
    //如果当前ADB设备未连接，且未正在扫描ADB设备
    else if (!isScaningADBDevice) {
        isScaningADBDevice = true;
        //ADBPath = "D:/ruanjain/工具类/ADB调试";
        //如果设置了ADB工具路径
        if (ADBPath != "") {
            adbDeviceLabel->setText("  正在扫描ADB设备....  ");
            QStringList result = execWindowsCmd(ADBPath,"adb start-server");//启动ADB服务器
            if (!result.at(0).isEmpty() && !result.at(0).contains("successfully")) {
                isScaningADBDevice = false;
                QMessageBox::information(this,"警告","ADB路径错误，请重新设置！");
                return;
            }
            
            QStringList deviceNames = getCurOnlineDeviceNames();    //获取当前在线的ADB设备表
            
            //如果有设备存在
            if (deviceNames.size() != 0) {
                adbDeviceLabel->setText("  扫描完成，发现了" + QString::number(deviceNames.size()) + "个设备  ");
                
                //创建并显示设备选择对话框
                ConnectADB *dialog = new ConnectADB;
                dialog->transParams(deviceNames);
                dialog->exec(); //阻塞式，直到用户点击了按钮返回
                
                //如果点击了确定
                if (dialog->getIsClickTrue()) {
                    mCurADBDevice = dialog->getDeviceName();        //获取选中的设备
                    QString info = "";
                    if (mCurADBDevice == "") {
                        mCurADBDeviceStatus = false;
                        info = "无设备";
                    }

                    //判断该设备是否连接成功
                    if (isADBConnectedSuccess(mCurADBDevice)) {
                        statusTimer->start(1000);       //启动ADB设备连接状态监控定时器（1s监测一次）
                        mCurADBDeviceStatus = true;
                        info = "连接成功";
                    } else {
                        mCurADBDeviceStatus = false;
                        info = "连接失败";
                    }
    
                    //更新软件底部的状态栏显示
                    adbDeviceLabel->setText("  【当前ADB设备】： " + mCurADBDevice + "  " + info + "  ");
                } else {
                    adbDeviceLabel->clear();
                }
            } else {
                QMessageBox::information(this,"警告","未发现设备");
                adbDeviceLabel->setText(NO_ADB_DEVICE);
            }
        } else {
            QMessageBox::information(this,"警告","请先设置ADB工具路径，再进行连接！");
        }
        
        isScaningADBDevice = false;
    }
}


/*
 * 函数名称：    gotoLineAction()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.16
 * 函数功能：    跳转到行
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::gotoLineAction()
{
    if (mCurLogFilePath.isEmpty()) {
        QMessageBox::information(this,"警告","当前无文件打开，无法进行行跳转！");
        return;
    }
    
    QVBoxLayout *vLayout = new QVBoxLayout;
    QHBoxLayout *hLayout1 = new QHBoxLayout;
    QHBoxLayout *hLayout2 = new QHBoxLayout;
    QHBoxLayout *hLayout3 = new QHBoxLayout;
    
    QLabel *label1 = new QLabel("当前行范围：0~" + QString::number(mCurLines));
    hLayout1->addWidget(label1);
    
    QLabel *label2 = new QLabel("跳转到行：");
    QLineEdit *edit = new QLineEdit;
    hLayout2->addWidget(label2);
    hLayout2->addWidget(edit);
    
    QPushButton *btnOK = new QPushButton("OK");
    QPushButton *btnCancel = new QPushButton("Cancel");
    hLayout3->addWidget(btnOK);
    hLayout3->addWidget(btnCancel);
    
    vLayout->addLayout(hLayout1);
    vLayout->addLayout(hLayout2);
    vLayout->addLayout(hLayout3);
    
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("跳转到行");
    dialog->setLayout(vLayout);
    dialog->adjustSize();                   //调整对话框尺寸，使其刚好容纳所有子控件
    dialog->setFixedSize(dialog->size());   //固定窗口大小
    
    QObject::connect(edit,SIGNAL(textChanged(QString)),this,SLOT(gotoLineEditTextChangedSLot(QString)));
    QObject::connect(btnCancel,SIGNAL(clicked()),dialog,SLOT(close()));
    QObject::connect(btnOK,SIGNAL(clicked()),this,SLOT(gotoLineSlot()));
    
    mCurOptDialog = dialog;
    
    dialog->exec();
}


/*
 * 函数名称：    registerSolftware()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.16
 * 函数功能：    注册软件动作
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::registerSolftware()
{
    Security *security = Security::getInstance();
    if (security->isRegisted()) {
        QMessageBox::information(this,"提示","软件已注册，注册码为：" + mRegisterNum);
    } else {
        //创建注册对话框并显示
        Register *dialog = new Register;
        dialog->exec();
    
        //如果点击了确定按钮且注册成功了
        if (dialog->getIsClickTrue()) {
            mRegisterNum = dialog->getRegisterNum();
            mSettingParse->saveRegisterNumToFile(mSettingFilePath,mRegisterNum);
        }
    }
}





/*******************************************************************************************
 *                                     私有函数区域
 * *****************************************************************************************/

/*
 * 函数名称：    closeEvent()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    应用程序右上角X关闭触发事件
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::closeEvent(QCloseEvent *event)
{
    //提醒用户是否退出软件，防止误操作使软件关闭而导致重要数据的丢失
    if (QMessageBox::Ok == QMessageBox::information(this,"警告",
                           "是否退出软件？",QMessageBox::Ok,QMessageBox::Cancel)) {
        event->accept();    //接收该事件，即允许退出软件
    } else {
        event->ignore();    //忽略该事件，即不允许退出软件
        return;
    }

    //关闭ADB设备连接状态监控定时器
    if (statusTimer->isActive()) {
        statusTimer->stop();
    }

    //关闭ADB服务器，停止所有的ADB进程
    execWindowsCmd(ADBPath,"adb kill-server");
    
    //清空日志缓存文件（如果存在的话）
    clearLogCacheFile();
}


/*
 * 函数名称：    clearLogCacheFile()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    清空日志缓存文件（如果存在的话）
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::clearLogCacheFile()
{
    //关闭日志缓存文件大小监控定时器
    if (fileTimer->isActive()) {
        logCurTime("fileTimer is Active");
        fileTimer->stop();
    }
    
    QFile *file = new QFile(mLogCacheFilePath);
    //如果日志缓存文件存在
    if (file->exists()) {
        //此处代码目的：避免用户误点击导致日志缓存文件被删除，在删除前添加了确认对话框
        //用户是否先将缓存文件另存为
        while (1) {
            //如果需要将日志缓存文件另存为
            if (QMessageBox::Ok == QMessageBox::information(this,"警告",
                                   "是否保存日志缓存文件？"
                                   ,QMessageBox::Ok,QMessageBox::Cancel)) {
                //设置要保存的路径和文件名
                QString fileName = QFileDialog::getSaveFileName(this, "保存文件",
                                           mLogCacheFilePath,"Text files (*.txt)");
                
                //如果设置了另存为的路径
                if (!fileName.isEmpty()) {
                    QFile saveFile(fileName);
                    //如果已经存在该文件，则选择是否覆盖
                    if (saveFile.exists()) {
                        if (QMessageBox::Ok == QMessageBox::information(this,"警告",
                                               "该文件已经存在，是否确定要覆盖？"
                                               ,QMessageBox::Ok,QMessageBox::Cancel)) {
                            saveFile.remove();
                            if (file->copy(fileName)) {
                                QMessageBox::information(this,"警告","保存成功！");
                            } else {
                                QMessageBox::information(this,"警告","保存失败！");
                            }
                            break;
                        }
                    } else {
                        if (file->copy(fileName)) {
                            QMessageBox::information(this,"警告","保存成功！");
                        } else {
                            QMessageBox::information(this,"警告","保存失败！");
                        }
                        break;
                    }
                } else {
                    QMessageBox::information(this,"警告","另存为的文件名为空，请重新设置！");
                }
            } 
            //如果不需要保存日志缓存文件
            else {
                //再次提醒用户是否真的不需要保存，因为一旦该文件被删除，
                //将无法再次回复，以免造成重要的调试信息因为误操作而丢失。
                if (QMessageBox::Ok == QMessageBox::information(this,"警告",
                                       "缓存文件即将被删除，是否确认？"
                                       ,QMessageBox::Ok,QMessageBox::Cancel)) {
                    break;
                }
            }
        }
        
        //关闭CMD进程
        if (cmdProcess != NULL) {
            logCurTime("!=NULL");
            if (cmdProcess->Running) {
                logCurTime("Running");
                cmdProcess->close();
                cmdProcess->kill();
                cmdProcess = NULL;
            }
        }
        
        //此处必须关闭ADB服务器，否则缓存文件一直被该进程占用着，导致无法删除该文件
        execWindowsCmd(ADBPath,"adb kill-server");
        
        //删除日志缓存文件
        file->remove();
    }
}


/*
 * 函数名称：    execWindowsCmd(QString adbPath,QString cmd)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    执行CMD命令
 * 输入参数：    adbPath:ADB工具路径     cmd:要执行的命令
 * 输出参数：    无
 * 返回值：      返回字符串列表，包含两个元素，其中[0]:表示执行结果返回； [1]:表示执行错误返回
*/
/*QStringList MainWindow::execWindowsCmd(QString adbPath,QString cmd)
{
    QStringList result;

    if (adbPath == "") {
        result.append("");
        result.append("未设置ADB路径");
        return result;
    }

    QProcess process(0);
    process.setWorkingDirectory(adbPath);
    process.start("cmd",QStringList()<<"/c"<<cmd);
    process.waitForStarted();
    process.waitForFinished(5000);

    QString out = QString::fromUtf8(process.readAllStandardOutput());
    QString err = QString::fromUtf8(process.readAllStandardError());

    process.close();
    process.kill();

    out = out.trimmed();
    err = err.trimmed();

    result.append(out);
    result.append(err);

    return result;
}*/


/*
 * 函数名称：    execWindowsCmd(QString adbPath,QString cmd)
 * 函数版本：        2.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.13
 * 函数功能：    执行CMD命令，该版本为通过启动一个后台线程去执行，避免耗时等待阻塞UI线程
 * 输入参数：    adbPath:ADB工具路径     cmd:要执行的命令
 * 输出参数：    无
 * 返回值：      返回字符串列表，包含两个元素，其中[0]:表示执行结果返回； [1]:表示执行错误返回
*/
QStringList MainWindow::execWindowsCmd(QString adbPath,QString cmd)
{
    QStringList result;
    
    //创建执行ADB命令的线程，并建立相关的信号与槽的连接
    ADBThread *thread = new ADBThread(this);
    QObject::connect(thread,SIGNAL(done()),this,SLOT(threadDoneSlot()));
    QObject::connect(thread,SIGNAL(showDialog()),this,SLOT(showDialogSlot()));
    QObject::connect(thread,SIGNAL(closeDialog()),this,SLOT(closeDialogSlot()));
    
    thread->transParams(adbPath,cmd);   //必须先向该线程传入执行参数
    thread->start();                    //启动线程
    mEventLoop->exec();                 //将主线程阻塞，以便等待ADB线程的命令执行完毕
    result = thread->getResult();       //获取ADB命令执行的结果

    return result;
}


/*
 * 函数名称：    isADBConnectedSuccess(QString deviceName)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    判断某个ADB设备是否连接成功
 * 输入参数：    deviceName:设备名
 * 输出参数：    无
 * 返回值：      连接成功返回true，否则返回false
*/
bool MainWindow::isADBConnectedSuccess(QString deviceName)
{
    if ((deviceName == "") || !isExistDevice(deviceName)) {
        return false;
    }

    //判断方式：通过执行shell命令"pwd"打印当前路径，若返回"/"则表示连接成功
    QStringList result = execWindowsCmd(ADBPath,"adb -s " + deviceName + " shell pwd");
//    logCurTime("【out】" + result.at(0));
//    logCurTime("【err】" + result.at(1));

    if (result.at(1).startsWith(ADB_ERR_RESULT)) {
        return false;
    }

    if (result.at(0) == "/") {
        return true;
    } else {
        return false;
    }
}


/*
 * 函数名称：    getCurOnlineDeviceNames()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    获取当前在线的ADB设备
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回在线的设备名列表
*/
QStringList MainWindow::getCurOnlineDeviceNames()
{
    QStringList result = execWindowsCmd(ADBPath,"adb devices");
    QString out = result.at(0);

    if (out.contains("List of devices attached")) {
        QStringList devices = out.split("\n");
        if (devices.size() > 1) {
            QStringList deviceNames;
            for (int i = 1; i < devices.size(); ++i) {
                QString temp = devices.at(i);
                if (temp.contains("device") && (!temp.contains("List of devices attached"))) {
                    deviceNames.append(temp.remove(temp.length() - 7,7));
                }
            }

            if (deviceNames.size() > 0) {
                return deviceNames;
            }
        }
    }

    QStringList temp;

    return temp;
}


/*
 * 函数名称：    isExistDevice(QString deviceName)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    判断某个ADB设备名是否存在
 * 输入参数：    deviceName:ADB设备名
 * 输出参数：    无
 * 返回值：      若存在则返回true，否则返回false
*/
bool MainWindow::isExistDevice(QString deviceName)
{
    QStringList deviceNames = getCurOnlineDeviceNames();
    for (int i = 0; i < deviceNames.size(); ++i) {
        if (deviceNames.at(i) == deviceName) {
            return true;
        }
    }

    return false;
}


/*
 * 函数名称：    logCurTime(QString text)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.10
 * 函数功能：    带时间戳的调试打印
 * 输入参数：    text:要打印的内容
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::logCurTime(QString text)
{
    //如果调试开关打开了，则打印信息，否则不进行打印
#if DEBUG_SWITCH
    qDebug()<<"【" + QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "】" + text;
#endif
}


/*
 * 函数名称：    updateCurRange()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.10
 * 函数功能：    更新当前显示缓冲区的范围
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::updateCurRange()
{
    if (lwFilter->count() > 0) {
        int visibleHeight = lwContent->height();        //获取内容显示区的总高度
        int itemHeight = lwFilter->sizeHintForRow(0);   //获取一行所占的高度
        int visibleItemNum = visibleHeight / itemHeight;//计算可完整显示的行数
        if (visibleItemNum < 0) {
            visibleItemNum = 0;
        }
        
        //更新当前显示缓冲区范围
        int first = mCurRange->getFirst();
        int visibleFirst = mCurRange->getVisibleFirst();
        int count = visibleItemNum + visibleItemNum / 2;
        mCurRange = new ItemsRange(this,first,visibleFirst,count,visibleItemNum,mCurLevels.size());
        
//        logCurTime("mVisibleHeight=" + QString::number(visibleHeight));
//        logCurTime("mItemHeight=" + QString::number(itemHeight));
//        logCurTime("mVisibleItemNum=" + QString::number(visibleItemNum));
//        logCurTime("first=" + QString::number(first));
//        logCurTime("visibleFirst=" + QString::number(visibleFirst));
//        logCurTime("last=" + QString::number(mCurRange->getLast()));
    }
}


/*
 * 函数名称：    loadFile(QString path)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    加载一个日志文件
 * 输入参数：    path:文件的绝对路径
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::loadFile(QString path)
{
    //如果已经有文件正在加载中，则不执行任何操作
    if (isLoadingFile) {
        return;
    }
    isLoadingFile = true;
    
    //打开文件
    QFile logFile(path);
    if(!logFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(this,"警告","读取文件失败，该文件可能不存在，请检查路径！");
        isLoadingFile = false;
        return;
    }
    
    //创建并显示加载进度条
    QProgressDialog *progressDialog = new QProgressDialog;
    progressDialog->setCancelButton(0);
    progressDialog->setLabelText("正在加载文件....");
    progressDialog->setValue(0);
    progressDialog->show();
    mainThreadWait_ms(10);  //阻塞主线程10ms，只有阻塞主线程，进度条才有机会显示出来
    
    logCurTime("开始加载文件....");
    
    long fileSize = logFile.size(); //文件总大小
    int curSize = 0;                //当前已读取的大小

    logCurTime("开始读取文件....");
    long time = QDateTime::currentMSecsSinceEpoch();
    long startTime = time;
    //开始读取文件内容
    QTextStream in(&logFile);
    in.setCodec(QTextCodec::codecForName(mCurCodeType.toLatin1()));
    QString buff;
    bool isChecked = false;
    while(!in.atEnd()) {
        buff = in.readLine();
        curSize += buff.size(); //更新当前已读取的大小
        progressDialog->setValue((100 * curSize) / fileSize);   //设置进度条
        
        //如果还未对文件进行过规范性检查
        if(!isChecked)
        {
            //进行规范性检查，判断是否为Android的日志文件
            if(checkLogCatFile(buff))
            {
                isChecked = true;
                mCurLogFilePath = path;
                fileLabel->setText("  【当前日志文件路径】: " + mCurLogFilePath + "  ");
                clearAllDataList();                      //初始化相关变量
                //mFilters.append(DEFAULT_TAG);   //添加默认的过滤器
            } else {
                continue;
            }
        } else {
            if(!checkLogCatFile(buff))
            {
                continue;
            }
        }

        //存储新的日志Tag
        QStringList logList = textToList(reConstruction);
        QString tag = logList.at(3);
        if (!mFilters.contains(tag)) {
            mFilters.append(tag);
        }

        //添加到全局数据表中
        addItemToDataList(logList);
    }
    mAllLines = mAllTags.size();
    progressDialog->setValue(100);
    
    //对数据解析后识别出来的所有Tag进行重新排序
    if ((mFilters.size() > 0) && (mFilters.at(0).contains(DEFAULT_TAG))) {
        mFilters.removeAt(0);
    }
    mFilters.sort();
    mFilters.insert(0,DEFAULT_TAG);
    
    logCurTime("读取文件完毕");
    time = QDateTime::currentMSecsSinceEpoch() - time;
    logCurTime("【读取文件耗时】" + QString::number(time) + "ms");
    logFile.close();
    logCurTime("关闭文件");

    if (!isChecked) {
        QMessageBox::information(this,"警告","解析LogCat文件失败，此文件可能不是LogCat文件！");
        progressDialog->close();
        isLoadingFile = false;
        return;
    }

    logCurTime("开始构建TAG....");
    time = QDateTime::currentMSecsSinceEpoch();
    //计算每个TAG的数目并显示
    int filterNum = mFilters.size();
    int tagNum = mAllLines;
    int count = 0;
    QString text;
    QString temp;
    text = DEFAULT_TAG;
    text += "(" + QString::number(tagNum) + ")";
    lwFilter->addItem(text); 
    lwFilter->item(lwFilter->count() - 1)->setToolTip(text);
    for (int i = 1; i < filterNum; ++i) {
        count = 0;
        temp = mFilters.at(i);
        
        for (int j = 0; j < tagNum; ++j) {
            if (temp == mAllTags.at(j)) {
                count++;
            }
        }
        text = temp + "(" + QString::number(count) + ")";
        lwFilter->addItem(text);
        lwFilter->item(lwFilter->count() - 1)->setToolTip(text);
    }
    logCurTime("构建TAG完毕");
    time = QDateTime::currentMSecsSinceEpoch() - time;
    logCurTime("【构建TAG耗时】" + QString::number(time) + "ms");
    
    ui->swTimeFilter->setChecked(false);//加载文件后，设置时间过滤器为失能状态

    //最后默认选中第一行，并显示所有日志内容
    time = QDateTime::currentMSecsSinceEpoch();
    QObject::disconnect(ui->lwFilter,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                     this,SLOT(filterChangedSlot()));
    lwFilter->setCurrentRow(0);
    updateCurRange();
    QObject::connect(ui->lwFilter,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                     this,SLOT(filterChangedSlot()));
    time = QDateTime::currentMSecsSinceEpoch() - time;
    logCurTime("【设置当前行耗时】" + QString::number(time) + "ms");
    
    logCurTime("开始显示日志....");
    time = QDateTime::currentMSecsSinceEpoch();
    dispContainSearchString(ui->etSearch->text());
    logCurTime("显示完毕");
    time = QDateTime::currentMSecsSinceEpoch() - time;
    logCurTime("【显示日志耗时】" + QString::number(time) + "ms");
    
    startTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    logCurTime("【****************加载一次显示耗时】" + QString::number(startTime) + "ms");
    
    addHistoryAction(path);
    
    progressDialog->close();
    isLoadingFile = false;
}


/*
 * 函数名称：    updateFile(QString path)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.10
 * 函数功能：    更新一个日志文件的内容
 * 输入参数：    path:文件路径
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::updateFile(QString path)
{
    logCurTime("开始加载文件....");
    QFile logFile(path);
    if(!logFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(this,"警告","读取文件失败！");
        return;
    }

    logCurTime("开始读取文件....");
    long time = QDateTime::currentMSecsSinceEpoch();
    long startTime = time;
    //开始读取文件内容
    QTextStream in(&logFile);
    in.setCodec(QTextCodec::codecForName(mCurCodeType.toLatin1()));
    QString buff;
    //计算上一次已经加载了的行数
    long lineCnt = (isCheckedUpdateFile == true) ? mAllLines : 0;
    long cnt = 0;
    while(!in.atEnd()) {
        buff = in.readLine();
        cnt++;
        //对已经加载过的行数据进行跳过，不需要重复进行解析，因为数据解析比较耗时间，影响实时显示的效果
        if (cnt <= lineCnt) {
            continue;
        }
        
        //如果还未对文件进行过规范性检查
        if(!isCheckedUpdateFile)
        {
            //进行规范性检查，判断是否为Android的日志文件
            if(checkLogCatFile(buff))
            {
                isCheckedUpdateFile = true;
                mCurLogFilePath = path;
                fileLabel->setText("  【当前日志文件路径】: " + mCurLogFilePath + "  ");
                clearAllDataList();                      //初始化相关变量
                //mFilters.append(DEFAULT_TAG);   //添加默认的过滤器
            } else {
                continue;
            }
        } else {
            if(!checkLogCatFile(buff))
            {
                continue;
            }
        }

        //存储新的日志Tag
        QStringList logList = textToList(reConstruction);
        QString tag = logList.at(3);
        if (!mFilters.contains(tag)) {
            mFilters.append(tag);
        }

        //添加到全局数据表中
        addItemToDataList(logList);
    }
    mAllLines = mAllTags.size();
    
    //对数据解析后识别出来的所有Tag进行重新排序
    if ((mFilters.size() > 0) && (mFilters.at(0).contains(DEFAULT_TAG))) {
        mFilters.removeAt(0);
    }
    mFilters.sort();
    mFilters.insert(0,DEFAULT_TAG);
    
    logCurTime("读取文件完毕");
    time = QDateTime::currentMSecsSinceEpoch() - time;
    logCurTime("【读取文件耗时】" + QString::number(time) + "ms");
    logFile.close();
    logCurTime("关闭文件");

    //更新显示时如果检查不合格，不需要弹出提示框
//    if (!isCheckedUpdateFile) {
//        QMessageBox::information(this,"警告","解析LogCat文件失败，此文件可能不是LogCat文件！");
//        return;
//    }

    logCurTime("开始构建TAG....");
    time = QDateTime::currentMSecsSinceEpoch();
    lwFilter->reset();
    lwFilter->clear();
    //计算每个TAG的数目并显示
    int filterNum = mFilters.size();
    int tagNum = mAllTags.size();
    int count = 0;
    QString text;
    QString temp;
    text = DEFAULT_TAG;
    text += "(" + QString::number(tagNum) + ")";
    lwFilter->addItem(text);    
    for (int i = 1; i < filterNum; ++i) {
        count = 0;
        temp = mFilters.at(i);
        for (int j = 0; j < tagNum; ++j) {
            if (temp == mAllTags.at(j)) {
                count++;
            }
        }
        text = temp + "(" + QString::number(count) + ")";
        lwFilter->addItem(text);
    }
    logCurTime("构建TAG完毕");
    time = QDateTime::currentMSecsSinceEpoch() - time;
    logCurTime("【构建TAG耗时】" + QString::number(time) + "ms");
    
    ui->swTimeFilter->setChecked(false);//加载文件后，设置时间过滤器为失能状态

    //最后默认选中第一行，并显示所有日志内容
    time = QDateTime::currentMSecsSinceEpoch();
    QObject::disconnect(ui->lwFilter,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                     this,SLOT(filterChangedSlot()));
    lwFilter->setCurrentRow(0);
    updateCurRange();
    QObject::connect(ui->lwFilter,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                     this,SLOT(filterChangedSlot()));
    time = QDateTime::currentMSecsSinceEpoch() - time;
    logCurTime("【设置当前行耗时】" + QString::number(time) + "ms");
    
    logCurTime("开始显示日志....");
    time = QDateTime::currentMSecsSinceEpoch();
    dispContainSearchString(ui->etSearch->text());
    logCurTime("显示完毕");
    time = QDateTime::currentMSecsSinceEpoch() - time;
    logCurTime("【显示日志耗时】" + QString::number(time) + "ms");
    
    startTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    logCurTime("【****************加载一次显示耗时】" + QString::number(startTime) + "ms");
}


/*
 * 函数名称：    isTimeString(QString str)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    判断一个字符串是否为时间字符串
 * 输入参数：    str:要判断的字符串
 * 输出参数：    无
 * 返回值：      是的返回true，否则返回false
*/
bool MainWindow::isTimeString(QString str)
{
    //时间字符串的格式为："01-01 00:00:00.000"
    QStringList tempStrList = str.split(" ");

    if (tempStrList.length() != 2) {
        return false;
    }

    if (!tempStrList[0].contains("-") || (tempStrList[0].length() != 5)) {
        return false;
    }

    tempStrList = tempStrList[1].split(":");
    if (tempStrList.length() != 3) {
        return false;
    }

    if (!tempStrList[2].contains(".") || (tempStrList[2].length() != 6)) {
        return false;
    }

    return true;
}


/*
 * 函数名称：    updateTimeStringToWidget(QString startTime, QString stopTime)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    将起止时间更新到控件显示
 * 输入参数：    startTime:起始时间  stopTime:终止时间
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::updateTimeStringToWidget(QString startTime, QString stopTime)
{
    if (isTimeString(startTime) && isTimeString(stopTime)) {
        QStringList tempList = startTime.split(" ");
        QStringList tempList1 = tempList[0].split("-");
        QStringList tempList2 = tempList[1].split(":");
        QStringList tempList3 = tempList2[2].split(".");
        
        QString startMonth = tempList1[0];
        QString startDay = tempList1[1];
        QString startHour = tempList2[0];
        QString startMinute = tempList2[1];
        QString startSecond = tempList3[0];

        tempList = stopTime.split(" ");
        tempList1 = tempList[0].split("-");
        tempList2 = tempList[1].split(":");
        tempList3 = tempList2[2].split(".");
                
        QString stopMonth = tempList1[0];
        QString stopDay = tempList1[1];
        QString stopHour = tempList2[0];
        QString stopMinute = tempList2[1];
        QString stopSecond = tempList3[0];

        //如果毫秒数不为0，则将时间秒加一
        if (QString::compare(tempList3[1],"000") != 0) {
            QDateTime date = QDateTime::fromString(stopTime.split(".")[0],"MM-dd hh:mm:ss");
            QString tempStr = date.addSecs(1).toString("MM-dd hh:mm:ss");
            stopSecond = tempStr.split(":")[2];
        }

        isConnectTimeFilter(false);  //断开时间过滤器的信号与槽的连接

        ui->cbStartMonth->setCurrentIndex(ui->cbStartMonth->findText(startMonth));
        ui->cbStartDay->setCurrentIndex(ui->cbStartDay->findText(startDay));
        ui->cbStartHour->setCurrentIndex(ui->cbStartHour->findText(startHour));
        ui->cbStartMinute->setCurrentIndex(ui->cbStartMinute->findText(startMinute));
        ui->cbStartSecond->setCurrentIndex(ui->cbStartSecond->findText(startSecond));

        ui->cbStopMonth->setCurrentIndex(ui->cbStopMonth->findText(stopMonth));
        ui->cbStopDay->setCurrentIndex(ui->cbStopDay->findText(stopDay));
        ui->cbStopHour->setCurrentIndex(ui->cbStopHour->findText(stopHour));
        ui->cbStopMinute->setCurrentIndex(ui->cbStopMinute->findText(stopMinute));
        ui->cbStopSecond->setCurrentIndex(ui->cbStopSecond->findText(stopSecond));

        isConnectTimeFilter(true);  //建立时间过滤器的信号与槽的连接
    }
}


/*
 * 函数名称：    dispContainSearchString(QString str)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    显示包含搜索栏信息的所有日志信息
 * 输入参数：    str:要搜索的字符串信息
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::dispContainSearchString(QString str)
{
    if (mAllLines > 0) {
        //加载大于等于某一日志等级且指定Tag的所有日志信息
        loadLevelMessage(ui->cbLevel->currentText(), mFilters.at(ui->lwFilter->currentRow()));

        if (ui->swTimeFilter->isChecked()) {
            for (int i = 0; i < mCurTexts.size(); ++i) {
                //将不属于起止时间范围的日志从当前数据表中移除
                if (!isContainTime(mCurTimes.at(i))) {
                    mCurTimes.removeAt(i);
                    mCurLevels.removeAt(i);
                    mCurPIDs.removeAt(i);
                    mCurTags.removeAt(i);
                    mCurTexts.removeAt(i);
                    i--;
                }
            }
        }

        //如果存在搜索字符串
        if (str != "") {
            for (int i = 0; i < mCurTexts.size(); ++i) {
                //将不包含搜索条件的日志从当前数据表中移除
                if (!isContainString(mCurTexts.at(i),str)) {
                    mCurTimes.removeAt(i);
                    mCurLevels.removeAt(i);
                    mCurPIDs.removeAt(i);
                    mCurTags.removeAt(i);
                    mCurTexts.removeAt(i);
                    i--;
                }
            }
        }
        
        mCurLines = mCurTags.size();
        
        updateCurRange();   //更新当前显示缓冲区范围

        //自动更新时间过滤器的起止时间显示
        if (mCurTimes.isEmpty()) {
            mStartTime = "01-01 00:00:00.000";
            mStopTime = "01-01 00:00:00.000";
        } else {
            mStartTime = mCurTimes.at(0);
            mStopTime = mCurTimes.at(mCurLines - 1);
        }
        updateTimeStringToWidget(mStartTime,mStopTime);

        //显示当前数据表
        dispCurDataList();
    }
}


/*
 * 函数名称：    autoAdjustTitleLabel()
 * 函数版本：        2.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.14
 * 函数功能：    自动调节标签栏
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::autoAdjustTitleLabel()
{
    if (mCurLines > 0) {
        QFontMetrics fm(ui->lwContent->font());
        QString space = "";     //控制偏移的空格数
        
        //此处的6为固定的偏移，可视具体情况来调整
        labelLine->setMinimumWidth(6 + fm.width(QString::number(mCurLines)));
        labelLine->setMaximumWidth(6 + fm.width(QString::number(mCurLines)));
        
        labelTime->setMinimumWidth(fm.width("  " + mCurTimes.at(0)));
        labelTime->setMaximumWidth(fm.width("  " + mCurTimes.at(0)));
        
        labelLevel->setMinimumWidth(fm.width("  " + mCurLevels.at(0) + "  "));
        labelLevel->setMaximumWidth(fm.width("  " + mCurLevels.at(0) + "  "));
        
        space = (isDispLevel) ? "" : "    ";
        labelPID->setMinimumWidth(fm.width(space + mCurPIDs.at(0)));
        labelPID->setMaximumWidth(fm.width(space + mCurPIDs.at(0)));
        
        if(isDispPID == isDispLevel) {
            space = "    ";
        } else {
            space = "";
        }
        labelTag->setMinimumWidth(fm.width(space + mCurTags.at(0)));
        labelTag->setMaximumWidth(fm.width(space + mCurTags.at(0)));
    }
}


/*
 * 函数名称：    intToString(int num, int bitNum)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    将int数字转换为固定位数的QString，位数不足的补0
 * 输入参数：    num:int型数字  bitNum:固定的位数
 * 输出参数：    无
 * 返回值：      返回转换后的字符串
*/
QString MainWindow::intToString(int num, int bitNum)
{
    return QString::number(num).rightJustified(bitNum,'0');
}


/*
 * 函数名称：    checkLogCatFile(QString str)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    检查是否为Android的日志文件
 * 输入参数：    str:要检查的日志文件的第一行
 * 输出参数：    无
 * 返回值：      是的返回true，否则返回false
*/
bool MainWindow::checkLogCatFile(QString str)
{
    reConstruction.clear();
    QString ori = str.split("): ").at(0);
    if (ori == str) {
        return false;
    }
    QString temp = ori;
    temp = temp.remove(" ");
    if (temp.length() < 22) {
        return false;
    }
    if (":" == temp.at(17)) {
        temp.remove(17,1);
    }
    temp.insert(5," ");
    temp.insert(18," ");
    
    reConstruction.append(temp.append("):"));
    reConstruction.append(str.mid(ori.length() + 3));
    
    
    QStringList list = temp.split(" ");
    if ("-" != list.at(0).at(2)) {
        reConstruction.clear();
        return false;
    }

    int num = list.at(1).split(":").length();
    if ((num != 3) && (num != 4)) {
        reConstruction.clear();
        return false;
    }

    temp = list.at(2);
    if ("/" != temp.at(1)) {
        reConstruction.clear();
        return false;
    }
    if (!QString("VDIWEA").contains(temp.at(0))) {
        reConstruction.clear();
        return false;
    }

    if (!temp.remove(0,2).contains("(")) {
        reConstruction.clear();
        return false;
    }

    return true;
}


/*
 * 函数名称：    textToList(QString str)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    解析日志文件的一行，提取时间、Tag、打印等级和日志内容等信息
 * 输入参数：    str:日志文件的一行
 * 输出参数：    无
 * 返回值：      返回解析后提取的信息
*/
QStringList MainWindow::textToList(QStringList str)
{
    QString header = str.at(0);
    QString content = str.at(1);
    
    QStringList list;
    list.append(header.mid(0,18));
    list.append(header.at(19));
    int index = header.indexOf("(");
    list.append(header.mid(index + 1).remove("):"));
    list.append(header.mid(21,index - 21));
    list.append(content);

    return list;
}


/*
 * 函数名称：    addItemToDataList(QString list)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    对日志文件的一行进行解析，并将结果添加到数据表中
 * 输入参数：    list:日志文件一行解析后的数据结构
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::addItemToDataList(QStringList list)
{
    if (list.length() != 5) {
        QMessageBox::information(this,"警告","添加项到数据表中出错，原因：解析数据后大小不为5！");
        return;
    }

    mAllTimes.append(list.at(0));
    mAllLevels.append(list.at(1));
    mAllPIDs.append(list.at(2));
    mAllTags.append(list.at(3));
    mAllTexts.append(list.at(4));
}


/*
 * 函数名称：    clearAllDataList()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    初始化变量
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::clearAllDataList()
{
    lwFilter->reset();
    lwFilter->clear();

    mAllTimes.clear();
    mAllLevels.clear();
    mAllPIDs.clear();
    mAllTags.clear();
    mAllTexts.clear();
    mFilters.clear();
    mAllLines = 0;

    clearCurDataList();
}


/*
 * 函数名称：    clearCurDataList()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    清空当前数据表
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::clearCurDataList()
{
    mCurTimes.clear();
    mCurLevels.clear();
    mCurPIDs.clear();
    mCurTags.clear();
    mCurTexts.clear();
    mCurLines = 0;
}


/*
 * 函数名称：    loadLevelMessage(QString logLevel, QString logTag)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    加载大于等于某一日志等级且指定Tag的所有日志信息
 * 输入参数：    logLevel:日志等级   logTag:Tag
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::loadLevelMessage(QString logLevel, QString logTag)
{
    int curLevel = getLevelIndex(logLevel);
    int length = mAllLines;
    clearCurDataList(); //先清空当前数据表
    
    if (logTag== DEFAULT_TAG) {
        for (int i = 0; i < length; i++) {
            if (getLevelIndex(mAllLevels.at(i)) >= curLevel) {
                mCurTimes.append(mAllTimes.at(i));
                mCurLevels.append(mAllLevels.at(i));
                mCurPIDs.append(mAllPIDs.at(i));
                mCurTags.append(mAllTags.at(i));
                mCurTexts.append(mAllTexts.at(i));
            }
        }
    } else {
        for (int i = 0; i < length; i++) {
            if (logTag == mAllTags.at(i)) {
                if (getLevelIndex(mAllLevels.at(i)) >= curLevel) {
                    mCurTimes.append(mAllTimes.at(i));
                    mCurLevels.append(mAllLevels.at(i));
                    mCurPIDs.append(mAllPIDs.at(i));
                    mCurTags.append(mAllTags.at(i));
                    mCurTexts.append(mAllTexts.at(i));
                }
            }
        }
    }
}


/*
 * 函数名称：    filterTextHead()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    将日志信息的定位信息头过滤掉（如果含有）
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回去掉定位信息头后的内容
*/
QString MainWindow::filterTextHead(QString text)
{
    //定位信息头的格式为:"[ (xxx.java:xxx)#xxx ] "
    if (isContainHead(text)) {
        QString head = text.split(" ] ")[0] + " ] ";
        if (head != text) {
            return text.remove(head);
        } else {
            return text;
        }
    } else {
        return text;
    }
}


/*
 * 函数名称：    isContainHead()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    判断日志信息内容是否含有定位信息头
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      包含返回true，否则返回false
*/
bool MainWindow::isContainHead(QString text)
{
    //定位信息头的格式为:"[ (xxx.java:xxx)#xxx ] "
    if (!text.startsWith("[ (")) {
        return false;
    }

    if (text.contains(" ] ")) {
        QString tempStr = text.split(" ] ")[0].split("[ ")[1];
        if (tempStr.contains(".java:") && tempStr.contains(")#")) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


/*
 * 函数名称：    isContainHead()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    是否建立时间过滤器的信号与槽的连接
 * 输入参数：    isConnect:是否建立连接
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::isConnectTimeFilter(bool isConnect)
{
    if (isConnect) {
        QObject::connect(ui->cbStartMonth,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::connect(ui->cbStartDay,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::connect(ui->cbStartHour,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::connect(ui->cbStartMinute,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::connect(ui->cbStartSecond,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));

        QObject::connect(ui->cbStopMonth,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::connect(ui->cbStopDay,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::connect(ui->cbStopHour,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::connect(ui->cbStopMinute,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::connect(ui->cbStopSecond,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
    } else {
        QObject::disconnect(ui->cbStartMonth,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::disconnect(ui->cbStartDay,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::disconnect(ui->cbStartHour,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::disconnect(ui->cbStartMinute,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::disconnect(ui->cbStartSecond,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));

        QObject::disconnect(ui->cbStopMonth,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::disconnect(ui->cbStopDay,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::disconnect(ui->cbStopHour,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::disconnect(ui->cbStopMinute,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
        QObject::disconnect(ui->cbStopSecond,SIGNAL(currentIndexChanged(int)),this,SLOT(timeFilterChangedSlot()));
    }
}


/*
 * 函数名称：    isConnectScroll(bool isConnect)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.10
 * 函数功能：    是否建立内容显示区域滚动条的信号与槽的连接
 * 输入参数：    isConnect:连接或断开
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::isConnectScroll(bool isConnect)
{
    if (isConnect) {
        QObject::connect(ui->lwContent->verticalScrollBar(),SIGNAL(valueChanged(int)),
                         this,SLOT(selfVerticalScrollSlot(int)));
        
        QObject::connect(ui->verticalScrollBar,SIGNAL(valueChanged(int)),
                         this,SLOT(verticalScrollSlot(int)));
    } else {
        QObject::disconnect(ui->lwContent->verticalScrollBar(),SIGNAL(valueChanged(int)),
                         this,SLOT(selfVerticalScrollSlot(int)));
        
        QObject::disconnect(ui->verticalScrollBar,SIGNAL(valueChanged(int)),
                         this,SLOT(verticalScrollSlot(int)));
    }
}


/*
 * 函数名称：    dispCurDataList()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.10
 * 函数功能：    显示当前的数据表
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::dispCurDataList()
{
    //计算滚动条的最大值
    int max = mCurRange->getTotal() - mCurRange->getPageItemNum() + 1;
    //如果该值大于0则显示滚动条，否则隐藏
    if (max > 0) {
        ui->verticalScrollBar->setHidden(false);
        ui->verticalScrollBar->setRange(0,max);
    } else {
        ui->verticalScrollBar->setHidden(true);
        ui->verticalScrollBar->setRange(0,0);
    }
    
    autoAdjustTitleLabel();     //自动调节标题标签的位置
    
    dispAreaData(mCurRange,0);  //将显示缓冲区的数据显示到屏幕
}


/*
 * 函数名称：    dispAreaData(ItemsRange *range, int direction)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.10
 * 函数功能：    将显示缓冲区中的数据显示到屏幕（所显示的内容由显示缓冲区范围决定）
 * 输入参数：    range:显示缓冲区范围   direction:滚动条滚动方向
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::dispAreaData(ItemsRange *range, int direction)
{
    isConnectScroll(false);         //必须先断开滚动条的信号与槽的连接，否则将造成很多不必要的计算，浪费时间
    lwContent->clear();             //清空屏幕显示

    //获取当前数据表中的数据量，从而确定行号显示栏的位数    
    int bitNum = QString::number(mCurLines).length();
    if (bitNum < 2) {
        bitNum = 2;
    }
    
    //创建颜色表，以便区分不同的打印等级
    QString colorStr = "#0843CE,#007BB9,#007F00,#FF7F00,#FF0000,#A50000,#FFFFFF";
    QStringList corlorList = colorStr.split(",");
    QList<QColor> *colorConfig = new QList<QColor>;
    for (int i = 0; i < 7; ++i) {
        colorConfig->append(QColor(corlorList.at(i)));
    }
    
    int first = range->getFirst();
    int last = range->getLast();
    logCurTime("显示范围：" + QString::number(first) + " ," 
               + QString::number(range->getVisibleFirst()) + " ," 
               + QString::number(last));
    
    QString tempText;
    QColor color;
    //根据缓冲区的范围显示对应的数据
    for (int i = first; i <= last; ++i) {
        tempText = intToString(i + 1, bitNum) + "  "
                + ((isDispTime == true) ? (mCurTimes.at(i) + "  ") : "")
                + ((isDispLevel == true) ? (mCurLevels.at(i) + "  ") : "")
                + ((isDispPID == true) ? (mCurPIDs.at(i) + "  ") : "")
                + ((isDispTag == true) ? (mCurTags.at(i) + "    ") : "")
                + ((isDispHead == true) ? (mCurTexts.at(i)) : filterTextHead(mCurTexts.at(i)));
        lwContent->addItem(tempText);

        color = colorConfig->at(getLevelIndex(mCurLevels.at(i)));

        //将对应日志等级的文本设置成对应的颜色
        lwContent->item(i - first)->setTextColor(color);
    }
    
    if (lwContent->count() > 0) {
        //如果是向下滑动，则将焦点设置在最后一行；否则，将焦点设置在第一行
        if (direction == 1) {
            lwContent->scrollToTop();
            lwContent->setCurrentRow(range->getVisibleFirst() + range->getPageItemNum() - 2 - first);
        } else {
            lwContent->scrollToBottom();
            lwContent->setCurrentRow(range->getVisibleFirst() - first);
        }
        
        //重新设定滚动条的位置
        ui->verticalScrollBar->setValue(range->getVisibleFirst());
    }
    
    isConnectScroll(true);  //重新连接滚动条的信号与槽的连接
}


/*
 * 函数名称：    isValidItemsRange(ItemsRange *range)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.10
 * 函数功能：    判断是否为一个有效的缓冲区范围
 * 输入参数：    range:显示缓冲区范围
 * 输出参数：    无
 * 返回值：      有效返回true，否则返回false
*/
bool MainWindow::isValidItemsRange(ItemsRange *range)
{
    return (range->getLast() < mCurLines) && (range->getCount() > 0);
}


/*
 * 函数名称：    isDirExist(QString fullPath, bool isCreate)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.13
 * 函数功能：    判断一个路径是否存在，若不存在，可选择是否创建该路径
 * 输入参数：    fullPath:路径     isCreate:若不存在，是否创建
 * 输出参数：    无
 * 返回值：      路径存在返回true，否则返回false
*/
bool MainWindow::isDirExist(QString fullPath, bool isCreate)
{
    QString path;
    QStringList temp = fullPath.split("/");
    if (temp.last().contains(".")) {
        path = fullPath.mid(0,fullPath.length() - temp.last().length() - 1);
    } else {
        path = fullPath;
    }
    
    QDir dir(path);
    if (dir.exists()) {
        return true;
    } else if(isCreate) {
        return dir.mkpath(path);
    } else {
        return false;
    }
}


/*
 * 函数名称：    mainThreadWait_ms(int ms)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.15
 * 函数功能：    使主线程阻塞一定的时间，目的是为了让所设置的UI界面得到及时的刷新和显示
 * 输入参数：    ms:要阻塞的毫秒数
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::mainThreadWait_ms(int ms)
{
    eventLoopTimer->start(ms);
    mEventLoop->exec();
}


/*
 * 函数名称：    addHistoryAction(QString path)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.16
 * 函数功能：    添加打开文件历史记录
 * 输入参数：    path:文件的绝对路径
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::addHistoryAction(QString path)
{
    QList<QAction*> list = ui->history->actions();
    QMenu *history = ui->history;
    int size = list.size();
    
    //清空当前的所有Action，后面重新添加
    for (int i = 0; i < size; ++i) {
        history->removeAction(list.at(i));
    }
    
    //如果数量大于1，且倒数第二个为分隔符
    if ((size > 1) && (list.at(size - 2)->text().isEmpty())) {
        //如果数量大于等于了最大数量，则删除最旧的一个
        if ((MAX_SAVE_HISTORY_NUM > 0) && (size >= (2 + MAX_SAVE_HISTORY_NUM))) {
            list.removeAt(size - 3);
            size--;
        }
    }
    
    //添加最新的
    history->addAction(path);
    //添加之前的
    QString text = "";
    for (int i = 0; i < (size - 1); ++i) {
        text = list.at(i)->text();
        //如果不是分隔符，且不是当前打开的文件
        if (!text.isEmpty() && (text != path)) {
            history->addAction(text);
        }
    }
    //添加分隔符，之前的分隔符已经被去掉了
    history->addSeparator();
    //添加最后一个（即清空历史记录）
    history->addAction(list.at(size - 1)->text());
    
    
    mHistory.clear();
    list = history->actions();
    size = list.size() - 2;
    for (int i = 0; i < size; ++i) {
        text = list.at(i)->text();
        logCurTime("history[" + QString::number(i) + "]=" + text);
        mHistory.append(text);
    }
    updateHistoryAction(mHistory);
    
    history->setEnabled(true);  //使能该选项（因为已经有历史记录存在了）
}


/*
 * 函数名称：    updateHistoryAction(QStringList history)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    更新文件打开的历史记录
 * 输入参数：    history:历史记录
 * 输出参数：    无
 * 返回值：      无
*/
void MainWindow::updateHistoryAction(QStringList history)
{
    QList<QAction*> list = ui->history->actions();
    QMenu *action = ui->history;
    int size = list.size();
    
    //清空当前的所有Action，后面重新添加
    for (int i = 0; i < size; ++i) {
        action->removeAction(list.at(i));
    }
    
    size = history.size();
    for (int i = 0; i < size; ++i) {
        action->addAction(history.at(i));
    }
    //添加分隔符
    action->addSeparator();
    //添加最后一个（即清空历史记录）
    action->addAction("清空历史记录");
    
    //将新的设置更新到设置文件中
    bool isSaveSuccess = mSettingParse->saveHistoryToFile(mSettingFilePath,mHistory);
    if (isSaveSuccess) {
        logCurTime("保存历史记录成功！");
    } else {
        logCurTime("保存历史记录失败！");
    }
}


/*
 * 函数名称：    isContainString(QString str, QString containStr)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    判断在一个字符串中是否包含某一字符串
 * 输入参数：    str:要判断的字符串     containStr:是否包含的字符串
 * 输出参数：    无
 * 返回值：      包含返回true，否则返回false
*/
bool MainWindow::isContainString(QString str, QString containStr)
{
    return str.contains(containStr,Qt::CaseInsensitive);
}


/*
 * 函数名称：    isContainTime()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.9
 * 函数功能：    判断一个时间是否属于起止时间范围内
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      属于返回true，否则返回false
*/
bool MainWindow::isContainTime(QString time)
{
    int result1 = QString::compare(time, mStartTime);
    int result2 = QString::compare(time, mStopTime);
    return (result1 >= 0) && (result2 <= 0);
}


/*
 * 函数名称：    getLevelIndex(QString logLevel)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.4
 * 函数功能：    根据日志等级获取等级的索引值
 * 输入参数：    logLevel:日志等级
 * 输出参数：    无
 * 返回值：      等级的索引值
*/
int MainWindow::getLevelIndex(QString logLevel)
{
    int levelIndex = 0;
    if ((logLevel == "V") || (logLevel == "verbose")) {
        levelIndex = 0;
    } else if (logLevel == "D" || (logLevel == "debug")) {
        levelIndex = 1;
    } else if (logLevel == "I" || (logLevel == "info")) {
        levelIndex = 2;
    } else if (logLevel == "W" || (logLevel == "warn")) {
        levelIndex = 3;
    } else if (logLevel == "E" || (logLevel == "error")) {
        levelIndex = 4;
    } else if (logLevel == "A" || (logLevel == "assert")) {
        levelIndex = 5;
    }

    return levelIndex;
}




