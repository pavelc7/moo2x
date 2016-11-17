#include	"stdafx.h"
#pragma pack(push, 1)


struct dp_create_player
{
	DWORD/*DPID*/ dpID;      //0
	char name1[DPLONGNAMELEN]; //4
	char name2[DPSHORTNAMELEN]; //56
	DWORD clock;    //76
};
#define nGUID_dword_560798 (*(i32*)(0x00560798))
#define lpGUID ((LPGUID*)(0x0058D770))
  //see http://www.microsoft.com/en-us/download/details.aspx?id=13287 //for aug 2007 directx sdk for directxplay
i32 __stdcall guid_sub_5385C0(int a1, const void *a2, int a3, int a4, int a5)
{
  DWORD v5; 
  signed int result;

  if ( nGUID_dword_560798 >= 9 )
  {
    result = 0;
  }
  else
  {
    lpGUID[nGUID_dword_560798] = (LPGUID)malloc(124);//124
    v5 = (DWORD)lpGUID[nGUID_dword_560798];
	memcpy((void *)lpGUID[nGUID_dword_560798], (void*)a1, 16);
    
    memcpy((void *)(v5 + 16), a2, 100u);
    *(DWORD *)(v5 + 116) = a3;
    *(DWORD *)(v5 + 120) = a4;
    _strlwr((char *)(v5 + 16));
    result = 1;
    ++nGUID_dword_560798;
  }
  return result;
}
i8 __cdecl net_check_GUID_sub_538680(const char *nettypename)
{
  int i; 
  i8 v3; 

  v3 = -1;
  for ( i = 0; i < nGUID_dword_560798; ++i )
  {
	 char* str = (char*) (((DWORD)(lpGUID[i])) + 16);
    if ( strstr((const char *)str, nettypename ) ) //"tcp/ip"
      v3 = i;
  }
  return v3;
}

void __cdecl DP_sub_539CE0(signed int type_err)
{	
	char Text[MSL]; 
	char*str="Unknown Err";
	strcpy(Text, "DirectPlay ERR: ");
	switch (type_err)
	{	
	case  DPERR_BUFFERTOOSMALL:		
		str="Buffer Too Small";
		break;	
	case DDERR_INVALIDOBJECT:
		str="Invalid Object";
		break;			
	case DPERR_NOMESSAGES:		
			str="No Messages";
			break;	
	case DDERR_INVALIDPARAMS:
		str="Invalid Params";
			break;	
	case DDERR_GENERIC:
		str="Generic";
			break;	
	default:		
		str="Unknown Err";
	}
	strcpy(&Text[16], str);	
	exit_Exit_With_Message(Text);
}
#define  dword_589990 (*(i32*)(0x00589990))
#define  dpid_dword_5899F8 (*(DWORD*)(0x005899F8))
#ifdef DPLAY_USE_IN_DLL
#undef dpid_dword_5899F8
#define  dpid_dword_5899F8 (*(DPID*)(0x005899F8))
#define  byte_551100 ((char*)(0x00551100))
#define  dword_55BF24 (*(i32*)(0x0055BF24))


