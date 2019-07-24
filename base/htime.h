#ifndef HW_TIME_H_
#define HW_TIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hplatform.h"
#include "hdef.h"

#define SECONDS_PER_HOUR    3600
#define SECONDS_PER_DAY     86400   // 24*3600
#define SECONDS_PER_WEEK    604800  // 7*24*3600

#define IS_LEAP_YEAR(year) (((year)%4 == 0 && (year)%100 != 0) ||\
                            (year)%100 == 0)

typedef struct datetime_s {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    int ms;
} datetime_t;

#ifdef OS_WIN
static inline void sleep(unsigned int s) {
    Sleep(s*1000);
}
#endif

static inline void msleep(unsigned int ms) {
#ifdef OS_WIN
    Sleep(ms);
#else
    usleep(ms*1000);
#endif
}

// ms
static inline unsigned int gettick() {
#ifdef OS_WIN
    return GetTickCount();
#else
    return clock()*(unsigned long long)1000 / CLOCKS_PER_SEC;
#endif
}

// us
unsigned long long gethrtime();

datetime_t datetime_now();
time_t     datetime_mktime(datetime_t* dt);

datetime_t* datetime_past(datetime_t* dt, int days DEFAULT(1));
datetime_t* datetime_future(datetime_t* dt, int days DEFAULT(1));

/*
 * minute   hour    day     week    month       action
 * 0~59     0~23    1~31    0~6     1~12
 *  30      -1      -1      -1      -1          cron.hourly
 *  30      1       -1      -1      -1          cron.daily
 *  30      1       15      -1      -1          cron.monthly
 *  30      1       -1       7      -1          cron.weekly
 *  30      1        1      -1      10          cron.yearly
 */
time_t calc_next_timeout(int minute, int hour, int day, int week, int month);

int days_of_month(int month, int year);
int month_atoi(const char* month);
const char* month_itoa(int month);

datetime_t get_compile_datetime();

#ifdef __cplusplus
} // extern "C"
#endif

#endif  // HW_TIME_H_