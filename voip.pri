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

# The multimedia stack travels with the app as well, for a different reason:
# SONAMEs move between Sailfish releases. 4.6 and 5.0 carry libvpx.so.9, 5.1 and
# later carry libvpx.so.12, so a package built against one release does not
# resolve on the other and the app dies at startup.
#
# It has to be the whole transitive set rather than libvpx alone: the system
# libavcodec.so.59 links libvpx.so.12, so bundling only libvpx would put two
# libvpx in one process, both exporting vpx_codec_*, and which one wins would
# come down to load order. Bundling libavcodec too keeps the graph resolving
# inside the rpath directory.
#
# The copies are taken from the build target's own sysroot at install time --
# no third-party binaries live in this repository -- and the license texts of
# the very same packages travel with them (see below).
isEmpty(BUNDLED_LIB_DIR):     BUNDLED_LIB_DIR     = /usr/lib64
isEmpty(BUNDLED_LICENSE_DIR): BUNDLED_LICENSE_DIR = /usr/share/licenses

# Transitive closure of libavcodec/libavutil/libswresample/libvpx/libopus,
# excluding glibc and libstdc++. Verify with:
#   readelf -d <lib> | grep NEEDED
BUNDLED_SONAMES = \
    libavcodec.so.59 libavutil.so.57 libswresample.so.4 \
    libvpx.so.9 libopus.so.0 \
    libogg.so.0 libvorbis.so.0 libvorbisenc.so.2 \
    libtheoradec.so.1 libtheoraenc.so.1 \
    libwebp.so.7 libwebpmux.so.3 libsharpyuv.so.0 \
    libopenjp2.so.7 libspeex.so.1

for(soname, BUNDLED_SONAMES) {
    !exists($${BUNDLED_LIB_DIR}/$${soname}) {
        error("CONFIG+=voicecalls: $${soname} not found in $${BUNDLED_LIB_DIR}. The build target does not provide the multimedia stack this build links against (see doc/voicecalls.md).")
    }
    bundledlibs.files += $${BUNDLED_LIB_DIR}/$${soname}
}
bundledlibs.path = /usr/share/$${TARGET}/lib
INSTALLS += bundledlibs

# License texts for the bundled libraries, taken from the same target packages.
# ffmpeg is LGPL-2.1+, the codecs are BSD; both require the text to reach
# whoever receives the RPM. One entry per package, because several of the files
# are called COPYING and would collide in a single directory. The package
# directories carry version numbers, and $$files() cannot glob a directory
# component, so the shell resolves them at qmake time -- and an empty result is
# a hard error, otherwise qmake would drop the entry without a word.
BUNDLED_LICENSE_ROOT = /usr/share/$${TARGET}/licenses/bundled

licffmpeg.files = $$system(ls -1 $${BUNDLED_LICENSE_DIR}/ffmpeg-*/COPYING.LGPLv2.1 2>/dev/null)
licffmpeg.path  = $${BUNDLED_LICENSE_ROOT}/ffmpeg
isEmpty(licffmpeg.files): error("CONFIG+=voicecalls: no license text found at $${BUNDLED_LICENSE_DIR}/ffmpeg-*/COPYING.LGPLv2.1 -- the bundled libraries may not be shipped without it (see doc/voicecalls.md).")
licvpx.files = $$system(ls -1 $${BUNDLED_LICENSE_DIR}/libvpx-*/LICENSE 2>/dev/null)
licvpx.path  = $${BUNDLED_LICENSE_ROOT}/libvpx
isEmpty(licvpx.files): error("CONFIG+=voicecalls: no license text found at $${BUNDLED_LICENSE_DIR}/libvpx-*/LICENSE -- the bundled libraries may not be shipped without it (see doc/voicecalls.md).")
licopus.files = $$system(ls -1 $${BUNDLED_LICENSE_DIR}/opus-*/COPYING 2>/dev/null)
licopus.path  = $${BUNDLED_LICENSE_ROOT}/opus
isEmpty(licopus.files): error("CONFIG+=voicecalls: no license text found at $${BUNDLED_LICENSE_DIR}/opus-*/COPYING -- the bundled libraries may not be shipped without it (see doc/voicecalls.md).")
licogg.files = $$system(ls -1 $${BUNDLED_LICENSE_DIR}/libogg-*/COPYING 2>/dev/null)
licogg.path  = $${BUNDLED_LICENSE_ROOT}/libogg
isEmpty(licogg.files): error("CONFIG+=voicecalls: no license text found at $${BUNDLED_LICENSE_DIR}/libogg-*/COPYING -- the bundled libraries may not be shipped without it (see doc/voicecalls.md).")
licvorbis.files = $$system(ls -1 $${BUNDLED_LICENSE_DIR}/libvorbis-*/COPYING 2>/dev/null)
licvorbis.path  = $${BUNDLED_LICENSE_ROOT}/libvorbis
isEmpty(licvorbis.files): error("CONFIG+=voicecalls: no license text found at $${BUNDLED_LICENSE_DIR}/libvorbis-*/COPYING -- the bundled libraries may not be shipped without it (see doc/voicecalls.md).")
lictheora.files = $$system(ls -1 $${BUNDLED_LICENSE_DIR}/libtheora-*/COPYING 2>/dev/null)
lictheora.path  = $${BUNDLED_LICENSE_ROOT}/libtheora
isEmpty(lictheora.files): error("CONFIG+=voicecalls: no license text found at $${BUNDLED_LICENSE_DIR}/libtheora-*/COPYING -- the bundled libraries may not be shipped without it (see doc/voicecalls.md).")
licwebp.files = $$system(ls -1 $${BUNDLED_LICENSE_DIR}/libwebp-*/COPYING 2>/dev/null)
licwebp.path  = $${BUNDLED_LICENSE_ROOT}/libwebp
isEmpty(licwebp.files): error("CONFIG+=voicecalls: no license text found at $${BUNDLED_LICENSE_DIR}/libwebp-*/COPYING -- the bundled libraries may not be shipped without it (see doc/voicecalls.md).")
licopenjpeg.files = $$system(ls -1 $${BUNDLED_LICENSE_DIR}/openjpeg-*/LICENSE 2>/dev/null)
licopenjpeg.path  = $${BUNDLED_LICENSE_ROOT}/openjpeg
isEmpty(licopenjpeg.files): error("CONFIG+=voicecalls: no license text found at $${BUNDLED_LICENSE_DIR}/openjpeg-*/LICENSE -- the bundled libraries may not be shipped without it (see doc/voicecalls.md).")
licspeex.files = $$system(ls -1 $${BUNDLED_LICENSE_DIR}/speex-*/COPYING 2>/dev/null)
licspeex.path  = $${BUNDLED_LICENSE_ROOT}/speex
isEmpty(licspeex.files): error("CONFIG+=voicecalls: no license text found at $${BUNDLED_LICENSE_DIR}/speex-*/COPYING -- the bundled libraries may not be shipped without it (see doc/voicecalls.md).")

INSTALLS += licffmpeg licvpx licopus licogg licvorbis lictheora licwebp licopenjpeg licspeex


LIBS += -L$${TG_OWT_ROOT}/out -ltg_owt \
    -lssl -lcrypto -lopus -lvpx \
    -lavcodec -lavformat -lavutil -lswresample -lswscale \
    -ljpeg -lopenh264 -lpulse -lasound -lpthread -ldl -lm -lz
