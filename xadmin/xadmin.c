#include "xadmin.h"

#define CLEAR \
	fprintf(stderr, "\033c");

extern int xlog_level;

int encryptStr(char *decStr);
int decryptStr(char *encStr);
void mygetch(void);
int sgadmin_start(struct context *ctx);
int network_setup_start(struct context *ctx);
int admin_task_start(struct context *ctx);
int database_task_start(struct context *ctx);
int backup(void);

struct select_char_str_t select_char_str[] = {
	{SELECT_Y_OR_N, "Are you sure you want to select? (y/n) : "},
	{SELECT_APPLY_OR_CALCLE, "Would you like to apply? (a/c) : "},
	{-1, NULL}
};

void mygetch(void)
{
	struct termios oldt, newt;
	int ch __attribute__((unused));
	printf("Press any key and continue");
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

static inline char *get_select_char_str(int code)
{   
	int i;
	for (i = 0; ; i++) {
		if (code == -1) break;
		if (code == select_char_str[i].code) 
			return select_char_str[i].str;
	}
	return NULL;
}

static inline void _get_char(char *c)
{
	char buf[SG_ISTRING_MAXLEN];

	fgets(buf, sizeof(buf), stdin);
	buf[1] = '\0';

	*c = buf[0];

	return ;
}

#if 0
static inline void _get_string(char *c)
{
	char buf[SG_ISTRING_MAXLEN];

	fgets(buf, sizeof(buf), stdin);

	if (strlen(buf) > SG_ISTRING_MAXLEN - 1) {
		printf("len over[%lu]\n", strlen(buf));   
		buf[SG_ISTRING_MAXLEN - 1] = '\0';
	}

	buf[strlen(buf) - 1] = '\0';
	*c = buf[0];

	return ;
}
#endif

static inline void _get_uint32(uint32_t *i)
{
	char buf[SG_UINT32_MAXLEN];

	fgets(buf, sizeof(buf), stdin);

	if (strlen(buf) > SG_UINT32_MAXLEN - 1) {
		printf("len over[%lu]\n", strlen(buf));   
		buf[SG_UINT32_MAXLEN - 1] = '\0';
	}

	buf[strlen(buf) - 1] = '\0';
	*i = atoi(buf);

	return ;
}

static inline void _get_enter(char *buf)
{
	fgets(buf, SG_ISTRING_MAXLEN, stdin);
	buf[strlen(buf) - 1] = '\0';
//	printf("buf:%s\n", buf);

	return ;
}


/*
 * get validated char
 */
static inline int get_vchar(int type, char *c)
{
	while (1) {
		switch (type) {
		case SELECT_Y_OR_N :
			_get_char(c);
			if (*c == 'y' || *c == 'Y') {
				return 1;
			} else if (*c == 'n' || *c == 'N') {
				return -1;
			}
			break;
		case SELECT_APPLY_OR_CALCLE :
			_get_char(c);
			if (*c == 'a' || *c == 'A') {
				return 1;
			} else if (*c == 'c' || *c == 'C') {
				return -1;
			}
			break;
		default : 
//			_get_char(c);
//			if (*c == 'y' || *c == 'Y' || *c == 'n' || *c == 'N' ||
//				*c == 'a' || *c == 'A' || *c == 'c' || *c == 'C') return ;
			break;
		}
	}

	return 0;
}

/*
 * get validated uinit32 
 */
static inline void get_vuint32(int type, uint32_t *i)
{
	while (1) {
		switch (type) {
		case SELECT_VALUE :
			_get_uint32(i);
			if (*i > 0 && *i < 300) return ;
			break;
		default : 
			_get_uint32(i);
			/* maybe... */
			if (*i > 0 && *i < (1 << 16)) return ;
			printf("default read_vint[%d]\n", *i);   
			break;
		}
	}

	return ;
}

/*
 * get validated menu
 */
static inline void select_menu(int type, uint32_t *menu)
{
	while (1) {
		switch (type) {
		case MENU_ADMIN_CONSOLE :
			_get_uint32(menu);
			if ((*menu > 0 && *menu < 5) || (*menu == 99999)) return ;
			break;
		case MENU_NETWORK_SETUP :
			_get_uint32(menu);
			if (*menu > 0 && *menu < 5) return ;
			break;
		case MENU_ADMIN_TASK :
			_get_uint32(menu);
			if (*menu > 0 && *menu < 10) return ;
			break;
		case MENU_DATABASE_TASK :
			_get_uint32(menu);
			if (*menu > 0 && *menu < 5) return ;
			break;
		default : 
			_get_uint32(menu);
			/* maybe... */
			if (*menu > 0 && *menu < (1 << 4)) return ;
			printf("default select_vmenu[%d]\n", *menu);   
			break;
		}
	}

	return ;
}


int default_ip_setup(void)
{
	FILE *fp = fopen(XADMIN_NETWORK_INFO_FILE, "w");
	char c; 
	char ipaddr[SG_ISTRING_MAXLEN], netmask[SG_ISTRING_MAXLEN], gateway[SG_ISTRING_MAXLEN];
	char sshport[SG_ISTRING_MAXLEN], interface[SG_ISTRING_MAXLEN], cmd[512];

	printf("default_ip_setup\n");
	printf("IP address : ");
	_get_enter(ipaddr);
	printf("Netmask : ");
	_get_enter(netmask);
	printf("Default gateway IP address : ");
	_get_enter(gateway);
	printf("SSH port number : ");
	_get_enter(sshport);
	printf("Interface name(default:eth1) : ");
	_get_enter(interface);

	printf("%s", get_select_char_str(SELECT_APPLY_OR_CALCLE));
	int res = get_vchar(SELECT_APPLY_OR_CALCLE, &c);
	if (res == 1) {
		printf("IP=%s\nNETMASK=%s\nGATEWAY=%s\nSSHPORT=%s\nIF=%s\n", ipaddr, netmask, gateway, sshport, interface);
		snprintf(cmd, sizeof(cmd), "IP=%s\n", ipaddr);
		fputs(cmd, fp);
		snprintf(cmd, sizeof(cmd), "NETMASK=%s\n", netmask);
		fputs(cmd, fp);
		snprintf(cmd, sizeof(cmd), "GATEWAY=%s\n", gateway);
		fputs(cmd, fp);
		snprintf(cmd, sizeof(cmd), "SSHPORT=%s\n", sshport);
		fputs(cmd, fp);
		snprintf(cmd, sizeof(cmd), "IF=%s", interface);
		fputs(cmd, fp);

		//system("/sbin/route delete default");
		snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s inet %s netmask %s", interface, ipaddr, netmask);
		system(cmd);

		mygetch();
	} else if (res == -1) {
		printf("Cancel default ip setup.\n");
		mygetch();
	}

	fclose(fp);
	return 0;
}


int network_setup_information(void)
{
	printf("[network_setup_information]\n");
	printf("\n");
	system("ifconfig");
	mygetch();

	return 0;
}

int network_setup_start(struct context *ctx)
{
	uint32_t menu;

	while (1) {
		CLEAR
		printf("SecureGuard Network Setup\n");		
		printf("=================================\n");		
		printf("1) Default IP Address setup\n");
		printf("2) Network setup Information\n");
		printf("3) Prev\n");
		printf("4) Exit\n\n");
		printf("Choice : ");

		select_menu(MENU_NETWORK_SETUP, &menu);
		switch (menu) {
		case 1 :
			default_ip_setup();
			break;
		case 2 :
			network_setup_information();
			break;
		case 3 :
			sgadmin_start(ctx);
			break;
		case 4 :
			exit(0);
			break;
		default : 
			break;
		}
	}

	return 0;
}

static inline int which_number(char *s) 
{
    int len, i;
    len = strlen(s);
    for(i=0;i<len;i++)
        if((s[i]<'0' || s[i]>'9'))
            return -1; 
    return atoi(s);
}

static inline int get_pid(char *str)
{
    DIR     *dp;
    struct  dirent *dir;
    char    buf[100], line[100], tag[100], name[100];
    int     pid;
    FILE    *fp;

    dp = opendir("/proc");
    if(!dp) 
        return -1; 
    
    while((dir = readdir(dp))) {
        pid = which_number (dir->d_name);
        if(pid == -1) 
            continue;

        snprintf(buf, 100, "/proc/%d/status", pid);
        fp = fopen(buf, "r");
        if(fp==NULL)
            continue;

        fgets(line, 1024, fp);
        fclose(fp);

        sscanf(line, "%s %s", tag, name);
        if(!strcmp(name, str)) {
            closedir(dp);
            return pid;
        }   
    }   
    closedir(dp);
    return -1; 
}

int check_all_service(void)
{
	FILE *fp = fopen(XADMIN_PROCLIST, "r");
	char procname[SG_ISTRING_MAXLEN];

	printf("[check all service]\n");

	while (fgets(procname, sizeof(procname), fp) != NULL) {
		if (procname[0] == '#' || procname[0] == '\n') {
			continue;
		}
		procname[strlen(procname)-1] = 0;
		if (get_pid(procname) == -1) {
			printf("SecureGuard %s service ", procname);
			printf("%c[1;31m",27);
			printf("down");
			printf("%c[0m\n",27);
		} else {
			printf("SecureGuard %s service ", procname);
			printf("%c[1;32m",27);
			printf("running");
			printf("%c[0m\n",27);
		}
	}

	fclose(fp);
	mygetch();
	return 0;
}

int sg_service_start(void)
{
	char c;
	char cmd[XADMIN_CMD_MAX_LEN];

	printf("sg service start\n");

	memset(cmd, 0x00, XADMIN_CMD_MAX_LEN);

	printf("%s", get_select_char_str(SELECT_Y_OR_N));

	int res = get_vchar(SELECT_Y_OR_N, &c);

	if (res == 1) {
		printf("Database service start.\n");
		snprintf(cmd, XADMIN_CMD_MAX_LEN, DB_START_CMD, OS_ACCOUNT_NAME, PG_BIN_DIR, PG_BDR_DATA_DIR);
		system(cmd);
		printf("Database service start done.\n");
		mygetch();
	} else {
		printf("Cancel sg service start.\n");
		mygetch();
	}

	return 0;
}

int sg_service_stop(void)
{
	char c;
	char cmd[XADMIN_CMD_MAX_LEN];

	printf("sg service stop\n");

	memset(cmd, 0x00, XADMIN_CMD_MAX_LEN);

	printf("%s", get_select_char_str(SELECT_Y_OR_N));

	int res = get_vchar(SELECT_Y_OR_N, &c);

	if (res == 1) {
		printf("Database service stop.\n");
		snprintf(cmd, XADMIN_CMD_MAX_LEN, DB_STOP_CMD, OS_ACCOUNT_NAME, PG_BIN_DIR);
		system(cmd);
		printf("Database service stop done.\n");
		mygetch();
	} else {
		printf("Cancel sg service stop.\n");
		mygetch();
	}

	return 0;
}

int system_shutdown(void)
{
	char c, cmd[SG_ISTRING_MAXLEN];

	printf("[system shutdown]\n");

	printf("%s", get_select_char_str(SELECT_Y_OR_N));

	int res = get_vchar(SELECT_Y_OR_N, &c);
	if (res == 1) {
		snprintf(cmd, sizeof(cmd), "poweroff");
		system(cmd);
	} else if (res == -1) {
		printf("Cancel system shutdown.\n");
		mygetch();
	}

	return 0;
}

int system_reboot(void)
{
	char c, cmd[SG_ISTRING_MAXLEN];

	printf("[system reboot]\n");

	printf("%s", get_select_char_str(SELECT_Y_OR_N));

	int res = get_vchar(SELECT_Y_OR_N, &c);
	if (res == 1) {
		snprintf(cmd, sizeof(cmd), "reboot");
		system(cmd);
	} else if (res == -1) {
		printf("Cancel system reboot.\n");
		mygetch();
	}

	return 0;
}

int change_admin_passwd(void)
{
	char c;
	char passwd[SG_ISTRING_MAXLEN];
	char confirm_passwd[SG_ISTRING_MAXLEN];
	char cmd[SG_ISTRING_MAXLEN];

	printf("[change admin password]\n");
	printf("Change password : ");
	_get_enter(passwd);
	printf("Confirm change password : ");
	_get_enter(confirm_passwd);

	int ret = strcmp(passwd, confirm_passwd);

	if (ret == 0) {
		printf("%s", get_select_char_str(SELECT_APPLY_OR_CALCLE));

		int res = get_vchar(SELECT_APPLY_OR_CALCLE, &c);
		
		if (res == 1) {
			snprintf(cmd, sizeof(cmd), "echo admin:%s | chpasswd;", passwd);
			system(cmd);
			printf("Admin password change complete\n");
			mygetch();
		} else if (res == -1) {
			printf("Cancel change admin password.\n");
			mygetch();
		}

	} else {
		printf("Passwords do not match");
		mygetch();
	}

	return 0;
}

#if 0
int change_database_passwd(void)
{
	PGconn *conn = NULL;
	PGresult *res = NULL;
	int records = 0;
//	int fields = 0;
	char query[XADMIN_CMD_MAX_LEN];
	char c;
	char passwd[SG_ISTRING_MAXLEN], confirm_passwd[SG_ISTRING_MAXLEN];

	printf("[change database password]\n");
	printf("Change password : ");
	_get_enter(passwd);
	printf("Confirm change password : ");
	_get_enter(confirm_passwd);

	int ret = strcmp(passwd, confirm_passwd);

	if (ret == 0) {
		printf("%s", get_select_char_str(SELECT_APPLY_OR_CALCLE));

		int result = get_vchar(SELECT_APPLY_OR_CALCLE, &c);
		
		if (result == 1) {
			memset(query, 0x00, sizeof(query));
			snprintf(query, sizeof(query), AM_QUERY_DBA_PASSWORD_CHANGE,
					OS_ACCOUNT_NAME, passwd);

			conn = pg_conf_connect(KEYWORD);
			if (conn == NULL) {
				fprintf(stderr, "Connection to database failed: %s\n", 
						PQerrorMessage(conn));
				mygetch();
				return records;
			}

			if ((res = pg_query(conn, query)) == NULL) {
				fprintf(stderr, "query null: %s\n", PQerrorMessage(conn));
				pg_disconnect(conn);
				mygetch();
				return records;
			}

			PQclear(res);
			pg_disconnect(conn);
			printf("Database password change complete\n");
			mygetch();

		} else if (result == -1) {
			printf("Cancel change database password.\n");
			mygetch();
		}

	} else {
		printf("Passwords do not match\n");
		mygetch();
	}

	return 0;
}
#endif

int admin_task_start(struct context *ctx)
{
	uint32_t menu;

	while (1) {
		CLEAR
		printf("SecureGuard Admin Task\n");		
		printf("=================================\n");
		printf("1) Check all service.\n");
		printf("2) Startup  SecureGuard service.\n");
		printf("3) Shutdown SecureGuard service.\n");
		printf("4) Shutdown system.\n");
		printf("5) Restart  system.\n");
		printf("6) Change admin password.\n");
		printf("7) Change database password.\n");
		printf("8) Prev\n");
		printf("9) Exit Admin\n\n");
		printf("Choice : ");

		select_menu(MENU_ADMIN_TASK, &menu);
		switch (menu) {
		case 1 :
			check_all_service();
			break;
		case 2 :
			sg_service_start();
			break;
		case 3 :
			sg_service_stop();
			break;
		case 4 :
			system_shutdown();
			break;
		case 5 :
			system_reboot();
			break;
		case 6 :
			change_admin_passwd();
			break;
		case 7 :
//			change_database_passwd();
			break;
		case 8 :
			sgadmin_start(ctx);
			break;
		case 9 :
			exit(0);
			break;
		default : 
			break;
		}
	}

	return 0;
}

#if 0
int backup(void)
{
	char c;

	printf("[Database backup]\n");
	printf("%s", get_select_char_str(SELECT_Y_OR_N));

	int res = get_vchar(SELECT_Y_OR_N, &c);

	if (res == 1) {
		if (_get_tablelist() == 1) {
			dlog(D_ERRO, "Fail to get table list");
			mygetch();
			return 1;
		}
		if (startPGdump() == 1) {
			dlog(D_ERRO, "Fail to pg_dump command");
			mygetch();
			return 1;
		}
	} else {
		printf("Cancel database backup.\n");
		mygetch();
		return 0;
	}
	mygetch();
	return 0;
}
#endif

int database_task_start(struct context *ctx)
{
	uint32_t menu;

	while (1) {
		CLEAR
		printf("SecureGuard Database task\n");		
		printf("=================================\n");
		printf("1) Backup all tables\n");
		printf("2) Select table to backup\n");
		printf("3) Prev\n");
		printf("4) Exit Database task\n\n");
		printf("Choice : ");

		select_menu(MENU_DATABASE_TASK, &menu);
		switch (menu) {
		case 1 :
//			backup();
			break;
		case 2 :
//			_get_tablelist();
			break;
		case 3 :
			sgadmin_start(ctx);
			break;
		case 4 :
			exit(0);
			break;
		default : 
			break;
		}
	}

	return 0;
}

void v_info(void)
{
#if 0
	FILE *fp = NULL;
	char line[SG_ISTRING_MAXLEN];
	
	fp = fopen(VERSION, "r");
	if (fp == NULL) {
		dlog(D_DBUG, "[%s] file is not exist\n", VERSION);
		return ;
	}
	while (!feof(fp)) {
		fscanf(fp, "%s", line);
	//	printf("line = %s", line);
		if (line[0] == '#' || line[0] == '\n') {
			continue;
		}
	}
	printf("%c[0;31m",27);
	printf("%s", line);
	printf("%c[0m",27);
	fclose(fp);
#endif
	return ;
}

int sgadmin_start(struct context *ctx)
{
	uint32_t menu;

	while (1) {
		CLEAR
		printf("SecureGuard Ver ");
		v_info();
		printf(" Console Admin\n");
		printf("====================================\n");		
		printf("1) Network setup\n");
		printf("2) Admin task\n");
		printf("3) Database task\n");
		printf("4) Exit\n\n");
		printf("Choice : ");

		select_menu(MENU_ADMIN_CONSOLE, &menu);
		switch (menu) {
		case 1 :
			network_setup_start(ctx);
			break;
		case 2 :
			admin_task_start(ctx);
			break;
		case 3 :
			database_task_start(ctx);
			break;
		case 4 :
			exit(0);
			break;
		default : 
			break;
		}
	}

	return 0;
}

int encryptStr(char *decStr)
{
	sgnseed_t	seed_ctx;
	char		enc_data[XADMIN_CMD_MAX_LEN];
	
	memset(enc_data, 0x00, XADMIN_CMD_MAX_LEN);

	init_seed(&seed_ctx);
	sgnseed_encrypt(&seed_ctx, decStr, enc_data);

	return 0;
}

int decryptStr(char *encStr)
{
	sgnseed_t	seed_ctx;
	char		dec_data[XADMIN_CMD_MAX_LEN];
	
	memset(dec_data, 0x00, XADMIN_CMD_MAX_LEN);

	init_seed(&seed_ctx);
	sgnseed_encrypt(&seed_ctx, encStr, dec_data);

	return 0;
}

static int daemon_get_conf(struct daemon_conf *conf)
{
    int  log_level = 0;
    FILE *fd = NULL;
    char tag[CONF_TAG_LEN];
    char value[CONF_VALUE_LEN];

    if ((fd = fopen(XADMIN_LOG_LEVEL_CONF_FILE, "r")) == NULL) {
        dlog(D_ERRO, "%s conf file open error [%s]", DAEMON_NAME, XADMIN_LOG_LEVEL_CONF_FILE);
        return 0;
    }   

    while (!feof(fd)) {
        memset(tag, 0x00, CONF_TAG_LEN);
        memset(value, 0x00, CONF_VALUE_LEN);

        fscanf(fd,"%s %s\n", tag, value);

        if (strcmp(tag, DAEMON_NAME) == 0) {
            conf->log_level = atoi(value);
            break;
        }   
    }   

    if (!conf->log_level)
        conf->log_level = LOG_DEFAULT_LEVEL;

    fclose(fd);

    return log_level;
}

struct context *
ctx_create()
{
	rstatus_t rstatus;
	struct context *ctx;

	ctx = (struct context *)malloc(sizeof(struct context));
	if (ctx == NULL) {
		dlog(D_ERRO, "ctx malloc fail");
		return NULL;
	}

	ctx->cf = NULL;
	ctx->dp = NULL;

	ctx->conf_filename = AM_CONF_FILE;
	ctx->cf = conf_create(ctx->conf_filename);
	if (ctx->cf == NULL) {
		dlog(D_ERRO, "conf create fail");
		return NULL;
	}

	ctx->dp = xdb_pool_create(&ctx->cf->pool);
	if (ctx->dp == NULL) {
		dlog(D_ERRO, "xdb_pool dp[%p] NULL", ctx->dp);
		return NULL;
	}

	rstatus = xdb_pool_preconnect(ctx->dp);
	if (rstatus != LD_OK) {
		dlog(D_ERRO, "xdb_pool preconnect fail");
		return NULL;
	}

#if 0
	ctx->biq = binaray_info_create(ctx);
	if (ctx->biq == NULL) {
		dlog(D_ERRO, "binaray info create fail");
		return NULL;
	}

	ctx->backup = xbackup_create(ctx);
	if (rstatus != LD_OK) {
		dlog(D_ERRO, "xbackup create fail");
		return NULL;
	}
#endif

	dlog(D_ERRO, "xadmin context create success");

	return ctx;
}

int main()
{
	struct context *ctx;
	struct daemon_conf conf;

	/* init xlog */
	memset(&conf, 0x00, sizeof(struct daemon_conf));

#if 0
	if (daemon_get_conf(&conf)) {
		if (conf.log_level)
			xlog_level = dlog_get(conf.log_level);
	}
#endif
	xlog_level = dlog_get(D_DBUG);
	dlog_init(XADMIN_LOG_FILE, LOG_DEFAULT_SIZE_100M,
		LOG_DEFAULT_ROTATE_CNT, &xlog_level);

	dlog(D_CRIT, "");
	dlog(D_CRIT, "%s START", DAEMON_NAME);

	ctx = ctx_create();
	if (ctx == NULL) {
		dlog(D_ERRO, "context create fail");
		return -1;
	}

	/* init sgadmin */
	sgadmin_start(ctx);

	return 0;
}
