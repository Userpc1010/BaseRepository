#ifndef UART_DUE_H
#define UART_DUE_H
#include <QObject>
#include <QTimerEvent>

class UART_Protokol : public QObject
{

    Q_OBJECT


public:

    UART_Protokol();

private:



int fd;

const char sp_startMarker [5] = "<@#>";    // Переменная, содержащая маркер начала пакета (0 окончание строки)
const char sp_stopMarker  [5] = "<&^>";    // Переменная, содержащая маркер конца пакета
const char sp_moveMarker  [5] = "Move";
const uint8_t sp_MarkerLength = 4;            // Переменная длинны маркера -1
unsigned char sp_dataString [255];            // Здесь будут храниться принимаемые данные
uint8_t sp_startMarkerStatus;                 // Флаг состояния маркера начала пакета
uint8_t sp_stopMarkerStatus;                  // Флаг состояния маркера конца пакета
uint8_t sp_dataLength;                        // Флаг состояния принимаемых данных
uint8_t sp_count;                             // Длинна входящего сообщения
uint8_t data_length;                          // Длинна структуры данных
bool sp_packetAvailable;                      // Флаг завершения приема пакета

bool init = false; int init_count = 0;

protected:

     void timerEvent(QTimerEvent*);


public:  

      void StartProtokol ();

      void ParseCommand();

      void sp_Read();

      void sp_ResetAll();

      void sp_Reset();

public slots:

      void sp_Send(QByteArray data);

signals:

  void ServerWrite (QByteArray data);

  void Telemetriy (QByteArray tele);


};
#endif // UART_DUE_H
