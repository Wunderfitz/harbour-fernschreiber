/*
    Copyright (C) 2020 Slava Monich at al.

    This file is part of Fernschreiber.

    Fernschreiber is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Fernschreiber is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Fernschreiber. If not, see <http://www.gnu.org/licenses/>.
*/

#include "tdlibfile.h"
#include <QDebug>

#define LOG(x) qDebug() << "[TDLibFile]" << x

namespace {
    const QString ID("id");
    const QString EXPECTED_SIZE("expected_size");
    const QString SIZE("size");
    const QString LOCAL("local");
    const QString CAN_BE_DELETED("can_be_deleted");
    const QString CAN_BE_DOWNLOADED("can_be_downloaded");
    const QString DOWNLOAD_OFFSET("download_offset");
    const QString DOWNLOADED_PREFIX_SIZE("downloaded_prefix_size");
    const QString DOWNLOADED_SIZE("downloaded_size");
    const QString IS_DOWNLODING_ACTIVE("is_downloading_active");
    const QString IS_DOWNLODING_COMPELETED("is_downloading_completed");
    const QString PATH("path");
    const QString REMOTE("remote");
    const QString IS_UPLOADING_ACTIVE("is_uploading_active");
    const QString IS_UPLOADING_COMPLETED("is_uploading_completed");
    const QString UNIQUE_ID("unique_id");
    const QString UPLOADED_SIZE("uploaded_size");

    const QString _TYPE("@type");
    const QString TYPE_FILE("file");
    const QString TYPE_LOCAL_FILE("localFile");
    const QString TYPE_REMOTE_FILE("remoteFile");
}

// s(SignalName,signalName)
#define QUEUED_SIGNALS(s) \
    s(TdLib,tdlib) \
    s(FileInfo,fileInfo) \
    s(AutoLoad,autoLoad) \
    s(Id,id) \
    s(ExpectedSize,expectedSize) \
    s(Size,size) \
    s(Path,path) \
    s(DownloadOffset,downloadOffset) \
    s(DownloadedPrefixSize,downloadedPrefixSize) \
    s(DownloadedSize,downloadedSize) \
    s(CanBeDeleted,canBeDeleted) \
    s(CanBeDownloaded,canBeDownloaded) \
    s(DownloadingActive,downloadingActive) \
    s(DownloadingCompleted,downloadingCompleted) \
    s(RemoteId,remoteId) \
    s(UniqueId,uniqueId) \
    s(UploadedSize,uploadedSize) \
    s(UploadingActive,uploadingActive) \
    s(UploadingCompleted,uploadingCompleted)

typedef void (TDLibFile::*TDLibFileSignalEmitter)();
enum TDLibFileSignal {
    #define SIGNAL_ENUM_(Name,name) Signal##Name##Changed,
    QUEUED_SIGNALS(SIGNAL_ENUM_)
    #undef SIGNAL_ENUM_
    SignalCount
};

TDLibFile::TDLibFile()
{
    init();
}

TDLibFile::TDLibFile(TDLibWrapper *tdlib)
{
    init();
    updateTDLibWrapper(tdlib);
    // Reset queued signals
    firstQueuedSignal = SignalCount;
    queuedSignals = 0;
}

TDLibFile::TDLibFile(TDLibWrapper *tdlib, const QVariantMap &fileInfo)
{
    init();
    updateTDLibWrapper(tdlib);
    updateFileInfo(fileInfo);
    // Reset queued signals
    firstQueuedSignal = SignalCount;
    queuedSignals = 0;
}

void TDLibFile::init()
{
    tdLibWrapper = Q_NULLPTR;
    queuedSignals = 0;
    firstQueuedSignal = SignalCount;
    autoLoad = false;
    id = 0;
    expected_size = 0;
    size = 0;
    download_offset = 0;
    downloaded_prefix_size = 0;
    downloaded_size = 0;
    can_be_deleted = false;
    can_be_downloaded = false;
    is_downloading_active = false;
    is_downloading_completed = false;
    uploaded_size = 0;
    is_uploading_active = false;
    is_uploading_completed = false;
}

