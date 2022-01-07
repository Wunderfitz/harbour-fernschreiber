/*
    Copyright (C) 2020 Sebastian J. Wolf and other contributors

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

#include "stickermanager.h"
#include <QListIterator>

#define DEBUG_MODULE StickerManager
#include "debuglog.h"

StickerManager::StickerManager(TDLibWrapper *tdLibWrapper, QObject *parent) : QObject(parent)
{
    LOG("Initializing...");
    this->tdLibWrapper = tdLibWrapper;
    this->reloadNeeded = false;

    connect(this->tdLibWrapper, SIGNAL(recentStickersUpdated(QVariantList)), this, SLOT(handleRecentStickersUpdated(QVariantList)));
    connect(this->tdLibWrapper, SIGNAL(stickersReceived(QVariantList)), this, SLOT(handleStickersReceived(QVariantList)));
    connect(this->tdLibWrapper, SIGNAL(installedStickerSetsUpdated(QVariantList)), this, SLOT(handleInstalledStickerSetsUpdated(QVariantList)));
    connect(this->tdLibWrapper, SIGNAL(stickerSetsReceived(QVariantList)), this, SLOT(handleStickerSetsReceived(QVariantList)));
    connect(this->tdLibWrapper, SIGNAL(stickerSetReceived(QVariantMap)), this, SLOT(handleStickerSetReceived(QVariantMap)));
}

StickerManager::~StickerManager()
{
    LOG("Destroying myself...");
}

QVariantList StickerManager::getRecentStickers()
{
    return this->recentStickers;
}

QVariantList StickerManager::getInstalledStickerSets()
{
    return this->installedStickerSets;
}

QVariantMap StickerManager::getStickerSet(const QString &stickerSetId)
{
    return this->stickerSets.value(stickerSetId).toMap();
}

bool StickerManager::hasStickerSet(const QString &stickerSetId)
{
    return this->stickerSets.contains(stickerSetId);
}

bool StickerManager::isStickerSetInstalled(const QString &stickerSetId)
{
    return this->installedStickerSetIds.contains(stickerSetId);
}

bool StickerManager::needsReload()
{
    return this->reloadNeeded;
}

void StickerManager::setNeedsReload(const bool &reloadNeeded)
{
    this->reloadNeeded = reloadNeeded;
}

void StickerManager::handleRecentStickersUpdated(const QVariantList &stickerIds)
{
    LOG("Receiving recent stickers...." << stickerIds);
    this->recentStickerIds = stickerIds;
}

void StickerManager::handleStickersReceived(const QVariantList &stickers)
{
    LOG("Receiving stickers....");
    QListIterator<QVariant> stickersIterator(stickers);
    while (stickersIterator.hasNext()) {
        QVariantMap newSticker = stickersIterator.next().toMap();
        this->stickers.insert(newSticker.value("sticker").toMap().value("id").toString(), newSticker);
    }

    this->recentStickers.clear();
    QListIterator<QVariant> stickerIdIterator(this->recentStickerIds);
    while (stickerIdIterator.hasNext()) {
        QString stickerId = stickerIdIterator.next().toString();
        this->recentStickers.append(this->stickers.value(stickerId));
    }
}

void StickerManager::handleInstalledStickerSetsUpdated(const QVariantList &stickerSetIds)
{
    LOG("Receiving installed sticker IDs...." << stickerSetIds);
    this->installedStickerSetIds = stickerSetIds;
}

void StickerManager::handleStickerSetsReceived(const QVariantList &stickerSets)
{
    LOG("Receiving sticker sets....");
    QListIterator<QVariant> stickerSetsIterator(stickerSets);
    while (stickerSetsIterator.hasNext()) {
        QVariantMap newStickerSet = stickerSetsIterator.next().toMap();
        QString newSetId = newStickerSet.value("id").toString();
        bool isInstalled = newStickerSet.value("is_installed").toBool();
        if (isInstalled && !this->installedStickerSetIds.contains(newSetId)) {
            this->installedStickerSetIds.append(newSetId);
        }
        if (!isInstalled && this->installedStickerSetIds.contains(newSetId)) {
            this->installedStickerSetIds.removeAll(newSetId);
        }
        if (!this->stickerSets.contains(newSetId)) {
            this->stickerSets.insert(newSetId, newStickerSet);
        }
    }

    this->installedStickerSets.clear();
    QListIterator<QVariant> stickerSetIdIterator(this->installedStickerSetIds);
    int i = 0;
    this->stickerSetMap.clear();
    while (stickerSetIdIterator.hasNext()) {
        QString stickerSetId = stickerSetIdIterator.next().toString();
        if (this->stickerSets.contains(stickerSetId)) {
            this->installedStickerSets.append(this->stickerSets.value(stickerSetId));
            this->stickerSetMap.insert(stickerSetId, i);
            i++;
        }
    }
    emit stickerSetsReceived();
}

void StickerManager::handleStickerSetReceived(const QVariantMap &stickerSet)
{
    QString stickerSetId = stickerSet.value("id").toString();
    this->stickerSets.insert(stickerSetId, stickerSet);
    if (this->installedStickerSetIds.contains(stickerSetId)) {
        LOG("Receiving installed sticker set...." << stickerSetId);
        int setIndex = this->stickerSetMap.value(stickerSetId).toInt();
        this->installedStickerSets.replace(setIndex, stickerSet);
    } else {
        LOG("Receiving new sticker set...." << stickerSetId);
    }
    QVariantList stickerList = stickerSet.value("stickers").toList();
    QListIterator<QVariant> stickerIterator(stickerList);
    while (stickerIterator.hasNext()) {
        QVariantMap singleSticker = stickerIterator.next().toMap();
        QVariantMap thumbnailFile = singleSticker.value("thumbnail").toMap().value("file").toMap();
        QVariantMap thumbnailLocalFile = thumbnailFile.value("local").toMap();
        if (!thumbnailLocalFile.value("is_downloading_completed").toBool()) {
            tdLibWrapper->downloadFile(thumbnailFile.value("id").toInt());
        }
    }
}
