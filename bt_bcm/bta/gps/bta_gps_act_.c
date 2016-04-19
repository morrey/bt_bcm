#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "bta_api.h"
#include "btm_api.h"

#include "bta_sys.h"

		   
const char * SOCKET_PATH = "/data/gps/brcm_gps_unix_socket";
const char * GPSStopString = "GpsSerialCom::Close()";

int hservSocket;
int bExit;
int dword_25E5F0;
pthread_t  ServThreadID;
bool bReadyToQuit, dword_26E6C4, dword_26E6CC;
int dword_25E5EC, dword_26E6C0, dword_25E5F4;
char byte_26E4C0[];
char unk_26E4C1[100];
char *byte_26E440;


int sub_1CD7B0(char *a1, int a2, char *a3)
{
  char *v3; // ebp@1
  int v4; // eax@4
  char *v5; // esi@5
  int v6; // ebp@5
  int v7; // eax@6
  signed int v8; // bp@8
  int result; // eax@14
  unsigned int v10; // ST0C_2@2
  int v11; // [sp+Ch] [bp-4F0h]@5
  int v12; // [sp+10h] [bp-4ECh]@5
  int v13; // [sp+14h] [bp-4E8h]@5
  unsigned int v14; // [sp+1Eh] [bp-4DEh]@4
  char v15[1200]; // [sp+2Ch] [bp-4D0h]@6
  //int v16; // [sp+4DCh] [bp-20h]@1

  v3 = a1;
  //v16 = _stack_chk_guard;
  if ( btif_trace_level > 3 )
  {
    v10 = a2;
    LogMsg(1283, "---------- %s ---------", a3);
    a2 = v10;
  }
  if ( a2 )
  {
    v14 = a2;
    v4 = a2;
    if ( !a2 )
      goto LABEL_12;
LABEL_5:
    v11 = (int)&v3[v4];
    v5 = v3;
    v12 = (signed int)v3;
    v13 = (int)(v3 + 16);
    v6 = 0;
    while ( 1 )
    {
      v7 = *v5++;
      v6 += sprintf(&v15[v6], "%02x ", v7);
      if ( v5 == (char *)v11 )
        break;
      if ( v5 == (char *)v13 )
      {
        v8 = 16;
        goto LABEL_9;
      }
    }
    v8 = v5 - v12;
LABEL_9:
    if ( btif_trace_level > 3 )
    {
      while ( 1 )
      {
        LogMsg(1283, "%s", (unsigned int)v15);
        v14 -= v8;
        if ( !v14 )
          break;
LABEL_11:
        v4 = v14;
        v3 = v5;
        if ( v14 )
          goto LABEL_5;
LABEL_12:
        v5 = v3;
        v8 = 0;
        if ( btif_trace_level <= 3 )
          goto LABEL_10;
      }
    }
    else
    {
LABEL_10:
      v14 -= v8;
      if ( v14 )
        goto LABEL_11;
    }
  }
  //result = _stack_chk_guard;
  //if ( v16 != _stack_chk_guard )
  //  sub_4AEF5();
  return result;
}
int  sub_1CD920(int a1)
{
  int v1; // ST0C_4@2
  int result; // eax@3

  if ( btif_trace_level > 3 )
  {
    v1 = *(char *)(a1 + 2);
    LogMsg(1283, "GPS VSC completed: opcode = 0x%.4x, param length %d", a1);
  }
  result = sub_1CD7B0(*(char **)(a1 + 4), *(char *)(a1 + 2), "GPS VSC CMPL");
  dword_26E6CC = 1;
  return result;
}

