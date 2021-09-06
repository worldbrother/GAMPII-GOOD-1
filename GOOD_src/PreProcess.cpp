/*------------------------------------------------------------------------------
* PreProcess.cpp : string functions
*
* Copyright (C) 2020-2099 by SpAtial SurveyIng and Navigation (SASIN), all rights reserved.
*    This file is part of GAMP II - GOOD (Gnss Observations and prOducts Downloader)
*
* References:
*    
*
* history : 2020/09/25 1.0  new (by Feng Zhou)
*           2021/04/30      a bug for EOP downloading is fixed (by Feng Zhou @ SDUST)
*           2021/04/30      the option "minusAdd1day" is added (by Feng Zhou @ SDUST)
*-----------------------------------------------------------------------------*/
#include "Good.h"
#include "StringUtil.h"
#include "TimeUtil.h"
#include "FtpUtil.h"
#include "PreProcess.h"


/* constants/macros ----------------------------------------------------------*/
#define MIN(x,y)    ((x) <= (y) ? (x) : (y))


/* function definition -------------------------------------------------------*/

/**
* @brief   : init - some initializations before processing
* @param[O]: popt (processing options)
* @param[O]: fopt (FTP options, nullptr:NO output)
* @return  : none
* @note    :
**/
void PreProcess::init(prcopt_t *popt, ftpopt_t *fopt)
{
    /* initialization for processing options */
    StringUtil str;
    /* processing directory */
    str.SetStr(popt->mainDir, "", 1);            /* the root/main directory of GNSS observations and products */
                                                 /* the settings below are the sub-directories, and two parameters are needed
                                                    1st: (0  1), 0: use the path of root/main directory; 1: NOT use the path of root/main directory
                                                    If '0' is set, the full path will NOT be needed, i.e., 'obsDir         = 0  obs', indicating 'obsDir         = D:\data\obs'
                                                    If '1' is set, the full path should be given, i.e., 'obsDir         = 0  D:\data\obs'
                                                 */
    str.SetStr(popt->obsDir, "", 1);             /* the directory of IGS RINEX format observation files (short name "d" files) */
    str.SetStr(popt->obmDir, "", 1);             /* the directory of MGEX RINEX format observation files (long name "crx" files) */
    str.SetStr(popt->obcDir, "", 1);             /* the directory of Curtin University of Technology (CUT) RINEX format observation files (long name "crx" files) */
    str.SetStr(popt->obgDir, "", 1);             /* the directory of Geoscience Australia (GA) RINEX format observation files (long name "crx" files) */
    str.SetStr(popt->obhDir, "", 1);             /* the directory of Hong Kong CORS RINEX format observation files (long name "crx" files) */
    str.SetStr(popt->obnDir, "", 1);             /* the directory of NGS/NOAA CORS RINEX format observation files (short name "d" files) */
    str.SetStr(popt->obeDir, "", 1);             /* the directory of EUREF Permanent Network (EPN) RINEX format observation files (long name 'crx' files and short name "d" files) */
    str.SetStr(popt->navDir, "", 1);             /* the directory of RINEX format broadcast ephemeris files */
    str.SetStr(popt->sp3Dir, "", 1);             /* the directory of SP3 format precise ephemeris files */
    str.SetStr(popt->clkDir, "", 1);             /* the directory of RINEX format precise clock files */
    str.SetStr(popt->eopDir, "", 1);             /* the directory of earth rotation parameter files */
    str.SetStr(popt->snxDir, "", 1);             /* the directory of SINEX format IGS weekly solution files */
    str.SetStr(popt->dcbDir, "", 1);             /* the directory of CODE and/or MGEX differential code bias (DCB) files */
    str.SetStr(popt->biaDir, "", 1);             /* the directory of CNES real-time phase bias files */
    str.SetStr(popt->ionDir, "", 1);             /* the directory of CODE and/or IGS global ionosphere map (GIM) files */
    str.SetStr(popt->ztdDir, "", 1);             /* the directory of CODE and/or IGS tropospheric product files */
    str.SetStr(popt->tblDir, "", 1);             /* the directory of table files for processing */
    
    /* time settings */
    popt->ts = { 0 };                            /* start time for processing */
    popt->ndays = 1;                             /* number of consecutive days */
    
    /* FTP downloading settings */
    str.SetStr(fopt->dir3party, "", 1);          /* (optional) the directory where third-party softwares (i.e., 'wget', 'gzip', 'crx2rnx' etc) are stored. This option is not needed if you have set the path or environment variable for them */
    fopt->isPath3party = false;                  /* if true: the path need be set for third-party softwares */
    str.SetStr(fopt->wgetFull, "", 1);           /* if isPath3party == true, set the full path where 'wget' is */
    str.SetStr(fopt->gzipFull, "", 1);           /* if isPath3party == true, set the full path where 'gzip' is */
    str.SetStr(fopt->crx2rnxFull, "", 1);        /* if isPath3party == true, set the full path where 'crx2rnx' is */
    fopt->minusAdd1day = true;                   /* (0:off  1:on) the day before and after the current day for precise satellite orbit and clock products downloading */
    fopt->printInfoWget = false;                 /* (0:off  1:on) print the information generated by 'wget' */
    str.SetStr(fopt->qr, "", 1);                 /* '-qr' (printInfoWget == false) or '-r' (printInfoWget == true) in 'wget' command line */

    /* initialization for FTP options */
    fopt->ftpDownloading = false;                /* the master switch for data downloading, 0:off  1:on, only for data downloading */
    str.SetStr(fopt->ftpFrom, "", 1);            /* FTP archive: CDDIS, IGN, or WHU */
    fopt->getObs = false;                        /* (0:off  1:on) IGS observation (RINEX version 2.xx, short name 'd') */
    fopt->getObm = false;                        /* (0:off  1:on) MGEX observation (RINEX version 3.xx, long name 'crx') */
    fopt->getObc = false;                        /* (0:off  1:on) Curtin University of Technology (CUT) observation (RINEX version 3.xx, long name 'crx') */
    fopt->getObg = false;                        /* (0:off  1:on) Geoscience Australia (GA) observation (RINEX version 3.xx, long name 'crx') */
    fopt->getObh = false;                        /* (0:off  1:on) Hong Kong CORS observation (RINEX version 3.xx, long name 'crx') */
    fopt->getObn = false;                        /* (0:off  1:on) NGS/NOAA CORS observation (RINEX version 2.xx, short name 'd') */
    fopt->getObe = false;                        /* (0:off  1:on) EUREF Permanent Network (EPN) observation (RINEX version 3.xx, long name 'crx' and RINEX version 2.xx, short name 'd') */
    str.SetStr(fopt->obsTyp, "", 1);             /* 'daily', 'hourly', or 'highrate' */
    str.SetStr(fopt->obmTyp, "", 1);             /* 'daily', 'hourly', or 'highrate' */
    str.SetStr(fopt->obcTyp, "", 1);             /* 'daily' */
    str.SetStr(fopt->obgTyp, "", 1);             /* 'daily', 'hourly', or 'highrate' */
    str.SetStr(fopt->obhTyp, "", 1);             /* '30s', '5s', or '1s' */
    str.SetStr(fopt->obnTyp, "", 1);             /* 'daily' */
    str.SetStr(fopt->obeTyp, "", 1);             /* 'daily' */
    str.SetStr(fopt->obsOpt, "", 1);             /* all; the full path of 'site.list' */
    str.SetStr(fopt->obmOpt, "", 1);             /* all; the full path of 'site.list' */
    str.SetStr(fopt->obcOpt, "", 1);             /* the full path of 'site.list' */
    str.SetStr(fopt->obgOpt, "", 1);             /* all; the full path of 'site.list' */
    str.SetStr(fopt->obhOpt, "", 1);             /* the full path of 'site.list' */
    str.SetStr(fopt->obnOpt, "", 1);             /* the full path of 'site.list' */
    str.SetStr(fopt->obeOpt, "", 1);             /* all; the full path of 'site.list' */
    fopt->getNav = false;                        /* (0:off  1:on) broadcast ephemeris */
    str.SetStr(fopt->navTyp, "", 1);             /* 'daily', 'hourly', or 'RTNAV' */
    str.SetStr(fopt->navOpt, "", 1);             /* 'gps', 'glo', 'bds', 'gal', 'qzs', 'irn', 'mixed' or 'all' */
    fopt->getOrbClk = false;                     /* (0:off  1:on) precise orbit and clock */
    str.SetStr(fopt->orbClkOpt, "", 1);          /* analysis center (i.e., final: igs, cod, esa, gfz, grg, jpl, com, gbm, grm, wum, ...; rapid: igr; ultra-rapid: igu, gfu, wuu) */
    fopt->getEop = false;                        /* (0:off  1:on) earth rotation parameter */
    str.SetStr(fopt->eopOpt, "", 1);             /* analysis center (i.e., final: igs, cod, esa, gfz, grg, jpl, ...; ultra: igu, gfu) */
    fopt->getSnx = false;                        /* (0:off  1:on) IGS weekly SINEX */
    fopt->getDcb = false;                        /* (0:off  1:on) CODE and/or MGEX differential code bias (DCB) */
    fopt->getIon = false;                        /* (0:off  1:on) global ionosphere map (GIM) */
    str.SetStr(fopt->ionOpt, "", 1);             /* analysis center (i.e., igs, cod, cas, ...) */
    fopt->getRoti = false;                       /* (0:off  1:on) rate of TEC index (ROTI) */
    fopt->getTrp = false;                        /* (0:off  1:on) CODE and/or IGS tropospheric product */
    str.SetStr(fopt->trpOpt, "", 1);             /* analysis center (i.e., igs, cod, ...). If 'igs' is set, it will choose the 3rd of 'getObs' for site-specific tropospheric product */
    fopt->getRtOrbClk = false;                   /* (0:off  1:on) real-time precise orbit and clock products from CNES offline files */
    fopt->getRtBias = false;                     /* (0:off  1:on) real-time precise code and phase products from CNES offline files */
    fopt->getAtx = false;                        /* (0:off  1:on) ANTEX format antenna phase center correction */
} /* end of init */

