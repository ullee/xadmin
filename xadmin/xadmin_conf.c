#include "xadmin.h"
#include "xadmin_conf.h"

#define nc_atoi(_line, _n)			\
	_nc_atoi((uint8_t *)_line, (size_t)_n)

static struct command conf_commands[] = {
	/* database configure */
	{ string("product"),
	  conf_set_string,
	  offsetof(struct conf_pool, product) },

	{ string("ip"),
	  conf_set_string,
	  offsetof(struct conf_pool, ip) },

	{ string("port"),
	  conf_set_num,
	  offsetof(struct conf_pool, port) },

	{ string("database"),
	  conf_set_string,
	  offsetof(struct conf_pool, database) },

	{ string("user"),
	  conf_set_string,
	  offsetof(struct conf_pool, user) },

	{ string("passwd"),
	  conf_set_string,
	  offsetof(struct conf_pool, passwd) },

	{ string("thr_pool_min"),
	  conf_set_num,
	  offsetof(struct conf_pool, thr_pool_min) },

	{ string("thr_pool_max"),
	  conf_set_num,
	  offsetof(struct conf_pool, thr_pool_max) },

	{ string("conn_pool_min"),
	  conf_set_num,
	  offsetof(struct conf_pool, conn_pool_min) },

	{ string("conn_pool_max"),
	  conf_set_num,
	  offsetof(struct conf_pool, conn_pool_max) },

	{ string("conn_pool_min"),
	  conf_set_num,
	  offsetof(struct conf_pool, conn_pool_min) },

	/* httpd configure */
	{ string("certificate"),
	  conf_set_string,
	  offsetof(struct conf_pool, certificate) },

	/* port */
/*	{ string("httpd_port"),
	  conf_set_num,
	  offsetof(struct conf_pool, httpd_port) }, */

	{ string("worker_thread"),
	  conf_set_num,
	  offsetof(struct conf_pool, worker_thread) },

	/* vrrpd configure */
	{ string("enable"),
	  conf_set_string,
	  offsetof(struct conf_pool, enable) },

	{ string("interface"),
	  conf_set_string,
	  offsetof(struct conf_pool, interface) },

	{ string("vrid"),
	  conf_set_num,
	  offsetof(struct conf_pool, vrid) },

	{ string("adver_int"),
	  conf_set_num,
	  offsetof(struct conf_pool, adver_int) },

	{ string("priority"),
	  conf_set_num,
	  offsetof(struct conf_pool, priority) },

	{ string("no_vmac"),
	  conf_set_string,
	  offsetof(struct conf_pool, no_vmac) },

	{ string("gateway"),
	  conf_set_string,
	  offsetof(struct conf_pool, gateway) },

	{ string("primary_addr"),
	  conf_set_string,
	  offsetof(struct conf_pool, primary_addr) },

#if 0
# system spec
system:
    cpu: 2.2Gz
    cpu_core: 2
    memory: 16G
    disk: 1000G
    interface: eth1
    gateway: 192.168.1.1
    ip: 192.168.1.57
    limit_session: 1000
#endif

    null_command
};

int
_nc_atoi(uint8_t *line, size_t n)
{
	int value;

	if (n == 0) {
		return -1;
	}

	for (value = 0; n--; line++) {
		if (*line < '0' || *line > '9') {
			return -1;
		}

		value = value * 10 + (*line - '0');
	}

	if (value < 0) {
		return -1;
	}

	return value;
}

static rstatus_t
conf_pool_init(struct conf_pool *cp, struct string *name)
{
    rstatus_t status;

    string_init(&cp->name);
    string_init(&cp->product);
    string_init(&cp->ip);
	cp->port = CONF_UNSET_NUM;
    string_init(&cp->database);
    string_init(&cp->user);
    string_init(&cp->passwd);
	cp->thr_pool_min = CONF_UNSET_NUM;
	cp->thr_pool_max = CONF_UNSET_NUM;
	cp->conn_pool_min = CONF_UNSET_NUM;
	cp->conn_pool_max = CONF_UNSET_NUM;

	/* httpd */
    string_init(&cp->certificate);
	/* port */
//	cp->httpd_port = CONF_UNSET_NUM;
	cp->worker_thread = CONF_UNSET_NUM;

	/* vrrpd */
    string_init(&cp->enable);
    string_init(&cp->interface);
	cp->vrid = CONF_UNSET_NUM;
	cp->adver_int = CONF_UNSET_NUM;
	cp->priority = CONF_UNSET_NUM;
    string_init(&cp->no_vmac);
    string_init(&cp->gateway);
    string_init(&cp->primary_addr);

    cp->valid = 0;

    status = string_duplicate(&cp->name, name);
    if (status != LD_OK) {
        return status;
    }

    if (status != LD_OK) {
        string_deinit(&cp->name);
        return status;
    }

	dlog(D_DBUG, "init conf pool[%p] %.*s", cp, name->len, name->data);

    return LD_OK;
}

static void
conf_pool_deinit(struct conf_pool *cp)
{
	dlog(D_DBUG, "deinit conf pool[%p] %.*s", cp, cp->name.len, cp->name.data);

    string_deinit(&cp->name);
    string_deinit(&cp->product);
    string_deinit(&cp->ip);
    string_deinit(&cp->database);
    string_deinit(&cp->user);
    string_deinit(&cp->passwd);

	/* httpd */
    string_init(&cp->certificate);

	/* vrrpd */
    string_init(&cp->enable);
    string_init(&cp->interface);
    string_init(&cp->no_vmac);
    string_init(&cp->gateway);
    string_init(&cp->primary_addr);

	return ;
}

