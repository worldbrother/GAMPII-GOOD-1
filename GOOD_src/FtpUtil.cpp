/*------------------------------------------------------------------------------
* FtpUtil.cpp : GNSS data downloading module
*
* Copyright (C) 2020-2099 by SpAtial SurveyIng and Navigation (SASIN) Group, all rights reserved.
*    This file is part of GAMP II - GOOD (Gnss Observations and prOducts Downloader)
*
* References:
*    Dr Cheng Wang @ BUAA provided the C++ code for observation downloading as reference
*
* history : 2021/04/16 1.0  new (by Feng Zhou @ SDUST)
*           2021/04/20      MGEX multi-GNSS precise products option added (by Feng Zhou @ SDUST)
*           2021/04/23 1.1  IGN and WHU FPT archives added (by Feng Zhou @ SDUST)
*           2021/04/30      the day before and after the current day for precise satellite 
                               orbit and clock products downloading is added (by Feng Zhou @ SDUST and Zan Liu @ CUMT)
*           2021/04/30 1.2  CNES real-time orbit, clock, bias files (in offline mode) option added (by Feng Zhou @ SDUST)
*           2021/05/01      from DOY 345/2020, 'brdcDDD0.YYn' was converted from '*.Z' to '*.gz' (by Feng Zhou @ SDUST and Zan Liu @ CUMT)
*           2021/05/01      added the full path of third-party softwares (by Feng Zhou @ SDUST and Yudan Yi)
*           2021/05/05 1.3  add 'GetRoti' for Rate of TEC index (ROTI) file downloading (by Feng Zhou @ SDUST and Lei Liu @ WHU)
*           2021/05/06 1.4  add the compatibility for changing directory (chdir) in Windows and Linux OS (by Feng Zhou @ SDUST)
*           2021/05/07      add the option 'printInfoWget' in configure file for (not) printing the information of running 'wget' (by Feng Zhou @ SDUST and Hong Hu @ AHU)
*           2021/05/08      add IGS and MGEX hourly observation (30s) downloading (by Feng Zhou @ SDUST and Menghao Li @ HEU)
*           2021/05/10 1.5  modify some codes to make GOOD adaptable to different compression form (i.e., '*.Z' or '*.gz') (by Feng Zhou @ SDUST)
*           2021/05/12      add the option for IGR, IGU, GFU (from GFZ), and WUU (from WHU) products as well as the site-specific broadcast ephemeris files downloading (by Feng Zhou @ SDUST and Menghao Li @ HEU)
*           2021/05/15      sub-directory (i.e., daily, hourly, and highrate) creation for observation downloading (by Feng Zhou @ SDUST and Menghao Li @ HEU)
*           2021/05/18 1.6  modify some codes to make configuration file and program look more concise (by Feng Zhou @ SDUST and Menghao Li @ HEU)
*           2021/05/21      add the extraction and conversion for the 'all' option in IGS and MGEX observation downloading (by Feng Zhou @ SDUST)
*           2021/06/01 1.7  add 'getObc', 'getObg', and 'getObh' options for Curtin University of Technology (CUT), Geoscience Australia (GA), and Hong Kong CORS observation downloading (by Feng Zhou @ SDUST)
*           2021/07/16      modify "*_R_" to "*" for CRX file naming convention (by Feng Zhou @ SDUST)
*           2021/09/01 1.8  add various types of ultra-rapid (i.e., ESA, GFZ, IGS, WHU) and rapid (i.e., CODE, NRCan, ESA, GFZ, IGS) orbit and clock products for downloading (by Feng Zhou @ SDUST and Sermet Ogutcu @ NEU)
            2021/09/04      add the option for NGS/NOAA CORS (from Amazon Web Services (AWS)) and EUREF Permanent Network (EPN) observation downloading (by Feng Zhou @ SDUST)
            2021/09/05      fix the bug of broadcast ephemeris downloading via WHU FTP before and after 2020 (by Feng Zhou @ SDUST and Zan Liu @ CUMT)
            2021/09/06      modify the setting of directories of GNSS observations and products in configure file to make them more elastic (by Feng Zhou @ SDUST and Caoming Fan @ SDU)
*-----------------------------------------------------------------------------*/
#include "Good.h"
#include "TimeUtil.h"
#include "StringUtil.h"
#include "FtpUtil.h"


/* constants/macros ----------------------------------------------------------*/
#define IDX_OBSD   0    /* index for IGS daily observation (30s) file downloaded (short name "d" files) */
#define IDX_OBSH   1    /* index for IGS hourly observation (30s) file downloaded (short name "d" files) */
#define IDX_OBSHR  2    /* index for IGS high-rate observation (1s) file downloaded (short name "d" files) */
#define IDX_OBMD   3    /* index for MGEX daily observation (30s) file downloaded (long name "crx" files) */
#define IDX_OBMH   4    /* index for MGEX daily observation (30s) file downloaded (long name "crx" files) */
#define IDX_OBMHR  5    /* index for MGEX high-rate observation (1s) file downloaded (long name "crx" files) */
#define IDX_NAV    6    /* index for broadcast ephemeris file downloaded */
#define IDX_SP3    7    /* index for IGS SP3 file downloaded */
#define IDX_CLK    8    /* index for IGS CLK file downloaded */
#define IDX_EOP    9    /* index for IGS EOP file downloaded */
#define IDX_SNX    10   /* index for IGS weekly SINEX file downloaded */
#define IDX_SP3M   11   /* index for MGEX SP3 file downloaded */
#define IDX_CLKM   12   /* index for MGEX CLK file downloaded */
#define IDX_DCBM   13   /* index for MGEX DCB file downloaded */
#define IDX_ION    14   /* index for global ionosphere map (GIM) downloaded */
#define IDX_ROTI   15   /* index for Rate of TEC index (ROTI) downloaded */
#define IDX_ZTD    16   /* index for IGS final tropospheric product downloaded */


/* function definition -------------------------------------------------------*/

/**
* @brief   : init - Get FTP archive for CDDIS, IGN, or WHU
* @param[I]: none
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::init()
{
    /* FTP archive for CDDIS */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/daily");                /* IGS daily observation (30s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/hourly");               /* IGS hourly observation (30s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/highrate");             /* IGS high-rate observation (1s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/daily");                /* MGEX daily observation (30s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/hourly");               /* MGEX hourly observation (30s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/highrate");             /* MGEX high-rate observation (1s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/daily");                /* broadcast ephemeris files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products");                  /* IGS SP3 files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products");                  /* IGS CLK files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products");                  /* IGS EOP files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products");                  /* IGS weekly SINEX files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/mgex");             /* MGEX SP3 files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/mgex");             /* MGEX CLK files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/bias");             /* MGEX DCB files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/ionex");            /* global ionosphere map (GIM) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/ionex");            /* Rate of TEC index (ROTI) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/troposphere/zpd");  /* IGS final tropospheric product files */

    /* FTP archive for IGN */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data");                                         /* IGS daily observation (30s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data/hourly");                                  /* IGS hourly observation (30s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data/highrate");                                /* IGS high-rate observation (1s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data");                                         /* MGEX daily observation (30s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data/hourly");                                  /* MGEX hourly observation (30s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data/highrate");                                /* MGEX high-rate observation (1s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data");                                         /* broadcast ephemeris files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products");                                     /* IGS SP3 files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products");                                     /* IGS CLK files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products");                                     /* IGS EOP files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products");                                     /* IGS weekly SINEX files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/mgex");                                /* MGEX SP3 files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/mgex");                                /* MGEX CLK files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/mgex/dcb");                            /* MGEX DCB files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/ionosphere");                          /* global ionosphere map (GIM) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/ionosphere");                          /* Rate of TEC index (ROTI) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/troposphere");                         /* IGS final tropospheric product files */

    /* FTP archive for WHU */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data/daily");                               /* IGS daily observation (30s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data/hourly");                              /* IGS hourly observation (30s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data");                                     /* IGS high-rate observation (1s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data/daily");                               /* MGEX daily observation (30s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data/hourly");                              /* MGEX hourly observation (30s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data");                                     /* MGEX high-rate observation (1s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data/daily");                               /* broadcast ephemeris files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products");                                 /* IGS SP3 files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products");                                 /* IGS CLK files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products");                                 /* IGS EOP files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products");                                 /* IGS weekly SINEX files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/mgex");                            /* MGEX SP3 files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/mgex");                            /* MGEX CLK files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/mgex/dcb");                        /* MGEX DCB files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/ionex");                           /* global ionosphere map (GIM) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/ionex");                           /* Rate of TEC index (ROTI) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/troposphere/new");                 /* IGS final tropospheric product files */
} /* end of init */