/**
* @brief   : ReadCfgFile - read configure file to get processing options
* @param[I]: cfgFile (configure file)
* @param[O]: popt (processing options)
* @param[O]: fopt (FTP options, nullptr:NO output)
* @return  : true:ok, false:error
* @note    :
**/
bool PreProcess::ReadCfgFile(const char *cfgFile, prcopt_t *popt, ftpopt_t *fopt)
{
    /* open configure file */
    FILE *fp = nullptr;
    if (!(fp = fopen(cfgFile, "r")))
    {
        cerr << "*** ERROR(PreProcess::ReadCfgFile): open configure file " << cfgFile << " FAILED!" << endl;

        return false;
    }

    StringUtil str;
    TimeUtil tu;
    char *p, sline[MAXCHARS] = { '\0' }, tmpLine[MAXCHARS] = { '\0' };
    bool debug = false;
    int j = 0;
    gtime_t tt = { 0 };
    if (debug) cout << "##################### Start of configure file #########################" << endl;
    while (fgets(sline, MAXCHARS, fp))
    {
        if ((sline != nullptr) && (sline[0] == '#')) continue;

        p = strrchr(sline, '=');

        /* processing directory */
        if (strstr(sline, "mainDir"))                 /* the root/main directory of GNSS observations and products */
        {
            sscanf(p + 1, "%[^%]", tmpLine);          /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            strcpy(popt->mainDir, tmpLine);
            if (debug) cout << "* mainDir = " << popt->mainDir << endl;
        }
        else if (strstr(sline, "obsDir"))             /* the directory of IGS RINEX format observation files (short name "d" files) */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'obsDir         = 0  obs', indicating 'obsDir         = D:\data\obs' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->obsDir, tmpDir);
            }
            else strcpy(popt->obsDir, tmpLine);  /* the full path should be given, i.e., 'obsDir         = 0  D:\data\obs' */
            if (debug) cout << "* obsDir = " << popt->obsDir << endl;
        }
        else if (strstr(sline, "obmDir"))             /* the directory of MGEX RINEX format observation files (long name "crx" files) */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'obmDir         = 0  obs', indicating 'obmDir         = D:\data\obm' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->obmDir, tmpDir);
            }
            else strcpy(popt->obmDir, tmpLine);  /* the full path should be given, i.e., 'obmDir         = 0  D:\data\obm' */
            if (debug) cout << "* obmDir = " << popt->obmDir << endl;
        }
        else if (strstr(sline, "obcDir"))             /* the directory of Curtin University of Technology (CUT) RINEX format observation files (long name "crx" files) */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'obcDir         = 0  obc', indicating 'obcDir         = D:\data\obc' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->obcDir, tmpDir);
            }
            else strcpy(popt->obcDir, tmpLine);  /* the full path should be given, i.e., 'obcDir         = 0  D:\data\obc' */
            if (debug) cout << "* obcDir = " << popt->obcDir << endl;
        }
        else if (strstr(sline, "obgDir"))             /* the directory of Geoscience Australia (GA) RINEX format observation files (long name "crx" files) */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'obgDir         = 0  obg', indicating 'obgDir         = D:\data\obg' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->obgDir, tmpDir);
            }
            else strcpy(popt->obgDir, tmpLine);  /* the full path should be given, i.e., 'obgDir         = 0  D:\data\obg' */
            if (debug) cout << "* obgDir = " << popt->obgDir << endl;
        }
        else if (strstr(sline, "obhDir"))             /* the directory of Hong Kong CORS RINEX format observation files (long name "crx" files) */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'obhDir         = 0  obh', indicating 'obhDir         = D:\data\obh' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->obhDir, tmpDir);
            }
            else strcpy(popt->obhDir, tmpLine);  /* the full path should be given, i.e., 'obhDir         = 0  D:\data\obh' */
            if (debug) cout << "* obhDir = " << popt->obhDir << endl;
        }
        else if (strstr(sline, "obnDir"))             /* the directory of NGS/NOAA CORS RINEX format observation files (short name "d" files) */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'obnDir         = 0  obn', indicating 'obnDir         = D:\data\obn' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->obnDir, tmpDir);
            }
            else strcpy(popt->obnDir, tmpLine);  /* the full path should be given, i.e., 'obnDir         = 0  D:\data\obn' */
            if (debug) cout << "* obnDir = " << popt->obnDir << endl;
        }
        else if (strstr(sline, "obeDir"))             /* the directory of EUREF Permanent Network (EPN) RINEX format observation files (long name "crx" files and short name "d" files) */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'obeDir         = 0  obe', indicating 'obeDir         = D:\data\obe' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->obeDir, tmpDir);
            }
            else strcpy(popt->obeDir, tmpLine);  /* the full path should be given, i.e., 'obeDir         = 0  D:\data\obe' */
            if (debug) cout << "* obeDir = " << popt->obeDir << endl;
        }
        else if (strstr(sline, "navDir"))             /* the directory of RINEX format broadcast ephemeris files */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'navDir         = 0  nav', indicating 'navDir         = D:\data\nav' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->navDir, tmpDir);
            }
            else strcpy(popt->navDir, tmpLine);  /* the full path should be given, i.e., 'navDir         = 0  D:\data\nav' */
            if (debug) cout << "* navDir = " << popt->navDir << endl;
        }
        else if (strstr(sline, "sp3Dir"))             /* the directory of SP3 format precise ephemeris files */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'sp3Dir         = 0  sp3', indicating 'sp3Dir         = D:\data\sp3' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->sp3Dir, tmpDir);
            }
            else strcpy(popt->sp3Dir, tmpLine);  /* the full path should be given, i.e., 'sp3Dir         = 0  D:\data\sp3' */
            if (debug) cout << "* sp3Dir = " << popt->sp3Dir << endl;
        }
        else if (strstr(sline, "clkDir"))             /* the directory of RINEX format precise clock files */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'clkDir         = 0  clk', indicating 'clkDir         = D:\data\clk' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->clkDir, tmpDir);
            }
            else strcpy(popt->clkDir, tmpLine);  /* the full path should be given, i.e., 'clkDir         = 0  D:\data\clk' */
            if (debug) cout << "* clkDir = " << popt->clkDir << endl;
        }
        else if (strstr(sline, "eopDir"))             /* the directory of earth rotation parameter files */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'eopDir         = 0  eop', indicating 'eopDir         = D:\data\eop' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->eopDir, tmpDir);
            }
            else strcpy(popt->eopDir, tmpLine);  /* the full path should be given, i.e., 'eopDir         = 0  D:\data\eop' */
            if (debug) cout << "* eopDir = " << popt->eopDir << endl;
        }
        else if (strstr(sline, "snxDir"))             /* the directory of SINEX format IGS weekly solution files */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'snxDir         = 0  snx', indicating 'snxDir         = D:\data\snx' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->snxDir, tmpDir);
            }
            else strcpy(popt->snxDir, tmpLine);  /* the full path should be given, i.e., 'snxDir         = 0  D:\data\snx' */
            if (debug) cout << "* snxDir = " << popt->snxDir << endl;
        }
        else if (strstr(sline, "dcbDir"))             /* the directory of CODE and/or MGEX differential code bias (DCB) files */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'dcbDir         = 0  dcb', indicating 'dcbDir         = D:\data\dcb' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->dcbDir, tmpDir);
            }
            else strcpy(popt->dcbDir, tmpLine);  /* the full path should be given, i.e., 'dcbDir         = 0  D:\data\dcb' */
            if (debug) cout << "* dcbDir = " << popt->dcbDir << endl;
        }
        else if (strstr(sline, "biaDir"))             /* the directory of CNES real-time phase bias files */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'biaDir         = 0  bia', indicating 'biaDir         = D:\data\bia' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->biaDir, tmpDir);
            }
            else strcpy(popt->biaDir, tmpLine);  /* the full path should be given, i.e., 'biaDir         = 0  D:\data\bia' */
            if (debug) cout << "* biaDir = " << popt->biaDir << endl;
        }
        else if (strstr(sline, "ionDir"))             /* the directory of CODE and/or IGS global ionosphere map (GIM) files */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'ionDir         = 0  ion', indicating 'ionDir         = D:\data\ion' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->ionDir, tmpDir);
            }
            else strcpy(popt->ionDir, tmpLine);  /* the full path should be given, i.e., 'ionDir         = 0  D:\data\ion' */
            if (debug) cout << "* ionDir = " << popt->ionDir << endl;
        }
        else if (strstr(sline, "ztdDir"))             /* the directory of CODE and/or IGS tropospheric product files */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'ztdDir         = 0  ztd', indicating 'ztdDir         = D:\data\ztd' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->ztdDir, tmpDir);
            }
            else strcpy(popt->ztdDir, tmpLine);  /* the full path should be given, i.e., 'ztdDir         = 0  D:\data\ztd' */
            if (debug) cout << "* ztdDir = " << popt->ztdDir << endl;
        }
        else if (strstr(sline, "tblDir"))             /* the directory of table files for processing */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            if (j == 0)  /* If '0' is set, the full path will NOT be needed, i.e., 'tblDir         = 0  tbl', indicating 'tblDir         = D:\data\tbl' */
            {
                char tmpDir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(tmpDir, "%s%c%s", popt->mainDir, sep, tmpLine);
                strcpy(popt->tblDir, tmpDir);
            }
            else strcpy(popt->tblDir, tmpLine);  /* the full path should be given, i.e., 'tblDir         = 0  D:\data\tbl' */
            if (debug) cout << "* tblDir = " << popt->tblDir << endl;
        }
        else if (strstr(sline, "3partyDir"))          /* (optional) the directory where third-party softwares (i.e., 'wget', 'gzip', 'crx2rnx' etc) are stored. This option is not needed if you have set the path or environment variable for them */
        {
            sscanf(p + 1, "%d %[^%]", &j, &tmpLine);  /* %[^%] denotes regular expression, which means that using % as the end sign of the input string */
            str.TrimSpace(tmpLine);
            str.CutFilePathSep(tmpLine);
            strcpy(fopt->dir3party, tmpLine);
            fopt->isPath3party = j == 1 ? true : false;
            if (debug) cout << "* 3partyDir = " << fopt->dir3party << "  " << fopt->isPath3party << endl;

            string tmpDir = fopt->dir3party;
            if (access(tmpDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + tmpDir;
#endif
                std::system(cmd.c_str());
            }
        }

        /* time settings */
        else if (strstr(sline, "procTime"))           /* start time for processing */
        {
            sscanf(p + 1, "%d", &j);
            if (j == 1)
            {
                double date[6] = { 0.0 };
                if (sscanf(p + 1, "%d %lf %lf %lf %d", &j, date + 0, date + 1, date + 2,
                    &popt->ndays) < 5)
                {
                    cerr << "*** ERROR(PreProcess::ReadCfgFile): the parameter of number of consecutive days is MISSING, please check it!" << endl;
                    
                    return false;
                }
                popt->ts = tu.ymdhms2time(date);
            }
            else if (j == 2)
            {
                double year = 0.0, doy = 0.0;
                if (sscanf(p + 1, "%d %lf %lf %d", &j, &year, &doy, &popt->ndays) < 4)
                {
                    cerr << "*** ERROR(PreProcess::ReadCfgFile): the parameter of number of consecutive days is MISSING, please check it!" << endl;

                    return false;
                };
                int iYear = (int)year, iDoy = (int)doy;
                popt->ts = tu.yrdoy2time(iYear, iDoy);
            }

            if (debug) cout << "* procTime = " << popt->ts.mjd << "  " << popt->ts.sod << endl;
        }

        /* FTP downloading settings */
        else if (strstr(sline, "minusAdd1day"))       /* (0:off  1:on) the day before and after the current day for precise satellite orbit and clock products downloading */
        {
            sscanf(p + 1, "%d", &j);
            fopt->minusAdd1day = j == 1 ? true : false;
            if (debug) cout << "* minusAdd1day = " << fopt->minusAdd1day << endl;
        }
        else if (strstr(sline, "printInfoWget"))      /* (0:off  1:on) print the information generated by 'wget' */
        {
            sscanf(p + 1, "%d", &j);
            fopt->printInfoWget = j == 1 ? true : false;
            if (debug) cout << "* printInfoWget = " << fopt->printInfoWget << endl;
        }

        /* handling of FTP downloading */
        else if (strstr(sline, "ftpDownloading"))     /* the master switch for data downloading (0:off  1:on, only for data downloading); the FTP archive, i.e., CDDIS, IGN */
        {
            sscanf(p + 1, "%d %s", &j, &fopt->ftpFrom);
            fopt->ftpDownloading = j == 1 ? true : false;
            if (debug) cout << "* ftpDownloading = " << fopt->ftpDownloading << "  " << fopt->ftpFrom << endl;

            if (fopt->ftpDownloading)
            {
                int hh = 0, nh = 0, step = 1;
                while (fgets(sline, MAXCHARS, fp))
                {
                    if ((sline != nullptr) && (sline[0] == '#')) continue;
                    p = strrchr(sline, '=');

                    if (strstr(sline, "getObs"))      /* (0:off  1:on) IGS observation (RINEX version 2.xx, short name 'd') */
                    {
                        sscanf(p + 1, "%d %s %s %d %d", &j, &fopt->obsTyp, &fopt->obsOpt, &hh, &nh);
                        fopt->getObs = j == 1 ? true : false;
                        if (debug) cout << "* getObs = " << fopt->getObs << "  " << fopt->obsTyp << "  " << fopt->obsOpt << endl;
                        
                        int imax = MIN(hh + nh, 24);
                        for (int i = hh; i < imax; i++) fopt->hhObs.push_back(i);
                    }
                    else if (strstr(sline, "getObm")) /* (0:off  1:on) MGEX observation (RINEX version 3.xx, long name 'crx') */
                    {
                        sscanf(p + 1, "%d %s %s %d %d", &j, &fopt->obmTyp, &fopt->obmOpt, &hh, &nh);
                        fopt->getObm = j == 1 ? true : false;
                        if (debug) cout << "* getObm = " << fopt->getObm << "  " << fopt->obmTyp << "  " << fopt->obmOpt << endl;

                        int imax = MIN(hh + nh, 24);
                        for (int i = hh; i < imax; i++) fopt->hhObm.push_back(i);
                    }
                    else if (strstr(sline, "getObc")) /* (0:off  1:on) Curtin University of Technology (CUT) observation (RINEX version 3.xx, long name 'crx') */
                    {
                        sscanf(p + 1, "%d %s %s %d %d", &j, &fopt->obcTyp, &fopt->obcOpt, &hh, &nh);
                        fopt->getObc = j == 1 ? true : false;
                        if (debug) cout << "* getObc = " << fopt->getObc << "  " << fopt->obcTyp << "  " << fopt->obcOpt << endl;
                    }
                    else if (strstr(sline, "getObg")) /* (0:off  1:on) Geoscience Australia (GA) observation (RINEX version 3.xx, long name 'crx') */
                    {
                        sscanf(p + 1, "%d %s %s %d %d", &j, &fopt->obgTyp, &fopt->obgOpt, &hh, &nh);
                        fopt->getObg = j == 1 ? true : false;
                        if (debug) cout << "* getObg = " << fopt->getObg << "  " << fopt->obgTyp << "  " << fopt->obgOpt << endl;

                        int imax = MIN(hh + nh, 24);
                        for (int i = hh; i < imax; i++) fopt->hhObg.push_back(i);
                    }
                    else if (strstr(sline, "getObh")) /* (0:off  1:on) Hong Kong CORS observation (RINEX version 3.xx, long name 'crx') */
                    {
                        sscanf(p + 1, "%d %s %s %d %d", &j, &fopt->obhTyp, &fopt->obhOpt, &hh, &nh);
                        fopt->getObh = j == 1 ? true : false;
                        if (debug) cout << "* getObh = " << fopt->getObh << "  " << fopt->obhTyp << "  " << fopt->obhOpt << endl;

                        int imax = MIN(hh + nh, 24);
                        for (int i = hh; i < imax; i++) fopt->hhObh.push_back(i);
                    }
                    else if (strstr(sline, "getObn")) /* (0:off  1:on) NGS/NOAA CORS observation (RINEX version 2.xx, short name 'd') */
                    {
                        sscanf(p + 1, "%d %s %s %d %d", &j, &fopt->obnTyp, &fopt->obnOpt, &hh, &nh);
                        fopt->getObn = j == 1 ? true : false;
                        if (debug) cout << "* getObn = " << fopt->getObn << "  " << fopt->obnTyp << "  " << fopt->obnOpt << endl;
                    }
                    else if (strstr(sline, "getObe")) /* (0:off  1:on) EUREF Permanent Network (EPN) observation (RINEX version 3.xx, long name 'crx' and RINEX version 2.xx, short name 'd') */
                    {
                        sscanf(p + 1, "%d %s %s %d %d", &j, &fopt->obeTyp, &fopt->obeOpt, &hh, &nh);
                        fopt->getObe = j == 1 ? true : false;
                        if (debug) cout << "* getObe = " << fopt->getObe << "  " << fopt->obeTyp << "  " << fopt->obeOpt << endl;
                    }
                    else if (strstr(sline, "getNav")) /* (0:off  1:on) broadcast ephemeris */
                    {
                        sscanf(p + 1, "%d %s %s %d %d", &j, &fopt->navTyp, &fopt->navOpt, &hh, &nh);
                        fopt->getNav = j == 1 ? true : false;
                        if (debug) cout << "* getNav = " << fopt->getNav << "  " << fopt->navTyp << "  " << fopt->navOpt << endl;

                        int imax = MIN(hh + nh, 24);
                        for (int i = hh; i < imax; i += step) fopt->hhNav.push_back(i);
                    }
                    else if (strstr(sline, "getOrbClk"))   /* (0:off  1:on) precise orbit and clock */
                    {
                        sscanf(p + 1, "%d %s %d %d", &j, &fopt->orbClkOpt, &hh, &nh);
                        fopt->getOrbClk = j == 1 ? true : false;
                        if (debug) cout << "* getOrbClk = " << fopt->getOrbClk << "  " << fopt->orbClkOpt << endl;

                        string ocOpt = fopt->orbClkOpt;
                        if (ocOpt == "igu" || ocOpt == "esu") step = 6;
                        else if (ocOpt == "gfu") step = 3;
                        else if (ocOpt == "wuu") step = 1;
                        else step = 24;
                        int imax = 24;
                        for (int i = 0; i < imax; i += step)
                        {
                            if (hh > i) hh = i + step;
                            else break;
                        }
                        imax = MIN(hh + nh * step, 24);
                        for (int i = hh; i < imax; i += step) fopt->hhOrbClk.push_back(i);
                    }
                    else if (strstr(sline, "getEop")) /* (0:off  1:on) earth rotation parameter */
                    {
                        sscanf(p + 1, "%d %s %d %d", &j, &fopt->eopOpt, &hh, &nh);
                        fopt->getEop = j == 1 ? true : false;
                        if (debug) cout << "* getEop = " << fopt->getEop << "  " << fopt->eopOpt << endl;

                        string eOpt = fopt->eopOpt;
                        if (eOpt == "igu" || eOpt == "esu") step = 6;
                        else if (eOpt == "gfu") step = 3;
                        else step = 24;
                        int imax = 24;
                        for (int i = 0; i < imax; i += step)
                        {
                            if (hh > i) hh = i + step;
                            else break;
                        }
                        imax = MIN(hh + nh * step, 24);
                        for (int i = hh; i < imax; i += step) fopt->hhEop.push_back(i);
                    }
                    else if (strstr(sline, "getSnx")) /* (0:off  1:on) IGS weekly SINEX */
                    {
                        sscanf(p + 1, "%d", &j);
                        fopt->getSnx = j == 1 ? true : false;
                        if (debug) cout << "* getSnx = " << fopt->getSnx << endl;
                    }
                    else if (strstr(sline, "getDcb")) /* (0:off  1:on) CODE and/or MGEX differential code bias (DCB) */
                    {
                        sscanf(p + 1, "%d", &j);
                        fopt->getDcb = j == 1 ? true : false;
                        if (debug) cout << "* getDcb = " << fopt->getDcb << endl;
                    }
                    else if (strstr(sline, "getIon")) /* (0:off  1:on) CODE and/or IGS global ionosphere map (GIM) */
                    {
                        sscanf(p + 1, "%d %s", &j, &fopt->ionOpt);
                        fopt->getIon = j == 1 ? true : false;
                        if (debug) cout << "* getIon = " << fopt->getIon << "  " << fopt->ionOpt << endl;
                    }
                    else if (strstr(sline, "getRoti"))/* (0:off  1:on) Rate of TEC index (ROTI) */
                    {
                        sscanf(p + 1, "%d", &j);
                        fopt->getRoti = j == 1 ? true : false;
                        if (debug) cout << "* getRoti = " << fopt->getRoti << endl;
                    }
                    else if (strstr(sline, "getTrp")) /* (0:off  1:on) CODE and/or IGS tropospheric product */
                    {
                        sscanf(p + 1, "%d %s", &j, &fopt->trpOpt);
                        fopt->getTrp = j == 1 ? true : false;
                        if (debug) cout << "* getTrp = " << fopt->getTrp << "  " << fopt->trpOpt << endl;
                    }
                    else if (strstr(sline, "getRtOrbClk")) /* (0:off  1:on) real-time precise orbit and clock products from CNES offline files */
                    {
                        sscanf(p + 1, "%d", &j);
                        fopt->getRtOrbClk = j == 1 ? true : false;
                        if (debug) cout << "* getRtOrbClk = " << fopt->getRtOrbClk << endl;
                    }
                    else if (strstr(sline, "getRtBias"))   /* (0:off  1:on) real-time code and phase bias products from CNES offline files */
                    {
                        sscanf(p + 1, "%d", &j);
                        fopt->getRtBias = j == 1 ? true : false;
                        if (debug) cout << "* getRtBias = " << fopt->getRtBias << endl;
                    }
                    else if (strstr(sline, "getAtx")) /* (0:off  1:on) ANTEX format antenna phase center correction */
                    {
                        sscanf(p + 1, "%d", &j);
                        fopt->getAtx = j == 1 ? true : false;
                        if (debug) cout << "* getAtx = " << fopt->getAtx << endl;
                        break;
                    }
                }
            }
        }
    }
    fclose(fp);

    if (debug) cout << "##################### End of configure file ###########################" << endl;

    return true;
} /* end of ReadCfgFile */

