#include "uart_due.h"
#include <wiringSerial.h>
#include <errno.h>
#include <QDebug>

UART_Protokol::UART_Protokol(){}

void UART_Protokol::StartProtokol()

{
    if ((fd = serialOpen ("/dev/ttyS1", 115200)) < 0) qDebug()<<"Ошибка открытия порта"<<strerror (errno) ;

    startTimer(50);

    sp_ResetAll();     // Полный сброс протокола

    qDebug()<<"Serial запущен";
}


void UART_Protokol::timerEvent(QTimerEvent*)
{
    if (!init) init_count ++;
    if (!init && init_count > 100) { init_count = 0; sp_Send("Start"); }

    if(serialDataAvail (fd))
   {      
      sp_Read(); // Вызов «читалки» принятых данных

      if(sp_packetAvailable)             // Если после вызова «читалки» пакет полностью принят
      {
       ParseCommand();                   // Обрабатываем принятую информацию
       sp_ResetAll();                    // Полный сброс протокола.

      }
   }
}

void UART_Protokol::sp_Reset()
{
    sp_count = 0;                   // Сброс длинны пакета
    sp_startMarkerStatus = 0;      // Сброс флага маркера начала пакета
    sp_stopMarkerStatus = 0;      // Сброс флага маркера конца пакета
    sp_dataLength = 0;           // Сброс флага принимаемых данных
}


void UART_Protokol::sp_ResetAll()
{
    for (uint16_t i = 0; i < 255; i++) sp_dataString[i] = 0;    // Обнуляем буфер приема данных (баг в цикле for нельзя использовать uint8_t и int8_t)
    sp_Reset();                                                       // Частичный сброс протокола
    sp_packetAvailable = false;                                      // Сброс флага завершения приема пакета
    data_length = 0;
}

void UART_Protokol::sp_Read()
{

  while(serialDataAvail(fd) && !sp_packetAvailable)
    {

      uint8_t bufferChar = serialGetchar(fd);

      if( sp_startMarkerStatus < sp_MarkerLength )
      {
       if(sp_startMarker[sp_startMarkerStatus] == bufferChar)
       {
         sp_startMarkerStatus++;
       }
       else
       {

         sp_ResetAll();
       }
      }
      else
      {
       // Стартовый маркер прочитан полностью
         if(sp_dataLength == 0)
         {
            sp_dataLength = bufferChar;
         }
         else
        {
          if(sp_dataLength > sp_count)
          {
            sp_dataString[sp_count] = bufferChar;
            sp_count++;
          }
          else
          {
            if( sp_stopMarkerStatus < sp_MarkerLength )
            {
              if(sp_stopMarker[sp_stopMarkerStatus] == bufferChar)
              {
                sp_stopMarkerStatus++;

                if( sp_stopMarkerStatus == sp_MarkerLength )
                {
                  data_length = sp_dataLength;
                  sp_Reset();
                  sp_packetAvailable = true;
                }
              }
              else
              {
                sp_ResetAll();
              }
            }
          }
        }
      }
    }
}

void UART_Protokol::sp_Send(QByteArray data)
{
  serialPuts(fd, "");                              // Изменяем лог напряжения
  serialPuts(fd, sp_startMarker);                  // Отправляем маркер начала пакета
  serialPutchar(fd, data.length());                // Отправляем длину передаваемых данных
  serialPuts_2 (fd, data, data.length());          // Отправляем сами данные
  serialPuts(fd, sp_stopMarker);                   // Отправляем маркер конца пакета
}

void UART_Protokol::ParseCommand()
{

  if( sp_dataString[0] == 'S' && sp_dataString[1] == 't' && sp_dataString[2] == 'a' && sp_dataString[3] == 'r' && sp_dataString[4] == 't' ) { emit ServerWrite ("UART_OK_1"); sp_Send("Start");  qDebug()<<"Самопроверка"; }
  if( sp_dataString[0] == 'r' && sp_dataString[1] == 'e' && sp_dataString[2] == 'a' && sp_dataString[3] == 'd' && sp_dataString[4] == 'y' ) { init = true; emit ServerWrite ("UART_OK_2");  qDebug()<<"Самопроверка"; }

  if (sp_dataString[0] == 'M' && sp_dataString[1] == 'G' && sp_dataString[2] == '1' )  {  emit ServerWrite ("MG1");     }
  if (sp_dataString[0] == 'M' && sp_dataString[1] == 'G' && sp_dataString[2] == '0' )  {  emit ServerWrite ("MG0");     }
  if (sp_dataString[0] == 'A' && sp_dataString[1] == 'C' && sp_dataString[2] == '1' )  {  emit ServerWrite ("AC1");     }
  if (sp_dataString[0] == 'A' && sp_dataString[1] == 'C' && sp_dataString[2] == '0' )  {  emit ServerWrite ("AC0");     }
  if (sp_dataString[0] == 'R' && sp_dataString[1] == 'C' && sp_dataString[2] == '1' )  {  emit ServerWrite ("RC1");     }
  if (sp_dataString[0] == 'S' && sp_dataString[1] == 'V' && sp_dataString[2] == '1' )  {  emit ServerWrite ("SV1");     }
  if (sp_dataString[0] == 'D' && sp_dataString[1] == 'L' && sp_dataString[2] == '1' )  {  emit ServerWrite ("DL1");     }
  if (sp_dataString[0] == 'D' && sp_dataString[1] == 'L' && sp_dataString[2] == '2' )  {  emit ServerWrite ("DL2");     }
  if (sp_dataString[0] == 'E' && sp_dataString[1] == 'R' && sp_dataString[2] == '1' )  {  emit ServerWrite ("ER1");     }


  if ( sp_dataString[0] == 'T' && sp_dataString[1] == 'E' && sp_dataString[2] == 'L' && sp_dataString[3] == 'E' ) //Телеметрия
  {
      uint8_t ee = 4; QByteArray data; for( uint16_t count = data_length - 4; count ; --count ) data += sp_dataString[ee++]; emit Telemetriy (data);
  }

  if ( sp_dataString[0] == 'P' && sp_dataString[1] == 'I' && sp_dataString[2] == 'D') //Настройки ПИД
  {
      uint8_t ee = 0; QByteArray data; for( uint16_t count = data_length; count ; --count ) data += sp_dataString[ee++]; emit ServerWrite (data);
  }

  if ( sp_dataString[0] == 'L' && sp_dataString[1] == 'I' && sp_dataString[2] == 'M') //Настройки ПИД
  {
    uint8_t ee = 0; QByteArray data; for( uint16_t count = data_length; count ; --count ) data += sp_dataString[ee++];  emit ServerWrite (data);
  }

  if ( sp_dataString[0] == 'T' && sp_dataString[1] == 'R' && sp_dataString[2] == 'I' && sp_dataString[3] == 'M') //Настройки ПИД
  {
      uint8_t ee = 0; QByteArray data; for( uint16_t count = data_length; count ; --count ) data += sp_dataString[ee++]; emit ServerWrite (data);
  }
}
