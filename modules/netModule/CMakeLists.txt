project(
        netModule
        VERSION 0.1
        LANGUAGES CXX)

# 导入fmt
find_package(fmt CONFIG REQUIRED)
# 导入boost
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
find_package(Boost COMPONENTS REQUIRED system locale ) # system是asio依赖的组件
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

# 避免在windows用mingw下提示file too big
# 注意qt的mingw默认是w32，但-mbig-obj标志位只对mingw-w64有效
if(CMAKE_COMPILER_IS_GNUCXX)
    #检查GNU是否支持-Wa,-mbig-obj
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-Wa,-mbig-obj" GNU_BIG_OBJ_FLAG_ENABLE)
    message(STATUS GNU_BIG_OBJ_FLAG_ENABLE=${GNU_BIG_OBJ_FLAG_ENABLE})
endif()

qt_add_library(${PROJECT_NAME} STATIC)
qt_add_qml_module(${PROJECT_NAME}
        URI ${PROJECT_NAME}
        VERSION 1.0
        SOURCES netConn.h
        SOURCES netConn.cpp
        SOURCES netConnItemSettings.h
        SOURCES tcpClientHandler.h
        SOURCES tcpClientHandler.cpp
        SOURCES netConnItem.h
        SOURCES netConnItem.cpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt6::Quick
        fmt::fmt
        Boost::system
        Boost::locale
)


# 根据之前的标志位加入相应的big obj标志位
target_compile_options(${PROJECT_NAME}
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${GNU_BIG_OBJ_FLAG_ENABLE}>>:-Wa,-mbig-obj>
)

# 公开头文件
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_NAME} include)

# windows下必须添加以下两个库
if (WIN32)
    target_link_libraries(${PROJECT_NAME}
            PRIVATE wsock32
            ws2_32
    )
endif ()

