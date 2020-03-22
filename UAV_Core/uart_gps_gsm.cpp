#include "uart_gps_gsm.h"
#include <wiringSerial.h>
#include <wiringPi.h>
#include <errno.h>
#include <QDebug>
#include <QThread>

#define scale_c 10000000.0f
#define scale_h 100000.0f

#define boot 6 // Контакт для упревления пином загрузки GSM под номером 22 GP5C3

UART_GPS_GSM::UART_GPS_GSM()
{
  QTimer *timer_gps = new QTimer(this);

  QTimer *timer_gsm = new QTimer(this);

  QTimer *timer_std = new QTimer(this);

  connect(timer_gps, SIGNAL(timeout()),this, SLOT (gps_timer()));

  connect(timer_gsm, SIGNAL(timeout()),this, SLOT (gsm_timer()));

  connect(timer_std, SIGNAL(timeout()),this, SLOT (gsm_update()));

  timer_gps->start(100);

  timer_gsm->start(500);

  timer_std->start(30000);

  wiringPiSetup();

}

void UART_GPS_GSM::translit_to_SI()
{

    si.lat = pvt.lat / scale_c; si.lon = pvt.lon / scale_c; si.heading = pvt.heading / scale_h; si.headingAcc = pvt.headingAcc/ scale_h; si.g_speed = pvt.gSpeed/ 1000.0f;
}

void UART_GPS_GSM::Start_GPS()

{
 if ((fd3 = serialOpen ("/dev/ttyACM0", 115200)) < 0) qDebug()<<"GPS: "<<strerror (errno) ;

 for(uint16_t i = 0; i < sizeof(UBLOX_INIT); i++) { serialPutchar(fd3, UBLOX_INIT[i] );}
}

void UART_GPS_GSM::GPS()
{
    static int fpos = 0;
    static unsigned char checksum[2];
    const int payloadSize = sizeof(NAV_PVT);

    gps_reset = true;

    while ( serialDataAvail(fd3) ) {
    uint8_t c = serialGetchar(fd3);

      if ( fpos < 2 ) {
        if ( c == UBX_HEADER[fpos] )
          fpos++;
        else
          fpos = 0;
      }
      else {
        if ( (fpos-2) < payloadSize )
          ((unsigned char*)(&pvt))[fpos-2] = c;

        fpos++;

        if ( fpos == (payloadSize+2) ) {
            calcChecksum(checksum);
        }
        else if ( fpos == (payloadSize+3) ) {
          if ( c != checksum[0] )
            fpos = 0;
        }
        else if ( fpos == (payloadSize+4) ) {
          fpos = 0;
          if ( c == checksum[1] ) {

              translit_to_SI(); //Первод в систему СИ
              GoToNavPoint(); // Обновление координат движения
              gps_reset = false;
          }
        }
        else if ( fpos > (payloadSize+4) ) {
          fpos = 0;
        }
      }
    }

    if(gps_reset) gps_reset_count ++;
    else gps_reset_count = 0;
    if (gps_reset_count > 3) Start_GPS();
}

void UART_GPS_GSM::calcChecksum(unsigned char* CK) { //Контрольная сумма для структуры
  memset(CK, 0, 2);
  for (int i = 0; i < (int)sizeof(NAV_PVT); i++) {
    CK[0] += ((unsigned char*)(&pvt))[i];
    CK[1] += CK[0];
  }
}

void UART_GPS_GSM::Start_GSM()
{

 if ((fd4 = serialOpen ("/dev/ttyS4", 115200)) < 0) qDebug()<<"GSM_Modem: "<<strerror (errno) ;

 pinMode( boot , OUTPUT);

 serialPuts(fd4, "AT+CPAS\r"); //Проверка состояния готовности

}


