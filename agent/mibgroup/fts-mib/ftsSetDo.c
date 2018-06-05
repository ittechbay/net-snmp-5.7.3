#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>


#include "fts_cfg.h"

#include "ftsSetDo.h"
#include "fts_scalar.h"
#include "ftsRefTable.h"
#include "ftsNtpTable.h"
#include "ftsPtpTable.h"
#include "ftsTimingTable.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#include <pthread.h>
#include <semaphore.h>







fts_set_cmd *ftsCmdHead;
pthread_mutex_t ftsSetMutex = PTHREAD_MUTEX_INITIALIZER;

struct thread_info *tinfo;

/** Initializes the ftsSetDo module */
void init_ftsSetDo(void)
{
	int ret, ret1;
	pthread_t thread;
	pthread_t thread1;
	void *res;
	
	ftsCmdHead = NULL;
	
	//create pthread ftsSetCmdThread;
	ret = pthread_create(&thread, NULL, &ftsSetCmdThread, NULL);
	if (ret != 0)
	{
		DEBUGMSGTL(("init_ftsSetDo", "pthread_create\n"));
		exit(1);
			
	}

	
	ret1 = pthread_create(&thread1, NULL, &ftsSetCmdThread, NULL);
	if (ret != 0)
	{
		DEBUGMSGTL(("init_ftsSetDo", "pthread_create\n"));
		exit(1);
			
	}

	return; //.................
	ret = pthread_join(thread, &res);
	if (ret != 0)
	{
		DEBUGMSGTL(("init_ftsSetDo", "pthread_join\n"));
		exit(1);
	}
	
}

fts_set_cmd * ftsSetCmd_make_scalar(netsnmp_request_info *requests, int type)
{
	fts_set_cmd *p = malloc(sizeof(fts_set_cmd));
	if (p == NULL)
	{
		DEBUGMSGTL(("ftsSetCmd_make_scalar", "malloc\n"));
		exit(1);
	}
	p->next = NULL;
	p->type = FTS_SET_CMD_TYPE_SCALAR;
	p->scalar_type = type;
	//p->scalar_data = data;
	
	return p;
}



fts_set_cmd * ftsSetCmd_make_refTable(netsnmp_request_info *requests)
{
	fts_set_cmd *p = malloc(sizeof(fts_set_cmd));

	if (p == NULL)
	{
		DEBUGMSGTL(("ftsSetCmd_make_ref", "malloc\n"));
		exit(1);
	}
	p->next = NULL;
	p->requests = requests;
	p->type = FTS_SET_CMD_TYPE_TABLE;
	p->table_type = FTS_SET_CMD_TABLE_REF;
	
/*	
	p->table_data.ref.ftsRefState;
	strncpy(p->table_data.ref.ftsRefDescr, ftsRefDescr, ftsRefDescr_len);
	p->table_data.ref.ftsRefDescr[ftsRefDescr_len] = '\0';
	p->table_data.ref.ftsRefDescr_len = ftsRefDescr_len;
	p->table_data.ref.ftsRefGrade = ftsRefGrade;
*/
	return p;
}


void ftsSetCmd_free(fts_set_cmd *cmd)
{
// 
	if (cmd != NULL)
		free((void *)cmd);
}

void ftsSetCmd_send(fts_set_cmd *ftscmd)
{
	fts_set_cmd *p;
	
	ftscmd->next = NULL;

	pthread_mutex_lock(&ftsSetMutex);		
	p = ftsCmdHead;
	if (ftsCmdHead == NULL)
		ftsCmdHead = ftscmd;
	else
	{
		while(p->next!=NULL)
		{
			p=p->next;
		}
		p->next = ftscmd;
	}
	pthread_mutex_unlock(&ftsSetMutex);		
	
}

// scalar can process: 0 ok; other error
int fts_scalar_set_process()
{
	return 0;
}

int fts_refTable_set_process()
{

}

extern struct fts_scalar_data_s fts_scalar_data;

//other way: create one thread for every snmp set request
void ftsSetCmdThread()
{
	fts_set_cmd *p;
	netsnmp_tdata *table_data;
	struct ftsRefTable_entry *table_entry;

	//hasn't consider access critical section protection: ftsCmdHead
	while(1)
	{
		netsnmp_request_info *requests, *request;

		p = ftsCmdHead;	
		if (p == NULL)
		{
			//sleep(1);
			continue; // should use semaphore
		}
		
		
		//process cmd
		if (p->type == FTS_SET_CMD_TYPE_SCALAR)
		{
		switch (p->scalar_type)
			{
			int ret;
			
			case FTS_SET_CMD_SCALAR_CLK_STATE:
				ret = fts_scalar_set_process(p->requests);
				if (ret != 0)
					break;
				
				fts_scalar_data.ftsClkState = *(requests->requestvb->val.integer);
				fts_scalar_save();
				
			break;
			case FTS_SET_CMD_SCALAR_CLK_MODE:
				//ret = fts_scalar_set_process();
				//if (ret == 0)
				//{
				//	fts_scalar_set_var(VAR_FTS_REF_CURRENT, p->scalar_data);
				//	fts_scalar_save();
				//}
			break;
			}
		}
		else if (p->type == FTS_SET_CMD_TYPE_TABLE)
		{
			switch (p->table_type)
			{
			int ret;
			netsnmp_table_request_info *table_info;
			
			case FTS_SET_CMD_TABLE_NTP:
				//set table data
				ret = fts_refTable_set_process();
				if (ret != 0)
					break;
				requests = p->requests;
				for (request=requests; request; request=request->next) {
					if (request->processed)
						continue;
				
					table_entry = (struct ftsRefTable_entry *)
									  netsnmp_tdata_extract_entry(request);
					table_info	=	  netsnmp_extract_table_info( request);
				
					switch (table_info->colnum) {
					case COLUMN_FTSREFSTATE:
						table_entry->ftsRefState	 = *request->requestvb->val.integer;
						break;
					case COLUMN_FTSREFDESCR:
						strcpy(table_entry->ftsRefDescr, request->requestvb->val.string);
						table_entry->ftsRefDescr_len = strlen(table_entry->ftsRefDescr);
						break;
					case COLUMN_FTSREFGRADE:
						table_entry->ftsRefGrade = *request->requestvb->val.integer;
						break;
					}
				}
				table_data = netsnmp_tdata_extract_table(p->requests);
				ftsRefTable_data_save(table_data);

				// set mib dat file
				break;
			}
		}
		pthread_mutex_lock(&ftsSetMutex);		
		if (ftsCmdHead->next == NULL)
		{
			ftsCmdHead = NULL;
		}
		else
		{
			ftsCmdHead = ftsCmdHead->next;
		}
		// free p;
		ftsSetCmd_free(p);
		pthread_mutex_unlock(&ftsSetMutex);
	}
	
}
