#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QIODevice>
#include <QDateTime>
#include <QTimer>
#include <QIcon>
#include <QTextBrowser>
#include <QScrollBar>
#include <QStringList>
#include <QColor>
#include <QLabel>
#include <QTextCodec>
#include <QByteArray>
#include <QProcess>
#include <QAction>
#include <QProgressDialog>
#include <QEventLoop>
#include <QCloseEvent>
#include <QThread>
#include "setting.h"
#include "aboutsoftware.h"
#include "connectadb.h"
#include "itemsrange.h"
#include "adbthread.h"
#include "qwaitdialog.h"
#include "parsesettingfile.h"
#include "register.h"




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    //自定义函数
    void closeEvent(QCloseEvent *event);//应用程序右上角X关闭触发事件
    void initEnvironment();              //初始化软件运行环境
    bool checkLogCatFile(QString);      //检查是否为Android的日志文件
    QStringList textToList(QStringList);//将日志一行的内容提取为时间、打印等级、TAG、打印信息，并存储到列表中
    void addItemToDataList(QStringList);//添加一项到全局数据表中
    void clearAllDataList();            //数据表初始化
    void clearCurDataList();            //清空当前数据表
    void loadLevelMessage(QString logLevel, QString logTag);    //加载满足给定打印等级和Tag要求的日志信息
    int getLevelIndex(QString logLevel);//将打印等级字符串转换为int型
    void dispCurDataList();             //显示当前数据表
    bool isContainString(QString str, QString containStr);      //判断一个字符串中是否包含另一字符串（大小写不敏感）
    bool isContainTime(QString time);   //判断一个时间是否属于起止时间范围内
    void dispContainSearchString(QString str);      //刷新显示内容
    void autoAdjustTitleLabel();        //自动调整标签栏的位置
    QString intToString(int num, int bitNum);       //将int型数字转换为固定位数的QString型
    void loadFile(QString path);        //加载一个日志文件
    void updateFile(QString path);      //更新一个日志文件的内容
    void updateTimeStringToWidget(QString startTime, QString stopTime); //将起止时间更新到控件显示
    bool isTimeString(QString str);     //判断一个字符串是否为时间字符串
    QString filterTextHead(QString text);//将日志信息的定位信息头过滤掉（如果含有）
    bool isContainHead(QString text);   //判断日志信息内容是否含有定位信息头
    void isConnectTimeFilter(bool isConnect);   //是否建立时间过滤器的信号与槽的连接
    void isConnectScroll(bool isConnect);       //是否建立内容显示区域滚动条的信号与槽的连接
    QStringList execWindowsCmd(QString adbPath,QString cmd);    //执行CMD命令
    bool isADBConnectedSuccess(QString deviceName);             //判断某个ADB设备是否连接成功
    QStringList getCurOnlineDeviceNames();                      //获取当前在线的ADB设备
    bool isExistDevice(QString deviceName);                     //判断某个ADB设备名是否存在
    void clearLogCacheFile();           //清空日志缓存文件
    void logCurTime(QString text);      //调试打印（带时间戳）
    void updateCurRange();              //更新当前显示缓冲区的范围
    void dispAreaData(ItemsRange *range,int direction);         //将显示缓冲区中的数据显示到屏幕
    bool isValidItemsRange(ItemsRange *range);                  //判断是否为一个有效的缓冲区范围
    bool isDirExist(QString fullPath,bool isCreate);   //判断一个路径是否存在，若不存在，可选择是否创建该路径
    void mainThreadWait_ms(int ms);     //使主线程阻塞一定的时间，目的是为了让所设置的UI界面得到及时的刷新和显示
    void addHistoryAction(QString path);//添加打开文件历史记录
    void updateHistoryAction(QStringList history);  //更新文件打开的历史记录
    void loadSoftwareSetting(QString path);         //加载软件的设置文件
    

    
    //动作函数
    void setingAction();        //菜单栏首选项设置动作
    void openFileAction();      //菜单栏打开文件动作
    void closeFileAction();     //菜单栏关闭文件动作
    void aboutAction();         //菜单栏关于软件动作
    void refreshLogFileAction();//重载刷新当前日志文件动作
    void clickTerminalAction(); //终端模式开关动作
    void connectADBAction();    //连接或断开ADB设备动作
    void gotoLineAction();      //跳转到行动作
    void registerSolftware();   //注册软件动作

