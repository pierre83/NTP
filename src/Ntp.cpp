// Arduino NTP library for WizNet W5x00 based Ethernet boards
// (c) Copyright 2020 Pierre CASAL

// Released under Apache License, version 2.0

#include <Arduino.h>
#include "Ethernet.h"
#include "Ntp.h"
#include "TimeLib.h"		// uses breakTime & makeTime
#include "Dns.h"

//#define DEBUG

// Return codes
#define SUCCESS          1
#define FAILED           0
#define TIMED_OUT        -1

// NTP
#define NTP_PACKET_SIZE		48
#define NTP_PORT			123
#define SEVENTY_YEARS		2208988800UL


static tmElements_t tm;          // Cache of time elements

// *************************************************
void NTPClient::begin( const int timeZone, const uint16_t timeout)
{
    nTimeZone  = timeZone;
    nNtpTimeout = timeout;		// Waiting for an answer from the NTP server
}

// *************************************************
uint32_t NTPClient::getEpoch(const char* NTPServer)
{
	IPAddress NTPAddress;
	DNSClient dns;
	int ret = dns.begin(Ethernet.dnsServerIP());	// Get the DNS @IP provided in Ethernet.begin(xxx))
	if ( ret == SUCCESS ) {
		ret = dns.getHostByName(NTPServer, NTPAddress);
		if ( ret == SUCCESS ) {
			//Serial.println("DNS success");
			return getEpoch(NTPAddress);
		}
	}
	//Serial.print("DNS failed: ");	//Serial.println(ret);
	return FAILED;
}
	
// *************************************************
uint32_t NTPClient::getEpoch(IPAddress& NTPAddress)
{	
    int result = 0;
    uint32_t epoch = 0;
    uint8_t packetBuffer[NTP_PACKET_SIZE];
    int packetSize = 0;
	uint16_t operation_timeout = NTP_TIMEOUT;
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0x23;
    packetBuffer[2] = 6;
    packetBuffer[3] = 0xEC;
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    
    if ( nNtpTimeout < REPLY_TIMEOUT ) {		// Verify that total timeout is
        nNtpTimeout = REPLY_TIMEOUT;			// compatible the a request timeout
        operation_timeout = REPLY_TIMEOUT;
    }
	
	uint32_t endWait = millis() + operation_timeout;
	while ( millis() < endWait ) {
		// Find a socket to use
		result = ntpUdp.begin();	//		Return: 0= no socket, 1= success
		if ( result == SUCCESS ) {
			// Successful get socket
			//Serial.println("ntpUdp.begin");
            result = ntpUdp.beginPacket( NTPAddress, NTP_PORT );   // Return 1= success, 0= bad IP, no socket
            if ( result == SUCCESS ) {
				// Configure destination ip address and port OK
				//Serial.println("ntpUdp.beginPacket");
                ntpUdp.write(packetBuffer, NTP_PACKET_SIZE);   // Writes UDP data to the TX buffer
                result = ntpUdp.endPacket();        // Send the packet, Returns 1= success, -1= W5x00 fail, 0= timeout
				//Serial.print("ntpUdp.endPacket: ");	//Serial.println(result);
                if ( result == SUCCESS ) {
					// The packet has been sent, wait for reply..
					//Serial.println("ntpUdp.endPacket");
					result = TIMED_OUT;
                    uint32_t stopWait = millis() + nNtpTimeout;
                    while ( millis() < stopWait ) {
						// Wait for a response packet
                        packetSize = ntpUdp.parsePacket();
                        if ( packetSize >= NTP_PACKET_SIZE ) {
							// We've got something
							//Serial.println("ntpUdp.parsePacket");
                            ntpUdp.read(packetBuffer, NTP_PACKET_SIZE);
                            uint32_t secsSince1900 = 0;
                            for (uint8_t i = 40; i < 44; i++) {
                                secsSince1900 <<= 8;
                                secsSince1900 |= packetBuffer[i];
                            }
                            epoch = secsSince1900 - SEVENTY_YEARS + NTP_LATENCY;
                            result = SUCCESS;
							break;		// We get something => exit
                        }
						delay(5);
					}
				}
            }
			ntpUdp.stop();        // Close the socket
			if ( result == SUCCESS ) break;	// exit if SUCCESS
			//delay(50);
        }
       delay(100);
    }
#if 0
	uint32_t stop = millis();
    //Serial.print("epoch\t");		//Serial.println(epoch);
    //Serial.print("Elapsed\t");		//Serial.println(stop - (endWait - operation_timeout));
    //Serial.print("Result\t");		//Serial.println(result);
#endif
    return epoch;		// GMT time
}


// *************************************************
bool NTPClient::isDST(uint32_t epoch)
{
    // Equations by Wei-Hwa Huang (US), and Robert H. van Gent (EC)
    // European Economic Community: Since 1996, valid through 2099
	uint32_t DST_begin_epoch, DST_end_epoch;
	
    breakTime(epoch, tm);    // breakTime gives the date corresponding to epoch(from 1970) in tm_elements
	int year  = tm.Year;	// year contained in the given epoch is offset from 1970
	tm.Month  = DST_BEGIN_MONTH;
	tm.Day    = 31 - (((5 * (1970+year)) / 4) + 4) % 7;  // last sunday of march
	tm.Hour   = DST_BEGIN_HOUR;
	tm.Second = 0;
	DST_begin_epoch = makeTime(tm);  // makeTime gives the corresponding epoch to DST START time
	if ( epoch >= DST_begin_epoch ) {
		tm.Year   = year;
		tm.Month  = DST_END_MONTH;
		tm.Day    = 31 - (((5 * (1970+year)) / 4) + 1) % 7;     // last sunday of october
		tm.Hour   = DST_END_HOUR;
		tm.Second = 0;
		DST_end_epoch = makeTime(tm);  // makeTime gives the corresponding epoch to DST END time
		if ( epoch < DST_end_epoch ) {
			return true;  // Summer time
		}
	}
	return false;   // Winter time
}

// *************************************************
uint32_t NTPClient::localTime()
{
	uint32_t epoch = getEpoch(NTP_SERVER_DEFAULT);
	return localTime(epoch);
}

// *************************************************
uint32_t NTPClient::localTime(uint32_t epoch)
{
	if ( epoch == 0 ) return FAILED;
	if ( isDST(epoch) == true ) {
		epoch += 3600;   // Summer time
	}
    epoch += nTimeZone * 3600;    //  GMT + TIMEZONE
    ////Serial.printf("epoch: %lu\n", epoch);
	return epoch;
}

