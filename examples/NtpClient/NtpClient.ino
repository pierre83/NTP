/*

  NTP Client

  Get the time from a Network Time Protocol (NTP) time server

  For more on NTP time servers and the messages needed to communicate with them,
  see http://en.wikipedia.org/wiki/Network_Time_Protocol

  created 25 Oct 2019
  by Pierre Casal

  This code is in the public domain.

*/

#include <SPI.h>
#include <Ethernet.h>
#include <TimeLib.h>
#include <Ntp.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

const char timeServer[] = "fr.pool.ntp.org";
const uint8_t TIMEZONE = 1;

NTPClient ntp;

tmElements_t tm;



void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println(F("Ethernet shield was not found.  Sorry, can't run without hardware. :("));
    }
    else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println(F("Ethernet cable is not connected."));
    }
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }
  } else {
    Serial.println(F("Ethernet started OK"));
  }
  ntp.begin(TIMEZONE, 2500);
}

void loop()
{
  Serial.println(F("\nGet time !"));
  uint32_t debut = millis();
  uint32_t epoch = ntp.getEpoch(timeServer); // send an NTP packet to a time server
  uint32_t fin = millis();
  Serial.print(F("Duree = "));
  Serial.print(fin - debut);

  Serial.print(F("ms\nUnix time = "));
  Serial.println(epoch);

  bool dst =  ntp.isDST(epoch);
  if ( dst == true ) {
    Serial.println(F("Heure d'ete"));
  } else {
    Serial.println(F("Heure d'hiver"));
  }

  epoch += TIMEZONE * 3600 + dst * 3600;

  Serial.print(F("Epoch DST = "));
  Serial.println(epoch);

  // print the hour, minute and second:
  Serial.print(F("UTC time is "));       // UTC is the time at Greenwich Meridian (GMT)
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

  // wait ten seconds before asking for the time again
  delay(10000);
}
