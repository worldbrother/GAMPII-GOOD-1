/*------------------------------------------------------------------------------
* Created by Feng Zhou on 2020-09-25
*
* main entry of run_GOOD.exe
*-----------------------------------------------------------------------------*/
#include "Good.h"
#include "PreProcess.h"
#include "StringUtil.h"


/* ppp_run main --------------------------------------------------------------*/
int main(int argc, char * argv[])
{
#ifdef _WIN32  /* for Windows */
    /* char cfgFile[1000] = "D:\\data\\gamp_good.cfg"; */
    char *cfgFile;
    if (argc == 1)
    {
        cout << endl;
        cout << "*** WARNING: The input command-line parameter indicating configure file is lost, please check it!" << endl;
        cout << endl << endl;
        cout << "------------------------------ Information of GAMP II - GOOD ------------------------------" << endl;
        cout << "*** INFO: The current version of GAMP II - GOOD is 1.8" << endl;
        cout << "*** INFO: GOOD is short for Gnss Observations and prOducts Downloader" << endl;
        cout << "*** INFO: GOOD is developed by SpAtial SurveyIng and Navigation (SASIN) group "
            "at Shandong University of Science and Technology (SDUST)" << endl;
        cout << "*** INFO: Copyright (C) 2020-2099 by SASIN, all rights reserved" << endl;

        return 0;
    }
    else cfgFile = argv[1];
#else          /* for Linux or Mac */
    char *cfgFile;
    if (argc == 1)
    {
        cout << endl;
        cout << "*** WARNING: The input command-line parameter indicating configure file is lost, please check it!" << endl;
        cout << endl << endl;
        cout << "------------------------------ Information of GAMP II - GOOD ------------------------------" << endl;
        cout << "*** INFO: The current version of GAMP II - GOOD is 1.8" << endl;
        cout << "*** INFO: GOOD is short for Gnss Observations and prOducts Downloader" << endl;
        cout << "*** INFO: GOOD is developed by SpAtial SurveyIng and Navigation (SASIN) group "
            "at Shandong University of Science and Technology (SDUST)" << endl;
        cout << "*** INFO: Copyright (C) 2020-2099 by SASIN, all rights reserved" << endl;

        return 0;
    }
    else cfgFile = argv[1];
#endif

#if (defined(_WIN32) && !defined(MinGW))  /* for Windows */
    _setmaxstdio(8192);   /* to open many files */
#endif

    PreProcess preProc;
    preProc.run(cfgFile);

#ifdef _WIN32  /* for Windows */
    /*cout << "Press any key to exit!" << endl;
    getchar();*/
#endif

    return 0;
}