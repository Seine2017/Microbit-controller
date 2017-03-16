/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/


/*
 * This is a simple example that uses the micro:bit radio to show if another
 * micro:bit running the same code is nearby.
 *
 * Each micro:bit is periodically broadcasting a group name, and listening
 * to see if another micro:bit in that group is also in-range. 
 *
 * When another micro:bit in the same group is detected, the display of the
 * detecting micro:bit will show a filled-in heart shape
 *
 * Otherwise, the display will show an empty heart shape
 *
 */

#include "MicroBit.h"

#if MICROBIT_BLE_ENABLED 
 #ifdef YOTTA_CFG
  #error "This example needs BLE to be disabled. Use the yotta config.json in the proximit-heart directory to do this"
 #else
  #error "This example needs BLE to be disabled in the microbit config file in the microbit-dal: MicroBitConfig.h"
 #endif
#endif

MicroBit    uBit;

// Have we seen a friend recently?
uint8_t friend_seen = 0;

int duty = 75;
int rollDuty = 75;
int pitchDuty = 75;

// Are we currently sending out messages?
uint8_t broadcast = 1;

/* We have a group name, and if any micro:bit is in range and in the group
 * then the others will see it.
 */
const char group_name[] = "tiger";


/* We send messages when people press buttons 'A' and 'B'.
 * At the moment, all micro:bits listening for messages
 * will see these and can respond to them
 * Challenge: make only certain micro:bits respond to these
 */
void onButtonA(MicroBitEvent)
{
    	uBit.radio.datagram.send("2");    
}

void onButtonB(MicroBitEvent)
{
    uBit.radio.datagram.send("1");    
}



void onData(MicroBitEvent)
{
    ManagedString s = uBit.radio.datagram.recv();
    int rssi = uBit.radio.getRSSI();

    if (s == "1")
    {
        if(duty < 89)
        	duty = duty + 1;
	}
	
    if (s == "2")
    {
        if(duty > 64)
        	duty = duty  - 1;
    }
    
    if (s == "3")
    {
        if(rollDuty < 89)
	    	rollDuty +=1;
    }
    
    if (s == "4")
    {
        if(rollDuty > 64)
            rollDuty -=1;
    }
    
    if (s == "5")
    {
        if(pitchDuty < 89)
	    	pitchDuty +=1;
    }
    
    if (s == "6")
    {
        if(pitchDuty > 64)
            pitchDuty -=1;
    }
   
	if (s == "7")
    {
        if(rollDuty >= 77)
            rollDuty -=1;
        else if(rollDuty <= 75)
            rollDuty +=1;
    }
    if (s == "8")
    {
        if(pitchDuty >= 77)
            pitchDuty -=1;
        else if(pitchDuty <= 75)
            pitchDuty +=1;
    }

    /* For detecting the presence of our friend, we require them to be sending
     * the same group name as we are in 
     */ 
    if (s == group_name && rssi < 70)
     {
        // We can make this larger to allow more missed packets
        friend_seen = 3;
    }
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    // Setup some button handlers to allow direct heartbeat control with buttons
  	uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, onButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, onButtonB);


    //Setup a hander to run when data is received
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);

    uBit.radio.enable();

    while(1) {
        //uBit.display.scroll("CONNECTED");
        if (uBit.accelerometer.getX() <= -500) 
        {
        	uBit.radio.datagram.send("3");

        } 
        else if (uBit.accelerometer.getX() >= 500) 
        {
            uBit.radio.datagram.send("4");

        }
        else
        {
        	uBit.radio.datagram.send("7");
        }
        
        if (uBit.accelerometer.getY() <= -500) 
        {
        	uBit.radio.datagram.send("5");

        } 
        else if (uBit.accelerometer.getY() >= 500) 
        {
            uBit.radio.datagram.send("6");

        }
		else
		{
			uBit.radio.datagram.send("8");
		
		}
        if (broadcast)
            uBit.radio.datagram.send(group_name);
            
            
        uBit.io.P2.setAnalogValue(pitchDuty);
    	uBit.io.P1.setAnalogValue(rollDuty);		
		uBit.io.P0.setAnalogValue(duty);
        uBit.sleep(10);
    }
}
