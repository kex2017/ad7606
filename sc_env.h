
#ifndef SC_ENV_H_
#define SC_ENV_H_

#ifdef __cplusplus
extern "C" {
#endif

int setenv_command(int argc, char **argv);
int printenv_command(int argc, char **argv);
int saveenv_command(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* SC_ENV_H_ */