rstatus_t
conf_data_passwd_set(void *elem, char *pw)
{
	struct string data_db;
	struct string mongodb;
	struct conf_pool *cp = elem;

	if (!(cp->valid)) {
		dlog(D_ERRO, "conf: conf pool[%d] NULL", cp->valid);
		return LD_OK;
	}

	/* database */
	string_set_text(&data_db, "data");
	string_set_text(&mongodb, "mongodb");

	if (string_compare(&cp->name, &data_db) == 0) {
		/* database type mongodb */
		if (string_compare(&cp->product, &mongodb) == 0) {
			string_copy(&cp->passwd, (uint8_t *)pw, (uint32_t)strlen(pw));
			dlog(D_DBUG, "database[%s] passwd set [%s]", cp->product.data, cp->passwd.data);
		}
	}

	return LD_OK;
}

rstatus_t
conf_meta_passwd_set(void *elem, char *pw)
{
	struct string meta_db;
	struct string postgres;
	struct conf_pool *cp = elem;

	if (!(cp->valid)) {
		dlog(D_ERRO, "conf: conf pool[%d] NULL", cp->valid);
		return LD_OK;
	}

	/* database */
	string_set_text(&meta_db, "meta");
	string_set_text(&postgres, "postgresql");

	if (string_compare(&cp->name, &meta_db) == 0) {
		/* database type postgresql */
		if (string_compare(&cp->product, &postgres) == 0) {
			string_copy(&cp->passwd, (uint8_t *)pw, (uint32_t)strlen(pw));
			dlog(D_DBUG, "database[%s] passwd set [%s]", cp->product.data, cp->passwd.data);
		}
	}

	return LD_OK;
}

rstatus_t
conf_db_each_transform(void *elem, void *data)
{
	struct string meta_db;
	struct string data_db;
	struct string postgres;
	struct string oracle;
	struct string mysql;
	struct string mongodb;
	struct conf_pool *cp = elem;
    struct xdb_pool *db_pool = data;
	char passwd[XADMIN_CMD_MAX_LEN];
	char *uri;

	if (!(cp->valid)) {
		dlog(D_ERRO, "conf: conf pool[%d] NULL", cp->valid);
		return LD_OK;
	}

	/* database */
	string_set_text(&meta_db, "meta");
	string_set_text(&postgres, "postgresql");
	string_set_text(&oracle, "oracle");
	string_set_text(&mysql, "mysql");
	string_set_text(&data_db, "data");
	string_set_text(&mongodb, "mongodb");

	if (string_compare(&cp->name, &meta_db) == 0) {
		/* database type postgresql */
		if (string_compare(&cp->product, &postgres) == 0) {
			db_pool->xmeta_db->type = 1;
			snprintf(db_pool->xmeta_db->hostaddr, sizeof(db_pool->xmeta_db->hostaddr), "%s", cp->ip.data);
			db_pool->xmeta_db->port= cp->port;
			snprintf(db_pool->xmeta_db->dbname, sizeof(db_pool->xmeta_db->dbname), "%s", cp->database.data);
			snprintf(db_pool->xmeta_db->user, sizeof(db_pool->xmeta_db->user), "%s", cp->user.data);
			snprintf(db_pool->xmeta_db->passwd, sizeof(db_pool->xmeta_db->passwd), "%s", cp->passwd.data);
			db_pool->xmeta_db->conn_min = cp->conn_pool_min;
			db_pool->xmeta_db->conn_max = cp->conn_pool_max;

			/* TODO
			 * password dec */
			if (cp->passwd.len != 0) {
				sgnseed_t seed_ctx;
				memset(passwd, 0x00, sizeof(passwd));
				init_seed(&seed_ctx);
				sgnseed_decrypt(&seed_ctx, (char *)cp->passwd.data, passwd);
			}

			uri = db_pool->xmeta_db->ping_uri;
			snprintf(uri, XDB_POOL_URI_MAX, "host=%s port=%d dbname=%s connect_timeout=5",
					cp->ip.data, cp->port, cp->database.data);
			dlog(D_DBUG, "database[postgres] db pool meta ping uri[%s]", db_pool->xmeta_db->ping_uri);

			uri = db_pool->xmeta_db->uri;
			snprintf(uri, XDB_POOL_URI_MAX, " hostaddr=%s dbname=%s user=%s password=%s",
					cp->ip.data, cp->database.data, cp->user.data, passwd);
			dlog(D_DBUG, "database[postgres] db pool meta uri[%s]", db_pool->xmeta_db->uri);
		}

		/* database type oracle */
		if (string_compare(&cp->product, &oracle) == 0) {
			dlog(D_DBUG, "database[oracle] db pool meta uri[%s]", "");
		}
		/* database type mysql */
		if (string_compare(&cp->product, &mysql) == 0) {
			dlog(D_DBUG, "database[mysql] db pool meta uri[%s]", "");
		}
	}

	if (string_compare(&cp->name, &data_db) == 0) {
		uri = db_pool->xdata_db->uri;

		if (string_compare(&cp->product, &mongodb) == 0) {
			db_pool->xdata_db->type = 1;
			snprintf(db_pool->xdata_db->hostaddr, sizeof(db_pool->xdata_db->hostaddr), "%s", cp->ip.data);
			db_pool->xdata_db->port= cp->port;
			snprintf(db_pool->xdata_db->dbname, sizeof(db_pool->xdata_db->dbname), "%s", cp->database.data);
			snprintf(db_pool->xdata_db->user, sizeof(db_pool->xdata_db->user), "%s", cp->user.data);
			snprintf(db_pool->xdata_db->passwd, sizeof(db_pool->xdata_db->passwd), "%s", cp->passwd.data);

			/* TODO
			 * password dec */
			if (cp->passwd.len != 0) {
				sgnseed_t seed_ctx;
				memset(passwd, 0x00, sizeof(passwd));
				init_seed(&seed_ctx);
				sgnseed_decrypt(&seed_ctx, (char *)cp->passwd.data, passwd);
			}

			snprintf(uri, XDB_POOL_URI_MAX, "%s://%s?maxPoolSize=%d&minPoolSize=%d",
					cp->product.data, cp->ip.data, cp->conn_pool_max, cp->conn_pool_min);
			dlog(D_DBUG, "db poll mongodb uri[%s]", db_pool->xdata_db->uri);
		}
	}

	return LD_OK;
}

