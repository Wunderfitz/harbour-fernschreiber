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
import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Page {
    id: aboutPage
    allowedOrientations: Orientation.All

    property variant userInformation : tdLibWrapper.getUserInformation();

    SilicaFlickable {
        id: aboutContainer
        contentHeight: column.height
        anchors.fill: parent

        Column {
            id: column
            width: aboutPage.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("About Fernschreiber")
            }

            Image {
                id: wunderfitzImage
                source: "../../images/fernschreiber.svg"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                sourceSize {
                    width: width
                    height: height
                }
                fillMode: Image.PreserveAspectFit
                asynchronous: true
                width: Math.min(2 * Theme.itemSizeHuge, Math.min(aboutPage.width, aboutPage.height) / 2)
                height: width
            }

            Label {
                text: "Fernschreiber 0.4"
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeExtraLarge
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("A Telegram client for Sailfish OS")
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                text: qsTr("By Sebastian J. Wolf and <a href=\"https://github.com/Wunderfitz/harbour-fernschreiber#contributions\">other contributors</a>")
                font.pixelSize: Theme.fontSizeSmall
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                linkColor: Theme.highlightColor
                onLinkActivated: Qt.openUrlExternally(link)
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"mailto:sebastian@ygriega.de\">" + qsTr("Send E-Mail") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("mailto:sebastian@ygriega.de")
            }

            Separator {
                width: parent.width
                color: Theme.primaryColor
                horizontalAlignment: Qt.AlignHCenter
            }

            Label {
                text: qsTr("Licensed under GNU GPLv3")
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/Wunderfitz/harbour-fernschreiber\">" + qsTr("Sources on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/Wunderfitz/harbour-fernschreiber")
            }

            SectionHeader {
                text: qsTr("About Telegram")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("This product uses the Telegram API but is not endorsed or certified by Telegram.")
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("TDLib version %1").arg(tdLibWrapper.getVersion())
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Logged in as %1").arg(Emoji.emojify(aboutPage.userInformation.first_name + " " + aboutPage.userInformation.last_name, Theme.fontSizeSmall))
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.Wrap
                color: Theme.primaryColor
                textFormat: Text.StyledText
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            ProfileThumbnail {
                photoData: aboutPage.userInformation.profile_photo.small
                width: Theme.itemSizeExtraLarge
                height: Theme.itemSizeExtraLarge
                replacementStringHint: aboutPage.userInformation.first_name + " " + aboutPage.userInformation.last_name
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Phone number: +%1").arg(aboutPage.userInformation.phone_number)
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Button {
                id: flickrTosButton
                text: qsTr("Terms of Service")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    Qt.openUrlExternally("https://telegram.org/tos");
                }
            }

            Button {
                id: flickrPrivacyButton
                text: qsTr("Privacy Policy")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    Qt.openUrlExternally("https://telegram.org/privacy")
                }
            }

            SectionHeader {
                text: qsTr("Credits")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                text: qsTr("This project uses the Telegram Database Library (TDLib). Thanks for making it available under the conditions of the Boost Software License 1.0!")
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/tdlib/td\">" + qsTr("Open Telegram Database Library on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/tdlib/td")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                text: qsTr("This project uses twemoji. Copyright 2018 Twitter, Inc. and other contributors. Thanks for making it available under the conditions of the MIT License (coding) and CC-BY 4.0 (graphics)!")
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/twitter/twemoji\">" + qsTr("Open twemoji on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/twitter/twemoji")
            }

            Label {
                id: separatorLabel
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            VerticalScrollDecorator {}
        }

    }
}
