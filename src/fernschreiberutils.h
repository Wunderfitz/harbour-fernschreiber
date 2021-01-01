/*
    Copyright (C) 2020-21 Sebastian J. Wolf and other contributors

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

#ifndef FERNSCHREIBERUTILS_H
#define FERNSCHREIBERUTILS_H

#include <QObject>
#include <QAudioRecorder>
#include "tdlibwrapper.h"

class FernschreiberUtils : public QObject
{
    Q_OBJECT
public:
    explicit FernschreiberUtils(QObject *parent = nullptr);

    enum VoiceNoteRecordingState {
        Unavailable,
        Stopped,
        Starting,
        Recording,
        Stopping
    };
    Q_ENUM(VoiceNoteRecordingState)

    static QString getMessageShortText(TDLibWrapper *tdLibWrapper, const QVariantMap &messageContent, const bool isChannel, const qlonglong currentUserId, const QVariantMap &messageSender);
    static QString getUserName(const QVariantMap &userInformation);

    Q_INVOKABLE void startRecordingVoiceNote();
    Q_INVOKABLE void stopRecordingVoiceNote();
    Q_INVOKABLE QString voiceNotePath();
    Q_INVOKABLE VoiceNoteRecordingState getVoiceNoteRecordingState();

signals:
    void voiceNoteDurationChanged(qlonglong duration);
    void voiceNoteRecordingStateChanged(VoiceNoteRecordingState state);

private slots:
    void handleAudioRecorderStatusChanged(QMediaRecorder::Status status);

private:
    QAudioRecorder audioRecorder;
    VoiceNoteRecordingState voiceNoteRecordingState;

};

#endif // FERNSCHREIBERUTILS_H
