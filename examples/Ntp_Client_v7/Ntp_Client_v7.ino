/*

  NTP Client

  Get the time from a Network Time Protocol (NTP) server

  For more on NTP time servers and the messages needed to communicate with them,
  see http://en.wikipedia.org/wiki/Network_Time_Protocol

  February 2020 - Pierre Casal

  ESP32  ->   W5500 -> ESP8266
  GPIO19  --> MISO  -> D6
  GPIO23  --> MOSI  -> D7
  GPIO18  --> SCK   -> D5
  GPIO5   --> CS    -> D2
*/

#include <SPI.h>
#include <Ethernet.h>
#include <TimeLib.h>
#include <Ntp.h>
#include "..\tools\Ethernet_W5x00\Init_W5x00\Init_W5x00.ino"

//------------------------------------------------------------
// CHANGE AS DESIRED:
const char timeServerName[] = "fr.pool.ntp.org";
IPAddress timeServerIP = {192, 168, 0, 254 };


#if defined (ESP8266)
const uint8_t CS = 4;   // ESP8266 CS -> D2 (GPIO04)
#elif defined (ESP32)
const uint8_t CS = 5;     // ESP32   CS -> D5 (GPIO05)
#elif defined (__AVR__)
const uint8_t CS = 10;  // AVR (default)
#endif

const uint8_t TIMEZONE = 1;

uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress ip(192, 168, 0, 56);
IPAddress myDns(192, 168, 0, 254);

bool dhcp = 1;  // 0= fixed IP, 1 = DHCP

const int DNS_TIMEOUT = 2500;
//------------------------------------------------------------


NTPClient ntp;

tmElements_t tm;



void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Ethernet.init(CS);

  // start the Ethernet connection:
  Ethernet_init( dhcp, mac, ip, myDns);

  // Start NTP
  ntp.begin(TIMEZONE, DNS_TIMEOUT);
}

void loop()
{
  Serial.println(F("\nGet time !"));
  Serial.println(F("ntp.getEpoch(timeServerIP)"));
  uint32_t debut = millis();
  uint32_t epoch = ntp.getEpoch(timeServerIP); // send an NTP packet to a time server
  uint32_t fin = millis();
  Serial.print(F("Epoch: "));       Serial.println(epoch);
  afficheTemps(fin - debut);

  Serial.println(F("ntp.getEpoch(timeServerName)"));
  debut = millis();
  epoch = ntp.getEpoch(timeServerName); // Ask to a time server
  Serial.print(F("Epoch: "));       Serial.println(epoch);
  fin = millis();
  afficheTemps(fin - debut);

  Serial.print(F("Unix time = "));  Serial.println(epoch);

  Serial.print(F("UTC time is "));       // UTC is the time at Greenwich Meridian (GMT)
  afficheHeure(epoch);

  Serial.println(F("\nntp.localTime(epoch)"));
  debut = millis();
  uint32_t epoch_here = ntp.localTime(epoch);
  fin = millis();
  Serial.print(F("Epoch: "));       Serial.println(epoch_here);
  afficheTemps(fin - debut);

  Serial.print(F("Epoch DST = "));
  Serial.println(epoch_here);

  Serial.print(F("Local time is "));       // Local time with time zone and DST
  afficheHeure(epoch_here);
  Serial.println(F("\nntp.localTime()"));
  debut = millis();
  epoch_here = ntp.localTime();
  fin = millis();
  afficheTemps(fin - debut);

  Serial.print(F("Epoch DST = "));  Serial.println(epoch_here);

  Serial.print(F("Local time is "));       // Local time with time zone and DST
  afficheHeure(epoch_here);

  bool dst = ntp.isDST(epoch);
  if ( dst == true ) {
    Serial.println(F("\nSummer time"));
  } else {
    Serial.println(F("\nWinter time"));
  }

  Serial.println(F("\nSocket Errors:"));
  Serial.print(F("Init\t"));  Serial.println(Ethernet.getSocketError(0));
  Serial.print(F("Connect\t"));  Serial.println(Ethernet.getSocketError(1));
  Serial.print(F("State\t"));  Serial.println(Ethernet.getSocketError(2));

  // wait ten seconds before asking for the time again
  delay(10000);
}

void afficheHeure(uint32_t epoch)
{
  // print the hour, minute and second:

  Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
  Serial.print(':');
  if (((epoch % 3600) / 60) < 10) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
  Serial.print(':');
  if ((epoch % 60) < 10) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.println(epoch % 60); // print the second
}


void afficheTemps(uint32_t temps)
{
  Serial.print(F("Duration = "));
  Serial.print(temps);
  Serial.println(F(" ms\n"));
}
