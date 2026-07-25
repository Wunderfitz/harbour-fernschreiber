# Build glue for Telegram voice/video calls (tgcalls + tg_owt) on Sailfish OS.
#
# Opt-in: pass CONFIG+=voicecalls to qmake. Only meaningful on aarch64.
# Requires a prebuilt tg_owt (libtg_owt.a) and the tgcalls sources. By default
# these are expected in a sibling "tgcalls-build" directory; override with
# TG_OWT_ROOT / TGCALLS_ROOT on the qmake command line. (A future step should
# vendor tgcalls + tg_owt as git submodules for a reproducible upstream build.)

isEmpty(TGCALLS_ROOT): TGCALLS_ROOT = $$PWD/../tgcalls-build/tgcalls
isEmpty(TG_OWT_ROOT):  TG_OWT_ROOT  = $$PWD/../tgcalls-build/tg_owt_yg
isEmpty(OPENH264_ROOT): OPENH264_ROOT = $$PWD/../tgcalls-build/openh264

TGCALLS_SRC = $$TGCALLS_ROOT/tgcalls
TG_OWT_SRC  = $$TG_OWT_ROOT/src

# Fail early and say what is missing — otherwise a wrong/absent tg_owt only
# surfaces as thousands of unresolved webrtc symbols at link time.
!exists($$TG_OWT_SRC/api/peer_connection_interface.h) {
    error("CONFIG+=voicecalls: no tg_owt sources at $${TG_OWT_ROOT}. Pass TG_OWT_ROOT=<path> to qmake (see doc/voicecalls.md).")
}
!exists($${TG_OWT_ROOT}/out/libtg_owt.a) {
    error("CONFIG+=voicecalls: tg_owt is not built — $${TG_OWT_ROOT}/out/libtg_owt.a is missing (see doc/voicecalls.md).")
}
!exists($$TGCALLS_SRC/InstanceImpl.cpp) {
    error("CONFIG+=voicecalls: no tgcalls sources at $${TGCALLS_ROOT}. Pass TGCALLS_ROOT=<path> to qmake (see doc/voicecalls.md).")
}
!exists($${OPENH264_ROOT}/libopenh264.so.8) {
    error("CONFIG+=voicecalls: libopenh264.so.8 not found in $${OPENH264_ROOT}. Pass OPENH264_ROOT=<path> to qmake (see doc/voicecalls.md).")
}
!equals(TARGET_ARCHITECTURE, aarch64) {
    warning("CONFIG+=voicecalls: only built and tested on aarch64; $${TARGET_ARCHITECTURE} is untested and needs a tg_owt built for it.")
}

# Compiling the vendored webrtc/tgcalls tree needs C++20.
QMAKE_CXXFLAGS += -std=gnu++2a

DEFINES += FERNSCHREIBER_VOIP

# webrtc/tgcalls compile definitions (mirrors tg_owt cmake/libwebrtcbuild.cmake,
# with X11/PipeWire off and dummy audio backend — the app provides audio I/O).
DEFINES += \
    NDEBUG WEBRTC_POSIX WEBRTC_LINUX \
    WEBRTC_ENABLE_PROTOBUF=0 WEBRTC_APM_DEBUG_DUMP=0 WEBRTC_USE_BUILTIN_ISAC_FLOAT \
    WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE WEBRTC_USE_H264 WEBRTC_LIBRARY_IMPL \
    WEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=1 WEBRTC_HAVE_DCSCTP WEBRTC_HAVE_SCTP \
    NO_MAIN_THREAD_WRAPPING HAVE_WEBRTC_VIDEO RTC_ENABLE_H265 RTC_ENABLE_VP9 \
    RTC_DISABLE_TRACE_EVENTS WEBRTC_ENABLE_LINUX_PULSE WEBRTC_ENABLE_LINUX_ALSA \
    TGCALLS_USE_STD_OPTIONAL

INCLUDEPATH += \
    $$PWD/src \
    $$TG_OWT_SRC \
    $$TG_OWT_SRC/third_party/abseil-cpp \
    $$TG_OWT_SRC/third_party/libyuv/include \
    $$TGCALLS_ROOT \
    $$TGCALLS_SRC

# tgcalls sources for 1:1 private calls. We register InstanceImpl (core),
# InstanceV2Impl and InstanceV2ReferenceImpl (v2/). The legacy/ and v2_4_0_0/
# variants target an OLDER webrtc API than tg_owt @3215153 and don't compile
# against it (cricket::Codec API changed); they are unused, so excluded.
# Group-call sources are excluded too (1:1 only for now).
SOURCES += \
    $$files($$TGCALLS_SRC/*.cpp) \
    $$files($$TGCALLS_SRC/v2/*.cpp) \
    $$files($$TGCALLS_SRC/utils/*.cpp) \
    $$files($$TGCALLS_SRC/third-party/*.cpp)

# Our Sailfish tgcalls platform interface, the TDLib<->tgcalls bridge, and the
# camera capture / video rendering pipeline (QtMultimedia <-> WebRTC).
SOURCES += \
    $$PWD/src/voip/sailfishinterface.cpp \
    $$PWD/src/voip/voipmanager.cpp \
    $$PWD/src/voip/callcameragrabber.cpp \
    $$PWD/src/voip/videocapturer.cpp \
    $$PWD/src/voip/videorenderer.cpp \
    $$PWD/src/voip/callaudiorouter.cpp
HEADERS += \
    $$PWD/src/voip/sailfishinterface.h \
    $$PWD/src/voip/voipmanager.h \
    $$PWD/src/voip/callcameragrabber.h \
    $$PWD/src/voip/videocapturer.h \
    $$PWD/src/voip/videorenderer.h \
    $$PWD/src/voip/videotracksource.h \
    $$PWD/src/voip/callaudiorouter.h

QT += multimedia sensors

# Sailfish does not package openh264 (patent reasons) but tg_owt hard-requires it,
# so it has to travel with the app — same treatment libtdjson already gets: dropped
# into the binary's rpath dir and taken out of the RPM's requires via
# __requires_exclude in rpm/harbour-fernschreiber.yaml. Without that the package
# would declare a dependency no Sailfish repository can satisfy, i.e. it would not
# install anywhere.
openh264lib.files = $${OPENH264_ROOT}/libopenh264.so.8
openh264lib.path = /usr/share/$${TARGET}/lib
INSTALLS += openh264lib

LIBS += -L$${TG_OWT_ROOT}/out -ltg_owt \
    -lssl -lcrypto -lopus -lvpx \
    -lavcodec -lavformat -lavutil -lswresample -lswscale \
    -ljpeg -lopenh264 -lpulse -lasound -lpthread -ldl -lm -lz
