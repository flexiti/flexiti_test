#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/TFT_ILI9163C/TFT_ILI9163C.h>

// TCP transmision error -8
// If you receive the first error, the error remains!!!!! until we make reset
// Httcp client stops working, while tcp client seems to be resistant to this error
// The physical display is not needed for the test, only SPI transfers
// In my tests error occurs between 15-60 minutes and already then stays until reset
// During the test, you should pay attention to the limitations thingspeak
//  - sending not more often than every 15 seconds.
// (Nor do I know how some people will simultaneously send the same KEY.
//   - Preferably make yourself your own key.
// Here is KEY=7XXUJWCWYTMXKN3L from Siming example

// Normal log:

// CONNECT1 tcp
// sending..
// answer ..323 bytes
// Tcp +++Success+++
// CONNECT2 http (2)
// Sending ..
// Sent..
// http +++Success+++

// Error log:

// CONNECT1 tcp
// sending..
// answer ..323 bytes
// Tcp +++Success+++
// CONNECT2 http (14)
// Sending ..
// Sent..
// http ERROR ERROR



#define LOOPS 8
// loops 3  in my hardware takes 241 ms
// loops 5  in my hardware takes 397 ms
// loops 10 in my hardware takes 785 ms
// For the test can be taken large LOOPS value,
// but we can not exceed 1 s (timer every 1s)
// and we must keep in mind what writes esspresif:
// Please do not call "os_delay_us" or "while" or "for" to occupy CPU more than 10 ms in timer callback.
// Timer interrupt should not exceed 500 ms


#define TIME_COUNTER  // for execution time measure

#ifndef WIFI_SSID
	#define WIFI_SSID "xxx" 					// Put you SSID and Password here
	#define WIFI_PWD "xxxx"
#endif



int Counter1[2];
Timer procTimer;
Timer clockTimer;

HttpClient thingSpeak;


bool NextTry = false;

TFT_ILI9163C tft(2, 0);

//******************************************************************
// Tcp
//******************************************************************

void khOnCompleted(TcpClient& client, bool successful)
{
    if (successful)
    	Serial.println(" Tcp +++Success+++");
    else
    	Serial.println(" Tcp ERROR ERROR");
}

void khOnReadyToSend(TcpClient& client, TcpConnectionEvent sourceEvent)
{
	if(sourceEvent == eTCE_Connected)
	{
		Serial.println("sending..");
		client.sendString("GET / HTTP/1.1\r\n\r\n", false);
	}
}

bool khOnReceive(TcpClient& client, char *buf, int size)
{


   Serial.print("answer ..");
   Serial.print(size);
   Serial.println(" bytes");

   Serial.print(buf);

   client.close();
}


TcpClient TcpHost(khOnCompleted, khOnReadyToSend, khOnReceive);


//********************************************************************
// Http
//********************************************************************

void InfoRefresh(bool IsS)
{
int a;
String text;

for(a=0;a<LOOPS;a++)
	{
	   text="1234567890";
	   tft.setCursor(80, 5+a*10);
	   tft.println(text.c_str());
	}
}





void clockRefresh()
{
	int a;
#ifdef TIME_COUNTER
	uint32 t;
	t=system_get_time();
#endif

	String str = "1234567890ABC";

	tft.setTextSize(1);
	tft.setTextColor(YELLOW,BLACK);
	for(a=0;a<LOOPS;a++)
		{
		tft.setCursor(2,20+a*10);
		tft.print(str.c_str());
		}
	tft.setTextColor(BLUE,BLACK);
	tft.setCursor(135,108);
	tft.print(Counter1[0]);
	tft.setCursor(135,118);
	tft.print(Counter1[1]);

	InfoRefresh(false);

	Serial.print("Connected to: ");
	Serial.print(WifiStation.getSSID());
	Serial.print(" Channel: ");
	Serial.print(WifiStation.getChannel());
	Serial.print(" Strength: ");
	Serial.print(WifiStation.getRssi());
	Serial.println(" dBm");


#ifdef TIME_COUNTER
		Serial.print("Timer instr time: ");
	    Serial.println((system_get_time()-t)/1000);
#endif
}






void onDataSent(HttpClient& client, bool successful)
{
	 Serial.println("Sent..");

	if (successful)
		Serial.println(" http +++Success+++");
	else
	{
		Counter1[1]=Counter1[1]+1;
		Serial.println(" http ERROR ERROR");
	}

}



void sendData()
{

	if (NextTry)
		NextTry=false;
	else
		NextTry=true;


	if (NextTry)
	{
		Counter1[0]=Counter1[0]+1;
			Serial.print("CONNECT2 http (");
			Serial.print(Counter1[0]);
			Serial.println(")");


		String str1 = "http://184.106.153.149/update?key=7XXUJWCWYTMXKN3L&field1=00.0&field2=11.1&field3=33.3&field4=44.4&field5=55.5&field6=66.6&field7=77.7&status=ALL";


	    Serial.println("Sending ..");
		thingSpeak.downloadString(str1,onDataSent);
	}
	else
	{
		Serial.println("CONNECT1 tcp");
		TcpHost.connect("thingspeak.com",80);
	}
}






void connectOk()
{

	Serial.println("I'm CONNECTED");
	Serial.print("My IP: ");
	Serial.println(WifiStation.getIP().toString().c_str());

	procTimer.initializeMs(20*1000, sendData).start(true);   // every 20 sec
	clockTimer.initializeMs(1000, clockRefresh).start(true); // every 1 sec


	procTimer.restart();
	clockTimer.restart();

	sendData();

}

void connectFail()
{
	Serial.println("I'm NOT CONNECTED. Need help :(");
	WifiStation.waitConnection(connectOk, 20, connectFail);
}


void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(false); // Disable debug output to serial

	Counter1[0]=0;
	Counter1[1]=0;
    NextTry=true;


	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

	tft.begin();
	tft.setRotation(3);

	WifiStation.waitConnection(connectOk, 20, connectFail);


}





