#ifndef TIME_H_
#define TIME_H_

#include <avr/io.h>

#ifndef __isleap
/* Nonzero if YEAR is a leap year (every 4 years,
   except every 100th isn't, and every 400th is).  */
#define	__isleap(year)	\
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#endif

#define	SECS_PER_HOUR	(uint32_t)(60 * 60)
#define	SECS_PER_DAY	(uint32_t)(SECS_PER_HOUR * 24)



#define CLOCKS_PER_SEC  1000
struct time_info
{
  char *abbrev_wkday[7];	/* Short weekday names.  */
  char *full_wkday[7];		/* Full weekday names.  */
  char *abbrev_month[12];	/* Short month names.  */
  char *full_month[12];		/* Full month names.  */
  char *ampm[2];		/* "AM" and "PM" strings.  */

  char *date_time;		/* Appropriate date and time format.  */
  char *date;			/* Appropriate date format.  */
  char *time;			/* Appropriate time format.  */

  char *ut0;			/* Name for GMT.  */
  char *tz;			/* Default TZ value.  */
};

typedef struct tm_t{
		int tm_sec;     /* seconds after the minute - [0,59] */
		int tm_min;     /* minutes after the hour - [0,59] */
		int tm_hour;    /* hours since midnight - [0,23] */
		int tm_mday;    /* day of the month - [1,31] */
		int tm_mon;     /* months since January - [1,12] */
		int tm_year;    /* year*/
		int tm_wday;    /* days since Sunday - [0,6] */
}tm_t;

void SystemTick(void);
void time_GetLocalTime(tm_t *localTime);
void Set_Unix_Time(uint32_t timestamp);
char *asctime(struct tm_t *pTime);

#endif