/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-14 15:43:20
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-14 16:14:23
 * @FilePath: \panoramic_code\src\include\avl\CConfig.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _CCONFIG_H
#define _CCONFIG_H

class CConfig
{
private:
    static CConfig *m_pEdvrCfg;
    const char *m_strFile;
    
public:
    CConfig(/* args */);
    ~CConfig();
    static CConfig *GetInstance();

    int SetValue(const char *pszSection, const char *pszKey, long nValue);

    int SetValue(const char *pszSection, const char *pszKey, char *nValue);

    int SetValue(const char *pszSection, const char *pszKey, float nValue);

    long GetValue(const char *pszSection, const char *pszKey, long nDefault);
    
    char *GetValue(const char *pszSection, const char *pszKey, char *nDefault, char *ndata, int ndataSize);

    float GetValue(const char *pszSection, const char *pszKey, float nDefault);

    int LoadFile(const char *pszFileName);

    int DeleteSection(const char *pszSection);

    int DeleteKey(const char *pszSection, const char *pszKey);

};










#endif