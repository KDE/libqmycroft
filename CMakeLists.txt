# Set minimum CMake version (required for CMake 3.0 or later)
cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0048 NEW)
project(libqmycroft VERSION "1.0.0")

set(QT_MIN_VERSION "5.12.0")
set(KF5_MIN_VERSION "5.88.0")
 
# Use Extra CMake Modules (ECM) for common functionality.
# See http://api.kde.org/ecm/manual/ecm.7.html
# and http://api.kde.org/ecm/manual/ecm-kde-modules.7.html 
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_MODULE_PATH})
set(VERSION 2.1.0)

include(ECMFindQmlModule)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Quick
    Core
    Qml
    Network
    WebSockets
    Multimedia
)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING -DQT_USE_QSTRINGBUILDER -DQT_NO_CAST_TO_ASCII -DQT_NO_CAST_FROM_ASCII)

add_subdirectory(lib)
