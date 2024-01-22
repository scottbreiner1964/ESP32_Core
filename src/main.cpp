
#include "GlobalDefinitions.h"

extern void TaskInitiator(void);

void setup()
{
   Serial.begin(1000000);
   //Serial.println(23);
   TaskInitiator();
}

void loop()
{


}
