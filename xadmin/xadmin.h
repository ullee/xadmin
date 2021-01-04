#ifndef __XADMIN_H_INCLUDED__
#define __XADMIN_H_INCLUDED__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <termios.h>
#include <dirent.h>

#include "am_conf.h"

#include "libx/xlog.h"
#include "crypt/seed.h"
#include "crypt/seed_sgn.h"
#include "xdb/xpg.h"

#include "xstring.h"
#include "xarray.h"
#include "xadmin_conf.h"
#include "xdb_pool.h"
#include "xdbbackup.h"

struct conf_pool;

typedef int32_t rstatus_t;

#define LD_OK		 0
#define LD_ERROR	-1
#define LD_EAGAIN	-2
#define LD_ENOMEM	-3

#define SG_UINT8_MAXLEN			(3 + 1)
#define SG_UINT16_MAXLEN		(5 + 1)
#define SG_UINT32_MAXLEN		(10 + 1)
#define SG_UINT64_MAXLEN		(20 + 1)
#define SG_UINTMAX_MAXLEN		SG_UINT64_MAXLEN

#define SG_ISTRING_MAXLEN		(32 + 1)

#define CONF_TAG_LEN			64
#define CONF_VALUE_LEN			128

#define DAEMON_NAME				"xadmin"

#define VERSION					"/secureguard/am/engine/conf/am_version_info"

#define XADMIN_DB_BAK_DIR		"/auditbackup/xadmin/backup_dump"

#define DB_START_CMD			"su - %s -c \"%s/pg_ctl -D %s/ -w start\" > /dev/null"
#define DB_STOP_CMD				"su - %s -c \"%s/pg_ctl -m fast stop\" > /dev/null"

/* xpg */
#define PG_DB_USER						"sgn"
#define PG_DB_NAME						"secureguard"
#define PG_DB_CONF_LEN					(1024)
#define PG_DB_PASSWD_LEN				(256)

/* xadmin */
#define OS_ACCOUNT_NAME					"prime"
#define XADMIN_NETWORK_INFO_FILE		AM_CONF_DIR "/network.conf"
#define XADMIN_LOG_LEVEL_CONF_FILE		AM_CONF_DIR "/log_level.conf"
#define XADMIN_PROCLIST					AM_CONF_DIR "/proc_info"
#define XADMIN_LOG_FILE					AM_LOG_DIR  "/xadmin"
#define XADMIN_TABLE_LIST				AM_CONF_DIR "/tablelist"

#define AM_DB_HOME_DIR					"/audit/db"
#define PG_HOME_DIR						AM_DB_HOME_DIR	"/pgsql"
#define PG_BIN_DIR						PG_HOME_DIR		"/bin"
#define PG_BDR_DATA_DIR					PG_HOME_DIR		"/bdr-data"

#define XADMIN_CMD_MAX_LEN				(1024)


typedef enum  {
	SELECT_Y_OR_N = 1,
	SELECT_APPLY_OR_CALCLE,

} sg_char_code;

typedef enum  {
	STRING_A = 1,

} sg_string_code;

typedef enum  {
	SELECT_VALUE = 1,

} sg_uint32_code;

typedef enum  {
	MENU_ADMIN_CONSOLE = 1,
	MENU_NETWORK_SETUP,
	MENU_ADMIN_TASK,
	MENU_DATABASE_TASK,

} sg_menu_code;

struct select_char_str_t {
	int code;
	char *str;
};

struct daemon_conf {
	int log_level;
};

struct context
{
	pid_t						pid;			/* process id */
	char						*conf_filename;	/* configuration xphbd filename */
	struct conf					*cf;			/* configuration */
	struct xdb_pool				*dp;			/* database connection pool */
};

#if 0
struct select_char_str_t {
	int code;
	char *str;
} select_char_str[] = { 
	{SELECT_Y_OR_N, "Are you sure you want to select? (y/n) : "},
	{SELECT_APPLY_OR_CALCLE, "Would you like to apply? (a/c) : "},
	{-1, NULL}
}; 
#endif

#endif /* __SGADMIN_H_INCLUDED__ */

