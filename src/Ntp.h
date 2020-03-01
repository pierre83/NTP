// Arduino NTP library for WizNet5x00-based Ethernet shield
// (c) Copyright 2019 Pierre CASAL
// Released under Apache License, version 2.0

#ifndef NTPClient_h
#define NTPClient_h

// Change as desired:
#define NTP_SERVER_DEFAULT	"fr.pool.ntp.org"
#define NTP_LATENCY			0	// NTP server-client Latency (secondes)
#define MAX_RETRIES			2	// Warning if watchdog is used

// DST settings (France), accord to yours
#define DST_BEGIN_HOUR		2
#define DST_END_HOUR		3
#define DST_BEGIN_MONTH		3
#define DST_END_MONTH		10


class NTPClient
{
public:
	
	/** Begin NTP client
	    @param Time Zone where the client belongs
	    @param Timeout of the requests in seconds
	    @result none
	*/
	void begin(const uint8_t timeZone, const uint16_t timeout = 2000);

	/** Connect to the remote NTP server and request for epoch
	    @param NTP Server IP address 
	    @param 
	    @result epoch 
	*/
	uint32_t getEpoch(IPAddress& NTPServer);

	/** Connect to the remote NTP server and request for epoch
	    @param NTP Server name
	    @param 
	    @result epoch 
	*/
	uint32_t getEpoch(const char* NTPServer = NTP_SERVER_DEFAULT);

	/** Calculate if epoch should be in DST mode or not
	    @param epoch
	    @param 
	    @result 1 if DST else 0
	*/
	bool isDST(uint32_t epoch);

	/** Calculate time including time zone and DST
	    @param 
	    @param 
	    @result epoch
	*/
	uint32_t localTime();

	/** Calculate time including time zone and DST
	    @param epoch
	    @param 
	    @result epoch
	*/
	uint32_t localTime(uint32_t epoch);

protected:

	uint16_t nNtpTimeout;
	uint8_t nTimeZone;
	EthernetUDP nUdp;
};

#endif
