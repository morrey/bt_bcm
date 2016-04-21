#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/poll.h>

#include "bta_api.h"
#include "bta_gps_api.h"
#include "btm_api.h"
#include "bta_sys.h"

const char * SOCKET_PATH = "/data/gps/brcm_gps_unix_socket";

UINT8 tail_8957[];
char unk_712[511];
int taillen_8958;
int hservSocket, clientfd;
int ctlsocket = -1;
pthread_t  ServThreadID;
bool bReadyToQuit, bExit;

const int iLevelMessages = 1; 

void bta_gps_cback() {}

const char *getUnixSocketPath()
{
  if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "[GPS] Unix Socket Path %s\n", SOCKET_PATH); 
  return SOCKET_PATH;
}

int receiveIncomingData(int fd)
{
  int i; // r4@4
  int v3; // r8@9
  int v4; // r3@9
  size_t v5; // r4@9
  char *v6; // r5@9
  char *v7; // r11@10
  char *v8; // r7@10
  UINT8 v9; // r4@10
  uint8_t *v10; // r12@10
  int v11; // t1@11
  uint8_t *v12; // ST00_4@12
  int result; // r0@17
  //int *v14; // [sp+4h] [bp-23Ch]@1
  char src[256]; // [sp+14h] [bp-22Ch]@6
  UINT8 v16; // [sp+114h] [bp-12Ch]@12
  uint8_t v17[3]; // [sp+115h] [bp-12Bh]@10

  while ( true )
  {
    v6 = src;
    result = recv(fd, src, 0xFFu, 0);
    v3 = result;
    if ( result <= 0 ) break;
    if ( taillen_8958 )
    {
      for ( i = 0; (uint8_t)src[i] != 252; ++i );
      if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "tail data length = %d, new data length = %d\n");
      v3 = v3 - i - 1;
      v4 = taillen_8958 + 1 + i;
      v5 = i + 1;
      tail_8957[0] = v4;
      memcpy(&tail_8957[taillen_8958 + 1], src, v5);
      taillen_8958 = 0;
      v6 = &src[v5];
      BTM_VendorSpecificCommand(64649, (tail_8957[0] + 1), tail_8957, bta_gps_cback);
    }
    v7 = v6;
    v8 = v6;
    v9 = 0;
    v10 = v17;
    while ( v8 - v7 < v3 )
    {
      v11 = (uint8_t)*v8++;
      ++v9;
      if ( v11 == 252 )
      {
        v12 = v10;
        __memcpy_chk(v10, v6, v9, 254);
        v16 = v9;
        BTM_VendorSpecificCommand(64649, (v9 + 1), &v16, bta_gps_cback);
        v10 = v12;
        v6 = v8;
        v9 = 0;
      }
    }
    if ( v9 )
    {
      __memcpy_chk(&unk_712, v6, v9, 511);
      taillen_8958 = v9;
      if ( btif_trace_level > iLevelMessages )
        LogMsg(1283, "tail data length = %d\n", v9);
    }
  }
  return result;
}

void * wait_4_command_thread()
{
  uint8_t *v1; // r4@3
  unsigned int v2; // r6@3
  bool v3; // cf@3
  bool v4; // zf@3
  uint8_t *v5; // r6@3
  int v6; // r2@6
  int *v7; // r3@15
  int rcvLen; // r2@19
  const char *v9; // r1@11
  int result; // r0@34
  int iSizePoll = 16; // [sp+0h] [bp-50h]@1
  struct pollfd fds[2]; // [sp+4h] [bp-4Ch]@12
  int v13; // [sp+Ch] [bp-44h]@12
  int v14; // [sp+10h] [bp-40h]@12
  int v15; int v11 = 16;
  struct sockaddr addr; 
  socklen_t peer_addr_size = 16;
  
  listen(hservSocket, 1);
  while ( true )
  {
LABEL_31:
    if ( bExit )
    {
LABEL_10:
      if ( btif_trace_level > iLevelMessages )
      {
        v9 = "wait_4_command_thread exit\n";
        goto LABEL_33;
      }
      goto LABEL_34;
    }
    if ( hservSocket <= 0 ) goto LABEL_34;

    if ( btif_trace_level > iLevelMessages)  LogMsg(1283, "TCP server accept ...\n");
    clientfd = accept(hservSocket, &addr, &peer_addr_size);
    if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "TCP server accept clientfd (%d)\n", clientfd);
    if ( clientfd != -1 )  break;
    if ( bExit )  goto LABEL_10;
  }
  do
  {
    fds[0].events = 73;
    fds[0].revents = 0;
    fds[0].fd = clientfd;
    
    fds[1].events = 73;
    fds[1].revents = 0;
    fds[1].fd = ctlsocket;
    
    if ( poll(fds, 2, -1) == -1 )
    {
      if ( btif_trace_level > iLevelMessages )
        LogMsg(1283, "poll failed\n");
      v7 = &clientfd;
LABEL_27:
      *v7 = -1;
      goto LABEL_31;
    }
    if ( btif_trace_level > iLevelMessages )
      LogMsg(
        1283,
        "poll result : fds[0].revents(0x%4x), fds[1].revents(0x%4x)\n",
        fds[0].revents,
        fds[1].revents);
         
    if ( fds[0].revents & 1 )
    {
      rcvLen = receiveIncomingData(clientfd);
      if ( rcvLen <= 0 )
      {
        if ( btif_trace_level > iLevelMessages )  LogMsg(1283, "rcvLen = %d\n", rcvLen);
        v7 = &clientfd;
        goto LABEL_27;
      }
    }
    if ( fds[0].revents & 0x10 )
    {
      if ( btif_trace_level > iLevelMessages )
        LogMsg(1283, "fds[0].revents & POLLHUP");
      v7 = &clientfd;
      goto LABEL_27;
    }
  }
  while ( !(fds[1].revents & 1) );
  if ( btif_trace_level > iLevelMessages )
  {
    v9 = "terminate thread from socket pair\n";
LABEL_33:
    LogMsg(1283, v9);
  }
