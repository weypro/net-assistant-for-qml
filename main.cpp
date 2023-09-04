#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include "modules/netModule/NetConn.h"

int main(int argc, char *argv[])
{
    // 这一句能让窗口移动时看起来不那么卡
    QQuickWindow::setDefaultAlphaBuffer(true);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // 注册类型
    qmlRegisterUncreatableMetaObject(module::net::connEnum::staticMetaObject,
                                     "module.net",
                                     1,
                                     0,
                                     "ConnEnum",
                                     "Error: only enums");
    qmlRegisterType<module::net::NetConn>("module.net", 1, 0, "NetConn");

    //    QStringList comboBoxModel = {"one", "two", "three"};
    //    engine.setContextProperty("comboBoxModel", &comboBoxModel);

    const QUrl url(u"qrc:/netassistant/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

