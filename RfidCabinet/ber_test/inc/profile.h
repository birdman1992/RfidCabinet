/*
 * profile.h
 *
 *  Created on: 2015年9月22日
 *      Author: lovenix
 */

#ifndef INC_PROFILE_H_
#define INC_PROFILE_H_

#define CONFIG_FILE "config"
extern int FileCheck();
extern int DefaultConfigSet(void);
extern int GetConfigArg(int num, char* buf);
#endif /* INC_PROFILE_H_ */
