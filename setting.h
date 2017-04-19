#ifndef SETTING_H
#define SETTING_H

#include <QDialog>
#include <QDebug>
#include "mainwindow.h"

namespace Ui {
class Setting;
}

class Setting : public QDialog
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = 0);
    ~Setting();

public:
    bool getIsClickTrue();  //退出时是否点击了确认
    QString getFontType();  //获取字体类型
    int getFontSize();      //获取字体大小
    QString getCodeType();  //获取编码类型
    QString getADBPath();   //获取ADB工具的路径
    void transParams(QString fontType,int fontSize,
                     QString codeType,QString ADBPath);   //向该对话框传入参数

private:
    void updatePreviewFont();       //更新预览字体样式

private slots:
    void btnOKClickedSlot();        //确认按钮槽函数
    void fontTypeChangedSlot();     //字体类型选择框内容变化槽函数
    void fontSizeChangedSlot();     //字体大小选择框内容变化槽函数
    void resetSlot();               //重置按钮槽函数
    void codeTypeChangedSlot();     //编码类型选择框内容变化槽函数
    void selectPathSlot();          //选择ADB工具路径的槽函数

private:
    Ui::Setting *ui;

    bool isClickTrue;   //是否点击了确认键
    QString fontType;   //字体类型
    int fontSize;       //字体大小

    QString codeType;   //编码类型

    QString ADBPath;    //ADB工具路径
};

#endif // SETTING_H
