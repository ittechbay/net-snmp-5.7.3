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
#include "can.h"

#include <sys/types.h>			/* See NOTES */
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#include <pthread.h>
#include <semaphore.h>




#define SOL_CAN_RAW (SOL_CAN_BASE + CAN_RAW)



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
	/* 
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
	*/

	return; //.................
	ret = pthread_join(thread, &res);
	if (ret != 0)
	{
		DEBUGMSGTL(("init_ftsSetDo", "pthread_join\n"));
		exit(1);
	}
	
}

extern struct fts_scalar_data_s fts_scalar_data;

struct can_frame * ftsCan_makeScalarFrame(netsnmp_request_info *requests, int type)
{
	struct can_frame *frame;
	
	frame = malloc(sizeof(struct can_frame));

	
	if (frame == NULL)
	{
		DEBUGMSGTL(("ftsCan_makeScalarFrame", "malloc\n"));
		exit(1);
	}

	switch (type)
	{
	case FTS_SET_SCALAR_FRAME_CLK_STATE:
		frame->can_id = FTS_FRAME_CLK_ID;
		frame->can_dlc = 2;
		memcpy(frame->data, (char *)&fts_scalar_data.ftsClkState, sizeof(fts_scalar_data.ftsClkState));
		
		
		break;
	case FTS_SET_SCALAR_FRAME_CLK_MODE:
		break;
		
	}
	return frame;
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
/* 	fts_set_cmd *p;
	
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
	*/
	int ret;
	int s;
	
	ret = pthread_create(NULL, NULL, &ftsSetCmdThread, ftscmd);
	if (ret != 0)
	{
		DEBUGMSGTL(("ftsSetCmd_send", "pthread_create\n"));
		exit(1);
	}
	
}
#define CAN_RAW_RECV_OWN_MSGS 0x4 

// scalar can process: 0 ok; other error
int fts_scalar_set_process(struct can_frame *frame)
{
	int sock;
	struct sockaddr_can addr;
	struct ifreq ifr;
	int ret;
	int recv_own_msgs = 0;//set loop back:  1 enable 0 disable

	sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if(sock < 0) {
		printf("error\n");
		return -1;
	}

	addr.can_family = AF_CAN;
	strcpy(ifr.ifr_name, "can0"); //?? which can should i use


	ret = ioctl(sock, SIOCGIFINDEX, &ifr);  //get index
	if(ret && ifr.ifr_ifindex == 0) 
	{
		printf("Can't get interface index for can0, code= %d, can0 ifr_ifindex value: %d, name: %s\n", ret, ifr.ifr_ifindex, ifr.ifr_name);
		close(sock);
		return -1;
	}

	printf("%s can_ifindex = %x\n",ifr.ifr_name,ifr.ifr_ifindex);
	addr.can_ifindex = ifr.ifr_ifindex;

	//ioctl(sock,SIOCGIFNAME,&ifr);
	//printf("ret = %d can0 can_ifname = %s\n",ret,ifr.ifr_name);	


	setsockopt(sock, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS,&recv_own_msgs, sizeof(recv_own_msgs));
	if (bind(sock,(struct sockaddr*)&addr,sizeof(addr))<0) {
		printf("bind error\n");
		close(sock);
		return -1;
	}

	write(sock, &frame, sizeof(struct can_frame));
	close(sock);
	return 0;
}

int fts_refTable_set_process()
{

}


//other way: create one thread for every snmp set request
void ftsSetCmdThread(void *arg)
{
	fts_set_cmd *cmd;
	netsnmp_request_info *requests, *request;
	
	netsnmp_tdata *table_data;
	struct ftsRefTable_entry *table_entry;


	cmd = (fts_set_cmd *)arg;
	
			
			//process cmd
			if (cmd->type == FTS_SET_CMD_TYPE_SCALAR)
			{
			switch (cmd->scalar_type)
				{
				int ret;
				
				case FTS_SET_CMD_SCALAR_CLK_STATE:
					ret = fts_scalar_set_process(cmd->requests);
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
			else if (cmd->type == FTS_SET_CMD_TYPE_TABLE)
			{
				switch (cmd->table_type)
				{
				int ret;
				netsnmp_table_request_info *table_info;
				
				case FTS_SET_CMD_TABLE_NTP:
					//set table data
					ret = fts_refTable_set_process();
					if (ret != 0)
						break;
					requests = cmd->requests;
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
					table_data = netsnmp_tdata_extract_table(cmd->requests);
					ftsRefTable_data_save(table_data);
	
					// set mib dat file
					break;
				}
			}
			// free p;
			ftsSetCmd_free(cmd);


#if 0
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
#endif
	
}
