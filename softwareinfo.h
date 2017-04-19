#ifndef SOFTWAREINFO_H
#define SOFTWAREINFO_H

#include <QObject>
#include <QStringList>

class SoftwareInfo : public QObject
{
    Q_OBJECT
public:
    explicit SoftwareInfo(QObject *parent = 0);

public:
    static QString getCurVersionInfo();         //获取当前版本信息
    static QString getSoftwareFuncInfo();       //获取软件功能简介
    static QStringList getUpdateInfo();         //获取更新日志索引信息
    static QStringList getUpdateInfoDetails();  //获取更新日志详细信息

};

#endif // SOFTWAREINFO_H
