cmake_minimum_required(VERSION 3.16)

project(netassistant VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

#add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
#add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)


set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# 自动下载并导入vcpkg
cmake_policy(SET CMP0135 NEW)
if(PROJECT_IS_TOP_LEVEL)
        include(FetchContent)
        message(STATUS "Fetching vcpkg...")
        FetchContent_Declare(vcpkg URL "https://github.com/microsoft/vcpkg/archive/master.zip")
        FetchContent_MakeAvailable(vcpkg)
        include("${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake")
endif()
# 导入fmt
find_package(fmt CONFIG REQUIRED)
# 导入boost

#set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
#set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
#set(Boost_USE_MULTITHREADED      ON)
find_package(Boost COMPONENTS system REQUIRED)
find_package(Boost COMPONENTS locale REQUIRED)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif()

qt_standard_project_setup()

add_subdirectory(modules/netModule)

qt_add_executable(appnetassistant
    main.cpp
)

qt_add_qml_module(appnetassistant
    URI netassistant
    VERSION 1.0
    QML_FILES
    Main.qml

    "qml/GlobalColor.qml"
    "qml/Component/CustomTextArea.qml"
    "qml/SettingsView/Settings.qml"
    "qml/SettingsView/SettingsView.qml"
    "qml/SettingsView/SettingsComponentView.qml"
    "qml/SettingsView/SettingsComponentSlider.qml"
    "qml/SettingsView/SettingsComponentCheckBox.qml"
    "qml/SettingsView/SettingsComponentComboBox.qml"
    "qml/SettingsView/SettingsComponentText.qml"
    "qml/SettingsView/SettingsComponentContainer.qml"
    RESOURCES
    "qml/images/arrow-right.svg"
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appnetassistant PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appnetassistant
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)



target_link_libraries(appnetassistant
    PRIVATE Qt6::Quick
#    fmt::fmt
#    Boost::system
#    Boost::locale
    netModule
)

include(GNUInstallDirs)
install(TARGETS appnetassistant
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


qt_generate_deploy_qml_app_script(
    TARGET appnetassistant
    OUTPUT_SCRIPT deploy_script
    MACOS_BUNDLE_POST_BUILD
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
)
install(SCRIPT ${deploy_script})