int sub_1CDAB0(UINT8 len, UINT8 *p)
{
  int i; // eax@1
  int v1; // eax@3
  int *v2; // esi@4
  int v3; // eax@6
  char v4; // al@10
  int v5; // eax@21
  char *v6; // eax@22
  char *v7; // esi@29
  int v8; // eax@29
  int v9; // edi@30
  int v10; // eax@31
  char *v11; // edi@33
  signed int v12; // si@33
  signed int v13; // esi@40
  int v14; // edx@53
  char v15; // si@55
  size_t v16; // edi@55
  char v17; // dl@58
  char *v18; // esi@58
  char v19; // dl@58
  char v20; // di@59
  int v21; // ax@61
  char *v22; // edi@62
  int v23; // esi@62
  char v24; // al@63
  int v25; // si@65
  signed int v26; // esi@71
  int v27; // eax@80
  int result; // eax@102
  char addr_len; // [sp+8h] [bp-744h]@0
  char addr_lena; // [sp+8h] [bp-744h]@40
  char addr_lenb; // [sp+8h] [bp-744h]@48
  char addr_lenc; // [sp+8h] [bp-744h]@59
  int v33; // [sp+18h] [bp-734h]@30
  int v34; // [sp+18h] [bp-734h]@62
  int v35; // [sp+1Ch] [bp-730h]@30
  int v36; // [sp+1Ch] [bp-730h]@62
  int v37; // [sp+20h] [bp-72Ch]@30
  int v38; // [sp+20h] [bp-72Ch]@62
  int v39; // [sp+28h] [bp-724h]@23
  int v40; // [sp+28h] [bp-724h]@55
  ssize_t v41; // [sp+2Ch] [bp-720h]@23
  int v42; // [sp+2Ch] [bp-720h]@55
  int v43; // [sp+30h] [bp-71Ch]@23
  ssize_t v44; // [sp+34h] [bp-718h]@18
  int v45; // [sp+3Ch] [bp-710h]@26
  int v46; // [sp+3Ch] [bp-710h]@61
  int v47; // [sp+44h] [bp-708h]@17
  socklen_t v48; // [sp+58h] [bp-6F4h]@1
  int v49; // [sp+5Ch] [bp-6F0h]@14
  int v50; // [sp+60h] [bp-6ECh]@14
  int v51; // [sp+62h] [bp-6EAh]@10
  int v52; // [sp+64h] [bp-6E8h]@14
  int v53; // [sp+68h] [bp-6E4h]@14
  int v54; // [sp+6Ah] [bp-6E2h]@12
  struct sockaddr addr; // [sp+6Ch] [bp-6E0h]@6
  char v56[255]; // [sp+7Eh] [bp-6CEh]@18
  char v57; // [sp+17Dh] [bp-5CFh]@26
  char v58; // [sp+17Eh] [bp-5CEh]@26
  char v59[1200]; // [sp+27Ch] [bp-4D0h]@31
  int v60; // [sp+72Ch] [bp-20h]@1

  v48 = 16;
  bReadyToQuit = 0;
  listen(hservSocket, 1);
  for ( i = bExit; ; i = bExit )
  {
    if ( i )
      goto LABEL_99;
    while ( 1 )
    {
      v1 = hservSocket;
      if ( hservSocket <= 0 )
        goto LABEL_102;
      v2 = &btif_trace_level;
      if ( btif_trace_level > 3 )
      {
        LogMsg(1283, "TCP server accept ...\n", addr_len);
        v1 = hservSocket;
      }
      v3 = accept(v1, &addr, &v48);
      dword_25E5EC = v3;
      if ( btif_trace_level > 3 )
      {
        LogMsg(1283, "TCP server accept clientfd (%d)\n", v3);
        v3 = dword_25E5EC;
      }
      if ( v3 != -1 )
        break;
      if ( bExit )
        goto LABEL_100;
    }
LABEL_14:
    v49 = v3;
    v50 = 73;
    v51 = 0;
    v53 = 73;
    v54 = 0;
    v52 = dword_25E5F0;
    if ( poll((struct pollfd *)&v49, 2u, -1) == -1 )
    {
      if ( btif_trace_level > 3 )
        LogMsg(1283, "poll failed\n", addr_len);
      goto LABEL_90;
    }
    if ( btif_trace_level <= 3 )
    {
      v4 = v51;
      if ( !(v51 & 1) )
        goto LABEL_11;
    }
    else
    {
      LogMsg(1283, "poll result : fds[0].revents(0x%4x), fds[1].revents(0x%4x)\n", v51);
      v4 = v51;
      if ( !(v51 & 1) )
      {
LABEL_11:
        if ( !(v4 & 0x10) )
          goto LABEL_12;
LABEL_87:
        if ( btif_trace_level > 3 )
          LogMsg(1283, "fds[0].revents & POLLHUP, bReadyToQuit=%d", bReadyToQuit);
        if ( bReadyToQuit == 1 )
          goto LABEL_98;
        goto LABEL_90;
      }
    }
    v47 = dword_25E5EC;
    while ( 1 )
    {
      v44 = recvfrom(v47, v56, 0xFFu, 0, 0, 0);
      if ( v44 <= 0 )
        break;
      if ( btif_trace_level > 3 )
        LogMsg(1283, "\nrecv length %d\n", v44);
      v5 = dword_26E6C0;
      if ( dword_26E6C0 )
      {
        if ( v56[0] == -4 )
        {
          v42 = 1;
          v16 = 1;
          v15 = 0;
          v40 = 0;
        }
        else
        {
          v14 = 0;
          do
            ++v14;
          while ( v56[v14] != -4 );
          v40 = v14;
          v15 = v14;
          v16 = v14 + 1;
          v42 = v14 + 1;
        }
        if ( btif_trace_level > 3 )
        {
          LogMsg(1283, "tail data length = %d, new data length = %d\n", dword_26E6C0);
          v5 = dword_26E6C0;
        }
        v17 = v15;
        v18 = byte_26E4C0;
        byte_26E4C0[0] = v5 + v17 + 1;
        memcpy(&byte_26E4C0[v5 + 1], v56, v16);
        v19 = byte_26E4C0[0];
        dword_26E6C0 = 0;
        if ( byte_26E4C0[0] != 22 || (v27 = memcmp(&GPSStopString, &byte_26E4C0[1], 0x16u), v19 = 22, v27) )
        {
          dword_26E6C4 = 0;
          dword_26E6CC = 0;
          BTM_VendorSpecificCommand(64649, (uint8_t)(v19 + 1), byte_26E4C0, sub_1CD920);
          v20 = byte_26E4C0[0];
          if ( btif_trace_level > 3 )
            LogMsg(1283, "---------- %s ---------", (unsigned int)"tail CMD from glgps");
          v46 = (unsigned int)v20 + 1;
          v21 = (unsigned int)v20 + 1;
          if ( (unsigned int)v20 != -1 )
          {
LABEL_62:
            v34 = (int)&v18[v21];
            v22 = v18;
            v36 = (signed int)v18;
            v38 = (int)(v18 + 16);
            v23 = 0;
            while ( 1 )
            {
              v24 = *v22++;
              addr_lenc = v24;
              v23 += sprintf(&v59[v23], "%02x ");
              if ( v22 == (char *)v34 )
                break;
              if ( v22 == (char *)v38 )
              {
                v25 = 16;
                goto LABEL_66;
              }
            }
            v25 = v22 - v36;
LABEL_66:
            if ( btif_trace_level > 3 )
              goto LABEL_70;
LABEL_67:
            v46 -= v25;
            if ( !v46 )
              goto LABEL_71;
            goto LABEL_68;
          }
          while ( 1 )
          {
            v22 = v18;
            v25 = 0;
            if ( btif_trace_level <= 3 )
              goto LABEL_67;
LABEL_70:
            LogMsg(1283, "%s", (unsigned int)v59);
            v46 -= v25;
            if ( !v46 )
              break;
LABEL_68:
            v21 = v46;
            v18 = v22;
            if ( v46 )
              goto LABEL_62;
          }
LABEL_71:
          v26 = 1000;
          while ( !dword_26E6CC )
          {
            usleep(0x3E8u);
            if ( !--v26 )
            {
              if ( !dword_26E6CC && btif_trace_level > 3 )
                LogMsg(1283, "no command completed for 1 sec !!", addr_lenc);
              break;
            }
          }
          v44 = v44 - v40 - 1;
          v6 = &v56[v42];
          if ( v44 > 0 )
            goto LABEL_23;
        }
        else
        {
LABEL_49:
          if ( btif_trace_level > 3 )
            LogMsg(1283, "received GPS stop string !! never quit", addr_lenb);
          bReadyToQuit = 0;
          dword_26E6C4 = 1;
        }
      }
      else
      {
        v6 = v56;
LABEL_23:
        v41 = 0;
        v43 = 0;
        v39 = (int)(v6 + 1);
        do
        {
          ++v43;
          if ( *(uint8_t *)(v39 - 1) != -4 )
            goto LABEL_24;
          __memcpy_chk(&v58, v6, v43, 254);
          v57 = v43;
          v45 = v43;
          if ( btif_trace_level > 3 )
            LogMsg(1283, "---------- %s ---------", "CMD from glgps");
          if ( v43 )
          {
            v7 = &v58;
            v8 = (unsigned int)v43;
            if ( v43 )
            {
LABEL_30:
              v33 = (int)&v7[v8];
              v9 = 0;
              v35 = (signed int)v7;
              v37 = (int)(v7 + 16);
              while ( 1 )
              {
                v10 = (unsigned int)*v7++;
                v9 += sprintf(&v59[v9], "%02x ", v10);
                if ( v7 == (char *)v33 )
                  break;
                if ( v7 == (char *)v37 )
                {
                  v11 = v7;
                  v12 = 16;
                  goto LABEL_34;
                }
              }
              v11 = v7;
              v12 = v7 - v35;
LABEL_34:
              if ( btif_trace_level > 3 )
                goto LABEL_38;
            }
            else
            {
              while ( 1 )
              {
                v11 = v7;
                v12 = 0;
                if ( btif_trace_level <= 3 )
                  break;
LABEL_38:
                LogMsg(1283, "%s", (unsigned int)v59);
                v45 -= v12;
                if ( !v45 )
                  goto LABEL_39;
LABEL_36:
                v8 = v45;
                v7 = v11;
                if ( v45 )
                  goto LABEL_30;
              }
            }
            v45 -= v12;
            if ( !v45 )
              goto LABEL_39;
            goto LABEL_36;
          }
LABEL_39:
          if ( v43 == 22 && !memcmp(&GPSStopString, &v58, 0x16u) )
            goto LABEL_49;
          dword_26E6C4 = 0;
          dword_26E6CC = 0;
          v13 = 1000;
          BTM_VendorSpecificCommand(64649, (unsigned int)(v43 + 1), &v57, sub_1CD920);
          do
          {
            if ( dword_26E6CC )
              goto LABEL_43;
            usleep(0x3E8u);
            --v13;
          }
          while ( v13 );
          if ( dword_26E6CC || btif_trace_level <= 3 )
          {
LABEL_43:
            v6 = (char *)v39;
            v43 = 0;
            goto LABEL_24;
          }
          LogMsg(1283, "no command completed for 1 sec !!", addr_lena);
          v6 = (char *)v39;
          v43 = 0;
LABEL_24:
          ++v41;
          ++v39;
        }
        while ( v41 < v44 );
        if ( v43 )
        {
          __memcpy_chk(&unk_26E4C1, v6, v43, 511);
          dword_26E6C0 = v43;
          if ( btif_trace_level > 3 )
            LogMsg(1283, "tail data length = %d\n", v43);
        }
      }
    }
    if ( btif_trace_level > 3 )
      LogMsg(1283, "rcvLen = %d, bReadyToQuit=%d\n", v44);
    if ( bReadyToQuit == 1 )
      goto LABEL_98;
    if ( dword_26E6C4 )
      break;
LABEL_90:
    dword_25E5EC = -1;
  }
  if ( v51 & 0x10 )
    goto LABEL_87;
LABEL_12:
  if ( !(v54 & 1) )
  {
    v3 = dword_25E5EC;
    goto LABEL_14;
  }
  if ( btif_trace_level > 3 )
    LogMsg(1283, "terminate thread from socket pair, bReadyToQuit=%d", bReadyToQuit);
  if ( bReadyToQuit == 1 )
  {
LABEL_98:
    bExit = 1;
LABEL_99:
    v2 = &btif_trace_level;
LABEL_100:
    if ( *v2 > 3 )
      LogMsg(1283, "wait_4_command_thread exit\n", addr_len);
  }
LABEL_102:
  result = 0;
  return result;
}
void * bta_gps_rcv_vse_cback(unsigned int a1, int a2)
{
  void *result; // eax@1
  bool v3; // zf@6
  void *v4; // ST1C_4@2
  int *v5; // eax@10
  char v6; // al@10
  char n; // [sp+8h] [bp-24h]@6

  sub_1CD7B0("GPS VSE", 0, "");
  result = (void *)*(int *)a2;
  if ( appl_trace_level > 4 )
  {
    v4 = (void *)*(int *)a2;
    LogMsg(1284, "****************%s: len = %d ", "bta_gps_rcv_vse_cback");
    result = v4;
  }
  if ( result == 16 )
  {
    result = (void *)dword_25E5EC;
    if ( dword_25E5EC > 0 )
    {
      v3 = send(dword_25E5EC, (const void *)(a2 + 1), a1 - 1, 0) == -1;
      result = &btif_trace_level;
      if ( v3 )
      {
        if ( btif_trace_level > 3 )
        {
          v5 = (int *)__errno();
          v6 =  strerror(*v5);
         LogMsg(1283, "send failed %s\n", v6);
        }
        dword_25E5EC = -1;
      }
      else if ( btif_trace_level > 3 )
      {
        LogMsg(1283, "sent VSE to GLGPS", n);
      }
    }
  }
  return result;
}

