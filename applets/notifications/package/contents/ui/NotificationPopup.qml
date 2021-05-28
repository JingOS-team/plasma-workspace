

/*
 * Copyright 2019 Kai Uwe Broulik <kde@privat.broulik.de>
 * Copyright 2021 Bob <pengbo.wu@jingos.com>
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
import QtQuick.Window 2.12
import QtQuick.Layouts 1.1
import QtQml 2.2

import org.kde.plasma.core 2.0 as PlasmaCore

import org.kde.notificationmanager 1.0 as NotificationManager

import ".."
import "jingos" as Jingos

PlasmaCore.Dialog {

    id: notificationPopup

    property int popupWidth

    property var notificationType

    property var applicationName
    property var applicationIconSource
    property var originName

    property string time

    property var summary;
    property var body
    property var icon
    property var urls

    property int urgency
    property int timeout
    property int dismissTimeout

    property var jobState
    property var percentage
    property var jobError
    property var suspendable
    property var killable
    property var jobDetails

    property var configureActionLabel
    property var configurable
    property var dismissable
    property var closable

    property bool hasDefaultAction
    property var defaultActionFallbackWindowIdx
    property var actionNames
    property var actionLabels

    property var hasReplyAction
    property var replyActionLabel
    property var replyPlaceholderText
    property var replySubmitButtonText
    property var replySubmitButtonIconName

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
        if (timeout === -1) {
            return defaultTimeout
        }
        if (dismissTimeout) {
            return dismissTimeout
        }
        return timeout
    }

    location: PlasmaCore.Types.Floating
    
    flags: hasReplyAction ? 0 : Qt.WindowDoesNotAcceptFocus
    visible: false

    backgroundHints: PlasmaCore.Dialog.NoBackground

    onTimeChanged: {
        if (timer.running) {
            timer.restart()
        }
    }

    mainItem: Loader {
        id: notificationLoader
        sourceComponent:  actionNames.length !== 0  ? notificationActionComponent : notificationComponent
        
        Component {

            id: notificationComponent
            Item {
                width: notificationItem.width  + 32
                height: notificationItem.height + 32
 
                Timer {
                    id: timer

                    property bool triggerEnd: false
                    interval: notificationPopup.effectiveTimeout
                    running: notificationPopup.visible && !notificationItem.hovered && interval > 0
                    onTriggered: {
                        triggerEnd = true
                        notificationItem.startExit()
                    }
                }

                Jingos.NotificationItem {
                    id: notificationItem

                    x: 16
                    y: 16

                    time: getLocalTimeString(notificationPopup.time) 
                    property bool timeout: timer.running ? timer.interval : 0
                    applicationName: notificationPopup.applicationName
                    summary: notificationPopup.summary
                    body: notificationPopup.body
                    applicationIconSource: notificationPopup.applicationIconSource
                    
                    onExitFinished: {
                        if(timer.triggerEnd) {
                            if (notificationPopup.dismissTimeout) {
                                notificationPopup.dismissClicked()
                            } else {
                                notificationPopup.expired()
                            }
                        } else {
                            notificationPopup.closeClicked()
                        }
                    }
                }  
            }
        }
        
        Component {
            id: notificationActionComponent

            Item {
                width: notificationItem.width + 32
                height: notificationItem.height + 32

                Timer {
                    id: timer

                    property bool triggerEnd: false
                    interval: notificationPopup.effectiveTimeout
                    running: notificationPopup.visible && !notificationItem.hovered && interval > 0
                    onTriggered: {
                        triggerEnd = true
                        notificationItem.startExit()
                    }
                }
                
                Jingos.NotificationActionItem {
                    id: notificationItem

                    x: 16
                    y: 16
                    property bool timeout: timer.running ? timer.interval : 0
                    applicationName: notificationPopup.applicationName
                    summary: notificationPopup.summary
                    actionNames: notificationPopup.actionNames
                    actionLabels: notificationPopup.actionLabels
                    applicationIconSource: notificationPopup.applicationIconSource
                    body: notificationPopup.body
                    onExitFinished: {
                        if(type !== ""){
                            notificationPopup.actionInvoked(type)
                        } else {
                            if(timer.triggerEnd) {
                                if (notificationPopup.dismissTimeout) {
                                    notificationPopup.dismissClicked();
                                } else {
                                    notificationPopup.expired();
                                }
                            } else {
                                notificationPopup.closeClicked()
                            }
                        }
                    }
                }
            }
        }
    }
}
