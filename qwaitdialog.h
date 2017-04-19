#ifndef QWAITDIALOG_H
#define QWAITDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QMovie>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>

class QWaitDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QWaitDialog(QWidget *parent = 0,QString title = "");
    ~QWaitDialog();
        
signals:
    
public slots:
        
private:
    QMovie *movie;          //gif动画
    QLabel *label;          //放置动画的容器
    QLabel *tip_label;      //标题
    QFrame *background;     //背景色
        
};

#endif // QWAITDIALOG_H
