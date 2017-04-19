#include "parsesettingfile.h"


#define     PARAMS_SEPARATOR    "$@$"               //键值分隔符

#define     SYSTEM_CODE_TYPE    "UTF-8"             //读写配置文件的编码格式，统一为UTF-8


/*
 *  配置文件格式：
 *      <setting-config>
 *      <start> 
 *      <FontType=Courier New>
 *      <FontSize=14>
 *      <CodeType=UTF-8>
 *      <ADBPath=>
 *      <end>
 * 
 * 【注意事项】：
 *      1，每一行必须顶格写入；
 *      2，相应参数若无数据则为空即可，比如上面的ADBPath项；
 *      3，文件中必须含有文件头标志、起始标志、结束标志；
 *      4，文件其他部分若有错误数据，或者有其他注释语句等，均不会干扰对配置参数的解析，
 *          因为解析过程会自动筛选屏蔽不符合参数规则的数据；
*/
//配置文件标志定义
#define     FILE_HEADER         "<setting-config>"  //配置文件头标志
#define     FILE_START          "<start>"           //配置文件开始标志
#define     FILE_END            "<end>"             //配置文件结束标志




ParseSettingFile::ParseSettingFile(QObject *parent,QString path) :
    QObject(parent)
{
    initEnvironment();      //初始化解析器运行环境
    loadDefaultConfig();    //先加载默认的配置
    
    //检查该文件是否为配置文件，若是，则解析文件
    if (checkFile(path)) {
        parseFile(path);        //解析文件
    }
    
    //将最新的配置参数更新到配置文件中
    saveParamsToFile(path,createCurParams());
}


/*
 * 函数名称：    initEnvironment()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    初始化解析器运行环境
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void ParseSettingFile::initEnvironment()
{
    mHistoryList.clear();
    mDefaultParams.clear();
    
    mDefaultParams.append(createParamsItem(FONT_TYPE,"Courier New"));
    mDefaultParams.append(createParamsItem(FONT_SIZE,"14"));
    mDefaultParams.append(createParamsItem(CODE_TYPE,"UTF-8"));
    mDefaultParams.append(createParamsItem(ADB_PATH,""));
    mDefaultParams.append(createParamsItem(REGISTER_NUM,""));
    
    mParamsNum = mDefaultParams.size();
}


/*
 * 函数名称：    loadDefaultConfig()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    加载默认的配置
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      无
*/
void ParseSettingFile::loadDefaultConfig()
{
    QString num;
    fontType = getValueByKey(mDefaultParams,FONT_TYPE);
    num = getValueByKey(mDefaultParams,FONT_SIZE);
    fontSize = (stringIsNum(num)) ? num.toInt() : 14;
    codeType = getValueByKey(mDefaultParams,CODE_TYPE);
    adbPath = getValueByKey(mDefaultParams,ADB_PATH);
    mHistoryList.clear();
}


/*
 * 函数名称：    createCurParams()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    创建当前的配置参数表
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回创建好的参数表
*/
QStringList ParseSettingFile::createCurParams()
{
    QStringList paramList;  //创建存储参数表
    paramList.append(createParamsItem(FONT_TYPE,fontType));
    paramList.append(createParamsItem(FONT_SIZE,QString::number(fontSize)));
    paramList.append(createParamsItem(CODE_TYPE,codeType));
    paramList.append(createParamsItem(ADB_PATH,adbPath));
    
    return paramList;
}


/*
 * 函数名称：    getFontType()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    获取字体类型
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回字体类型
*/
QString ParseSettingFile::getFontType()
{
    return fontType;
}


/*
 * 函数名称：    getFontSize()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    获取字体大小
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回字体大小
*/
int ParseSettingFile::getFontSize()
{
    return fontSize;
}


/*
 * 函数名称：    getCodeType()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    获取文本编码类型
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回文本编码类型
*/
QString ParseSettingFile::getCodeType()
{
    return codeType;
}


/*
 * 函数名称：    getADBPath()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    获取ADB路径
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回ADB路径
*/
QString ParseSettingFile::getADBPath()
{
    return adbPath;
}


/*
 * 函数名称：    getRegisterNum()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    获取软件注册码
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回软件注册码
*/
QString ParseSettingFile::getRegisterNum()
{
    return registerNum;
}


