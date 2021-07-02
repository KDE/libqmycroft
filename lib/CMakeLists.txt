set(qmycroft_SRCS qmycroftplugin.cpp skillmanager.cpp skillentry.cpp controller.cpp audiotranscribe.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
qt5_add_resources(qmycroft_SRCS libqmycroft.qrc)

add_library(qmycroftplugin SHARED ${qmycroft_SRCS} ${RESOURCES})

target_link_libraries(qmycroftplugin
    Qt5::Core 
    Qt5::Quick
    Qt5::Qml
    Qt5::Network
    Qt5::WebSockets
    Qt5::Multimedia
)

########### install files ###############
install(TARGETS qmycroftplugin DESTINATION ${KDE_INSTALL_QMLDIR}/Libqmycroft)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/Libqmycroft)
install(DIRECTORY qml/ DESTINATION ${KDE_INSTALL_QMLDIR}/Libqmycroft)
install(FILES plugins.qmltypes DESTINATION ${KDE_INSTALL_QMLDIR}/Libqmycroft)
