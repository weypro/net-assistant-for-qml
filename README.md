# net-assistant-for-qml

网络助手QML版，提供tcp client和server

分别使用asio2(boost::asio)和qtcpsocket实现，其中asio2为当前版本，qtcpsocket作为早期版本，单独放在另一个分支

打开项目时会自动单独下载最新的vcpkg，无需提前配置，需要的库在`vcpkg.json`中

开发环境：qt6.5.2, msvc(windows), gcc(linux)



### clion简单配置

参考链接

https://blog.csdn.net/yyz_1987/article/details/123651161

https://blog.csdn.net/lordman19820219/article/details/130840929

https://www.jetbrains.com/help/clion/qt-tutorial.html#troubleshooting

以链接1为例，加入`-DCMAKE_PREFIX_PATH=C:\Qt\6.5.2\msvc2019_64\lib\cmake -Wno-dev`

vcpkg默认会自动下载，无需配置，也可以自行修改cmake以手动安装

在debug/release configuration有设置environment variables系统变量的地方，需要自行修改path来模拟qtcreator的做法（见qtcreator项目配置项中的add build library search path to PATH）

例如可以写`Path=C:\Qt\Tools\mingw1120_64\lib\;C:\Qt\Tools\mingw1120_64\bin\;C:\Qt\6.5.2\msvc2019_64\bin`