int __cdecl net_sub_538760()
{

  DirectPlayEnumerate((LPDPENUMDPCALLBACKA)&guid_sub_5385C0,0);

  return 0;
}
#define dword_591F00 ((i32*)(0x00591F00))
#define lpDP (*(LPDIRECTPLAY*)(0x0055BF28))
i32 __cdecl sub_539AD0(DWORD* a1)
{

	DWORD v5=0; // [sp+10h] [bp-18h]@1
	DPID *v6=0; // [sp+14h] [bp-14h]@1
	DPID idFrom=0,idTo=0;
	
	DWORD dwDataSize = dword_591F00[0];
	
	LPVOID lpData = (LPVOID)a1[3];
		
	//STDMETHOD(Receive)              (THIS_ LPDPID,LPDPID,DWORD,LPVOID,LPDWORD) PURE;
	HRESULT res = lpDP->Receive(&idFrom,&idTo,DPRECEIVE_ALL|DPRECEIVE_PEEK,lpData,&dwDataSize);		
	if (!SUCCEEDED(res) )
	{
		if ( res != DPERR_NOMESSAGES )
			DP_sub_539CE0(res);
		return 0;
	}
	else
	{
		
		OutputDebugStringA("Receiving Message....\n");
		pLog.Printf("Net Receive (i=0x%x) From 0x%X to 0x%X [0]=0x%x, size=%d\n",dpid_dword_5899F8,idFrom,idTo,*((DWORD*)a1[3]),dwDataSize);
		pLog.Dump(lpData,dwDataSize);
		//DP2 STDMETHOD(Receive)              (THIS_ LPDPID,LPDPID,DWORD,LPVOID,LPDWORD) PURE;				
		dwDataSize = dword_591F00[0];
		res =  lpDP->Receive(&idFrom,&idTo,DPRECEIVE_ALL,lpData,&dwDataSize);
		
		
		if ( !SUCCEEDED(res)  )
		{
			DP_sub_539CE0(res);
			return 0;
		}
		else
		{
		  a1[0] = idFrom;      
		  a1[1] = idTo;
		  a1[2] = dwDataSize;
		  pLog.Printf("Net Receive (i=0x%x) From 0x%X to 0x%X [0]=0x%x, size=%d\n",dpid_dword_5899F8,idFrom,idTo,*((DWORD*)a1[3]),dwDataSize);
		  pLog.Dump(lpData,dwDataSize);
		  return 3;
		}
	}

	return 0;
}
#endif
#define netcode_connection_type (*(i32*)(0x00589A0C)) 
#define dword_55BF2C (*(i32*)(0x0055BF2C))
#define dword_589988 (*(i32*)(0x00589988))
#define dword_5899A8  ((i32*)(0x005899A8)) //array of pointers to players
#define dword_5897F0  (*(i32_cdecl_funcname_void_t*)(0x005897F0))
#define dword_589A10 (*(i32_cdecl_funcname_void_t*)(0x00589A10))

#ifdef DPLAY_USE_IN_DLL
#define dword_5897F8 ((LPDPSESSIONDESC*)(0x005897F8))
void __cdecl kill_net_sub_534B40()
{
  unsigned int v0; // esi@4
  void **v1; // edi@5

  if ( !netcode_connection_type )
  {
    if ( lpDP )
    {
	#ifdef DPLAY_USE_IN_DLL
      lpDP->Close();
      lpDP->Release();
	 #endif
      lpDP = 0;
    }    
    
	for(int i=0;i<dword_55BF2C;i++)
	{
		if ( dword_5897F8[i] )
		{
			free(dword_5897F8[i]);
			dword_5897F8[i] = 0;
		}        
    }    
    dword_55BF2C = 0;
  }
  if ( netcode_connection_type == 1 || netcode_connection_type == 2 )
    sub_535570();
}



