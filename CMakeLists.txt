cmake_minimum_required(VERSION 3.16)

project(CryptocurrencyPricesMonitor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "C:/Tools/SDK/Qt/6.2.4/msvc2022_64/lib/cmake/")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_MSVC_STATIC_RT TRUE)
find_package(OpenSSL REQUIRED)

find_package(Qt6 6.2
    COMPONENTS
        Core
        Qml
        Quick
        QuickControls2
        QuickLayouts
        Network
        WebSockets
    REQUIRED
)

set(SOURCES CryptocurrencyPricesMonitor.cpp)
set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/images/logo.rc")
qt_add_resources(SOURCES resources.qrc)

qt_add_executable(CryptocurrencyPricesMonitor
    WIN32
    ${SOURCES}
    ${APP_ICON}
)

target_compile_definitions(CryptocurrencyPricesMonitor PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(CryptocurrencyPricesMonitor PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::QuickLayouts
    Qt6::Network
    Qt6::WebSockets
    OpenSSL::SSL
    OpenSSL::Crypto
)
