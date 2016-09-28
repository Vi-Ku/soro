#include "gamepadmanager.h"

#define LOG_TAG "GamepadManager"

#define MAP_FILE "gamecontrollerdb.txt"

namespace Soro {
namespace MissionControl {

GamepadManager::GamepadManager(QObject *parent) : QObject(parent) { }

bool GamepadManager::init(int interval, QString *error) {
    if (_sdlInitialized) {
        SDL_Quit();
    }
    KILL_TIMER(_inputSelectorTimerId);
    KILL_TIMER(_updateTimerId);
    _interval = interval;
    _sdlInitialized = SDL_Init(SDL_INIT_GAMECONTROLLER) == 0;
    if (_sdlInitialized) {
        QString path = QCoreApplication::applicationDirPath() + "/config/" + MAP_FILE;
        if (SDL_GameControllerAddMappingsFromFile(path.toLocal8Bit().constData()) != -1) {
            START_TIMER(_inputSelectorTimerId, 1000);
            return true;
        }
        else {
            *error = "The gamepad map file is missing or invalid. You can obtain this file from https://github.com/gabomdq/SDL_GameControllerDB";
        }
    }
    else {
        *error = "SDL failed to initialize: " +QString(SDL_GetError());
    }
    return false;
}

GamepadManager::~GamepadManager() {
    SDL_Quit();
}

SDL_GameController* GamepadManager::getGamepad() {
    return _gameController;
}

QString GamepadManager::getGamepadName() const {
    return _gamepadName;
}

bool GamepadManager::isGamepadConnected() const {
    return _gameController != NULL;
}

void GamepadManager::timerEvent(QTimerEvent *e) {
    QObject::timerEvent(e);
    if (e->timerId() == _inputSelectorTimerId) {
        /***************************************
         * This timer querys SDL at regular intervals to look for a
         * suitable controller
         */
        SDL_GameControllerUpdate();
        for (int i = 0; i < SDL_NumJoysticks(); ++i) {
            if (SDL_IsGameController(i)) {
                SDL_GameController *controller = SDL_GameControllerOpen(i);
                if (controller && SDL_GameControllerMapping(controller)) {
                    //this gamepad will do
                    setGamepad(controller);
                    KILL_TIMER(_inputSelectorTimerId);
                    START_TIMER(_updateTimerId, _interval);
                    break;
                }
                SDL_GameControllerClose(controller);
            }
        }
    }
    else if (e->timerId() == _updateTimerId) {
        SDL_GameControllerUpdate();
        if (_gameController && SDL_GameControllerGetAttached(_gameController)) {
            // Update gamepad data
            axisLeftX           = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_LEFTX);
            axisLeftY           = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_LEFTY);
            axisRightX          = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_RIGHTX);
            axisRightY          = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_RIGHTY);
            axisLeftTrigger     = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
            axisRightTrigger    = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
            buttonA             = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_A);
            buttonB             = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_B);
            buttonX             = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_X);
            buttonY             = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_Y);
            buttonLeftShoulder  = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
            buttonRightShoulder = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
            buttonStart         = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_START);
            buttonBack          = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_BACK);
            buttonLeftStick     = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_LEFTSTICK);
            buttonRightStick    = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
            dpadUp              = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_DPAD_UP);
            dpadLeft            = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
            dpadDown            = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
            dpadRight           = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        }
        else {
            // Controller is no longer attached
            setGamepad(NULL);
            KILL_TIMER(_updateTimerId);
            START_TIMER(_inputSelectorTimerId, 1000);
            LOG_I(LOG_TAG, "The gamepad has been disconnected");
        }
    }
}

void GamepadManager::setGamepad(SDL_GameController *controller) {
    if (_gameController != controller) {
        _gameController = controller;
        _gamepadName = _gameController ? QString(SDL_GameControllerName(_gameController)) : "";
        LOG_I(LOG_TAG, "Active controller is \'" + _gamepadName + "\'");
        emit gamepadChanged(_gameController, _gamepadName);
    }
}

}
}