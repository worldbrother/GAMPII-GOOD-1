/*------------------------------------------------------------------------------
* PreProcess.h : header file of PreProcess.cpp
*-----------------------------------------------------------------------------*/
#pragma once

class PreProcess
{
private:

    /**
    * @brief   : init - some initializations before processing
    * @param[O]: popt (processing options)
    * @param[O]: fopt (FTP options, nullptr:NO output)
    * @return  : none
    * @note    :
    **/
    void init(prcopt_t *popt, ftpopt_t *fopt);

    /**
    * @brief   : ReadCfgFile - read configure file to get processing options
    * @param[I]: cfgFile (configure file)
    * @param[O]: popt (processing options)
    * @param[O]: fopt (FTP options, nullptr:NO output)
    * @return  : true:ok, false:error
    * @note    :
    **/
    bool ReadCfgFile(const char *cfgFile, prcopt_t *popt, ftpopt_t *fopt);

public:
    PreProcess()
	{

	}
	~PreProcess()
	{

	}

    /**
    * @brief   : run - start iPPP-RTK processing
    * @param[I]: cfgFile (configure file with full path)
    * @param[O]: none
    * @return  : none
    * @note    :
    **/
    void run(const char *cfgFile);
};