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

#define FTS_BOARD_TYPE_NTP 0
#define FTS_BOARD_TYPE_PTP 1
#define FTS_BOARD_TYPE_10M 2
#define FTS_BOARD_TYPE_1PPS 3
#define FTS_BOARD_TYPE_IRIG_B 4
#define FTS_BOARD_TYPE_NULL 5


int fts_slot_state[3];

void init_ftsCan(void)
{


}

int fts_can_mk_send_id(unsigned int slot, unsigned int board_type)
{
	unsigned int byte0, byte1, byte2, byte3;

	byte0 = 0;
	byte1 = 0xF0;
	byte2 = (slot<<4)|board_type;
	byte3 = 0x00011000;

	return (byte3<<24)|(byte2<<16)|(byte1<<8)|byte0;
}

int fts_can_mk_recv_id(int slot, int board_type)
{
	
}

int fts_can_id_to_slot(int canid)
{
}

int fts_can_id_to_board_type(int canid)
{

}

int fts_can_send_and_recv(struct can_frame *send_frame, struct can_frame *recv_frame)
{
	int sock;
	struct sockaddr_can addr;
	struct ifreq ifr;
	int ret;
	int recv_own_msgs = 0; //set loop back:  1 enable 0 disable

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

	setsockopt(sock, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof(recv_own_msgs));
	if (bind(sock,(struct sockaddr*)&addr,sizeof(addr))<0) {
		printf("bind error\n");
		close(sock);
		return -1;
	}

	ret = write(sock, (void *)send_frame, sizeof(struct can_frame));
	///////////////////////
	
	struct timeval tv;
	fd_set readfds;
	
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);
	// don't care about writefds and exceptfds:
	ret = select(sock+1, &readfds, NULL, NULL, &tv);
	if (ret <=0)
	{
		close(sock);
		return -1;
	}
	ret = read(sock, (void *)recv_frame, sizeof(struct can_frame));

	

	close(sock);
	return 0;
}

struct can_frame * fts_can_make_scalar_frame_ptp_poll(int canid)
{
	struct can_frame *frame;
	char *p1;
	char *p2;

	int test_size = sizeof (struct in_addr);  //test size
	
	frame = malloc(sizeof(struct can_frame));
	if (frame == NULL)
	{
		DEBUGMSGTL(("ftsCan_makeScalarFrame", "malloc\n"));
		exit(1);
	}

	frame->can_id = canid;
	frame->can_dlc = 2;
	frame->data[0] = FTS_CAN_FRAME_CMD_CODE_PTP_SET_MODE;

	return frame;
}

void fts_can_parse_scalar_frame_ptp_poll_reply(struct can_frame *frame, int *flag)
{
	if (frame->data[0] != FTS_CAN_FRAME_CMD_CODE_PTP_SET_MODE)
		DEBUGMSGTL(("fts_can_parse_scalar_frame_ptp_poll_reply", "FTS_CAN_FRAME_CMD_CODE_PTP_SET_MODE\n"));
	*flag = (int)frame->data[1];
}


struct can_frame * fts_can_make_scalar_frame_ptp_get_state(int canid)
{
	struct can_frame *frame;
	char *p1;
	char *p2;
	int test_size = sizeof (struct in_addr);  //test size
	
	frame = malloc(sizeof(struct can_frame));
	if (frame == NULL)
	{
		DEBUGMSGTL(("ftsCan_makeScalarFrame", "malloc\n"));
		exit(1);
	}

	frame->can_id = canid;
	frame->can_dlc = 1;
	frame->data[0] = FTS_CAN_FRAME_CMD_CODE_PTP_GET_STATE;

	return frame;
}

void fts_can_parse_scalar_frame_ptp_get_state_reply(struct can_frame *frame, int *mode, int *synRate, int *DelayRate)
{
	if (frame->data[0] != FTS_CAN_FRAME_CMD_CODE_PTP_GET_STATE_REPLY)
		DEBUGMSGTL(("fts_can_parse_scalar_frame_ptp_poll_reply", "FTS_CAN_FRAME_CMD_CODE_PTP_SET_MODE\n"));
	*mode = (int)frame->data[1];
	*synRate = (int)frame->data[2];
	*DelayRate = (int)frame->data[3];
}

