/*

  NTP Client

  Get the time from a Network Time Protocol (NTP) time server

  For more on NTP time servers and the messages needed to communicate with them,
  see http://en.wikipedia.org/wiki/Network_Time_Protocol

  February 2020 - Pierre Casal

*/

#include <SPI.h>
#include <Ethernet.h>
#include <TimeLib.h>
#include <Ntp.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};


const char timeServerName[] = "fr.pool.ntp.org";

// Modify as needed:
IPAddress timeServerIP = {192, 168, 0, 254 };
const int DNS_TIMEOUT = 2500;
const uint8_t TIMEZONE = 1;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 56);
IPAddress myDns(192, 168, 0, 254);

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

   // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Serial.println("Configure using given IP address");
    int res = Ethernet.begin(mac, ip, myDns, myDns);
      if ( res != 1  ) {
        Serial.println("Ethernet did not start, verify settings");
        while (true) {
          delay(1); // do nothing
        }
    }
  }
  Serial.print("\tIP address ");
  Serial.println(Ethernet.localIP());

  // Check for Ethernet hardware present
  uint8_t hardware_type = Ethernet.hardwareStatus();
  if ( hardware_type == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.");
    while (true) {
      delay(1); // do nothing
    }
  }
  else if ( hardware_type == EthernetW5500) {
    Serial.println("Ethernet type W5500");
  }
  else if ( hardware_type == EthernetW5100) {
    Serial.println("Ethernet type W5100");
  }
  uint8_t link_status = Ethernet.linkStatus();
  if (link_status == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    while (true) {
      delay(1); // do nothing
    }
  }
  else if (link_status == LinkON) {
    Serial.println("Ethernet cable is connected");
  }
  else if (link_status == Unknown) {
    Serial.println("If DHCP ok, maybe W5100 otherwise Ethernet cable is NOT connected");
  }
  
  ntp.begin(TIMEZONE, DNS_TIMEOUT);
}

void loop()
{
  Serial.println(F("\nGet time !"));
  Serial.println(F("ntp.getEpoch(timeServerIP)"));
  uint32_t debut = millis();
  uint32_t epoch = ntp.getEpoch(timeServerIP); // send an NTP packet to a time server
  uint32_t fin = millis();
  afficheTemps(fin - debut);

  Serial.println(F("ntp.getEpoch(timeServerName)"));
  debut = millis();
  epoch = ntp.getEpoch(timeServerName); // Ask to a time server
  fin = millis();
  afficheTemps(fin - debut);

  Serial.print(F("Unix time = "));
  Serial.println(epoch);

  Serial.print(F("UTC time is "));       // UTC is the time at Greenwich Meridian (GMT)
  afficheHeure(epoch);

  Serial.println(F("\nntp.localTime(epoch)"));
  debut = millis();
  uint32_t epoch_here = ntp.localTime(epoch);
  fin = millis();
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

  Serial.print(F("Epoch DST = "));
  Serial.println(epoch_here);

  Serial.print(F("Local time is "));       // Local time with time zone and DST
  afficheHeure(epoch_here);

  bool dst = ntp.isDST(epoch);
  if ( dst == true ) {
    Serial.println(F("\nSummer time"));
  } else {
    Serial.println(F("\nWinter time"));
  }

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