int start_tcp_server()
{
  int v0; // edi@2
  int result; // eax@4
  int v2; // eax@6
  char *v3; // eax@8
  char *v4; // eax@13
  const char *v5; // eax@16
  char v6; // di@18
  const char *v7; // eax@19
  char v8; // di@21
  int *v9; // eax@29
  int *v10; // eax@22
  char *v11; // eax@33
  char protocol; // [sp+8h] [bp-B4h]@0

  struct sockaddr addr; // [sp+2Eh] [bp-8Eh]@7
  //int v16; // [sp+9Ch] [bp-20h]@1
  
  int optval = 1;
  bExit = 0;
  //v16 = stack_chk_guard;
  if ( hservSocket <= 0 )
  {
    hservSocket = socket(AF_UNIX, SOCK_STREAM, 0);
     
    v2 = setsockopt(hservSocket, 1, 2, &optval, 4);
    if ( btif_trace_level > 3 )
    {
      optval = v2;
      LogMsg(1283, "[GPS] start unix listening socket %d, set opt = %d\n", hservSocket);
      addr.sa_family = 1;
      if ( btif_trace_level > 3 )
        LogMsg(1283, "[GPS] Unix Socket Path %s\n", byte_26E440);
    }
    else
    {
      addr.sa_family = 1;
    }
    v3 = &byte_26E440;
    if ( !byte_26E440 )
      v3 = SOCKET_PATH;
    __strcpy_chk(addr.sa_data, v3, 108, optval);
    unlink(addr.sa_data);
    __strlen_chk(addr.sa_data, 108);
    if ( btif_trace_level > 3 )
    {
      LogMsg(1283, "[GPS] Unix Socket Path %s\n", byte_26E440);
      v11 = &byte_26E440;
      if ( !byte_26E440 )
        v11 = SOCKET_PATH;
      LogMsg(1283, "AYCDBG getUnixSocketPath=%s", v11);
    }
    v0 = bind(hservSocket, &addr, 0x6Eu);
    if ( v0 == -1 )
    {
      if ( btif_trace_level > 3 )
      {
        v4 = __errno();
        LogMsg(1283, "tcp bind socket failed errno = %d\n", v4);
      }
    }
    else
    {
      if ( btif_trace_level > 3 )
        LogMsg(1283, "[GPS] Unix Socket Path %s\n", byte_26E440);
      v5 = &byte_26E440;
      if ( !byte_26E440 )
        v5 = SOCKET_PATH;
      v6 = chown(v5, 0xFFFFFFFF, 0x3E8);
      if ( btif_trace_level > 3 )
      {
        v9 = (int *)__errno();
        strerror(*v9);
        LogMsg(1283, "[GPS] chown %d, %s\n", v6);
        if ( btif_trace_level > 3 )
          LogMsg(1283, "[GPS] Unix Socket Path %s\n", byte_26E440);
      }
      v7 = &byte_26E440;
      
      if ( !byte_26E440 ) v7 = SOCKET_PATH;
      
      v8 = chmod(v7, 0x1F8u);
      
      if ( btif_trace_level > 3 )
      {
        v10 = (int *)__errno();
        strerror(*v10);
        LogMsg(1283, "[GPS] chmod %d\n", v8);
      }
      if ( socketpair(1, 1, 0, &dword_25E5F0) < 0 && btif_trace_level > 3 )
        LogMsg(1283, "%s failed", "initCtrlSocekt");
      pthread_create(&ServThreadID, NULL, (void *(*)(void *))sub_1CDAB0, NULL);
      if ( btif_trace_level > 3 )
        LogMsg(1283, "tcp listening thread id = %ld\n", ServThreadID);
      v0 = 0;
      BTM_RegisterForVSEvents(bta_gps_rcv_vse_cback, true);
    }
  }
  else
  {
    v0 = 0;
    if ( btif_trace_level > 3 )
      LogMsg(1283, "[GPS] tcp service is already up\n", protocol);
  }
  result = v0;
  //if ( v16 != stack_chk_guard )
  //  sub_4AEF5();
  return result;
}

