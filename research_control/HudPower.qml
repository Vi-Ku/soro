/*
 * Copyright 2017 The University of Oklahoma.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import QtQuick 2.4
import QtGraphicalEffects 1.0
import QtGraphicalEffects 1.0
import Soro 1.0

Item {
    property bool halfWidth: false
    property alias wheelFLPower: impl.wheelFLPower
    property alias wheelFRPower: impl.wheelFRPower
    property alias wheelMLPower: impl.wheelMLPower
    property alias wheelMRPower: impl.wheelMRPower
    property alias wheelBLPower: impl.wheelBLPower
    property alias wheelBRPower: impl.wheelBRPower

    width: height * 0.8
    height: 300
    opacity: 0.8

    transform: Scale { xScale: halfWidth ? 0.5 : 1 }

    HudBackground {
        id: background
        anchors.fill: parent
    }

    HudPowerImpl {
        id: impl
        anchors.fill: parent
        anchors.margins: 16 + parent.width / 10
    }
}
