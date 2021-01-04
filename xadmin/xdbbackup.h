#ifndef __XDBBACKUP_H_INCLUDED__
#define __XDBBACKUP_H_INCLUDED__

#define AM_QUERY_DBA_PASSWORD_CHANGE \
	"ALTER USER " \
	"\"%s\" " \
	"WITH PASSWORD " \
	"\'%s\' "

#define AM_QUERY_SG_USER_SELECT \
	"SELECT " \
	"	count(*) " \
	"FROM " \
	"	SG_USER"

#define AM_QUERY_ALL_TABLE_SELECT \
	"SELECT " \
	"	tablename " \
	"FROM " \
	"	pg_tables " \
	"WHERE " \
	"	schemaname = 'public' " \
	"ORDER BY tablename"

#define AM_QUERY_SEQ_LIST_SELECT \
	"SELECT " \
	"	relname " \
	"FROM " \
	"	pg_class " \
	"WHERE " \
	"	relkind = 'S' " \
	"ORDER BY relname"

#define AM_PG_DUMP_CMD \
	"su - %s -c \"%s/pg_dump -U %s " \
	"--table=%s %s > " \
	"%s/%s.sql\" "

#define KEYWORD "meta"

int _get_tablelist(void);
int startPGdump(void);

#endif /* __SGDBBACKUP_H_INCLUDED__ */