LABEL_34:
  return NULL;
}

void bta_gps_rcv_vse_cback(UINT8 event, UINT8 *p_data) 
//void bta_gps_rcv_vse_cback(tBTA_GPS_EVT event, tBTA_GPS *p_data)
{
  if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "sizeof(p_data) is %s\n", sizeof(p_data));	 
	
  if ( (p_data) == 16 )
  {
    event = clientfd;
    if ( clientfd > 0 )
    {
      event = send(clientfd, p_data, event - 1, NULL) + 1;
      if ( !event )
      {
        if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "send failed %s\n", strerror(__errno()));
        event = -1;
        clientfd = -1;
      }
    }
  }
}

signed int getServerPort()
{
  return 10001;
}

void start_tcp_server()
{  
  struct sockaddr_un un, peer_addr;
  int optval;
  optval = 1;
  bExit = 0;
  if ( hservSocket > 0 )
  {
    if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "[GPS] tcp service is already up\n");
  
    hservSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    int opt = setsockopt(hservSocket, 1, 2, &optval, 4);
    
    if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "[GPS] start unix listening socket %d, set opt = %d\n", hservSocket, opt);
    un.sa_family = 1;

    __strcpy_chk(un.sun_path, getUnixSocketPath(), 108, optval);
    unlink(un.sun_path);
  
    int ret = bind(hservSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
    
    if (ret != -1 )
    {
	int Result = chown(getUnixSocketPath(), 0xFFFFFFFF, 0xBC0u);
	if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "[GPS] chown %d\n", Result);
	    Result = chmod(getUnixSocketPath(), 0x1F8u);
	if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "[GPS] chmod %d\n",  Result);
    
        pthread_create(&ServThreadID, NULL, wait_4_command_thread, NULL);
        if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "tcp listening thread id = %ld\n", ServThreadID);
    
        BTM_RegisterForVSEvents(bta_gps_rcv_vse_cback, true);
   
    } else if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "tcp bind socket failed \n");
  }
}

void stop_tcp_server()
{
  if ( btif_trace_level > 3 ) LogMsg(1283, "[GPS] stop_tcp_server");
  
  BTM_RegisterForVSEvents(bta_gps_rcv_vse_cback, false);
  
  if ( hservSocket > 0 )
  {
    shutdown(hservSocket, 2);
    hservSocket = -1;
  }
  pthread_join(ServThreadID, false);
  
  if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "%s", "uninitCtrlSocekt");
  
  if ( clientfd > 0 )
  {
    close(clientfd);
    clientfd = -1;
  }
  
  if ( btif_trace_level > iLevelMessages )  LogMsg(1283, "stop_tcp_server exit...");
}

void bta_gps_enable()
{
   start_tcp_server();
}
void bta_gps_disable()
{
   stop_tcp_server();
}
void bta_gps_sm_execute(BT_HDR *p_msg)
{	
  if      ( appl_trace_level > iLevelMessages ) LogMsg(1283, "bta_gps_sm_execute event:0x%x", p_msg->event);
  
  if      ( p_msg->event == 9472 ) bta_gps_enable();
  else if ( p_msg->event == 9473 ) bta_gps_disable();
  else if ( appl_trace_level > 1 ) LogMsg(1281, "bta_gps_sm_execute: unknown event 0x%x", p_msg->event);
  
}
