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

namespace {
    const QString KEY_SEND_BY_ENTER("sendByEnter");
    const QString KEY_FOCUS_TEXTAREA_AFTER_SEND("focusTextAreaAfterSend");
    const QString KEY_USE_OPEN_WITH("useOpenWith");
    const QString KEY_SHOW_STICKERS_AS_EMOJIS("showStickersAsEmojis");
    const QString KEY_SHOW_STICKERS_AS_IMAGES("showStickersAsImages");
    const QString KEY_ANIMATE_STICKERS("animateStickers");
    const QString KEY_NOTIFICATION_TURNS_DISPLAY_ON("notificationTurnsDisplayOn");
    const QString KEY_NOTIFICATION_SOUNDS_ENABLED("notificationSoundsEnabled");
    const QString KEY_NOTIFICATION_FEEDBACK("notificationFeedback");
    const QString KEY_STORAGE_OPTIMIZER("useStorageOptimizer");
    const QString KEY_INLINEBOT_LOCATION_ACCESS("allowInlineBotLocationAccess");
    const QString KEY_REMAINING_INTERACTION_HINTS("remainingInteractionHints");
    const QString KEY_ONLINE_ONLY_MODE("onlineOnlyMode");
    const QString KEY_DELAY_MESSAGE_READ("delayMessageRead");
    const QString KEY_FOCUS_TEXTAREA_ON_CHAT_OPEN("focusTextAreaOnChatOpen");
    const QString KEY_SPONSORED_MESS("sponsoredMess");
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

bool AppSettings::getFocusTextAreaAfterSend() const
{
    return settings.value(KEY_FOCUS_TEXTAREA_AFTER_SEND, false).toBool();
}

void AppSettings::setFocusTextAreaAfterSend(bool focusTextAreaAfterSend)
{
    if (getFocusTextAreaAfterSend() != focusTextAreaAfterSend) {
        LOG(KEY_FOCUS_TEXTAREA_AFTER_SEND << focusTextAreaAfterSend);
        settings.setValue(KEY_FOCUS_TEXTAREA_AFTER_SEND, focusTextAreaAfterSend);
        emit focusTextAreaAfterSendChanged();
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

bool AppSettings::showStickersAsEmojis() const
{
    return settings.value(KEY_SHOW_STICKERS_AS_EMOJIS, false).toBool();
}

void AppSettings::setShowStickersAsEmojis(bool showAsEmojis)
{
    if (showStickersAsEmojis() != showAsEmojis) {
        LOG(KEY_SHOW_STICKERS_AS_EMOJIS << showAsEmojis);
        settings.setValue(KEY_SHOW_STICKERS_AS_EMOJIS, showAsEmojis);
        emit showStickersAsEmojisChanged();
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

bool AppSettings::notificationSoundsEnabled() const
{
    return settings.value(KEY_NOTIFICATION_SOUNDS_ENABLED, true).toBool();
}

void AppSettings::setNotificationSoundsEnabled(bool enable)
{
    if (notificationSoundsEnabled() != enable) {
        LOG(KEY_NOTIFICATION_SOUNDS_ENABLED << enable);
        settings.setValue(KEY_NOTIFICATION_SOUNDS_ENABLED, enable);
        emit notificationSoundsEnabledChanged();
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
    return settings.value(KEY_STORAGE_OPTIMIZER, true).toBool();
}

void AppSettings::setStorageOptimizer(bool enable)
{
    if (storageOptimizer() != enable) {
        LOG(KEY_STORAGE_OPTIMIZER << enable);
        settings.setValue(KEY_STORAGE_OPTIMIZER, enable);
        emit storageOptimizerChanged();
    }
}

bool AppSettings::allowInlineBotLocationAccess() const
{
    return settings.value(KEY_INLINEBOT_LOCATION_ACCESS, false).toBool();
}

void AppSettings::setAllowInlineBotLocationAccess(bool enable)
{

    if (allowInlineBotLocationAccess() != enable) {
        LOG(KEY_INLINEBOT_LOCATION_ACCESS << enable);
        settings.setValue(KEY_INLINEBOT_LOCATION_ACCESS, enable);
        emit allowInlineBotLocationAccessChanged();
    }
}

int AppSettings::remainingInteractionHints() const
{
    return settings.value(KEY_REMAINING_INTERACTION_HINTS, 3).toInt();
}

void AppSettings::setRemainingInteractionHints(int remainingHints)
{
    if (remainingInteractionHints() != remainingHints) {
        LOG(KEY_REMAINING_INTERACTION_HINTS << remainingHints);
        settings.setValue(KEY_REMAINING_INTERACTION_HINTS, remainingHints);
        emit remainingInteractionHintsChanged();
    }
}

bool AppSettings::onlineOnlyMode() const
{
    return settings.value(KEY_ONLINE_ONLY_MODE, false).toBool();
}

void AppSettings::setOnlineOnlyMode(bool enable)
{
    if (onlineOnlyMode() != enable) {
        LOG(KEY_ONLINE_ONLY_MODE << enable);
        settings.setValue(KEY_ONLINE_ONLY_MODE, enable);
        emit onlineOnlyModeChanged();
    }
}

bool AppSettings::delayMessageRead() const
{
    return settings.value(KEY_DELAY_MESSAGE_READ, true).toBool();
}

void AppSettings::setDelayMessageRead(bool enable)
{
    if (delayMessageRead() != enable) {
        LOG(KEY_DELAY_MESSAGE_READ << enable);
        settings.setValue(KEY_DELAY_MESSAGE_READ, enable);
        emit delayMessageReadChanged();
    }
}

bool AppSettings::getFocusTextAreaOnChatOpen() const
{
    return settings.value(KEY_FOCUS_TEXTAREA_ON_CHAT_OPEN, false).toBool();
}

void AppSettings::setFocusTextAreaOnChatOpen(bool focusTextAreaOnChatOpen)
{
    if (getFocusTextAreaOnChatOpen() != focusTextAreaOnChatOpen) {
        LOG(KEY_FOCUS_TEXTAREA_ON_CHAT_OPEN << focusTextAreaOnChatOpen);
        settings.setValue(KEY_FOCUS_TEXTAREA_ON_CHAT_OPEN, focusTextAreaOnChatOpen);
        emit focusTextAreaOnChatOpenChanged();
    }
}

AppSettings::SponsoredMess AppSettings::getSponsoredMess() const
{
    return (SponsoredMess) settings.value(KEY_SPONSORED_MESS, (int)
        AppSettings::SponsoredMessHandle).toInt();
}

void AppSettings::setSponsoredMess(SponsoredMess sponsoredMess)
{
    if (getSponsoredMess() != sponsoredMess) {
        LOG(KEY_SPONSORED_MESS << sponsoredMess);
        settings.setValue(KEY_SPONSORED_MESS, sponsoredMess);
        emit sponsoredMessChanged();
    }
}