void TDLibFile::queueSignal(uint signal)
{
    const uint signalBit = 1 << signal;
    if (queuedSignals) {
        queuedSignals |= signalBit;
        if (firstQueuedSignal > signal) {
            firstQueuedSignal = signal;
        }
    } else {
        queuedSignals = signalBit;
        firstQueuedSignal = signal;
    }
}

void TDLibFile::emitQueuedSignals()
{
    static const TDLibFileSignalEmitter emitSignal[] = {
        #define SIGNAL_EMITTER_(Name,name) &TDLibFile::name##Changed,
        QUEUED_SIGNALS(SIGNAL_EMITTER_)
        #undef SIGNAL_EMITTER_
    };
    if (queuedSignals) {
        // Reset first queued signal before emitting the signals.
        // Signal handlers may emit new signals.
        uint i = firstQueuedSignal;
        firstQueuedSignal = SignalCount;
        for (; i < SignalCount && queuedSignals; i++) {
            const uint signalBit = 1 << i;
            if (queuedSignals & signalBit) {
                queuedSignals &= ~signalBit;
                emit (this->*(emitSignal[i]))();
            }
        }
    }
}

void TDLibFile::setTDLibWrapper(QObject* obj)
{
    updateTDLibWrapper(qobject_cast<TDLibWrapper*>(obj));
    emitQueuedSignals();
}

void TDLibFile::updateTDLibWrapper(TDLibWrapper* tdlib)
{
    if (tdlib != tdLibWrapper) {
        if (tdLibWrapper) {
            tdLibWrapper->disconnect(this);
        }
        tdLibWrapper = tdlib;
        if (tdlib) {
            connect(tdlib, SIGNAL(fileUpdated(int,QVariantMap)), SLOT(handleFileUpdated(int,QVariantMap)));
            if (autoLoad) {
                load();
            }
        }
        queueSignal(SignalTdLibChanged);
    }
}

void TDLibFile::setAutoLoad(bool enableAutoLoad)
{
    if (autoLoad != enableAutoLoad) {
        autoLoad = enableAutoLoad;
        queueSignal(SignalAutoLoadChanged);
        if (autoLoad) {
            load();
        }
        emitQueuedSignals();
    }
}

bool TDLibFile::load()
{
    if (id && tdLibWrapper && !is_downloading_active && !is_downloading_completed && can_be_downloaded) {
        tdLibWrapper->downloadFile(id);
        return true;
    }
    return false;
}

void TDLibFile::setFileInfo(const QVariantMap &fileInfo)
{
    updateFileInfo(fileInfo);
    if (autoLoad) {
        load();
    }
    emitQueuedSignals();
}

void TDLibFile::handleFileUpdated(int fileId, const QVariantMap &fileInfo)
{
    if (id == fileId) {
        LOG("File" << fileId << "updated");
        setFileInfo(fileInfo);
        emitQueuedSignals();
    }
}

/*
{
    "@type": "file",
    "expected_size": 0,
    "id": 12,
    "size": 0,
    "local": {
        "@type": "localFile",
        "can_be_deleted": false,
        "can_be_downloaded": true,
        "download_offset": 0,
        "downloaded_prefix_size": 0,
        "downloaded_size": 0,
        "is_downloading_active": true,
        "is_downloading_completed": false,
        "path": ""
    },
    "remote": {
        "@type": "remoteFile",
        "id": "AQADAgATXYBrly4AAwIAA2E2itkW____9UdPDiSysLQ4fwIAARgE",
        "is_uploading_active": false,
        "is_uploading_completed": true,
        "unique_id": "AQADXYBrly4AAzh_AgAB",
        "uploaded_size": 0
    }
}
*/