int __cdecl sub_5387A0(DPID            dpID,
    LPCSTR           lpName1,
    LPCSTR       lpName2)
{
  dp_create_player* pdpcp; // eax@1
  int v4; // ecx@1
  int result; // eax@1

  pdpcp = (dp_create_player*)malloc(sizeof(dp_create_player));
  assert(sizeof(dp_create_player)==80);
  //v4 = dword_589988;
  dword_5899A8[dword_589988] = (DWORD)pdpcp;
  pdpcp->dpID=dpID;
  strcpy(pdpcp->name1, lpName2);
  strcpy(pdpcp->name2, lpName1);
  pdpcp->clock=dword_5897F0();
  //*(DWORD *)dword_5899A8[v4] = dpId;
  //strcpy((char *)(dword_5899A8[dword_589988] + 4), (char*)lpName2);
  //strcpy((char *)(dword_5899A8[dword_589988] + 56), (char*)lpName1);
  //result = dword_5897F0();
  //*(DWORD *)(dword_5899A8[dword_589988++] + 76) = result;
  dword_589988++;
  return pdpcp->clock;
}
/*
    DPID    dpId,
    LPSTR   lpFriendlyName,
    LPSTR   lpFormalName,
    DWORD   dwFlags,
    LPVOID  lpContext 
*/
/*
DPID            dpId,
    DWORD           dwPlayerType,
    LPCDPNAME       lpName,
    DWORD           dwFlags,
    LPVOID          lpContext
*/
i32 __stdcall sub_538780(DPID            dpId,
    //DWORD           dwPlayerType,
    //LPCDPNAME       lpName,
	LPCSTR   lpFriendlyName,
	LPCSTR   lpFormalName,
    DWORD           dwFlags,
    LPVOID          lpContext )
{
	
	sub_5387A0(dpId,
		lpFriendlyName,lpFormalName);
		//lpName->lpszShortNameA, lpName->lpszLongNameA);
  return 1;
}
//STDMETHOD(EnumSessions)         (THIS_ LPDPSESSIONDESC2,DWORD,LPDPENUMSESSIONSCALLBACK2,LPVOID,DWORD) PURE;
signed int __cdecl DP_enum_players_sub_534DB0(int a1)
{
  unsigned int v1; // esi@1
  void **v2; // edi@2

  v1 = 0;
  if ( dword_589988 )
  {
    v2 = (void **)dword_5899A8;
    do
    {
      if ( *v2 )
        free(*v2);
      ++v2;
      ++v1;
    }
    while ( v1 < dword_589988 );
  }
  dword_589988 = 0;
#ifdef DPLAY_USE_IN_DLL
  //(*((void (__stdcall **)(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD))
	//  lpDP->lpVtbl + 13))(lpDP, a1, sub_538780, g_hWnd, 128);
  //STDMETHOD(EnumSessions)         (THIS_ LPDPSESSIONDESC2,DWORD,LPDPENUMSESSIONSCALLBACK2,LPVOID,DWORD) PURE;
  //STDMETHOD(EnumPlayers)          (THIS_ LPGUID,LPDPENUMPLAYERSCALLBACK2,LPVOID,DWORD) PURE;
  //LPDPENUMSESSIONSCALLBACK2
  //STDMETHOD(EnumPlayers)          (THIS_ DWORD_PTR, LPDPENUMPLAYERSCALLBACK,LPVOID,DWORD) PURE;
  lpDP->EnumPlayers(a1,(LPDPENUMPLAYERSCALLBACK) &sub_538780,g_hWnd,128);
#endif
  return 1;
}

#define dword_589998 ((i32*)(0x00589998))
#define byte_56079C (*(i8*)(0x0056079C))
#define dword_58998C (*(i32*)(0x0058998C))
int __cdecl DP_enum_sessions_sub_534FA0(int a1, int a2, int a3, int a4)
{
  unsigned int v4; // esi@1
  char v5; // zf@1
  void **v6; // edi@2
  int v7; // eax@7
  int v8; // esi@7
  //int v10; // [sp+8h] [bp-7Ch]@6
  int v11; // [sp+Ch] [bp-78h]@6
  int v12; // [sp+10h] [bp-74h]@6
  int v13; // [sp+14h] [bp-70h]@6
  int v14; // [sp+18h] [bp-6Ch]@6

  dword_589998[0] = a1;
  dword_589998[1] = a2;
  v4 = 0;
  dword_589998[2] = a3;
  v5 = dword_55BF2C == 0;
  dword_589998[3] = a4;
  if ( !v5 )
  {
    v6 = (void **)dword_5897F8;
    do
    {
      if ( *v6 )
        free(*v6);
      ++v6;
      ++v4;
    }
    while ( v4 < dword_55BF2C );
  }
  dword_55BF2C = 0;
  DPSESSIONDESC v10;
  memset(&v10, 0, sizeof(DPSESSIONDESC));
  assert(sizeof(DPSESSIONDESC)==124);
  v10.dwSize = 124;  
  DWORD*p= (DWORD*)&v10.guidSession.Data1;
  *p = a1;
  *(p+1) = a2;
  *(p+2) = a3;
  *(p+3) = a4;
  if ( !netcode_connection_type )
  {
    byte_56079C = 1;
    HRESULT res = //(*(int (__stdcall **)(IDirectPlay2 *, int *, signed int, signed int (__stdcall *)(int, int, int, char), HWND))((int (__stdcall **)(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD))lpDP->lpVtbl + 14))(

		lpDP->EnumSessions(
           &v10,
           1000,
           (LPDPENUMSESSIONSCALLBACK)sub_538990,
           g_hWnd,1);
    
    if ( res )
    {
      if ( res == DPERR_INVALIDOBJECT )
        exit_Exit_With_Message("Error Enum Current Games Invalid Object");
      if ( res == DPERR_INVALIDPARAMS )
        exit_Exit_With_Message("Error Enum Current Games Invalid Params");
    }
  }
  return dword_55BF2C;
}

