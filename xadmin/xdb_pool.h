#ifndef __XDB_POOL_H_INCLUDED__
#define __XDB_POOL_H_INCLUDED__

#include "xadmin.h"

#define XDB_POOL_URI_MAX		(512)

/* postgreSQL */
struct xmeta_db
{
	int						type;					/* postgresql, oracle, mysql */
	char					hostaddr[128];
	int						port;
	char					dbname[128];
	char					user[128];
	char					passwd[128];
	int						conn_min;
	int						conn_max;
	char					ping_uri[XDB_POOL_URI_MAX];
	char					uri[XDB_POOL_URI_MAX];
	PGconn					*conn;
};

/* mongodb */
struct xdata_db
{
	int						type;					/* mongodb */
	char					hostaddr[128];
	int						port;
	char					dbname[128];
	char					user[128];
	char					passwd[128];
	int						conn_min;
	int						conn_max;
	char					ping_uri[XDB_POOL_URI_MAX];
	char					uri[XDB_POOL_URI_MAX];
};

struct xdb_pool
{
	struct xmeta_db			*xmeta_db;
	struct xdata_db			*xdata_db;
	unsigned				preconnect:1;
};

int xdb_pool_disconnect(struct xdb_pool *dp);
int xdb_pool_preconnect(struct xdb_pool *dp);
struct xdb_pool *xdb_pool_create(struct array *conf_pool);

#endif /* __XDB_POOL_H_INCLUDED__ */

