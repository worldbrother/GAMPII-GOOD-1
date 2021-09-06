/*------------------------------------------------------------------------------
* TimeUtil.cpp : time conversion functions
*
* Copyright (C) 2020-2099 by SpAtial SurveyIng and Navigation (SASIN), all rights reserved.
*    This file is part of GAMP II - GOOD (Gnss Observation and Other data Downloading) tool
*
* References:
*    
*
* history : 2020/08/23 1.0  new (by Feng Zhou)
*           2020/09/12      fix a bug (missing t1 = t0) in TimeAdd (by Yuze Yang and Feng Zhou)
*           2020/10/25      add VectorXd type for date (by Feng Zhou)
*-----------------------------------------------------------------------------*/
#include "Good.h"
#include "TimeUtil.h"


/* constants/macros ----------------------------------------------------------*/
#define MAXLEAPS    64       /* max number of leap seconds table */


static const double gpst0[] = { 1980,1, 6, 0, 0, 0 };  /* GPS time reference */
static const double bdst0[] = { 2006,1, 1, 0, 0, 0 };  /* BDS time reference */
static double leaps[MAXLEAPS + 1][7] =
{
    /* leap seconds (y,m,d,h,m,s,UTC-GPST) */
    { 2017,1,1,0,0,0,-18 },
    { 2015,7,1,0,0,0,-17 },
    { 2012,7,1,0,0,0,-16 },
    { 2009,1,1,0,0,0,-15 },
    { 2006,1,1,0,0,0,-14 },
    { 1999,1,1,0,0,0,-13 },
    { 1997,7,1,0,0,0,-12 },
    { 1996,1,1,0,0,0,-11 },
    { 1994,7,1,0,0,0,-10 },
    { 1993,7,1,0,0,0, -9 },
    { 1992,7,1,0,0,0, -8 },
    { 1991,1,1,0,0,0, -7 },
    { 1990,1,1,0,0,0, -6 },
    { 1988,1,1,0,0,0, -5 },
    { 1985,7,1,0,0,0, -4 },
    { 1983,7,1,0,0,0, -3 },
    { 1982,7,1,0,0,0, -2 },
    { 1981,7,1,0,0,0, -1 },
    { 0 }
};


/* function definition -------------------------------------------------------*/

/**
* @brief   : LeapYear - to check if it is leap year
* @param[I]: year (4-digit year)
* @param[O]: none
* @return  : true, leap year; false, not leap year
* @note    :
**/
bool TimeUtil::LeapYear(int year)
{
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        return true;
    else
        return false;
} /* end of LeapYear */

/**
* @brief   : yy2yyyy - the conversion from 2-digit year to 4-digit year
* @param[I]: yy (2-digit year)
* @param[O]: none
* @return  : 4-digit year
* @note    :
**/
int TimeUtil::yy2yyyy(int yy)
{
    int yyyy = yy;
    if (yyyy <= 50) yyyy += 2000;
    else if (yy > 50 && yy < 1900) yyyy += 1900;

    return yyyy;
} /* end of yy2yyyy */

/**
* @brief   : yyyy2yy - the conversion from 4-digit year to 2-digit year
* @param[I]: yyyy (4-digit year)
* @param[O]: none
* @return  : 2-digit year
* @note    :
**/
int TimeUtil::yyyy2yy(int yyyy)
{
    int yy = yyyy;
    if (yy >= 2000) yy -= 2000;
    else if (yy >= 1900 && yy < 2000) yy -= 1900;

    return yy;
} /* end of yyyy2yy */

/**
* @brief   : TimeAdd - to increase time t0 by dt (in seconds), return t1 with t1(1) < 604800.0
* @param[I]: t0 (time struct (.mjd and .sod))
* @param[I]: dt (time struct (increased seconds))
* @param[O]: none
* @return  : time struct (.mjd and .sod)
* @note    :
**/
gtime_t TimeUtil::TimeAdd(gtime_t t0, double dt)
{
    gtime_t t1 = t0;
    t1.sod = t0.sod + dt;
    
    while (true)
    {
        if (t1.sod >= 86400.0)
        {
            t1.sod -= 86400.0;
            t1.mjd += 1;
        }
        else if (t1.sod < 0.0)
        {
            t1.sod += 86400.0;
            t1.mjd -= 1;
        }
        else break;
    }

    return t1;
} /* end of TimeAdd */

