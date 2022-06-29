#include "CryptocurrencyPricesMonitor.h"

int main(int argc, char* argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QQmlDebuggingEnabler();
    
    QString path("config.ini"), default_apikey("Put your API key here.");
    QSettings settings(path, QSettings::IniFormat);
    QFile file(path);
    if (!file.exists()) {
        settings.setValue("apikey", default_apikey);
        settings.setValue("proxy", "http://127.0.0.1:10081");
        settings.setValue("subscribe", "{\"method\": \"SUBSCRIBE\", \"params\": [\"btcusdt@markPrice@1s\",\"ethusdt@markPrice@1s\"], \"id\": 1}");
        QCoreApplication::exit(-1);
    }

    QString apikey = settings.value("apikey").toString();
    if (apikey.compare(default_apikey) == 0) {
        QCoreApplication::exit(-1);
    }

    QGuiApplication app(argc, argv);
    auto icon = QIcon("qrc:/images/logo.png");
    app.setWindowIcon(icon);

    auto proxy_url = QUrl(settings.value("proxy").toString());
    auto proxy = QNetworkProxy();
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(proxy_url.host());
    proxy.setPort(proxy_url.port());
    QNetworkProxy::setApplicationProxy(proxy);
    
    Binance binance(settings);

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/qml");
    const QUrl url(u"qrc:/qml/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    QObject::connect(&engine, &QQmlApplicationEngine::quit, &QGuiApplication::quit);
    engine.rootContext()->setContextProperty("binance", &binance);
    engine.load(url);

    return app.exec();
}
