# Voice and video calls

Fernschreiber can be built with 1:1 voice and video calls. The feature is
**opt-in at build time** (`qmake CONFIG+=voicecalls`); without that flag nothing
in this document applies and the build is byte-for-byte the one you know.

## Why an extra library is needed

TDLib does call **signalling only** — `createCall` / `acceptCall` /
`discardCall`, the DH key exchange, protocol negotiation and the relay list. It
never produces or consumes a single audio or video frame. The media has to come
from a separate library, and for Telegram that is **tgcalls**, which is built on
Telegram's fork of webrtc.org (**tg_owt**). Both have to be compiled for the
Sailfish target; there are no packages for them.

That is the whole reason this is opt-in rather than always-on: a plain
`mb2 build` must not suddenly require a WebRTC tree.

## What has been built and tested

| | |
|---|---|
| Architecture | **aarch64** for the media stack (the app itself also builds on armv7hl) |
| Builds against | `SailfishOS-4.6.0.13-aarch64` and `SailfishOS-5.0.0.62-aarch64` |
| Tested on | Xperia 10 III, Sailfish OS 5.1.0.11, from a 5.0.0.62 build |
| Tested against | official Telegram clients (Android, Desktop), both call directions |
| Scope | 1:1 calls, voice and video. **No group calls / voice chats.** |

The **app** builds on armv7hl as well — verified, and with `voicecalls` off it is
byte-for-byte the stock build. **Calls** are aarch64-only in practice: nobody has
built the media stack for 32-bit ARM, which would mean compiling both tg_owt and
openh264 for that target. `voip.pri` warns rather than pretending, and
hard-errors with an actionable message when the tg_owt/tgcalls/openh264 trees are
missing or unbuilt.

Even with a tg_owt for armv7hl, expect video to be marginal there: both encoding
(VP8/VP9) and decoding run in software. On a comparable 32-bit device a
GStreamer-based WebRTC client did manage video, but only at poor quality — so
voice would be the realistic target on armv7hl. i486 is untested entirely.

## Dependencies

Built from source, inside the SDK build engine, for the aarch64 target:

| Component | Source | Revision used |
|---|---|---|
| tg_owt | `github.com/Michal-Szczepaniak/tg_owt` | `3215153` ("Add -fPIC") |
| tgcalls | `github.com/Michal-Szczepaniak/tgcalls` | `8a5b4eb` ("Fix for sfos") |
| openh264 | `github.com/cisco/openh264` | v2.6.0 |

