#pragma once

#include <iostream>
#include <string>

#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlDebuggingEnabler>
#include <QQmlContext>
#include <QNetworkProxy>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QFile>
#include <QUrl>
#include <QTimer>
#include <QIcon>
#include <QFont>
#include <QLocale>
#include <QTranslator>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QSettings>

#ifndef BINANCE_H
#define BINANCE_H
class Binance : public QObject
{
    /*
        The base API endpoint is: https://fapi.binance.com

        Start User Data Stream
        POST /fapi/v1/listenKey

        Keepalive User Data Stream
        PUT /fapi/v1/listenKey

        Close User Data Stream
        DELETE /fapi/v1/listenKey

        wss://fstream-auth.binance.com/ws/<listenKey>?listenKey=<listenKey>
    */

    Q_OBJECT

public:
    Binance(QSettings& s) :
    apiKey(s.value("apikey").toString().toStdString()),
    subscribe(QJsonDocument::fromJson(QByteArray::fromStdString(s.value("subscribe").toString().toStdString())).toJson().toStdString()) {
        connect(&keepaliveTimer, &QTimer::timeout, this, &Binance::keepalive);
        request.setRawHeader("X-MBX-APIKEY", QByteArray::fromStdString(apiKey));
    }

    void keepalive() {
        disconnect(conn);
        networkAccessManager.put(request, QByteArray::fromStdString(""));
        qDebug() << "keepalive";
    }

public
slots:

    void obtainedLicenseKey(QNetworkReply* reply) {
        QString res = reply->readAll();
        qDebug() << "obtainedLicenseKey: " << res;
        QJsonDocument ret = QJsonDocument::fromJson(QByteArray::fromStdString(res.toStdString()));
        auto listen_key = ret["listenKey"].toString();

        QString wss_url = QString("wss://fstream-auth.binance.com/ws/%1?listenKey=%2").arg(listen_key, listen_key);

        keepaliveTimer.start(30 * 60 * 1000);
        emit signalObtainedLicenseKey(wss_url);
    }

    void getUserDataStreamLicenseKey() {
        conn = connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(obtainedLicenseKey(QNetworkReply*)));
        networkAccessManager.post(request, QByteArray::fromStdString(""));
    }

    QString getSubscribeParam() {
        return QString::fromStdString(subscribe);
    }

signals:
    void signalObtainedLicenseKey(const QString& listenKey) const;

private:
    std::string apiKey, subscribe;
    QUrl apiUrl = QUrl("https://fapi.binance.com/fapi/v1/listenKey");
    std::string listenKey;
    QTimer keepaliveTimer;
    QMetaObject::Connection conn;
    QNetworkRequest request = QNetworkRequest(apiUrl);
    QNetworkAccessManager networkAccessManager;
};
#endif