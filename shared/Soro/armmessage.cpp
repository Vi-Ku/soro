/*********************************************************
 * This code can be compiled on a Qt or mbed enviornment *
 *********************************************************/

#include "armmessage.h"

namespace Soro {
namespace ArmMessage {

#ifdef QT_CORE_LIB

void setGamepadData(char *armMessage, short leftXAxis, short leftYAxis, short rightYAxis,
                           short leftTriggerAxis, short rightTriggerAxis, bool rightButton, bool leftButton, bool yButton) {
    armMessage[0] = reinterpret_cast<const char&>(Header_Gamepad);  //identify this message as gamepad and not master arm
    armMessage[Index_GamepadX] = axisShortToAxisByte(leftXAxis);
    armMessage[Index_GamepadY] = axisShortToAxisByte(rightYAxis);
    armMessage[Index_GamepadYaw] = axisShortToAxisByte(leftYAxis);
    armMessage[Index_GamepadWrist] = axisShortToAxisByte(rightTriggerAxis - leftTriggerAxis);
    armMessage[Index_OpenBucket] = rightButton ? 1 : 0;
    armMessage[Index_CloseBucket] = leftButton ? 1 : 0;
    armMessage[Index_Stow] = yButton ? 1 : 0;
}

void translateMasterArmValues(char *message, const MasterArmConfig& ranges) {
    //yaw
    float yaw = (float)deserialize<unsigned short>(message + Index_MasterYaw) - ranges.yawMin;
    yaw = (float)ranges.yawAdd + yaw * ((float)USHRT_MAX / (float)(ranges.yawMax - ranges.yawMin));
    if (yaw > USHRT_MAX) yaw = USHRT_MAX;
    else if (yaw < 0) yaw = 0;
    if (ranges.yawReverse) yaw = USHRT_MAX - yaw;
    serialize<unsigned short>(message + Index_MasterYaw, (unsigned short)yaw);
    //shoulder
    float shoulder = (float)deserialize<unsigned short>(message + Index_MasterShoulder) - ranges.shoulderMin;
    shoulder = (float)ranges.shoulderAdd + shoulder * ((float)USHRT_MAX / (float)(ranges.shoulderMax - ranges.shoulderMin));
    if (shoulder > USHRT_MAX) shoulder = USHRT_MAX;
    else if (shoulder < 0) shoulder = 0;
    if (ranges.shoulderReverse) shoulder = USHRT_MAX - shoulder;
    serialize<unsigned short>(message + Index_MasterShoulder, (unsigned short)shoulder);
    //elbow
    float elbow = (float)deserialize<unsigned short>(message + Index_MasterElbow) - ranges.elbowMin;
    elbow = (float)ranges.elbowAdd + elbow * ((float)USHRT_MAX / (float)(ranges.elbowMax - ranges.elbowMin));
    if (elbow > USHRT_MAX) elbow = USHRT_MAX;
    else if (elbow < 0) elbow = 0;
    if (ranges.elbowReverse) elbow = USHRT_MAX - elbow;
    serialize<unsigned short>(message + Index_MasterElbow, (unsigned short)elbow);
    //wrist
    float wrist = (float)deserialize<unsigned short>(message + Index_MasterWrist) - ranges.wristMin;
    wrist = (float)ranges.wristAdd + wrist * ((float)USHRT_MAX / (float)(ranges.wristMax - ranges.wristMin));
    if (wrist > USHRT_MAX) wrist = USHRT_MAX;
    else if (wrist < 0) wrist = 0;
    if (ranges.wristReverse) wrist = USHRT_MAX - wrist;
    serialize<unsigned short>(message + Index_MasterWrist, (unsigned short)wrist);
}
#endif
#ifdef TARGET_LPC1768

void setMasterArmData(char *message, unsigned short yaw, unsigned short shoulder,
                                    unsigned short elbow, unsigned short wrist) {
    message[0] = reinterpret_cast<const char&>(Header_Master);  //identify this message as master and not glfw
    serialize<unsigned short>(message + Index_MasterYaw, yaw);
    serialize<unsigned short>(message + Index_MasterShoulder, shoulder);
    serialize<unsigned short>(message + Index_MasterElbow, elbow);
    serialize<unsigned short>(message + Index_MasterWrist, wrist);
}


#endif

}
}