Those two forks are the ones used by
[RooTelegram](https://github.com/RootGPT-YouTube/RooTelegram-SailfishOS) and
Yottagram; they carry the Sailfish/Halium fixes that upstream tgcalls lacks.

Taken from the target's own packages: openssl, opus, libvpx, ffmpeg
(avcodec/avformat/avutil/swresample/swscale), libjpeg, pulseaudio, alsa-lib —
plus their `-devel` packages in the build engine.

**openh264 is different.** Sailfish packages it nowhere (patent reasons) but
tg_owt hard-requires it, so it has to be built from source *and travel with the
app*: `voip.pri` installs `libopenh264.so.8` into the binary's rpath directory
next to `libtdjson`, and `__requires_exclude` in
`rpm/harbour-fernschreiber.yaml` keeps it out of the RPM's dependencies. Without
that the package would declare a dependency no Sailfish repository can satisfy —
it would not install anywhere. Point `OPENH264_ROOT` at your build if it is not
in the default sibling directory.

**libvpx is deliberately not bundled.** It is a real, packaged Sailfish
dependency, so the RPM requires it by soname (`libvpx.so.9` for the targets
above) and a device of the same release line satisfies that. Mind the mismatch
if the device is newer than your SDK: Sailfish 5.1 carries `libvpx.so.12`, so a
5.0-built package will not resolve there. The clean fix is a target matching the
device, not a second libvpx shipped beside the system's.

## Building

Everything below runs **inside the SDK build engine** so that the toolchain and
sysroot are the target's:

```sh
sfdk -c target=SailfishOS-4.6.0.13-aarch64 build-shell bash
```

### 1. tg_owt

```sh
git clone --recursive https://github.com/Michal-Szczepaniak/tg_owt.git
cd tg_owt && git checkout 3215153 && git submodule update --init --recursive
mkdir out && cd out
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DTG_OWT_PACKAGED_BUILD=ON \
    -DTG_OWT_BUILD_AUDIO_BACKENDS=ON \
    -DTG_OWT_USE_X11=OFF \
    -DTG_OWT_USE_PIPEWIRE=OFF \
    -DTG_OWT_USE_PROTOBUF=OFF
make -j$(nproc)
```

This produces `out/libtg_owt.a` (a static library — it is linked into the app,
nothing extra is installed on the device). X11 and PipeWire are off because
neither exists on the device; the audio backends stay on because tgcalls' own
AudioDeviceModule drives PulseAudio.

### 2. tgcalls

Only a checkout is needed — the sources are compiled as part of Fernschreiber
(see `voip.pri`), so they follow the app's own flags:

```sh
git clone https://github.com/Michal-Szczepaniak/tgcalls.git
cd tgcalls && git checkout 8a5b4eb
```

### 3. Fernschreiber

```sh
mb2 -t SailfishOS-4.6.0.13-aarch64 build -- \
    --with voicecalls \
    --define "tg_owt_root /path/to/tg_owt" \
    --define "tgcalls_root /path/to/tgcalls" \
    --define "openh264_root /path/to/openh264"
```

`--with voicecalls` is a plain rpmbuild conditional; it is declared as
`QMakeOptions` in `rpm/harbour-fernschreiber.yaml`, so the generated spec keeps
it across regeneration. **Without `--with voicecalls` the build is exactly the
one you have today** — the macros expand to nothing.

Both roots default to a sibling `../tgcalls-build/` directory, so the two
`--define`s can be omitted if the trees live next to the Fernschreiber checkout.

Building without RPM packaging (faster while iterating) works too:

```sh
mb2 -t SailfishOS-4.6.0.13-aarch64 qmake CONFIG+=voicecalls
mb2 -t SailfishOS-4.6.0.13-aarch64 make -j$(nproc)
```

## The PulseAudio policy rule

`pulse/harbour-fernschreiber.conf` is installed to
`/etc/pulse/xpolicy.conf.d/` and is **not optional** — without it the remote
party is silent.

tgcalls' WebRTC AudioDeviceModule names its streams `WEBRTC VoiceEngine`. The
Sailfish audio policy sorts unknown streams into the `othermedia` group, which
carries the `cork_stream` flag: as soon as the call's microphone capture starts,
the policy **corks the incoming-audio playback stream**. The rule moves the
WebRTC streams into the telephony `call` group (set_sink, set_source, and
crucially no cork_stream), which is what a real call uses.

**This needs a decision from the maintainer:** the file ships outside the app's
own tree, which a Harbour store build would flag. It may be better placed in a
separate configuration sub-package. The `INSTALLS` entry sits inside the
`voicecalls {}` block in `harbour-fernschreiber.pro`, so a stock build is
unaffected either way.

## Sailjail

The `.desktop` file gains one permission, `Camera`, next to the `Microphone` and
`Audio` permissions that were already there. A missing Sailjail permission fails
*silently* on the device — the app simply behaves as if the hardware were broken.

## Code layout

| Path | Role |
|---|---|
| `src/tdlibwrapper.*`, `src/tdlibreceiver.*` | TDLib call signalling: `createCall`/`acceptCall`/`discardCall`/`sendCallSignalingData`, `updateCall`/`updateNewCallSignalingData`. Deliberately media-library agnostic. |
| `src/voip/voipmanager.*` | The bridge: tgcalls instance lifetime, descriptor (relays, encryption key, protocol), signalling relay, exposed to QML. |
| `src/voip/sailfishinterface.*` | tgcalls `PlatformInterface` for Sailfish: codec factories, video capturer creation. |
| `src/voip/callcameragrabber.*`, `videocapturer.*`, `videotracksource.h` | Camera capture: QtMultimedia → I420 → WebRTC, including device-orientation handling. |
| `src/voip/videorenderer.*` | Incoming/outgoing frames → QML `VideoOutput`. |
| `src/voip/callaudiorouter.*` | Moves the WebRTC playout stream to the call sink and unmutes it, via an in-process libpulse connection (Sailjail blocks an external `pactl`). |
| `qml/components/CallOverlay.qml` | In-call UI: peer, state, SAS emojis, remote video + local PiP, mute/hang up/flip. |

## Known limitations

- 1:1 calls only. The group-call sources of tgcalls are deliberately excluded.
- aarch64 only in practice (see above).
- tg_owt and tgcalls are **not vendored** as submodules; they have to be
  provided out-of-tree. Vendoring them would make the build reproducible for
  everyone and is the obvious next step, but it is a packaging decision for the
  maintainer rather than something to slip into this change.
- The video encoder is software VP8/VP9. Decoding accepts everything the builtin
  factory offers (including H264/H265) so that official clients — which prefer
  H264 — are received reliably.