#if 0
rstatus_t
conf_thread_pool_transform(void *elem, void *data)
{
	struct string db;
    struct conf_pool *cp = elem;
    struct xthr_pool_size *pool_size = data;

	if (!(cp->valid)) {
		dlog(D_ERRO, "conf: conf pool[%d] NULL", cp->valid);
		return LD_OK;
	}

	string_set_text(&db, "meta");
	if (string_compare(&cp->name, &db) == 0) {
		pool_size->meta_pool_min = cp->thr_pool_min;
		pool_size->meta_pool_max = cp->thr_pool_max;
		dlog(D_DBUG, "meta thread pool min[%d] max[%d]",
				pool_size->meta_pool_min, pool_size->meta_pool_max);
	}

	string_set_text(&db, "data");
	if (string_compare(&cp->name, &db) == 0) {
		pool_size->data_pool_min = cp->thr_pool_min;
		pool_size->data_pool_max = cp->thr_pool_max;
		dlog(D_DBUG, "data thread pool min[%d] max[%d]",
				pool_size->data_pool_min, pool_size->data_pool_max);
	}

    return LD_OK;
}
#endif

//static void
void
conf_dump(struct conf *cf)
{
    uint32_t i, npool;
    struct conf_pool *cp;
	struct string meta;
	struct string data;
	struct string httpd;
	struct string vrrpd;

    npool = array_n(&cf->pool);
    if (npool == 0) {
        return;
    }

	dlog(D_DBUG, "%"PRIu32" pools in configuration file[%s]", npool, cf->fname);

	string_set_text(&meta, "meta");
	string_set_text(&data, "data");
	string_set_text(&httpd, "httpd");
	string_set_text(&vrrpd, "vrrpd");

    for (i = 0; i < npool; i++) {
        cp = array_get(&cf->pool, i);

		/* meta database */
		if (string_compare(&cp->name, &meta) == 0 || string_compare(&cp->name, &data) == 0) {
			dlog(D_DBUG, "%.*s", cp->name.len, cp->name.data);
			dlog(D_DBUG, "   product: %.*s", cp->product.len, cp->product.data);
			dlog(D_DBUG, "   ip: %.*s", cp->ip.len, cp->ip.data);
			dlog(D_DBUG, "   port: %d", cp->port);
			dlog(D_DBUG, "   database: %.*s", cp->database.len, cp->database.data);
			dlog(D_DBUG, "   user: %.*s", cp->user.len, cp->user.data);
			dlog(D_DBUG, "   passwd: %.*s", cp->passwd.len, cp->passwd.data);
			dlog(D_DBUG, "   thr_pool_min: %d", cp->thr_pool_min);
			dlog(D_DBUG, "   thr_pool_max: %d", cp->thr_pool_max);
			dlog(D_DBUG, "   conn_pool_min: %d", cp->conn_pool_min);
			dlog(D_DBUG, "   conn_pool_max: %d", cp->conn_pool_max);
		}

		/* data database */

		/* httpd */
		if (string_compare(&cp->name, &httpd) == 0) {
			dlog(D_DBUG, "%.*s", cp->name.len, cp->name.data);
			dlog(D_DBUG, "   certificate: %.*s", cp->certificate.len, cp->certificate.data);
			dlog(D_DBUG, "   port: %d", cp->port);
			dlog(D_DBUG, "   worker thread: %d", cp->worker_thread);
		}

		/* vrrpd */
		if (string_compare(&cp->name, &vrrpd) == 0) {
			dlog(D_DBUG, "%.*s", cp->name.len, cp->name.data);
			dlog(D_DBUG, "   enable: %.*s", cp->enable.len, cp->enable.data);
			dlog(D_DBUG, "   interface: %.*s", cp->interface.len, cp->interface.data);
			dlog(D_DBUG, "   vrid: %d", cp->vrid);
			dlog(D_DBUG, "   adver int: %d", cp->adver_int);
			dlog(D_DBUG, "   no_vmac: %.*s", cp->no_vmac.len, cp->no_vmac.data);
			dlog(D_DBUG, "   gateway: %.*s", cp->gateway.len, cp->gateway.data);
			dlog(D_DBUG, "   primary addr: %.*s", cp->primary_addr.len, cp->primary_addr.data);
		}
    }

	return ;
}

