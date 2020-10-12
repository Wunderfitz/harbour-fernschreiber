/*
    Copyright (C) 2020 Sebastian J. Wolf

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

#ifndef STICKERMANAGER_H
#define STICKERMANAGER_H

#include <QObject>
#include <QVariantMap>
#include <QVariantList>

#include "tdlibwrapper.h"

class StickerManager : public QObject
{
    Q_OBJECT
public:
    explicit StickerManager(TDLibWrapper *tdLibWrapper, QObject *parent = nullptr);
    ~StickerManager();

    Q_INVOKABLE QVariantList getRecentStickers();
    Q_INVOKABLE QVariantList getInstalledStickerSets();

signals:

private slots:

    void handleRecentStickersUpdated(const QVariantList &stickerIds);
    void handleStickersReceived(const QVariantList &stickers);
    void handleInstalledStickerSetsUpdated(const QVariantList &stickerSetIds);
    void handleStickerSetsReceived(const QVariantList &stickerSets);

private:

    TDLibWrapper *tdLibWrapper;

    QVariantList recentStickers;
    QVariantList recentStickerIds;
    QVariantList installedStickerSets;
    QVariantList installedStickerSetIds;
    QVariantMap stickers;
    QVariantMap stickerSets;

};

#endif // STICKERMANAGER_H
