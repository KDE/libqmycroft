set(qmycroft_SRCS qmycroftplugin.cpp skillmanager.cpp skillentry.cpp controller.cpp)

add_library(qmycroftplugin SHARED ${qmycroft_SRCS})

target_link_libraries(qmycroftplugin
    Qt5::Core 
    Qt5::Quick
    Qt5::Qml
    Qt5::Network
    Qt5::WebSockets
)

########### install files ###############
install(TARGETS qmycroftplugin DESTINATION ${KDE_INSTALL_QMLDIR}/Libqmycroft)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/Libqmycroft)
install(FILES plugins.qmltypes DESTINATION ${KDE_INSTALL_QMLDIR}/Libqmycroft)
