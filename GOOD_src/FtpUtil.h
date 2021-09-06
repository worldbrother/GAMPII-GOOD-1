/*------------------------------------------------------------------------------
* FtpUtil.h : header file of FtpUtil.cpp
*-----------------------------------------------------------------------------*/
#pragma once

class FtpUtil
{
private:

    struct ftpArchive_t
    {
        std::vector<string> CDDIS;  /* CDDIS FTP */
        std::vector<string> IGN;    /* IGN FTP */
        std::vector<string> WHU;    /* WHU FTP */
    };
    ftpArchive_t _ftpArchive;

private:

    /**
    * @brief   : init - Get FTP archive for CDDIS, IGN, or WHU
    * @param[I]: none
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void init();

    /**
    * @brief   : GetDailyObsIgs - download IGS RINEX daily observation (30s) files (short name "d") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetDailyObsIgs(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetHourlyObsIgs - download IGS RINEX hourly observation (30s) files (short name "d") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetHourlyObsIgs(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetHrObsIgs - download IGS RINEX high-rate observation (1s) files (short name "d") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetHrObsIgs(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetDailyObsMgex - download MGEX RINEX daily observation (30s) files (long name "crx") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetDailyObsMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetHourlyObsMgex - download MGEX RINEX hourly observation (30s) files (long name "crx") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetHourlyObsMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetHrObsMgex - download MGEX RINEX high-rate observation (1s) files (long name "crx") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetHrObsMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetDailyObsCut - download Curtin University of Technology (CUT) RINEX daily observation (30s) files (long name "crx") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetDailyObsCut(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetDailyObsGa - download Geoscience Australia (GA) RINEX daily observation (30s) files (long name "crx") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetDailyObsGa(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetHourlyObsGa - download Geoscience Australia (GA) RINEX hourly observation (30s) files (long name "crx") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetHourlyObsGa(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetHrObsGa - download Geoscience Australia (GA) RINEX high-rate observation (1s) files (long name "crx") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetHrObsGa(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : Get30sObsHk - download Hong Kong CORS RINEX daily observation (30s) files (long name "crx") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void Get30sObsHk(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : Get5sObsHk - download Hong Kong CORS RINEX daily observation (5s) files (long name "crx") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void Get5sObsHk(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : Get1sObsHk - download Hong Kong CORS RINEX daily observation (1s) files (long name "crx") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void Get1sObsHk(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetDailyObsNgs - download NGS/NOAA CORS RINEX daily observation (30s) files (short name "d") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetDailyObsNgs(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetDailyObsEpn - download EUREF Permanent Network (EPN) RINEX daily observation (30s) files (long name 'crx' and short name "d") according to 'site.list' file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetDailyObsEpn(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetNav - download daily GPS, GLONASS and mixed RINEX broadcast ephemeris files
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetNav(gtime_t ts, const char dir[], const ftpopt_t *fopt);

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
    void GetOrbClk(gtime_t ts, const char dir[], int mode, const ftpopt_t *fopt);

    /**
    * @brief   : GetEop - download earth rotation parameter files
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetEop(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetSnx - download IGS weekly SINEX file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetSnx(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetDcbMgex - download MGEX daily DCB file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetDcbMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt);

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
    void GetDcbCode(gtime_t ts, const char dir[], const char dType[], const ftpopt_t *fopt);

    /**
    * @brief   : GetIono - download daily GIM file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetIono(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetRoti - download daily Rate of TEC index (ROTI) file
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetRoti(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetTrop - download daily tropospheric product file from CDDIS and/or CODE
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetTrop(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetRtOrbClkCNT - download real-time precise orbit and clock products from CNES offline files
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: mode (=1:sp3 downloaded; =2:clk downloadeded)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetRtOrbClkCNT(gtime_t ts, const char dir[], int mode, const ftpopt_t *fopt);

    /**
    * @brief   : GetRtBiasCNT - download real-time code and phase bias products from CNES offline files
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetRtBiasCNT(gtime_t ts, const char dir[], const ftpopt_t *fopt);

    /**
    * @brief   : GetAntexIGS - download ANTEX file from IGS
    * @param[I]: ts (start time)
    * @param[I]: dir (data directory)
    * @param[I]: fopt (FTP options)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void GetAntexIGS(gtime_t ts, const char dir[], const ftpopt_t *fopt);

public:
    FtpUtil()
	{

	}
	~FtpUtil()
	{

	}

    /**
    * @brief     : FtpDownload - GNSS data downloading via FTP
    * @param[I]  : popt (processing options)
    * @param[I/O]: fopt (FTP options)
    * @return    : none
    * @note      :
    **/
    void FtpDownload(const prcopt_t *popt, ftpopt_t *fopt);
};