rstatus_t
conf_file_dump(char *conf_file, struct conf *cf)
{
    uint32_t i, npool;
	struct string meta;
	struct string data;
	struct string httpd;
	struct string vrrpd;
    struct conf_pool *cp;
//	struct conf *cf = ctx->cf;;

    npool = array_n(&cf->pool);
    if (npool == 0) {
		dlog(D_ERRO, "EROR: conf pool array count zero");
        return -1;
    }

	/* TODO
	 * write to file */

	dlog(D_DBUG, "%"PRIu32" pools in configuration file[%s]", npool, cf->fname);

	string_set_text(&meta, "meta");
	string_set_text(&data, "data");
	string_set_text(&httpd, "httpd");
	string_set_text(&vrrpd, "vrrpd");

    for (i = 0; i < npool; i++) {
        cp = array_get(&cf->pool, i);

		/* meta database */
		if (string_compare(&cp->name, &meta) == 0 || string_compare(&cp->name, &data) == 0) {
			dlog(D_DBUG, "%.*s", cp->name.len, cp->name.data);
			dlog(D_DBUG, "   product: %.*s", cp->product.len, cp->product.data);
			dlog(D_DBUG, "   ip: %.*s", cp->ip.len, cp->ip.data);
			dlog(D_DBUG, "   port: %d", cp->port);
			dlog(D_DBUG, "   database: %.*s", cp->database.len, cp->database.data);
			dlog(D_DBUG, "   user: %.*s", cp->user.len, cp->user.data);
			dlog(D_DBUG, "   passwd: %.*s", cp->passwd.len, cp->passwd.data);
			dlog(D_DBUG, "   thr_pool_min: %d", cp->thr_pool_min);
			dlog(D_DBUG, "   thr_pool_max: %d", cp->thr_pool_max);
			dlog(D_DBUG, "   conn_pool_min: %d", cp->conn_pool_min);
			dlog(D_DBUG, "   conn_pool_max: %d", cp->conn_pool_max);
		}

		/* data database */

		/* httpd */
		if (string_compare(&cp->name, &httpd) == 0) {
			dlog(D_DBUG, "%.*s", cp->name.len, cp->name.data);
			dlog(D_DBUG, "   certificate: %.*s", cp->certificate.len, cp->certificate.data);
			dlog(D_DBUG, "   port: %d", cp->port);
			dlog(D_DBUG, "   worker thread: %d", cp->worker_thread);
		}

		/* vrrpd */
		if (string_compare(&cp->name, &vrrpd) == 0) {
			dlog(D_DBUG, "%.*s", cp->name.len, cp->name.data);
			dlog(D_DBUG, "   enable: %.*s", cp->enable.len, cp->enable.data);
			dlog(D_DBUG, "   interface: %.*s", cp->interface.len, cp->interface.data);
			dlog(D_DBUG, "   vrid: %d", cp->vrid);
			dlog(D_DBUG, "   adver int: %d", cp->adver_int);
			dlog(D_DBUG, "   no_vmac: %.*s", cp->no_vmac.len, cp->no_vmac.data);
			dlog(D_DBUG, "   gateway: %.*s", cp->gateway.len, cp->gateway.data);
			dlog(D_DBUG, "   primary addr: %.*s", cp->primary_addr.len, cp->primary_addr.data);
		}
    }

	return 0;
}
static rstatus_t
conf_yaml_init(struct conf *cf)
{
	int rv;

	if (!(!cf->valid_parser)) {
		dlog(D_ERRO, "conf: valid_parser[%d]", cf->valid_parser);
		return LD_ERROR;
	}

	rv = fseek(cf->fh, 0L, SEEK_SET);
	if (rv < 0) {
		dlog(D_ERRO, "conf: failed to seek to the beginning of file[%s] error[%s]",
				cf->fname, strerror(errno));
		return LD_ERROR;
	}

	rv = yaml_parser_initialize(&cf->parser);
	if (!rv) {
		dlog(D_ERRO, "conf: failed (err %d) to initialize yaml parser",
				cf->parser.error);
		return LD_ERROR;
	}

	yaml_parser_set_input_file(&cf->parser, cf->fh);
	cf->valid_parser = 1;

	return LD_OK;
}

static void
conf_yaml_deinit(struct conf *cf)
{
	if (cf->valid_parser) {
		yaml_parser_delete(&cf->parser);
		cf->valid_parser = 0;
	}
}

static rstatus_t
conf_token_next(struct conf *cf)
{
	int rv;

	if (!(cf->valid_parser && !cf->valid_token)) {
		dlog(D_ERRO, "conf: valid_parser[%d] valid_token[%d]",
				cf->valid_parser, cf->valid_token);
		return LD_ERROR;
	}

	rv = yaml_parser_scan(&cf->parser, &cf->token);
	if (!rv) {
		dlog(D_ERRO, "conf: failed (err %d) to scan next token", cf->parser.error);
		return LD_ERROR;
	}
	cf->valid_token = 1;

	return LD_OK;
}

static void
conf_token_done(struct conf *cf)
{
	if (!(cf->valid_parser)) {
		dlog(D_ERRO, "conf: valid_parser[%d]", cf->valid_parser);
		return ;
	}

	if (cf->valid_token) {
		yaml_token_delete(&cf->token);
		cf->valid_token = 0;
	}
}

static rstatus_t
conf_event_next(struct conf *cf)
{
	int rv;

	if (!(cf->valid_parser && !cf->valid_event)) {
		dlog(D_ERRO, "conf: valid_parser[%d] valid_event[%d]",
				cf->valid_parser, cf->valid_event);
		return LD_ERROR;
	}

	rv = yaml_parser_parse(&cf->parser, &cf->event);
	if (!rv) {
		dlog(D_DBUG, "conf: failed (err %d) to get next event", cf->parser.error);
		return LD_ERROR;
	}
	cf->valid_event = 1;

	return LD_OK;
}

static void
conf_event_done(struct conf *cf)
{
	if (cf->valid_event) {
		yaml_event_delete(&cf->event);
		cf->valid_event = 0;
	}
}

static rstatus_t
conf_push_scalar(struct conf *cf)
{
	rstatus_t status;
	struct string *value;
	uint8_t *scalar;
	uint32_t scalar_len;

	scalar = cf->event.data.scalar.value;
	scalar_len = (uint32_t)cf->event.data.scalar.length;

	value = array_push(&cf->arg);
	if (value == NULL) {
		return LD_ENOMEM;
	}
	string_init(value);

	status = string_copy(value, scalar, scalar_len);
	if (status != LD_OK) {
		array_pop(&cf->arg);
		return status;
	}

	return LD_OK;
}

