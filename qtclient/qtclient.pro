######################################################################
# Automatically generated by qmake (2.01a) Tue Nov 15 17:49:49 2011
######################################################################

# Version number in <major>.<minor>.<patch> form
isEmpty(VERSION) {
        VERSION = 1.2.90
}

# Application name displayed to user
isEmpty(APPNAME) {
        jammr {
                APPNAME = "jammr"
        } else {
                APPNAME = "Wahjam"
        }
}

# Organization name used for Qt settings
isEmpty(ORGNAME) {
        jammr {
                ORGNAME = "jammr"
        } else {
                ORGNAME = "Wahjam Project"
        }
}

# Organization domain displayed to user and used for Qt settings
isEmpty(ORGDOMAIN) {
        jammr {
                ORGDOMAIN = "jammr.net"
        } else {
                ORGDOMAIN = "wahjam.org"
        }
}

# Qt client binary name
isEmpty(QTCLIENT_TARGET) {
        jammr {
                TARGET = jammr
        } else {
                TARGET = wahjam
        }
} else {
        TARGET = $$QTCLIENT_TARGET
}

jammr {
        JAMMR_API_URL = "https://jammr.net/api/"
        JAMMR_REGISTER_URL = "https://jammr.net/accounts/register/"
        JAMMR_UPGRADE_URL = "https://jammr.net/pricing.html"
        win32:JAMMR_UPDATE_URL = "https://jammr.net/static/latest-windows.txt"
        mac:JAMMR_UPDATE_URL = "https://jammr.net/static/latest-mac.txt"
        linux:JAMMR_UPDATE_URL = "https://jammr.net/static/latest-linux.txt"
        JAMMR_DOWNLOAD_URL = "https://jammr.net/download.html"
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

unix {
	# Build reverse domain name application ID (org.wahjam.Wahjam)
	APP_ID = $$split(ORGDOMAIN, .)
	APP_ID = $$reverse(APP_ID)
	APP_ID = $$join(APP_ID, .).$$APPNAME

	exists($${APP_ID}.svg) {
		icon_svg.path = /share/icons/hicolor/scalable/apps
		icon_svg.files = $${APP_ID}.svg
		INSTALLS += icon_svg
	}
	exists($${APP_ID}.png) {
		icon_png.path = /share/icons/hicolor/48x48/apps
		icon_png.files = $${APP_ID}.png
		INSTALLS += icon_png
	}
	exists($${APP_ID}.metainfo.xml) {
		metainfo.path = /share/metainfo
		metainfo.files = $${APP_ID}.metainfo.xml
		INSTALLS += metainfo
	}
	exists($${APP_ID}.desktop) {
		desktop.path = /share/applications
		desktop.files = $${APP_ID}.desktop
		INSTALLS += desktop
	}

	target.path = /bin
	INSTALLS += target
}

TEMPLATE = app
DEPENDPATH += ..
INCLUDEPATH += ..
QT += network widgets

include(../common/libcommon.pri)

QMAKE_CXXFLAGS += -Wno-write-strings
CONFIG += link_pkgconfig
PKGCONFIG += ogg vorbis vorbisenc portaudio-2.0

# portmidi does not use pkg-config
mac {
       INCLUDEPATH += /usr/local/Cellar/portmidi/217/include
       LIBS += -L/usr/local/Cellar/portmidi/217/lib
}
LIBS += -lportmidi

# Add QtKeychain directly because its QT module file does not work
LIBS += -lqt5keychain

# On Ubuntu PortTime is separate from PortMidi
!isEmpty(USE_LIBPORTTIME) {
	LIBS += -lporttime
}

win32 {
	exists($${TARGET}.ico) {
		RC_ICONS = $${TARGET}.ico
	}
}

mac {
	LIBS += -framework Foundation -framework AudioUnit -framework AppKit
	QMAKE_TARGET_BUNDLE_PREFIX = $$split(ORGDOMAIN, .)
	QMAKE_TARGET_BUNDLE_PREFIX = $$reverse(QMAKE_TARGET_BUNDLE_PREFIX)
	QMAKE_TARGET_BUNDLE_PREFIX = $$join(QMAKE_TARGET_BUNDLE_PREFIX, .)
	QMAKE_INFO_PLIST = Info.plist

	exists($${TARGET}.icns) {
		ICON = $${TARGET}.icns
	}
}

# Input
HEADERS += MainWindow.h
HEADERS += ConnectDialog.h
HEADERS += JammrConnectDialog.h
HEADERS += ChannelTreeWidget.h
HEADERS += SettingsDialog.h
HEADERS += PortAudioSettingsPage.h
HEADERS += PortMidiSettingsPage.h
HEADERS += ServerBrowser.h
HEADERS += MetronomeBar.h
HEADERS += ChatOutput.h
HEADERS += AddEffectPluginDialog.h
HEADERS += EffectSettingsPage.h
HEADERS += EffectPluginRoutingDialog.h
HEADERS += JammrLoginDialog.h
HEADERS += JammrServerBrowser.h
HEADERS += JammrAccessControlDialog.h
HEADERS += JammrUpdateChecker.h
HEADERS += NINJAMServerBrowser.h
HEADERS += logging.h
HEADERS += audiostream_pa.h
HEADERS += EffectPlugin.h
HEADERS += VSTPlugin.h
mac:HEADERS += PmEventParser.h
mac:HEADERS += AudioUnitPlugin.h
mac:HEADERS += createuiwidget.h
HEADERS += EffectProcessor.h
HEADERS += PortMidiStreamer.h
HEADERS += screensleep.h
HEADERS += UISettingsPage.h

SOURCES += qtclient.cpp
SOURCES += MainWindow.cpp
SOURCES += ConnectDialog.cpp
SOURCES += JammrConnectDialog.cpp
SOURCES += ChannelTreeWidget.cpp
SOURCES += SettingsDialog.cpp
SOURCES += PortAudioSettingsPage.cpp
SOURCES += PortMidiSettingsPage.cpp
SOURCES += ServerBrowser.cpp
SOURCES += MetronomeBar.cpp
SOURCES += ChatOutput.cpp
SOURCES += AddEffectPluginDialog.cpp
SOURCES += EffectSettingsPage.cpp
SOURCES += EffectPluginRoutingDialog.cpp
SOURCES += JammrLoginDialog.cpp
SOURCES += JammrServerBrowser.cpp
SOURCES += JammrAccessControlDialog.cpp
SOURCES += JammrUpdateChecker.cpp
SOURCES += NINJAMServerBrowser.cpp
SOURCES += logging.cpp
SOURCES += audiostream_pa.cpp
SOURCES += EffectPlugin.cpp
SOURCES += VSTPlugin.cpp
mac:SOURCES += PmEventParser.cpp
mac:SOURCES += AudioUnitPlugin.cpp
mac:OBJECTIVE_SOURCES += createuiwidget.mm
SOURCES += EffectProcessor.cpp
SOURCES += PortMidiStreamer.cpp
win32 {
	SOURCES += screensleep_win32.cpp
} else:mac {
	SOURCES += screensleep_mac.cpp
} else {
	SOURCES += screensleep_stub.cpp
}
SOURCES += UISettingsPage.cpp