/**
* @brief   : GetDailyObsIgs - download IGS RINEX daily observation (30s) files (short name "d") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDailyObsIgs(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('daily') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(subDir.c_str());
#else           /* for Linux or Mac */
    chdir(subDir.c_str());
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obsOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the IGS observation files */
        string url, cutDirs = " --cut-dirs=7 ";
        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBSD] + "/" +
            sYyyy + "/" + sDoy + "/" + sYy + "d";
        else if (ftpName == "IGN")
        {
            url = _ftpArchive.IGN[IDX_OBSD] + "/" + sYyyy + "/" + sDoy;
            cutDirs = " --cut-dirs=5 ";
        }
        else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_OBSD] + "/" +
            sYyyy + "/" + sDoy + "/" + sYy + "d";
        else url = _ftpArchive.CDDIS[IDX_OBSD] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d";

        /* it is OK for '*.Z' or '*.gz' format */
        string dFile = "*" + sDoy + "0." + sYy + "d";
        string dxFile = dFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
        std::system(cmd.c_str());

        /* get the file list */
        string suffix = "." + sYy + "d";
        vector<string> dFiles;
        str.GetFilesAll(subDir, suffix, dFiles);
        char sitName[MAXCHARS];
        for (int i = 0; i < dFiles.size(); i++)
        {
            if (access(dFiles[i].c_str(), 0) == 0)
            {
                /* extract it */
                cmd = gzipFull + " -d -f " + dFiles[i];
                std::system(cmd.c_str());

                str.StrMid(sitName, dFiles[i].c_str(), 0, 4);
                string site = sitName;
                str.ToLower(site);
                dFile = site + sDoy + "0." + sYy + "d";
                if (access(dFile.c_str(), 0) == -1) continue;
                string oFile = site + sDoy + "0." + sYy + "o";
                /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                std::system(cmd.c_str());

                /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                cmd = "del " + dFile;
#else           /* for Linux or Mac */
                cmd = "rm -rf " + dFile;
#endif
                std::system(cmd.c_str());
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the IGS observation file site-by-site */
        if (access(fopt->obsOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obsOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetDailyObsIgs): open site.list = " << fopt->obsOpt << 
                    " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                str.ToLower(sitName);
                string oFile = sitName + sDoy + "0." + sYy + "o";
                string dFile = sitName + sDoy + "0." + sYy + "d";
                if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                {
                    string url, cutDirs = " --cut-dirs=7 ";
                    if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBSD] + "/" +
                        sYyyy + "/" + sDoy + "/" + sYy + "d";
                    else if (ftpName == "IGN")
                    {
                        url = _ftpArchive.IGN[IDX_OBSD] + "/" + sYyyy + "/" + sDoy;
                        cutDirs = " --cut-dirs=5 ";
                    }
                    else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_OBSD] + "/" +
                        sYyyy + "/" + sDoy + "/" + sYy + "d";
                    else url = _ftpArchive.CDDIS[IDX_OBSD] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d";

                    /* it is OK for '*.Z' or '*.gz' format */
                    string dxFile = dFile + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string dgzFile = dFile + ".gz", dzFile = dFile + ".Z";
                    if (access(dgzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + dgzFile;
                        std::system(cmd.c_str());
                    }
                    if (access(dFile.c_str(), 0) == -1 && access(dzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + dzFile;
                        std::system(cmd.c_str());
                    }
                    if (access(dFile.c_str(), 0) == -1)
                    {
                        cout << "*** WARNING(FtpUtil::GetDailyObsIgs): failed to download IGS daily observation file " << oFile << endl;

                        continue;
                    }

#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    if (access(oFile.c_str(), 0) == 0)
                    {
                        cout << "*** INFO(FtpUtil::GetDailyObsIgs): successfully download IGS daily observation file " << oFile << endl;

                        /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                        cmd = "del " + dFile;
#else           /* for Linux or Mac */
                        cmd = "rm -rf " + dFile;
#endif
                        std::system(cmd.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::GetDailyObsIgs): IGS daily observation file " << oFile << 
                    " or " << dFile << " has existed!" << endl;
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetDailyObsIgs */

/**
* @brief   : GetHourlyObsIgs - download IGS RINEX hourly observation (30s) files (short name "d") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetHourlyObsIgs(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('hourly') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "hourly");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obsOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the IGS observation files */
        for (int i = 0; i < fopt->hhObs.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhObs[i]);
            char tmpDir[MAXSTRPATH] = { '\0' };
            char sep = (char)FILEPATHSEP;
            sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
            string sHhDir = tmpDir;
            if (access(sHhDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + sHhDir;
#endif
                std::system(cmd.c_str());
            }

            /* change directory */
#ifdef _WIN32   /* for Windows */
            _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
            chdir(sHhDir.c_str());
#endif

            string url, cutDirs = " --cut-dirs=7 ";
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBSH] + "/" +
                sYyyy + "/" + sDoy + "/" + sHh;
            else if (ftpName == "IGN")
            {
                url = _ftpArchive.IGN[IDX_OBSH] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=6 ";
            }
            else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_OBSH] + "/" +
                sYyyy + "/" + sDoy + "/" + sHh;
            else url = _ftpArchive.CDDIS[IDX_OBSH] + "/" + sYyyy + "/" + sDoy + "/" + sHh;

            /* 'a' = 97, 'b' = 98, ... */
            int ii = fopt->hhObs[i] + 97;
            char ch = ii;
            string sch;
            sch.push_back(ch);
            /* it is OK for '*.Z' or '*.gz' format */
            string dFile = "*" + sDoy + sch + "." + sYy + "d";
            string dxFile = dFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
            std::system(cmd.c_str());

            /* get the file list */
            string suffix = "." + sYy + "d";
            vector<string> dFiles;
            str.GetFilesAll(sHhDir, suffix, dFiles);
            char sitName[MAXCHARS];
            for (int i = 0; i < dFiles.size(); i++)
            {
                if (access(dFiles[i].c_str(), 0) == 0)
                {
                    /* extract it */
                    cmd = gzipFull + " -d -f " + dFiles[i];
                    std::system(cmd.c_str());

                    str.StrMid(sitName, dFiles[i].c_str(), 0, 4);
                    string site = sitName;
                    str.ToLower(site);
                    dFile = site + sDoy + sch + "." + sYy + "d";
                    if (access(dFile.c_str(), 0) == -1) continue;
                    string oFile = site + sDoy + sch + "." + sYy + "o";
                    /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                    cmd = "del " + dFile;
#else           /* for Linux or Mac */
                    cmd = "rm -rf " + dFile;
#endif
                    std::system(cmd.c_str());
                }
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the IGS observation file site-by-site */
        if (access(fopt->obsOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obsOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHourlyObsIgs): open site.list = " << fopt->obsOpt << 
                    " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhObs.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhObs[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhObs[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);
                    str.ToLower(sitName);
                    string oFile = sitName + sDoy + sch + "." + sYy + "o";
                    string dFile = sitName + sDoy + sch + "." + sYy + "d";
                    if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                    {
                        string url, cutDirs = " --cut-dirs=7 ";
                        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBSH] + "/" +
                            sYyyy + "/" + sDoy + "/" + sHh;
                        else if (ftpName == "IGN")
                        {
                            url = _ftpArchive.IGN[IDX_OBSH] + "/" + sYyyy + "/" + sDoy;
                            cutDirs = " --cut-dirs=6 ";
                        }
                        else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_OBSH] + "/" +
                            sYyyy + "/" + sDoy + "/" + sHh;
                        else url = _ftpArchive.CDDIS[IDX_OBSH] + "/" + sYyyy + "/" + sDoy + "/" + sHh;

                        /* it is OK for '*.Z' or '*.gz' format */
                        string dxFile = dFile + ".*";
                        string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
                        std::system(cmd.c_str());

                        string dgzFile = dFile + ".gz", dzFile = dFile + ".Z";
                        if (access(dgzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.gz' */
                            cmd = gzipFull + " -d -f " + dgzFile;
                            std::system(cmd.c_str());
                        }
                        if (access(dFile.c_str(), 0) == -1 && access(dzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.Z' */
                            cmd = gzipFull + " -d -f " + dzFile;
                            std::system(cmd.c_str());
                        }
                        if (access(dFile.c_str(), 0) == -1)
                        {
                            cout << "*** WARNING(FtpUtil::GetHourlyObsIgs): failed to download IGS Hourly observation file " << oFile << endl;

                            continue;
                        }

#ifdef _WIN32  /* for Windows */
                        cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                        cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                        std::system(cmd.c_str());

                        if (access(oFile.c_str(), 0) == 0)
                        {
                            cout << "*** INFO(FtpUtil::GetHourlyObsIgs): successfully download IGS hourly observation file " << oFile << endl;

                            /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                            cmd = "del " + dFile;
#else           /* for Linux or Mac */
                            cmd = "rm -rf " + dFile;
#endif
                            std::system(cmd.c_str());
                        }
                    }
                    else cout << "*** INFO(FtpUtil::GetHourlyObsIgs): IGS hourly observation file " <<
                        oFile << " or " << dFile << " has existed!" << endl;
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetHourlyObsIgs */

/**
* @brief   : GetHrObsIgs - download IGS RINEX high-rate observation (1s) files (short name "d") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetHrObsIgs(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('highrate') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "highrate");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obsOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the IGS observation files */
        for (int i = 0; i < fopt->hhObs.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhObs[i]);
            char tmpDir[MAXSTRPATH] = { '\0' };
            char sep = (char)FILEPATHSEP;
            sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
            string sHhDir = tmpDir;
            if (access(sHhDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + sHhDir;
#endif
                std::system(cmd.c_str());
            }

            /* change directory */
#ifdef _WIN32   /* for Windows */
            _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
            chdir(sHhDir.c_str());
#endif

            string url, cutDirs = " --cut-dirs=8 ";
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBSHR] + "/" +
                sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;
            else if (ftpName == "IGN")
            {
                url = _ftpArchive.IGN[IDX_OBSHR] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=6 ";
            }
            else url = _ftpArchive.CDDIS[IDX_OBSHR] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;

            /* 'a' = 97, 'b' = 98, ... */
            int ii = fopt->hhObs[i] + 97;
            char ch = ii;
            string sch;
            sch.push_back(ch);
            /* it is OK for '*.Z' or '*.gz' format */
            string dFile = "*" + sDoy + sch + "*." + sYy + "d";
            string dxFile = dFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
            std::system(cmd.c_str());

            std::vector<string> minuStr = { "00", "15", "30", "45" };
            for (int i = 0; i < minuStr.size(); i++)
            {
                /* get the file list */
                string suffix = minuStr[i] + "." + sYy + "d";
                vector<string> dFiles;
                str.GetFilesAll(sHhDir, suffix, dFiles);
                char sitName[MAXCHARS];
                for (int j = 0; j < dFiles.size(); j++)
                {
                    if (access(dFiles[j].c_str(), 0) == 0)
                    {
                        /* extract it */
                        cmd = gzipFull + " -d -f " + dFiles[j];
                        std::system(cmd.c_str());

                        str.StrMid(sitName, dFiles[j].c_str(), 0, 4);
                        string site = sitName;
                        str.ToLower(site);
                        dFile = site + sDoy + sch + minuStr[i] + "." + sYy + "d";
                        if (access(dFile.c_str(), 0) == -1) continue;
                        string oFile = site + sDoy + sch + minuStr[i] + "." + sYy + "o";
                        /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                        cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                        cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                        std::system(cmd.c_str());

                        /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                        cmd = "del " + dFile;
#else           /* for Linux or Mac */
                        cmd = "rm -rf " + dFile;
#endif
                        std::system(cmd.c_str());
                    }
                }
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the IGS observation file site-by-site */
        if (access(fopt->obsOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obsOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHrObsIgs): open site.list = " << fopt->obsOpt << 
                    " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhObs.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhObs[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhObs[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);

                    std::vector<string> minuStr = { "00", "15", "30", "45" };
                    for (int i = 0; i < minuStr.size(); i++)
                    {
                        str.ToLower(sitName);
                        string oFile = sitName + sDoy + sch + minuStr[i] + "." + sYy + "o";
                        string dFile = sitName + sDoy + sch + minuStr[i] + "." + sYy + "d";
                        if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                        {
                            string url, cutDirs = " --cut-dirs=8 ";
                            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBSHR] + "/" +
                                sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;
                            else if (ftpName == "IGN")
                            {
                                url = _ftpArchive.IGN[IDX_OBSHR] + "/" + sYyyy + "/" + sDoy;
                                cutDirs = " --cut-dirs=6 ";
                            }
                            else url = _ftpArchive.CDDIS[IDX_OBSHR] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;

                            /* it is OK for '*.Z' or '*.gz' format */
                            string dxFile = dFile + ".*";
                            string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
                            std::system(cmd.c_str());

                            string dgzFile = dFile + ".gz", dzFile = dFile + ".Z";
                            if (access(dgzFile.c_str(), 0) == 0)
                            {
                                /* extract '*.gz' */
                                cmd = gzipFull + " -d -f " + dgzFile;
                                std::system(cmd.c_str());
                            }
                            if (access(dFile.c_str(), 0) == -1 && access(dzFile.c_str(), 0) == 0)
                            {
                                /* extract '*.Z' */
                                cmd = gzipFull + " -d -f " + dzFile;
                                std::system(cmd.c_str());
                            }
                            if (access(dFile.c_str(), 0) == -1)
                            {
                                cout << "*** WARNING(FtpUtil::GetHrObsIgs): failed to download IGS high-rate observation file " << oFile << endl;

                                continue;
                            }

#ifdef _WIN32  /* for Windows */
                            cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                            cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                            std::system(cmd.c_str());

                            if (access(oFile.c_str(), 0) == 0)
                            {
                                cout << "*** INFO(FtpUtil::GetHrObsIgs): successfully download IGS high-rate observation file " << oFile << endl;

                                /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                                cmd = "del " + dFile;
#else           /* for Linux or Mac */
                                cmd = "rm -rf " + dFile;
#endif
                                std::system(cmd.c_str());
                            }
                        }
                        else cout << "*** INFO(FtpUtil::GetHrObsIgs): IGS high-rate observation file " <<
                            oFile << " or " << dFile << " has existed!" << endl;
                    }
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetHrObsIgs */

/**
* @brief   : GetDailyObsMgex - download MGEX RINEX daily observation (30s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDailyObsMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('daily') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(subDir.c_str());
#else           /* for Linux or Mac */
    chdir(subDir.c_str());
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obmOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the MGEX observation files */
        string url, cutDirs = " --cut-dirs=7 ";
        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBMD] + "/" + 
            sYyyy + "/" + sDoy + "/" + sYy + "d";
        else if (ftpName == "IGN")
        {
            url = _ftpArchive.IGN[IDX_OBMD] + "/" + sYyyy + "/" + sDoy;
            cutDirs = " --cut-dirs=5 ";
        }
        else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_OBMD] + "/" + 
            sYyyy + "/" + sDoy + "/" + sYy + "d";
        else url = _ftpArchive.CDDIS[IDX_OBMD] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d";

        /* it is OK for '*.Z' or '*.gz' format */
        string crxFile = "*" + sYyyy + sDoy + "0000_01D_30S_MO.crx";
        string crxxFile = crxFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
        std::system(cmd.c_str());

        /* get the file list */
        string suffix = ".crx";
        vector<string> crxFiles;
        str.GetFilesAll(subDir, suffix, crxFiles);
        char sitName[MAXCHARS];
        for (int i = 0; i < crxFiles.size(); i++)
        {
            if (access(crxFiles[i].c_str(), 0) == 0)
            {
                /* extract it */
                cmd = gzipFull + " -d -f " + crxFiles[i];
                std::system(cmd.c_str());

                str.StrMid(sitName, crxFiles[i].c_str(), 0, 4);
                string site = sitName;
                str.ToLower(site);
                str.StrMid(sitName, crxFiles[i].c_str(), 0, (int)crxFiles[i].find_last_of('.'));
                crxFile = sitName;
                if (access(crxFile.c_str(), 0) == -1) continue;
                string oFile = site + sDoy + "0." + sYy + "o";
                /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                std::system(cmd.c_str());

                /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                cmd = "rm -rf " + crxFile;
#endif
                std::system(cmd.c_str());
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the MGEX observation file site-by-site */
        if (access(fopt->obmOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obmOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetDailyObsMgex): open site.list = " << fopt->obmOpt << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                str.ToLower(sitName);
                string oFile = sitName + sDoy + "0." + sYy + "o";
                string dFile = sitName + sDoy + "0." + sYy + "d";
                if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                {
                    string url, cutDirs = " --cut-dirs=7 ";
                    if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBMD] + "/" + 
                        sYyyy + "/" + sDoy + "/" + sYy + "d";
                    else if (ftpName == "IGN")
                    {
                        url = _ftpArchive.IGN[IDX_OBMD] + "/" + sYyyy + "/" + sDoy;
                        cutDirs = " --cut-dirs=5 ";
                    }
                    else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_OBMD] + "/" + 
                        sYyyy + "/" + sDoy + "/" + sYy + "d";
                    else url = _ftpArchive.CDDIS[IDX_OBMD] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d";

                    /* it is OK for '*.Z' or '*.gz' format */
                    str.ToUpper(sitName);
                    string crxFile = sitName + "*" + sYyyy + sDoy + "0000_01D_30S_MO.crx";
                    string crxxFile = crxFile + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
                    std::system(cmd.c_str());

                    /* extract '*.gz' */
                    string crxgzFile = crxFile + ".gz";
                    cmd = gzipFull + " -d -f " + crxgzFile;
                    std::system(cmd.c_str());
                    string changeFileName;
#ifdef _WIN32  /* for Windows */
                    changeFileName = "rename";
#else          /* for Linux or Mac */
                    changeFileName = "mv";
#endif
                    cmd = changeFileName + " " + crxFile + " " + dFile;
                    std::system(cmd.c_str());
                    if (access(dFile.c_str(), 0) == -1)
                    {
                        /* extract '*.Z' */
                        string crxzFile = crxFile + ".Z";
                        cmd = gzipFull + " -d -f " + crxzFile;
                        std::system(cmd.c_str());

                        cmd = changeFileName + " " + crxFile + " " + dFile;
                        std::system(cmd.c_str());
                        if (access(dFile.c_str(), 0) == -1)
                        {
                            cout << "*** WARNING(FtpUtil::GetDailyObsMgex): failed to download MGEX daily observation file " << oFile << endl;

                            continue;
                        }
                    }

#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    if (access(oFile.c_str(), 0) == 0)
                    {
                        cout << "*** INFO(FtpUtil::GetDailyObsMgex): successfully download MGEX daily observation file " << oFile << endl;

                        /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                        cmd = "del " + dFile;
#else           /* for Linux or Mac */
                        cmd = "rm -rf " + dFile;
#endif
                        std::system(cmd.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::GetDailyObsMgex): MGEX daily observation file " << oFile << 
                    " or " << dFile << " has existed!" << endl;
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetDailyObsMgex */

/**
* @brief   : GetHourlyObsMgex - download MGEX RINEX hourly observation (30s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetHourlyObsMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('hourly') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "hourly");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obmOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the MGEX observation files */
        for (int i = 0; i < fopt->hhObm.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhObm[i]);
            char tmpDir[MAXSTRPATH] = { '\0' };
            char sep = (char)FILEPATHSEP;
            sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
            string sHhDir = tmpDir;
            if (access(sHhDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + sHhDir;
#endif
                std::system(cmd.c_str());
            }

            /* change directory */
#ifdef _WIN32   /* for Windows */
            _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
            chdir(sHhDir.c_str());
#endif

            string url, cutDirs = " --cut-dirs=7 ";
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBMH] + "/" +
                sYyyy + "/" + sDoy + "/" + sHh;
            else if (ftpName == "IGN")
            {
                url = _ftpArchive.IGN[IDX_OBMH] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=6 ";
            }
            else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_OBMH] + "/" +
                sYyyy + "/" + sDoy + "/" + sHh;
            else url = _ftpArchive.CDDIS[IDX_OBMH] + "/" + sYyyy + "/" + sDoy + "/" + sHh;

            /* it is OK for '*.Z' or '*.gz' format */
            string crxFile = "*" + sYyyy + sDoy + sHh + "00_01H_30S_MO.crx";
            string crxxFile = crxFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
            std::system(cmd.c_str());

            /* 'a' = 97, 'b' = 98, ... */
            int ii = fopt->hhObm[i] + 97;
            char ch = ii;
            string sch;
            sch.push_back(ch);

            /* get the file list */
            string suffix = ".crx";
            vector<string> crxFiles;
            str.GetFilesAll(sHhDir, suffix, crxFiles);
            char sitName[MAXCHARS];
            for (int i = 0; i < crxFiles.size(); i++)
            {
                if (access(crxFiles[i].c_str(), 0) == 0)
                {
                    /* extract it */
                    cmd = gzipFull + " -d -f " + crxFiles[i];
                    std::system(cmd.c_str());

                    str.StrMid(sitName, crxFiles[i].c_str(), 0, 4);
                    string site = sitName;
                    str.ToLower(site);
                    str.StrMid(sitName, crxFiles[i].c_str(), 0, (int)crxFiles[i].find_last_of('.'));
                    crxFile = sitName;
                    if (access(crxFile.c_str(), 0) == -1) continue;
                    string oFile = site + sDoy + sch + "." + sYy + "o";
                    /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                    cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                    cmd = "rm -rf " + crxFile;
#endif
                    std::system(cmd.c_str());
                }
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the MGEX observation file site-by-site */
        if (access(fopt->obmOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obmOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHourlyObsMgex): open site.list = " << fopt->obmOpt << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhObm.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhObm[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhObm[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);
                    str.ToLower(sitName);
                    string oFile = sitName + sDoy + sch + "." + sYy + "o";
                    string dFile = sitName + sDoy + sch + "." + sYy + "d";
                    if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                    {
                        string url, cutDirs = " --cut-dirs=7 ";
                        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBMH] + "/" +
                            sYyyy + "/" + sDoy + "/" + sHh;
                        else if (ftpName == "IGN")
                        {
                            url = _ftpArchive.IGN[IDX_OBMH] + "/" + sYyyy + "/" + sDoy;
                            cutDirs = " --cut-dirs=6 ";
                        }
                        else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_OBMH] + "/" +
                            sYyyy + "/" + sDoy + "/" + sHh;
                        else url = _ftpArchive.CDDIS[IDX_OBMH] + "/" + sYyyy + "/" + sDoy + "/" + sHh;

                        /* it is OK for '*.Z' or '*.gz' format */
                        str.ToUpper(sitName);
                        string crxFile = sitName + "*" + sYyyy + sDoy + sHh + "00_01H_30S_MO.crx";
                        string crxxFile = crxFile + ".*";
                        string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
                        std::system(cmd.c_str());

                        /* extract '*.gz' */
                        string crxgzFile = crxFile + ".gz";
                        cmd = gzipFull + " -d -f " + crxgzFile;
                        std::system(cmd.c_str());
                        string changeFileName;
#ifdef _WIN32  /* for Windows */
                        changeFileName = "rename";
#else          /* for Linux or Mac */
                        changeFileName = "mv";
#endif
                        cmd = changeFileName + " " + crxFile + " " + dFile;
                        std::system(cmd.c_str());
                        if (access(dFile.c_str(), 0) == -1)
                        {
                            /* extract '*.Z' */
                            string crxzFile = crxFile + ".Z";
                            cmd = gzipFull + " -d -f " + crxzFile;
                            std::system(cmd.c_str());

                            cmd = changeFileName + " " + crxFile + " " + dFile;
                            std::system(cmd.c_str());
                            if (access(dFile.c_str(), 0) == -1)
                            {
                                cout << "*** WARNING(FtpUtil::GetDailyObsMgex): failed to download MGEX daily observation file " << oFile << endl;

                                continue;
                            }
                        }

#ifdef _WIN32  /* for Windows */
                        cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                        cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                        std::system(cmd.c_str());

                        if (access(oFile.c_str(), 0) == 0)
                        {
                            cout << "*** INFO(FtpUtil::GetHourlyObsMgex): successfully download MGEX hourly observation file " << oFile << endl;

                            /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                            cmd = "del " + dFile;
#else           /* for Linux or Mac */
                            cmd = "rm -rf " + dFile;
#endif
                            std::system(cmd.c_str());
                        }
                    }
                    else cout << "*** INFO(FtpUtil::GetHourlyObsMgex): MGEX hourly observation file " <<
                        oFile << " or " << dFile << " has existed!" << endl;
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetHourlyObsMgex */

/**
* @brief   : GetHrObsMgex - download MGEX RINEX high-rate observation (1s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetHrObsMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('highrate') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "highrate");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obmOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the MGEX observation files */
        for (int i = 0; i < fopt->hhObm.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhObm[i]);
            char tmpDir[MAXSTRPATH] = { '\0' };
            char sep = (char)FILEPATHSEP;
            sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
            string sHhDir = tmpDir;
            if (access(sHhDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + sHhDir;
#endif
                std::system(cmd.c_str());
            }

            /* change directory */
#ifdef _WIN32   /* for Windows */
            _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
            chdir(sHhDir.c_str());
#endif

            string url, cutDirs = " --cut-dirs=8 ";
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBMHR] + "/" +
                sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;
            else if (ftpName == "IGN")
            {
                url = _ftpArchive.IGN[IDX_OBMHR] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=6 ";
            }
            else url = _ftpArchive.CDDIS[IDX_OBMHR] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;

            /* it is OK for '*.Z' or '*.gz' format */
            string crxFile = "*_15M_01S_MO.crx";
            string crxxFile = crxFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
            std::system(cmd.c_str());

            /* 'a' = 97, 'b' = 98, ... */
            int ii = fopt->hhObm[i] + 97;
            char ch = ii;
            string sch;
            sch.push_back(ch);

            std::vector<string> minuStr = { "00", "15", "30", "45" };
            for (int i = 0; i < minuStr.size(); i++)
            {
                /* get the file list */
                string suffix = minuStr[i] + "_15M_01S_MO.crx";
                vector<string> crxFiles;
                str.GetFilesAll(sHhDir, suffix, crxFiles);
                char sitName[MAXCHARS];
                for (int j = 0; j < crxFiles.size(); j++)
                {
                    if (access(crxFiles[j].c_str(), 0) == 0)
                    {
                        /* extract it */
                        cmd = gzipFull + " -d -f " + crxFiles[j];
                        std::system(cmd.c_str());

                        str.StrMid(sitName, crxFiles[j].c_str(), 0, 4);
                        string site = sitName;
                        str.ToLower(site);
                        str.StrMid(sitName, crxFiles[j].c_str(), 0, (int)crxFiles[j].find_last_of('.'));
                        crxFile = sitName;
                        if (access(crxFile.c_str(), 0) == -1) continue;
                        string oFile = site + sDoy + sch + minuStr[i] +  "." + sYy + "o";
                        /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                        cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                        cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                        std::system(cmd.c_str());

                        /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                        cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                        cmd = "rm -rf " + crxFile;
#endif
                        std::system(cmd.c_str());
                    }
                }
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the MGEX observation file site-by-site */
        if (access(fopt->obmOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obmOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHrObsMgex): open site.list = " << fopt->obmOpt << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhObm.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhObm[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhObm[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);

                    std::vector<string> minuStr = { "00", "15", "30", "45" };
                    for (int i = 0; i < minuStr.size(); i++)
                    {
                        str.ToLower(sitName);
                        string oFile = sitName + sDoy + sch + minuStr[i] + "." + sYy + "o";
                        string dFile = sitName + sDoy + sch + minuStr[i] + "." + sYy + "d";
                        if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                        {
                            string url, cutDirs = " --cut-dirs=8 ";
                            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBMHR] + "/" +
                                sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;
                            else if (ftpName == "IGN")
                            {
                                url = _ftpArchive.IGN[IDX_OBMHR] + "/" + sYyyy + "/" + sDoy;
                                cutDirs = " --cut-dirs=6 ";
                            }
                            else url = _ftpArchive.CDDIS[IDX_OBMHR] + "/" + sYyyy + "/" + sDoy + "/" + 
                                sYy + "d" + "/" + sHh;

                            /* it is OK for '*.Z' or '*.gz' format */
                            str.ToUpper(sitName);
                            string crxFile = sitName + "*" + sYyyy + sDoy + sHh + minuStr[i] + "_15M_01S_MO.crx";
                            string crxxFile = crxFile + ".*";
                            string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
                            std::system(cmd.c_str());

                            /* extract '*.gz' */
                            string crxgzFile = crxFile + ".gz";
                            cmd = gzipFull + " -d -f " + crxgzFile;
                            std::system(cmd.c_str());
                            string changeFileName;
#ifdef _WIN32  /* for Windows */
                            changeFileName = "rename";
#else          /* for Linux or Mac */
                            changeFileName = "mv";
#endif
                            cmd = changeFileName + " " + crxFile + " " + dFile;
                            std::system(cmd.c_str());
                            if (access(dFile.c_str(), 0) == -1)
                            {
                                /* extract '*.Z' */
                                string crxzFile = crxFile + ".Z";
                                cmd = gzipFull + " -d -f " + crxzFile;
                                std::system(cmd.c_str());

                                cmd = changeFileName + " " + crxFile + " " + dFile;
                                std::system(cmd.c_str());
                                if (access(dFile.c_str(), 0) == -1)
                                {
                                    cout << "*** WARNING(FtpUtil::GetDailyObsMgex): failed to download MGEX daily observation file " << oFile << endl;

                                    continue;
                                }
                            }

#ifdef _WIN32  /* for Windows */
                            cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                            cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                            std::system(cmd.c_str());

                            if (access(oFile.c_str(), 0) == 0)
                            {
                                cout << "*** INFO(FtpUtil::GetHrObsMgex): successfully download MGEX high-rate observation file " << oFile << endl;

                                /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                                cmd = "del " + dFile;
#else           /* for Linux or Mac */
                                cmd = "rm -rf " + dFile;
#endif
                                std::system(cmd.c_str());
                            }
                        }
                        else cout << "*** INFO(FtpUtil::GetHrObsMgex): MGEX high-rate observation file " << 
                            oFile << " or " << dFile << " has existed!" << endl;
                    }
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetHrObsMgex */

/**
* @brief   : GetDailyObsCut - download Curtin University of Technology (CUT) RINEX daily observation (30s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDailyObsCut(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('daily') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(subDir.c_str());
#else           /* for Linux or Mac */
    chdir(subDir.c_str());
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    string url = "http://saegnss2.curtin.edu/ldc/rinex3/daily/" + sYyyy + "/" + sDoy;
    string cutDirs = " --cut-dirs=5 ";
    /* download the Curtin University of Technology (CUT) observation file site-by-site */
    if (access(fopt->obcOpt, 0) == 0)
    {
        ifstream sitLst(fopt->obcOpt);
        if (!sitLst.is_open())
        {
            cerr << "*** ERROR(FtpUtil::GetDailyObsCut): open site.list = " << fopt->obcOpt << " file failed, please check it" << endl;

            return;
        }

        string sitName;
        while (getline(sitLst, sitName))
        {
            str.ToLower(sitName);
            string oFile = sitName + sDoy + "0." + sYy + "o";
            if (access(oFile.c_str(), 0) == -1)
            {
                /* it is OK for '*.gz' format */
                str.ToUpper(sitName);
                string crxFile = sitName + "00AUS_R_" + sYyyy + sDoy + "0000_01D_30S_MO.crx";
                string crxgzFile = crxFile + ".gz";
                string cmd = wgetFull + " " + qr + " -nH " + cutDirs + url + "/" + crxgzFile;
                std::system(cmd.c_str());

                /* extract '*.gz' */
                cmd = gzipFull + " -d -f " + crxgzFile;
                std::system(cmd.c_str());
                if (access(crxFile.c_str(), 0) == -1)
                {
                    cout << "*** WARNING(FtpUtil::GetDailyObsCut): failed to download CUT daily observation file " << oFile << endl;

                    continue;
                }

#ifdef _WIN32  /* for Windows */
                cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                std::system(cmd.c_str());

                if (access(oFile.c_str(), 0) == 0)
                {
                    cout << "*** INFO(FtpUtil::GetDailyObsCut): successfully download CUT daily observation file " << oFile << endl;

                    /* delete 'crx' file */
#ifdef _WIN32   /* for Windows */
                    cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                    cmd = "rm -rf " + crxFile;
#endif
                    std::system(cmd.c_str());
                }
            }
            else cout << "*** INFO(FtpUtil::GetDailyObsCut): CUT daily observation file " << oFile <<
                " has existed!" << endl;
        }

        /* close 'site.list' */
        sitLst.close();
    }
} /* end of GetDailyObsCut */

/**
* @brief   : GetDailyObsGa - download Geoscience Australia (GA) RINEX daily observation (30s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDailyObsGa(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('daily') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(subDir.c_str());
#else           /* for Linux or Mac */
    chdir(subDir.c_str());
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    string url = "ftp://ftp.data.gnss.ga.gov.au/daily/" + sYyyy + "/" + sDoy;
    string cutDirs = " --cut-dirs=3 ";
    if (strlen(fopt->obgOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* it is OK for '*.gz' format */
        string crxFile = "*" + sYyyy + sDoy + "0000_01D_30S_MO.crx";
        string crxgzFile = crxFile + ".gz";
        string cmd = wgetFull + " " + qr + " -nH -A " + crxgzFile + cutDirs + url;
        std::system(cmd.c_str());

        /* get the file list */
        string suffix = ".crx";
        vector<string> crxFiles;
        str.GetFilesAll(subDir, suffix, crxFiles);
        char sitName[MAXCHARS];
        for (int i = 0; i < crxFiles.size(); i++)
        {
            if (access(crxFiles[i].c_str(), 0) == 0)
            {
                /* extract it */
                cmd = gzipFull + " -d -f " + crxFiles[i];
                std::system(cmd.c_str());

                str.StrMid(sitName, crxFiles[i].c_str(), 0, 4);
                string site = sitName;
                str.ToLower(site);
                str.StrMid(sitName, crxFiles[i].c_str(), 0, (int)crxFiles[i].find_last_of('.'));
                crxFile = sitName;
                if (access(crxFile.c_str(), 0) == -1) continue;
                string oFile = site + sDoy + "0." + sYy + "o";
                /* convert from 'crx' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                std::system(cmd.c_str());

                /* delete crxFile */
#ifdef _WIN32   /* for Windows */
                cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                cmd = "rm -rf " + crxFile;
#endif
                std::system(cmd.c_str());
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the GA observation file site-by-site */
        if (access(fopt->obgOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obgOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetDailyObsGa): open site.list = " << fopt->obgOpt << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                str.ToLower(sitName);
                string oFile = sitName + sDoy + "0." + sYy + "o";
                string dFile = sitName + sDoy + "0." + sYy + "d";
                if (access(oFile.c_str(), 0) == -1)
                {
                    /* it is OK for '*.gz' format */
                    str.ToUpper(sitName);
                    string crxFile = sitName + "*" + sYyyy + sDoy + "0000_01D_30S_MO.crx";
                    string crxgzFile = crxFile + ".gz";
                    string cmd = wgetFull + " " + qr + " -nH -A " + crxgzFile + cutDirs + url;
                    std::system(cmd.c_str());

                    /* extract '*.gz' */
                    cmd = gzipFull + " -d -f " + crxgzFile;
                    std::system(cmd.c_str());
                    string changeFileName;
#ifdef _WIN32  /* for Windows */
                    changeFileName = "rename";
#else          /* for Linux or Mac */
                    changeFileName = "mv";
#endif
                    cmd = changeFileName + " " + crxFile + " " + dFile;
                    std::system(cmd.c_str());
                    if (access(dFile.c_str(), 0) == -1)
                    {
                        cout << "*** WARNING(FtpUtil::GetDailyObsGa): failed to download GA daily observation file " << oFile << endl;

                        continue;
                    }

#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    if (access(oFile.c_str(), 0) == 0)
                    {
                        cout << "*** INFO(FtpUtil::GetDailyObsGa): successfully download GA daily observation file " << oFile << endl;

                        /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                        cmd = "del " + dFile;
#else           /* for Linux or Mac */
                        cmd = "rm -rf " + dFile;
#endif
                        std::system(cmd.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::GetDailyObsGa): GA daily observation file " << oFile <<
                    " has existed!" << endl;
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetDailyObsGa */

/**
* @brief   : GetHourlyObsGa - download Geoscience Australia (GA) RINEX hourly observation (30s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetHourlyObsGa(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('hourly') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "hourly");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    string url0 = "ftp://ftp.data.gnss.ga.gov.au/hourly/" + sYyyy + "/" + sDoy;
    string cutDirs = " --cut-dirs=4 ";
    if (strlen(fopt->obgOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the GA observation files */
        for (int i = 0; i < fopt->hhObg.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhObg[i]);
            char tmpDir[MAXSTRPATH] = { '\0' };
            char sep = (char)FILEPATHSEP;
            sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
            string sHhDir = tmpDir;
            if (access(sHhDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + sHhDir;
#endif
                std::system(cmd.c_str());
            }

            /* change directory */
#ifdef _WIN32   /* for Windows */
            _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
            chdir(sHhDir.c_str());
#endif

            string url = url0 + "/" + sHh;
            /* it is OK for '*.gz' format */
            string crxFile = "*" + sYyyy + sDoy + sHh + "00_01H_30S_MO.crx";
            string crxgzFile = crxFile + ".gz";
            string cmd = wgetFull + " " + qr + " -nH -A " + crxgzFile + cutDirs + url;
            std::system(cmd.c_str());

            /* 'a' = 97, 'b' = 98, ... */
            int ii = fopt->hhObg[i] + 97;
            char ch = ii;
            string sch;
            sch.push_back(ch);

            /* get the file list */
            string suffix = ".crx";
            vector<string> crxFiles;
            str.GetFilesAll(sHhDir, suffix, crxFiles);
            char sitName[MAXCHARS];
            for (int i = 0; i < crxFiles.size(); i++)
            {
                if (access(crxFiles[i].c_str(), 0) == 0)
                {
                    /* extract it */
                    cmd = gzipFull + "  -d -f " + crxFiles[i];
                    std::system(cmd.c_str());

                    str.StrMid(sitName, crxFiles[i].c_str(), 0, 4);
                    string site = sitName;
                    str.ToLower(site);
                    str.StrMid(sitName, crxFiles[i].c_str(), 0, (int)crxFiles[i].find_last_of('.'));
                    crxFile = sitName;
                    if (access(crxFile.c_str(), 0) == -1) continue;
                    string oFile = site + sDoy + sch + "." + sYy + "o";
                    /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                    cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                    cmd = "rm -rf " + crxFile;
#endif
                    std::system(cmd.c_str());
                }
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the GA observation file site-by-site */
        if (access(fopt->obgOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obgOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHourlyObsGa): open site.list = " << fopt->obgOpt << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhObg.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhObg[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhObg[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);
                    str.ToLower(sitName);
                    string oFile = sitName + sDoy + sch + "." + sYy + "o";
                    string dFile = sitName + sDoy + sch + "." + sYy + "d";
                    if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                    {
                        string url = url0 + "/" + sHh;
                        /* it is OK for '*.gz' format */
                        str.ToUpper(sitName);
                        string crxFile = sitName + "*" + sYyyy + sDoy + sHh + "00_01H_30S_MO.crx";
                        string crxgzFile = crxFile + ".gz";
                        string cmd = wgetFull + " " + qr + " -nH -A " + crxgzFile + cutDirs + url;
                        std::system(cmd.c_str());

                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + crxgzFile;
                        std::system(cmd.c_str());
                        string changeFileName;
#ifdef _WIN32  /* for Windows */
                        changeFileName = "rename";
#else          /* for Linux or Mac */
                        changeFileName = "mv";
#endif
                        cmd = changeFileName + " " + crxFile + " " + dFile;
                        std::system(cmd.c_str());
                        if (access(dFile.c_str(), 0) == -1)
                        {
                            cout << "*** WARNING(FtpUtil::GetDailyObsGa): failed to download GA daily observation file " << oFile << endl;

                            continue;
                        }

#ifdef _WIN32  /* for Windows */
                        cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                        cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                        std::system(cmd.c_str());

                        if (access(oFile.c_str(), 0) == 0)
                        {
                            cout << "*** INFO(FtpUtil::GetHourlyObsGa): successfully download GA hourly observation file " << oFile << endl;

                            /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                            cmd = "del " + dFile;
#else           /* for Linux or Mac */
                            cmd = "rm -rf " + dFile;
#endif
                            std::system(cmd.c_str());
                        }
                    }
                    else cout << "*** INFO(FtpUtil::GetHourlyObsGa): GA hourly observation file " <<
                        oFile << " or " << dFile << " has existed!" << endl;
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetHourlyObsMgex */

/**
* @brief   : GetHrObsGa - download Geoscience Australia (GA) RINEX high-rate observation (1s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetHrObsGa(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('highrate') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "highrate");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    string url0 = "ftp://ftp.data.gnss.ga.gov.au/highrate/" + sYyyy + "/" + sDoy;
    string cutDirs = " --cut-dirs=4 ";
    if (strlen(fopt->obgOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the MGEX observation files */
        for (int i = 0; i < fopt->hhObg.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhObg[i]);
            char tmpDir[MAXSTRPATH] = { '\0' };
            char sep = (char)FILEPATHSEP;
            sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
            string sHhDir = tmpDir;
            if (access(sHhDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + sHhDir;
#endif
                std::system(cmd.c_str());
            }

            /* change directory */
#ifdef _WIN32   /* for Windows */
            _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
            chdir(sHhDir.c_str());
#endif

            string url = url0 + "/" + sHh;
            /* it is OK for '*.gz' format */
            string crxFile = "*_15M_01S_MO.crx";
            string crxgzFile = crxFile + ".gz";
            string cmd = wgetFull + " " + qr + " -nH -A " + crxgzFile + cutDirs + url;
            std::system(cmd.c_str());

            /* 'a' = 97, 'b' = 98, ... */
            int ii = fopt->hhObg[i] + 97;
            char ch = ii;
            string sch;
            sch.push_back(ch);

            std::vector<string> minuStr = { "00", "15", "30", "45" };
            for (int i = 0; i < minuStr.size(); i++)
            {
                /* get the file list */
                string suffix = minuStr[i] + "_15M_01S_MO.crx";
                vector<string> crxFiles;
                str.GetFilesAll(sHhDir, suffix, crxFiles);
                char sitName[MAXCHARS];
                for (int j = 0; j < crxFiles.size(); j++)
                {
                    if (access(crxFiles[j].c_str(), 0) == 0)
                    {
                        /* extract it */
                        cmd = gzipFull + " -d -f " + crxFiles[j];
                        std::system(cmd.c_str());

                        str.StrMid(sitName, crxFiles[j].c_str(), 0, 4);
                        string site = sitName;
                        str.ToLower(site);
                        str.StrMid(sitName, crxFiles[j].c_str(), 0, (int)crxFiles[j].find_last_of('.'));
                        crxFile = sitName;
                        if (access(crxFile.c_str(), 0) == -1) continue;
                        string oFile = site + sDoy + sch + minuStr[i] + "." + sYy + "o";
                        /* convert from 'crx' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                        cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                        cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                        std::system(cmd.c_str());

                        /* delete 'crx' file */
#ifdef _WIN32   /* for Windows */
                        cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                        cmd = "rm -rf " + crxFile;
#endif
                        std::system(cmd.c_str());
                    }
                }
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the GA observation file site-by-site */
        if (access(fopt->obgOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obgOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHrObsGa): open site.list = " << fopt->obgOpt << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhObg.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhObg[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhObg[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);

                    string url = url0 + "/" + sHh;
                    std::vector<string> minuStr = { "00", "15", "30", "45" };
                    for (int i = 0; i < minuStr.size(); i++)
                    {
                        str.ToLower(sitName);
                        string oFile = sitName + sDoy + sch + minuStr[i] + "." + sYy + "o";
                        string dFile = sitName + sDoy + sch + minuStr[i] + "." + sYy + "d";
                        if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                        {
                            /* it is OK for '*.gz' format */
                            str.ToUpper(sitName);
                            string crxFile = sitName + "*" + sYyyy + sDoy + sHh + minuStr[i] + "_15M_01S_MO.crx";
                            string crxgzFile = crxFile + ".gz";
                            string cmd = wgetFull + " " + qr + " -nH -A " + crxgzFile + cutDirs + url;
                            std::system(cmd.c_str());

                            /* extract '*.gz' */
                            cmd = gzipFull + " -d -f " + crxgzFile;
                            std::system(cmd.c_str());
                            string changeFileName;
#ifdef _WIN32  /* for Windows */
                            changeFileName = "rename";
#else          /* for Linux or Mac */
                            changeFileName = "mv";
#endif
                            cmd = changeFileName + " " + crxFile + " " + dFile;
                            std::system(cmd.c_str());
                            if (access(dFile.c_str(), 0) == -1)
                            {
                                cout << "*** WARNING(FtpUtil::GetDailyObsGa): failed to download GA daily observation file " << oFile << endl;

                                continue;
                            }

#ifdef _WIN32  /* for Windows */
                            cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                            cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                            std::system(cmd.c_str());

                            if (access(oFile.c_str(), 0) == 0)
                            {
                                cout << "*** INFO(FtpUtil::GetHrObsGa): successfully download GA high-rate observation file " << oFile << endl;

                                /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                                cmd = "del " + dFile;
#else           /* for Linux or Mac */
                                cmd = "rm -rf " + dFile;
#endif
                                std::system(cmd.c_str());
                            }
                        }
                        else cout << "*** INFO(FtpUtil::GetHrObsGa): GA high-rate observation file " <<
                            oFile << " or " << dFile << " has existed!" << endl;
                    }
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetHrObsGa */

/**
* @brief   : Get30sObsHk - download Hong Kong CORS RINEX daily observation (30s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::Get30sObsHk(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('daily') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "30s");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(subDir.c_str());
#else           /* for Linux or Mac */
    chdir(subDir.c_str());
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    string url0 = "ftp://ftp.geodetic.gov.hk/rinex3/" + sYyyy + "/" + sDoy;
    string cutDirs = " --cut-dirs=5 ";
    /* download the Hong Kong CORS observation file site-by-site */
    if (access(fopt->obhOpt, 0) == 0)
    {
        ifstream sitLst(fopt->obhOpt);
        if (!sitLst.is_open())
        {
            cerr << "*** ERROR(FtpUtil::Get30sObsHk): open site.list = " << fopt->obhOpt << " file failed, please check it" << endl;

            return;
        }

        string sitName;
        while (getline(sitLst, sitName))
        {
            str.ToLower(sitName);
            string oFile = sitName + sDoy + "0." + sYy + "o";
            string url = url0 + "/" + sitName + "/30s";
            if (access(oFile.c_str(), 0) == -1)
            {
                /* it is OK for '*.gz' format */
                str.ToUpper(sitName);
                string crxFile = sitName + "00HKG_R_" + sYyyy + sDoy + "0000_01D_30S_MO.crx";
                string crxgzFile = crxFile + ".gz";
                str.ToLower(sitName);
                string cmd = wgetFull + " " + qr + " -nH " + cutDirs + url + "/" + crxgzFile;
                std::system(cmd.c_str());

                /* extract '*.gz' */
                cmd = gzipFull + " -d -f " + crxgzFile;
                std::system(cmd.c_str());
                if (access(crxFile.c_str(), 0) == -1)
                {
                    cout << "*** WARNING(FtpUtil::Get30sObsHk): failed to download HK CORS 30s observation file " << oFile << endl;

                    continue;
                }

#ifdef _WIN32  /* for Windows */
                cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                std::system(cmd.c_str());

                if (access(oFile.c_str(), 0) == 0)
                {
                    cout << "*** INFO(FtpUtil::Get30sObsHk): successfully download HK CORS 30s observation file " << oFile << endl;

                    /* delete 'crx' file */
#ifdef _WIN32   /* for Windows */
                    cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                    cmd = "rm -rf " + crxFile;
#endif
                    std::system(cmd.c_str());
                }
            }
            else cout << "*** INFO(FtpUtil::Get30sObsHk): HK CORS 30s observation file " << oFile <<
                " has existed!" << endl;
        }

        /* close 'site.list' */
        sitLst.close();
    }
} /* end of Get30sObsHk */

/**
* @brief   : Get5sObsHk - download Hong Kong CORS RINEX daily observation (5s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::Get5sObsHk(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('daily') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "5s");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(subDir.c_str());
#else           /* for Linux or Mac */
    chdir(subDir.c_str());
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    string url0 = "ftp://ftp.geodetic.gov.hk/rinex3/" + sYyyy + "/" + sDoy;
    string cutDirs = " --cut-dirs=5 ";
    /* download the Hong Kong CORS observation file site-by-site */
    if (access(fopt->obhOpt, 0) == 0)
    {
        ifstream sitLst(fopt->obhOpt);
        if (!sitLst.is_open())
        {
            cerr << "*** ERROR(FtpUtil::Get5sObsHk): open site.list = " << fopt->obhOpt << " file failed, please check it" << endl;

            return;
        }

        string sitName;
        while (getline(sitLst, sitName))
        {
            for (int i = 0; i < fopt->hhObh.size(); i++)
            {
                string sHh = str.hh2str(fopt->hhObh[i]);
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                string sHhDir = tmpDir;
                if (access(sHhDir.c_str(), 0) == -1)
                {
                    /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                    string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                    string cmd = "mkdir -p " + sHhDir;
#endif
                    std::system(cmd.c_str());
                }

                /* change directory */
#ifdef _WIN32   /* for Windows */
                _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                chdir(sHhDir.c_str());
#endif

                /* 'a' = 97, 'b' = 98, ... */
                int ii = fopt->hhObh[i] + 97;
                char ch = ii;
                string sch;
                sch.push_back(ch);
                str.ToLower(sitName);
                string oFile = sitName + sDoy + sch + "." + sYy + "o";
                string url = url0 + "/" + sitName + "/5s";
                if (access(oFile.c_str(), 0) == -1)
                {
                    /* it is OK for '*.gz' format */
                    str.ToUpper(sitName);
                    string crxFile = sitName + "00HKG_R_" + sYyyy + sDoy + sHh + "00_01H_05S_MO.crx";
                    string crxgzFile = crxFile + ".gz";
                    str.ToLower(sitName);
                    string cmd = wgetFull + " " + qr + " -nH " + cutDirs + url + "/" + crxgzFile;
                    std::system(cmd.c_str());

                    /* extract '*.gz' */
                    cmd = gzipFull + " -d -f " + crxgzFile;
                    std::system(cmd.c_str());
                    if (access(crxFile.c_str(), 0) == -1)
                    {
                        cout << "*** WARNING(FtpUtil::Get5sObsHk): failed to download HK CORS 5s observation file " << oFile << endl;

                        continue;
                    }

#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    if (access(oFile.c_str(), 0) == 0)
                    {
                        cout << "*** INFO(FtpUtil::Get5sObsHk): successfully download HK CORS 5s observation file " << oFile << endl;

                        /* delete 'crx' file */
#ifdef _WIN32   /* for Windows */
                        cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                        cmd = "rm -rf " + crxFile;
#endif
                        std::system(cmd.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::Get5sObsHk): HK CORS 5s observation file " << oFile <<
                    " has existed!" << endl;
            }
        }

        /* close 'site.list' */
        sitLst.close();
    }
} /* end of Get5sObsHk */

/**
* @brief   : Get1sObsHk - download Hong Kong CORS RINEX daily observation (1s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::Get1sObsHk(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('daily') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "1s");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(subDir.c_str());
#else           /* for Linux or Mac */
    chdir(subDir.c_str());
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    string url0 = "ftp://ftp.geodetic.gov.hk/rinex3/" + sYyyy + "/" + sDoy;
    string cutDirs = " --cut-dirs=5 ";
    /* download the Hong Kong CORS observation file site-by-site */
    if (access(fopt->obhOpt, 0) == 0)
    {
        ifstream sitLst(fopt->obhOpt);
        if (!sitLst.is_open())
        {
            cerr << "*** ERROR(FtpUtil::Get1sObsHk): open site.list = " << fopt->obhOpt << " file failed, please check it" << endl;

            return;
        }

        string sitName;
        while (getline(sitLst, sitName))
        {
            for (int i = 0; i < fopt->hhObh.size(); i++)
            {
                string sHh = str.hh2str(fopt->hhObh[i]);
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                string sHhDir = tmpDir;
                if (access(sHhDir.c_str(), 0) == -1)
                {
                    /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                    string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                    string cmd = "mkdir -p " + sHhDir;
#endif
                    std::system(cmd.c_str());
                }

                /* change directory */
#ifdef _WIN32   /* for Windows */
                _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                chdir(sHhDir.c_str());
#endif

                /* 'a' = 97, 'b' = 98, ... */
                int ii = fopt->hhObh[i] + 97;
                char ch = ii;
                string sch;
                sch.push_back(ch);
                str.ToLower(sitName);
                string oFile = sitName + sDoy + sch + "." + sYy + "o";
                string url = url0 + "/" + sitName + "/1s";
                if (access(oFile.c_str(), 0) == -1)
                {
                    /* it is OK for '*.gz' format */
                    str.ToUpper(sitName);
                    string crxFile = sitName + "00HKG_R_" + sYyyy + sDoy + sHh + "00_01H_01S_MO.crx";
                    string crxgzFile = crxFile + ".gz";
                    str.ToLower(sitName);
                    string cmd = wgetFull + " " + qr + " -nH " + cutDirs + url + "/" + crxgzFile;
                    std::system(cmd.c_str());

                    /* extract '*.gz' */
                    cmd = gzipFull + " -d -f " + crxgzFile;
                    std::system(cmd.c_str());
                    if (access(crxFile.c_str(), 0) == -1)
                    {
                        cout << "*** WARNING(FtpUtil::Get1sObsHk): failed to download HK CORS 1s observation file " << oFile << endl;

                        continue;
                    }

#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    if (access(oFile.c_str(), 0) == 0)
                    {
                        cout << "*** INFO(FtpUtil::Get1sObsHk): successfully download HK CORS 1s observation file " << oFile << endl;

                        /* delete 'crx' file */
#ifdef _WIN32   /* for Windows */
                        cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                        cmd = "rm -rf " + crxFile;
#endif
                        std::system(cmd.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::Get1sObsHk): HK CORS 1s observation file " << oFile <<
                    " has existed!" << endl;
            }
        }

        /* close 'site.list' */
        sitLst.close();
    }
} /* end of Get1sObsHk */

/**
* @brief   : GetDailyObsNgs - download NGS/NOAA CORS RINEX daily observation (30s) files (short name "d") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDailyObsNgs(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('daily') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(subDir.c_str());
#else           /* for Linux or Mac */
    chdir(subDir.c_str());
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    string url = "https://noaa-cors-pds.s3.amazonaws.com/rinex/" + sYyyy + "/" + sDoy;
    string cutDirs = " --cut-dirs=4 ";
    /* download the NGS/NOAA CORS observation file site-by-site */
    if (access(fopt->obnOpt, 0) == 0)
    {
        ifstream sitLst(fopt->obnOpt);
        if (!sitLst.is_open())
        {
            cerr << "*** ERROR(FtpUtil::GetDailyObsNgs): open site.list = " << fopt->obnOpt << " file failed, please check it" << endl;

            return;
        }

        string sitName;
        while (getline(sitLst, sitName))
        {
            str.ToLower(sitName);
            string oFile = sitName + sDoy + "0." + sYy + "o";
            if (access(oFile.c_str(), 0) == -1)
            {
                /* it is OK for '*.gz' format */
                string dFile = sitName + sDoy + "0." + sYy + "d";
                string dgzFile = dFile + ".gz";
                string cmd = wgetFull + " " + qr + " -nH " + cutDirs + url + "/" + sitName + "/" + dgzFile;
                std::system(cmd.c_str());

                /* extract '*.gz' */
                cmd = gzipFull + " -d -f " + dgzFile;
                std::system(cmd.c_str());
                if (access(dFile.c_str(), 0) == -1)
                {
                    cout << "*** WARNING(FtpUtil::GetDailyObsNgs): failed to download NGS/NOAA CORS daily observation file " << oFile << endl;

                    continue;
                }

#ifdef _WIN32  /* for Windows */
                cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                std::system(cmd.c_str());

                if (access(oFile.c_str(), 0) == 0)
                {
                    cout << "*** INFO(FtpUtil::GetDailyObsNgs): successfully download NGS/NOAA CORS daily observation file " << oFile << endl;

                    /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                    cmd = "del " + dFile;
#else           /* for Linux or Mac */
                    cmd = "rm -rf " + dFile;
#endif
                    std::system(cmd.c_str());
                }
            }
            else cout << "*** INFO(FtpUtil::GetDailyObsNgs): NGS/NOAA CORS daily observation file " << oFile <<
                " has existed!" << endl;
        }

        /* close 'site.list' */
        sitLst.close();
    }
} /* end of GetDailyObsNgs */

/**
* @brief   : GetDailyObsEpn - download EUREF Permanent Network (EPN) RINEX daily observation (30s) files (long name 'crx' and short name "d") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDailyObsEpn(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('daily') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(subDir.c_str());
#else           /* for Linux or Mac */
    chdir(subDir.c_str());
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    string url = "ftp://ftp.epncb.oma.be/pub/obs/" + sYyyy + "/" + sDoy;
    string cutDirs = " --cut-dirs=4 ";
    if (strlen(fopt->obeOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* it is OK for '*.gz' format */
        string crxFile = "*" + sYyyy + sDoy + "0000_01D_30S_MO.crx";
        string crxgzFile = crxFile + ".gz";
        string cmd = wgetFull + " " + qr + " -nH -A " + crxgzFile + cutDirs + url;
        std::system(cmd.c_str());

        /* get the file list */
        string suffix = ".crx";
        vector<string> crxFiles;
        str.GetFilesAll(subDir, suffix, crxFiles);
        char sitName[MAXCHARS];
        for (int i = 0; i < crxFiles.size(); i++)
        {
            if (access(crxFiles[i].c_str(), 0) == 0)
            {
                /* extract it */
                cmd = gzipFull + " -d -f " + crxFiles[i];
                std::system(cmd.c_str());

                str.StrMid(sitName, crxFiles[i].c_str(), 0, 4);
                string site = sitName;
                str.ToLower(site);
                str.StrMid(sitName, crxFiles[i].c_str(), 0, (int)crxFiles[i].find_last_of('.'));
                crxFile = sitName;
                if (access(crxFile.c_str(), 0) == -1) continue;
                string oFile = site + sDoy + "0." + sYy + "o";
                /* convert from 'crx' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                std::system(cmd.c_str());

                /* delete crxFile */
#ifdef _WIN32   /* for Windows */
                cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                cmd = "rm -rf " + crxFile;
#endif
                std::system(cmd.c_str());
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the EPN observation file site-by-site */
        if (access(fopt->obeOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obeOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetDailyObsEpn): open site.list = " << fopt->obeOpt << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                str.ToLower(sitName);
                string oFile = sitName + sDoy + "0." + sYy + "o";
                string dFile = sitName + sDoy + "0." + sYy + "d";
                if (access(oFile.c_str(), 0) == -1)
                {
                    /* it is OK for '*.gz' format */
                    str.ToUpper(sitName);
                    string crxFile = sitName + "*" + sYyyy + sDoy + "0000_01D_30S_MO.crx";
                    string crxgzFile = crxFile + ".gz";
                    string cmd = wgetFull + " " + qr + " -nH -A " + crxgzFile + cutDirs + url;
                    std::system(cmd.c_str());

                    /* extract '*.gz' */
                    cmd = gzipFull + " -d -f " + crxgzFile;
                    std::system(cmd.c_str());
                    string changeFileName;
#ifdef _WIN32  /* for Windows */
                    changeFileName = "rename";
#else          /* for Linux or Mac */
                    changeFileName = "mv";
#endif
                    cmd = changeFileName + " " + crxFile + " " + dFile;
                    std::system(cmd.c_str());
                    if (access(dFile.c_str(), 0) == -1)
                    {
                        cout << "*** WARNING(FtpUtil::GetDailyObsEpn): failed to download EPN daily observation file " << oFile << endl;

                        continue;
                    }

#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    if (access(oFile.c_str(), 0) == 0)
                    {
                        cout << "*** INFO(FtpUtil::GetDailyObsEpn): successfully download EPN daily observation file " << oFile << endl;

                        /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                        cmd = "del " + dFile;
#else           /* for Linux or Mac */
                        cmd = "rm -rf " + dFile;
#endif
                        std::system(cmd.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::GetDailyObsEpn): EPN daily observation file " << oFile <<
                    " has existed!" << endl;
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetDailyObsEpn */

/**
* @brief   : GetNav - download daily GPS, GLONASS and mixed RINEX broadcast ephemeris files
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetNav(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string nTyp(fopt->navTyp);
    str.ToLower(nTyp);
    string nOpt(fopt->navOpt);
    str.ToLower(nOpt);
    if (nTyp == "daily")
    {
        /* creation of sub-directory ('daily') */
        char tmpDir[MAXSTRPATH] = { '\0' };
        char sep = (char)FILEPATHSEP;
        sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
        string subDir = tmpDir;
        if (access(subDir.c_str(), 0) == -1)
        {
            /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + subDir;
#endif
            std::system(cmd.c_str());
        }

        /* change directory */
#ifdef _WIN32   /* for Windows */
        _chdir(subDir.c_str());
#else           /* for Linux or Mac */
        chdir(subDir.c_str());
#endif

        string navFile, nav0File;
        if (nOpt == "gps")
        {
            /* GPS broadcast ephemeris file */
            navFile = "brdc" + sDoy + "0." + sYy + "n";
            nav0File = "brdc" + sDoy + "0." + sYy + "n";
        }
        else if (nOpt == "glo")
        {
            /* GLONASS broadcast ephemeris file */
            navFile = "brdc" + sDoy + "0." + sYy + "g";
            nav0File = "brdc" + sDoy + "0." + sYy + "g";
        }
        else if (nOpt == "mixed")
        {
            /* multi-GNSS broadcast ephemeris file */
            if (ftpName == "IGN") navFile = "BRDC00IGN_R_" + sYyyy + sDoy + "0000_01D_MN.rnx";
            else navFile = "BRDC00IGS_R_" + sYyyy + sDoy + "0000_01D_MN.rnx";
            nav0File = "brdm" + sDoy + "0." + sYy + "p";
        }

        if (access(navFile.c_str(), 0) == -1 && access(nav0File.c_str(), 0) == -1)
        {
            string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
            string url, cutDirs = " --cut-dirs=6 ";
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_NAV] + "/" +
                sYyyy + "/brdc";
            else if (ftpName == "IGN")
            {
                url = _ftpArchive.IGN[IDX_NAV] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=5 ";
            }
            else if (ftpName == "WHU")
            {
                cutDirs = " --cut-dirs=7 ";
                if (yyyy >= 2020) url = _ftpArchive.WHU[IDX_NAV] + "/" + sYyyy + "/brdc";
                else
                {
                    if (nOpt == "gps") url = _ftpArchive.WHU[IDX_NAV] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "n";
                    else if (nOpt == "glo") url = _ftpArchive.WHU[IDX_NAV] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "g";
                    if (nOpt == "mixed") url = _ftpArchive.WHU[IDX_NAV] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "p";
                }
            }
            else url = _ftpArchive.CDDIS[IDX_NAV] + "/" + sYyyy + "/brdc";

            /* it is OK for '*.Z' or '*.gz' format */
            string navxFile = navFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + navxFile + cutDirs + url;
            std::system(cmd.c_str());

            string navgzFile = navFile + ".gz", navzFile = navFile + ".Z";
            if (access(navgzFile.c_str(), 0) == 0)
            {
                /* extract '*.gz' */
                cmd = gzipFull + " -d -f " + navgzFile;
                std::system(cmd.c_str());
            }
            else if (access(navzFile.c_str(), 0) == 0)
            {
                /* extract '*.Z' */
                cmd = gzipFull + " -d -f " + navzFile;
                std::system(cmd.c_str());
            }
            if (access(navFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetNav): failed to download broadcast ephemeris file " <<
                nav0File << endl;

            if (nOpt == "mixed")
            {
                string changeFileName;
#ifdef _WIN32  /* for Windows */
                changeFileName = "rename";
#else          /* for Linux or Mac */
                changeFileName = "mv";
#endif
                cmd = changeFileName + " " + navFile + " " + nav0File;
                std::system(cmd.c_str());
            }

            if (access(nav0File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetNav): successfully download broadcast ephemeris file " << nav0File << endl;
        }
        else cout << "*** INFO(FtpUtil::GetNav): broadcast ephemeris file " << nav0File << " or " <<
            navFile << " has existed!" << endl;
    }
    else if (nTyp == "hourly")
    {
        /* creation of sub-directory ('hourly') */
        char tmpDir[MAXSTRPATH] = { '\0' };
        char sep = (char)FILEPATHSEP;
        sprintf(tmpDir, "%s%c%s", dir, sep, "hourly");
        string subDir = tmpDir;
        if (access(subDir.c_str(), 0) == -1)
        {
            /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + subDir;
#endif
            std::system(cmd.c_str());
        }

        string sitFile = fopt->obsOpt;
        if (fopt->getObs) sitFile = fopt->obsOpt;
        else if (fopt->getObm) sitFile = fopt->obmOpt;
        /* download the broadcast ephemeris file site-by-site */
        if (access(sitFile.c_str(), 0) == 0)
        {
            ifstream sitLst(sitFile.c_str());
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHourlyObsMgex): open site.list = " << sitFile << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhNav.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhNav[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhNav[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);
                    str.ToLower(sitName);
                    string navnsFile = sitName + sDoy + sch + "." + sYy + "n";  /* short file name */
                    string navgsFile = sitName + sDoy + sch + "." + sYy + "g";
                    
                    str.ToUpper(sitName);
                    string navnlFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_GN.rnx";  /* long file name */
                    string navglFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_RN.rnx";
                    string navclFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_CN.rnx";
                    string navelFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_EN.rnx";
                    string navjlFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_JN.rnx";
                    string navilFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_IN.rnx";
                    string navmlFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_MN.rnx";
                    std::vector<string> navFiles;
                    if (nOpt == "gps")
                    {
                        navFiles.push_back(navnsFile);
                        navFiles.push_back(navnlFile);
                    }
                    else if (nOpt == "glo")
                    {
                        navFiles.push_back(navgsFile);
                        navFiles.push_back(navglFile);
                    }
                    else if (nOpt == "bds") navFiles.push_back(navclFile);
                    else if (nOpt == "gal") navFiles.push_back(navelFile);
                    else if (nOpt == "qzs") navFiles.push_back(navjlFile);
                    else if (nOpt == "irn") navFiles.push_back(navilFile);
                    else if (nOpt == "mixed") navFiles.push_back(navmlFile);
                    else if (nOpt == "all")
                    {
                        navFiles.push_back(navnsFile);
                        navFiles.push_back(navnlFile);
                        navFiles.push_back(navgsFile);
                        navFiles.push_back(navglFile);
                        navFiles.push_back(navclFile);
                        navFiles.push_back(navelFile);
                        navFiles.push_back(navjlFile);
                        navFiles.push_back(navilFile);
                        navFiles.push_back(navmlFile);
                    }

                    str.ToLower(sitName);
                    string navns0File = sitName + sDoy + sch + "." + sYy + "n";
                    string navgs0File = sitName + sDoy + sch + "." + sYy + "g";
                    string navnl0File = sitName + sDoy + sch + "." + sYy + "gn";
                    string navgl0File = sitName + sDoy + sch + "." + sYy + "rn";
                    string navcl0File = sitName + sDoy + sch + "." + sYy + "cn";
                    string navel0File = sitName + sDoy + sch + "." + sYy + "en";
                    string navjl0File = sitName + sDoy + sch + "." + sYy + "jn";
                    string navil0File = sitName + sDoy + sch + "." + sYy + "in";
                    string navml0File = sitName + sDoy + sch + "." + sYy + "mn";
                    std::vector<string> nav0Files;
                    if (nOpt == "gps")
                    {
                        nav0Files.push_back(navns0File);
                        nav0Files.push_back(navnl0File);
                    }
                    else if (nOpt == "glo")
                    {
                        nav0Files.push_back(navgs0File);
                        nav0Files.push_back(navgl0File);
                    }
                    else if (nOpt == "bds") nav0Files.push_back(navcl0File);
                    else if (nOpt == "gal") nav0Files.push_back(navel0File);
                    else if (nOpt == "qzs") nav0Files.push_back(navjl0File);
                    else if (nOpt == "irn") nav0Files.push_back(navil0File);
                    else if (nOpt == "mixed") nav0Files.push_back(navml0File);
                    else if (nOpt == "all")
                    {
                        nav0Files.push_back(navns0File);
                        nav0Files.push_back(navnl0File);
                        nav0Files.push_back(navgs0File);
                        nav0Files.push_back(navgl0File);
                        nav0Files.push_back(navcl0File);
                        nav0Files.push_back(navel0File);
                        nav0Files.push_back(navjl0File);
                        nav0Files.push_back(navil0File);
                        nav0Files.push_back(navml0File);
                    }
                    
                    for (int i = 0; i < navFiles.size(); i++)
                    {
                        string url, cutDirs = " --cut-dirs=7 ";
                        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_OBMH] + "/" +
                            sYyyy + "/" + sDoy + "/" + sHh;
                        else if (ftpName == "IGN")
                        {
                            url = _ftpArchive.IGN[IDX_OBMH] + "/" + sYyyy + "/" + sDoy;
                            cutDirs = " --cut-dirs=6 ";
                        }
                        else url = _ftpArchive.CDDIS[IDX_OBMH] + "/" + sYyyy + "/" + sDoy + "/" + sHh;

                        /* it is OK for '*.Z' or '*.gz' format */
                        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
                        string navxFile = navFiles[i] + ".*";
                        string cmd = wgetFull + " " + qr + " -nH -A " + navxFile + cutDirs + url;
                        std::system(cmd.c_str());

                        /* extract '*.gz' */
                        string navgzFile = navFiles[i] + ".gz";
                        cmd = gzipFull + " -d -f " + navgzFile;
                        std::system(cmd.c_str());
                        string changeFileName;
#ifdef _WIN32  /* for Windows */
                        changeFileName = "rename";
#else          /* for Linux or Mac */
                        changeFileName = "mv";
#endif
                        cmd = changeFileName + " " + navFiles[i] + " " + nav0Files[i];
                        std::system(cmd.c_str());
                        if (access(nav0Files[i].c_str(), 0) == -1)
                        {
                            /* extract '*.Z' */
                            string navzFile = navFiles[i] + ".Z";
                            cmd = gzipFull + " -d -f " + navzFile;
                            std::system(cmd.c_str());

                            cmd = changeFileName + " " + navFiles[i] + " " + nav0Files[i];
                            std::system(cmd.c_str());
                            if (access(nav0Files[i].c_str(), 0) == -1)
                            {
                                cout << "*** WARNING(FtpUtil::GetNav): failed to download hourly broadcast ephemeris file " << 
                                    navFiles[i] << endl;
                            }
                            else
                            {
                                cout << "*** INFO(FtpUtil::GetNav): successfully download hourly broadcast ephemeris file " <<
                                    navFiles[i] << endl;

                                continue;
                            }
                        }
                        else
                        {
                            cout << "*** INFO(FtpUtil::GetNav): successfully download hourly broadcast ephemeris file " <<
                                navFiles[i] << endl;

                            continue;
                        }
                    }
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
    else if (nTyp == "RTNAV")
    {
        /* creation of sub-directory ('daily') */
        char tmpDir[MAXSTRPATH] = { '\0' };
        char sep = (char)FILEPATHSEP;
        sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
        string subDir = tmpDir;
        if (access(subDir.c_str(), 0) == -1)
        {
            /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + subDir;
#endif
            std::system(cmd.c_str());
        }

        /* change directory */
#ifdef _WIN32   /* for Windows */
        _chdir(subDir.c_str());
#else           /* for Linux or Mac */
        chdir(subDir.c_str());
#endif

        string navFile = "brdm" + sDoy + "z." + sYy + "p";
        if (access(navFile.c_str(), 0) == -1)
        {
            /* download brdmDDDz.YYp.Z file */
            string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
            string url = "ftp://ftp.lrz.de/transfer/steigenb/brdm";

            /* it is OK for '*.Z' or '*.gz' format */
            string navxFile = navFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + navxFile + " --cut-dirs=3 " + url;
            std::system(cmd.c_str());

            string navgzFile = navFile + ".gz", navzFile = navFile + ".Z";
            if (access(navgzFile.c_str(), 0) == 0)
            {
                /* extract '*.gz' */
                cmd = gzipFull + " -d -f " + navgzFile;
                std::system(cmd.c_str());
            }
            else if (access(navzFile.c_str(), 0) == 0)
            {
                /* extract '*.Z' */
                cmd = gzipFull + " -d -f " + navzFile;
                std::system(cmd.c_str());
            }

            if (access(navFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetNav): failed to download real-time broadcast ephemeris file " <<
                navFile << endl;
            else if (access(navFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetNav): successfully download real-time broadcast ephemeris file " <<
                navFile << endl;
        }
        else cout << "*** INFO(FtpUtil::GetNav): real-time broadcast ephemeris file " << navFile <<
            " has existed!" << endl;
    }
} /* end of GetNav */

/**
* @brief   : GetOrbClk - download IGS/MGEX precise orbit and clock files
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: mode (=1:sp3 downloaded; =2:clk downloadeded)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetOrbClk(gtime_t ts, const char dir[], int mode, const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute GPS week and day of week */
    TimeUtil tu;
    int wwww, dow;
    dow = tu.time2gpst(ts, &wwww, nullptr);
    StringUtil str;
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    string sYyyy = str.yyyy2str(yyyy);
    string sDoy = str.doy2str(doy);

    string ac(fopt->orbClkOpt);
    str.ToLower(ac);

    /* esu: ESA ultra-rapid orbit and clock products
       gfu: GFZ ultra-rapid orbit and clock products
       igu: IGS ultra-rapid orbit and clock products
       wuu: WHU ultra-rapid orbit and clock products
    */
    std::vector<string> ultraAc = { "esu", "gfu", "igu", "wuu" };
    bool isUltra = false;
    for (int i = 0; i < ultraAc.size(); i++)
    {
        if (ac == ultraAc[i]) isUltra = true;
    }

    /* cor: CODE rapid orbit and clock products
       emp: NRCan rapid orbit and clock products
       esr: ESA rapid orbit and clock products
       gfr: GFZ rapid orbit and clock products
       igr: IGS rapid orbit and clock products
    */
    std::vector<string> rapidAc = { "cor", "emp", "esr", "gfr", "igr" };
    bool isRapid = false;
    for (int i = 0; i < rapidAc.size(); i++)
    {
        if (ac == rapidAc[i]) isRapid = true;
    }
    
    /* cod: CODE final orbit and clock products
       emr: NRCan final orbit and clock products
       esa: ESA final orbit and clock products
       gfz: GFZ final orbit and clock products
       grg: CNES final orbit and clock products
       igs: IGS final orbit and clock products
       jpl: JPL final orbit and clock products
       mit: MIT final orbit and clock products
    */
    std::vector<string> finalAcIGS = { "cod", "emr", "esa", "gfz", "grg", "igs", "jpl", "mit" };
    bool isIGS = false;
    for (int i = 0; i < finalAcIGS.size(); i++)
    {
        if (ac == finalAcIGS[i]) isIGS = true;
    }

    /* com: CODE multi-GNSS final orbit and clock products
       gbm: GFZ multi-GNSS final orbit and clock products
       grm: CNES multi-GNSS final orbit and clock products
       wum: WHU multi-GNSS final orbit and clock products
    */
    std::vector<string> acMGEX = { "com", "gbm", "grm", "wum" };
    bool isMGEX = false;
    for (int i = 0; i < acMGEX.size(); i++)
    {
        if (ac == acMGEX[i]) isMGEX = true;
    }

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
    if (isUltra)  /* for ultra-rapid orbit and clock products  */
    {
        string url, cutDirs, acName;
        if (ac == "esu")       /* ESA */
        {
            url = "http://navigation-office.esa.int/products/gnss-products/" + sWwww;
            cutDirs = " --cut-dirs=3 ";
            acName = "ESA";
        }
        else if (ac == "gfu")  /* GFZ */
        {
            url = "ftp://ftp.gfz-potsdam.de/pub/GNSS/products/ultra/w" + sWwww;
            cutDirs = " --cut-dirs=5 ";
            acName = "GFZ";
        }
        else if (ac == "igu")  /* IGS */
        {
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_SP3] + "/" + sWwww;
            else if (ftpName == "IGN") url = _ftpArchive.IGN[IDX_SP3] + "/" + sWwww;
            else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_SP3] + "/" + sWwww;
            else url = _ftpArchive.CDDIS[IDX_SP3] + "/" + sWwww;
            cutDirs = " --cut-dirs=4 ";
            acName = "IGS";
        }
        else if (ac == "wuu")  /* WHU */
        {
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_SP3M] + "/" + sWwww;
            else if (ftpName == "IGN") url = _ftpArchive.IGN[IDX_SP3M] + "/" + sWwww;
            else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_SP3M] + "/" + sWwww;
            else url = _ftpArchive.CDDIS[IDX_SP3M] + "/" + sWwww;
            cutDirs = " --cut-dirs=5 ";
            acName = "WHU";
        }
            
        for (int i = 0; i < fopt->hhOrbClk.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhOrbClk[i]);
            string sp3File = ac + sWwww + sDow + "_" + sHh + ".sp3";
            if (access(sp3File.c_str(), 0) == -1)
            {
                string cmd;
                if (ac == "esu")  /* ESA */
                {
                    string sp3zFile = sp3File + ".Z";
                    string url0 = url + "/" + sp3zFile;
                    cmd = wgetFull + " " + qr + " -nH " + cutDirs + url0;
                    std::system(cmd.c_str());

                    if (access(sp3zFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + sp3zFile;
                        std::system(cmd.c_str());
                    }
                    else
                    {
                        string sp3gzFile = sp3File + ".gz";
                        string url0 = url + "/" + sp3gzFile;
                        cmd = wgetFull + " " + qr + " -nH " + cutDirs + url0;
                        std::system(cmd.c_str());

                        if (access(sp3gzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.gz' */
                            cmd = gzipFull + " -d -f " + sp3gzFile;
                            std::system(cmd.c_str());
                        }
                    }
                }
                else if (ac == "wuu")  /* WHU */
                {
                    string sp30File = "WUM0MGXULA_" + sYyyy + sDoy + sHh + "00_01D_*_ORB.SP3";
                    /* it is OK for '*.Z' or '*.gz' format */
                    string sp3xFile = sp30File + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + sp3xFile + cutDirs + url;
                    std::system(cmd.c_str());

                    /* extract '*.gz' */
                    string sp3gzFile = sp30File + ".gz";
                    cmd = gzipFull + " -d -f " + sp3gzFile;
                    std::system(cmd.c_str());
                    string changeFileName;
#ifdef _WIN32  /* for Windows */
                    changeFileName = "rename";
#else          /* for Linux or Mac */
                    changeFileName = "mv";
#endif
                    string sp3File = ac + sWwww + sDow + "_" + sHh + ".sp3";
                    cmd = changeFileName + " " + sp30File + " " + sp3File;
                    std::system(cmd.c_str());
                    if (access(sp3File.c_str(), 0) == -1)
                    {
                        /* extract '*.Z' */
                        string sp3zFile = sp30File + ".Z";
                        cmd = gzipFull + " -d -f " + sp3zFile;
                        std::system(cmd.c_str());

                        cmd = changeFileName + " " + sp30File + " " + sp3File;
                        std::system(cmd.c_str());
                    }
                }
                else
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string sp3xFile = sp3File + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + sp3xFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string sp3gzFile = sp3File + ".gz", sp3zFile = sp3File + ".Z";
                    if (access(sp3gzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + sp3gzFile;
                        std::system(cmd.c_str());
                    }
                    if (access(sp3File.c_str(), 0) == -1 && access(sp3zFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + sp3zFile;
                        std::system(cmd.c_str());
                    }
                }

                if (access(sp3File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download " << acName << " ultra-rapid orbit file " <<
                    sp3File << endl;
                else if (access(sp3File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download " << acName << " ultra-rapid orbit file " <<
                    sp3File << endl;

                if (ac == "igu")
                {
                    /* delete some temporary directories */
                    string tmpDir = "repro3";
                    if (access(tmpDir.c_str(), 0) == 0)
                    {
#ifdef _WIN32  /* for Windows */
                        cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
                        cmd = "rm -rf " + tmpDir;
#endif
                        std::system(cmd.c_str());
                    }
                }
            }
            else cout << "*** INFO(FtpUtil::GetOrbClk): " << acName << " ultra-rapid orbit file " << sp3File <<
                " has existed!" << endl;
        }
    }
    else if (isRapid)  /* for rapid orbit and clock products  */
    {
        string url, cutDirs, acName;
        string sp3File = ac + sWwww + sDow + ".sp3";
        string clkFile = ac + sWwww + sDow + ".clk";
        if (ac == "cor")  /* CODE */
        {
            sp3File = "COD" + sWwww + sDow + ".EPH_M";
            clkFile = "COD" + sWwww + sDow + ".CLK_M";
            url = "ftp://ftp.aiub.unibe.ch/CODE/" + sYyyy + "_M";
            cutDirs = " --cut-dirs=2 ";
            acName = "CODE";
        }
        else if (ac == "emp")  /* NRCan */
        {
            sp3File = "emr" + sWwww + sDow + ".sp3";
            clkFile = "emr" + sWwww + sDow + ".clk";
            url = "ftp://rtopsdata1.geod.nrcan.gc.ca/gps/products/rapid/" + sWwww;
            cutDirs = " --cut-dirs=4 ";
            acName = "NRCan";
        }
        else if (ac == "esr")  /* ESA */
        {
            url = "http://navigation-office.esa.int/products/gnss-products/" + sWwww;
            cutDirs = " --cut-dirs=3 ";
            acName = "ESA";
        }
        else if (ac == "gfr")  /* GFZ */
        {
            sp3File = "gfz" + sWwww + sDow + ".sp3";
            clkFile = "gfz" + sWwww + sDow + ".clk";
            url = "ftp://ftp.gfz-potsdam.de/pub/GNSS/products/rapid/w" + sWwww;
            cutDirs = " --cut-dirs=5 ";
            acName = "GFZ";
        }
        else if (ac == "igr")  /* IGS */
        {
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_SP3] + "/" + sWwww;
            else if (ftpName == "IGN") url = _ftpArchive.IGN[IDX_SP3] + "/" + sWwww;
            else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_SP3] + "/" + sWwww;
            else url = _ftpArchive.CDDIS[IDX_SP3] + "/" + sWwww;
            cutDirs = " --cut-dirs=4 ";
            acName = "IGS";
        }

        if (mode == 1)       /* SP3 file downloaded */
        {
            if (access(sp3File.c_str(), 0) == -1)
            {
                string cmd;
                if (ac == "esr")  /* ESA */
                {
                    string sp3zFile = sp3File + ".Z";
                    string url0 = url + "/" + sp3zFile;
                    cmd = wgetFull + " " + qr + " -nH " + cutDirs + url0;
                    std::system(cmd.c_str());

                    if (access(sp3zFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + sp3zFile;
                        std::system(cmd.c_str());
                    }
                    else
                    {
                        string sp3gzFile = sp3File + ".gz";
                        string url0 = url + "/" + sp3gzFile;
                        cmd = wgetFull + " " + qr + " -nH " + cutDirs + url0;
                        std::system(cmd.c_str());

                        if (access(sp3gzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.gz' */
                            cmd = gzipFull + " -d -f " + sp3gzFile;
                            std::system(cmd.c_str());
                        }
                    }
                }
                else
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string sp3xFile = sp3File + ".*";
                    cmd = wgetFull + " " + qr + " -nH -A " + sp3xFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string sp3gzFile = sp3File + ".gz", sp3zFile = sp3File + ".Z";
                    if (access(sp3gzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + sp3gzFile;
                        std::system(cmd.c_str());

                        if (access(sp3File.c_str(), 0) == 0 && access(sp3zFile.c_str(), 0) == 0)
                        {
                            /* delete '*.Z' file */
#ifdef _WIN32   /* for Windows */
                            cmd = "del " + sp3zFile;
#else           /* for Linux or Mac */
                            cmd = "rm -rf " + sp3zFile;
#endif
                            std::system(cmd.c_str());
                        }
                    }
                    if (access(sp3File.c_str(), 0) == -1 && access(sp3zFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + sp3zFile;
                        std::system(cmd.c_str());
                    }
                }

                if (access(sp3File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download " << acName << " rapid orbit file " <<
                    sp3File << endl;
                else if (access(sp3File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download " << acName << " rapid orbit file " <<
                    sp3File << endl;

                if (ac == "emp" || ac == "igr")
                {
                    /* delete some temporary directories */
                    string tmpDir;
                    if (ac == "emp") tmpDir = "dcm";
                    else if (ac == "igr") tmpDir = "repro3";
                    if (access(tmpDir.c_str(), 0) == 0)
                    {
#ifdef _WIN32  /* for Windows */
                        cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
                        cmd = "rm -rf " + tmpDir;
#endif
                        std::system(cmd.c_str());
                    }
                }
            }
            else cout << "*** INFO(FtpUtil::GetOrbClk): " << acName << " rapid orbit file " << sp3File <<
                " has existed!" << endl;
        }
        else if (mode == 2)  /* CLK file downloaded */
        {
            if (access(clkFile.c_str(), 0) == -1)
            {
                string cmd;
                if (ac == "esr")  /* ESA */
                {
                    string clkzFile = clkFile + ".Z";
                    url += "/" + clkzFile;
                    cmd = wgetFull + " " + qr + " -nH " + cutDirs + url;
                    std::system(cmd.c_str());

                    if (access(clkzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + clkzFile;
                        std::system(cmd.c_str());
                    }
                    else
                    {
                        string clkgzFile = clkFile + ".gz";
                        url += "/" + clkgzFile;
                        cmd = wgetFull + " " + qr + " -nH " + cutDirs + url;
                        std::system(cmd.c_str());

                        if (access(clkgzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.gz' */
                            cmd = gzipFull + " -d -f " + clkgzFile;
                            std::system(cmd.c_str());
                        }
                    }
                }
                else
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string clkxFile = clkFile + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + clkxFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string clkgzFile = clkFile + ".gz", clkzFile = clkFile + ".Z";
                    if (access(clkgzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + clkgzFile;
                        std::system(cmd.c_str());

                        if (access(clkFile.c_str(), 0) == 0 && access(clkzFile.c_str(), 0) == 0)
                        {
                            /* delete '*.Z' file */
#ifdef _WIN32   /* for Windows */
                            cmd = "del " + clkzFile;
#else           /* for Linux or Mac */
                            cmd = "rm -rf " + clkzFile;
#endif
                            std::system(cmd.c_str());
                        }
                    }
                    if (access(clkFile.c_str(), 0) == -1 && access(clkzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + clkzFile;
                        std::system(cmd.c_str());
                    }
                }

                if (access(clkFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download " << acName << " rapid clock file " <<
                    clkFile << endl;
                else if (access(clkFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download " << acName << " rapid clock file " <<
                    clkFile << endl;

                if (ac == "emp" || ac == "igr")
                {
                    /* delete some temporary directories */
                    string tmpDir;
                    if (ac == "emp") tmpDir = "dcm";
                    else if (ac == "igr") tmpDir = "repro3";
                    if (access(tmpDir.c_str(), 0) == 0)
                    {
#ifdef _WIN32  /* for Windows */
                        cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
                        cmd = "rm -rf " + tmpDir;
#endif
                        std::system(cmd.c_str());
                    }
                }
            }
            else cout << "*** INFO(FtpUtil::GetOrbClk): " << acName << " rapid clock file " << clkFile <<
                " has existed!" << endl;
        }
    }
    else if (isIGS)  /* for IGS final orbit and clock products  */
    {
        string sp3File = ac + sWwww + sDow + ".sp3";
        string clkFile = ac + sWwww + sDow + ".clk";
        string acName;
        if (ac == "cod")
        {
            sp3File = ac + sWwww + sDow + ".eph";
            clkFile = ac + sWwww + sDow + ".clk_05s";
            acName = "CODE";
        }
        else if (ac == "igs")
        {
            sp3File = ac + sWwww + sDow + ".sp3";
            clkFile = ac + sWwww + sDow + ".clk_30s";
            acName = "IGS";
        }
        else if (ac == "emr") acName = "NRCan";
        else if (ac == "esa") acName = "ESA";
        else if (ac == "gfz") acName = "GFZ";
        else if (ac == "grg") acName = "CNES";
        else if (ac == "jpl") acName = "JPL";

        string url, cutDirs = " --cut-dirs=4 ";
        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_SP3] + "/" + sWwww;
        else if (ftpName == "IGN") url = _ftpArchive.IGN[IDX_SP3] + "/" + sWwww;
        else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_SP3] + "/" + sWwww;
        else url = _ftpArchive.CDDIS[IDX_SP3] + "/" + sWwww;
        if (mode == 1)       /* SP3 file downloaded */
        {
            if (access(sp3File.c_str(), 0) == -1)
            {
                /* it is OK for '*.Z' or '*.gz' format */
                string sp3xFile = sp3File + ".*";
                string cmd = wgetFull + " " + qr + " -nH -A " + sp3xFile + cutDirs + url;
                std::system(cmd.c_str());

                string sp3gzFile = sp3File + ".gz", sp3zFile = sp3File + ".Z";
                if (access(sp3gzFile.c_str(), 0) == 0)
                {
                    /* extract '*.gz' */
                    cmd = gzipFull + " -d -f " + sp3gzFile;
                    std::system(cmd.c_str());
                }
                if (access(sp3File.c_str(), 0) == -1 && access(sp3zFile.c_str(), 0) == 0)
                {
                    /* extract '*.Z' */
                    cmd = gzipFull + " -d -f " + sp3zFile;
                    std::system(cmd.c_str());
                }

                if (access(sp3File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download " << acName << " precise orbit file " <<
                    sp3File << endl;
                else if (access(sp3File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download " << acName << " precise orbit file " <<
                    sp3File << endl;

                /* delete some temporary directories */
                string tmpDir = "repro3";
                if (access(tmpDir.c_str(), 0) == 0)
                {
    #ifdef _WIN32  /* for Windows */
                    cmd = "rd /s /q " + tmpDir;
    #else          /* for Linux or Mac */
                    cmd = "rm -rf " + tmpDir;
    #endif
                    std::system(cmd.c_str());
                }
            }
            else cout << "*** INFO(FtpUtil::GetOrbClk): " << acName << " precise orbit file " << sp3File <<
                " has existed!" << endl;
        }
        else if (mode == 2)  /* CLK file downloaded */
        {
            if (access(clkFile.c_str(), 0) == -1)
            {
                /* it is OK for '*.Z' or '*.gz' format */
                string clkxFile = clkFile + ".*";
                string cmd = wgetFull + " " + qr + " -nH -A " + clkxFile + cutDirs + url;
                std::system(cmd.c_str());

                string clkgzFile = clkFile + ".gz", clkzFile = clkFile + ".Z";
                if (access(clkgzFile.c_str(), 0) == 0)
                {
                    /* extract '*.gz' */
                    cmd = gzipFull + " -d -f " + clkgzFile;
                    std::system(cmd.c_str());
                }
                if (access(clkFile.c_str(), 0) == -1 && access(clkzFile.c_str(), 0) == 0)
                {
                    /* extract '*.Z' */
                    cmd = gzipFull + " -d -f " + clkzFile;
                    std::system(cmd.c_str());
                }

                if (access(clkFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download " << acName << " precise clock file " <<
                    clkFile << endl;
                else if (access(clkFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download " << acName << " precise clock file " <<
                    clkFile << endl;

                /* delete some temporary directories */
                string tmpDir = "repro3";
                if (access(tmpDir.c_str(), 0) == 0)
                {
    #ifdef _WIN32  /* for Windows */
                    cmd = "rd /s /q " + tmpDir;
    #else          /* for Linux or Mac */
                    cmd = "rm -rf " + tmpDir;
    #endif
                    std::system(cmd.c_str());
                }
            }
            else cout << "*** INFO(FtpUtil::GetOrbClk): " << acName << " precise clock file " << clkFile <<
                " has existed!" << endl;
        }
    }
    else if (isMGEX)  /* for MGEX final orbit and clock products */
    {
        string url, cutDirs = " --cut-dirs=5 ";
        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_SP3M] + "/" + sWwww;
        else if (ftpName == "IGN") url = _ftpArchive.IGN[IDX_SP3M] + "/" + sWwww;
        else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_SP3M] + "/" + sWwww;
        else url = _ftpArchive.CDDIS[IDX_SP3M] + "/" + sWwww;

        string sp3File, clkFile, sp30File, clk0File, acName;
        if (ac == "com")
        {
            sp3File = "COD0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_ORB.SP3";
            clkFile = "COD0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_CLK.CLK";
            sp30File = "com" + sWwww + sDow + ".sp3";
            clk0File = "com" + sWwww + sDow + ".clk";
            acName = "CODE";
        }
        else if (ac == "gbm")
        {
            sp3File = "GFZ0MGXRAP_" + sYyyy + sDoy + "0000_01D_" + "*_ORB.SP3";
            clkFile = "GFZ0MGXRAP_" + sYyyy + sDoy + "0000_01D_" + "*_CLK.CLK";
            sp30File = "gbm" + sWwww + sDow + ".sp3";
            clk0File = "gbm" + sWwww + sDow + ".clk";
            acName = "GFZ";
        }
        else if (ac == "grm")
        {
            sp3File = "GRG0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_ORB.SP3";
            clkFile = "GRG0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_CLK.CLK";
            sp30File = "grm" + sWwww + sDow + ".sp3";
            clk0File = "grm" + sWwww + sDow + ".clk";
            acName = "CNES";
        }
        else if (ac == "wum")
        {
            sp3File = "WUM0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_ORB.SP3";
            clkFile = "WUM0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_CLK.CLK";
            sp30File = "wum" + sWwww + sDow + ".sp3";
            clk0File = "wum" + sWwww + sDow + ".clk";
            acName = "WHU";
        }

        if (mode == 1)       /* SP3 file downloaded */
        {
            if (access(sp30File.c_str(), 0) == -1)
            {
                /* it is OK for '*.Z' or '*.gz' format */
                string sp3xFile = sp3File + ".*";
                string cmd = wgetFull + " " + qr + " -nH -A " + sp3xFile + cutDirs + url;
                std::system(cmd.c_str());

                /* extract '*.gz' */
                string sp3gzFile = sp3File + ".gz";
                cmd = gzipFull + " -d -f " + sp3gzFile;
                std::system(cmd.c_str());
                string changeFileName;
#ifdef _WIN32  /* for Windows */
                changeFileName = "rename";
#else          /* for Linux or Mac */
                changeFileName = "mv";
#endif
                cmd = changeFileName + " " + sp3File + " " + sp30File;
                std::system(cmd.c_str());
                if (access(sp30File.c_str(), 0) == -1)
                {
                    /* extract '*.Z' */
                    string sp3zFile = sp3File + ".Z";
                    cmd = gzipFull + " -d -f " + sp3zFile;
                    std::system(cmd.c_str());

                    cmd = changeFileName + " " + sp3File + " " + sp30File;
                    std::system(cmd.c_str());

                    if (access(sp30File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download " << acName << " MGEX precise orbit file " <<
                        sp30File << endl;
                }

                if (access(sp30File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download " << acName << " MGEX precise orbit file " << sp30File << endl;
            }
            else cout << "*** INFO(FtpUtil::GetOrbClk): " << acName << " MGEX precise orbit file " << sp30File <<
                " has existed!" << endl;
        }
        else if (mode == 2)  /* CLK file downloaded */
        {
            if (access(clk0File.c_str(), 0) == -1)
            {
                /* it is OK for '*.Z' or '*.gz' format */
                string clkxFile = clkFile + ".*";
                string cmd = wgetFull + " " + qr + " -nH -A " + clkxFile + cutDirs + url;
                std::system(cmd.c_str());

                /* extract '*.gz' */
                string clkgzFile = clkFile + ".gz";
                cmd = gzipFull + " -d -f " + clkgzFile;
                std::system(cmd.c_str());
                string changeFileName;
#ifdef _WIN32  /* for Windows */
                changeFileName = "rename";
#else          /* for Linux or Mac */
                changeFileName = "mv";
#endif
                cmd = changeFileName + " " + clkFile + " " + clk0File;
                std::system(cmd.c_str());
                if (access(clk0File.c_str(), 0) == -1)
                {
                    /* extract '*.Z' */
                    string clkzFile = clkFile + ".Z";
                    cmd = gzipFull + " -d -f " + clkzFile;
                    std::system(cmd.c_str());

                    cmd = changeFileName + " " + clkFile + " " + clk0File;
                    std::system(cmd.c_str());

                    if (access(clkFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download " << acName << " MGEX precise clock file " <<
                        clk0File << endl;
                }

                if (access(clk0File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download " << acName << " MGEX precise clock file " <<
                    clk0File << endl;
            }
            else cout << "*** INFO(FtpUtil::GetOrbClk): " << acName << " MGEX precise clock file " << clk0File <<
                " has existed!" << endl;
        }
    }
} /* end of GetOrbClk */

/**
* @brief   : GetEop - download earth rotation parameter files
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetEop(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute GPS week and day of week */
    TimeUtil tu;
    int wwww, dow;
    dow = tu.time2gpst(ts, &wwww, nullptr);
    StringUtil str;
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));

    string ac(fopt->orbClkOpt);
    str.ToLower(ac);

    /* esu: ESA ultra-rapid EOP products
       gfu: GFZ ultra-rapid EOP products
       igu: IGS ultra-rapid EOP products
    */
    std::vector<string> ultraAc = { "esu", "gfu", "igu" };
    bool isUltra = false;
    for (int i = 0; i < ultraAc.size(); i++)
    {
        if (ac == ultraAc[i]) isUltra = true;
    }

    /* cod: CODE final EOP products
       emr: NRCan final EOP products
       esa: ESA final EOP products
       gfz: GFZ final EOP products
       grg: CNES final EOP products
       igs: IGS final EOP products
       jpl: JPL final EOP products
    */
    std::vector<string> finalAcIGS = { "cod", "emr", "esa", "gfz", "grg", "igs", "jpl", "mit" };
    bool isIGS = false;
    for (int i = 0; i < finalAcIGS.size(); i++)
    {
        if (ac == finalAcIGS[i]) isIGS = true;
    }

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
    if (isUltra)  /* for ultra-rapid EOP products  */
    {
        string url, cutDirs, acName;
        if (ac == "esu")       /* ESA */
        {
            url = "http://navigation-office.esa.int/products/gnss-products/" + sWwww;
            cutDirs = " --cut-dirs=3 ";
            acName = "ESA";
        }
        else if (ac == "gfu")  /* GFZ */
        {
            url = "ftp://ftp.gfz-potsdam.de/pub/GNSS/products/ultra/w" + sWwww;
            cutDirs = " --cut-dirs=5 ";
            acName = "GFZ";
        }
        else if (ac == "igu")  /* IGS */
        {
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_EOP] + "/" + sWwww;
            else if (ftpName == "IGN") url = _ftpArchive.IGN[IDX_EOP] + "/" + sWwww;
            else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_EOP] + "/" + sWwww;
            else url = _ftpArchive.CDDIS[IDX_SP3] + "/" + sWwww;
            cutDirs = " --cut-dirs=4 ";
            acName = "IGS";
        }

        for (int i = 0; i < fopt->hhEop.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhEop[i]);
            string eopFile = ac + sWwww + sDow + "_" + sHh + ".erp";
            if (access(eopFile.c_str(), 0) == -1)
            {
                string cmd;
                if (ac == "esu")  /* ESA */
                {
                    string eopzFile = eopFile + ".Z";
                    string url0 = url + "/" + eopzFile;
                    cmd = wgetFull + " " + qr + " -nH " + cutDirs + url0;
                    std::system(cmd.c_str());

                    if (access(eopzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + eopzFile;
                        std::system(cmd.c_str());
                    }
                    else
                    {
                        string eopgzFile = eopFile + ".gz";
                        string url0 = url + "/" + eopgzFile;
                        cmd = wgetFull + " " + qr + " -nH " + cutDirs + url0;
                        std::system(cmd.c_str());

                        if (access(eopgzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.gz' */
                            cmd = gzipFull + " -d -f " + eopgzFile;
                            std::system(cmd.c_str());
                        }
                    }
                }
                else
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string eopxFile = eopFile + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + eopxFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string eopgzFile = eopFile + ".gz", eopzFile = eopFile + ".Z";
                    if (access(eopgzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + eopgzFile;
                        std::system(cmd.c_str());
                    }
                    if (access(eopFile.c_str(), 0) == -1 && access(eopzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + eopzFile;
                        std::system(cmd.c_str());
                    }
                }

                if (access(eopFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetEop): failed to download " << acName << " ultra-rapid EOP file " <<
                    eopFile << endl;
                else if (access(eopFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetEop): successfully download " << acName << " ultra-rapid EOP file " <<
                    eopFile << endl;

                if (ac == "igu")
                {
                    /* delete some temporary directories */
                    string tmpDir = "repro3";
                    if (access(tmpDir.c_str(), 0) == 0)
                    {
#ifdef _WIN32  /* for Windows */
                        cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
                        cmd = "rm -rf " + tmpDir;
#endif
                        std::system(cmd.c_str());
                    }
                }
            }
            else cout << "*** INFO(FtpUtil::GetEop): " << acName << " ultra-rapid EOP file " << eopFile << " has existed!" << endl;
        }
    }
    else if (isIGS)  /* for IGS final EOP products  */
    {
        string acName;
        if (ac == "cod") acName = "CODE";
        else if (ac == "emr") acName = "NRCan";
        else if (ac == "esa") acName = "ESA";
        else if (ac == "gfz") acName = "GFZ";
        else if (ac == "grg") acName = "CNES";
        else if (ac == "igs") acName = "IGS";
        else if (ac == "jpl") acName = "JPL";

        string eopFile = ac + sWwww + "7.erp";
        if (access(eopFile.c_str(), 0) == -1)
        {
            /* download the EOP file */
            string url, cutDirs = " --cut-dirs=4 ";
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_EOP] + "/" + sWwww;
            else if (ftpName == "IGN") url = _ftpArchive.IGN[IDX_EOP] + "/" + sWwww;
            else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_EOP] + "/" + sWwww;
            else url = _ftpArchive.CDDIS[IDX_EOP] + "/" + sWwww;

            /* it is OK for '*.Z' or '*.gz' format */
            string eopxFile = eopFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + eopxFile + cutDirs + url;
            std::system(cmd.c_str());

            string eopgzFile = eopFile + ".gz", eopzFile = eopFile + ".Z";
            if (access(eopgzFile.c_str(), 0) == 0)
            {
                /* extract '*.gz' */
                cmd = gzipFull + " -d -f " + eopgzFile;
                std::system(cmd.c_str());
            }
            if (access(eopFile.c_str(), 0) == -1 && access(eopzFile.c_str(), 0) == 0)
            {
                /* extract '*.Z' */
                cmd = gzipFull + " -d -f " + eopzFile;
                std::system(cmd.c_str());
            }

            if (access(eopFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetEop): failed to download " << acName << " final EOP file " <<
                eopFile << endl;
            else if (access(eopFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetEop): successfully download " << acName << " final EOP file " <<
                eopFile << endl;

            /* delete some temporary directories */
            string tmpDir = "repro3";
            if (access(tmpDir.c_str(), 0) == 0)
            {
#ifdef _WIN32  /* for Windows */
                cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
                cmd = "rm -rf " + tmpDir;
#endif
                std::system(cmd.c_str());
            }
        }
        else cout << "*** INFO(FtpUtil::GetEop): " << acName << " final EOP file " << eopFile << " has existed!" << endl;
    }
} /* end of GetOrbClkEop */

/**
* @brief   : GetSnx - download IGS weekly SINEX file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetSnx(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute GPS week and day of week */
    TimeUtil tu;
    int wwww;
    int dow = tu.time2gpst(ts, &wwww, nullptr);
    StringUtil str;
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string snx0File = "igs" + sWwww + ".snx";
    if (access(snx0File.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;        
        string url, cutDirs = " --cut-dirs=4 ";
        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_SNX] + "/" + sWwww;
        else if (ftpName == "IGN") url = _ftpArchive.IGN[IDX_SNX] + "/" + sWwww;
        else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_SNX] + "/" + sWwww;
        else url = _ftpArchive.CDDIS[IDX_SNX] + "/" + sWwww;
        
        /* it is OK for '*.Z' or '*.gz' format */
        string snxFile = "igs*P" + sWwww + ".snx";
        string snxxFile = snxFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + snxxFile + cutDirs + url;
        std::system(cmd.c_str());

        /* extract '*.Z' */
        string snxzFile = snxFile + ".Z";
        cmd = gzipFull + " -d -f " + snxzFile;
        std::system(cmd.c_str());
        string changeFileName;
#ifdef _WIN32  /* for Windows */
        changeFileName = "rename";
#else          /* for Linux or Mac */
        changeFileName = "mv";
#endif
        cmd = changeFileName + " " + snxFile + " " + snx0File;
        std::system(cmd.c_str());
        if (access(snx0File.c_str(), 0) == -1)
        {
            /* extract '*.Z' */
            string snxgzFile = snxFile + ".gz";
            cmd = gzipFull + " -d -f " + snxgzFile;
            std::system(cmd.c_str());

            cmd = changeFileName + " " + snxFile + " " + snx0File;
            std::system(cmd.c_str());
            if (access(snx0File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetSnx): failed to download IGS weekly SINEX file " <<
                snx0File << endl;
        }

        /* If the weekly SINEX file does not exist,  we use the daily SINEX file instead */
        if (access(snx0File.c_str(), 0) == -1)
        {
            string url, cutDirs = " --cut-dirs=4 ";
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_SNX] + "/" + sWwww;
            else if (ftpName == "IGN") url = _ftpArchive.IGN[IDX_SNX] + "/" + sWwww;
            else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_SNX] + "/" + sWwww;
            else url = _ftpArchive.CDDIS[IDX_SNX] + "/" + sWwww;
            
            /* it is OK for '*.Z' or '*.gz' format */
            string snxFile = "igs*P" + sWwww + sDow + ".snx";
            string snxxFile = snxFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + snxxFile + cutDirs + url;
            std::system(cmd.c_str());

            /* extract '*.Z' */
            string snxzFile = snxFile + ".Z";
            cmd = gzipFull + " -d -f " + snxzFile;
            std::system(cmd.c_str());
            string changeFileName;
#ifdef _WIN32  /* for Windows */
            changeFileName = "rename";
#else          /* for Linux or Mac */
            changeFileName = "mv";
#endif
            cmd = changeFileName + " " + snxFile + " " + snx0File;
            std::system(cmd.c_str());
            if (access(snx0File.c_str(), 0) == -1)
            {
                /* extract '*.Z' */
                string snxgzFile = snxFile + ".gz";
                cmd = gzipFull + " -d -f " + snxgzFile;
                std::system(cmd.c_str());

                cmd = changeFileName + " " + snxFile + " " + snx0File;
                std::system(cmd.c_str());
                if (access(snx0File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetSnx): failed to download IGS weekly SINEX file " <<
                    snx0File << endl;
            }
        }

        if (access(snx0File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetSnx): successfully download IGS weekly SINEX file " << 
            snx0File << endl;

        /* delete some temporary directories */
        string tmpDir = "repro3";
        if (access(tmpDir.c_str(), 0) == 0)
        {
#ifdef _WIN32  /* for Windows */
            cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
            cmd = "rm -rf " + tmpDir;
#endif
            std::system(cmd.c_str());
        }
    }
    else cout << "*** INFO(FtpUtil::GetSnx): IGS weekly SINEX file " << snx0File << " has existed!" << endl;
} /* end of GetSnx */

/**
* @brief   : GetDcbMgex - download MGEX daily DCB file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDcbMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sDoy = str.doy2str(doy);

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string dcbFile = "CAS0MGXRAP_" + sYyyy + sDoy + "0000_01D_01D_DCB.BSX";
    if (access(dcbFile.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
        string url, cutDirs = " --cut-dirs=5 ";
        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_DCBM] + "/" + sYyyy;
        else if (ftpName == "IGN")
        {
            url = _ftpArchive.IGN[IDX_DCBM] + "/" + sYyyy;
            cutDirs = " --cut-dirs=6 ";
        }
        else if (ftpName == "WHU")
        {
            url = _ftpArchive.WHU[IDX_DCBM] + "/" + sYyyy;
            cutDirs = " --cut-dirs=6 ";
        }
        else url = _ftpArchive.CDDIS[IDX_DCBM] + "/" + sYyyy;

        /* it is OK for '*.Z' or '*.gz' format */
        string dcbxFile = dcbFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + dcbxFile + cutDirs + url;
        std::system(cmd.c_str());

        string dcbgzFile = dcbFile + ".gz", dcbzFile = dcbFile + ".Z";
        if (access(dcbgzFile.c_str(), 0) == 0)
        {
            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + dcbgzFile;
            std::system(cmd.c_str());
        }
        else if (access(dcbzFile.c_str(), 0) == 0)
        {
            /* extract '*.Z' */
            cmd = gzipFull + " -d -f " + dcbzFile;
            std::system(cmd.c_str());
        }

        if (access(dcbFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetDcbMgex): failed to download multi-GNSS DCB file " <<
            dcbFile << endl;
        else if (access(dcbFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetDcbMgex): successfully download multi-GNSS DCB file " <<
            dcbFile << endl;
    }
    else cout << "*** INFO(FtpUtil::GetDcbMgex): multi-GNSS DCB file " << dcbFile << " has existed!" << endl;
} /* end of GetDcbMgex */

/**
* @brief   : GetDcbCode - download daily P1-P2, P1-C1, and P2-C2 DCB files from CODE
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: dType (DCB type, i.e., "P1P2", "P1C1", or "P2C2")
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDcbCode(gtime_t ts, const char dir[], const char dType[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy;
    tu.time2yrdoy(ts, &yyyy, nullptr);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    double date[6];
    tu.time2ymdhms(ts, date);
    int mm = (int)date[1];
    string sMm = str.mm2str(mm);

    string dcbFile, dcb0File;
    string type(dType);
    str.ToUpper(type);
    if (type == "P1P2")
    {
        dcbFile = "P1P2" + sYy + sMm + ".DCB";
        dcb0File = "P1P2" + sYy + sMm + ".DCB";
    }
    else if (type == "P1C1")
    {
        dcbFile = "P1C1" + sYy + sMm + ".DCB";
        dcb0File = "P1C1" + sYy + sMm + ".DCB";
    }
    else if (type == "P2C2")
    {
        dcbFile = "P2C2" + sYy + sMm + "_RINEX.DCB";
        dcb0File = "P2C2" + sYy + sMm + ".DCB";
    }
    
    if (access(dcbFile.c_str(), 0) == -1 && access(dcb0File.c_str(), 0) == -1)
    {
        /* download DCB file */
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
        string url = "ftp://ftp.aiub.unibe.ch/CODE/" + sYyyy;

        /* it is OK for '*.Z' or '*.gz' format */
        string dcbxFile = dcbFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + dcbxFile + " --cut-dirs=2 " + url;
        std::system(cmd.c_str());

        string dcbgzFile = dcbFile + ".gz", dcbzFile = dcbFile + ".Z";
        if (access(dcbgzFile.c_str(), 0) == 0)
        {
            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + dcbgzFile;
            std::system(cmd.c_str());
        }
        else if (access(dcbzFile.c_str(), 0) == 0)
        {
            /* extract '*.Z' */
            cmd = gzipFull + " -d -f " + dcbzFile;
            std::system(cmd.c_str());
        }

        if (access(dcbFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetDcbCode): failed to download CODE DCB file " <<
            dcbFile << endl;

        if (type == "P2C2")
        {
            string changeFileName;
#ifdef _WIN32  /* for Windows */
            changeFileName = "rename";
#else          /* for Linux or Mac */
            changeFileName = "mv";
#endif
            cmd = changeFileName + " " + dcbFile + " " + dcb0File;
            std::system(cmd.c_str());
        }

        if (access(dcb0File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetDcbCode): successfully download CODE DCB file " << dcb0File << endl;
    }
    else cout << "*** INFO(FtpUtil::GetDcbCode): CODE DCB file " << dcb0File << " or " << dcbFile <<
        " has existed!" << endl;
} /* end of GetDcbCode */

/**
* @brief   : GetIono - download daily GIM file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetIono(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string ac(fopt->ionOpt);
    str.ToLower(ac);
    string ionFile = ac + "g" + sDoy + "0." + sYy + "i";
    if (access(ionFile.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
        string url, cutDirs = " --cut-dirs=6 ";
        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_ION] + "/" + 
            sYyyy + "/" + sDoy;
        else if (ftpName == "IGN") url = _ftpArchive.IGN[IDX_ION] + "/" + 
            sYyyy + "/" + sDoy;
        else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_ION] + "/" + 
            sYyyy + "/" + sDoy;
        else url = _ftpArchive.CDDIS[IDX_ION] + "/" + sYyyy + "/" + sDoy;

        /* it is OK for '*.Z' or '*.gz' format */
        string ionxFile = ionFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + ionxFile + cutDirs + url;
        std::system(cmd.c_str());

        string iongzFile = ionFile + ".gz", ionzFile = ionFile + ".Z";
        if (access(iongzFile.c_str(), 0) == 0)
        {
            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + iongzFile;
            std::system(cmd.c_str());
        }
        else if (access(ionzFile.c_str(), 0) == 0)
        {
            /* extract '*.Z' */
            cmd = gzipFull + " -d -f " + ionzFile;
            std::system(cmd.c_str());
        }

        if (access(ionFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetIono): failed to download GIM file " <<
            ionFile << endl;
        else if (access(ionFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetIono): successfully download GIM file " <<
            ionFile << endl;

        /* delete some temporary directories */
        string tmpDir = "topex";
        if (access(tmpDir.c_str(), 0) == 0)
        {
#ifdef _WIN32  /* for Windows */
            cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
            cmd = "rm -rf " + tmpDir;
#endif
            std::system(cmd.c_str());
        }
    }
    else cout << "*** INFO(FtpUtil::GetIono): GIM file " << ionFile << " has existed!" << endl;
} /* end of GetIono */

/**
* @brief   : GetRoti - download daily Rate of TEC index (ROTI) file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetRoti(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string rotFile = "roti" + sDoy + "0." + sYy + "f";
    if (access(rotFile.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
        string url, cutDirs = " --cut-dirs=6 ";
        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_ROTI] + "/" +
            sYyyy + "/" + sDoy;
        else if (ftpName == "IGN") url = _ftpArchive.IGN[IDX_ROTI] + "/" +
            sYyyy + "/" + sDoy;
        else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_ROTI] + "/" +
            sYyyy + "/" + sDoy;
        else url = _ftpArchive.CDDIS[IDX_ROTI] + "/" + sYyyy + "/" + sDoy;

        /* it is OK for '*.Z' or '*.gz' format */
        string rotxFile = rotFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + rotxFile + cutDirs + url;
        std::system(cmd.c_str());

        string rotgzFile = rotFile + ".gz", rotzFile = rotFile + ".Z";
        if (access(rotgzFile.c_str(), 0) == 0)
        {
            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + rotgzFile;
            std::system(cmd.c_str());
        }
        else if (access(rotzFile.c_str(), 0) == 0)
        {
            /* extract '*.Z' */
            cmd = gzipFull + " -d -f " + rotzFile;
            std::system(cmd.c_str());
        }

        if (access(rotFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetRoti): failed to download ROTI file " <<
            rotFile << endl;
        else if (access(rotFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetRoti): successfully download ROTI file " <<
            rotFile << endl;

        /* delete some temporary directories */
        string tmpDir = "topex";
        if (access(tmpDir.c_str(), 0) == 0)
        {
#ifdef _WIN32  /* for Windows */
            cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
            cmd = "rm -rf " + tmpDir;
#endif
            std::system(cmd.c_str());
        }
    }
    else cout << "*** INFO(FtpUtil::GetRoti): ROTI file " << rotFile << " has existed!" << endl;
} /* end of GetRoti */

/**
* @brief   : GetTrop - download daily tropospheric product file from CDDIS and/or CODE
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetTrop(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);
    int wwww;
    int dow = tu.time2gpst(ts, &wwww, nullptr);
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));

    string ac(fopt->trpOpt);
    str.ToLower(ac);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
    if (ac == "igs")
    {
        string ftpName = fopt->ftpFrom;
        str.ToUpper(ftpName);
        string sitFile = fopt->obsOpt;
        if (fopt->getObs) sitFile = fopt->obsOpt;
        else if (fopt->getObm) sitFile = fopt->obmOpt;
        if (strlen(sitFile.c_str()) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
        {
            /* download all the IGS ZPD files */
            string url, cutDirs = " --cut-dirs=7 ";
            if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_ZTD] + "/" +
                sYyyy + "/" + sDoy;
            else if (ftpName == "IGN")
            {
                url = _ftpArchive.IGN[IDX_ZTD] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=6 ";
            }
            else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_ZTD] + "/" +
                sYyyy + "/" + sDoy;
            else url = _ftpArchive.CDDIS[IDX_ZTD] + "/" + sYyyy + "/" + sDoy;

            /* it is OK for '*.Z' or '*.gz' format */
            string zpdFile = "*" + sDoy + "0." + sYy + "zpd";
            string zpdxFile = zpdFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + zpdxFile + cutDirs + url;
            std::system(cmd.c_str());

            /* get the file list */
            string suffix = "." + sYy + "zpd";
            vector<string> zpdFiles;
            str.GetFilesAll(dir, suffix, zpdFiles);
            for (int i = 0; i < zpdFiles.size(); i++)
            {
                if (access(zpdFiles[i].c_str(), 0) == 0)
                {
                    /* extract it */
                    cmd = gzipFull + " -d -f " + zpdFiles[i];
                    std::system(cmd.c_str());
                }
            }
        }
        else  /* the option of full path of site list file is selected */
        {
            /* download the IGS ZPD file site-by-site */
            if (access(sitFile.c_str(), 0) == 0)
            {
                ifstream sitLst(sitFile.c_str());
                if (!sitLst.is_open())
                {
                    cerr << "*** ERROR(FtpUtil::GetTrop): open site.list = " << sitFile << " file failed, please check it" << endl;

                    return;
                }

                string sitName;
                while (getline(sitLst, sitName))
                {
                    str.ToLower(sitName);
                    string zpdFile = sitName + sDoy + "0." + sYy + "zpd";
                    if (access(zpdFile.c_str(), 0) == -1)
                    {
                        string url, cutDirs = " --cut-dirs=7 ";
                        if (ftpName == "CDDIS") url = _ftpArchive.CDDIS[IDX_ZTD] + "/" +
                            sYyyy + "/" + sDoy;
                        else if (ftpName == "IGN")
                        {
                            url = _ftpArchive.IGN[IDX_ZTD] + "/" + sYyyy + "/" + sDoy;
                            cutDirs = " --cut-dirs=6 ";
                        }
                        else if (ftpName == "WHU") url = _ftpArchive.WHU[IDX_ZTD] + "/" +
                            sYyyy + "/" + sDoy;
                        
                        /* it is OK for '*.Z' or '*.gz' format */
                        string zpdxFile = zpdFile + ".*";
                        string cmd = wgetFull + " " + qr + " -nH -A " + zpdxFile + cutDirs + url;
                        std::system(cmd.c_str());

                        string zpdgzFile = zpdFile + ".gz", zpdzFile = zpdFile + ".Z";
                        if (access(zpdgzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.gz' */
                            cmd = gzipFull + " -d -f " + zpdgzFile;
                            std::system(cmd.c_str());
                        }
                        else if (access(zpdzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.Z' */
                            cmd = gzipFull + " -d -f " + zpdzFile;
                            std::system(cmd.c_str());
                        }

                        if (access(zpdFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetTrop): failed to download IGS tropospheric product file " <<
                            zpdFile << endl;
                        else if (access(zpdFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetTrop): successfully download IGS tropospheric product file " <<
                            zpdFile << endl;
                    }
                    else cout << "*** INFO(FtpUtil::GetTrop): IGS tropospheric product file " << zpdFile << 
                        " has existed!" << endl;
                }

                /* close 'site.list' */
                sitLst.close();
            }
        }
    }
    else if (ac == "cod")
    {
        string trpFile = "COD" + sWwww + sDow + ".TRO";
        if (access(trpFile.c_str(), 0) == -1)
        {
            string url = "ftp://ftp.aiub.unibe.ch/CODE/" + sYyyy;
            string cutDirs = " --cut-dirs=2 ";

            /* it is OK for '*.Z' or '*.gz' format */
            string trpxFile = trpFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + trpxFile + cutDirs + url;
            std::system(cmd.c_str());

            string trpgzFile = trpFile + ".gz", trpzFile = trpFile + ".Z";
            if (access(trpgzFile.c_str(), 0) == 0)
            {
                /* extract '*.gz' */
                cmd = gzipFull + " -d -f " + trpgzFile;
                std::system(cmd.c_str());
            }
            else if (access(trpzFile.c_str(), 0) == 0)
            {
                /* extract '*.Z' */
                cmd = gzipFull + " -d -f " + trpzFile;
                std::system(cmd.c_str());
            }

            if (access(trpFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetTrop): failed to download CODE tropospheric product file " <<
                trpFile << endl;
            else if (access(trpFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetTrop): successfully download CODE tropospheric product file " <<
                trpFile << endl;
        }
        else cout << "*** INFO(FtpUtil::GetTrop): CODE tropospheric product file " << trpFile <<
            " has existed!" << endl;
    }
} /* end of GetTrop */

/**
* @brief   : GetRtOrbClkCNT - download real-time precise orbit and clock products from CNES offline files
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: mode (=1:sp3 downloaded; =2:clk downloadeded)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetRtOrbClkCNT(gtime_t ts, const char dir[], int mode, const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute GPS week and day of week */
    TimeUtil tu;
    int wwww, dow;
    dow = tu.time2gpst(ts, &wwww, nullptr);
    StringUtil str;
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));

    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
    if (mode == 1)       /* SP3 file downloaded */
    {
        string sp3File = "cnt" + sWwww + sDow + ".sp3";
        if (access(sp3File.c_str(), 0) == -1)
        {
            string sp3gzFile = sp3File + ".gz";
            string url = "http://www.ppp-wizard.net/products/REAL_TIME/" + sp3gzFile;
            string cmd = wgetFull + " " + qr + " -nH --cut-dirs=2 " + url;
            std::system(cmd.c_str());

            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + sp3gzFile;
            std::system(cmd.c_str());
            if (access(sp3File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): successfully download CNES real-time precise orbit file " << 
                sp3File << endl;
            else cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): failed to download CNES real-time precise orbit file " <<
                sp3File << endl;

            /* delete some temporary directories */
            std::vector<string> tmpDir = { "FORMAT_BIAIS_OFFI1", "FORMATBIAS_OFF_v1" };
            for (int i = 0; i < tmpDir.size(); i++)
            {
                if (access(tmpDir[i].c_str(), 0) == 0)
                {
#ifdef _WIN32  /* for Windows */
                    cmd = "rd /s /q " + tmpDir[i];
#else          /* for Linux or Mac */
                    cmd = "rm -rf " + tmpDir[i];
#endif
                    std::system(cmd.c_str());
                }
            }
        }
        else cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): CNES real-time precise orbit file " << sp3File << 
            " has existed!" << endl;
    }
    else if (mode == 2)  /* CLK file downloaded */
    {
        string clkFile = "cnt" + sWwww + sDow + ".clk";
        if (access(clkFile.c_str(), 0) == -1)
        {
            string clkgzFile = clkFile + ".gz";
            string url = "http://www.ppp-wizard.net/products/REAL_TIME/" + clkgzFile;
            string cmd = wgetFull + " " + qr + " -nH --cut-dirs=2 " + url;
            std::system(cmd.c_str());

            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + clkgzFile;
            std::system(cmd.c_str());
            if (access(clkFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): successfully download CNES real-time precise clock file " <<
                clkFile << endl;
            else cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): failed to download CNES real-time precise clock file " <<
                clkFile << endl;

            /* delete some temporary directories */
            std::vector<string> tmpDir = { "FORMAT_BIAIS_OFFI1", "FORMATBIAS_OFF_v1" };
            for (int i = 0; i < tmpDir.size(); i++)
            {
                if (access(tmpDir[i].c_str(), 0) == 0)
                {
#ifdef _WIN32  /* for Windows */
                    cmd = "rd /s /q " + tmpDir[i];
#else          /* for Linux or Mac */
                    cmd = "rm -rf " + tmpDir[i];
#endif
                    std::system(cmd.c_str());
                }
            }
        }
        else cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): CNES real-time precise clock file " << clkFile << 
            " has existed!" << endl;
    }
} /* end of GetRtOrbClkCNT */

/**
* @brief   : GetRtBiasCNT - download real-time code and phase bias products from CNES offline files
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetRtBiasCNT(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute GPS week and day of week */
    TimeUtil tu;
    int wwww, dow;
    dow = tu.time2gpst(ts, &wwww, nullptr);
    StringUtil str;
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));

    /* code and phase bias file downloaded */
    string biaFile = "cnt" + sWwww + sDow + ".bia";
    if (access(biaFile.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
        string biagzFile = biaFile + ".gz";
        string url = "http://www.ppp-wizard.net/products/REAL_TIME/" + biagzFile;
        string cmd = wgetFull + " " + qr + " -nH --cut-dirs=2 " + url;
        std::system(cmd.c_str());

        /* extract '*.gz' */
        cmd = gzipFull + " -d -f " + biagzFile;
        std::system(cmd.c_str());
        if (access(biaFile.c_str(), 0) == 0) cout << "*** INFO(GetRtBiasCNT): successfully download CNES real-time code and phase bias file " << 
            biaFile << endl;
        else cout << "*** INFO(FtpUtil::GetRtBiasCNT): failed to download CNES real-time code and phase bias file " <<
            biaFile << endl;

        /* delete some temporary directories */
        std::vector<string> tmpDir = { "FORMAT_BIAIS_OFFI1", "FORMATBIAS_OFF_v1" };
        for (int i = 0; i < tmpDir.size(); i++)
        {
            if (access(tmpDir[i].c_str(), 0) == 0)
            {
#ifdef _WIN32  /* for Windows */
                cmd = "rd /s /q " + tmpDir[i];
#else          /* for Linux or Mac */
                cmd = "rm -rf " + tmpDir[i];
#endif
                std::system(cmd.c_str());
            }
        }
    }
    else cout << "*** INFO(FtpUtil::GetRtBiasCNT): CNES real-time code and phase bias file " << biaFile <<
        " has existed!" << endl;
} /* end of GetRtBiasCNT */

/**
* @brief   : GetAntexIGS - download ANTEX file from IGS
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetAntexIGS(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    string atxFile("igs14.atx");
    if (access(atxFile.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, qr = fopt->qr;
        string url = "https://files.igs.org/pub/station/general/" + atxFile;
        string cmd = wgetFull + " " + qr + " -nH --cut-dirs=3 " + url;
        std::system(cmd.c_str());

        if (access(atxFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetAntexIGS): successfully download IGS ANTEX file " << atxFile << endl;
    }
    else cout << "*** INFO(FtpUtil::GetAntexIGS): IGS ANTEX file " << atxFile << " has existed!" << endl;
} /* end of GetAntexIGS */

/**
* @brief     : FtpDownload - GNSS data downloading via FTP
* @param[I]  : popt (processing options)
* @param[I/O]: fopt (FTP options)
* @return    : none
* @note      :
**/
void FtpUtil::FtpDownload(const prcopt_t *popt, ftpopt_t *fopt)
{
    /* Get FTP archive for CDDIS, IGN, or WHU */
    init();

    /* setting of the third-party softwares (.i.e, wget, gzip, crx2rnx etc.) */
    StringUtil str;
    if (fopt->isPath3party)  /* the path of third-party softwares is NOT set in the environmental variable */
    {
        char cmdTmp[MAXCHARS] = { '\0' };
        char sep = (char)FILEPATHSEP;

        /* for wget */
        sprintf(cmdTmp, "%s%c%s", fopt->dir3party, sep, "wget");
        str.TrimSpace(cmdTmp);
        str.CutFilePathSep(cmdTmp);
        strcpy(fopt->wgetFull, cmdTmp);

        /* for gzip */
        sprintf(cmdTmp, "%s%c%s", fopt->dir3party, sep, "gzip");
        str.TrimSpace(cmdTmp);
        str.CutFilePathSep(cmdTmp);
        strcpy(fopt->gzipFull, cmdTmp);

        /* for crx2rnx */
        sprintf(cmdTmp, "%s%c%s", fopt->dir3party, sep, "crx2rnx");
        str.TrimSpace(cmdTmp);
        str.CutFilePathSep(cmdTmp);
        strcpy(fopt->crx2rnxFull, cmdTmp);
    }
    else  /* the path of third-party softwares is set in the environmental variable */
    {
        str.SetStr(fopt->wgetFull, "wget", 5);
        str.SetStr(fopt->gzipFull, "gzip", 5);
        str.SetStr(fopt->crx2rnxFull, "crx2rnx", 8);
    }

    /* "-qr" (printInfoWget == false, not print information of 'wget') or "r" (printInfoWget == true, print information of 'wget') in 'wget' command line */
    if (fopt->printInfoWget) str.SetStr(fopt->qr, "-r", 3);
    else str.SetStr(fopt->qr, "-qr", 4);

    /* IGS observation (short name 'd') downloaded */
    if (fopt->getObs)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->obsDir, 0) == -1)
        {
            string tmpDir = popt->obsDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        if (strcmp(fopt->obsTyp, "daily") == 0) GetDailyObsIgs(popt->ts, popt->obsDir, fopt);
        else if (strcmp(fopt->obsTyp, "hourly") == 0) GetHourlyObsIgs(popt->ts, popt->obsDir, fopt);
        else if (strcmp(fopt->obsTyp, "highrate") == 0) GetHrObsIgs(popt->ts, popt->obsDir, fopt);
    }

    /* MGEX observation (long name 'crx') downloaded */
    if (fopt->getObm)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->obmDir, 0) == -1)
        {
            string tmpDir = popt->obmDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        if (strcmp(fopt->obmTyp, "daily") == 0) GetDailyObsMgex(popt->ts, popt->obmDir, fopt);
        else if (strcmp(fopt->obmTyp, "hourly") == 0) GetHourlyObsMgex(popt->ts, popt->obmDir, fopt);
        else if (strcmp(fopt->obmTyp, "highrate") == 0) GetHrObsMgex(popt->ts, popt->obmDir, fopt);
    }

    /* Curtin University of Technology (CUT) observation (long name 'crx') downloaded */
    if (fopt->getObc)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->obcDir, 0) == -1)
        {
            string tmpDir = popt->obcDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        if (strcmp(fopt->obcTyp, "daily") == 0) GetDailyObsCut(popt->ts, popt->obcDir, fopt);
    }

    /* Geoscience Australia (GA) observation (long name 'crx') downloaded */
    if (fopt->getObg)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->obgDir, 0) == -1)
        {
            string tmpDir = popt->obgDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        if (strcmp(fopt->obgTyp, "daily") == 0) GetDailyObsGa(popt->ts, popt->obgDir, fopt);
        else if (strcmp(fopt->obgTyp, "hourly") == 0) GetHourlyObsGa(popt->ts, popt->obgDir, fopt);
        else if (strcmp(fopt->obgTyp, "highrate") == 0) GetHrObsGa(popt->ts, popt->obgDir, fopt);
    }

    /* Hong Kong CORS observation (long name 'crx') downloaded */
    if (fopt->getObh)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->obhDir, 0) == -1)
        {
            string tmpDir = popt->obhDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        if (strcmp(fopt->obhTyp, "30s") == 0 || strcmp(fopt->obhTyp, "30 s") == 0)
            Get30sObsHk(popt->ts, popt->obhDir, fopt);
        else if (strcmp(fopt->obhTyp, "5s") == 0 || strcmp(fopt->obhTyp, "05s") == 0)
            Get5sObsHk(popt->ts, popt->obhDir, fopt);
        else if (strcmp(fopt->obhTyp, "1s") == 0 || strcmp(fopt->obhTyp, "01s") == 0)
            Get1sObsHk(popt->ts, popt->obhDir, fopt);
    }

    /* NGS/NOAA CORS observation (short name 'd') downloaded */
    if (fopt->getObn)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->obnDir, 0) == -1)
        {
            string tmpDir = popt->obnDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        if (strcmp(fopt->obnTyp, "daily") == 0) GetDailyObsNgs(popt->ts, popt->obnDir, fopt);
    }

    /* EUREF Permanent Network (EPN) observation (long name 'crx' and short name 'd') downloaded */
    if (fopt->getObe)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->obeDir, 0) == -1)
        {
            string tmpDir = popt->obeDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        if (strcmp(fopt->obeTyp, "daily") == 0) GetDailyObsEpn(popt->ts, popt->obeDir, fopt);
    }

    /* broadcast ephemeris downloaded */
    if (fopt->getNav)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->navDir, 0) == -1)
        {
            string tmpDir = popt->navDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetNav(popt->ts, popt->navDir, fopt);
    }

    /* precise orbit and clock product downloaded for the current day */
    if (fopt->getOrbClk)
    {
        string ac(fopt->orbClkOpt);
        str.ToLower(ac);

        /* esu: ESA ultra-rapid orbit and clock products
           gfu: GFZ ultra-rapid orbit and clock products
           igu: IGS ultra-rapid orbit and clock products
           wuu: WHU ultra-rapid orbit and clock products
        */
        std::vector<string> ultraAc = { "esu", "gfu", "igu", "wuu" };
        bool isUltra = false;
        for (int i = 0; i < ultraAc.size(); i++)
        {
            if (ac == ultraAc[i]) isUltra = true;
        }

        /* cor: CODE rapid orbit and clock products
           emp: NRCan rapid orbit and clock products
           esr: ESA rapid orbit and clock products
           gfr: GFZ rapid orbit and clock products
           igr: IGS rapid orbit and clock products
        */
        std::vector<string> rapidAc = { "cor", "emp", "esr", "gfr", "igr" };
        bool isRapid = false;
        for (int i = 0; i < rapidAc.size(); i++)
        {
            if (ac == rapidAc[i]) isRapid = true;
        }

        /* cod: CODE final orbit and clock products
           emr: NRCan final orbit and clock products
           esa: ESA final orbit and clock products
           gfz: GFZ final orbit and clock products
           grg: CNES final orbit and clock products
           igs: IGS final orbit and clock products
           jpl: JPL final orbit and clock products
        */
        std::vector<string> finalAcIGS = { "cod", "emr", "esa", "gfz", "grg", "igs", "jpl", "mit" };
        bool isIGS = false;
        for (int i = 0; i < finalAcIGS.size(); i++)
        {
            if (ac == finalAcIGS[i]) isIGS = true;
        }

        /* com: CODE multi-GNSS final orbit and clock products
           gbm: GFZ multi-GNSS final orbit and clock products
           grm: CNES multi-GNSS final orbit and clock products
           wum: WHU multi-GNSS final orbit and clock products
        */
        std::vector<string> acMGEX = { "com", "gbm", "grm", "wum" };
        bool isMGEX = false;
        for (int i = 0; i < acMGEX.size(); i++)
        {
            if (ac == acMGEX[i]) isMGEX = true;
        }

        /* creation of sub-directory */
        string subSp3Dir = popt->sp3Dir;
        string subClkDir = popt->clkDir;
        char tmpDir[MAXSTRPATH] = { '\0' };
        char sep = (char)FILEPATHSEP;
        if (isUltra)
        {
            sprintf(tmpDir, "%s%c%s", popt->sp3Dir, sep, "ultra");
            subSp3Dir = tmpDir;
        }
        else if (isRapid)
        {
            sprintf(tmpDir, "%s%c%s", popt->sp3Dir, sep, "rapid");
            subSp3Dir = tmpDir;
            sprintf(tmpDir, "%s%c%s", popt->clkDir, sep, "rapid");
            subClkDir = tmpDir;
        }
        else if (isIGS || isMGEX)
        {
            sprintf(tmpDir, "%s%c%s", popt->sp3Dir, sep, "final");
            subSp3Dir = tmpDir;
            sprintf(tmpDir, "%s%c%s", popt->clkDir, sep, "final");
            subClkDir = tmpDir;
        }

        /* for sp3 */
        if (access(subSp3Dir.c_str(), 0) == -1)
        {
            /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + subSp3Dir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + subSp3Dir;
#endif
            std::system(cmd.c_str());
        }

        GetOrbClk(popt->ts, subSp3Dir.c_str(), 1, fopt);  /* sp3 */

        /* for clk */
        if (isRapid || isIGS || isMGEX)
        {
            if (access(subClkDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + subClkDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + subClkDir;
#endif
                std::system(cmd.c_str());
            }

            GetOrbClk(popt->ts, subClkDir.c_str(), 2, fopt);  /* clk */
        }

        if (fopt->minusAdd1day && (isRapid || isIGS || isMGEX))
        {
            TimeUtil tu;
            gtime_t tt;
            /* precise orbit and clock product downloaded for the day before */
            tt = tu.TimeAdd(popt->ts, -86400.0);
            GetOrbClk(tt, subSp3Dir.c_str(), 1, fopt);
            GetOrbClk(tt, subClkDir.c_str(), 2, fopt);

            /* precise orbit and clock product downloaded for the day after */
            tt = tu.TimeAdd(popt->ts, 86400.0);
            GetOrbClk(tt, subSp3Dir.c_str(), 1, fopt);
            GetOrbClk(tt, subClkDir.c_str(), 2, fopt);
        }
    }
    
    /* EOP file downloaded */
    if (fopt->getEop)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->eopDir, 0) == -1)
        {
            string tmpDir = popt->eopDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetEop(popt->ts, popt->eopDir, fopt);
    }

    /* IGS SINEX file downloaded */
    if (fopt->getSnx)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->snxDir, 0) == -1)
        {
            string tmpDir = popt->snxDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetSnx(popt->ts, popt->snxDir, fopt);
    }

    /* MGEX and/or CODE DCB files downloaded */
    if (fopt->getDcb)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->dcbDir, 0) == -1)
        {
            string tmpDir = popt->dcbDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetDcbCode(popt->ts, popt->dcbDir, "P1P2", fopt);
        GetDcbCode(popt->ts, popt->dcbDir, "P1C1", fopt);
        GetDcbCode(popt->ts, popt->dcbDir, "P2C2", fopt);
        GetDcbMgex(popt->ts, popt->dcbDir, fopt);
    }

    /* global ionosphere map (GIM) downloaded */
    if (fopt->getIon)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->ionDir, 0) == -1)
        {
            string tmpDir = popt->ionDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetIono(popt->ts, popt->ionDir, fopt);
    }

    /* Rate of TEC index (ROTI) file downloaded */
    if (fopt->getRoti)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->ionDir, 0) == -1)
        {
            string tmpDir = popt->ionDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetRoti(popt->ts, popt->ionDir, fopt);
    }

    /* final tropospheric product downloaded */
    if (fopt->getTrp)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->ztdDir, 0) == -1)
        {
            string tmpDir = popt->ztdDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetTrop(popt->ts, popt->ztdDir, fopt);
    }

    /* real-time precise orbit and clock products from CNES offline files downloaded */
    if (fopt->getRtOrbClk)
    {
        /* creation of sub-directory */
        char tmpDir[MAXSTRPATH] = { '\0' };
        char sep = (char)FILEPATHSEP;
        sprintf(tmpDir, "%s%c%s", popt->sp3Dir, sep, "real_time");
        string subSp3Dir = tmpDir;
        /* for sp3 */
        if (access(subSp3Dir.c_str(), 0) == -1)
        {
            /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + subSp3Dir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + subSp3Dir;
#endif
            std::system(cmd.c_str());
        }

        GetRtOrbClkCNT(popt->ts, subSp3Dir.c_str(), 1, fopt);  /* sp3 */

        /* creation of sub-directory */
        sprintf(tmpDir, "%s%c%s", popt->clkDir, sep, "real_time");
        string subClkDir = tmpDir;
        /* for clk */
        if (access(subClkDir.c_str(), 0) == -1)
        {
            /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + subClkDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + subClkDir;
#endif
            std::system(cmd.c_str());
        }

        GetRtOrbClkCNT(popt->ts, subClkDir.c_str(), 2, fopt);  /* clk */

        if (fopt->minusAdd1day)
        {
            TimeUtil tu;
            gtime_t tt;
            /* real-time precise orbit and clock files downloaded for the day before */
            tt = tu.TimeAdd(popt->ts, -86400.0);
            GetRtOrbClkCNT(tt, subSp3Dir.c_str(), 1, fopt);
            GetRtOrbClkCNT(tt, subClkDir.c_str(), 2, fopt);

            /* real-time precise orbit and clock files downloaded for the day after */
            tt = tu.TimeAdd(popt->ts, 86400.0);
            GetRtOrbClkCNT(tt, subSp3Dir.c_str(), 1, fopt);
            GetRtOrbClkCNT(tt, subClkDir.c_str(), 2, fopt);
        }
    }

    /* real-time code and phase bias products from CNES offline files downloaded */
    if (fopt->getRtBias)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->biaDir, 0) == -1)
        {
            string tmpDir = popt->biaDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetRtBiasCNT(popt->ts, popt->biaDir, fopt);
    }

    /* IGS ANTEX file downloaded */
    if (fopt->getAtx)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->tblDir, 0) == -1)
        {
            string tmpDir = popt->tblDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetAntexIGS(popt->ts, popt->tblDir, fopt);
    }
} /* end of FtpDownload */