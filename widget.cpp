#include "widget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QEventLoop>
#include <QDebug>
#include <QLineEdit>
#include <QtConcurrent>
#include <QJsonObject>
#include <QThread>

Widget::Widget(QWidget *parent) : QWidget(parent){
    QVBoxLayout* layout = new QVBoxLayout;
    QPushButton* requestPush = new QPushButton("Request");
    QPushButton* receivePush = new QPushButton("Receive");
    QPushButton* workerPush = new QPushButton("Start Worker");
    QLineEdit* serverStrEdit = new QLineEdit;
    this->setLayout(layout);
        layout->addWidget(requestPush);
        layout->addWidget(receivePush);
        layout->addWidget(serverStrEdit);
        layout->addWidget(workerPush);
    connect(requestPush,&QPushButton::clicked,[=]{
//        QtConcurrent::run([=]{
            QString serverStr = request();
            qDebug()<<"In Call"<<serverStr;
//        });
    });
    connect(receivePush,&QPushButton::clicked,[=]{
        QString serverStr = serverStrEdit->text();
        emit(receiveButtonClicked(serverStr));
    });
    connect(workerPush,&QPushButton::clicked,[=]{
        startWorker();
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
void Widget::startWorker(){
    QThread* thread = new QThread;
    QObject* worker = new QObject;
    worker->moveToThread(thread);
    thread->start();
    connect(this,&Widget::receiveButtonClicked,worker,[worker]{
        worker->deleteLater();
    },Qt::BlockingQueuedConnection);
    connect(worker,&QObject::destroyed,thread,&QThread::quit);
    connect(thread,&QThread::finished,thread,&QThread::deleteLater);
}