/**
* @brief   : TimeDiff - time difference (t1-t0), return in seconds
* @param[I]: t1, t0 (time struct (.mjd and .sod))
* @param[O]: none
* @return  : time difference in seconds
* @note    : the first time subtracts the second time
**/
double TimeUtil::TimeDiff(gtime_t t1, gtime_t t0)
{
    return (t1.mjd - t0.mjd) * 86400.0 + (t1.sod - t0.sod);
} /* end of TimeDiff */

/**
* @brief   : sod2hms - the conversion from seconds of day to hour, minute, and second
* @param[I]: sod (seconds of day)
* @param[O]: hh (hour, nullptr:NO output)
* @param[O]: minu (minute, nullptr:NO output)
* @param[O]: sec (second, nullptr:NO output)
* @return  : none
* @note    :
**/
void TimeUtil::sod2hms(double sod, int *hh, int *minu, double *sec)
{
    int hh_tmp = (int)floor(sod / 3600.0 + 1.0e-9);
    if (hh) *hh = hh_tmp;

    double tmp = sod - hh_tmp * 3600.0;
    int minu_tmp = (int)floor(tmp / 60.0 + 1.0e-9);
    if (minu) *minu = minu_tmp;

    double sec_tmp = sod - hh_tmp * 3600.0 - minu_tmp * 60.0;
    if (sec) *sec = sec_tmp;
} /* end of sod2hms */

/**
* @brief   : hms2sod - the conversion from hour, minute, and second to seconds of day
* @param[I]: hh (hour)
* @param[I]: minu (minute)
* @param[I]: sec (second)
* @param[O]: none
* @return  : seconds of day
* @note    :
**/
double TimeUtil::hms2sod(int hh, int minu, double sec)
{
    return hh * 3600.0 + minu * 60.0 + sec;
} /* end of hms2sod */

/**
* @brief   : time2fmjd - the conversion from time to modified Julian date
* @param[I]: tt (time struct (.mjd and .sod))
* @param[O]: none
* @return  : modified Julian date
* @note    :
**/
double TimeUtil::time2fmjd(gtime_t tt)
{
    return tt.mjd + tt.sod / 86400.0;
} /* end of time2fmjd */

/**
* @brief   : fmjd2time - the conversion from modified Julian date to time
* @param[I]: modified Julian date
* @param[O]: none
* @return  : time struct (.mjd and .sod)
* @note    :
**/
gtime_t TimeUtil::fmjd2time(double fmjd)
{
    gtime_t tt = { 0 };
    tt.mjd = (int)floor(fmjd);
    tt.sod = (fmjd - tt.mjd) * 86400.0;

    return tt;
} /* end of fmjd2time */

/**
* @brief   : ymdhms2time - the conversion from year, month, day, hour, minute, second to time
* @param[I]: date (6 x 1 vector indicating year, month, day, hour, minute, and second)
* @param[O]: none
* @return  : time struct (.mjd and .sod)
* @note    :
**/
gtime_t TimeUtil::ymdhms2time(const double *date)
{
    gtime_t tt = {0};
    tt.sod = hms2sod((int)floor(date[3]), (int)floor(date[4]), date[5]);
    
    int yyyy = yy2yyyy((int)floor(date[0]));
    int month = (int)floor(date[1]);
    int day = (int)floor(date[2]);
    if (month <= 2)
    {
        yyyy -= 1;
        month += 12;
    }

    double jd = (int)floor(365.25 * yyyy + 1.0e-9) + (int)floor(30.6001 * (month + 1) + \
        1.0e-9) + day + 1720981.5;
    tt.mjd = (int)floor(jd - 2400000.5);

    return tt;
} /* end of ymdhms2time */

