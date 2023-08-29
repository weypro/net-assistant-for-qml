cmake_minimum_required(VERSION 3.16)

project(netassistant VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()

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
    "qml/images/arrow.png"
    qml.qrc
    SOURCES modules/netModule/netConn.h
    SOURCES modules/netModule/netConn.cpp


    SOURCES modules/netModule/netConnItemSettings.h
    SOURCES modules/netModule/netConnItem.h modules/netModule/netConnItem.cpp
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
