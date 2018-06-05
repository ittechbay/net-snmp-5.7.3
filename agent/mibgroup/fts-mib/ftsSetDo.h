
#ifndef FTSSETDO_H
#define FTSSETDO_H

typedef struct fts_set_cmd_s{
	struct fts_set_cmd_s *next;
	//used int get table data
	netsnmp_request_info *requests;
	int type;
	int scalar_type;
	int table_type;

/*	
	void *scalar_data; // all scalar type should be defined this way(int or string): refine fts_scalar_data_s
	int scalar_len;
	int table_type;
	union table_data {
	struct ftsRefTable_entry ref;
	struct ftsNtpTable_entry ntp;
	//...
	}
*/
} fts_set_cmd;

/* function declarations */
void init_ftsSetDo(void);
void ftsSetCmdThread();


#define FTS_SET_CMD_TYPE_SCALAR 1
#define FTS_SET_CMD_TYPE_TABLE 2



#define FTS_SET_CMD_SCALAR_CLK_STATE 1
#define FTS_SET_CMD_SCALAR_CLK_MODE 2






#define FTS_SET_CMD_TABLE_REF 1
#define FTS_SET_CMD_TABLE_NTP 2

       
#endif /* FTSSETDO_H */