i32 __cdecl net_create_sub_5386D0(i32 type)
{
  int guid_index;
  int result=0;
  const char *str="tcp"; 

  guid_index = -1;
  switch ( type )
  {
	case 0:
		str = "modem";
		break;
	case 1:
		str = "ipx";
		break;
	default:
	case 2:
		str = "tcp";
		break;
  }

  guid_index = net_check_GUID_sub_538680(str);
 
  if ( guid_index == -1 )
  {
    return 0;
  }
  else
  {
    if ( lpDP )
    {
      lpDP->Release();
      lpDP = 0;
    }
	//HRESULT WINAPI DirectPlayCreate( LPGUID lpGUID, LPDIRECTPLAY *lplpDP, IUnknown *pUnk);
	HRESULT res = DPERR_UNAVAILABLE;
	#ifdef DPLAY_USE_IN_DLL
	res = DirectPlayCreate(lpGUID[guid_index], &lpDP, NULL);
			//Создание интерфейса IDirectPlay
			/*LPDIRECTPLAY lpdp1;
			res = DirectPlayCreate(lpGUID[guid_index], &lpdp1, NULL);
			//Получение интерфейса необходимой версии
			res = lpdp1->QueryInterface(IID_IDirectPlay4A, (VOID**)&lpDP);
			//Уничтожение ненужного интерфейса
			lpdp1->Release();
			//res = DirectPlayCreate(lpGUID[guid_index],&lpDP,0);
			*/
	#endif
	if ( !SUCCEEDED(res) )
    {
      return 0;
    }
    else
    {
      dword_58998C = guid_index;
	  return 1;
    }
  }
  return 0;
}


void __cdecl DP_Create_Player_sub_538940(LPDPID a1,  LPSTR a2,  LPSTR a3)
{
  HRESULT res = DPERR_NOINTERFACE;
  #ifdef DPLAY_USE_IN_DLL
	  res = lpDP->CreatePlayer((LPDPID)a1,
		 (LPSTR)a2,
         (LPSTR)a3,
	  0);
	  pLog.Printf("NET Create_Player 0x%X %s %s\n",*a1,a2,a3);
	  //(*((HRESULT (__stdcall **)(IDirectPlay *, LPDPID, LPSTR, LPSTR, LPHANDLE))lpDP->lpVtbl + 5))(
       /*  (IDirectPlay *)lpDP,
         (LPDPID)a1,
         (LPSTR)a2,
         (LPSTR)a3,
         0);*/
#endif
  if ( !SUCCEEDED(res) )
  {
    *a1 = 0;
    DP_sub_539CE0(res);
  }
  else
  {
    sub_5387A0(*a1, a2, a3);
  }
}

