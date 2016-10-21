/*
 * Copyright 2016 The University of Oklahoma.
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

#include <QCoreApplication>

#include "rover2process.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // set root log output file
    Logger::rootLogger()->setLogfile(QCoreApplication::applicationDirPath()
                                     + "/Rover2_" + QDateTime::currentDateTime().toString("M-dd_h.mm_AP") + ".log");
    Logger::rootLogger()->setMaxFileLevel(Logger::LogLevelDebug);
    Logger::rootLogger()->setMaxQtLoggerLevel(Logger::LogLevelInformation);

    // create secondary rover worker object
    Soro::Rover::Rover2Process worker(&a);

    return a.exec();
}
