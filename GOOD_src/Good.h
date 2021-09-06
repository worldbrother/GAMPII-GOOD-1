/*------------------------------------------------------------------------------
* iPPP_RTK.h : constants, types and function prototypes
*
* Copyright (C) 2020-2099 by Surveying and Spatial Information, all rights reserved.
*    This file is part of GOOD (Gnss Observation and Other data Downloading) tool
*
* history : 2020/08/16 1.0  new (by Feng Zhou)
*-----------------------------------------------------------------------------*/
#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#ifdef _WIN32  /* for Windows */
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <direct.h>
#include "win\unistd.h"
#else          /* for Linux or Mac */
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#endif


/* load namespace */
using namespace std;

/* constant definitions ----------------------------------------------------------*/
#define MAXCHARS        1024      /* maximum characters in one line */
#define MAXSTRPATH      1024      /* max length of stream path */

#ifdef _WIN32  /* for Windows */
#define FILEPATHSEP '\\'
#define strcasecmp  _stricmp
#define access      _access
#else          /* for Linux or Mac */
#define FILEPATHSEP '/'
#endif

/* type definitions ----------------------------------------------------------*/
struct gtime_t
{                                 /* time struct */
    int mjd;                      /* modified Julian date (MJD) */
    double sod;                   /* seconds of the day */
};

struct ftpopt_t
{                                 /* the type of GNSS data downloading */
    bool ftpDownloading;          /* the master switch for data downloading, 0:off  1:on, only for data downloading */
    char ftpFrom[MAXCHARS];       /* FTP archive: CDDIS, IGN, or WHU */
    bool getObs;                  /* (0:off  1:on) IGS observation (RINEX version 2.xx, short name 'd') */
    char obsTyp[MAXCHARS];        /* 'daily', 'hourly', or 'highrate' */
    char obsOpt[MAXCHARS];        /* 'all'; the full path of 'site.list' */
    std::vector<int> hhObs;       /* hours array (0: 00:00, 1: 1:00, 2: 2:00, 3: 3:00, ...) */
    bool getObm;                  /* (0:off  1:on) MGEX observation (RINEX version 3.xx, long name 'crx') */
    char obmTyp[MAXCHARS];        /* 'daily', 'hourly', or 'highrate' */
    char obmOpt[MAXCHARS];        /* 'all'; the full path of 'site.list' */
    std::vector<int> hhObm;       /* hours array (0: 00:00, 1: 1:00, 2: 2:00, 3: 3:00, ...) */
    bool getObc;                  /* (0:off  1:on) Curtin University of Technology (CUT) observation (RINEX version 3.xx, long name 'crx') */
    char obcTyp[MAXCHARS];        /* only 'daily' is valid */
    char obcOpt[MAXCHARS];        /* only the full path of 'site.list' */
    bool getObg;                  /* (0:off  1:on) Geoscience Australia (GA) observation (RINEX version 3.xx, long name 'crx') */
    char obgTyp[MAXCHARS];        /* 'daily', 'hourly', or 'highrate' */
    char obgOpt[MAXCHARS];        /* 'all'; the full path of 'site.list' */
    std::vector<int> hhObg;       /* hours array (0: 00:00, 1: 1:00, 2: 2:00, 3: 3:00, ...) */
    bool getObh;                  /* (0:off  1:on) Hong Kong CORS observation (RINEX version 3.xx, long name 'crx') */
    char obhTyp[MAXCHARS];        /* '30s', '5s', or '1s' */
    char obhOpt[MAXCHARS];        /* only the full path of 'site.list' is valid */
    std::vector<int> hhObh;       /* hours array (0: 00:00, 1: 1:00, 2: 2:00, 3: 3:00, ...) */
    bool getObn;                  /* (0:off  1:on) NGS/NOAA CORS observation (RINEX version 2.xx, short name 'd') */
    char obnTyp[MAXCHARS];        /* only 'daily' is valid */
    char obnOpt[MAXCHARS];        /* only the full path of 'site.list' */
    bool getObe;                  /* (0:off  1:on) EUREF Permanent Network (EPN) observation (RINEX version 3.xx, long name 'crx' and RINEX version 2.xx, short name 'd') */
    char obeTyp[MAXCHARS];        /* only 'daily' is valid */
    char obeOpt[MAXCHARS];        /* 'all'; the full path of 'site.list' */
    bool getNav;                  /* (0:off  1:on) broadcast ephemeris */
    char navTyp[MAXCHARS];        /* 'daily', 'hourly', or 'RTNAV' */
    char navOpt[MAXCHARS];        /* 'gps', 'glo', 'bds', 'gal', 'qzs', 'irn', 'mixed' or 'all' */
    std::vector<int> hhNav;       /* hours array (0: 00:00, 1: 1:00, 2: 2:00, 3: 3:00, ...) */
    bool getOrbClk;               /* (0:off  1:on) precise orbit and clock */
    char orbClkOpt[MAXCHARS];     /* analysis center (i.e., final: "cod", "emr", "esa", "gfz", "grg", "igs", "jpl", "mit", "com", "gbm", "grm", "wum"; rapid: "cor", "emp", "esr", "gfr", "igr"; ultra-rapid: "esu", "gfu", "igu", "wuu") */
    std::vector<int> hhOrbClk;    /* hours array (0: 00:00, 6: 6:00, 12: 12:00, 18: 18:00 for esu and/or igu; 0: 00:00, 3: 3:00, 6: 6:00, ... for gfu; 0: 00:00, 1: 1:00, 2: 2:00, ... for wuu) */
    bool getEop;                  /* (0:off  1:on) earth rotation parameter */
    char eopOpt[MAXCHARS];        /* analysis center (i.e., final: "cod", "emr", "esa", "gfz", "grg", "igs", "jpl", "mit"; ultra: "esu", "gfu", "igu") */
    std::vector<int> hhEop;       /* hours array (0: 00:00, 6: 6:00, 12: 12:00, 18: 18:00 for esu and/or igu; 0: 00:00, 3: 3:00, 6: 6:00, ... for gfu) */
    bool getSnx;                  /* (0:off  1:on) IGS weekly SINEX */
    bool getDcb;                  /* (0:off  1:on) CODE and/or MGEX differential code bias (DCB) */
    bool getIon;                  /* (0:off  1:on) global ionosphere map (GIM) */
    char ionOpt[10];              /* analysis center (i.e., igs, cod, cas, ...) */
    bool getRoti;                 /* (0:off  1:on) rate of TEC index (ROTI) */
    bool getTrp;                  /* (0:off  1:on) CODE and/or IGS tropospheric product */
    char trpOpt[10];              /* analysis center (i.e., igs, cod, ...). If 'igs' is set, it will choose the 3rd of 'getObs' for site-specific tropospheric product */
    bool getRtOrbClk;             /* (0:off  1:on) real-time precise orbit and clock products from CNES offline files */
    bool getRtBias;               /* (0:off  1:on) real-time code and phase bias products from CNES offline files */
    bool getAtx;                  /* (0:off  1:on) ANTEX format antenna phase center correction */
    char dir3party[MAXSTRPATH];   /* (optional) the directory where third-party softwares (i.e., 'wget', 'gzip', 'crx2rnx' etc) are stored. This option is not needed if you have set the path or environment variable for them */
    bool isPath3party;            /* if true: the path need be set for third-party softwares */
    char wgetFull[MAXCHARS];      /* if isPath3party == true, set the full path where 'wget' is */
    char gzipFull[MAXCHARS];      /* if isPath3party == true, set the full path where 'gzip' is */
    char crx2rnxFull[MAXCHARS];   /* if isPath3party == true, set the full path where 'crx2rnx' is */
    bool minusAdd1day;            /* (0:off  1:on) the day before and after the current day for precise satellite orbit and clock products downloading */
    bool printInfoWget;           /* (0:off  1 : on) print the information generated by 'wget' */
    char qr[MAXCHARS];            /* "-qr" (printInfoWget == false) or "r" (printInfoWget == true) in 'wget' command line */
};