i32 __cdecl netcode_Net_Create_Game(int a1, int a2, int a3, int a4,  char *a5, const char *a6, const char *a7, char *a8) //00535290
{ 
  DPSESSIONDESC dpSD; 

  memset(&dpSD, 0, sizeof(DPSESSIONDESC));
  assert(sizeof(DPSESSIONDESC)==124);
  dpSD.dwSize = sizeof(DPSESSIONDESC);
  dpSD.guidSession.Data1 = a1;
  *(DWORD *)&dpSD.guidSession.Data2 = a2;  
  *(DWORD *)&dpSD.guidSession.Data4[0] = a3;
  dpSD.dwFlags = DPOPEN_CREATESESSION;//0x02
  *(DWORD *)&dpSD.guidSession.Data4[4] = a4;
  dpSD.dwMaxPlayers = (DWORD)a6;
  strcpy(dpSD.szSessionName, a5);
  if ( a7 && a7[0] )
    strcpy(dpSD.szPassword, a7);
  /*(*((HRESULT (__stdcall **)(IDirectPlay *, LPDPSESSIONDESC))lpDP->lpVtbl + 20))((IDirectPlay *)lpDP, &v12)*/
  HRESULT res  = DPERR_NOINTERFACE;
#ifdef DPLAY_USE_IN_DLL
  res = lpDP->Open(&dpSD);
#endif
  if ( !SUCCEEDED(res) )
  {
	#ifdef DPLAY_USE_IN_DLL
    //(*((void (__stdcall **)(IDirectPlay *))lpDP->lpVtbl + 2))((IDirectPlay *)lpDP);
	lpDP->Release();    
	#endif
    lpDP = 0;
	return 0;
  }
  else
  {	
	LPDPSESSIONDESC buf = (LPDPSESSIONDESC )malloc(dpSD.dwSize);
    dword_5897F8[dword_55BF2C] = buf;
    memcpy(buf, &dpSD, dpSD.dwSize);
    ++dword_55BF2C;
    net_sub_535400();
	DPID id=0;

	char str1[MSL];
	char str2[MSL];
	time_t t=time(0);
	wsprintf(str1,"%s%X",a8,t);
	wsprintf(byte_551100,"%X",t);
    DP_Create_Player_sub_538940(&id, str1, byte_551100);
	pLog.Printf("Net Create as 0x%x %s %s in %s\n",id,str1,byte_551100,dpSD.szSessionName);
    dpid_dword_5899F8 = id;
    sub_539800(450u*1024, (int)a5, (int (__cdecl *)(DWORD, DWORD, DWORD, DWORD))dword_589990);    
    dword_55BF24 = 1;
	return 1;
  }
  return 0;
}
HRESULT __cdecl net_sub_535400()
{
	HRESULT result = DPERR_NOINTERFACE; // eax@2
  if ( lpDP )
	  result = lpDP->EnableNewPlayers(TRUE);
    //result = (*((int (__stdcall **)(_DWORD, _DWORD))lpDP->lpVtbl + 10))(lpDP, 1);
  return result;
}
HRESULT __cdecl net_sub_535420()
{
  HRESULT result = DPERR_NOINTERFACE; // eax@2

  if ( lpDP )
	  result = lpDP->EnableNewPlayers(FALSE);
    //result = (*((HRESULT (__stdcall **)(IDirectPlay *, BOOL))lpDP->lpVtbl + 10))((IDirectPlay *)lpDP, 0);
  return result;
}

