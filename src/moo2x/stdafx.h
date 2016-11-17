#pragma once
//#define WINVER 0x0400
//#define _WIN32_WINNT 0x0400
#include	<Windows.h>

#define DPLAY_USE_IN_DLL
#ifdef DPLAY_USE_IN_DLL
#undef IDIRECTPLAY2_OR_GREATER //MOO2 use dplay ver 1 !!!
//#define IDIRECTPLAY2_OR_GREATER 
#include <dplay.h> 	// directplay main
//#include <dplobby.h>   // the directplay lobby
#else
#define _FACDP  0x877
#define MAKE_DPHRESULT( code )    MAKE_HRESULT( 1, _FACDP, code )
#define DPERR_BUFFERTOOSMALL            MAKE_DPHRESULT(  30 )
#define DPERR_NOMESSAGES                MAKE_DPHRESULT( 190 )
#endif

#include <dsound.h>

#include	<ddraw.h>
#include	<process.h>

// STL stuff
#include	<string>
#include	<map>

#include <errno.h>	
#include <io.h> //findfirst findnext...
#include <time.h> //ctime localtime struct tm

#define MSL	512

//////////////////////////////////////////////////////////////////////////
#include	"Types.h"
#include	"Patcher.h"
#include	"Log.h"
#include	"Config.h"
//////////////////////////////////////////////////////////////////////////
#include	"MOX.h"
#include	"struct.h"
#include	"FunctionDef.h"
#include	"Platform.h"
#include	"HACCESS.h"


#include <assert.h>


