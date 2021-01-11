/*
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

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>

class AppSettings : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool sendByEnter READ getSendByEnter WRITE setSendByEnter NOTIFY sendByEnterChanged)
    Q_PROPERTY(bool focusTextAreaAfterSend READ getFocusTextAreaAfterSend WRITE setFocusTextAreaAfterSend NOTIFY focusTextAreaAfterSendChanged)
    Q_PROPERTY(bool useOpenWith READ getUseOpenWith WRITE setUseOpenWith NOTIFY useOpenWithChanged)
    Q_PROPERTY(bool showStickersAsImages READ showStickersAsImages WRITE setShowStickersAsImages NOTIFY showStickersAsImagesChanged)
    Q_PROPERTY(bool animateStickers READ animateStickers WRITE setAnimateStickers NOTIFY animateStickersChanged)
    Q_PROPERTY(bool notificationTurnsDisplayOn READ notificationTurnsDisplayOn WRITE setNotificationTurnsDisplayOn NOTIFY notificationTurnsDisplayOnChanged)
    Q_PROPERTY(NotificationFeedback notificationFeedback READ notificationFeedback WRITE setNotificationFeedback NOTIFY notificationFeedbackChanged)
    Q_PROPERTY(bool storageOptimizer READ storageOptimizer WRITE setStorageOptimizer NOTIFY storageOptimizerChanged)
    Q_PROPERTY(int remainingInteractionHints READ remainingInteractionHints WRITE setRemainingInteractionHints NOTIFY remainingInteractionHintsChanged)
    Q_PROPERTY(bool onlineOnlyMode READ onlineOnlyMode WRITE setOnlineOnlyMode NOTIFY onlineOnlyModeChanged)

public:
    enum NotificationFeedback {
        NotificationFeedbackNone,
        NotificationFeedbackNew,
        NotificationFeedbackAll
    };
    Q_ENUM(NotificationFeedback)

public:
    AppSettings(QObject *parent = Q_NULLPTR);

    bool getSendByEnter() const;
    void setSendByEnter(bool sendByEnter);

    bool getFocusTextAreaAfterSend() const;
    void setFocusTextAreaAfterSend(bool focusTextAreaAfterSend);

    bool getUseOpenWith() const;
    void setUseOpenWith(bool useOpenWith);

    bool showStickersAsImages() const;
    void setShowStickersAsImages(bool showAsImages);

    bool animateStickers() const;
    void setAnimateStickers(bool animate);

    bool notificationTurnsDisplayOn() const;
    void setNotificationTurnsDisplayOn(bool turnOn);

    NotificationFeedback notificationFeedback() const;
    void setNotificationFeedback(NotificationFeedback feedback);

    bool storageOptimizer() const;
    void setStorageOptimizer(bool enable);

    int remainingInteractionHints() const;
    void setRemainingInteractionHints(int remainingHints);

    bool onlineOnlyMode() const;
    void setOnlineOnlyMode(bool enable);

signals:
    void sendByEnterChanged();
    void focusTextAreaAfterSendChanged();
    void useOpenWithChanged();
    void showStickersAsImagesChanged();
    void animateStickersChanged();
    void notificationTurnsDisplayOnChanged();
    void notificationFeedbackChanged();
    void storageOptimizerChanged();
    void remainingInteractionHintsChanged();
    void onlineOnlyModeChanged();

private:
    QSettings settings;
};

#endif // APPSETTINGS_H