void TDLibFile::updateFileInfo(const QVariantMap &file)
{
    if (file.value(_TYPE).toString() == TYPE_FILE) {
        QString s;
        qlonglong ll;
        bool b, fileChanged = false;
        int i = file.value(ID).toInt();
        if (id != i) {
            id = i;
            fileChanged = true;
            queueSignal(SignalIdChanged);
        }
        ll = file.value(EXPECTED_SIZE).toLongLong();
        if (expected_size != ll) {
            expected_size = ll;
            fileChanged = true;
            queueSignal(SignalExpectedSizeChanged);
        }
        ll = file.value(SIZE).toLongLong();
        if (size != ll) {
            size = ll;
            fileChanged = true;
            queueSignal(SignalSizeChanged);
        }

        const QVariantMap local(file.value(LOCAL).toMap());
        if (local.value(_TYPE).toString() == TYPE_LOCAL_FILE) {
            ll = local.value(DOWNLOAD_OFFSET).toLongLong();
            if (download_offset != ll) {
                download_offset = ll;
                fileChanged = true;
                queueSignal(SignalDownloadOffsetChanged);
            }
            ll = local.value(DOWNLOADED_PREFIX_SIZE).toLongLong();
            if (downloaded_prefix_size != ll) {
                downloaded_prefix_size = ll;
                fileChanged = true;
                queueSignal(SignalDownloadedPrefixSizeChanged);
            }
            ll = local.value(DOWNLOADED_SIZE).toLongLong();
            if (downloaded_size != ll) {
                downloaded_size = ll;
                fileChanged = true;
                queueSignal(SignalDownloadedSizeChanged);
            }
            b = local.value(CAN_BE_DELETED).toBool();
            if (can_be_deleted != b) {
                can_be_deleted = b;
                fileChanged = true;
                queueSignal(SignalCanBeDeletedChanged);
            }
            b = local.value(CAN_BE_DOWNLOADED).toBool();
            if (can_be_downloaded != b) {
                can_be_downloaded = b;
                fileChanged = true;
                queueSignal(SignalCanBeDownloadedChanged);
            }
            b = local.value(IS_DOWNLODING_ACTIVE).toBool();
            if (is_downloading_active != b) {
                is_downloading_active = b;
                fileChanged = true;
                queueSignal(SignalDownloadingActiveChanged);
            }
            b = local.value(IS_DOWNLODING_COMPELETED).toBool();
            if (is_downloading_completed != b) {
                is_downloading_completed = b;
                fileChanged = true;
                queueSignal(SignalDownloadingCompletedChanged);
            }
            s = local.value(PATH).toString();
            if (path != s) {
                path = s;
                fileChanged = true;
                queueSignal(SignalPathChanged);
            }
        }

        const QVariantMap remote(file.value(REMOTE).toMap());
        if (remote.value(_TYPE).toString() == TYPE_REMOTE_FILE) {
            ll = file.value(UPLOADED_SIZE).toLongLong();
            if (uploaded_size != ll) {
                uploaded_size = ll;
                fileChanged = true;
                queueSignal(SignalUploadedSizeChanged);
            }
            b = file.value(IS_UPLOADING_ACTIVE).toBool();
            if (is_uploading_active != b) {
                is_uploading_active = b;
                fileChanged = true;
                queueSignal(SignalUploadingActiveChanged);
            }
            b = file.value(IS_UPLOADING_COMPLETED).toBool();
            if (is_uploading_completed != b) {
                is_uploading_completed = b;
                fileChanged = true;
                queueSignal(SignalUploadingCompletedChanged);
            }
            s = local.value(ID).toString();
            if (remote_id != s) {
                remote_id = s;
                fileChanged = true;
                queueSignal(SignalRemoteIdChanged);
            }
            s = local.value(UNIQUE_ID).toString();
            if (unique_id != s) {
                unique_id = s;
                fileChanged = true;
                queueSignal(SignalUniqueIdChanged);
            }
        }

        if (fileChanged) {
            infoMap = file;
            queueSignal(SignalFileInfoChanged);
        }
    }
}
