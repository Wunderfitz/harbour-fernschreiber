/*
    Copyright (C) 2026 Sebastian J. Wolf and other contributors

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
#include "photolimitsworker.h"
#include <QFileInfo>
#include <QImageReader>
#include <QSize>

#define DEBUG_MODULE PhotoLimitsWorker
#include "debuglog.h"

namespace {
    // https://core.telegram.org/tdlib/docs/classtd_1_1td__api_1_1input_message_photo.html
    const qint64 MAX_PHOTO_FILE_SIZE = 10 * 1024 * 1024;
    const int MAX_PHOTO_DIMENSIONS_SUM = 10000;
    const int MAX_PHOTO_RATIO = 20;
}

PhotoLimitsWorker::PhotoLimitsWorker(const QStringList &filePaths, QObject *parent) : QThread(parent)
{
    this->filePaths = filePaths;
}

void PhotoLimitsWorker::performCheck()
{
    bool limitsExceeded = false;
    for (const QString &filePath : filePaths) {
        if (isInterruptionRequested()) {
            // Replaced by another check, this one doesn't count any more
            return;
        }
        if (QFileInfo(filePath).size() > MAX_PHOTO_FILE_SIZE) {
            LOG("Too large to be sent as photo" << filePath);
            limitsExceeded = true;
            break;
        }
        // Only reads the header, the picture itself isn't decoded. Neither the
        // sum nor the ratio depend on the orientation, so it is left as it is.
        const QSize size = QImageReader(filePath).size();
        if (!size.isValid() || size.isEmpty()) {
            // Not known here, TDLib will tell
            continue;
        }
        const int longerSide = qMax(size.width(), size.height());
        const int shorterSide = qMin(size.width(), size.height());
        if (size.width() + size.height() > MAX_PHOTO_DIMENSIONS_SUM || longerSide > MAX_PHOTO_RATIO * shorterSide) {
            LOG("Dimensions not accepted for a photo" << filePath << size);
            limitsExceeded = true;
            break;
        }
    }
    emit checkCompleted(filePaths, limitsExceeded);
}
