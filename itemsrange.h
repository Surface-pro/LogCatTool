#ifndef ITEMSRANGE_H
#define ITEMSRANGE_H

#include <QObject>

class ItemsRange : public QObject
{
    Q_OBJECT
public:
    explicit ItemsRange(QObject *parent = 0,int first = 0,int visibleFirst = 0,
                        int count = 0,int pageItemNum = 0,int total = 0);
    
signals:
    
public slots:
    
public:
    int getFirst();         //获取缓冲区范围第一行
    int getVisibleFirst();  //获取缓冲区范围可见的第一行
    int getLast();          //获取缓冲区范围最后一行
    int getCount();         //获取缓冲区范围数据总行数
    int getPageItemNum();   //获取屏幕一次能够完整显示的行数
    int getTotal();         //获取当前数据表的总数据行数
    
private:
    int first;              //第一行
    int visibleFirst;       //可见的第一行
    int last;               //最后一行
    int count;              //总行数
    int pageItemNum;        //一界面能够完整显示的行数
    int total;              //当前数据表中的总数据行数
    
};

#endif // ITEMSRANGE_H
