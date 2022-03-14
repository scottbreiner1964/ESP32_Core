
#include "GlobalDefinitions.h"

extern void TaskInitiator(void);

void setup()
{
   Serial.begin(115200);
   TaskInitiator();
}

void loop()
{
   
}
