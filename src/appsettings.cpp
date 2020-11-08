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

#include "appsettings.h"
#include <QDebug>

#define LOG(x) qDebug() << "[AppSettings]" << x

namespace {
    const QString KEY_SEND_BY_ENTER("sendByEnter");
    const QString KEY_USE_OPEN_WITH("useOpenWith");
    const QString KEY_SHOW_STICKERS_AS_IMAGES("showStickersAsImages");
    const QString KEY_NOTIFICATION_FEEDBACK("notificationFeedback");
}

AppSettings::AppSettings(QObject *parent) : QObject(parent), settings("harbour-fernschreiber", "settings")
{
}

bool AppSettings::getSendByEnter() const
{
    return settings.value(KEY_SEND_BY_ENTER, false).toBool();
}

void AppSettings::setSendByEnter(bool sendByEnter)
{
    if (getSendByEnter() != sendByEnter) {
        LOG(KEY_SEND_BY_ENTER << sendByEnter);
        settings.setValue(KEY_SEND_BY_ENTER, sendByEnter);
        emit sendByEnterChanged();
    }
}

bool AppSettings::getUseOpenWith() const
{
    return settings.value(KEY_USE_OPEN_WITH, true).toBool();
}

void AppSettings::setUseOpenWith(bool useOpenWith)
{
    if (getUseOpenWith() != useOpenWith) {
        LOG(KEY_USE_OPEN_WITH << useOpenWith);
        settings.setValue(KEY_USE_OPEN_WITH, useOpenWith);
        emit useOpenWithChanged();
    }
}

bool AppSettings::showStickersAsImages() const
{
    return settings.value(KEY_SHOW_STICKERS_AS_IMAGES, true).toBool();
}

void AppSettings::setShowStickersAsImages(bool showAsImages)
{
    if (showStickersAsImages() != showAsImages) {
        LOG(KEY_SHOW_STICKERS_AS_IMAGES << showAsImages);
        settings.setValue(KEY_SHOW_STICKERS_AS_IMAGES, showAsImages);
        emit showStickersAsImagesChanged();
    }
}

AppSettings::NotificationFeedback AppSettings::notificationFeedback() const
{
    return (NotificationFeedback) settings.value(KEY_NOTIFICATION_FEEDBACK, (int) NotificationFeedbackAll).toInt();
}

void AppSettings::setNotificationFeedback(NotificationFeedback feedback)
{
    if (notificationFeedback() != feedback) {
        LOG(KEY_NOTIFICATION_FEEDBACK << feedback);
        settings.setValue(KEY_NOTIFICATION_FEEDBACK, (int) feedback);
        emit notificationFeedbackChanged();
    }
}
