#include "xadmin.h"

rstatus_t
xdb_pool_ping(struct xdb_pool *dp)
{
	int rstatus;
	struct xmeta_db *meta;

	meta = dp->xmeta_db;
	if (meta == NULL) {
		dlog(D_ERRO, "ERROR: postgres meta[%p] is NULL", meta);
		return LD_ERROR;
	}

	rstatus = pg_ping(meta->ping_uri);
	if (rstatus != PQPING_OK) {
		switch (rstatus) {
		case PQPING_REJECT:
			dlog(D_ERRO, "postgres ping fail[rejecting connections]");
			break;
		case PQPING_NO_RESPONSE:
			dlog(D_ERRO, "postgres ping fail[no response]");
			break;
		case PQPING_NO_ATTEMPT:
			dlog(D_ERRO, "postgres ping fail[no attempt]");
			break;
		default:
			dlog(D_ERRO, "postgres ping fail[connection unknown]");
			break;
		}
		return LD_ERROR;
	}

	dlog(D_INFO, "postgres ping success");

	/* TODO
	   mognodb ping test */

	return LD_OK;
}

rstatus_t
xdb_pool_disconnect(struct xdb_pool *dp)
{
	/* postgresql */
	pg_disconnect(dp->xmeta_db->conn);

	return LD_OK;
}

rstatus_t
xdb_pool_preconnect(struct xdb_pool *dp)
{
	int rstatus;
	struct xmeta_db *meta;

	rstatus = xdb_pool_ping(dp);
	if (rstatus < 0) {
		dlog(D_ERRO, "ERROR: database pool ping fail");
		return LD_ERROR;
	}

	meta = dp->xmeta_db;
	meta->conn = pg_connect(meta->uri);
	if (meta->conn == NULL) {
		dlog(D_ERRO, "ERROR: pg NOT connection to database: NOK");
		return LD_ERROR;
	}
	dlog(D_ERRO, "db_pool meta conn[%p] connect to postgreSQL success", meta->conn);
	usleep(1000);

#if 0
	dp->xdata_db->pool = pool;
	dlog(D_ERRO, "db_pool connect to mongodb success");
#endif
	/*  TODO
	  data db preconnect and database, table exist? */

	return LD_OK;
}

struct xdb_pool *
xdb_pool_create(struct array *conf_pool)
{
    rstatus_t rstatus;
	struct xdb_pool *db_pool;

	db_pool = (struct xdb_pool *)malloc(sizeof(struct xdb_pool));
	if (db_pool == NULL) {
		dlog(D_ERRO, "db pool malloc fail");
		return NULL;
	}

	db_pool->xmeta_db = (struct xmeta_db *)malloc(sizeof(struct xmeta_db));
	if (db_pool->xmeta_db == NULL) {
		free(db_pool);
		dlog(D_ERRO, "meta_db pool malloc fail");
		return NULL;
	}

	db_pool->xmeta_db->type = 0;
	db_pool->xmeta_db->hostaddr[0] = '\0';
	db_pool->xmeta_db->port = 0;
	db_pool->xmeta_db->dbname[0] = '\0';
	db_pool->xmeta_db->user[0] = '\0';
	db_pool->xmeta_db->passwd[0] = '\0';
	db_pool->xmeta_db->conn_min = 0;
	db_pool->xmeta_db->conn_max = 0;
	db_pool->xmeta_db->ping_uri[0] = '\0';
	db_pool->xmeta_db->uri[0] = '\0';
	db_pool->xmeta_db->conn = NULL;

	/* transform conf db to db pool */
	rstatus = array_each(conf_pool, conf_db_each_transform, db_pool);
	if (rstatus != LD_OK) {
		free(db_pool->xmeta_db);
		free(db_pool);
		dlog(D_ERRO, "xdb pool trnsform conf pool fail");
		return NULL;
	}

	dlog(D_INFO, "db pool create success");

	return db_pool;
}