void UART_GPS_GSM::GSM() //Приём сообщений от модема
{
     QByteArray data;

     while ( serialDataAvail(fd4) ) data +=  serialGetchar(fd4);

     //qDebug()<<": "<<data;


    if ( data == "\r\nMODEM:STARTUP\r\n") { last = 0; type_client = 0; ini = false;  qDebug()<< "Загрузка GSM..."; Tele_process("GSM_load"); }

    if ( data == "\r\n+PBREADY\r\n") Initializing_GSM(1);

    if ( data == "AT+CLIP=1\r\r\nOK\r\n") Initializing_GSM(2);

    if ( data == "AT+CMGF=1\r\r\nOK\r\n") Initializing_GSM(3);

    if ( data == "AT+CSCS=\"GSM\"\r\r\nOK\r\n") Initializing_GSM(4);

    if ( data ==  "\r\nRING\r\n\r\n+CLIP: \"7918791****\",145,,,\"\",0\r\n" && ini ){ type_client = 1; send_cordinat_SMS(1);} // Звонок

    if ( data ==  "\r\nRING\r\n\r\n+CLIP: \"7918767****\",145,,,\"\",0\r\n" && ini ){ type_client = 2; send_cordinat_SMS(1);} // Звонок

    if ( data == "\r\nNO CARRIER\r\n") { Tele_process("Call"); qDebug()<< "Вызов окончен"; }

    if ( data == "\r\nRING\r\n" ) { last++; Tele_process("Un_call"); qDebug()<< "Неизвесный вызов"; };

    if ( data == "ATH\r\r\nOK\r\n") send_cordinat_SMS(2);

    if ( data == "AT+CMGS=\"+7918791***\"\r\r\n> ") send_cordinat_SMS(3);

    if ( data == "AT+CMGS=\"+7918767****\"\r\r\n> ") send_cordinat_SMS(3);

    if ( data == "AT+CPAS\r\r\n+CPAS: 0\r\n\r\nOK\r\n" || data == "AT+CPAS\r\r\n+CPAS: 3\r\n\r\nOK\r\n" || data == "AT+CPAS\r\r\n+CPAS: 4\r\n\r\nOK\r\n") last = 0;

    if ( data == "AT+CPAS\r\r\n+CPAS: 1\r\n\r\nOK\r\n" ) { last = 0; Tele_process("GSM_no_sim"); qDebug()<< "Симкарта не определена";} //Могут быть проблемы т.к. когда модем не готов нужен сброс
}


void UART_GPS_GSM::Initializing_GSM (const uint8_t step) // Подготовка модема к отправке сообщений и АОН
{

    if ( step == 1 ) { last = 0; serialPuts(fd4, "AT+CLIP=1\r"); }

    if ( step == 2 ) { last = 0; serialPuts(fd4, "AT+CMGF=1\r"); }

    if ( step == 3 ) { last = 0; serialPuts(fd4, "AT+CSCS=\"GSM\"\r" ); }

    if ( step == 4 ) { last = 0; ini = true; Tele_process("GSM_init"); qDebug()<< "Модем в режиме ожидания";}

}

void UART_GPS_GSM::reset(bool res) // Сброс модема
{  
     digitalWrite (boot, LOW);

     if(res){ qDebug()<< "Сброс"; digitalWrite(boot, HIGH);}        //Элемент сброса GSM модема, что-бы не тормозить поток на 0.5с завязан на таймер
}

void UART_GPS_GSM::send_cordinat_SMS ( const uint8_t step ) //Отправка координат
{

   if (step == 1 && type_client > 0) { last = 0; serialPuts(fd4, "ATH\r"); }


   if (step == 2 && type_client == 1) { last = 0;

       QString hed = "AT+CMGS=\"+7918791****\"""\r";

       serialPuts(fd4, hed.toUtf8());
   }

   if (step == 2 && type_client == 2) { last = 0;

       QString hed = "AT+CMGS=\"+7918767****\"""\r";

       serialPuts(fd4, hed.toUtf8());
   }

   if (step == 3 && type_client > 0) { last = 0;

       QString sms = "lon: "+QString("%1").arg(si.lon, 0, 'f', 7 )+" lat: "+ QString("%1").arg(si.lat, 0, 'f', 7 );

       serialPuts(fd4, sms.toUtf8());

       const unsigned char c = 26;

       serialPutchar(fd4, c);

       Tele_process("GSM_send");

       send_cordinat_SMS(4);

       type_client = 0;

       serialPuts(fd4, "AT+CMGD=1,3\r"); //Стирание всех сообщений

       qDebug()<< "Координаты отправлены";
   }
}

