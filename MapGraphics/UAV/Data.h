#ifndef DATA_H
#define DATA_H

#include<iostream>

struct Telemetry {

 double lon = 0;
 double lat = 0;
 float altitude = 0.0f;
 float g_speed = 0.0f;
 float dist_to_point = 0.0f;
 float mismatch = 0.0f;
 int8_t pitch = 0;
 int16_t roll = 0;
 int16_t yaw = 0;
 int16_t gps_mismatch = 0;
 uint8_t cal_system = 0;
 uint8_t cal_gyro = 0;
 uint8_t cal_accel = 0;
 uint8_t cal_mag = 0;
};

struct MOVE { //хранит точку мгновенного премещения

    double lon = 0;
    double lat = 0;
    int16_t alt = 0;
    uint8_t climb = 0;
    uint8_t type_point = 0;
    uint8_t radius_point = 0;
    uint8_t radius_circl = 0;
};

struct ROUT { //хранит маршрут полёта

    double lon = 0;
    double lat = 0;
    int16_t alt = 0;
    uint8_t climb = 0;
    uint8_t type_point = 0;
    uint8_t radius_point = 0;
    uint8_t radius_circl = 0;

};

struct PID
{

 float P_Alt = 0.0f;
 float D_Alt = 0.0f;

 float P_Bank = 0.0f;
 float D_Bank = 0.0f;

 float P_Pitch = 0.0f;
 float P_Roll = 0.0f;
 float P_Yaw = 0.0f;

 float D_Pitch = 0.0f;
 float D_Roll = 0.0f;
 float D_Yaw = 0.0f;

};

#endif // DATA_H