/**
* @brief   : time2ymdhms - the conversion from time to year, month, day, hour, minute, second
* @param[I]: tt (time struct (.mjd and .sod))
* @param[O]: date (6 x 1 vector indicating year, month, day, hour, minute, and second)
* @return  : none
* @note    :
**/
void TimeUtil::time2ymdhms(gtime_t tt, double *date)
{
    double mjd = tt.mjd + tt.sod / 86400.0;

    int a = (int)floor(mjd + 1.0 + 1.0e-9) + 2400000;
    double frac = mjd + 0.5 + 2400000.5 - a;
    int b = a + 1537;
    int c = (int)floor((b - 122.1) / 365.25 + 1.0e-9);
    int d = (int)floor(365.25 * c + 1.0e-9);
    int e = (int)floor((b - d) / 30.6001 + 1.0e-9);
    int day = b - d - (int)floor(30.6001 * e);
    int month = e - 1 - 12 * (int)floor(e / 14.0 + 1.0e-9);
    int year = c - 4715 - (int)floor((7 + month) / 10.0 + 1.0e-9);

    double thh = frac * 24.0;
    int hour = (int)floor(thh + 1.0e-9);
    double tmin = (thh - hour) * 60.0;
    int min = (int)floor(tmin + 1.0e-9);
    double sec = (tmin - min) * 60.0;
    if (sec > 59.999)
    {
        sec = 0.0;
        min += 1;
    }
    if (min >= 60)
    {
        min -= 60;
        hour += 1;
    }

    date[0] = year; date[1] = month; date[2] = day;
    date[3] = hour; date[4] = min;   date[5] = sec;
} /* end of time2ymdhms */

/**
* @brief   : yrdoy2time - the conversion from year, day of year to time
* @param[I]: year (4- or 2-digit year)
* @param[I]: doy (day of year)
* @param[O]: none
* @return  : time struct (.mjd and .sod)
* @note    :
**/
gtime_t TimeUtil::yrdoy2time(int year, int doy)
{
    /* time at yyyy-01-01 00:00:00 */
    double date[6] = { 0 };
    date[0] = year; date[1] = 1; date[2] = 1;
    date[3] = 0;    date[4] = 0; date[5] = 0;
    gtime_t t_jan1 = ymdhms2time(date);
    double dt = (doy - 1) * 86400.0;
    gtime_t tt = TimeAdd(t_jan1, dt);

    return tt;
} /* end of yrdoy2time */

/**
* @brief   : time2yrdoy - the conversion from time to year, day of year
* @param[I]: tt (time struct (.mjd and .sod))
* @param[O]: year (4-digit year, nullptr:NO output)
* @param[O]: doy (day of year, nullptr:NO output)
* @return  : none
* @note    :
**/
void TimeUtil::time2yrdoy(gtime_t tt, int *year, int *doy)
{
    double date[6] = { 0 };
    time2ymdhms(tt, date);
    if (year) *year = (int)date[0];

    /* time at yyyy-01-01 00:00:00 */
    date[1] = 1; date[2] = 1; date[3] = 0; date[4] = 0; date[5] = 0;
    gtime_t t_jan1 = ymdhms2time(date);

    double dt = TimeDiff(tt, t_jan1);
    if (doy) *doy = (int)(dt / 86400.0 + 1.0e-9) + 1;
} /* end of time2yrdoy */

/**
* @brief   : ymd2yrdoy - the conversion from year, month, day to year, day of year
* @param[I]: year (2- or 4-digit year)
* @param[I]: month (1-12)
* @param[I]: day (day within a month, 1-31)
* @param[O]: none
* @return  : doy (day of year)
* @note    :
**/
int TimeUtil::ymd2yrdoy(int year, int month, int day)
{
    double date[6] = { 0 };
    date[0] = year; date[1] = month; date[2] = day;
    gtime_t tt_now = ymdhms2time(date);
    int doy = 0;
    time2yrdoy(tt_now, nullptr, &doy);

    return doy;
} /* end of ymd2yrdoy */

/**
* @brief   : yrdoy2ymd - the conversion from year, day of year to year, month, day
* @param[I]: year (2- or 4-digit year)
* @param[I]: doy (day of year)
* @param[O]: month (1-12, nullptr:NO output)
* @param[O]: day (day within a month, 1-31, nullptr:NO output)
* @return  : none
* @note    :
**/
void TimeUtil::yrdoy2ymd(int year, int doy, int *month, int *day)
{
    gtime_t tt_now = yrdoy2time(year, doy);
    
    double date[6] = { 0 };
    time2ymdhms(tt_now, date);
    if (month) *month = (int)floor(date[1]);
    if (day) *day = (int)floor(date[2]);
} /* end of yrdoy2ymd */

