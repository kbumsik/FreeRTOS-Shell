#ifndef _FRS_SH_H_
#define _FRS_SH_H_

#define FRS_SH_CMD_SIMPLE   0
#define FRS_SH_CMD_PIPELINE 1
#define FRS_SH_CMD_REDIRECT 2

#ifdef __cplusplus
extern "C"{
#endif
void frs_sh(void *parameters);

void frs_sh_cmd(const char * const commend);

#ifdef __cplusplus
}
#endif

#endif // !_FRS_SH_H_