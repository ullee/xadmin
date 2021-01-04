#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <dirent.h>

#include "xdb/xpg.h"

#include "libx/xlog.h"
#include "xadmin.h"
#include "xdbbackup.h"

PGconn *conn;

bool conn_pgdb(void)
{
	if((conn = pg_conf_connect(KEYWORD)) == NULL) {
		dlog(D_ERRO, "AM Postgres connection config is NULL");
		return false;
	}
	return true;
}

int _get_tablelist(void)
{
	PGresult *res = NULL;
	int records = 0;
	int fields = 0;
	char query[XADMIN_CMD_MAX_LEN];
	char result[XADMIN_CMD_MAX_LEN];
	FILE *fp = fopen(XADMIN_TABLE_LIST, "w");

	dlog(D_DBUG, "Get table list start.");

	memset(query, 0x00, sizeof(query));
	snprintf(query, sizeof(query), AM_QUERY_ALL_TABLE_SELECT);

	if((conn == NULL) || (PQstatus(conn) != CONNECTION_OK)) {
		if(!conn_pgdb()) {
			dlog(D_ERRO, "AM Posgtres connection to database: NOK");
			return 1;
		}
	}

	if((res = pg_query(conn, query)) == NULL) {
		fprintf(stderr, "Query NULL: %s\n", PQerrorMessage(conn));
		dlog(D_ERRO, "[%s] query is NULL", AM_QUERY_ALL_TABLE_SELECT);
		pg_disconnect(conn);
		return 1;
	}

	records = PQntuples(res);
	fields = PQnfields(res);

	if(records == 0) {
		fprintf(stderr, "Query Result NULL: %s\n", PQerrorMessage(conn));
		dlog(D_ERRO, "Query result [%s] is NULL", res);
		PQclear(res);
		pg_disconnect(conn);
		return 1;
	}

	char ret_put[XADMIN_CMD_MAX_LEN];
	memset(ret_put, 0x00, XADMIN_CMD_MAX_LEN);

	int i = 1;
	for(fields = 0; fields < records; fields++) {
		strcpy(result, PQgetvalue(res, fields, 0));
		snprintf(ret_put, XADMIN_CMD_MAX_LEN, "%s\n", result);
		dlog(D_DBUG, "[count : %d] [%s]", i, result);
		fputs(ret_put, fp);
		i++;
	}

	dlog(D_DBUG, "Get table list done.");
	fclose(fp);
	PQclear(res);
	pg_disconnect(conn);
	return 0;
}

int startPGdump(void)
{
	char cmd[XADMIN_CMD_MAX_LEN];
	char buf[XADMIN_CMD_MAX_LEN];
	FILE *fp = NULL;
	DIR	 *dir = NULL;
	char *DB_BACKUP_DIR = XADMIN_DB_BAK_DIR;
	char *pStr;

	if ((fp =fopen(XADMIN_TABLE_LIST, "r")) == NULL) {
		dlog(D_ERRO, "File open error [%s]", XADMIN_TABLE_LIST);
		return 1;
	}

	if((conn == NULL) || (PQstatus(conn) != CONNECTION_OK)) {
		if(!conn_pgdb()) {
			dlog(D_ERRO, "AM Posgtres connection to database: NOK");
			return 1;
		}
	}

	dlog(D_DBUG, "Database backup start.");
	memset(cmd, 0x00, sizeof(cmd));

	/* pre-task */
	dir = opendir(DB_BACKUP_DIR);
	//if (ENOENT == errno) {
	if (dir == NULL) {
		dlog(D_DBUG, "Backup directory not exsit. mkdir %s", cmd);
		snprintf(cmd, sizeof(cmd), "mkdir %s && chown %s:%s %s", 
				DB_BACKUP_DIR, OS_ACCOUNT_NAME, OS_ACCOUNT_NAME, DB_BACKUP_DIR);
		system(cmd);
	}
	closedir(dir);

	int i = 1;
	while(!feof(fp)) {
		pStr = fgets(buf, sizeof(buf), fp);
		buf[strlen(buf) - 1] = '\0';
		if (pStr == NULL)
			continue;

		//printf("[%d] %s\n", i,pStr);
		/* pg dump command */
		snprintf(cmd, sizeof(cmd), AM_PG_DUMP_CMD,
			OS_ACCOUNT_NAME, PG_BIN_DIR, PG_DB_USER, pStr, PG_DB_NAME, DB_BACKUP_DIR, pStr);
		dlog(D_DBUG, "[count : %d] %s", i, cmd);
		system(cmd);
		i++;
	}

	dlog(D_DBUG, "Success backup count : [%d]",i-1);
	dlog(D_DBUG, "Database backup done.");
	fclose(fp);	
	pg_disconnect(conn);
	return 0;
}