i32 __cdecl netcode_Net_Join_Game(const char *a1, const char *a2,  char *a3)
{
  unsigned int v3; // ebx@1
  
  signed int result; // eax@7
  int *v6; // ebx@10
  int v7; // ecx@10
  int v8; // esi@10
  int v9; // eax@10
  int v10; // ST04_4@14
  DPID v11; // [sp+10h] [bp-80h]@14
  DPSESSIONDESC dpSD; // [sp+14h] [bp-7Ch]@10
  int v13; // [sp+18h] [bp-78h]@10
  int v14; // [sp+1Ch] [bp-74h]@10
  int v15; // [sp+20h] [bp-70h]@10
  int v16; // [sp+24h] [bp-6Ch]@10
  int v17; // [sp+28h] [bp-68h]@10
  signed int v18; // [sp+34h] [bp-5Ch]@10
  //int v19; // [sp+6Ch] [bp-24h]@11

  v3 = -1;
  
  for(int i=0;i<dword_55BF2C;i++)
  {
	  if(strcmp(a1, sub_5350C0(i))==0)
	  {
		  v3=i;
		  break;
	  }
  }
  
  if ( v3 == -1 )
  {
    return 0;
  }
  else
  {
    if ( v3 < dword_55BF2C )
    {
      LPDPSESSIONDESC v6 = dword_5897F8[v3]; 
	  DP_enum_players_sub_534DB0(v6->dwSession);
      memset(&dpSD, 0, sizeof(DPSESSIONDESC));
	   assert(sizeof(DPSESSIONDESC)==124);
      //v7 = *v6;
      //v8 = *v6 + 4;      
	  memcpy(&dpSD,v6,24);//sizeof(DPSESSIONDESC));
	  dpSD.dwSize = sizeof(DPSESSIONDESC);
	  dpSD.dwFlags = DPOPEN_OPENSESSION; //0x01
	  //dpSD.
      //v13 = *(_DWORD *)v8;
      //v14 = *(_DWORD *)(v8 + 4);
      //v15 = *(_DWORD *)(v8 + 8);
      //v9 = *(_DWORD *)(v8 + 12);
      //v18 = 1;
      //v16 = v9;
      //v17 = *(_DWORD *)(v7 + 20);
	  if ( dpSD.szPassword[0] )
        strcpy(dpSD.szPassword, a2);
	  HRESULT res  = DPERR_NOINTERFACE;
	  /* (*((HRESULT (__stdcall **)(IDirectPlay *, LPDPSESSIONDESC))lpDP->lpVtbl + 20))((IDirectPlay *)lpDP, &v12)*/
	  res = lpDP->Open(&dpSD);
      if (!SUCCEEDED(res) )
      {
        lpDP->Release();        
        lpDP = 0;
		return 0;
      }
      else
      {
		  DPID id=0;
		  char str1[MSL];
		  time_t t=time(0);
		  wsprintf(str1,"%s%X",a3,t);		  
			wsprintf(byte_551100,"%X",t);
    
        DP_Create_Player_sub_538940(&id, str1, byte_551100);
		pLog.Printf("Net Join as 0x%x %s %s in %s\n",id,str1,byte_551100,dpSD.szSessionName);
        dpid_dword_5899F8 = id;
        sub_539800(450u*1024, (int)dpSD.szSessionName, (i32_pfunc_4xUI32_t)dword_589990);
        result = 1;
        dword_55BF24 = 1;
		return 1;
      }
    }   
  }
  return 0;
}