static void
conf_pop_scalar(struct conf *cf)
{
	struct string *value;

	value = array_pop(&cf->arg);
	string_deinit(value);
}

static rstatus_t
conf_handler(struct conf *cf, void *data)
{
	struct command *cmd;
	struct string *key, *value;
	uint32_t narg;

	if (array_n(&cf->arg) == 1) {
		value = array_top(&cf->arg);
		dlog(D_DBUG, "conf pool init on [%.*s]", value->len, value->data);
		return conf_pool_init(data, value);
	}

	narg = array_n(&cf->arg);
	value = array_get(&cf->arg, narg - 1);
	key = array_get(&cf->arg, narg - 2);

	dlog(D_DBUG, "conf handler on key[%.*s] value[%.*s]", key->len, key->data, value->len, value->data);
//	dlog(D_DBUG, "conf handler on %.*s: %.*s", key->len, key->data,
//			value->len, value->data);

	for (cmd = conf_commands; sizeof(cmd->name) != 0; cmd++) {
		char *rv;

		if (string_compare(key, &cmd->name) != 0) {
			continue;
		}

		rv = cmd->set(cf, cmd, data);
		if (rv != CONF_OK) {
			dlog(D_DBUG, "conf: directive \"%.*s\" return[%s]", key->len, key->data, rv);
			return LD_ERROR;
		}

		return LD_OK;
	}

	dlog(D_DBUG, "conf: directive \"%.*s\" is unknown", key->len, key->data);

	return LD_ERROR;
}

static rstatus_t
conf_begin_parse(struct conf *cf)
{
	rstatus_t status;
	bool done;

	if (!(cf->sound && !cf->parsed)) {
		dlog(D_ERRO, "conf: sound[%d] parsed[%d]", cf->sound, cf->parsed);
		return LD_ERROR;
	}

	if (!(cf->depth == 0)) {
		dlog(D_ERRO, "conf: depth[%d]", cf->depth);
		return LD_ERROR;
	}

	status = conf_yaml_init(cf);
	if (status != LD_OK) {
		return status;
	}

	done = false;
	do {
		status = conf_event_next(cf);
		if (status != LD_OK) {
			return status;
		}

		switch (cf->event.type) {
			case YAML_STREAM_START_EVENT:
			case YAML_DOCUMENT_START_EVENT:
				break;

			case YAML_MAPPING_START_EVENT:
				if (!(cf->depth < CONF_MAX_DEPTH)) {
					dlog(D_ERRO, "conf: depth[%d] < max_depth[%d]", cf->depth, CONF_MAX_DEPTH);
				}
				cf->depth++;
				done = true;
				break;

			default:
				;
//				NOT_REACHED();
		}

		conf_event_done(cf);

	} while (!done);

	return LD_OK;
}

static rstatus_t
conf_end_parse(struct conf *cf)
{
	rstatus_t status;
	bool done;

	if (!(cf->sound && !cf->parsed)) {
		dlog(D_ERRO, "conf: sound[%d] parsed[%d]", cf->sound, cf->parsed);
		return LD_ERROR;
	}

	if (!(cf->depth == 0)) {
		dlog(D_ERRO, "conf: depth[%d]", cf->depth);
		return LD_ERROR;
	}

	done = false;
	do {
		status = conf_event_next(cf);
		if (status != LD_OK) {
			return status;
		}

		switch (cf->event.type) {
			case YAML_STREAM_END_EVENT:
				done = true;
				break;

			case YAML_DOCUMENT_END_EVENT:
				break;

			default:
				;
//				NOT_REACHED();
		}

		conf_event_done(cf);
	} while (!done);

	conf_yaml_deinit(cf);

	return LD_OK;
}

static rstatus_t
conf_parse_core(struct conf *cf, void *data)
{
	rstatus_t status;
	bool done, leaf, new_pool;

	if (!(cf->sound)) {
		dlog(D_ERRO, "conf: sound[%d]", cf->sound);
		return LD_ERROR;
	}

	status = conf_event_next(cf);
	if (status != LD_OK) {
		dlog(D_ERRO, "conf: conf_event_next fail");
		return status;
	}

	done = false;
	leaf = false;
	new_pool = false;

	switch (cf->event.type) {
		case YAML_MAPPING_END_EVENT:
			cf->depth--;
			if (cf->depth == 1) {
				conf_pop_scalar(cf);
			} else if (cf->depth == 0) {
				done = true;
			}
			break;

		case YAML_MAPPING_START_EVENT:
			cf->depth++;
			break;

		case YAML_SEQUENCE_START_EVENT:
			cf->seq = 1;
			break;

		case YAML_SEQUENCE_END_EVENT:
			conf_pop_scalar(cf);
			cf->seq = 0;
			break;

		case YAML_SCALAR_EVENT:
			status = conf_push_scalar(cf);
			if (status != LD_OK) {
				break;
			}

			/* take appropriate action */
			if (cf->seq) {
				/* for a sequence, leaf is at CONF_MAX_DEPTH */
				if (!(cf->depth == CONF_MAX_DEPTH)) {
					dlog(D_ERRO, "conf: depth[%d] == max_depth[%d]", cf->depth, CONF_MAX_DEPTH);
				}
				leaf = true;
			} else if (cf->depth == CONF_ROOT_DEPTH) {
				/* create new conf_pool */
				data = array_push(&cf->pool);
				if (data == NULL) {
					dlog(D_ERRO, "conf: cf pool data[%p]", data);
					status = LD_ENOMEM;
					break;
				}
				new_pool = true;
			} else if (array_n(&cf->arg) == (uint32_t)cf->depth + 1) {
				/* for {key: value}, leaf is at CONF_MAX_DEPTH */
				if (!(cf->depth == CONF_MAX_DEPTH)) {
					dlog(D_ERRO, "conf: depth[%d] == max_depth[%d]", cf->depth, CONF_MAX_DEPTH);
				}
				leaf = true;
			}
			break;

		default:
			;
//			NOT_REACHED();
			break;
	}

	conf_event_done(cf);

	if (status != LD_OK) {
		return status;
	}

	if (done) {
		/* terminating condition */
		return LD_OK;
	}

	if (leaf || new_pool) {
		status = conf_handler(cf, data);

		if (leaf) {
			conf_pop_scalar(cf);
			if (!cf->seq) {
				conf_pop_scalar(cf);
			}
		}

		if (status != LD_OK) {
			dlog(D_ERRO, "conf: status[%d] fail", status);
			return status;
		}
	}

	return conf_parse_core(cf, data);
}

