#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <thread>
#include <QThread>
#include <fstream>


#define BUFF_LEN 2048
#define BUFF_FILE "tcp_512x256U32.raw"

#ifdef BUFF_FILE_OVERLOAD
#undef BUFF_FILE
#define BUFF_FILE BUFF_FILE_OVERLOAD
#endif

class Worker : public QObject {
    Q_OBJECT
public:
	QTcpSocket* socket;
    Worker(){};
    ~Worker(){};
    std::ifstream *file;
    int length{0};

public slots:
    void process()
	{

		uint8_t buf[BUFF_LEN];
        for(int i = 0 ; i < BUFF_LEN ; ++i)
        {
            buf[i] = 'x';
        }


		file= new std::ifstream(BUFF_FILE, std::ifstream::binary);
		if(file->is_open())
		{
			printf("is open !");

			file->seekg (0, file->end);
			length = file->tellg();
			file->seekg (0, file->beg);        
			printf("%d\n",length);
		}
		else
		{
			printf("not exists \"%s\" !!!\n", BUFF_FILE);
		}
 

		while (socket->state() ==  QAbstractSocket::ConnectedState)
		{
            file->read((char*)&buf[0],2048);
            if (file->tellg() == length)
            {
                file->seekg (0, file->beg); 
            }

			socket->write((char*)&buf[0],BUFF_LEN);
			socket->flush();
            std::this_thread::sleep_for(std::chrono::microseconds(320));
		}

		printf("thread FINISH\n"); 

	}
signals:
    void finished();
    void error(QString err);
private:
    // add your variables here
};


class http_handler: public QObject
{
	Q_OBJECT
	
	public:
	http_handler(QObject* parent)
	: QObject(parent)
	{}

	signals:
	void get(QString const &uri);

	public slots:
	void socket_connection(QTcpSocket* socket)
	{
		QThread* thread = new QThread();
		Worker* worker = new Worker();
		worker->socket = socket;
		worker->moveToThread(thread);
		//connect( worker, &Worker::error, this, &MyClass::errorString);
		connect( thread, &QThread::started, worker, &Worker::process);
		connect( worker, &Worker::finished, thread, &QThread::quit);
		connect( worker, &Worker::finished, worker, &Worker::deleteLater);
		connect( thread, &QThread::finished, thread, &QThread::deleteLater);
		thread->start();		
	}

	void socket_disconnected(QTcpSocket* socket)
	{
	}

	void socket_data(QTcpSocket* socket, QByteArray const &data)
	{
		qDebug()<< "got data! "<< data;
		if(data.startsWith("GET"))
		{
			socket-> write(
				"HTTP/1.0 200 OK \
				Date: Fri, 31 Dec 1999 23:59:59 GMT \
				Content-Type: text/html \
				Content-Length: 1354 \
				\
				<html> \
				<body> \
				<h1>Happy New Millennium!</h1> \
				</body> \
				</html>"
			);
			socket-> flush();
			
		}
	}
};
