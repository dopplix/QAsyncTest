#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class Widget : public QWidget{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QString request();
    bool isBusy = false;
signals:
    void receiveButtonClicked(QString serverStr);
};
#endif // WIDGET_H
