#-------------------------------------------------
#
# Project created by QtCreator 2017-03-06T09:25:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_obs
TEMPLATE = app

DESTDIR  = ./debug/bin/32bit

include(./obs/obs.pri)
INCLUDEPATH += ./UI
INCLUDEPATH += ./UI/obs-frontend-api

HEADERS += \
    UI/adv-audio-control.hpp \
    UI/audio-encoders.hpp \
    UI/crash-report.hpp \
    UI/display-helpers.hpp \
    UI/double-slider.hpp \
    UI/focus-list.hpp \
    UI/hotkey-edit.hpp \
    UI/item-widget-helpers.hpp \
    UI/menu-button.hpp \
    UI/mute-checkbox.hpp \
    UI/obs-app.hpp \
    UI/platform.hpp \
    UI/properties-view.hpp \
    UI/properties-view.moc.hpp \
    UI/qt-display.hpp \
    UI/qt-wrappers.hpp \
    UI/remote-text.hpp \
    UI/slider-absoluteset-style.hpp \
    UI/source-label.hpp \
    UI/source-list-widget.hpp \
    UI/vertical-scroll-area.hpp \
    UI/visibility-checkbox.hpp \
    UI/visibility-item-widget.hpp \
    UI/volume-control.hpp \
    UI/window-basic-adv-audio.hpp \
    UI/window-basic-filters.hpp \
    UI/window-basic-interaction.hpp \
    UI/window-basic-main.hpp \
    UI/window-basic-main-outputs.hpp \
    UI/window-basic-preview.hpp \
    UI/window-basic-properties.hpp \
    UI/window-basic-settings.hpp \
    UI/window-basic-source-select.hpp \
    UI/window-basic-status-bar.hpp \
    UI/window-basic-transform.hpp \
    UI/window-license-agreement.hpp \
    UI/window-log-reply.hpp \
    UI/window-main.hpp \
    UI/window-namedialog.hpp \
    UI/window-projector.hpp \
    UI/window-remux.hpp \
    cobspublic.h

SOURCES += \
    UI/adv-audio-control.cpp \
    UI/api-interface.cpp \
    UI/audio-encoders.cpp \
    UI/crash-report.cpp \
    UI/double-slider.cpp \
    UI/focus-list.cpp \
    UI/hotkey-edit.cpp \
    UI/item-widget-helpers.cpp \
    UI/menu-button.cpp \
    UI/obs-app.cpp \
    UI/platform-windows.cpp \
#    UI/platform-x11.cpp \
    UI/properties-view.cpp \
    UI/qt-display.cpp \
    UI/qt-wrappers.cpp \
    UI/remote-text.cpp \
    UI/slider-absoluteset-style.cpp \
    UI/source-label.cpp \
    UI/source-list-widget.cpp \
    UI/vertical-scroll-area.cpp \
    UI/visibility-checkbox.cpp \
    UI/visibility-item-widget.cpp \
    UI/volume-control.cpp \
    UI/window-basic-adv-audio.cpp \
    UI/window-basic-filters.cpp \
    UI/window-basic-interaction.cpp \
    UI/window-basic-main.cpp \
    UI/window-basic-main-dropfiles.cpp \
    UI/window-basic-main-outputs.cpp \
    UI/window-basic-main-profiles.cpp \
    UI/window-basic-main-scene-collections.cpp \
    UI/window-basic-main-transitions.cpp \
    UI/window-basic-preview.cpp \
    UI/window-basic-properties.cpp \
    UI/window-basic-settings.cpp \
    UI/window-basic-source-select.cpp \
    UI/window-basic-status-bar.cpp \
    UI/window-basic-transform.cpp \
    UI/window-license-agreement.cpp \
    UI/window-log-reply.cpp \
    UI/window-namedialog.cpp \
    UI/window-projector.cpp \
    UI/window-remux.cpp

RESOURCES += \
    UI/forms/obs.qrc

FORMS += \
    UI/forms/NameDialog.ui \
    UI/forms/OBSBasic.ui \
    UI/forms/OBSBasicFilters.ui \
    UI/forms/OBSBasicInteraction.ui \
    UI/forms/OBSBasicSettings.ui \
    UI/forms/OBSBasicSourceSelect.ui \
    UI/forms/OBSBasicTransform.ui \
    UI/forms/OBSLicenseAgreement.ui \
    UI/forms/OBSLogReply.ui \
    UI/forms/OBSRemux.ui

DISTFILES += \
    UI/obs.rc
