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

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include <QScopedPointer>
#include <QQuickView>
#include <QtQml>
#include <QQmlContext>
#include <QQmlEngine>
#include <QGuiApplication>

#include "appsettings.h"
#include "tdlibfile.h"
#include "tdlibwrapper.h"
#include "chatlistmodel.h"
#include "chatmodel.h"
#include "notificationmanager.h"
#include "dbusadaptor.h"
#include "processlauncher.h"
#include "stickermanager.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    QQmlContext *context = view.data()->rootContext();

    const char *uri = "WerkWolf.Fernschreiber";
    qmlRegisterType<TDLibFile>(uri, 1, 0, "TDLibFile");

    AppSettings *appSettings = new AppSettings(view.data());
    context->setContextProperty("appSettings", appSettings);
    qmlRegisterUncreatableType<AppSettings>(uri, 1, 0, "AppSettings", QString());

    TDLibWrapper *tdLibWrapper = new TDLibWrapper(appSettings, view.data());
    context->setContextProperty("tdLibWrapper", tdLibWrapper);
    qmlRegisterUncreatableType<TDLibWrapper>(uri, 1, 0, "TelegramAPI", QString());

    DBusAdaptor *dBusAdaptor = tdLibWrapper->getDBusAdaptor();
    context->setContextProperty("dBusAdaptor", dBusAdaptor);

    ChatListModel chatListModel(tdLibWrapper);
    context->setContextProperty("chatListModel", &chatListModel);

    ChatModel chatModel(tdLibWrapper);
    context->setContextProperty("chatModel", &chatModel);

    NotificationManager notificationManager(tdLibWrapper, appSettings);
    context->setContextProperty("notificationManager", &notificationManager);

    ProcessLauncher processLauncher;
    context->setContextProperty("processLauncher", &processLauncher);

    StickerManager stickerManager(tdLibWrapper);
    context->setContextProperty("stickerManager", &stickerManager);

    view->setSource(SailfishApp::pathTo("qml/harbour-fernschreiber.qml"));
    view->show();
    return app->exec();
}