/**
* @brief   : gpst2time - the conversion from GPS week, seconds of week to time
* @param[I]: week (GPS week)
* @param[I]: sow (seconds of week)
* @param[O]: none
* @return  : time struct (.mjd and .sod)
* @note    :
**/
gtime_t TimeUtil::gpst2time(int week, double sow)
{
    /* compute the time of the GPS starting time (1980-01-06 00:00:00) */
    gtime_t t_gpst0 = ymdhms2time(gpst0);

    double dt = week * 7 * 86400.0 + sow;
    gtime_t tt = TimeAdd(t_gpst0, dt);

    return tt;
} /* end of gpst2time */

/**
* @brief   : time2gpst - the conversion from time to GPS week, seconds of week
* @param[I]: tt (time struct (.mjd and .sod))
* @param[O]: week (GPS week, nullptr:NO output)
* @param[O]: sow (seconds of week, nullptr:NO output)
* @return  : day of week
* @note    :
**/
int TimeUtil::time2gpst(gtime_t tt, int *week, double *sow)
{
    /* compute the time of the GPS starting time (1980-01-06 00:00:00) */
    gtime_t t_gpst0 = ymdhms2time(gpst0);

    /* compute the time differences */
    double delta_day = TimeDiff(tt, t_gpst0) / 86400.0;

    /* compute the GPS week */
    int week_tmp = (int)floor(delta_day / 7.0 + 1.0e-9);
    if (week) *week = week_tmp;

    /* compute the days from the last GPS week to current day */
    int dow = (int)floor(delta_day - week_tmp * 7 + 1.0e-9);

    /* compute the seconds within GPS week */
    if (sow) *sow = dow * 24.0 * 3600.0 + tt.sod;

    return dow;
} /* end of time2gpst */

/**
* @brief   : bdst2time - the conversion from BDS week, seconds of week to time
* @param[I]: week (BDS week)
* @param[I]: sow (seconds of week)
* @param[O]: none
* @return  : time struct (.mjd and .sod)
* @note    :
**/
gtime_t TimeUtil::bdst2time(int week, double sow)
{
    /* compute the time of the BDS starting time (2006-01-01 00:00:00) */
    gtime_t t_bdst0 = ymdhms2time(bdst0);

    double dt = week * 7 * 86400.0 + sow;
    gtime_t tt = TimeAdd(t_bdst0, dt);

    return tt;
} /* end of bdst2time */

/**
* @brief   : time2bdst - the conversion from time to BDS week, seconds of week
* @param[I]: tt (time struct (.mjd and .sod))
* @param[O]: week (BDS week, nullptr:NO output)
* @param[O]: sow (seconds of week, nullptr:NO output)
* @return  : day of week
* @note    :
**/
int TimeUtil::time2bdst(gtime_t tt, int *week, double *sow)
{
    /* compute the time of the BDS starting time (2006-01-01 00:00:00) */
    gtime_t t_bdst0 = ymdhms2time(bdst0);

    /* compute the time differences */
    double delta_day = TimeDiff(tt, t_bdst0) / 86400.0;;

    /* compute the BDS week */
    int week_tmp = (int)floor(delta_day / 7.0 + 1.0e-9);
    if (week) *week = week_tmp;

    /* compute the days from the last BDS week to current day */
    int dow = (int)floor(delta_day - week_tmp * 7 + 1.0e-9);

    /* compute the seconds within BDS week */
    if (sow) *sow = dow * 24.0 * 3600.0 + tt.sod;

    return dow;
} /* end of time2bdst */

/**
* @brief   : bdst2gpst - the conversion from BDS time to GPS time
* @param[I]: tt_bds (BDS time struct (.mjd and .sod))
* @param[O]: none
* @return  : GPS time struct (.mjd and .sod)
* @note    :
**/
gtime_t TimeUtil::bdst2gpst(gtime_t tt_bds)
{
    return TimeAdd(tt_bds, 14.0);
} /* end of bdst2gpst */

