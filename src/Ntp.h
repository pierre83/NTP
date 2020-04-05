// Arduino NTP library for WizNet5x00-based Ethernet shield
// (c) Copyright 2019 Pierre CASAL
// Released under Apache License, version 2.0

#ifndef NTPClient_h
#define NTPClient_h

// Change as desired:
#define NTP_SERVER_DEFAULT	"fr.pool.ntp.org"
#define NTP_LATENCY			0		// NTP server-client Latency (secondes)

// Timeout and retries
// Total time allowed to get an answer, permitting retries
// Here 3 x REPLY_TIMEOUT <= NTP_TIMEOUT => 3 retries at the minimum
#define	NTP_TIMEOUT			7500	// Warning if WDT used
// Time allowed for a NTP server answer after a request
#define	REPLY_TIMEOUT		2400	// Warning if too low

// DST settings
#define DST_BEGIN_HOUR		1
#define DST_END_HOUR		1
#define DST_BEGIN_MONTH		3
#define DST_END_MONTH		10


class NTPClient
{
public:
	
	/** Begin NTP client
	    @param Time Zone where the client belongs (-x to +x)
	    @param Timeout of the request in seconds, upto 7.5sec by default(does'nt include DNS time)
	    @result none
	*/
	void begin(const int timeZone = 0, const uint16_t timeout = REPLY_TIMEOUT);

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

	/** Calculate if epoch is in DST window or not
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
	int nTimeZone;
	EthernetUDP ntpUdp;
};

#endif