static rstatus_t
conf_parse(struct conf *cf)
{
	rstatus_t status;

	if (!(cf->sound && !cf->parsed)) {
		dlog(D_ERRO, "conf: sound[%d] parsed[%d]", cf->sound, cf->parsed);
		return LD_ERROR;
	}
	if (!(array_n(&cf->arg) == 0)) {
		dlog(D_ERRO, "conf: cf arg[%p] NULL", &cf->arg);
		return LD_ERROR;
	}

	status = conf_begin_parse(cf);
	if (status != LD_OK) {
		dlog(D_DBUG, "ERROR: conf_begin_parse");
		return status;
	}

	status = conf_parse_core(cf, NULL);
	if (status != LD_OK) {
		dlog(D_DBUG, "ERROR: conf_parse_core");
		return status;
	}

	status = conf_end_parse(cf);
	if (status != LD_OK) {
		dlog(D_DBUG, "ERROR: conf_end_parse");
		return status;
	}

	cf->parsed = 1;

	return LD_OK;
}

static struct conf *
conf_open(char *filename)
{
	rstatus_t status;
	struct conf *cf;
	FILE *fh;

	fh = fopen(filename, "r");
	if (fh == NULL) {
		dlog(D_DBUG, "conf: failed to open config[%s]: %s", filename,
				strerror(errno));
		return NULL;
	}

	cf = malloc(sizeof(*cf));
	if (cf == NULL) {
		fclose(fh);
		return NULL;
	}

	status = array_init(&cf->arg, CONF_DEFAULT_ARGS, sizeof(struct string));
	if (status != LD_OK) {
		free(cf);
		fclose(fh);
		return NULL;
	}

	status = array_init(&cf->pool, CONF_DEFAULT_POOL, sizeof(struct conf_pool));
	if (status != LD_OK) {
		array_deinit(&cf->arg);
		free(cf);
		fclose(fh);
		return NULL;
	}

	cf->fname = filename;
	cf->fh = fh;
	cf->depth = 0;
	/* parser, event, and token are initialized later */
	cf->seq = 0;
	cf->valid_parser = 0;
	cf->valid_event = 0;
	cf->valid_token = 0;
	cf->sound = 0;
	cf->parsed = 0;
	cf->valid = 0;

	dlog(D_DBUG, "opened conf file[%s]", filename);

	return cf;
}

static rstatus_t
conf_validate_document(struct conf *cf)
{
	rstatus_t status;
	uint32_t count;
	bool done;

	status = conf_yaml_init(cf);
	if (status != LD_OK) {
		return status;
	}

	count = 0;
	done = false;
	do {
		yaml_document_t document;
		yaml_node_t *node;
		int rv;

		rv = yaml_parser_load(&cf->parser, &document);
		if (!rv) {
			dlog(D_DBUG, "conf: failed (err %d) to get the next yaml document", cf->parser.error);
			conf_yaml_deinit(cf);
			return LD_ERROR;
		}

		node = yaml_document_get_root_node(&document);
		if (node == NULL) {
			done = true;
		} else {
			count++;
		}

		yaml_document_delete(&document);
	} while (!done);

	conf_yaml_deinit(cf);

	if (count != 1) {
		dlog(D_DBUG, "conf: '%s' must contain only 1 document; found %"PRIu32" "
				"documents", cf->fname, count);
		return LD_ERROR;
	}

	return LD_OK;
}