/*
 * 函数名称：    getHistory()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    获取文件打开的历史记录
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回文件打开的历史记录
*/
QStringList ParseSettingFile::getHistory()
{
    return mHistoryList;
}


/*
 * 函数名称：    saveParamsToFile(QString path, QStringList paramList)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    保存参数到文件中
 * 输入参数：    path:要保存为的文件路径       paramList:要保存的参数表
 * 输出参数：    无
 * 返回值：      保存成功返回true，否恩泽返回false
*/
bool ParseSettingFile::saveParamsToFile(QString path, QStringList paramList)
{
    //判断该文件的父路径是否存在，若不存在则创建
    if (!isDirExist(path, true)) {
        return false;
    }
    
    bool isCreateNewFile = false;
    QString allContent = "";
    QStringList list;
    QFile file(path);
    //如果已经存在该文件
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //如果该文件是配置文件
        if (checkFile(path)) {
            QTextStream in(&file);
            in.setCodec(QTextCodec::codecForName(QString(SYSTEM_CODE_TYPE).toLatin1()));
            allContent = in.readAll();
        } else {
            allContent = "";
        }
        
        file.close();
    }
    
    //尝试3次打开该文件，若均失败则存储参数失败（该方式打开文件时，若文件不存在会主动创建）
    int cnt = 3;
    while (cnt--) {
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            break;
        }
    }
    
    //若未空，则表示该文件为新建的
    if (allContent == "") {
        isCreateNewFile = true;
    }
    
    //cnt大于0，表示成功打开了该文件
    if(cnt > 0)
    {
        //创建并初始化参数是否更新记录表
        int num = paramList.size();
        QStringList isSaveList;
        for (int i = 0; i < num; ++i) {
            isSaveList.append("0");
        }
        
        QTextStream out(&file);
        out.setCodec(QTextCodec::codecForName(QString(SYSTEM_CODE_TYPE).toLatin1()));
        list = allContent.split("\n");
        int lines = list.size();
        int length;
        QString tempKey;
        QString tempStr;
        QString newStr;
        bool isStart = false;
        bool isEnd = false;
        int step = 0;
        //开始更新原先已有的参数
        for (int i = 0; i < lines; ++i) {
            tempStr = list.at(i);
            //过滤掉空白行
            if (tempStr.isEmpty()) {
                continue;
            }
            
            //若已识别到参数起始标志
            if (isStart) {
                if (!tempStr.startsWith(HISTORY_PATH)) {
                    for (int i = 0; i < mParamsNum; ++i) {
                        tempKey = getKey(mDefaultParams.at(i));
                        if (tempStr.startsWith(tempKey)) {
                            newStr = updateNewValue(paramList,tempKey,isSaveList);
                            if (!newStr.isEmpty()) {
                                length = tempStr.indexOf('>') + 1;
                                tempStr = newStr + tempStr.mid(length,tempStr.length() - length);
                            }
                            break;
                        }
                    }
                }
                
                if (!tempStr.startsWith(FILE_END)) {
                    out<<tempStr + "\n";
                }
            }
            
            //配置文件标志识别
            switch (step) {
            case 0:
                if (tempStr.startsWith(FILE_HEADER)) {
                    step = 1;
                    out<<FILE_HEADER<<tempStr.remove(FILE_HEADER)<<"\n";
                }
                break;
            case 1:
                if (tempStr.startsWith(FILE_START)) {
                    step = 2;
                    isStart = true;
                    out<<FILE_START<<tempStr.remove(FILE_START)<<"\n";
                } else {
                    step = 0;
                }
                break;
            case 2:
                if (tempStr.startsWith(FILE_END)) {
                    isEnd = true;
                }
                break;
            default:
                break;
            }
            
            //若识别到了文件结束标志，则跳出
            if (isEnd) {
                break;
            }
        }
        
        //如果未识别到起始标志，且该文件不是新建的，则表示保存参数失败
        if (!isStart && !isCreateNewFile) {
            return false;
        }
        
        //如果是新建的文件，则写入配置文件的头和起始标志
        if (isCreateNewFile) {
            out<<FILE_HEADER<<"\n";
            out<<FILE_START<<"\n";
        }
        
        //写入原先没有的参数
        for (int i = 0; i < num; ++i) {
            //qDebug()<<"isSaveList[" + QString::number(i) + "]=" + isSaveList.at(i);
            if (isSaveList.at(i) == "0") {
                newStr = paramList.at(i);
                tempStr = getKey(newStr) + getValue(newStr) + ">";
                out<<tempStr + "\n";
            }
        }
        out<<FILE_END;      //写入文件结束标志
        
        return true;
    } else {
        return false;
    }
    
}


