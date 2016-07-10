#ifndef SORO_MISSIONCONTROL_GAMEPADMANAGER_H
#define SORO_MISSIONCONTROL_GAMEPADMANAGER_H

#include <QObject>
#include <QTimerEvent>

#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>

#include "soro_global.h"
#include "logger.h"

namespace Soro {
namespace MissionControl {

/* Manages gamepad input for the application
 */
class GamepadManager : public QObject {
    Q_OBJECT
public:
    explicit GamepadManager(QObject *parent = 0);
    ~GamepadManager();

    bool init(int interval, QString *error);

    /* Gets the currently connected gamepad, or null if no gamepad is connected.
     */
    SDL_GameController* getGamepad();

    /* Gets the name of the currently connected gamepad, or an empty string
     * if no gamepad is connected.
     */
    QString getGamepadName() const;

    /* Returns true if a gamepad is current connected and being polled
     */
    bool isGamepadConnected() const;

    qint16 axisLeftX = 0, axisLeftY = 0, axisRightX = 0, axisRightY = 0,
        axisLeftTrigger = 0, axisRightTrigger = 0;
    bool buttonA = false, buttonB = false, buttonX = false, buttonY = false,
        buttonLeftShoulder = false, buttonRightShoulder = false,
        buttonLeftStick = false, buttonRightStick = false,
        buttonBack = false, buttonStart = false;

    bool dpadUp = false, dpadLeft = false, dpadRight = false, dpadDown = false;

signals:
    void gamepadChanged(SDL_GameController *controller, QString name);

protected:
    void timerEvent(QTimerEvent *event);

private:
    bool _sdlInitialized = false;
    QString _gamepadName;
    bool _initialized = false;
    int _interval;
    SDL_GameController *_gameController = NULL;
    int _inputSelectorTimerId = TIMER_INACTIVE;
    int _updateTimerId = TIMER_INACTIVE;

    void setGamepad(SDL_GameController *controller);

public:
};

}
}

#endif // SORO_MISSIONCONTROL_GAMEPADMANAGER_H