static rstatus_t
conf_validate_tokens(struct conf *cf)
{
	rstatus_t status;
	bool done, error;
	int type;

	status = conf_yaml_init(cf);
	if (status != LD_OK) {
		return status;
	}

	done = false;
	error = false;
	do {
		status = conf_token_next(cf);
		if (status != LD_OK) {
			return status;
		}
		type = cf->token.type;

		switch (type) {
			case YAML_NO_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: no token (%d) is disallowed", type);
				break;

			case YAML_VERSION_DIRECTIVE_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: version directive token (%d) is disallowed", type);
				break;

			case YAML_TAG_DIRECTIVE_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: tag directive token (%d) is disallowed", type);
				break;

			case YAML_DOCUMENT_START_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: document start token (%d) is disallowed", type);
				break;

			case YAML_DOCUMENT_END_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: document end token (%d) is disallowed", type);
				break;

			case YAML_FLOW_SEQUENCE_START_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: flow sequence start token (%d) is disallowed", type);
				break;

			case YAML_FLOW_SEQUENCE_END_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: flow sequence end token (%d) is disallowed", type);
				break;

			case YAML_FLOW_MAPPING_START_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: flow mapping start token (%d) is disallowed", type);
				break;

			case YAML_FLOW_MAPPING_END_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: flow mapping end token (%d) is disallowed", type);
				break;

			case YAML_FLOW_ENTRY_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: flow entry token (%d) is disallowed", type);
				break;

			case YAML_ALIAS_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: alias token (%d) is disallowed", type);
				break;

			case YAML_ANCHOR_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: anchor token (%d) is disallowed", type);
				break;

			case YAML_TAG_TOKEN:
				error = true;
				dlog(D_DBUG, "conf: tag token (%d) is disallowed", type);
				break;

			case YAML_BLOCK_SEQUENCE_START_TOKEN:
			case YAML_BLOCK_MAPPING_START_TOKEN:
			case YAML_BLOCK_END_TOKEN:
			case YAML_BLOCK_ENTRY_TOKEN:
				break;

			case YAML_KEY_TOKEN:
			case YAML_VALUE_TOKEN:
			case YAML_SCALAR_TOKEN:
				break;

			case YAML_STREAM_START_TOKEN:
				break;

			case YAML_STREAM_END_TOKEN:
				done = true;
				dlog(D_DBUG, "conf file[%s] has valid tokens", cf->fname);
				break;

			default:
				error = true;
				dlog(D_DBUG, "conf: unknown token (%d) is disallowed", type);
				break;
		}

		conf_token_done(cf);
	} while (!done && !error);

	conf_yaml_deinit(cf);

	return !error ? LD_OK : LD_ERROR;
}

static rstatus_t
conf_validate_structure(struct conf *cf)
{
	rstatus_t status;
	int type, depth;
	uint32_t i, count[CONF_MAX_DEPTH + 1];
	bool done, error, seq;

	status = conf_yaml_init(cf);
	if (status != LD_OK) {
		return status;
	}

	done = false;
	error = false;
	seq = false;
	depth = 0;
	for (i = 0; i < CONF_MAX_DEPTH + 1; i++) {
		count[i] = 0;
	}

	/*
	 * Validate that the configuration conforms roughly to the following
	 * yaml tree structure:
	 *
	 * keyx:
	 *   key1: value1
	 *   key2: value2
	 *   seq:
	 *     - elem1
	 *     - elem2
	 *     - elem3
	 *   key3: value3
	 *
	 * keyy:
	 *   key1: value1
	 *   key2: value2
	 *   seq:
	 *     - elem1
	 *     - elem2
	 *     - elem3
	 *   key3: value3
	 */
	do {
		status = conf_event_next(cf);
		if (status != LD_OK) {
			return status;
		}

		type = cf->event.type;

//		dlog(D_DBUG, "next event %d depth %d seq %d", type, depth, seq);

		switch (type) {
			case YAML_STREAM_START_EVENT:
			case YAML_DOCUMENT_START_EVENT:
				break;

			case YAML_DOCUMENT_END_EVENT:
				break;

			case YAML_STREAM_END_EVENT:
				done = true;
				break;

			case YAML_MAPPING_START_EVENT:
				if (depth == CONF_ROOT_DEPTH && count[depth] != 1) {
					error = true;
					dlog(D_DBUG, "conf: '%s' has more than one \"key:value\" at depth %d",
							cf->fname, depth);
				} else if (depth >= CONF_MAX_DEPTH) {
					error = true;
					dlog(D_DBUG, "conf: '%s' has a depth greater than %d",
							cf->fname, CONF_MAX_DEPTH);
				}
				depth++;
				break;

			case YAML_MAPPING_END_EVENT:
				if (depth == CONF_MAX_DEPTH) {
					if (seq) {
						seq = false;
					}
#if 0
					} else {
						error = true;
						dlog(D_DBUG, "conf: '%s' missing sequence directive at depth "
								"%d", cf->fname, depth);
					}
#endif
				}
				depth--;
				count[depth] = 0;
				break;

			case YAML_SEQUENCE_START_EVENT:
				if (seq) {
					error = true;
					dlog(D_DBUG, "conf: '%s' has more than one sequence directive",
							cf->fname);
				} else if (depth != CONF_MAX_DEPTH) {
					error = true;
					dlog(D_DBUG, "conf: '%s' has sequence at depth %d instead of %d",
							cf->fname, depth, CONF_MAX_DEPTH);
				} else if (count[depth] != 1) {
					error = true;
					dlog(D_DBUG, "conf: '%s' has invalid \"key:value\" at depth %d",
							cf->fname, depth);
				}
				seq = true;
				break;

			case YAML_SEQUENCE_END_EVENT:
				if (!(depth == CONF_MAX_DEPTH)) {
					dlog(D_ERRO, "conf: depth[%d] max_depth[%d]", depth, CONF_MAX_DEPTH);
				}
				count[depth] = 0;
				break;

			case YAML_SCALAR_EVENT:
				if (depth == 0) {
					error = true;
					dlog(D_DBUG, "conf: '%s' has invalid empty \"key:\" at depth %d",
							cf->fname, depth);
				} else if (depth == CONF_ROOT_DEPTH && count[depth] != 0) {
					error = true;
					dlog(D_DBUG, "conf: '%s' has invalid mapping \"key:\" at depth %d",
							cf->fname, depth);
				} else if (depth == CONF_MAX_DEPTH && count[depth] == 2) {
					/* found a "key: value", resetting! */
					count[depth] = 0;
				}
				count[depth]++;
				break;

			default:
				dlog(D_DBUG, "unknown yaml type");
//				NOT_REACHED();
		}

		conf_event_done(cf);
	} while (!done && !error);

	conf_yaml_deinit(cf);

	return !error ? LD_OK : LD_ERROR;
}

