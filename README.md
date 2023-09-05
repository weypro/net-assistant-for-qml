# net-assistant-for-qml

网络助手QML版，提供tcp client和server

分别使用boost::asio和qtcpsocket实现，其中boost::asio为当前版本，qtcpsocket作为早期版本，单独放在另一个分支

打开项目时会自动单独下载最新的vcpkg，无需提前配置

开发环境：qt6.5







`-DCMAKE_PREFIX_PATH=C:\Qt\6.5.2\msvc2019_64\lib\cmake -Wno-dev`

`Path=C:\Qt\Tools\mingw1120_64\lib\;C:\Qt\Tools\mingw1120_64\bin\;C:\Qt\6.5.2\msvc2019_64\bin`