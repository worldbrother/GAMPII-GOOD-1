/*------------------------------------------------------------------------------
* TimeUtil.h : header file of TimeUtil.cpp
*-----------------------------------------------------------------------------*/
#pragma once

class TimeUtil
{
private:

    /**
    * @brief   : LeapYear - to check if it is leap year
    * @param[I]: year (4-digit year)
    * @param[O]: none
    * @return  : true, leap year; false, not leap year
    * @note    :
    **/
    bool LeapYear(int year);

    /**
    * @brief   : yy2yyyy - the conversion from 2-digit year to 4-digit year
    * @param[I]: yy (2-digit year)
    * @param[O]: none
    * @return  : 4-digit year
    * @note    :
    **/
    int yy2yyyy(int yy);

    /**
    * @brief   : sod2hms - the conversion from seconds of day to hour, minute, and second
    * @param[I]: sod (seconds of day)
    * @param[O]: hh (hour, nullptr:NO output)
    * @param[O]: minu (minute, nullptr:NO output)
    * @param[O]: sec (second, nullptr:NO output)
    * @return  : none
    * @note    :
    **/
    void sod2hms(double sod, int *hh, int *minu, double *sec);

    /**
    * @brief   : hms2sod - the conversion from hour, minute, and second to seconds of day
    * @param[I]: hh (hour)
    * @param[I]: minu (minute)
    * @param[I]: sec (second)
    * @param[O]: none
    * @return  : seconds of day
    * @note    :
    **/
    double hms2sod(int hh, int minu, double sec);

public:
    TimeUtil()
	{

	}
	~TimeUtil()
	{

	}

    /**
    * @brief   : yyyy2yy - the conversion from 4-digit year to 2-digit year
    * @param[I]: yyyy (4-digit year)
    * @param[O]: none
    * @return  : 2-digit year
    * @note    :
    **/
    int yyyy2yy(int yyyy);

    /**
    * @brief   : TimeAdd - to increase time t0 by dt (in seconds), return t1 with t1(1) < 604800.0
    * @param[I]: t0 (time struct (.mjd and .sod))
    * @param[I]: dt (time struct (increased seconds))
    * @param[O]: none
    * @return  : time struct (.mjd and .sod)
    * @note    :
    **/
    gtime_t TimeAdd(gtime_t t0, double dt);

    /**
    * @brief   : TimeDiff - time difference (t1-t0), return in seconds
    * @param[I]: t1, t0 (time struct (.mjd and .sod))
    * @param[O]: none
    * @return  : time difference in seconds
    * @note    : the first time subtracts the second time
    **/
    double TimeDiff(gtime_t t1, gtime_t t0);

    /**
    * @brief   : time2fmjd - the conversion from time to modified Julian date
    * @param[I]: tt (time struct (.mjd and .sod))
    * @param[O]: none
    * @return  : modified Julian date
    * @note    :
    **/
    double time2fmjd(gtime_t tt);

    /**
    * @brief   : fmjd2time - the conversion from modified Julian date to time
    * @param[I]: modified Julian date
    * @param[O]: none
    * @return  : time struct (.mjd and .sod)
    * @note    :
    **/
    gtime_t fmjd2time(double fmjd);

    /**
    * @brief   : ymdhms2time - the conversion from year, month, day, hour, minute, second to time
    * @param[I]: date (6 x 1 vector indicating year, month, day, hour, minute, and second)
    * @param[O]: none
    * @return  : time struct (.mjd and .sod)
    * @note    :
    **/
    gtime_t ymdhms2time(const double *date);

    /**
    * @brief   : time2ymdhms - the conversion from time to year, month, day, hour, minute, second
    * @param[I]: tt (time struct (.mjd and .sod))
    * @param[O]: date (6 x 1 vector indicating year, month, day, hour, minute, and second)
    * @return  : none
    * @note    :
    **/
    void time2ymdhms(gtime_t tt, double *date);

    /**
    * @brief   : yrdoy2time - the conversion from year, day of year to time
    * @param[I]: year (4- or 2-digit year)
    * @param[I]: doy (day of year)
    * @param[O]: none
    * @return  : time struct (.mjd and .sod)
    * @note    :
    **/
    gtime_t yrdoy2time(int year, int doy);

    /**
    * @brief   : time2yrdoy - the conversion from time to year, day of year
    * @param[I]: tt (time struct (.mjd and .sod))
    * @param[O]: year (4-digit year, nullptr:NO output)
    * @param[O]: doy (day of year, nullptr:NO output)
    * @return  : none
    * @note    :
    **/
    void time2yrdoy(gtime_t tt, int *year, int *doy);

