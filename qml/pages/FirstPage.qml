/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import ru.sashikknox 1.0

Page {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    MultiPointTouchArea {
        anchors.fill:parent
        IrrQuickItem2 {
            id: irrItem
            anchors.fill: parent
            focus: true

            SequentialAnimation on t {
                NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
                NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
                loops: Animation.Infinite
                running: true
            }
        }
    }

    Button {
        text: qsTr("Quake 3 Map")
        anchors {
            margins: Theme.paddingMedium
            left: parent.left
            top: parent.top
        }
        onClicked: irrItem.loadExample(16)
    }

    // To enable PullDownMenu, place our content in a SilicaFlickable
//    SilicaFlickable {
//        anchors.fill: parent

//        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
//        PullDownMenu {
//            MenuItem {
//                text: qsTr("0. Reset Scene")
//                onClicked: {
//                    irrItem.loadExample(0);
//                }
//            }
//            MenuItem {
//                text: qsTr("1. Hello World")
//                onClicked: {
//                    irrItem.loadExample(1);
//                }
//            }
//            MenuItem {
//                text: qsTr("2. Quake 3 Map")
//                onClicked: {
//                    irrItem.loadExample(2);
//                }
//            }
//            MenuItem {
//                text: qsTr("11. Per Pixel Lighting")
//                onClicked: {
//                    irrItem.loadExample(11);
//                }
//            }
//            MenuItem {
//                text: qsTr("16. Quake 3 Map Shader")
//                onClicked: {
//                    irrItem.loadExample(16);
//                }
//            }
//        }

//        // Tell SilicaFlickable the height of its content.
//        contentHeight: column.height

//        // Place our content in a Column.  The PageHeader is always placed at the top
//        // of the page, followed by our content.
//        Column {
//            id: column

//            width: page.width
//            spacing: Theme.paddingLarge
//            PageHeader {
//                title: qsTr("UI Template")
//            }
//            Label {
//                x: Theme.horizontalPageMargin
//                text: qsTr("Hello Sailors")
//                color: Theme.secondaryHighlightColor
//                font.pixelSize: Theme.fontSizeExtraLarge
//            }
//        }
//    }
}

