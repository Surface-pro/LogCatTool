#include "itemsrange.h"

ItemsRange::ItemsRange(QObject *parent,int first,int visibleFirst,int count,
                       int pageItemNum,int total) :
    QObject(parent)
{
    //确保所有数据不为负数
    first = (first >= 0) ? first : 0;
    visibleFirst = (visibleFirst >= 0) ? visibleFirst : 0;
    count = (count >= 0) ? count : 0;
    pageItemNum = (pageItemNum >= 0) ? pageItemNum : 0;
    total = (total >= 0) ? total : 0;
    
    //如果当前数据表为空
    if (total == 0) {
        first = 0;
        count = 0;
    }
    
    //如果缓冲区数据总数大于当前数据表中的总数据数
    if (count > total) {
        count = total;
    }
    
    //如果可见第一行小于第一行
    if (visibleFirst < first) {
        visibleFirst = first;
    }
    
    //如果最后一行大于总数据行数，则自动重新设置可见第一行的位置
    if ((visibleFirst + pageItemNum - 1) > total) {
        visibleFirst = total - pageItemNum + 1;
        visibleFirst = (visibleFirst >= 0) ? visibleFirst : 0;
    }
    
    //存储计算后的值
    this->first = first;
    this->visibleFirst = visibleFirst;
    this->count = count;
    this->last = first + count - 1;
    this->pageItemNum = pageItemNum;
    this->total = total;
    
    //如果最后一行大于等于当前数据表中的数据总数，则重新设置最后一行
    if (this->last >= total) {
        this->last = total - 1;
        this->first = total - count;
    }
}


/*
 * 函数名称：    getFirst()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    获取缓冲区范围第一行
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回缓冲区范围第一行
*/
int ItemsRange::getFirst()
{
    return first;
}


/*
 * 函数名称：    getVisibleFirst()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    获取缓冲区范围可见的第一行
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回缓冲区范围可见的第一行
*/
int ItemsRange::getVisibleFirst()
{
    return visibleFirst;
}


/*
 * 函数名称：    getLast()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    获取缓冲区范围最后一行
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回缓冲区范围最后一行
*/
int ItemsRange::getLast()
{
    return last;
}


/*
 * 函数名称：    getCount()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    获取缓冲区范围数据总行数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回缓冲区范围数据总行数
*/
int ItemsRange::getCount()
{
    return count;
}


/*
 * 函数名称：    getPageItemNum()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    获取屏幕一次能够完整显示的行数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回屏幕一次能够完整显示的行数
*/
int ItemsRange::getPageItemNum()
{
    return pageItemNum;
}


/*
 * 函数名称：    getTotal()
 * 函数版本：        1.0.0
 * 作者：            HXL
 * 创建日期：    2017.2.16
 * 函数功能：    获取当前数据表的总数据行数
 * 输入参数：    无
 * 输出参数：    无
 * 返回值：      返回当前数据表的总数据行数
*/
int ItemsRange::getTotal()
{
    return total;
}