struct can_frame * fts_can_make_scalar_frame_ptp_set_mode(int canid, int mode)
{
	struct can_frame *frame;
	char *p1;
	char *p2;

	int test_size = sizeof (struct in_addr);  //test size
	
	frame = malloc(sizeof(struct can_frame));
	if (frame == NULL)
	{
		DEBUGMSGTL(("ftsCan_makeScalarFrame", "malloc\n"));
		exit(1);
	}

	frame->can_id = canid;
	frame->can_dlc = 2;
	frame->data[0] = FTS_CAN_FRAME_CMD_CODE_PTP_SET_MODE;
	frame->data[1] = (char)mode; 

	return frame;
}


struct can_frame * fts_can_make_scalar_frame_ptp_set_param(int canid, int syncRate, int delayRate)
{
	struct can_frame *frame;
	char *p1;
	char *p2;

	int test_size = sizeof (struct in_addr);  //test size
	
	frame = malloc(sizeof(struct can_frame));
	if (frame == NULL)
	{
		DEBUGMSGTL(("ftsCan_makeScalarFrame", "malloc\n"));
		exit(1);
	}

	frame->can_id = canid;
	frame->can_dlc = 3;
	frame->data[0] = FTS_CAN_FRAME_CMD_CODE_PTP_SET_PARAM;
	frame->data[1] = (char)syncRate; 
	frame->data[2] = (char)delayRate; 

	return frame;
}



struct can_frame * fts_can_make_scalar_frame_ptp_set_ip_p2(int canid, struct in_addr mask, struct in_addr gateway)
{
	struct can_frame *frame;
	char *p1;
	char *p2;

	int test_size = sizeof (struct in_addr);  //test size
	
	frame = malloc(sizeof(struct can_frame));
	if (frame == NULL)
	{
		DEBUGMSGTL(("ftsCan_makeScalarFrame", "malloc\n"));
		exit(1);
	}

	frame->can_id = canid;
	frame->can_dlc = 8;
	frame->data[0] = FTS_CAN_FRAME_CMD_CODE_PTP_SET_IP_P2;
	p1 = (char *)&mask;
	frame->data[1] = p1[3]; 

	p2 = (char *)&gateway;
	frame->data[2] = p2[0]; 
	frame->data[3] = p2[1]; 
	frame->data[4] = p2[2]; 
	frame->data[5] = p2[3]; 

	return frame;
}

struct can_frame * fts_can_make_scalar_frame_ptp_set_multicast_master(int canid, struct in_addr mask, struct in_addr gateway)
{
	struct can_frame *frame;
	char *p1;
	char *p2;

	int test_size = sizeof (struct in_addr);  //test size
	
	frame = malloc(sizeof(struct can_frame));
	if (frame == NULL)
	{
		DEBUGMSGTL(("ftsCan_makeScalarFrame", "malloc\n"));
		exit(1);
	}

		frame->can_id = canid;
		frame->can_dlc = 8;
		frame->data[0] = FTS_CAN_FRAME_CMD_CODE_PTP_SET_IP_P2;
		p1 = (char *)&mask;
		frame->data[1] = p1[3]; 

		p2 = (char *)&gateway;
		frame->data[2] = p2[0]; 
		frame->data[3] = p2[1]; 
		frame->data[4] = p2[2]; 
		frame->data[5] = p2[2]; 
		
	return frame;
}

void fts_can_make_ref_table_frame()
{

}

void fts_can_make_ntp_table_frame()
{

}

void fts_can_make_ptp_table_frame()
{

}

void fts_can_get_scalar_int()
{
	
}

void fts_can_get_scalar_string()
{
	
}

void fts_can_get_table_ntp()
{
	
}



