#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    // new std::thread(
    //     [this](){    
    pwned::tcp_server *server  = new pwned::tcp_server  (6001, this);
    http_handler * handler = new http_handler(server);
    QObject::connect(server, SIGNAL(socket_connection(QTcpSocket*)), handler, SLOT(socket_connection(QTcpSocket*)));
    QObject::connect(server, SIGNAL(socket_disconnected(QTcpSocket*)), handler, SLOT(socket_disconnected(QTcpSocket*)));
    //QObject::connect(server, SIGNAL(socket_data(QTcpSocket*, QByteArray const &)), handler, SLOT(socket_data(QTcpSocket*, QByteArray const &)));
        // });

    ui->pushButton->setEnabled(false);
}

