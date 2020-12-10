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

#define DEBUG_MODULE AppSettings
#include "debuglog.h"

#include <QDBusInterface>
#include <QDBusReply>
#include <QGuiApplication>

namespace {
    const QString KEY_SEND_BY_ENTER("sendByEnter");
    const QString KEY_USE_OPEN_WITH("useOpenWith");
    const QString KEY_STAY_IN_BACKGROUND("stayInBackground");
    const QString KEY_SHOW_STICKERS_AS_IMAGES("showStickersAsImages");
    const QString KEY_ANIMATE_STICKERS("animateStickers");
    const QString KEY_NOTIFICATION_TURNS_DISPLAY_ON("notificationTurnsDisplayOn");
    const QString KEY_NOTIFICATION_FEEDBACK("notificationFeedback");
    const QString KEY_STORAGE_OPTIMIZER("storageOptimizer");
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

bool AppSettings::getStayInBackground() const
{
    return settings.value(KEY_STAY_IN_BACKGROUND, false).toBool();
}

void AppSettings::setStayInBackground(bool stayInBackground)
{
    if (getStayInBackground() != stayInBackground) {
        LOG(KEY_STAY_IN_BACKGROUND << stayInBackground);
        settings.setValue(KEY_STAY_IN_BACKGROUND, stayInBackground);
        QGuiApplication::setQuitOnLastWindowClosed(!stayInBackground);
        emit stayInBackgroundChanged();
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

bool AppSettings::animateStickers() const
{
    return settings.value(KEY_ANIMATE_STICKERS, true).toBool();
}

void AppSettings::setAnimateStickers(bool animate)
{
    if (animateStickers() != animate) {
        LOG(KEY_ANIMATE_STICKERS << animate);
        settings.setValue(KEY_ANIMATE_STICKERS, animate);
        emit animateStickersChanged();
    }
}

bool AppSettings::notificationTurnsDisplayOn() const
{
    return settings.value(KEY_NOTIFICATION_TURNS_DISPLAY_ON, false).toBool();
}

void AppSettings::setNotificationTurnsDisplayOn(bool turnOn)
{
    if (notificationTurnsDisplayOn() != turnOn) {
        LOG(KEY_NOTIFICATION_TURNS_DISPLAY_ON << turnOn);
        settings.setValue(KEY_NOTIFICATION_TURNS_DISPLAY_ON, turnOn);
        emit notificationTurnsDisplayOnChanged();
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

bool AppSettings::storageOptimizer() const
{
    return settings.value(KEY_STORAGE_OPTIMIZER, false).toBool();
}

void AppSettings::setStorageOptimizer(bool enable)
{
    if (storageOptimizer() != enable) {
        LOG(KEY_STORAGE_OPTIMIZER << enable);
        settings.setValue(KEY_STORAGE_OPTIMIZER, enable);
        emit storageOptimizerChanged();
    }
}

bool AppSettings::isAppRunning()
{
    LOG("Checking via D-Bus if app is already running...");
    QDBusInterface dBusInterface("de.ygriega.stayawake", "/de/ygriega/stayawake", "", QDBusConnection::sessionBus());
    if (dBusInterface.isValid()) {
        QDBusReply<bool> reply = dBusInterface.call("showUI");
        if (reply.isValid()) {
            return reply.value();
        }
        LOG("D-Bus call to show UI failed. App doesn't seem to be running (properly)!" << reply.error().message());
        return false;
    } else {
        LOG("Fernschreiber D-Bus session interface is not existing. App doesn't seem to be running!");
        return false;
    }
}
