#ifndef __XADMIN_CONF_H_INCLUDED__
#define __XADMIN_CONF_H_INCLUDED__

#include <sys/types.h>
#include <sys/un.h>

#include "yaml/yaml.h"

#include "xadmin.h"

#define CONF_OK				(void *) NULL
#define CONF_ERROR			(void *) "has an invalid value"

#define CONF_ROOT_DEPTH		1
#define CONF_MAX_DEPTH		CONF_ROOT_DEPTH + 1

#define CONF_DEFAULT_ARGS		3
#define CONF_DEFAULT_POOL		8
#define CONF_DEFAULT_SERVERS	8

#define CONF_UNSET_NUM  -1
#define CONF_UNSET_PTR  NULL

#if 0
#define CONF_UNSET_HASH (hash_type_t) -1
#define CONF_UNSET_DIST (dist_type_t) -1

#define CONF_DEFAULT_HASH                    HASH_FNV1A_64
#define CONF_DEFAULT_DIST                    DIST_KETAMA
#endif

#define CONF_DEFAULT_PG_PORT				5432
#define CONF_DEFAULT_MG_PORT				27017
#define CONF_DEFAULT_THR_POOL_MIN			1
#define CONF_DEFAULT_THR_POOL_MAX			1
#define CONF_DEFAULT_CONN_POOL_MIN			1
#define CONF_DEFAULT_CONN_POOL_MAX			1

#define CONF_DEFAULT_TIMEOUT                 -1

struct context;

struct conf_pool
{
	struct string	name;			/* pool name (root node) */

	/* database configure */
	struct string	product;
	struct string	ip;
	int				port;
	struct string	database;
	struct string	user;
	struct string	passwd;
	int				thr_pool_min;
	int				thr_pool_max;
	int				conn_pool_min;	/* thr_pool min */
	int				conn_pool_max;	/* thr_pool max */

	/* httpd configure */
	struct string	certificate;
	/* port */
//	int				httpd_port;
	int				worker_thread;

	/* vrrpd configure */
	struct string	enable;
	struct string	interface;
	int				vrid;
	int				adver_int;
	int				priority;
	struct string	no_vmac;
	struct string	gateway;
	struct string	primary_addr;

	unsigned		valid:1;		/* valid? */
};

struct conf
{
	char			*fname;			/* file name (ref in argv[]) */
	FILE			*fh;			/* file handle */
	struct array	arg;			/* string[] (parsed {key, value} pairs) */
	struct array	pool;			/* conf_pool[] (parsed pools) */
	int				depth;			/* parsed tree depth */
	yaml_parser_t	parser;			/* yaml parser */
	yaml_event_t	event;			/* yaml event */
	yaml_token_t	token;			/* yaml token */
	unsigned		seq:1;			/* sequence? */
	unsigned		valid_parser:1;	/* valid parser? */
	unsigned		valid_event:1;	/* valid event? */
	unsigned		valid_token:1;	/* valid token? */
	unsigned		sound:1;		/* sound? */
	unsigned		parsed:1;		/* parsed? */
	unsigned		valid:1;		/* valid? */
};

struct command
{
    struct string	name;
	char			*(*set)(struct conf *cf, struct command *cmd, void *data);
	int				offset;
};

#define null_command { null_string, NULL, 0 }

char *conf_set_string(struct conf *cf, struct command *cmd, void *conf);
char *conf_set_num(struct conf *cf, struct command *cmd, void *conf);

rstatus_t conf_file_dump(char *conf_file, struct conf *cf);
rstatus_t conf_data_passwd_set(void *elem, char *pw);
rstatus_t conf_meta_passwd_set(void *elem, char *pw);
rstatus_t conf_db_each_transform(void *elem, void *data);

struct conf *conf_create(char *filename);
void conf_destroy(struct conf *cf);

#endif

