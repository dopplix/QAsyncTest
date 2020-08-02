#include "widget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QEventLoop>
#include <QDebug>
#include <QLineEdit>
#include <QtConcurrent>
#include <QJsonObject>

Widget::Widget(QWidget *parent) : QWidget(parent){
    QVBoxLayout* layout = new QVBoxLayout;
    QPushButton* requestPush = new QPushButton("Request");
    QPushButton* receivePush = new QPushButton("Receive");
    QLineEdit* serverStrEdit = new QLineEdit;
    this->setLayout(layout);
        layout->addWidget(requestPush);
        layout->addWidget(receivePush);
        layout->addWidget(serverStrEdit);
    connect(requestPush,&QPushButton::clicked,[=]{
        QtConcurrent::run([=]{
            QString serverStr = request();
            qDebug()<<"In Call"<<serverStr;
        });
    });
    connect(receivePush,&QPushButton::clicked,[=]{
        QString serverStr = serverStrEdit->text();
        emit(receiveButtonClicked(serverStr));
    });
}
Widget::~Widget(){

}
QString Widget::request(){
    qDebug()<<"Request";
    QEventLoop* loop = new QEventLoop;
    QJsonObject* resultHolder = new QJsonObject;
    QObject receiver;
    QTimer timer;
    connect(this,&Widget::receiveButtonClicked,&receiver,[loop,resultHolder](QString serverStr){
        qDebug()<<"Received";
        resultHolder->insert("result",serverStr);
        loop->quit();
    });
    timer.singleShot(5000,&receiver,[loop,resultHolder]{
        qDebug()<<"Timeout";
        resultHolder->insert("result","Timeout");
        loop->quit();
    });
    loop->exec();
    qDebug()<<"Quit";
    QString resultStr = resultHolder->value("result").toString();
    delete loop;
    delete resultHolder;
    return resultStr;
}