    /**
    * @brief   : ymd2yrdoy - the conversion from year, month, day to year, day of year
    * @param[I]: year (2- or 4-digit year)
    * @param[I]: month (1-12)
    * @param[I]: day (day within a month, 1-31)
    * @param[O]: none
    * @return  : doy (day of year)
    * @note    :
    **/
    int ymd2yrdoy(int year, int month, int day);

    /**
    * @brief   : yrdoy2ymd - the conversion from year, day of year to year, month, day
    * @param[I]: year (2- or 4-digit year)
    * @param[I]: doy (day of year)
    * @param[O]: month (1-12, nullptr:NO output)
    * @param[O]: day (day within a month, 1-31, nullptr:NO output)
    * @return  : none
    * @note    :
    **/
    void yrdoy2ymd(int year, int doy, int *month, int *day);

    /**
    * @brief   : gpst2time - the conversion from GPS week, seconds of week to time
    * @param[I]: week (GPS week)
    * @param[I]: sow (seconds of week)
    * @param[O]: none
    * @return  : time struct (.mjd and .sod)
    * @note    :
    **/
    gtime_t gpst2time(int week, double sow);

    /**
    * @brief   : time2gpst - the conversion from time to GPS week, seconds of week
    * @param[I]: tt (time struct (.mjd and .sod))
    * @param[O]: week (GPS week, nullptr:NO output)
    * @param[O]: sow (seconds of week, nullptr:NO output)
    * @return  : day of week
    * @note    :
    **/
    int time2gpst(gtime_t tt, int *week, double *sow);

    /**
    * @brief   : bdst2time - the conversion from BDS week, seconds of week to time
    * @param[I]: week (BDS week)
    * @param[I]: sow (seconds of week)
    * @param[O]: none
    * @return  : time struct (.mjd and .sod)
    * @note    :
    **/
    gtime_t bdst2time(int week, double sow);

    /**
    * @brief   : time2bdst - the conversion from time to BDS week, seconds of week
    * @param[I]: tt (time struct (.mjd and .sod))
    * @param[O]: week (BDS week, nullptr:NO output)
    * @param[O]: sow (seconds of week, nullptr:NO output)
    * @return  : day of week
    * @note    :
    **/
    int time2bdst(gtime_t tt, int *week, double *sow);

    /**
    * @brief   : bdst2gpst - the conversion from BDS time to GPS time
    * @param[I]: tt_bds (BDS time struct (.mjd and .sod))
    * @param[O]: none
    * @return  : GPS time struct (.mjd and .sod)
    * @note    :
    **/
    gtime_t bdst2gpst(gtime_t tt_bds);

    /**
    * @brief   : gpst2bdst - the conversion from GPS time to BDS time
    * @param[I]: tt_gps (GPS time struct (.mjd and .sod))
    * @param[O]: none
    * @return  : BDS time struct (.mjd and .sod)
    * @note    :
    **/
    gtime_t gpst2bdst(gtime_t tt_gps);

    /**
    * @brief   : gpst2utc - the conversion from GPS time to UTC time considering leap seconds
    * @param[I]: tt_gps (GPS time struct (.mjd and .sod))
    * @param[O]: none
    * @return  : UTC time struct (.mjd and .sod)
    * @note    : ignore slight time offset under 100 ns
    **/
    gtime_t gpst2utc(gtime_t tt_gps);

    /**
    * @brief   : utc2gpst - the conversion from UTC time to GPS time considering leap seconds
    * @param[I]: tt_utc (UTC time struct (.mjd and .sod))
    * @param[O]: none
    * @return  : GPS time struct (.mjd and .sod)
    * @note    : ignore slight time offset under 100 ns
    **/
    gtime_t utc2gpst(gtime_t tt_utc);

    /**
    * @brief   : str2time - the conversion from substring in string to gtime_t struct
    * @param[I]: s (string ("... yyyy mm dd hh mm ss ..."))
    * @param[I]: i,n (substring position and width)
    * @param[O]: tt (time struct (.mjd and .sod))
    * @return  : true:ok, false:error
    * @note    :
    **/
    bool str2time(const char *s, int i, int n, gtime_t &tt);

    /**
    * @brief   : time2str - the conversion from gtime_t struct to string
    * @param[I]: tt (time struct (.mjd and .sod))
    * @param[I]: n (number of decimals)
    * @param[O]: s (string ("yyyy/mm/dd hh:mm:ss.ssss"))
    * @return  : none
    * @note    :
    **/
    void time2str(gtime_t tt, char *s, int n);

    /**
    * @brief   : TimeStr - get time string
    * @param[I]: tt (time struct (.mjd and .sod))
    * @param[I]: n (number of decimals)
    * @param[O]: none
    * @return  : time string
    * @note    :
    **/
    char* TimeStr(gtime_t tt, int n);
};