struct prcopt_t
{                                 /* processing options type */
    /* processing directory */
    char mainDir[MAXSTRPATH];     /* the root/main directory of GNSS observations and products */
                                  /* the settings below are the sub-directories, and two parameters are needed
                                     1st: (0  1), 0: use the path of root/main directory; 1: NOT use the path of root/main directory
                                     If '0' is set, the full path will NOT be needed, i.e., 'obsDir         = 0  obs', indicating 'obsDir         = D:\data\obs'
                                     If '1' is set, the full path should be given, i.e., 'obsDir         = 0  D:\data\obs'
                                  */
    char obsDir[MAXSTRPATH];      /* the directory of IGS RINEX format observation files (short name "d" files) */
    char obmDir[MAXSTRPATH];      /* the directory of MGEX RINEX format observation files (long name "crx" files) */
    char obcDir[MAXSTRPATH];      /* the directory of Curtin University of Technology (CUT) RINEX format observation files (long name "crx" files) */
    char obgDir[MAXSTRPATH];      /* the directory of Geoscience Australia (GA) RINEX format observation files (long name "crx" files) */
    char obhDir[MAXSTRPATH];      /* the directory of Hong Kong CORS RINEX format observation files (long name "crx" files) */
    char obnDir[MAXSTRPATH];      /* the directory of NGS/NOAA CORS RINEX format observation files (short name "d" files) */
    char obeDir[MAXSTRPATH];      /* the directory of EUREF Permanent Network (EPN) RINEX format observation files (long name 'crx' files and short name 'd' files) */
    char navDir[MAXSTRPATH];      /* the directory of RINEX format broadcast ephemeris files */
    char sp3Dir[MAXSTRPATH];      /* the directory of SP3 format precise ephemeris files */
    char clkDir[MAXSTRPATH];      /* the directory of RINEX format precise clock files */
    char eopDir[MAXSTRPATH];      /* the directory of earth rotation parameter files */
    char snxDir[MAXSTRPATH];      /* the directory of SINEX format IGS weekly solution files */
    char dcbDir[MAXSTRPATH];      /* the directory of CODE and/or MGEX differential code bias (DCB) files */
    char biaDir[MAXSTRPATH];      /* the directory of CNES real-time phase bias files */
    char ionDir[MAXSTRPATH];      /* the directory of CODE and/or IGS global ionosphere map (GIM) files */
    char ztdDir[MAXSTRPATH];      /* the directory of CODE and/or IGS tropospheric product files */
    char tblDir[MAXSTRPATH];      /* the directory of table files for processing */

    /* time settings */
    gtime_t ts;                   /* start time for processing */
    int ndays;                    /* number of consecutive days */
};