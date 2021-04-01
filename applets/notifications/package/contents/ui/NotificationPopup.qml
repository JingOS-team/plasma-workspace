/*
 * Copyright 2019 Kai Uwe Broulik <kde@privat.broulik.de>
 * Copyright 2021 Rui Wang <wangrui@jingos.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
import QtQuick 2.8
import QtQuick.Layouts 1.1

import org.kde.plasma.core 2.0 as PlasmaCore

import org.kde.notificationmanager 1.0 as NotificationManager

import ".."
import "jingos" as Jingos

PlasmaCore.Dialog {
    id: notificationPopup

    property int popupWidth

    property var notificationType//: notificationItem.notificationType

    property alias applicationName: notificationItem.applicationName
    property alias applicationIconSource: notificationItem.applicationIconSource
    property var originName//: notificationItem.originName

    property string time

    property alias summary: notificationItem.summary
    property alias body: notificationItem.body
    property var icon//: notificationItem.icon
    property var urls//: notificationItem.urls

    property int urgency
    property int timeout
    property int dismissTimeout

    property var jobState//: notificationItem.jobState
    property var percentage//: notificationItem.percentage
    property var jobError//: notificationItem.jobError
    property var suspendable//: notificationItem.suspendable
    property var killable//: notificationItem.killable
    property var jobDetails//: notificationItem.jobDetails

    property var configureActionLabel//: notificationItem.configureActionLabel
    property var configurable//: notificationItem.configurable
    property var dismissable//: notificationItem.dismissable
    property var closable//: notificationItem.closable

    property bool hasDefaultAction
    property var defaultActionFallbackWindowIdx
    property var actionNames//: notificationItem.actionNames
    property var actionLabels//: notificationItem.actionLabels

    property var hasReplyAction//: notificationItem.hasReplyAction
    property var replyActionLabel//: notificationItem.replyActionLabel
    property var replyPlaceholderText//: notificationItem.replyPlaceholderText
    property var replySubmitButtonText//: notificationItem.replySubmitButtonText
    property var replySubmitButtonIconName//: notificationItem.replySubmitButtonIconName

    signal configureClicked
    signal dismissClicked
    signal closeClicked

    signal defaultActionInvoked
    signal actionInvoked(string actionName)
    signal replied(string text)
    signal openUrl(string url)
    signal fileActionInvoked

    signal expired
    signal hoverEntered
    signal hoverExited

    signal suspendJobClicked
    signal resumeJobClicked
    signal killJobClicked

    property int defaultTimeout: 3000
    readonly property int effectiveTimeout: {
        return 3000
        if (timeout === -1) {
            return defaultTimeout
        }
        if (dismissTimeout) {
            return dismissTimeout
        }
        return timeout
    }

    location: PlasmaCore.Types.Floating
    flags: notificationItem.replying ? 0 : Qt.WindowDoesNotAcceptFocus

    visible: false

    backgroundHints: PlasmaCore.Dialog.NoBackground

    // When notification is updated, restart hide timer
    onTimeChanged: {
        if (timer.running) {
            timer.restart()
        }
    }

    mainItem: Item {
        width: notificationPopup.popupWidth + 32
        height: notificationItem.implicitHeight + 32
        // color: "#f0f0f0"
        // clip: true
        // radius: 20

            Timer {
                    id: timer
                    property bool triggerEnd: false
                    interval: notificationPopup.effectiveTimeout
                    running: notificationPopup.visible && !notificationItem.hovered
                             && interval > 0
                    onTriggered: {
                        triggerEnd = true
                        notificationItem.startExit()
                    }
                }

                Jingos.NotificationItem {
                    id: notificationItem
                    x: 16
                    y: 16
                    // let the item bleed into the dialog margins so the close button margins cancel out
                    // headingRightPadding: -notificationPopup.margins.right
                    width: parent.width
                    time: Qt.formatTime(notificationPopup.time,"hh:mm AP")
                    // hovered: area.containsMouse
                    // maximumLineCount: 8
                    // bodyCursorShape: notificationPopup.hasDefaultAction ? Qt.PointingHandCursor : 0

                    // thumbnailLeftPadding: -notificationPopup.margins.left
                    // thumbnailRightPadding: -notificationPopup.margins.right
                    // thumbnailTopPadding: -notificationPopup.margins.top
                    // thumbnailBottomPadding: -notificationPopup.margins.bottom

                    property bool timeout: timer.running ? timer.interval : 0

                    // closable: area.containsMouse
                    // onBodyClicked: {
                        // if (area.acceptedButtons & mouse.button) {
                            // area.clicked(null /*mouse*/
                                        //  )
                        // }
                    // }
                    // onCloseClicked: notificationPopup.closeClicked()
                    // onDismissClicked: notificationPopup.dismissClicked()
                    // onConfigureClicked: notificationPopup.configureClicked()
                    // onActionInvoked: notificationPopup.actionInvoked(actionName)
                    // onReplied: notificationPopup.replied(text)
                    // onOpenUrl: notificationPopup.openUrl(url)
                    // onFileActionInvoked: notificationPopup.fileActionInvoked()

                    // onSuspendJobClicked: notificationPopup.suspendJobClicked()
                    // onResumeJobClicked: notificationPopup.resumeJobClicked()
                    // onKillJobClicked: notificationPopup.killJobClicked()

                    onExitFinished: {
                        if(timer.triggerEnd){
                            if (notificationPopup.dismissTimeout) {
                                notificationPopup.dismissClicked();
                            } else {
                                notificationPopup.expired();
                            }
                        }else{
                            notificationPopup.closeClicked()
                        }
                        
                    }
                }
    }
}
