#include "Signals.h"
#include <myserver.h>
#include <QDebug>

Signals::Signals(){}

void Signals::setValue()
{
  emit event_uart();
}



