#ifndef __LGC_H__
#define __LGC_H__

#include "lgc_sys.h"
#include "lgc_algo.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief lgc 模块初始化
 *
 * @return int
 */
int LGC_Init();

/**
 * @brief lgc 模块反初始化
 *
 * @return
 */
void LGC_DeInit();

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif