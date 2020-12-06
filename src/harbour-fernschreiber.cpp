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
#include <QLoggingCategory>

#include "appsettings.h"
#include "debuglogjs.h"
#include "tdlibfile.h"
#include "tdlibwrapper.h"
#include "chatlistmodel.h"
#include "chatmodel.h"
#include "namedaction.h"
#include "notificationmanager.h"
#include "mceinterface.h"
#include "dbusadaptor.h"
#include "processlauncher.h"
#include "stickermanager.h"
#include "tgsplugin.h"
#include "fernschreiberutils.h"
#include "knownusersmodel.h"
#include "contactsmodel.h"

// The default filter can be overridden by QT_LOGGING_RULES envinronment variable, e.g.
// QT_LOGGING_RULES="fernschreiber.*=true" harbour-fernschreiber
#if defined (QT_DEBUG) || defined(DEBUG)
#  define DEFAULT_LOG_FILTER "fernschreiber.*=true"
#else
#  define DEFAULT_LOG_FILTER "fernschreiber.*=false"
#endif

Q_IMPORT_PLUGIN(TgsIOPlugin)

int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules(DEFAULT_LOG_FILTER);

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    QQmlContext *context = view.data()->rootContext();

    const char *uri = "WerkWolf.Fernschreiber";
    qmlRegisterType<TDLibFile>(uri, 1, 0, "TDLibFile");
    qmlRegisterType<NamedAction>(uri, 1, 0, "NamedAction");
    qmlRegisterSingletonType<DebugLogJS>(uri, 1, 0, "DebugLog", DebugLogJS::createSingleton);

    AppSettings *appSettings = new AppSettings(view.data());
    context->setContextProperty("appSettings", appSettings);
    qmlRegisterUncreatableType<AppSettings>(uri, 1, 0, "AppSettings", QString());

    MceInterface *mceInterface = new MceInterface(view.data());
    TDLibWrapper *tdLibWrapper = new TDLibWrapper(appSettings, mceInterface, view.data());
    context->setContextProperty("tdLibWrapper", tdLibWrapper);
    qmlRegisterUncreatableType<TDLibWrapper>(uri, 1, 0, "TelegramAPI", QString());

    FernschreiberUtils *fernschreiberUtils = new FernschreiberUtils(view.data());
    context->setContextProperty("fernschreiberUtils", fernschreiberUtils);

    DBusAdaptor *dBusAdaptor = tdLibWrapper->getDBusAdaptor();
    context->setContextProperty("dBusAdaptor", dBusAdaptor);

    ChatListModel chatListModel(tdLibWrapper);
    context->setContextProperty("chatListModel", &chatListModel);

    ChatModel chatModel(tdLibWrapper);
    context->setContextProperty("chatModel", &chatModel);

    NotificationManager notificationManager(tdLibWrapper, appSettings, mceInterface, &chatModel);
    context->setContextProperty("notificationManager", &notificationManager);

    ProcessLauncher processLauncher;
    context->setContextProperty("processLauncher", &processLauncher);

    StickerManager stickerManager(tdLibWrapper);
    context->setContextProperty("stickerManager", &stickerManager);

    KnownUsersModel knownUsersModel(tdLibWrapper, view.data());
    context->setContextProperty("knownUsersModel", &knownUsersModel);
    QSortFilterProxyModel knownUsersProxyModel(view.data());
    knownUsersProxyModel.setSourceModel(&knownUsersModel);
    knownUsersProxyModel.setFilterRole(KnownUsersModel::RoleFilter);
    knownUsersProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    context->setContextProperty("knownUsersProxyModel", &knownUsersProxyModel);
    
    ContactsModel contactsModel(tdLibWrapper, view.data());
    context->setContextProperty("contactsModel", &contactsModel);
    QSortFilterProxyModel contactsProxyModel(view.data());
    contactsProxyModel.setSourceModel(&contactsModel);
    contactsProxyModel.setFilterRole(ContactsModel::RoleFilter);
    contactsProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    context->setContextProperty("contactsProxyModel", &contactsProxyModel);

    view->setSource(SailfishApp::pathTo("qml/harbour-fernschreiber.qml"));
    view->show();
    return app->exec();
}
