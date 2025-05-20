#ifndef _CMD_H_
#define _CMD_H_

#include "type_def.h"

/* Check command */
void checkCmd(const char *cmd);

/* Output list commands */
void outputCmdList();

/* Parse command codes */
cmd_codes_t parseCmdCodes(const char *cmd);

#endif