/**
* @brief   : gpst2bdst - the conversion from GPS time to BDS time
* @param[I]: tt_gps (GPS time struct (.mjd and .sod))
* @param[O]: none
* @return  : BDS time struct (.mjd and .sod)
* @note    :
**/
gtime_t TimeUtil::gpst2bdst(gtime_t tt_gps)
{
    return TimeAdd(tt_gps, -14.0);
} /* end of gpst2bdst */

/**
* @brief   : gpst2utc - the conversion from GPS time to UTC time considering leap seconds
* @param[I]: tt_gps (GPS time struct (.mjd and .sod))
* @param[O]: none
* @return  : UTC time struct (.mjd and .sod)
* @note    : ignore slight time offset under 100 ns
**/
gtime_t TimeUtil::gpst2utc(gtime_t tt_gps)
{
    gtime_t tu;
    for (int i = 0; leaps[i][0] > 0; i++)
    {
        tu = TimeAdd(tt_gps, leaps[i][6]);
        if (TimeDiff(tu, ymdhms2time(leaps[i])) >= 0.0) return tu;
    }

    return tt_gps;
} /* end of gpst2utc */

/**
* @brief   : utc2gpst - the conversion from UTC time to GPS time considering leap seconds
* @param[I]: tt_utc (UTC time struct (.mjd and .sod))
* @param[O]: none
* @return  : GPS time struct (.mjd and .sod)
* @note    : ignore slight time offset under 100 ns
**/
gtime_t TimeUtil::utc2gpst(gtime_t tt_utc)
{
    for (int i = 0; leaps[i][0] > 0; i++)
    {
        if (TimeDiff(tt_utc, ymdhms2time(leaps[i])) >= 0.0) return TimeAdd(tt_utc, -leaps[i][6]);
    }

    return tt_utc;
} /* end of utc2gpst */

/**
* @brief   : str2time - the conversion from substring in string to gtime_t struct
* @param[I]: s (string ("... yyyy mm dd hh mm ss ..."))
* @param[I]: i,n (substring position and width)
* @param[O]: tt (time struct (.mjd and .sod))
* @return  : true:ok, false:error
* @note    :
**/
bool TimeUtil::str2time(const char *s, int i, int n, gtime_t &tt)
{
    char str[256], *p = str;

    if (i < 0 || (int)strlen(s) < i || (int)sizeof(str) - 1 < i) return false;
    for (s += i; *s && --n >= 0;) *p++ = *s++; *p = '\0';
    double date[6];
    if (sscanf(str, "%lf %lf %lf %lf %lf %lf", &date[0], &date[1], &date[2],
        &date[3], &date[4], &date[5]) < 6) return false;
    if (date[0] < 100.0) date[0] += date[0] < 80.0 ? 2000.0 : 1900.0;
    tt = ymdhms2time(date);

    return true;
} /* end of str2time */

/**
* @brief   : time2str - the conversion from gtime_t struct to string
* @param[I]: tt (time struct (.mjd and .sod))
* @param[I]: n (number of decimals)
* @param[O]: s (string ("yyyy/mm/dd hh:mm:ss.ssss"))
* @return  : none
* @note    :
**/
void TimeUtil::time2str(gtime_t tt, char *s, int n)
{
    int nn = n;
    if (nn < 0) nn = 0;
    else if (nn > 12) nn = 12;

    double date[6];
    time2ymdhms(tt, date);
    sprintf(s, "%04.0f/%02.0f/%02.0f %02.0f:%02.0f:%0*.*f", date[0], date[1], date[2],
        date[3], date[4], nn <= 0 ? 2 : nn + 3, nn <= 0 ? 0 : nn, date[5]);
} /* end of time2str */

/**
* @brief   : TimeStr - get time string
* @param[I]: tt (time struct (.mjd and .sod))
* @param[I]: n (number of decimals)
* @param[O]: none
* @return  : time string
* @note    :
**/
char* TimeUtil::TimeStr(gtime_t tt, int n)
{
    static char buff[64];
    time2str(tt, buff, n);

    return buff;
} /* end of TimeStr */