private slots:
    //槽函数
    void trigerMenuSlot(QAction* action);   //菜单栏触发槽函数
    void trigerToolSlot(QAction* action);   //工具栏触发槽函数
    void trigerHistorySlot(QAction* action);//文件打开历史记录触发槽函数
    void logLevelChangedSlot();             //打印等级选择框内容变化槽函数
    void filterChangedSlot();               //Tag过滤器变化槽函数
    void searchEditChangedSlot();           //搜索框内容变化槽函数
    void swTimeFilterChangedSlot();         //时间过滤开关变化槽函数
    void timeFilterChangedSlot();           //时间过滤选择框变化槽函数
    void swMessageFilterChangedSlot();      //信息过滤开关变化槽函数
    void fileTimeoutSlot();                 //日志缓存文件大小监控定时器槽函数
    void statusTimeoutSlot();               //ADB设备连接状态监控定时器槽函数
    void eventLoopTimeoutSlot();            //主线程的事件循环控制定时器槽函数
    void selfVerticalScrollSlot(int value); //QListWidget自带的垂直滚动条值变化槽函数
    void verticalScrollSlot(int value);     //自定义的垂直滚动条值变化槽函数
    void threadDoneSlot();                  //执行ADB命令线程完毕槽函数
    void showDialogSlot();                  //显示等待对话框槽函数
    void closeDialogSlot();                 //关闭等待对话框槽函数
    void gotoLineEditTextChangedSLot(QString text);     //跳转到行输入编辑框当前文本发生变化触发槽函数
    void gotoLineSlot();                    //跳转到指定行槽函数

private:
    Ui::MainWindow *ui;

    QLabel *fileLabel;          //软件底部状态栏标签
    QLabel *adbDeviceLabel;     //ADB设备状态标签
    QLabel *workModeLabel;      //软件工作模式标签（离线查看模式、实时终端模式）

    QListWidget *lwFilter;      //Tag过滤器列表
    QLabel *labelLine;          //行标签
    QLabel *labelTime;          //时间标签
    QLabel *labelLevel;         //打印等级标签
    QLabel *labelPID;           //进程PID标签
    QLabel *labelTag;           //Tag标签
    QLabel *labelText;          //文本内容标签
    QListWidget *lwContent;     //日志内容列表

    QStringList mAllTimes;      //时间全局数据表
    QStringList mAllLevels;     //打印等级全局数据表
    QStringList mAllPIDs;       //进程PID全局数据表
    QStringList mAllTags;       //Tag全局数据表
    QStringList mAllTexts;      //文本内容全局数据表

    QStringList mCurTimes;      //时间当前数据表
    QStringList mCurLevels;     //打印等级当前数据表
    QStringList mCurPIDs;       //进程PID当前数据表
    QStringList mCurTags;       //Tag当前数据表
    QStringList mCurTexts;      //文本内容当前数据表

    QStringList mFilters;       //当前日志文件中的所有Tag种类

    QString mStartTime;         //时间过滤器的起始时间
    QString mStopTime;          //时间过滤器的终止时间

    bool isDispTime;            //是否显示时间信息
    bool isDispLevel;           //是否显示打印等级信息
    bool isDispPID;             //是否显示进程PID信息
    bool isDispTag;             //是否显示Tag信息
    bool isDispHead;            //是否显示定位头信息

    QString mCurFontType;       //当前的字体类型
    int mCurFontSize;           //当前的字体大小
    QString mCurCodeType;       //当前的编码类型

    QString ADBPath;            //ADB工具路径

    QProcess *cmdProcess;       //CMD进程

    QTimer *fileTimer;          //日志缓存文件监控定时器
    QTimer *statusTimer;        //ADB设备连接状态监控定时器
    QTimer *eventLoopTimer;     //主线程的事件循环控制定时器

    int logFileSize;            //日志缓存文件的大小，单位：字节
    QString mCurLogFilePath;    //当前加载的日志文件路径
    QString mLogCacheFilePath;  //日志缓存文件路径，与程序运行的目录相同
    QString mSettingFilePath;   //软件设置相关的记录文件路径
    
    QString mCurADBDevice;      //当前ADB设备名
    bool mCurADBDeviceStatus;   //当前ADB设备的连接状态
    bool isTerminalMode;        //软件的当前工作模式（离线查看模式、实时终端模式）
    
    QStringList reConstruction; //日志内容一行数据解析重构后的临时存储区域
    int mLastScrollValue;       //滚动条上一次的值
    
    ItemsRange *mCurRange;      //当前显示缓冲区的范围
    
    bool isCheckedUpdateFile;   //通过更新日志文件内容的方式显示时，是否检查了内容的规范性
    
    QWaitDialog *mWaitDialog;   //等待对话框
    QEventLoop *mEventLoop;     //时间循环控制器，用于阻塞主线程，但仍可响应事件
    int mReConnecCnt;           //ADB重连次数计数器
    
    bool isLoadingFile;         //是否正在加载文件
    bool isScaningADBDevice;    //是否正在扫描ADB设备
    QString mGotoLine;          //跳转到行窗口中输入框中当前输入的内容
    int mCurLines;              //当前日志显示区的总行数；
    int mAllLines;              //当前打开的日志文件内容的总行数
    QDialog *mCurOptDialog;     //当前操作的对话框
    
    ParseSettingFile *mSettingParse;   //软件配置文件解析器
    QStringList mHistory;       //文件打开的历史记录
    QString mRegisterNum;       //软件注册码
};

#endif // MAINWINDOW_H
