/*
    Copyright (C) 2020 Sebastian J. Wolf

    This file is part of Fernschreiber.

    fernschreiber is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    fernschreiber is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Fernschreiber. If not, see <http://www.gnu.org/licenses/>.
*/

#include "tdlibwrapper.h"

TDLibWrapper::TDLibWrapper(QObject *parent) : QObject(parent)
{
    qDebug() << "[TDLibWrapper] Initializing TD Lib...";
    this->tdLibClient = td_json_client_create();
    //this->testIt();
}

TDLibWrapper::~TDLibWrapper()
{
    qDebug() << "[TDLibWrapper] Destroying TD Lib...";
    td_json_client_destroy(this->tdLibClient);
}

void TDLibWrapper::testIt()
{
    qDebug() << "[TDLibWrapper] Test it!";
    td_json_client_send(this->tdLibClient, "BLUBB");
}

