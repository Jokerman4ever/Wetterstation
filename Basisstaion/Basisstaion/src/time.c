#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "Storage/FileSys.h"

uint32_t g_ulTimeStamp;


struct time_info __time_C =
  {
    { 
(char *) "Sun", (char *) "Mon", (char *) "Tue", (char *) "Wed", (char *) "Thu", (char *) "Fri", (char *) "Sat",  },

    { 
(char *) "Sunday", (char *) "Monday", (char *) "Tuesday", (char *) "Wednesday", (char *) "Thursday", (char *) "Friday", (char *) "Saturday",  },
    { 
(char *) "Jan", (char *) "Feb", (char *) "Mar", (char *) "Apr", (char *) "May", (char *) "Jun", (char *) "Jul", (char *) "Aug", (char *) "Sep", (char *) "Oct", (char *) "Nov", (char *) "Dec",  },
    { 
(char *) "January", (char *) "February", (char *) "March", (char *) "April", (char *) "May", (char *) "June", (char *) "July", (char *) "August", (char *) "September", (char *) "October", (char *) "November", (char *) "December",  },
    { (char *) "AM", (char *) "PM" },
    (char *) "%a %b %d %H:%M:%S %Y", (char *) "%m/%d/%y", (char *) "%H:%M:%S",
    (char *) "GMT", (char *) ""
  };

struct time_info *_time_info = &__time_C;

const unsigned char __mon_lengths[2][12] = 
	{
		/* Normal years.  */
		{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
		/* Leap years.  */
		{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
	};
void SystemTick() {
	g_ulTimeStamp++; //1 sec tick
}

void Set_Unix_Time(uint32_t timestamp)
{
	g_ulTimeStamp = timestamp;	
	FS_SetUnix(timestamp);
}

void time_GetLocalTime(tm_t *localTime) {
	uint32_t days, rem;
	uint16_t year;
	char *ip;
	
	days = g_ulTimeStamp / SECS_PER_DAY;
	rem = g_ulTimeStamp % SECS_PER_DAY;
	while (rem < 0) {
		rem += SECS_PER_DAY;
		--days;
	}
	while (rem >= SECS_PER_DAY) {
		rem -= SECS_PER_DAY;
		++days;
	}
	localTime->tm_hour = rem / SECS_PER_HOUR;	// Set hours
	rem %= SECS_PER_HOUR;
	localTime->tm_min = rem / 60; //Set minutes
	localTime->tm_sec = rem % 60; //Set seconds
	
	/* January 1, 1970 was a Thursday.  */
	localTime->tm_wday = (4 + days) % 7;
	if (localTime->tm_wday < 0)
	{
		localTime->tm_wday += 7;	//Set Week-Day
	}
	year = 1970;
	while (days >= (rem = __isleap(year) ? 366 : 365)) {
		++year;
		days -= rem;
	}

	while (days < 0) {
		--year;
		days += __isleap(year) ? 366 : 365;
	}
	localTime->tm_year = year; //Set Year
	ip = (char *)__mon_lengths[__isleap(year)];
	for (year = 0; days >= ip[year]; ++year)
	{
		days -= ip[year];
	}
	localTime->tm_mon = year + 1; //Set Month
	localTime->tm_mday = days + 1; //Set Month-Day
}

char *asctime(struct tm_t *pTime) {
	if (pTime==0) {
		pTime=malloc(sizeof(struct tm_t));
		time_GetLocalTime(pTime);
	}
	static const char format[] = "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n";
//	static char result[	         3+1+ 3+1+20+1+20+1+20+1+20+1+20+1 + 1];
	static char result[	         3+1+ 3+1+3+1+3+1+3+1+3+1+3+1 + 1];

  if (sprintf (result, format,
	       (pTime->tm_wday < 0 || pTime->tm_wday >= 7 ?
		"???" : _time_info->abbrev_wkday[pTime->tm_wday]),
	       (pTime->tm_mon < 0 || pTime->tm_mon >= 12 ?
		"???" : _time_info->abbrev_month[pTime->tm_mon]),
	       pTime->tm_mday, pTime->tm_hour, pTime->tm_min,
	       pTime->tm_sec, pTime->tm_year) < 0)
    return NULL;

  return result;
}