static rstatus_t
conf_pre_validate(struct conf *cf)
{
	rstatus_t status;

	status = conf_validate_document(cf);
	if (status != LD_OK) {
		return status;
	}

	status = conf_validate_tokens(cf);
	if (status != LD_OK) {
		return status;
	}

	status = conf_validate_structure(cf);
	if (status != LD_OK) {
		return status;
	}

	cf->sound = 1;

	return LD_OK;
}

static rstatus_t
conf_validate_pool(struct conf *cf, struct conf_pool *cp)
{
	struct string meta;
	struct string data;

	string_set_text(&meta, "meta");
	string_set_text(&data, "data");

	if (!(!cp->valid)) {
		dlog(D_ERRO, "conf pool cp invalid[%d]", cp->valid);
	}

	if (!(!string_empty(&cp->name))) {
		dlog(D_ERRO, "conf pool cp name[%s] is empty", cp->name);
	}

	if (cp->port == CONF_UNSET_NUM) {
		if (string_compare(&cp->name, &meta) == 0) {
			cp->port = CONF_DEFAULT_PG_PORT;
		}
		if (string_compare(&cp->name, &data) == 0) {
			cp->port = CONF_DEFAULT_MG_PORT;
		}
	}

	if (cp->thr_pool_min == CONF_UNSET_NUM) {
		cp->thr_pool_min = CONF_DEFAULT_THR_POOL_MIN;
	}

	if (cp->thr_pool_max == CONF_UNSET_NUM) {
		cp->thr_pool_max = CONF_DEFAULT_THR_POOL_MAX;
	}

	if (cp->conn_pool_min == CONF_UNSET_NUM) {
		cp->conn_pool_min = CONF_DEFAULT_CONN_POOL_MIN;
	}

	if (cp->conn_pool_max == CONF_UNSET_NUM) {
		cp->conn_pool_max = CONF_DEFAULT_CONN_POOL_MAX;
	}

	cp->valid = 1;

	return LD_OK;
}

static rstatus_t
conf_post_validate(struct conf *cf)
{
	rstatus_t status;
	uint32_t i, npool;
	bool valid;

	if (!(cf->sound && cf->parsed)) {
		dlog(D_ERRO, "cf->sound[%d] cf->parsed[%d]", cf->sound, cf->parsed);
		return LD_ERROR;
	}

	if (!(!cf->valid)) {
		dlog(D_ERRO, "cf->valid[%d]", cf->valid);
		return LD_ERROR;
	}

	npool = array_n(&cf->pool);
	if (npool == 0) {
		dlog(D_DBUG, "conf: '%.*s' has no pools", cf->fname);
		return LD_ERROR;
	}

	/* validate pool */
	for (i = 0; i < npool; i++) {
		struct conf_pool *cp = array_get(&cf->pool, i);

		status = conf_validate_pool(cf, cp);
		if (status != LD_OK) {
			return status;
		}
	}

#if 0
	/* disallow pools with duplicate names */
	array_sort(&cf->pool, conf_pool_name_cmp);
	for (valid = true, i = 0; i < npool - 1; i++) {
		struct conf_pool *p1, *p2;

		p1 = array_get(&cf->pool, i);
		p2 = array_get(&cf->pool, i + 1);

		if (string_compare(&p1->name, &p2->name) == 0) {
			dlog(D_DBUG, "conf: '%s' has pools with same name %.*s'", cf->fname,
					p1->name.len, p1->name.data);
			valid = false;
			break;
		}
	}
	if (!valid) {
		return LD_ERROR;
	}
#endif

	return LD_OK;
}

struct conf *
conf_create(char *filename)
{
	rstatus_t status;
	struct conf *cf;

	cf = conf_open(filename);
	if (cf == NULL) {
		return NULL;
	}

	/* validate configuration file before parsing */
	status = conf_pre_validate(cf);
	if (status != LD_OK) {
		dlog(D_ERRO, "pre validate fail");
		goto error;
	}

	/* parse the configuration file */
	status = conf_parse(cf);
	if (status != LD_OK) {
		dlog(D_ERRO, "parse fail");
		goto error;
	}

	/* validate parsed configuration */
	status = conf_post_validate(cf);
	if (status != LD_OK) {
		dlog(D_ERRO, "post validate fail");
		goto error;
	}

	conf_dump(cf);

	fclose(cf->fh);
	cf->fh = NULL;

	return cf;

error:
	fclose(cf->fh);
	cf->fh = NULL;
	conf_destroy(cf);
	return NULL;
}

void
conf_destroy(struct conf *cf)
{
	while (array_n(&cf->arg) != 0) {
		conf_pop_scalar(cf);
	}
	array_deinit(&cf->arg);

	while (array_n(&cf->pool) != 0) {
		conf_pool_deinit(array_pop(&cf->pool));
	}
	array_deinit(&cf->pool);

	free(cf);
}

char *
conf_set_string(struct conf *cf, struct command *cmd, void *conf)
{
	rstatus_t status;
	uint8_t *p;
	struct string *field, *value;

	p = conf;
	field = (struct string *)(p + cmd->offset);

	if (field->data != CONF_UNSET_PTR) {
		return "is a duplicate";
	}

	value = array_top(&cf->arg);

	status = string_duplicate(field, value);
	if (status != LD_OK) {
		return CONF_ERROR;
	}

	return CONF_OK;
}

char *
conf_set_num(struct conf *cf, struct command *cmd, void *conf)
{
	uint8_t *p;
	int num, *np;
	struct string *value;

	p = conf;
	np = (int *)(p + cmd->offset);

	if (*np != CONF_UNSET_NUM) {
		return "is a duplicate";
	}

	value = array_top(&cf->arg);

	num = nc_atoi(value->data, value->len);
	if (num < 0) {
		return "is not a number";
	}

	*np = num;

	return CONF_OK;
}