/*
 * 函数名称：    saveParamsToFile(QString path, QStringList paramList)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    保存文件打开历史记录到文件中
 * 输入参数：    path:要保存为的文件路径       history:要保存历史记录
 * 输出参数：    无
 * 返回值：      保存成功返回true，否恩泽返回false
*/
bool ParseSettingFile::saveHistoryToFile(QString path, QStringList history)
{
    //判断该文件的父路径是否存在，若不存在则创建
    if (!isDirExist(path, true)) {
        return false;
    }
    
    bool isCreateNewFile = false;
    QString allContent = "";
    QStringList list;
    QFile file(path);
    //如果已经存在该文件
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //如果该文件是配置文件
        if (checkFile(path)) {
            QTextStream in(&file);
            in.setCodec(QTextCodec::codecForName(QString(SYSTEM_CODE_TYPE).toLatin1()));
            allContent = in.readAll();
        } else {
            allContent = "";
        }
        
        file.close();
    }
    
    //尝试3次打开该文件，若均失败则存储参数失败（该方式打开文件时，若文件不存在会主动创建）
    int cnt = 3;
    while (cnt--) {
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            break;
        }
    }
    
    //若未空，则表示该文件为新建的
    if (allContent == "") {
        isCreateNewFile = true;
    }
    
    //cnt大于0，表示成功打开了该文件
    if(cnt > 0)
    {
        //创建并初始化参数是否更新记录表
        int num = history.size();
        QStringList isSaveList;
        for (int i = 0; i < num; ++i) {
            isSaveList.append("0");
        }
        
        QTextStream out(&file);
        out.setCodec(QTextCodec::codecForName(QString(SYSTEM_CODE_TYPE).toLatin1()));
        list = allContent.split("\n");
        int lines = list.size();
        QString tempStr;
        for (int i = 0; i < lines; ++i) {
            tempStr = list.at(i);
            //过滤掉空白行
            if (tempStr.isEmpty()) {
                continue;
            }
            
            if (tempStr.startsWith(HISTORY_PATH)) {
                continue;
            }
            
            if (!tempStr.startsWith(FILE_END)) {
                out<<tempStr + "\n";
            }
        }
        
        //如果是新建的文件，则写入配置文件的头和起始标志
        if (isCreateNewFile) {
            out<<FILE_HEADER<<"\n";
            out<<FILE_START<<"\n";
        }
        
        //写入新的历史记录
        for (int i = 0; i < num; ++i) {
            out<<HISTORY_PATH<<history.at(i)<<">\n";
        }
        out<<FILE_END;      //写入文件结束标志
        
        mHistoryList.clear();
        mHistoryList = history;
        
        return true;
    } else {
        return false;
    }
}


