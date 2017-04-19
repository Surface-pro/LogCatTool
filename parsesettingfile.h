#ifndef PARSESETTINGFILE_H
#define PARSESETTINGFILE_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QTextCodec>
#include "security.h"


//配置文件中配置参数格式的头定义
#define     FONT_TYPE           "<FontType="        //字体类型
#define     FONT_SIZE           "<FontSize="        //字体大小
#define     CODE_TYPE           "<CodeType="        //编码类型
#define     ADB_PATH            "<ADBPath="         //ADB路径
#define     REGISTER_NUM        "<RegisterNumber="  //软件注册码
#define     HISTORY_PATH        "<HistoryPath="     //文件打开历史记录



class ParseSettingFile : public QObject
{
    Q_OBJECT
public:
    explicit ParseSettingFile(QObject *parent = 0,QString path = "");
    
signals:
    
public slots:
    
public:
    QString getFontType();                  //获取字体类型
    int getFontSize();                      //获取字体大小
    QString getCodeType();                  //获取文本编码类型
    QString getADBPath();                   //获取ADB路径
    QString getRegisterNum();               //获取软件注册码
    QStringList getHistory();               //获取文件打开的历史记录
    bool saveParamsToFile(QString path,QStringList paramList);  //保存参数到文件中
    bool saveHistoryToFile(QString path,QStringList history);   //保存文件打开历史记录到文件中
    bool saveRegisterNumToFile(QString path,QString num);       //保存软件注册码包文件中
    QString createParamsItem(QString key,QString value);        //创建一个参数项（其格式为：键$@$值）
    
private:
    void initEnvironment();                 //初始化解析器运行环境
    bool checkFile(QString path);           //检查文件是否为设置文件
    void loadDefaultConfig();               //加载默认的配置
    QStringList createCurParams();          //创建当前的配置参数表
    void parseFile(QString path);           //解析配置文件
    bool stringIsNum(QString str);          //判断一个字符串是否可以转换为数字
    QString getKey(QString params);         //从一个参数项中获取键
    QString getValue(QString params);       //从一个参数项中获取值
    QString getValueByKey(QStringList paramList,QString key); //从参数集合中查找指定键对应的值
    bool isContainKey(QStringList paramList,QString key);     //判断某个参数集合中是否包含一个键
    bool isDirExist(QString fullPath,bool isCreate);          //判断一个路径是否存在，若不存在，可选择是否创建该路径
    //重新构建指定键值在配置文件中的字符串，如"<FontType=Courier New>"
    QString updateNewValue(QStringList paramList,QString key,QStringList &isSaveList);
    
private:
    QStringList mDefaultParams;     //默认参数表
    int mParamsNum;                 //当前系统支持的参数数目
    QStringList mHistoryList;       //文件打开历史记录
    
    QString fontType;               //字体类型
    int fontSize;                   //字体大小
    QString codeType;               //编码类型
    QString adbPath;                //ADB路径
    QString registerNum;            //软件注册码
    
};

#endif // PARSESETTINGFILE_H
