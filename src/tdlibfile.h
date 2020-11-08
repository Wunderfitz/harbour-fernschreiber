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

#ifndef TDLIBFILE_H
#define TDLIBFILE_H

#include "tdlibwrapper.h"

class TDLibFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* tdlib READ getTDLibWrapper WRITE setTDLibWrapper NOTIFY tdlibChanged)
    Q_PROPERTY(QVariantMap fileInformation READ getFileInfo WRITE setFileInfo NOTIFY fileInfoChanged)
    Q_PROPERTY(bool autoLoad READ isAutoLoad WRITE setAutoLoad NOTIFY autoLoadChanged)
    Q_PROPERTY(int fileId READ getId NOTIFY idChanged)
    Q_PROPERTY(qlonglong expectedSize READ getExpectedSize NOTIFY expectedSizeChanged)
    Q_PROPERTY(qlonglong size READ getSize NOTIFY sizeChanged)
    Q_PROPERTY(QString path READ getPath NOTIFY pathChanged)
    Q_PROPERTY(qlonglong downloadOffset READ getDownloadOffset NOTIFY downloadOffsetChanged)
    Q_PROPERTY(qlonglong downloadedPrefixSize READ getDownloadedPrefixSize NOTIFY downloadedPrefixSizeChanged)
    Q_PROPERTY(qlonglong downloadedSize READ getDownloadedSize NOTIFY downloadedSizeChanged)
    Q_PROPERTY(bool canBeDeleted READ canBeDeleted NOTIFY canBeDeletedChanged)
    Q_PROPERTY(bool canBeDownloaded READ canBeDownloaded NOTIFY canBeDownloadedChanged)
    Q_PROPERTY(bool isDownloadingActive READ isDownloadingActive NOTIFY downloadingActiveChanged)
    Q_PROPERTY(bool isDownloadingCompleted READ isDownloadingCompleted NOTIFY downloadingCompletedChanged)
    Q_PROPERTY(QString remoteId READ getRemoteId NOTIFY remoteIdChanged)
    Q_PROPERTY(QString uniqueId READ getUniqueId NOTIFY uniqueIdChanged)
    Q_PROPERTY(qlonglong uploadedSize READ getUploadedSize NOTIFY uploadedSizeChanged)
    Q_PROPERTY(bool isUploadingActive READ isUploadingActive NOTIFY uploadingActiveChanged)
    Q_PROPERTY(bool isUploadingCompleted READ isUploadingCompleted NOTIFY uploadingCompletedChanged)

public:
    TDLibFile();
    TDLibFile(TDLibWrapper *tdlib);
    TDLibFile(TDLibWrapper *tdlib, const QVariantMap &fileInfo);

    TDLibWrapper *getTDLibWrapper() const;
    void setTDLibWrapper(QObject* obj);

    const QVariantMap &getFileInfo() const;
    void setFileInfo(const QVariantMap &fileInfo);

    bool isAutoLoad() const;
    void setAutoLoad(bool autoLoad);

    int getId() const;
    qlonglong getExpectedSize() const;
    qlonglong getSize() const;
    const QString &getPath() const;
    qlonglong getDownloadOffset() const;
    qlonglong getDownloadedPrefixSize() const;
    qlonglong getDownloadedSize() const;
    bool canBeDeleted() const;
    bool canBeDownloaded() const;
    bool isDownloadingActive() const;
    bool isDownloadingCompleted() const;
    const QString &getRemoteId() const;
    const QString &getUniqueId() const;
    qlonglong getUploadedSize() const;
    bool isUploadingActive() const;
    bool isUploadingCompleted() const;

    Q_INVOKABLE bool load();

signals:
    void tdlibChanged();
    void fileInfoChanged();
    void autoLoadChanged();
    void idChanged();
    void expectedSizeChanged();
    void sizeChanged();
    void pathChanged();
    void downloadOffsetChanged();
    void downloadedPrefixSizeChanged();
    void downloadedSizeChanged();
    void canBeDeletedChanged();
    void canBeDownloadedChanged();
    void downloadingActiveChanged();
    void downloadingCompletedChanged();
    void remoteIdChanged();
    void uniqueIdChanged();
    void uploadedSizeChanged();
    void uploadingActiveChanged();
    void uploadingCompletedChanged();

private slots:
    void handleFileUpdated(int fileId, const QVariantMap &fileInfo);

private:
    void init();
    void updateTDLibWrapper(TDLibWrapper* tdlib);
    void updateFileInfo(const QVariantMap &fileInfo);
    void queueSignal(uint signal);
    void emitQueuedSignals();

private:
    TDLibWrapper *tdLibWrapper;
    uint queuedSignals;
    uint firstQueuedSignal;
    bool autoLoad;
    // file
    QVariantMap infoMap;
    int id;
    qlonglong expected_size;
    qlonglong size;
    // localFile
    QString path;
    qlonglong download_offset;
    qlonglong downloaded_prefix_size;
    qlonglong downloaded_size;
    bool can_be_deleted;
    bool can_be_downloaded;
    bool is_downloading_active;
    bool is_downloading_completed;
    // remoteFile
    QString remote_id;
    QString unique_id;
    qlonglong uploaded_size;
    bool is_uploading_active;
    bool is_uploading_completed;
};

inline TDLibWrapper *TDLibFile::getTDLibWrapper() const { return tdLibWrapper; }
inline const QVariantMap &TDLibFile::getFileInfo() const { return infoMap; }
inline bool TDLibFile::isAutoLoad() const { return autoLoad; }
inline int TDLibFile::getId() const { return id; }
inline qlonglong TDLibFile::getExpectedSize() const { return expected_size; }
inline qlonglong TDLibFile::getSize() const { return size; }
inline const QString &TDLibFile::getPath() const { return path; }
inline qlonglong TDLibFile::getDownloadOffset() const { return download_offset; }
inline qlonglong TDLibFile::getDownloadedPrefixSize() const { return downloaded_prefix_size; }
inline qlonglong TDLibFile::getDownloadedSize() const { return downloaded_size; }
inline bool TDLibFile::canBeDeleted() const { return can_be_deleted; }
inline bool TDLibFile::canBeDownloaded() const { return can_be_downloaded; }
inline bool TDLibFile::isDownloadingActive() const { return is_downloading_active; }
inline bool TDLibFile::isDownloadingCompleted() const { return is_downloading_completed; }
inline const QString &TDLibFile::getRemoteId() const { return remote_id; }
inline const QString &TDLibFile::getUniqueId() const { return unique_id; }
inline qlonglong TDLibFile::getUploadedSize() const { return uploaded_size; }
inline bool TDLibFile::isUploadingActive() const { return is_uploading_active; }
inline bool TDLibFile::isUploadingCompleted() const { return is_uploading_completed; }

#endif // TDLIBFILE_H