/*
 * 函数名称：    saveRegisterNumToFile(QString path, QString num)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    保存软件注册码包文件中
 * 输入参数：    path:要保存为的文件路径       num:要保存的软件注册码
 * 输出参数：    无
 * 返回值：      保存成功返回true，否恩泽返回false
*/
bool ParseSettingFile::saveRegisterNumToFile(QString path, QString num)
{
    //判断该文件的父路径是否存在，若不存在则创建
    if (!isDirExist(path, true)) {
        return false;
    }
    
    bool isCreateNewFile = false;
    QString allContent = "";
    QStringList list;
    QFile file(path);
    //如果已经存在该文件
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //如果该文件是配置文件
        if (checkFile(path)) {
            QTextStream in(&file);
            in.setCodec(QTextCodec::codecForName(QString(SYSTEM_CODE_TYPE).toLatin1()));
            allContent = in.readAll();
        } else {
            allContent = "";
        }
        
        file.close();
    }
    
    //尝试3次打开该文件，若均失败则存储参数失败（该方式打开文件时，若文件不存在会主动创建）
    int cnt = 3;
    while (cnt--) {
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            break;
        }
    }
    
    //若未空，则表示该文件为新建的
    if (allContent == "") {
        isCreateNewFile = true;
    }
    
    //cnt大于0，表示成功打开了该文件
    if(cnt > 0)
    {
        QTextStream out(&file);
        out.setCodec(QTextCodec::codecForName(QString(SYSTEM_CODE_TYPE).toLatin1()));
        list = allContent.split("\n");
        int lines = list.size();
        QString tempStr;
        for (int i = 0; i < lines; ++i) {
            tempStr = list.at(i);
            //过滤掉空白行
            if (tempStr.isEmpty()) {
                continue;
            }
            
            if (tempStr.startsWith(REGISTER_NUM)) {
                continue;
            }
            
            if (!tempStr.startsWith(FILE_END)) {
                out<<tempStr + "\n";
            }
            
        }
        
        //如果是新建的文件，则写入配置文件的头和起始标志
        if (isCreateNewFile) {
            out<<FILE_HEADER<<"\n";
            out<<FILE_START<<"\n";
        }
        
        //写入新的软件注册码
        out<<REGISTER_NUM<<num<<">\n";
        out<<FILE_END;      //写入文件结束标志
        
        Security *security = Security::getInstance();
        security->setRegisted(true);
        
        return true;
    } else {
        return false;
    }
}


/*
 * 函数名称：    createParamsItem(QString key, QString value)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    创建一个参数项（其格式为：键$@$值）
 * 输入参数：    key:键        value:值
 * 输出参数：    无
 * 返回值：      无
*/
QString ParseSettingFile::createParamsItem(QString key, QString value)
{
    return QString(key + PARAMS_SEPARATOR + value);
}


/*
 * 函数名称：    checkFile(QString path)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    检查文件是否为设置文件
 * 输入参数：    path:要检查的文件路径
 * 输出参数：    无
 * 返回值：      是的返回true，否则返回false
*/
bool ParseSettingFile::checkFile(QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    
    bool isSuccess = false;
    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName(QString(SYSTEM_CODE_TYPE).toLatin1()));
    QString buff;
    int step = 0;
    while(!in.atEnd() && !isSuccess) {
        buff = in.readLine();
        
        switch (step) {
        case 0:     //判断文件头
            if (buff.startsWith(FILE_HEADER)) {
                step = 1;
            }
            break;
        case 1:     //判断起始标志
            if (buff.startsWith(FILE_START)) {
                step = 2;
            } else {
                step = 0;
            }
            break;
        case 2:     //判断结束标志
            if (buff.startsWith(FILE_END)) {
                isSuccess = true;
            }
            break;
        default:
            break;
        }
    }
    
    return isSuccess;
}


/*
 * 函数名称：    parseFile(QString path)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    解析配置文件
 * 输入参数：    path:要解析的文件路径
 * 输出参数：    无
 * 返回值：      无
*/
void ParseSettingFile::parseFile(QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        loadDefaultConfig();
        return;
    }
    
    bool isStart = false;
    bool isEnd = false;
    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName(QString(SYSTEM_CODE_TYPE).toLatin1()));
    QString buff;
    mHistoryList.clear();
    while(!in.atEnd()) {
        buff = in.readLine();
        
        if ((isStart == false) && (buff.startsWith(FILE_START))) {
            isStart = true;
            continue;
        }
        if ((isStart == true) && (isEnd == false) && (buff.startsWith(FILE_END))) {
            isEnd = true;
            break;
        }
        
        if (isStart) {
            if (buff.startsWith(FONT_TYPE)) {
                QString text = buff.split("=")[1].split(">")[0];
                fontType = (text.isEmpty()) ? getValueByKey(mDefaultParams,FONT_TYPE) : text;
            } else if (buff.startsWith(FONT_SIZE)) {
                QString text = buff.split("=")[1].split(">")[0];
                fontSize = (stringIsNum(text)) ? text.toInt() : 14;
            } else if (buff.startsWith(CODE_TYPE)) {
                QString text = buff.split("=")[1].split(">")[0];
                codeType = (text.isEmpty()) ? getValueByKey(mDefaultParams,CODE_TYPE) : text;
            } else if (buff.startsWith(ADB_PATH)) {
                QString text = buff.split("=")[1].split(">")[0];
                adbPath = (text.isEmpty()) ? getValueByKey(mDefaultParams,ADB_PATH) : text;
            } else if (buff.startsWith(HISTORY_PATH)) {
                QString text = buff.split("=")[1].split(">")[0];
                mHistoryList.append(text);
            } else if (buff.startsWith(REGISTER_NUM)) {
                QString text = buff.split("=")[1].split(">")[0];
                registerNum = text;
            }
        }
    }
    
    //如果未识别到其实标志和结束标志，则表示文件解析失败，故而加载默认的配置
    if (!isStart || !isEnd) {
        loadDefaultConfig();
        return;
    }
}


