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
#ifndef PHOTOLIMITSWORKER_H
#define PHOTOLIMITSWORKER_H

#include <QThread>
#include <QStringList>

// Telegram only takes a picture as a photo up to 10 MB, with width and height
// adding up to at most 10000 and a ratio of at most 20. Reading the sizes
// means opening every file, so this happens off the UI thread.
class PhotoLimitsWorker : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE {
        performCheck();
    }
public:
    explicit PhotoLimitsWorker(const QStringList &filePaths, QObject *parent = nullptr);

signals:
    void checkCompleted(const QStringList &filePaths, bool limitsExceeded);

private:
    QStringList filePaths;

    void performCheck();
};

#endif // PHOTOLIMITSWORKER_H
