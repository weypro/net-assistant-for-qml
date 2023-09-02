#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include "modules/netModule/NetConn.h"

int main(int argc, char* argv[]) {
    //    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    //    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    //    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
    //        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
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
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
// #include <QDebug>
// #include <cstdio>
// #include <iostream>
// #include <thread>

// #include <boost/asio/co_spawn.hpp>
// #include <boost/asio/detached.hpp>
// #include <boost/asio/io_context.hpp>
// #include <boost/asio/ip/tcp.hpp>
// #include <boost/asio/signal_set.hpp>
// #include <boost/asio/write.hpp>

// using boost::asio::awaitable;
// using boost::asio::co_spawn;
// using boost::asio::detached;
// using boost::asio::ip::tcp;
// using boost::asio::use_awaitable;
// namespace this_coro = boost::asio::this_coro;

// #if defined(BOOST_ASIO_ENABLE_HANDLER_TRACKING)
// #define use_awaitable boost::asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
// #endif

// awaitable<void> echo(tcp::socket socket)
//{
//     try {
//         char data[1024];
//         for (;;) {
//             std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data),
//                                                             use_awaitable);
//             co_await async_write(socket, boost::asio::buffer(data, n), use_awaitable);
//         }
//     } catch (std::exception &e) {
//         std::printf("echo Exception: %s\n", e.what());
//     }
// }

// awaitable<void> listener()
//{
//     auto executor = co_await this_coro::executor;
//     tcp::acceptor acceptor(executor, {tcp::v4(), 55555});
//     for (;;) {
//         tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
//         co_spawn(executor, echo(std::move(socket)), detached);
//     }
// }

// int main()
//{
//     try {
//         boost::asio::io_context io_context(1);

//        boost::asio::signal_set signals2(io_context, SIGINT, SIGTERM, SIGILL);
//        signals2.async_wait([&](auto, auto) { io_context.stop(); });

//        co_spawn(io_context, listener(), detached);

//        auto serviceThread = std::thread([&io_context = io_context]() {
//            qDebug() << "service start";
//            auto work = boost::asio::make_work_guard(io_context);
//            io_context.run();
//            qDebug() << "service stop";
//        });
//        serviceThread.join();
//    } catch (std::exception &e) {
//        std::printf("Exception: %s\n", e.what());
//    }
//}
