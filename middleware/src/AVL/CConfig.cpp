/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-14 15:43:49
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-08-29 09:06:13
 * @FilePath: \panoramic_code\src\AVL\CConfig.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "CConfig.h"
#include <pthread.h>
#include <string>

#include "minIni.h"
#include "sdk_log.h"
#include <unistd.h>
#define MAX_STRING_VALUE    256
#define EDVR_CONFIG_PATH        "/oem/config/devece_config.ini"

CConfig *CConfig::m_pEdvrCfg = NULL;
CConfig::CConfig()
{
    m_strFile = EDVR_CONFIG_PATH;
}

CConfig::~CConfig()
{
    m_strFile = NULL;
}

int CConfig::LoadFile(const char *pszFileName)
{
    if(access(pszFileName, 0))
    {
        ERROR("no such file \n");
        return -1;
    }
    m_strFile = pszFileName;
    return 0;
}

int CConfig::DeleteSection(const char *pszSection)
{
    int n = ini_puts(pszSection, NULL, NULL, m_strFile);
    if(n != 1){
        return -1;
    }
    return 0;
}

int CConfig::DeleteKey(const char *pszSection, const char *pszKey)
{
    int n = ini_puts(pszSection, pszKey, NULL, m_strFile);
    if(n != 1){
        return -1;
    }
    return 0;
}

CConfig *CConfig::GetInstance()
{
    if(m_pEdvrCfg == NULL)
    {
        m_pEdvrCfg = new CConfig();
    }

    return m_pEdvrCfg;
}

int CConfig::SetValue(const char *pszSection, const char *pszKey, long nValue)
{
    if(NULL == pszSection || NULL == pszKey){
        return -1;
    }
    int n = ini_putl(pszSection, pszKey, nValue, m_strFile);
    if(n != 1){
        return -1;
    }
    return 0;
}

int CConfig::SetValue(const char *pszSection, const char *pszKey, char *nValue)
{
    if(NULL == pszSection || NULL == pszKey){
        return -1;
    }
    int n = ini_puts(pszSection, pszKey, nValue, m_strFile);
    if(n != 1){
        return -1;
    }
    return 0;
}

int CConfig::SetValue(const char *pszSection, const char *pszKey, float nValue)
{
    if(NULL == pszSection || NULL == pszKey){
        return -1;
    }
    int n = ini_putf(pszSection, pszKey, nValue, m_strFile);
    if(n != 1){
        return -1;
    }
    return 0;
}

long CConfig::GetValue(const char *pszSection, const char *pszKey, long nDefault)
{
    if(NULL == pszSection || NULL == pszKey){
        return -1;
    }
    long n = ini_getl(pszSection, pszKey, nDefault, m_strFile);
    return n;
}

char *CConfig::GetValue(const char *pszSection, const char *pszKey, char *nDefault, char *ndata, int ndataSize)
{
    if(NULL == pszSection || NULL == pszKey){
        return NULL;
    }
    //char value[MAX_STRING_VALUE];
    ini_gets(pszSection, pszKey, nDefault, ndata, ndataSize,m_strFile);

    return NULL;
}

float CConfig::GetValue(const char *pszSection, const char *pszKey, float nDefault)
{
    if(NULL == pszSection || NULL == pszKey){
        return -1;
    }
    //char value[MAX_STRING_VALUE];
    float n = ini_getf(pszSection, pszKey, nDefault, m_strFile);
    return n;
}