const char* __cdecl sub_5350C0(ui32 a1)
{
  //int *v1; // eax@3
  char* result; // eax@4

  if ( dword_55BF2C > a1 && lpDP )
  {
    LPDPSESSIONDESC v1 = dword_5897F8[a1];
    if ( v1 )
		result = v1->szSessionName;//v1 + 36;
    else
      result = 0;
  }
  else
  {
    result = 0;
  }
  return result;
}
#define dword_591F28 ((i32*)(0x00591F28))
i16 __cdecl DP_send_sub_539950(int data, unsigned __int16 size, int a3, unsigned __int16 a4, char a5, __int16 a6)
{
  signed __int16 result; // ax@2
  //signed int v7; // eax@14
  DPID idTo;  
  DWORD dwFlags; // [sp-Ch] [bp-18h]@12  
  DWORD dwDataSize; // [sp-4h] [bp-10h]@12

  if ( a4 >= 100u )
  {
    if ( size > 350u || a5 == 2 )
    {
      result = sub_53A560(a6, data, size, a3);
    }
    else
    {
      if ( a4 == 1969 )
      {
        dword_591F28[0] = 113;
      }
      else
      {
        if ( a4 == 1983 )
           dword_591F28[0] = 97;
        else
           dword_591F28[0] = 81;
      }
      memcpy((char *)&dword_591F28[1], (const void *)data, size);
      if ( a3 == 32767 || a3 == 32766 )
      {
        OutputDebugStringA("Sending Message To All....\n");
        dwDataSize = size + 4;
        //lpData = (LPVOID)dword_591F28;
        dwFlags = 0;
        idTo = DPID_ALLPLAYERS;
      }
      else
      {
        OutputDebugStringA("Sending Message To Individual....\n");
        dwDataSize = size + 4;
        //lpData = (LPVOID)dword_591F28;
        dwFlags = 0;
        idTo = a3;
      }
	  dwFlags = DPSEND_GUARANTEED;
	  //STDMETHOD(Send)                 (THIS_ DPID, DPID, DWORD, LPVOID, DWORD) PURE;
      //v7 = //(*((HRESULT (__stdcall **)(IDirectPlay *, DPID, DPID, DWORD, LPVOID, DWORD))lpDP->lpVtbl + 23))(
	  /*DPID idFrom,
        DPID idTo,
        DWORD dwFlags,
        LPVOID lpData,
        DWORD dwDataSize*/
	  pLog.Printf("Net Send From 0x%X to 0x%X [0]=0x%x, size=%d\n",dpid_dword_5899F8,idTo,dword_591F28[0],dwDataSize);
	  pLog.Dump(dword_591F28,dwDataSize);
	  HRESULT res=lpDP->Send(dpid_dword_5899F8,             
             idTo,
             dwFlags,
             (LPVOID)dword_591F28,
             dwDataSize);
	  
      if ( res )
        DP_err_sub_534E30(res);
      result = 1;
    }
  }
  else
  {
    result = -205;
  }
  return result;
}
void __cdecl DP_err_sub_534E30(HRESULT err)
{
  const char *str = "Error Unknown"; // edi@4
  char Text; // [sp+8h] [bp-64h]@19

  switch(err)
  {
	  case DDERR_GENERIC:
	      str = "Generic";
		  break;
	  case  DDERR_UNSUPPORTED:
		str = "Unsupported";
		break;      
	  case DPERR_ACTIVEPLAYERS:
		str = "Active Players";
		break;
	  case DDERR_INVALIDPARAMS:
		str = "Invalid Params";
		break;
		case DPERR_INVALIDOBJECT:
		  str = "Invalid Object";
		  break;
		case DPERR_INVALIDPLAYER:
		  str = "Invalid Player";
		  break;
		case DPERR_UNAVAILABLE:
		  str = "Unavailable";
		  break;
		case DPERR_BUSY:
		  str = "Busy";
		  break;
		case DPERR_USERCANCEL:
		str = "User Cancel";
		break;
	 default:		
		break;
  }
  exit_Exit_With_Message(str);
}

void __cdecl sub_538850(int a1)
{  
  int v1 = -1;
  int found = 0;
  
  for(int i=0;i<dword_589988;i++)
    {      
      if ( dword_5899A8[i] == a1 )
      {
        v1 = i;
        found = 1;
		break;
      }     
    }    
  
  if ( found == 1 )
  {
    if ( dword_589988 - 1 > v1 )
    {
		for(int i=v1;i<dword_589988 - 1;i++)
		{
			dp_create_player* v4 = (dp_create_player*)&dword_5899A8[v1];
			v4[0]=v4[1];
		}              
    }
    free((void *)*(dword_589998 + dword_589988-- + 3));
  }
}



#endif
int __cdecl net_decode_sub_539BE0(DWORD *data) //DPMSG_ADDPLAYER
{
  int result; // eax@2

  if ( data[1] == DPPLAYERTYPE_PLAYER ) //0x01 
    result = sub_5387A0(data[2], (const char *)&data[16]  /*+64*/, (const char *)&data[3] /*+ 12*/);
  return result;
}
void __cdecl net_decode_sub_539BC0(DWORD *data) //DPMSG_DELETEPLAYER
{
  sub_538850(data[1]);
}

