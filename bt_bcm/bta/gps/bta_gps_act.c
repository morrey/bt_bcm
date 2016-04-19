#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/poll.h>

#include "bta_api.h"
#include "btm_api.h"
#include "bta_sys.h"

const char * SOCKET_PATH = "/data/gps/brcm_gps_unix_socket";

char tail_8957[];
char unk_712[511];
int taillen_8958, fd;
int hservSocket, clientfd, ctlSocket;
pthread_t  ServThreadID;
bool bReadyToQuit, bExit;
//int _stack_chk_guard; // weak

const int iLevelMessages = 1;  //default 3

void bta_gps_cback() {}

char *getUnixSocketPath()
{
  char *result = SOCKET_PATH;
  
  if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "[GPS] Unix Socket Path %s\n", result);
  
  return result;
}

int receiveIncomingData(int fd)
{
  //int v1; // r9@1
  int i; // r4@4
  int v3; // r8@9
  int v4; // r3@9
  size_t v5; // r4@9
  char *v6; // r5@9
  char *v7; // r11@10
  char *v8; // r7@10
  int v9; // r4@10
  uint8_t *v10; // r12@10
  int v11; // t1@11
  uint8_t *v12; // ST00_4@12
  int result; // r0@17
  //int *v14; // [sp+4h] [bp-23Ch]@1
  char src[256]; // [sp+14h] [bp-22Ch]@6
  char v16; // [sp+114h] [bp-12Ch]@12
  uint8_t v17[3]; // [sp+115h] [bp-12Bh]@10

  //v1 = fd;

  while ( true )
  {
    v6 = src;
    result = recv(fd, src, 0xFFu, 0);
    v3 = result;
    if ( result <= 0 ) break;
    if ( taillen_8958 )
    {
      for ( i = 0; (uint8_t)src[i] != 252; ++i )
        ;
      if ( btif_trace_level > iLevelMessages )
        LogMsg(1283, "tail data length = %d, new data length = %d\n");
      v3 = v3 - i - 1;
      v4 = taillen_8958 + 1 + i;
      v5 = i + 1;
      tail_8957[0] = v4;
      memcpy(&tail_8957[taillen_8958 + 1], src, v5);
      taillen_8958 = 0;
      v6 = &src[v5];
      BTM_VendorSpecificCommand(64649, (uint8_t)(tail_8957[0] + 1), tail_8957, bta_gps_cback);
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
        BTM_VendorSpecificCommand(64649, (uint8_t)(v9 + 1), &v16, bta_gps_cback);
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

static void * wait_4_command_thread()
{
  //int *v0; // r7@1
  uint8_t *v1; // r4@3
  unsigned int v2; // r6@3
  bool v3; // cf@3
  bool v4; // zf@3
  uint8_t *v5; // r6@3
  int v6; // r2@6
  int *v7; // r3@15
  int v8; // r2@19
  const char *v9; // r1@11
  int result; // r0@34
  int v11; // [sp+0h] [bp-50h]@1
  struct pollfd fds[2]; // [sp+4h] [bp-4Ch]@12
  int v13; // [sp+Ch] [bp-44h]@12
  int v14; // [sp+10h] [bp-40h]@12
  int v15; // [sp+12h] [bp-3Eh]@12
  struct sockaddr addr; // [sp+14h] [bp-3Ch]@6
  //int v17; // [sp+24h] [bp-2Ch]@1

  v11 = 16;
  //v0 = &_stack_chk_guard;
 // v17 = _stack_chk_guard;
  listen(hservSocket, 1);
  while ( 1 )
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
    v6 = accept(hservSocket, &addr, (socklen_t *)&v11);
    clientfd = v6;
    if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "TCP server accept clientfd (%d)\n", v6);
    if ( clientfd != -1 )  break;
    if ( bExit )  goto LABEL_10;
  }
  do
  {
    fds[0].events = 73;
    fds[0].revents = 0;
    v13 = ctlSocket;
    v14 = 73;
    v15 = 0;
    fds[0].fd = clientfd;
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
        v15,
        v11,
        fds[0].fd,
        *(char *)&fds[0].events,
        v13,
        *(char *)&v14);
    if ( fds[0].revents & 1 )
    {
      v8 = receiveIncomingData(clientfd);
      if ( v8 <= 0 )
      {
        if ( btif_trace_level > iLevelMessages )
          LogMsg(1283, "rcvLen = %d\n", v8);
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
  while ( !(v15 & 1) );
  if ( btif_trace_level > iLevelMessages )
  {
    v9 = "terminate thread from socket pair\n";
LABEL_33:
    LogMsg(1283, v9);
  }
LABEL_34:
  result = 0;
  return NULL;
}

int bta_gps_rcv_vse_cback(int result, int a2) 
{
  int iLen = result; 
  if ( result == 16 )
  {
    result = clientfd;
    if ( clientfd > 0 )
    {
      //send(int sockfd, const void *buf, size_t len, int flags)
      //result = send(clientfd, (const void *)(a2 + 1), v2 - 1, 0) + 1;
      result = send(clientfd, (const void *)(a2 + 1), iLen - 1, 0) + 1;
      if ( !result )
      {
        if ( btif_trace_level > iLevelMessages )
        {
          char * strErr = strerror(__errno());
          LogMsg(1283, "send failed %s\n", strErr);
        }
        result = -1;
        clientfd = -1;
      }
    }
  }
  return result;
}

signed int getServerPort()
{
  return 10001;
}

int start_tcp_server()
{
  uint8_t *v2; // r4@1
  int v3; // r0@4
  int v4; // r0@6
  int v5; // r0@6
  int v6; // r0@6
  int v7; // r5@6
  int *v8; // r0@8
  const char *v9; // r0@9
  int v10; // r0@9
  int v11; // r5@9
  int *v12; // r0@10
  char *v13; // r0@10
  const char *v14; // r0@11
  int v15; // r5@11
  int *v16; // r0@12
  char *v17; // r0@12
  int result; // r0@16
  int optval; // [sp+8h] [bp-90h]@1
  signed int v20; // [sp+Ch] [bp-8Ch]@6
  int v21; // [sp+Eh] [bp-8Ah]@6
  int v22; // [sp+7Ch] [bp-1Ch]@1

  optval = 1;
  bExit = 0;
  v2 = btif_trace_level;
  if ( hservSocket > 0 )
  {
    if ( btif_trace_level > iLevelMessages )
      LogMsg(1283, "[GPS] tcp service is already up\n");
LABEL_16:
    result = 0;
    goto LABEL_18;
  }
  hservSocket = socket(1, 1, 0);
  v3 = setsockopt(hservSocket, 1, 2, &optval, 4);
  if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "[GPS] start unix listening socket %d, set opt = %d\n", hservSocket, v3);
  v20 = 1;
  v4 = getUnixSocketPath();
  __memcpy_chk(&v21, v4, 108);
  unlink((const char *)&v21);
  v5 = __strlen_chk(&v21, 108);
  v6 = bind(hservSocket, (const struct sockaddr *)&v20, v5 + 2);
  v7 = v6;
  if ( v6 != -1 )
  {
    v9 = getUnixSocketPath();
    v10 = chown(v9, 0xFFFFFFFF, 0xBC0u);
    v11 = v10;
    if ( btif_trace_level > iLevelMessages )
    {
      v12 = __errno();
      v13 = strerror(*v12);
      LogMsg(1283, "[GPS] chown %d, %s\n", v11, v13);
    }
    v14 = getUnixSocketPath();
    v15 = chmod(v14, 0x1F8u);
    if ( btif_trace_level > iLevelMessages )
    {
      v16 = __errno();
      v17 = strerror(*v16);
      LogMsg(1283, "[GPS] chmod %d\n", v15, v17);
    }
    pthread_create(&ServThreadID, NULL, wait_4_command_thread, NULL);
    if ( btif_trace_level > iLevelMessages )
      LogMsg(1283, "tcp listening thread id = %ld\n", ServThreadID);
    BTM_RegisterForVSEvents(bta_gps_rcv_vse_cback, 1);
    goto LABEL_16;
  }
  if ( btif_trace_level > iLevelMessages )
  {
    v8 = __errno();
    LogMsg(1283, "tcp bind socket failed errno = %d\n", *v8);
  }
  result = v7;
LABEL_18:
  return result;
}

int stop_tcp_server()
{
  //uint8_t *v3; // r4@1
  int v5[120];  // [sp+4h] [bp-14h]@1
  int v6; // [sp+8h] [bp-10h]@1
  
  char buf = 120;

  if ( btif_trace_level > 3 ) LogMsg(1283, "[GPS] stop_tcp_server");
  BTM_RegisterForVSEvents(bta_gps_rcv_vse_cback, 0);
  write(fd, &buf, 1);
  if ( hservSocket > 0 )
  {
    shutdown(hservSocket, 2);
    hservSocket = -1;
  }
  pthread_join(ServThreadID, 0);
  if ( btif_trace_level > iLevelMessages ) LogMsg(1283, "%s", "uninitCtrlSocekt");
  
  if ( ctlSocket > 0 )
  {
    close(ctlSocket);
    ctlSocket = -1;
  }
  
  if ( fd > 0 )
  {
    close(fd);
    fd = -1;
  }
  
  if ( btif_trace_level > iLevelMessages )  LogMsg(1283, "stop_tcp_server exit...");
  
  return 0;
}

int bta_gps_enable()
{
  return start_tcp_server();
}
int bta_gps_disable()
{
  return stop_tcp_server();
}
int bta_gps_sm_execute(BT_HDR *p_msg)
{	
  if ( appl_trace_level > iLevelMessages ) LogMsg(1283, "bta_gps_sm_execute event:0x%x", p_msg->event);
  
  if ( p_msg->event == 9472 )
  {
    bta_gps_enable();
  }
  else if ( p_msg->event == 9473 )
  {
    bta_gps_disable();
  }
  else if ( appl_trace_level > 1 )
  {
    LogMsg(1281, "bta_gps_sm_execute: unknown event 0x%x", p_msg->event);
  }
  
  return 1;
}
