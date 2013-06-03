######################################################################
# Automatically generated by qmake (2.01a) Tue Nov 15 17:49:49 2011
######################################################################

# Version number in <major>.<minor>.<patch> form
isEmpty(VERSION) {
        VERSION = 0.9.0
}

# Application name displayed to user
isEmpty(APPNAME) {
        APPNAME = "Wahjam"
}

# Organization name used for Qt settings
isEmpty(ORGNAME) {
        ORGNAME = "Wahjam Project"
}

# Organization domain displayed to user and used for Qt settings
isEmpty(ORGDOMAIN) {
        ORGDOMAIN = "wahjam.org"
}

# Qt client binary name
isEmpty(QTCLIENT_TARGET) {
	TARGET = wahjam
} else {
	TARGET = $$QTCLIENT_TARGET
}

DEFINES += "VERSION=\'\"$$VERSION\"\'"
DEFINES += "COMMIT_ID=\'\"$$system(git rev-parse HEAD)\"\'"
DEFINES += "APPNAME=\'\"$$APPNAME\"\'"
DEFINES += "ORGNAME=\'\"$$ORGNAME\"\'"
DEFINES += "ORGDOMAIN=\'\"$$ORGDOMAIN\"\'"
DEFINES += "JAMMR_API_URL=\'\"$$JAMMR_API_URL\"\'"
DEFINES += "JAMMR_REGISTER_URL=\'\"$$JAMMR_REGISTER_URL\"\'"
DEFINES += "JAMMR_UPGRADE_URL=\'\"$$JAMMR_UPGRADE_URL\"\'"
DEFINES += "JAMMR_UPDATE_URL=\'\"$$JAMMR_UPDATE_URL\"\'"
DEFINES += "JAMMR_DOWNLOAD_URL=\'\"$$JAMMR_DOWNLOAD_URL\"\'"

target.path = /bin
INSTALLS += target

TEMPLATE = app
DEPENDPATH += ..
INCLUDEPATH += ..
QT += network xml

include(../common/libcommon.pri)

QMAKE_CXXFLAGS += -Wno-write-strings
CONFIG += link_pkgconfig
PKGCONFIG += vorbis vorbisenc portaudio-2.0

# Code in common/ does not use wide characters
win32:DEFINES -= UNICODE

win32:RC_FILE = qtclient.rc

mac:QMAKE_INFO_PLIST = Info.plist-$${TARGET}
mac:ICON = $${TARGET}.icns

# Input
HEADERS += MainWindow.h
HEADERS += ConnectDialog.h
HEADERS += JammrConnectDialog.h
HEADERS += ChannelTreeWidget.h
HEADERS += PortAudioConfigDialog.h
HEADERS += ServerBrowser.h
HEADERS += MetronomeBar.h
HEADERS += ChatOutput.h
HEADERS += AddVSTPluginDialog.h
HEADERS += VSTConfigDialog.h
HEADERS += JammrLoginDialog.h
HEADERS += JammrServerBrowser.h
HEADERS += JammrAccessControlDialog.h
HEADERS += JammrUpdateChecker.h
HEADERS += NINJAMServerBrowser.h
HEADERS += logging.h
HEADERS += VSTPlugin.h
HEADERS += VSTProcessor.h

SOURCES += qtclient.cpp
SOURCES += MainWindow.cpp
SOURCES += ConnectDialog.cpp
SOURCES += JammrConnectDialog.cpp
SOURCES += ChannelTreeWidget.cpp
SOURCES += PortAudioConfigDialog.cpp
SOURCES += ServerBrowser.cpp
SOURCES += MetronomeBar.cpp
SOURCES += ChatOutput.cpp
SOURCES += AddVSTPluginDialog.cpp
SOURCES += VSTConfigDialog.cpp
SOURCES += JammrLoginDialog.cpp
SOURCES += JammrServerBrowser.cpp
SOURCES += JammrAccessControlDialog.cpp
SOURCES += JammrUpdateChecker.cpp
SOURCES += NINJAMServerBrowser.cpp
SOURCES += logging.cpp
SOURCES += VSTPlugin.cpp
SOURCES += VSTProcessor.cpp
