#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QHttpServer>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHttpServerRequest>
#include <QHttpServerResponder>
#include <QNetworkAccessManager>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    defineRoutes();
    connectSlots();
    startServer();
    doRequests();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    m_httpServer = new QHttpServer(this);
    m_manager = new QNetworkAccessManager(this);
}

void MainWindow::connectSlots()
{
    connect(m_manager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            qDebug() << reply->readAll();
        }
    });
}

void MainWindow::defineRoutes()
{
    m_httpServer->route("/", [&]() -> QString
    {
        return "hello world";
    });

    m_httpServer->route("/name", [&]() -> QString
    {
        return "DuarteCorporation";
    });

    m_httpServer->route("/create", [&](const QHttpServerRequest &request) -> QString
    {
        if (request.method() == QHttpServerRequest::Method::Post)
        {
            QJsonDocument doc = QJsonDocument::fromJson(request.body());
            QJsonObject object = doc.object();
            return QString::number(object["value"].toInt());
        }
        else
        {
            return "Error: Only POST method is allowed!";
        }
    });
}

void MainWindow::startServer()
{
    m_httpServer->listen(QHostAddress::Any, 1234);
}

void MainWindow::doRequests()
{
    m_manager->get(QNetworkRequest(QUrl("http://localhost:1234")));

    m_manager->get(QNetworkRequest(QUrl("http://localhost:1234/name")));

    QNetworkRequest postRequest(QUrl("http://localhost:1234/create"));
    postRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_manager->post(postRequest, QString("{\"value\": 10}").toLatin1());

    m_manager->get(QNetworkRequest(QUrl("http://localhost:1234/create")));
}