void UART_GPS_GSM::gps_timer()
{
  if(serialDataAvail(fd3)) GPS(); //Приём очереддных данных GPS
}

void UART_GPS_GSM::gsm_timer()
{
    if(serialDataAvail(fd4)) GSM(); // Приём очередных данных GSM

    reset(false);
    if (last >= 3) { last = 0; reset(true); } //Сброс модема если спит или не инициализирован
}

void UART_GPS_GSM::gsm_update() //Опрос модема на случай засыпания
{
    serialPuts(fd4, "AT+CPAS\r"); last++;

    if (!ini) last = 3;

    if( current.fly_point != 0 && geoDistance() > 10 && si.g_speed > 3 && -5 > geoBearing() - si.heading < 5 )  _move_.mag_dec = Tele_u.yaw - si.heading;

    //qDebug()<<Tele_u.yaw - si.heading;
}

void UART_GPS_GSM::reciver_point(QByteArray point) //Приём маршрута из GUI карты
{
    if ( point == "dr" ) { if(!array_rout.isEmpty())array_rout.clear(); current.rout_count = 0; }

    if ( point == "st" ) current.fly_point = 0;

    if ( point == "rt" ) current.fly_point = 2;

     if (point [0] == 'M' && point[1] == 'O' && point[2] == 'V' && point[3] == 'E' )

     { uint8_t ee = 4; uint8_t* p = (uint8_t*)(void*)&_move; for( int count = point.length(); count ; --count ) *p++ = point[ee++]; current.fly_point = 1; current.move_count = 0; }


     if (point[0] == 'R' && point[1] == 'O' && point[2] == 'U' && point[3] == 'T' )

     { uint8_t ee = 4; uint8_t* p = (uint8_t*)(void*)&_rout; for( int count = point.length(); count ; --count ) *p++ = point[ee++]; array_rout.push_back(_rout); Tele_process("Reciv"); current.fly_point = 2; }

     UpdateNavPiont(current.fly_point);
}

void UART_GPS_GSM::GoToNavPoint()
{

   if ( current.fly_point != 0 && geoDistance() < current.radius_point ){ // Если расстояние меньше 6м и можно вызывать следующую точку то вызываем, нет циркуляризируемся

   if (current.fly_point == 1) current.move_count++;
   if (current.fly_point == 2) current.rout_count++; UpdateNavPiont(current.fly_point);}

   if (current.fly_point > 0)  Move_line(); //qDebug()<<"Движение"; }

   if (current.fly_point == 0)  Move_cirle(); //qDebug()<<"Циркуляризация"; }
}

void UART_GPS_GSM::Move_line()
{
    _move_.alt = current.alt;
    _move_.azimut = geoBearing();
    _move_.circle = false;
    _move_.climb = current.climb;
    _move_.type_point = current.type_point;


    QByteArray data;

    data += "Move"; //Пришлось модифицировать WiringPi для отправки данных

    uint8_t* p = (uint8_t*)(void*)&_move_; for( int count = sizeof(_move_); count ; --count ) data += *p++;

    azimut (data);
}


void UART_GPS_GSM::Move_cirle() //Вращения самолёта вокруг точки
{
    _move_.alt = current.alt;   // Высота для DUE
    _move_.azimut = (geoBearing() + 90.0f ) + ( geoDistance() - current.radius_circl ); //Радиус вращения вокруг точки
    _move_.circle = true;  // Вращение вокруг точки
    _move_.climb = current.climb;  //Угол атаки
    _move_.type_point = current.type_point; //Тип точки


    QByteArray data;

    data += "Move";

    uint8_t* p = (uint8_t*)(void*)&_move_; for( int count = sizeof(_move_); count ; --count ) data += *p++;

    azimut (data);

}