int stop_tcp_server()
{
  signed int v0; // esi@3
  char n; // [sp+8h] [bp-34h]@0
  char na; // [sp+8h] [bp-34h]@10
  char v4; // [sp+2Fh] [bp-Dh]@1

  v4 = 120;
  if ( btif_trace_level > 3 )
    LogMsg(1283, "[GPS] stop_tcp_server++", n);
  
  v0 = 200;
  do
  {
    if ( bReadyToQuit == 1 ) break;
    if ( dword_26E6C4 == 1 ) break;
    usleep(0x2710u);
    --v0;
  }
  while ( v0 );
  
  if ( btif_trace_level > 3 )
    LogMsg(1283, "[GPS] stop_tcp_server--", n);
  usleep(0x2710u);
  if ( dword_26E6C4 == 1 && !bReadyToQuit )
  {
    if ( btif_trace_level > 3 )
      LogMsg(1283, "[GPS] keep socket up , just return", n);
    return 0;
  }
  BTM_RegisterForVSEvents(bta_gps_rcv_vse_cback, false);
  write(dword_25E5F4, &v4, 1);
  usleep(0x2710u);
  if ( hservSocket > 0 )
  {
    shutdown(hservSocket, 2);
    close(hservSocket);
    hservSocket = -1;
  }
  pthread_join(ServThreadID, 0);
  if ( btif_trace_level > 3 )
    LogMsg(1283, "%s, %d, %d, %d", "uninitCtrlSocekt");
  if ( dword_25E5F0 > 0 )
  {
    close(dword_25E5F0);
    dword_25E5F0 = -1;
  }
  if ( dword_25E5F4 > 0 )
  {
    close(dword_25E5F4);
    dword_25E5F4 = -1;
  }
  if ( dword_25E5EC > 0 )
  {
    close(dword_25E5EC);
    dword_25E5EC = -1;
  }
  if ( btif_trace_level <= 3 )
    return 0;
  LogMsg(1283, "stop_tcp_server exit...", na);
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
  if ( appl_trace_level > 3 ) LogMsg(1283, "bta_gps_sm_execute event:0x%x", p_msg->event);
  
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