/**
* @brief   : run - start iPPP-RTK processing
* @param[I]: cfgFile (configure file with full path)
* @param[O]: none
* @return  : none
* @note    :
**/
void PreProcess::run(const char *cfgFile)
{
    prcopt_t popt;
    ftpopt_t fopt;
    /* initialization */
    init(&popt, &fopt);

    /* read configure file to get processing information */
    ReadCfgFile(cfgFile, &popt, &fopt);

    /* data downloading for GNSS further processing */
    if (fopt.ftpDownloading)
    {
        FtpUtil ftp;
        TimeUtil tu;
        StringUtil str;
        string obsDirMain = popt.obsDir;
        string obmDirMain = popt.obmDir;
        string obcDirMain = popt.obcDir;
        string obgDirMain = popt.obgDir;
        string obhDirMain = popt.obhDir;
        string obnDirMain = popt.obnDir;
        string obeDirMain = popt.obeDir;
        string navDirMain = popt.navDir;
        string ztdDirMain = popt.ztdDir;
        for (int i = 0; i < popt.ndays; i++)
        {
            int yyyy, doy;
            tu.time2yrdoy(popt.ts, &yyyy, &doy);
            string sYyyy = str.yyyy2str(yyyy);
            string sDoy = str.doy2str(doy);

            /* creat new IGS observation sub-directory */
            if (fopt.getObs)
            {
                char dir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(dir, "%s%c%s%c%s", obsDirMain.c_str(), sep, sYyyy.c_str(), sep, sDoy.c_str());
                str.TrimSpace(dir);
                str.CutFilePathSep(dir);
                strcpy(popt.obsDir, dir);
                string tmpDir = dir;
                if (access(tmpDir.c_str(), 0) == -1)
                {
                    /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                    string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
                    string cmd = "mkdir -p " + tmpDir;
#endif
                    std::system(cmd.c_str());
                }
            }

            /* creat new MGEX observation sub-directory */
            if (fopt.getObm)
            {
                char dir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(dir, "%s%c%s%c%s", obmDirMain.c_str(), sep, sYyyy.c_str(), sep, sDoy.c_str());
                str.TrimSpace(dir);
                str.CutFilePathSep(dir);
                strcpy(popt.obmDir, dir);
                string tmpDir = dir;
                if (access(tmpDir.c_str(), 0) == -1)
                {
                    /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                    string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
                    string cmd = "mkdir -p " + tmpDir;
#endif
                    std::system(cmd.c_str());
                }
            }

            /* creat new Curtin University of Technology (CUT) observation sub-directory */
            if (fopt.getObc)
            {
                char dir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(dir, "%s%c%s%c%s", obcDirMain.c_str(), sep, sYyyy.c_str(), sep, sDoy.c_str());
                str.TrimSpace(dir);
                str.CutFilePathSep(dir);
                strcpy(popt.obcDir, dir);
                string tmpDir = dir;
                if (access(tmpDir.c_str(), 0) == -1)
                {
                    /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                    string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
                    string cmd = "mkdir -p " + tmpDir;
#endif
                    std::system(cmd.c_str());
                }
            }

            /* creat new Geoscience Australia (GA) observation sub-directory */
            if (fopt.getObg)
            {
                char dir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(dir, "%s%c%s%c%s", obgDirMain.c_str(), sep, sYyyy.c_str(), sep, sDoy.c_str());
                str.TrimSpace(dir);
                str.CutFilePathSep(dir);
                strcpy(popt.obgDir, dir);
                string tmpDir = dir;
                if (access(tmpDir.c_str(), 0) == -1)
                {
                    /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                    string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
                    string cmd = "mkdir -p " + tmpDir;
#endif
                    std::system(cmd.c_str());
                }
            }

            /* creat new Hong Kong CORS observation sub-directory */
            if (fopt.getObh)
            {
                char dir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(dir, "%s%c%s%c%s", obhDirMain.c_str(), sep, sYyyy.c_str(), sep, sDoy.c_str());
                str.TrimSpace(dir);
                str.CutFilePathSep(dir);
                strcpy(popt.obhDir, dir);
                string tmpDir = dir;
                if (access(tmpDir.c_str(), 0) == -1)
                {
                    /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                    string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
                    string cmd = "mkdir -p " + tmpDir;
#endif
                    std::system(cmd.c_str());
                }
            }

            /* creat new NGS/NOAA CORS observation sub-directory */
            if (fopt.getObn)
            {
                char dir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(dir, "%s%c%s%c%s", obnDirMain.c_str(), sep, sYyyy.c_str(), sep, sDoy.c_str());
                str.TrimSpace(dir);
                str.CutFilePathSep(dir);
                strcpy(popt.obnDir, dir);
                string tmpDir = dir;
                if (access(tmpDir.c_str(), 0) == -1)
                {
                    /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                    string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
                    string cmd = "mkdir -p " + tmpDir;
#endif
                    std::system(cmd.c_str());
                }
            }

            /* creat new EUREF Permanent Network (EPN) observation sub-directory */
            if (fopt.getObe)
            {
                char dir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(dir, "%s%c%s%c%s", obeDirMain.c_str(), sep, sYyyy.c_str(), sep, sDoy.c_str());
                str.TrimSpace(dir);
                str.CutFilePathSep(dir);
                strcpy(popt.obeDir, dir);
                string tmpDir = dir;
                if (access(tmpDir.c_str(), 0) == -1)
                {
                    /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                    string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
                    string cmd = "mkdir -p " + tmpDir;
#endif
                    std::system(cmd.c_str());
                }
            }

            /* creat new NAV sub-directory */
            if (fopt.getNav)
            {
                char dir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(dir, "%s%c%s%c%s", navDirMain.c_str(), sep, sYyyy.c_str(), sep, sDoy.c_str());
                str.TrimSpace(dir);
                str.CutFilePathSep(dir);
                strcpy(popt.navDir, dir);
                string tmpDir = dir;
                if (access(tmpDir.c_str(), 0) == -1)
                {
                    /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                    string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
                    string cmd = "mkdir -p " + tmpDir;
#endif
                    std::system(cmd.c_str());
                }
            }

            /* creat new ZTD sub-directory */
            if (fopt.getTrp)
            {
                char dir[MAXSTRPATH] = { '\0' };
                char sep = (char)FILEPATHSEP;
                sprintf(dir, "%s%c%s%c%s", ztdDirMain.c_str(), sep, sYyyy.c_str(), sep, sDoy.c_str());
                str.TrimSpace(dir);
                str.CutFilePathSep(dir);
                strcpy(popt.ztdDir, dir);
                string tmpDir = dir;
                if (access(tmpDir.c_str(), 0) == -1)
                {
                    /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                    string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
                    string cmd = "mkdir -p " + tmpDir;
#endif
                    std::system(cmd.c_str());
                }
            }

            /* the main entry of FTP downloader */
            ftp.FtpDownload(&popt, &fopt);

            popt.ts = tu.TimeAdd(popt.ts, 86400.0);
        }
    }
} /* end of run */