int UART_GPS_GSM::UpdateNavPiont(int type ) //Получение точек из парсера
{
  if (type == 1 && _move.lon != 0 && _move.lat != 0 && 0 == current.move_count ) // Идти к точке
  {
     current.lon =           _move.lon;
     current.lat =           _move.lat;
     current.alt =           _move.alt;
     current.climb =         _move.climb;
     current.type_point =    _move.type_point;
     current.radius_point =  _move.radius_point;
     current.radius_circl =  _move.radius_circl;



     return current.fly_point = 1;
  }

  if (type == 2 && !array_rout.isEmpty() && array_rout.length() > current.rout_count ) // Идти по маршруту
  {
      qDebug()<<"Точка: "<< current.rout_count << " достигнута всего заргружено " << array_rout.length() << " точек";

      uint8_t* p = (uint8_t*)(void*)&array_rout[current.rout_count]; uint8_t* b = (uint8_t*)(void*)&_rout; for( int count = sizeof(_rout); count ; --count ) *b++ = *p++;

      current.lon =            _rout.lon;
      current.lat =            _rout.lat;
      current.climb =          _rout.climb;
      current.alt =            _rout.alt;
      current.type_point =     _rout.type_point;
      current.radius_point =   _rout.radius_point;
      current.radius_circl =   _rout.radius_circl;

      return current.fly_point = 2;
  }

  qDebug()<<current.rout_count;

   return current.fly_point = 0;
}


#ifndef DEGTORAD
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#endif

// http://www.movable-type.co.uk/scripts/latlong.html

float UART_GPS_GSM::geoDistance () {  //Расстояние
  const float R = 6371000; // km
  float p1 = si.lat * DEGTORAD;
  float p2 = current.lat * DEGTORAD;
  float dp = (current.lat- si.lat) * DEGTORAD;
  float dl = (current.lon- si.lon) * DEGTORAD;

  float x = sin(dp/2) * sin(dp/2) + cos(p1) * cos(p2) * sin(dl/2) * sin(dl/2);
  float y = 2 * atan2(sqrt(x), sqrt(1-x));

  return R * y;
}

float UART_GPS_GSM::geoBearing () {  //Азимут на цель
  float y = sin(current.lon-si.lon) * cos(current.lat);
  float x = cos(si.lat)*sin(current.lat) - sin(si.lat)*cos(current.lat)*cos(current.lon - si.lon);
  return atan2(y, x) * RADTODEG;
}



void UART_GPS_GSM::telemetry( QByteArray data)
{ 

    uint8_t ee = 0; uint8_t* p = (uint8_t*)(void*)&Tele_u; for( int count = data.length(); count ; --count ) *p++ = data[ee++];

    data.clear();

    data += "TELE" ;

    float turn_1  = geoBearing() - Tele_u.yaw;

    float turn_2  = geoBearing() - si.heading;

    while (turn_1 < -180) turn_1 += 360;
    while (turn_1 >  180) turn_1 -= 360;

    while (turn_2 < -180) turn_2 += 360;
    while (turn_2 >  180) turn_2 -= 360;

    Tele_w.lon = si.lon;
    Tele_w.lat = si.lat;
    Tele_w.altitude = Tele_u.altitude;
    Tele_w.g_speed = si.g_speed;
    Tele_w.dist_to_point = geoDistance();
    Tele_w.mismatch = turn_1;
    Tele_w.pitch = Tele_u.pitch;
    Tele_w.roll = Tele_u.roll;
    Tele_w.yaw = Tele_u.yaw;
    Tele_w.gps_mismatch = turn_2;
    Tele_w.cal_system = Tele_u.cal_system;
    Tele_w.cal_gyro = Tele_u.cal_gyro;
    Tele_w.cal_accel = Tele_u.cal_accel;
    Tele_w.cal_mag = Tele_u.cal_mag;

    uint8_t* b = (uint8_t*)(void*)&Tele_w; for( int count = sizeof(Tele_w); count ; --count ) data += *b++;

    Tele_process (data);

}
