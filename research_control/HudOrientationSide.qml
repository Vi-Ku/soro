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

import QtQuick 2.0
import QtGraphicalEffects 1.0
import Soro 1.0


Item {
    property bool halfWidth: false;

    property alias rearPitch: impl.rearPitch
    property alias frontPitch: impl.frontPitch
    property alias frontPitchZero: impl.frontPitchZero
    property alias rearPitchZero: impl.rearPitchZero

    width: height
    opacity: 0.8

    transform: Scale { xScale: halfWidth ? 0.5 : 1 }

    HudBackground {
        anchors.fill: parent
    }

    HudOrientationSideImpl {
        id: impl
        anchors.fill: parent
        anchors.margins: 16 + parent.width / 10
    }
}