signed int __cdecl netcode_Net_Init(int type, int guid1, int guid2, int guid3, int guid4, int pFunc, char *data)
{
  signed int v7; // edi@1
  int v8; // ebx@2

  v7 = 0;
  netcode_connection_type = type;
  
  if ( type )
  {
    v8 = pFunc;
  }
  else
  {
    dword_5897F0 = &callback_sub_534A70;
    dword_589A10 = &callback_sub_534A70;
    v7 = net_create_sub_5386D0(2); //1 - ipx //2 - tcp //0 - modem
    DP_enum_sessions_sub_534FA0(guid1, guid2, guid3, guid4);
    v8 = pFunc;
    dword_589990 = pFunc;
  }
  if ( type == 1 || type == 2 )//1- modem //2 - nullmodem
    v7 = simmodem_Init_Modem(data, v8);
  if ( type == 3 ) //TEN
  {
    v7 = 1;
    sub_5337F0(3, (int)&guid1, v8);
  }
  return v7;
}

signed int __cdecl net_decode_sub_539B80(DWORD *data)
{
  signed int result; // eax@1

  result = 0;
  if(!data)return -1;
  switch (  data[0])
  {
  case DPSYS_CREATEPLAYERORGROUP://join //0x03 DPSYS_ADDPLAYER
	{
    net_decode_sub_539BE0(data);
	result = 1;
	break;
	}
  case DPSYS_DESTROYPLAYERORGROUP://leave //0x05 DPSYS_DELETEPLAYER
    {
      net_decode_sub_539BC0(data);
      result = 1;
	  break;
    }
  }
  return result;
}

int __cdecl callback_sub_534A70()
{
  return 1000 * clock() / 1000;
}
#define dword_591ED8 ((DWORD*)(0x00591ED8))
i8 __cdecl net_decode_sub_539A60()
{
  errno=0;  
  int res=0;
  for(int i=0;i<100;i++)
  {
    res = sub_539AD0(dword_591ED8);
    if ( res != 3 )
      break;
  
    DWORD *data = (DWORD*)dword_591ED8[3];
	res = net_decode_sub_539B80(data);
    if ( !res )
    {      
	  int k=data[0];
      if ( k == 81 || k == 97 || k == 113 )//0x51  0x61 0x71
        res = sub_539C00(dword_591ED8);
      if ( k == 65 ) //0x41
        res = sub_539C90(dword_591ED8);
    }
  }  
  return res;
}
#define pFunc_dword_591F1C (*(i32_pfunc_3xUI32_t*)(0x00591F1C))
i8 __cdecl sub_539C00(DWORD* data)
{
  
  if ( data[0] == dpid_dword_5899F8 )
  {
    return 1;
  }
  else
  {
	if(pFunc_dword_591F1C==NULL)
		  return 0;
    switch (*(DWORD*)data[3])
	{
	case 81:    
      {       
        pFunc_dword_591F1C(1970, data[2]-4, data[3]+4);
		break;
      }
	case 113:
      {       
        pFunc_dword_591F1C(1969, data[2]-4, data[3]+4);
		break;
      }
	case 97:
      {        
        pFunc_dword_591F1C(1983, data[2]-4, data[3]+4);
		break;
      }
    }    
  }
  return 0;
}

int __cdecl sub_539C90(DWORD *data)
{
  int result; // eax@1
  unsigned int v2; // esi@2
  int v3; // ebp@3
  DWORD *v4; // edi@4

  result = dpid_dword_5899F8;
  if ( data[0] == dpid_dword_5899F8 )
	return dpid_dword_5899F8;
   
  v3 = (int)dword_5899A8;
  for(int i=0; i<dword_589988;i++)
  {
      dp_create_player* v4 = (dp_create_player*)&dword_5899A8[i];      
        
        result = data[0];
		if ( v4->dpID == data[0] )
        {
          v4->clock = dword_5897F0();          
		  result = v4->clock;
        }        
  }
  
  return result;
}