/*
 * 函数名称：    stringIsNum(QString str)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    判断一个字符串是否可以转换为数字
 * 输入参数：    str:要判断的字符串
 * 输出参数：    无
 * 返回值：      可以转换返回true，否则返回false
*/
bool ParseSettingFile::stringIsNum(QString str)
{
    if (str.isEmpty()) {
        return false;
    }
    
    QChar temp;
    for (int i = 0; i < str.length(); ++i) {
        temp = str.at(i);
        if ((temp < '0') || (temp > '9')) {
            return false;
        }
    }
    
    return true;
}


/*
 * 函数名称：    getKey(QString params)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    从一个参数项中获取键
 * 输入参数：    params:参数项
 * 输出参数：    无
 * 返回值：      返回键
*/
QString ParseSettingFile::getKey(QString params)
{
    return params.split(PARAMS_SEPARATOR)[0];
}


/*
 * 函数名称：    getValue(QString params)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    从一个参数项中获取值
 * 输入参数：    params:参数项
 * 输出参数：    无
 * 返回值：      返回值
*/
QString ParseSettingFile::getValue(QString params)
{
    return params.split(PARAMS_SEPARATOR)[1];
}


/*
 * 函数名称：    getValueByKey(QStringList paramList, QString key)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    从参数集合中查找指定键对应的值
 * 输入参数：    paramList:要查找的参数集合      key:要查找的值对应的键
 * 输出参数：    无
 * 返回值：      返回查找到的值，若未找到则返回空字符串""
*/
QString ParseSettingFile::getValueByKey(QStringList paramList, QString key)
{
    int num = paramList.size();
    for (int i = 0; i < num; ++i) {
        if (key == getKey(paramList.at(i))) {
            return getValue(paramList.at(i));
        }
    }
    return "";
}


/*
 * 函数名称：    isContainKey(QStringList paramList, QString key)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    判断某个参数集合中是否包含一个键
 * 输入参数：    paramList:要判断的参数集合      key:要判断的键
 * 输出参数：    无
 * 返回值：      包含返回true，否则返回false
*/
bool ParseSettingFile::isContainKey(QStringList paramList, QString key)
{
    int num = paramList.size();
    for (int i = 0; i < num; ++i) {
        if (key == getKey(paramList.at(i))) {
            return true;
        }
    }
    return false;
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
bool ParseSettingFile::isDirExist(QString fullPath, bool isCreate)
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
 * 函数名称：    updateNewValue(QStringList paramList, QString key, QStringList &isSaveList)
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.3.17
 * 函数功能：    重新构建指定键值在配置文件中的字符串，如"<FontType=Courier New>"
 * 输入参数：    paramList:参数集合     key:键      isSaveList:是否更新该值的记录表（1:表示更新了，0:表示未更新）
 * 输出参数：    无
 * 返回值：      返回更新后的值，若无更新则返回空字符串""
*/
QString ParseSettingFile::updateNewValue(QStringList paramList, QString key, QStringList &isSaveList)
{
    QString tempStr;
    QString value = getValueByKey(paramList,key);//获取该键对应的值
    int index = paramList.indexOf(createParamsItem(key,value));//获取该参数项在参数表中的索引号
    //-1表示获取索引号失败
    if (index == -1) {
        return "";
    }
    
    //如果参数表中包含有该键
    if (isContainKey(paramList,key)) {
        tempStr = key + value + ">";
        isSaveList.replace(index,"1");//将对应的记录标志置1，表示更新了该项参数
        return tempStr;
    } else {
        return "";
    }
}




