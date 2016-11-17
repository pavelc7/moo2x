#include	"stdafx.h"
#pragma pack(push, 1)
namespace	nPrivate
{
	// Primary surface
	LPDIRECTDRAWSURFACE	PrimarySurface = NULL;
	LPDIRECTDRAWSURFACE	TempSurface = NULL;

	// Color shifts
	int		rshift;
	int		gshift;
	int		bshift;

	int sx;//=640;
	int sy;//=480;
	// Shift from mask calculator
	int GetMaskInfo (DWORD Bitmask, int* lpShift)
	{
		int Precision, Shift;

		Precision = Shift = 0;
		//count the zeros on right hand side
		while (!(Bitmask & 0x01L))
		{
			Bitmask >>= 1;
			Shift++;
		}

		//count the ones on right hand side
		while (Bitmask & 0x01L)
		{
			Bitmask >>= 1;
			Precision++;
		}
		*lpShift = Shift;
		return Precision;
	}

	void	BlitSurfaceToScreen(LPDIRECTDRAWSURFACE lpSurface)
	{
		HRESULT hr;

		PALETTEENTRY	palette[256];

		RECT rt;
		POINT p = {0, 0};
		ClientToScreen(g_hWnd, &p);
		rt.left   = 0 + p.x;
		rt.top    = 0 + p.y;

		// TODO: Fix me
		//int sx,sy;
		//sx=800;sy=600;
		//sx=640,sy=480;
		
		rt.right  = sx + p.x;
		rt.bottom = sy + p.y;

		// Lock buffers
		DDSURFACEDESC	srcDesc = {0};
		srcDesc.dwSize = sizeof(srcDesc);
		DDSURFACEDESC	dstDesc = {0};
		dstDesc.dwSize = sizeof(dstDesc);

		DDPIXELFORMAT DDpf = {0};
		int BytesPerPixel;
		DDpf.dwSize = sizeof(DDpf);
		TempSurface->GetPixelFormat(&DDpf);
		BytesPerPixel = DDpf.dwRGBBitCount / 8;
		
		if(g_pPalette)
			g_pPalette->GetEntries(0, 0, 256, &palette[0]);

		lpSurface->Lock(NULL, &srcDesc, DDLOCK_READONLY, NULL);
		TempSurface->Lock(NULL, &dstDesc, DDLOCK_WRITEONLY, NULL);

		BYTE * srcData = (BYTE*)srcDesc.lpSurface;
		BYTE * dstData = (BYTE*)dstDesc.lpSurface;
		for (DWORD y1 = 0; y1 < srcDesc.dwHeight; y1++)
		{
			BYTE * srcLine = srcData;
			BYTE * dstLine = dstData;

			for (DWORD x1 = 0; x1 < srcDesc.dwWidth; ++x1)
			{
				tagPALETTEENTRY	& pal = palette[*(srcLine++)];

				DWORD	finalColor = 0;
				finalColor |= ((pal.peRed   << rshift) & DDpf.dwRBitMask);
				finalColor |= ((pal.peGreen << gshift) & DDpf.dwGBitMask);
				finalColor |= ((pal.peBlue  << bshift) & DDpf.dwBBitMask);

				BYTE * byteBuf = (BYTE *)&finalColor;

				for (int i = 0; i < BytesPerPixel; ++i)
					*(dstLine++) = byteBuf[i];
			}

			srcData += srcDesc.lPitch;
			dstData += dstDesc.lPitch;
		}

		TempSurface->Unlock(dstDesc.lpSurface);
		lpSurface->Unlock(srcDesc.lpSurface);

		do 
		{
			hr = PrimarySurface->Blt(&rt, TempSurface, NULL, DDBLT_WAIT, NULL);

			if (hr == DDERR_SURFACELOST)
				hr = RestoreSurfaces();
		} while(hr == DDERR_WASSTILLDRAWING);

		if (lpSurface != g_lpPrimarySurface)
		{
			do 
			{
				hr = g_lpPrimarySurface->Blt(NULL, lpSurface, NULL, DDBLT_WAIT, NULL);

				if (hr == DDERR_SURFACELOST)
					hr = RestoreSurfaces();
			} while(hr == DDERR_WASSTILLDRAWING);
		}
	}
}

// Hooked CreateDirectDraw function
HWND	CreateDirectDraw(DWORD videoMode)
{
	nPrivate::sx=800;//640;
	nPrivate::sy=600;//480;

	MEMORYSTATUS	memStatus;
	GlobalMemoryStatus(&memStatus);
	g_globalAvailableMemory = memStatus.dwAvailPhys;

	// No idea what this doing.
	dword_578E20 = &byte_579350;

	if (FAILED(DirectDrawCreate(NULL, &g_pDirectDraw, NULL)))
	{
		QuitCleanup();

		DestroyWindow(g_hWnd);
		g_hWnd = 0;

		return (HWND)0;
	}
	nConfig::Render_FullScreen=0;
	if (nConfig::Render_FullScreen)
	{
		g_pDirectDraw->SetCooperativeLevel(g_hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);		
	} else
	{
		g_pDirectDraw->SetCooperativeLevel(g_hWnd, DDSCL_NORMAL);
	}
	pLog.Printf("videomode = %ld\n",videoMode);
	// Choose screen resolution
	//videoMode=5;
	switch (videoMode)
	{
	case 0:
		g_screen_res_x = 320;
		g_screen_res_y = 200;
		break;
	case 2:
		g_screen_res_x = 320;
		g_screen_res_y = 240;
		break;
	case 4:
		g_screen_res_x = 640;
		g_screen_res_y = 480;
		break;
	case 5:
		g_screen_res_x = nPrivate::sx;
		g_screen_res_y = nPrivate::sy;
		break;
	}
	nPrivate::sx=g_screen_res_x;
	nPrivate::sy=g_screen_res_y;
	g_screen_stride = g_screen_res_x * g_screen_res_y;
	sub_528740(g_screen_stride, g_screen_res_y);

	// Huh!?
	word_57931C = 0;//unused???

	DDSURFACEDESC	desc = {0};
	desc.dwSize = sizeof(desc);

	if (nConfig::Render_FullScreen)
	{
		g_pDirectDraw->SetDisplayMode(g_screen_res_x, g_screen_res_y, 8);

		// For full screen mode create primary surface with 1 backbuffer
		desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		desc.dwBackBufferCount = 1;
	} else
	{
		// Create windowed primary surface
		desc.dwFlags = DDSD_CAPS;
		desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		g_pDirectDraw->CreateSurface(&desc, &nPrivate::PrimarySurface, NULL);

		// Create "fake" primary surface for MOO2 with 8 bit palette
		memset(&desc, 0, sizeof(desc));
		desc.dwSize = sizeof(desc);
		desc.dwWidth  = g_screen_res_x;
		desc.dwHeight = g_screen_res_y;
		desc.dwFlags  = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
		desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		desc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_RGB;
		desc.ddpfPixelFormat.dwRGBBitCount = 8;
	}

	if (g_pDirectDraw->CreateSurface(&desc, &g_lpPrimarySurface, NULL) != DD_OK)
	{
		QuitCleanup();

		DestroyWindow(g_hWnd);
		g_hWnd = 0;

		return (HWND)0;
	}

	if (!nConfig::Render_FullScreen)
	{
		// Set clipper for primary surface
		LPDIRECTDRAWCLIPPER	pcClipper;
		g_pDirectDraw->CreateClipper(0, &pcClipper, NULL);
		pcClipper->SetHWnd(0, g_hWnd);
		nPrivate::PrimarySurface->SetClipper(pcClipper);

		// Create additional surface with primary surface properties
		DDSURFACEDESC	desc2 = {0};
		desc2.dwSize = sizeof(desc2);
		desc2.dwWidth  = g_screen_res_x;
		desc2.dwHeight = g_screen_res_y;
		desc2.dwFlags  = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		desc2.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;

		DDPIXELFORMAT	pFormat = {0};
		pFormat.dwSize = sizeof(pFormat);
		nPrivate::PrimarySurface->GetPixelFormat(&pFormat);

		desc2.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		desc2.ddpfPixelFormat.dwFlags = DDPF_RGB;
		desc2.ddpfPixelFormat.dwRGBBitCount = pFormat.dwRGBBitCount;
		desc2.ddpfPixelFormat.dwRBitMask = pFormat.dwRBitMask;
		desc2.ddpfPixelFormat.dwGBitMask = pFormat.dwGBitMask;
		desc2.ddpfPixelFormat.dwBBitMask = pFormat.dwBBitMask;

		g_pDirectDraw->CreateSurface(&desc2, &nPrivate::TempSurface, NULL);
		if (nPrivate::TempSurface == NULL)
		{
			QuitCleanup();

			DestroyWindow(g_hWnd);
			g_hWnd = 0;

			return (HWND)0;
		}

		// Get color information
		nPrivate::GetMaskInfo(desc2.ddpfPixelFormat.dwRBitMask, &nPrivate::rshift);
		nPrivate::GetMaskInfo(desc2.ddpfPixelFormat.dwGBitMask, &nPrivate::gshift);
		nPrivate::GetMaskInfo(desc2.ddpfPixelFormat.dwBBitMask, &nPrivate::bshift);
	}

	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwWidth  = g_screen_res_x;
	desc.dwHeight = g_screen_res_y;
	desc.dwFlags  = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
	desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	desc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_RGB;
	desc.ddpfPixelFormat.dwRGBBitCount = 8;

	if (nConfig::Render_FullScreen)
	{
		DDSCAPS	caps;
		caps.dwCaps = DDSCAPS_BACKBUFFER;

		// TODO: Error checking
		g_lpPrimarySurface->GetAttachedSurface(&caps, &g_lpBackBufferSurface);
	} else
	{
		// Create back buffer surface (fake)
		g_pDirectDraw->CreateSurface(&desc, &g_lpBackBufferSurface, NULL);
		if (g_lpBackBufferSurface == NULL)
		{
			QuitCleanup();

			DestroyWindow(g_hWnd);
			g_hWnd = 0;

			return (HWND)0;
		}
	}

	// Create first buffer surface
	g_pDirectDraw->CreateSurface(&desc, &g_lpSurface1, NULL);
	if (g_lpSurface1 == NULL)
	{
		QuitCleanup();

		DestroyWindow(g_hWnd);
		g_hWnd = 0;

		return (HWND)0;
	}

	// Create second buffer surface
	g_pDirectDraw->CreateSurface(&desc, &g_lpSurface2, NULL);
	if (g_lpSurface2 == NULL)
	{
		QuitCleanup();

		DestroyWindow(g_hWnd);
		g_hWnd = 0;

		return (HWND)0;
	}

	if (g_pPalette != NULL)
	{
		g_pPalette->Release();
		g_pPalette = NULL;
	}

	// Create palette
	if (g_pDirectDraw->CreatePalette(DDPCAPS_ALLOW256|DDPCAPS_8BIT/* 0x44*/, &g_pGlobalPalette, &g_pPalette, NULL) != DD_OK)
	{
		QuitCleanup();

		DestroyWindow(g_hWnd);
		g_hWnd = 0;

		return (HWND)0;
	}

	// Set palette
	if (nConfig::Render_FullScreen)
		g_lpPrimarySurface->SetPalette(g_pPalette);

	g_error_status = 0;

	// Huh?
	video_Set_Page_Off();
	graphics_fill(0, 0, nPrivate::sx-1, nPrivate::sy-1, 0);

	video_Set_Page_Back();
	graphics_fill(0, 0, nPrivate::sx-1, nPrivate::sy-1, 0);

	// Huh #5&6?
	sub_5287A0();
	sub_5287C0();

	// Huh #7?
	video_screen_refresh_flag = 0;

	graphics_Reset_Window();

	return g_hWnd;
}

// Hooker FlipSurface function
void	FlipSurface()
{
	if (nConfig::Render_FullScreen)
	{
		HRESULT hr;
		do 
		{
			hr = g_lpPrimarySurface->Flip(NULL, 0);

			if (hr == DDERR_SURFACELOST)
				hr = RestoreSurfaces();
		} while(hr == DDERR_WASSTILLDRAWING);
	} else
	{
		nPrivate::BlitSurfaceToScreen(g_lpBackBufferSurface);
	}

	FrameCount++;
}

// UnlockAppropriateRenderBuffer function
HRESULT	UnlockAppropriateRenderBuffer() //0x005281A0
{
	HRESULT hr = S_OK;

	switch (bRenderBuffer)
	{
	case 0:
		hr = g_lpPrimarySurface->Unlock(NULL);

		if (!nConfig::Render_FullScreen)
			nPrivate::BlitSurfaceToScreen(g_lpPrimarySurface);

		break;
	case 1:
		hr = g_lpSurface1->Unlock(NULL);
		break;
	case 2:
		hr = g_lpSurface2->Unlock(NULL);
		break;
	}

	// TODO: Error checking
	if ( SUCCEEDED( hr) )
		return hr;
	switch(hr)
	{
	case DDERR_SURFACELOST:
		exit_Exit_With_Message("Unlock Surface Lost");
		break;
	case DDERR_NOTLOCKED:
		exit_Exit_With_Message("Unlock Not Locked");
		break;		
	case DDERR_INVALIDRECT:
		exit_Exit_With_Message("Unlock Invalid Rect");
		break;
	case DDERR_INVALIDOBJECT:
		exit_Exit_With_Message("Unlock Invalid Object");
		break;
	case DDERR_INVALIDPARAMS:
		exit_Exit_With_Message("Unlock Invalid Params");
		break;
	case DDERR_GENERIC:
		exit_Exit_With_Message("Unlock Generic");
		break;
	}

	return hr;
}

// Primary surface unlocker
void	UnlockPrimarySurface()
{
	g_lpPrimarySurface->Unlock(NULL);

	if (!nConfig::Render_FullScreen)
		nPrivate::BlitSurfaceToScreen(g_lpPrimarySurface);
}

void	BlitToBackbuffer()
{
	RECT	rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = g_screen_res_x;
	rect.bottom = g_screen_res_y;

	while (g_lpBackBufferSurface->BltFast(0, 0, g_lpSurface1, &rect, DDBLTFAST_WAIT) != DD_OK);
}

void	DrawCursorXY(i16 x, i16 y)
{
	if (word_578AC4 > 0)
	{
		if (word_578AC4 != dword_578B3C)
		{
			sub_526F80();
			dword_578B3C = word_578AC4;
		}

		RECT rect;
		rect.left = 0;
		rect.top  = 0;

		if (x + 24 >= g_screen_res_x - 1)
			rect.right = g_screen_res_x - x - 1;
		else
			rect.right = 24;

		if (y + 24 >= g_screen_res_y - 1)
			rect.bottom = g_screen_res_y - y - 1;
		else
			rect.bottom = 24;

		DDCOLORKEY	key;
		key.dwColorSpaceHighValue = 0;
		key.dwColorSpaceLowValue  = 0;
		g_pCursorSurface1->SetColorKey(8, &key);

		g_lpPrimarySurface->BltFast(x, y, g_pCursorSurface1, &rect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

		if (!nConfig::Render_FullScreen)
			nPrivate::BlitSurfaceToScreen(g_lpPrimarySurface);
	}
}

void	DrawAutoCursor()
{
	RECT rect;
	rect.left = 0;
	rect.top  = 0;

	if (cached_mouse_x + 24 >= g_screen_res_x - 1)
		rect.right = g_screen_res_x - cached_mouse_x - 1;
	else
		rect.right = 24;

	if (cached_mouse_y + 24 >= g_screen_res_y - 1)
		rect.bottom = g_screen_res_y - cached_mouse_y - 1;
	else
		rect.bottom = 24;

	g_lpPrimarySurface->BltFast(cached_mouse_x, cached_mouse_y, g_pCursorSurface2, &rect, DDBLTFAST_WAIT);

	if (!nConfig::Render_FullScreen)
		nPrivate::BlitSurfaceToScreen(g_lpPrimarySurface);
}

void	CreateCursorSurfaces()
{
	DDSURFACEDESC	desc2 = {0};
	desc2.dwSize = sizeof(desc2);
	desc2.dwWidth  = 24;
	desc2.dwHeight = 24;
	desc2.dwFlags  = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc2.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
	desc2.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	desc2.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_RGB;
	desc2.ddpfPixelFormat.dwRGBBitCount = 8;

	g_pDirectDraw->CreateSurface(&desc2, &g_pCursorSurface1, NULL);

	g_pDirectDraw->CreateSurface(&desc2, &g_pCursorSurface2, NULL);
}

HRESULT	RestoreSurfaces()
{
	HRESULT hr = S_OK;

	if (g_lpPrimarySurface != NULL)
		g_lpPrimarySurface->Restore();

	if (g_lpBackBufferSurface != NULL)
		g_lpBackBufferSurface->Restore();

	if (g_lpSurface1 != NULL)
		g_lpSurface1->Restore();

	if (g_lpSurface2 != NULL)
		g_lpSurface2->Restore();

	if (g_pCursorSurface1 != NULL)
		g_pCursorSurface1->Restore();

	if (g_pCursorSurface2 != NULL)
		hr = g_pCursorSurface2->Restore();

	if (nPrivate::PrimarySurface != NULL)
		nPrivate::PrimarySurface->Restore();

	if (nPrivate::TempSurface != NULL)
		nPrivate::TempSurface->Restore();

	FrameCount = 0;
	if (video_screen_refresh_flag == 3)
	{
		video_Set_Refresh_Stencil();
	}

	return hr;
}

void	CleanupSurfaces()
{
	if (g_pDirectDraw != NULL)
	{
		if (g_lpPrimarySurface != NULL)
		{
			g_lpPrimarySurface->Release();
			g_lpPrimarySurface = NULL;
		}

		if (g_lpSurface1 != NULL)
		{
			g_lpSurface1->Release();
			g_lpSurface1 = NULL;
		}

		if (g_lpSurface2 != NULL)
		{
			g_lpSurface2->Release();
			g_lpSurface2 = NULL;
		}

		if (g_pPalette != NULL)
		{
			g_pPalette->Release();
			g_pPalette = NULL;
		}

		if (g_pCursorSurface1 != NULL)
		{
			g_pCursorSurface1->Release();
			g_pCursorSurface1 = NULL;
		}

		if (g_pCursorSurface2 != NULL)
		{
			g_pCursorSurface2->Release();
			g_pCursorSurface2 = NULL;
		}

		if (nPrivate::PrimarySurface != NULL)
		{
			nPrivate::PrimarySurface->Release();
			nPrivate::PrimarySurface = NULL;
		}

		if (nPrivate::TempSurface != NULL)
		{
			nPrivate::TempSurface->Release();
			nPrivate::TempSurface = NULL;
		}

		g_pDirectDraw->Release();
		g_pDirectDraw = NULL;
	}
}

void	ForceCursorPos(i16 x, i16 y)
{
	MouseX = x;
	MouseY = y;

	if (!nConfig::Render_FullScreen)
	{
		// Quick hack to ignore window size
		POINT p = {0, 0};
		ClientToScreen(g_hWnd, &p);

		x += (i16)p.x;
		y += (i16)p.y;
	}

	SetCursorPos(x, y);
}
//#define primSurfaceDesc (*(DDSURFACEDESC*)(0x00578E70))
#define dword_5792F0 (*(i32*)(0x005792F0))
#define dword_578E80 (*(i32*)(0x00578E80))
//#define dword_578E94 (*(i32*)(0x00578E94))

HRESULT __cdecl LockPrimarySurface() //0x00528470
{
	HRESULT hr=S_OK;
	DDSURFACEDESC	desc = {0};
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_HEIGHT|DDSD_WIDTH;
	//primSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);//local or global???	
	//assert(primSurfaceDesc.dwSize==108);	
	//primSurfaceDesc.dwFlags = DDSD_HEIGHT|DDSD_WIDTH;	
	//assert(primSurfaceDesc.dwFlags==6);

	g_lpPrimarySurface->GetSurfaceDesc(&desc);

	hr = g_lpPrimarySurface->Lock(NULL,&desc,DDLOCK_SURFACEMEMORYPTR,NULL);

	if(SUCCEEDED(hr)){
		dword_5792F0 = (i32)desc.lpSurface;		
		//assert(dword_578E80==desc.lPitch);
		return desc.lPitch;
	}
	if ( hr != DDERR_SURFACELOST && hr != DDERR_WASSTILLDRAWING )
		hr=0;
	if ( hr != DDERR_SURFACEBUSY && hr != DDERR_INVALIDOBJECT )
		hr=0;
	if ( hr != DDERR_INVALIDPARAMS && hr != DDERR_OUTOFMEMORY )
		hr=0;
	return hr;
}
#define g_lpSurfaceMem (*(i32*)(0x005792F8))

HRESULT __cdecl LockAppropriateRenderBuffer() //0x005282D0
{
	HRESULT hr=S_OK;
	DDSURFACEDESC	desc = {0};
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_HEIGHT|DDSD_WIDTH;
	desc.lPitch = 0;
	//primSurfaceDesc.lPitch = 0;
	//primSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);
	//assert(primSurfaceDesc.dwSize==108);
	//primSurfaceDesc.dwFlags = DDSD_HEIGHT|DDSD_WIDTH;	
	//assert(primSurfaceDesc.dwFlags==6);

	switch(bRenderBuffer)
	{
	case 1:
		g_lpSurface1->GetSurfaceDesc(&desc);
        hr = g_lpSurface1->Lock(NULL,&desc,DDLOCK_WAIT,NULL);
		break;
	case 2:
		g_lpSurface2->GetSurfaceDesc(&desc);
        hr = g_lpSurface2->Lock(NULL,&desc,DDLOCK_WAIT,NULL);
		break;
	case 0:
		g_lpPrimarySurface->GetSurfaceDesc(&desc);
        hr = g_lpPrimarySurface->Lock(NULL,&desc,DDLOCK_WAIT,NULL);
		break;
	default:
		hr = DDERR_INVALIDMODE;
		break;
	}
	if(SUCCEEDED(hr))
	{
		g_lpSurfaceMem = (i32)desc.lpSurface;
        return desc.lPitch;
	}
	switch(hr)
	{
	case DDERR_SURFACELOST:	
		exit_Exit_With_Message("lock Surface Lost");
        return DDERR_SURFACELOST;	
	
	case DDERR_SURFACEBUSY: 
		exit_Exit_With_Message("lock Invalid Rect");
        return DDERR_SURFACEBUSY;
	case DDERR_INVALIDOBJECT:
		exit_Exit_With_Message("lock Invalid Object");
        return DDERR_INVALIDOBJECT;
	case DDERR_INVALIDPARAMS:    
      exit_Exit_With_Message("lock Invalid Params");
      return DDERR_INVALIDPARAMS;
	case DDERR_OUTOFMEMORY:
		exit_Exit_With_Message("lock Not Locked");
      return DDERR_OUTOFMEMORY;
	case DDERR_WASSTILLDRAWING:	 
	default:		
        exit_Exit_With_Message("lock Generic");
        return DDERR_WASSTILLDRAWING;
	}	
	return hr;
}


//
void myWorkerThread( void * param)
{
	for(;;){
		
		Sleep(1000);
		if(global_QuitFlag==1)break;
		printf("date=%d\t",(ui32)MOX__stardate);
		printf("plrs=%d\t",(ui8)MOX__NUM_PLAYERS);
		printf("scrflag=%d\t",(ui32)MOX__FULL_DRAW_SCREEN_FLAG );
		printf("nebs=%d\t",(ui8)MOX__NUM_NEBULAS);
		printf("pl=0x%X\t",(ui32)MOX__player );		
		printf("scr=0x%X \r",(ui32)MOX__current_screen );

	}
	return;
}
//////////////////////////////////////////////////////////////////////////

void	WorkerThread(void*param)
{
	MOX2_Main(param);
}

const	char	*szClassName = "Microprose";
char CmdLine[MSL];
int __stdcall	WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	if (nConfig::Debug_Console)
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	}

	lpCmdLine = GetCommandLine(); //reextract command line
	strcpy(CmdLine,lpCmdLine);
	ParseCommandLine(lpCmdLine);

	WNDCLASS	wndClass = {0};
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
	wndClass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wndClass.lpszMenuName = szClassName;
	wndClass.lpszClassName = szClassName;
	RegisterClass(&wndClass);

	HWND hWindow = FindWindow(szClassName, szClassName);
	if (hWindow != NULL)
	{
		MessageBox(0, "MOO II is already running.", szClassName, MB_ICONERROR);
		//ExitProcess(0);
	}

	int width = 0;
	int height = 0;
	DWORD flags = 0;
	nConfig::Render_FullScreen = 0;
	if (nConfig::Render_FullScreen)
	{
		width = GetSystemMetrics(SM_CXSCREEN);
		height= GetSystemMetrics(SM_CYSCREEN);
		flags = WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_EX_TOPMOST;
	} else
	{
		width = 645;
		height= 500;
																			//added| WS_OVERLAPPEDWINDOW | WS_SIZEBOX
		flags = WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_OVERLAPPEDWINDOW | WS_SIZEBOX;
	}

	g_hWnd = CreateWindowEx(0, szClassName, szClassName, flags, 0, 0, width, height, 0, 0, hInstance, NULL);
	if (g_hWnd == NULL)
		return 0;

	
	UpdateWindow(g_hWnd);

	SetCursor(FALSE);

	CreateDirectDraw(4);

	ThreadHandle = _beginthread(WorkerThread, 0xFFFF, 0);
	
	if (nConfig::Debug_Console)
	{
		HANDLE myThreadHandle = (HANDLE)_beginthread(&myWorkerThread, 0xFFFF, 0);
	}

	MSG	msg;
	bool	flag = false;

	while (GetMessage(&msg, 0, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (dword_579328 == 0)
			flag = true;

		if (flag == true && dword_579328 == 1)
			RestoreSurfaces();

		if (global_QuitFlag == 1)
		{
			CleanupSurfaces();

			DestroyWindow(g_hWnd);

			break;
		}
	}

	return 0;
}

_finddata64i32_t finddata;//local or global???
i32 DIR(const char* a1, char *a2)
{
  pLog.Printf("func.DIR call %s\n",a1);
  static int v2=0; // eax@2 //local or global???
  i32 result; // eax@5  
  if ( *a1 )
    v2 = _findfirst(a1, &finddata);
  else //dead code???
    v2 = _findnext(v2, &finddata); 
  if ( v2 == -1 )
  {
    result = 0;
    *a2 = 0;
  }
  else
  {
    strcpy(a2, finddata.name);
    result = 1;
  }
  pLog.Printf("func.DIR ret  a2=%s res=%d\n",a2,result);
  return result;
}
i32 sub_49E9B0(char* filename, ui16* month, ui16* mday, ui16* year, ui16* hour, ui16* min1, ui16* sec)
{
  pLog.Printf("func.sub_49E9B0 call %s\n",filename);
  struct tm *v7; // eax@1
//  int v8; // edx@1
  int result=0; // eax@1
  result = (i32)sec;
  v7 = get_timestamp_52D3D0(filename);
  if(v7){
	  *year = v7->tm_year+1900;
	  *month = v7->tm_mon;
	  *mday = v7->tm_mday;
	  *hour = v7->tm_hour;
	  *min1 = v7->tm_min;
	  *sec = v7->tm_sec;	  
  }
    
  pLog.Printf("func.sub_49E9B0 ret  %d-%d-%d %d:%d:%d\n",*year,*month,*mday,*hour,*min1,*sec);
  return result;
}
struct tm* get_timestamp_52D3D0(char* filename) //get_timestamp
{
  static struct tm result; // eax@2
  struct tm *ret = 0;
  char * a=0;
  pLog.Printf("func.get_timestamp_52D3D0 call %s\n",filename);
  //struct _finddata_t fdata;//local or global???
  if ( _findfirst(filename, &finddata) == -1 ) //was finddata_57B5D8
  {
    ret = 0;
  }
  else
  {
    a=ctime(&finddata.time_write);
    result = *localtime(&finddata.time_write);
    dword_timestamp_57B5D0 = (i32)&result; //used only in get_timestamp_52D3D0 ?
	ret = &result;
  }
  pLog.Printf("func.get_timestamp_52D3D0 ret  res=%d ctime=%s\n",&result,a);
  return ret;
}
//bool is_file_exists (char* name) {
//  struct stat buffer;     
//  return (stat(name, &buffer) == 0); 
//}
signed int MOX2_Set_Mox_Language_()
{  
  FILE* file; // esi@2
  char buf[MSL]; // [sp+4h] [bp-A0h]@2
  char path[MSL]; // [sp+54h] [bp-50h]@1
  int result = 0;
  result = DIR("language.ini", path);
  //int result = is_file_exists("language.ini");
  //if ( result )
  //{
  file = fopen("language.ini", "rt");
  if(file){
		fgets(buf, 60, file);
		fclose(file);
		result = atoi(buf);
  }
  MOX__settings.config_language = 0;
  //int result = ((i32 (*)(void))0x00483EF0)();
	  
  return result;
}
#define word_578DD8					(*(i16 *)(0x00578DD8))
i16 sub_527630()
{
  __int16 result; // ax@1

  result = word_578DD8;
  word_578DD8 = 0;
  return result;
}
#define word_578B50					(*(i16 *)(0x00578B50))
i16 sub_527640()
{
  __int16 result; // ax@1

  result = word_578B50;
  word_578B50 = 0;
  return result;
}
#define word_572C00					(*(i16 *)(0x00572C00))
#define word_55AFE4					(*(i16 *)(0x0055AFE4))
#define word_55B004					(*(i16 *)(0x0055B004))
#define word_55B040					(*(i16 *)(0x0055B040))
#define word_55B04C					(*(i16 *)(0x0055B04C))
#define word_55B064					(*(i16 *)(0x0055B064))
#define fields_fields_count			(*(i16 *)(0x00572D18))

i16 fields_Clear_Fields()
{
  fields_fields_count = 1;
  word_572C00 = 0;
  word_55AFE4 = -1;
  word_55B004 = 0;
  word_55B040 = 0;
  word_55B04C = -1;
  word_55B064 = 0;
  sub_527630();
  return sub_527640();
}
void sub_533630()
{
  sub_52DC80();
  sub_534B40();
  global_QuitFlag = 1;
}
void exit_Exit_With_Message(const CHAR *lpText)
{
  fields_Clear_Fields();
  g_error_status = 1;
  sub_52D290();
  MessageBox(g_hWnd, lpText, "Microprose", MB_OK);
  pLog.Printf("call exit_Exit_With_Message\n");  
  pLog.Printf("%s\n",lpText);
  sub_533630();
  pLog.Deinitialize();
  DestroyWindow(g_hWnd);
  
  exit(1);
}
void __cdecl ERIC_Eric_Defaults_()  //empty
{
	;
}
void __cdecl nullsub() //empty
{
	;
}
void __cdecl KEN_Ken_Init_()  //004C1D40 empty
{
  ;
}
#define MOX__bill_savegame (*(MOX__bill_savegame_t *)(0x00559478))
//#define word_55947A (*(i16 *)(0x0055947A))
//#define dword_55947C (*(i32 *)(0x0055947C))
//#define dword_559480 (*(i32 *)(0x00559480))

i32 __cdecl BILL_Bill_Init_()
{
  i32 result; // eax@1
  result = 1;
  MOX__bill_savegame.arr[0] = 2;
  MOX__bill_savegame.arr[1] = 0;  
  MOX__bill_savegame.arr[2] = 1;
  MOX__bill_savegame.arr[3] = 1;
  MOX__bill_savegame.arr[4] = 1;
  MOX__bill_savegame.arr[5] = 1;
  return result;
}
/*struct struct1_t //14 words
{
	i16 m_id;
	i16 m_star_id;
	i16 m_3;//x?
	i16 m_4;//
	i16 m_5;
	i16 m_arr[9];
};*/
//struct MOX__moveable_box_t1 //140 bytes 
//{	
//	struct struct1_t m_struct[5]; // 
//};
#define MOX__moveable_box (*(MOX__moveable_box_t *)(0x00597860)) // 140 bytes
i32 __cdecl sub_4F0E90()
{
  signed int result; // eax@1

  (&MOX__moveable_box)[2].arr[3] = 0xC8;//word_59789E = 200; //offset 31
  (&MOX__moveable_box)[2].arr[2] = 0x110;//word_59789C = 272; //offset 30 words
  result = 0x16;//22;
  (&MOX__moveable_box)[2].arr[4] = 0x16;//0x160016// dword_5978A0 = 1441814;  //32words
  (&MOX__moveable_box)[2].arr[5] = 0x16;
  (&MOX__moveable_box)[2].arr[10] = 0xC8;//word_5978AC = 200; //38 words
  (&MOX__moveable_box)[2].arr[11] = 0x27;//word_5978AE = 39;//39words
   
  return result;
}

//#define star_id (*(i16 *)(0x00597862))

#define MOX__moveable_box_order (*(i16 *)(0x0059BD20))
#define word_59C420 (*(i16 *)(0x0059C420))
i16 __cdecl sub_4F0ED0()
{
  //i16 v0; // dx@1
  //int v1; // ecx@2
  //int v2; // eax@2
  //int v3; // eax@2

  //v0 = 0;
  memset(&MOX__moveable_box, 0, 0x8C); // 140 bytes
  (&MOX__moveable_box)[0].arr[0] = 0;
  (&MOX__moveable_box)[1].arr[0] = 1;  
  (&MOX__moveable_box)[2].arr[0] = 2;
  (&MOX__moveable_box)[3].arr[0] = 3;
  (&MOX__moveable_box)[4].arr[0] = 4;
  for(int i=0;i<5;i++)//do
  {
    
    (&MOX__moveable_box_order)[i] = i;
    
    (&MOX__moveable_box)[i].arr[1] = (&word_59C420)[i];//offset 1 word
    (&MOX__moveable_box)[i].arr[4] = -1;//word_597868[v3] = -1; //offset 4
    (&MOX__moveable_box)[i].arr[5] = -1;//word_59786A[v3] = -1; //offset 5
  }
  //while ( v0 < 5 );
  sub_4F0E90();
  return sub_4F0F60();
}


#define word_59C422 (*(i16 *)(0x0059C422))
#define word_59C424 (*(i16 *)(0x0059C424))
#define word_59C426 (*(i16 *)(0x0059C426))
#define word_59C428 (*(i16 *)(0x0059C428))
#define dword_5973EC (*(i32 *)(0x005973EC))
#define MOX__given_chance_to_rename_home_star (*(i32 *)(0x005973E8))
i32* __cdecl HAROLD_Harold_Init_()
{
  int *result; // eax@1

  (&word_59C420)[0] = -1;
  word_59C422 = 0;
  word_59C424 = -1;
  word_59C426 = 0;
  word_59C428 = 0;
  sub_4F0ED0();
  MOX__given_chance_to_rename_home_star = 0;
  result = &MOX__given_chance_to_rename_home_star;
  dword_5973EC = 0;
  return result;
}
#define MOX__cur_map_x (*(i16 *)(0x005A18F0))
#define MOX__cur_map_y (*(i16 *)(0x005A18F8))
#define DIPLODEF__council_winner (*(i16 *)(0x00596010))
#define MOX__global_esc (*(i16 *)(0x00597774))
char* DoSomeInit()
{
  char *result; // eax@1
  MOX__cur_map_x = 0;
  MOX__cur_map_y = 0;
  MOX__current_screen = 0x27; //39;
  DIPLODEF__council_winner = -1; //0xFFFF;
  nullsub();//nullsub_21
  HAROLD_Harold_Defaults_();
  ERIC_Eric_Defaults_();
  result = BILL_Bill_Default_();
  MOX__global_esc = 0;
  return result;
}
#define REPORT__n_reports (*(i16*)(0x005A4EAA))
#define REPORT__reports (*(REPORT__reports_t*)(0x005A4EC0))
#define REPORT__done_player_reports (*(i8*)(0x005A4EB8))
#define REPORT__something_interesting_happened (*(REPORT__something_interesting_happened_t*)(0x005A4EB0))
#define REPORT__done_colonize_check (*(REPORT__done_colonize_check_t*)(0x005A4EA0))
#define REPORT__displayed_gnn (*(i8*)(0x005A4EAC))
#define EVENTS__event_data (*(EVENTS__event_data_t*)(0x005A6460))
#define MSG__msgs (*(MSG__msgs_t*)(0x005A6700))
#define MSG__max_msgs (*(i8*)(0x005A6E08))
#define MSG__first_msg (*(i8*)(0x005A66F0))
#define MSG__n_msgs (*(i8*)(0x005A66F8))



char *__cdecl BILL_Cache_Player_Scientist_Anim_() //0x00407910
{
  return sub_407920(); //char *__cdecl sub_407920()
}
//char __cdecl MAPGEN_Load_Nebula_Pictures_() 0x004B7040
//int __cdecl MAINSCR_Update_Ship_Icons_In_Between_Turns_()  0x04CD550
//__int16 __cdecl FLEETPOP_Reload_Fleet_Box_Data_() 00405990
void __cdecl COLCALC_Rebuild_WFI_List_() //empty 004FBEF0 COLCALC_Rebuild_WFI_List_
{
  ;
}
void __cdecl RUSS_Assert_Settings_() //empty 004D7810
{
  ;
}
bool __cdecl MOVEBOX_Moveable_Box_Selected_(i16 a1) //00513B90 MOVEBOX_Moveable_Box_Selected_
{
	return (&word_59C420)[a1] < (&MOX__moveable_box)[a1].arr[1];
}
//#define bRenderBuffer (*(i8*)(0x00578EDC))
#define copy_to_on_page_flag (*(i16*)(0x00578E60))
void __cdecl video_Set_Page_Off() //0x00528680
{
  bRenderBuffer = 1; //00578EDC byte
  copy_to_on_page_flag = 0; //00578E60 word
}

//
#define memcopy_from_savegame(x,y) memcpy(&x,&savegame.##y,sizeof(savegame.##y));
BYTE __cdecl FILEDEF_Load_Game_(i16 numfile)
{
	helper_clean_ship_array();
	INITGAME_Init_Colonies_Array_();
  
  char filename[MSL];
  
  HAROLD_Save_Mouse_List_(); //int __cdecl HAROLD_Save_Mouse_List_() 004F2190
  
  wsprintf(filename,"SAVE%d.GAM",numfile+1);
  
  RUSS_Mox_Update_(); // 004D7820 RUSS_Mox_Update_ char* __cdecl RUSS_Mox_Update_()
  RUSS_Assert_Settings_();
  FILE* f = fopen(filename, "rb");
  if ( !f )
  {
    RUSS_PFMT_(0, 0, "File read error"); //empty
    ASSERT_hgetch_(); //char __cdecl ASSERT_hgetch_() 0x004AB400
    return 0;
  }
  savegame_t savegame;  
  size_t readed=fread(&savegame, sizeof(savegame_t), 1, f);
  if(savegame.extension==1 && savegame.extension_version==200)
  {
		fread(&savegame.extension,sizeof(WORD),1,f);
		fread(&savegame.extension_version,sizeof(DWORD),1,f);
		fread(&savegame.num_ships,sizeof(WORD),1,f);
		fread(&savegame.ships,SHIP_RECORD_SIZE,savegame.num_ships,f);
		//memcopy_from_savegame(MOX__NUM_SHIPS,num_ships);	
  }
  fclose(f);
  RUSS_Mox_Update_();
  char error_message[MSL];
  char *err_s;
  if ( savegame.first != 0xE0 || readed!=1 || sizeof(savegame_t)!=SAVEGAVE_RECORD_SIZE) //224
  {
    switch ( MOX__settings.config_language )
      {
		
        case 1:
          err_s = "SAVE%d.GAM ist ungßltig! WÊhlen Sie ein anderes Spiel, oder starten Sie ein NEUES Spiel.";
          break;
        case 2:
          err_s = "SAUVEGARDER%d.GAM n'est pas valide! Essayez un autre emplacement de sauvegarde ou commencez une NOUVELLE partie";
          break;
        case 3:
          err_s = "#SAVE%d.GAM no es v<lido! Prueba otro hueco para guardar or comienza una partida NUEVA";
          break;
		case 4:
		  err_s = "Il file SAVE%d.GAM non > valido! Provate a caricarne un altro o iniziate una nuova partita";
		case 0:					
        default:    
		  err_s = "SAVE%d.GAM is invalid! Try another save slot or start a NEW game";
          break;      
    }
    wsprintf(error_message, err_s,numfile+1);
    //GENDRAW_Warning_Box_(a1 + 1, (int)&v43); //usercall
    video_Set_Page_Off(); //
    graphics_fill(0, 0, nPrivate::sx-1, nPrivate::sy-1, 0); //0x00528CC0 i32 __cdecl graphics_Fill(i16 x1, i16 y1, i16 x2, i16 y2, i16 color)
    return 0;    
  }
  
  MISC_Increment_Mouse_Clock_Image_(); //0x0049EA60 i32 __cdecl MISC_Increment_Mouse_Clock_Image_()
  if ( numfile >= 9 )
    numfile = 9;

  memcpy(&MOX__save_game_description+37 * numfile,&savegame.save_game_description,37);
  memcopy_from_savegame(MOX__stardate,stardate);//memcpy(&MOX__stardate,&savegame.stardate,sizeof(savegame.stardate));
  

  memcopy_from_savegame(MOX__game_type,game_type);//memcpy(&MOX__game_type,&savegame.game_type,sizeof(savegame.game_type));
  //memcpy(&moxset,&savegame.moxset,sizeof(savegame.moxset));
  memcopy_from_savegame(MOX__settings,moxset);
//DWORD random_seed=0;
  //fread(&random_seed, sizeof(DWORD), 1, f);
  //RUSS_Mox_Update_();

  //fread(&MOX__NUM_COLONIES, sizeof(WORD), 1, f);
	memcopy_from_savegame(MOX__NUM_COLONIES,num_colonies);//memcpy(&MOX__NUM_COLONIES,&savegame.num_colonies,sizeof(savegame.num_colonies));


  

  memcpy(MOX__colony,&savegame.colonies,MAX_COLONIES*sizeof(MOX__colony_t)); 
  //RUSS_Mox_Update_();

  memcopy_from_savegame(MOX__planet_count,planet_count);//memcpy(&MOX__planet_count,&savegame.planet_count,sizeof(savegame.planet_count));  
  //fread(&MOX__planet_count, sizeof(WORD), 1, f);


  memcpy(MOX__planet,&savegame.planets,MAX_PLANETS*sizeof(MOX__planet_t));     
  //RUSS_Mox_Update_();

  //fread(&MOX__NUM_STARS, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__NUM_STARS,num_stars);//memcpy(&MOX__NUM_STARS,&savegame.num_stars,sizeof(savegame.num_stars));


  memcpy(MOX__star,&savegame.stars,MAX_STARS*sizeof(MOX__star_t));     
    
  memcpy(MOX__leaders,&savegame.leaders,MAX_LEADERS*sizeof(MOX__leader_t));
  
  memcopy_from_savegame(MOX__NUM_PLAYERS,num_players);//memcpy(&MOX__NUM_PLAYERS,&savegame.num_players,sizeof(savegame.num_players));     
  
  //fread(&MOX__NUM_PLAYERS, sizeof(WORD), 1, f);
  memcpy(MOX__player,&savegame.players,MAX_PLAYERS*sizeof(MOX__player_t));     //30024 bytes
 
  memcopy_from_savegame(MOX__NUM_SHIPS,num_ships);//memcpy(&MOX__NUM_SHIPS,&savegame.num_ships,sizeof(savegame.num_ships));     //30024 bytes
  //fread(&MOX__NUM_SHIPS, sizeof(WORD), 1, f);
  
  memcpy(MOX__ship,&savegame.ships,MAX_SHIPS*sizeof(MOX__ship_t));     //64500 bytes

  memcpy(&MOX__moveable_box,&savegame.moveable_boxes,MAX_MOVEABLE_BOXS*sizeof(MOX__moveable_box_t)); //5*28
  //fread(&MOX__moveable_box, 28, 5, f);
  //fread(MOX__moveable_box_order, sizeof(WORD), 5, f);
  memcpy(&MOX__moveable_box_order,&savegame.moveable_box_orderes,MAX_MOVEABLE_BOXS*sizeof(WORD)); //5*2
  //fread(&MOX__PLAYER_NUM, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__PLAYER_NUM,player_num);//memcpy(&MOX__PLAYER_NUM,&savegame.player_num,sizeof(savegame.player_num));
  //fread(&MOX__max_map_scale, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__max_map_scale,max_map_scale);//memcpy(&MOX__max_map_scale,&savegame.max_map_scale,sizeof(savegame.max_map_scale));
  //fread(&MOX__max_zoom_count, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__max_zoom_count,max_zoom_count);//memcpy(&MOX__max_zoom_count,&savegame.max_zoom_count,sizeof(savegame.max_zoom_count));
//  fread(&MOX__star_bg_index, 1, 1, f);
  memcopy_from_savegame(MOX__star_bg_index,star_bg_index);//memcpy(&MOX__star_bg_index,&savegame.star_bg_index,sizeof(savegame.star_bg_index));
  //fread(&MOX__MAP_MAX_X, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__MAP_MAX_X,max_x);//memcpy(&MOX__MAP_MAX_X,&savegame.max_x,sizeof(savegame.max_x));
  //fread(&MOX__MAP_MAX_Y, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__MAP_MAX_Y,max_y);//memcpy(&MOX__MAP_MAX_Y,&savegame.max_y,sizeof(savegame.max_y));
  //RUSS_Mox_Update_();
  //fread(&MOX__fleet_icon_button_count, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__fleet_icon_button_count,fleet_icon_button_count);//memcpy(&MOX__fleet_icon_button_count,&savegame.fleet_icon_button_count,sizeof(savegame.fleet_icon_button_count));
  //fread(MOX__nebula, 20, 1, f);
  memcpy(MOX__nebula,&savegame.nebulas,MAX_NEBULAS*sizeof(MOX__nebula_t));
  //memcopy_from_savegame(MOX__nebula,nebula);//memcpy(&MOX__nebula,&savegame.nebula,sizeof(savegame.nebula));
  //fread(&MOX__NUM_NEBULAS, 1, 1, f);
  memcopy_from_savegame(MOX__NUM_NEBULAS,num_nebulas);//memcpy(&MOX__NUM_NEBULAS,&savegame.num_nebulas,sizeof(savegame.num_nebulas));
  if ( !*MOX__nebula_screen_seg )
    MOX__NUM_NEBULAS = 0;
  //fread(&MOX__bill_savegame, 12, 1, f);
  memcopy_from_savegame(MOX__bill_savegame,bill_savegame);//memcpy(&MOX__bill_savegame,&savegame.bill_savegame,sizeof(savegame.bill_savegame));
  //RUSS_Mox_Update_();
  //fread(&REPORT__n_reports, sizeof(WORD), 1, f);
  memcopy_from_savegame(REPORT__n_reports,n_reports);//memcpy(&REPORT__reports,&savegame.n_reports,sizeof(savegame.n_reports));
  memcopy_from_savegame(REPORT__reports,reports);//fread(REPORT__reports, 950, 1, f);
  memcopy_from_savegame(REPORT__done_player_reports,done_player_reports);//fread(&REPORT__done_player_reports, 1, 1, f);
  memcopy_from_savegame(REPORT__something_interesting_happened,something_interesting_happened);//fread(REPORT__something_interesting_happened, 8, 1, f);
  memcopy_from_savegame(REPORT__done_colonize_check,done_colonize_check);//fread(REPORT__done_colonize_check, 9, 1, f);
  memcpy(&EVENTS__event_data,&savegame.event_datas,MAX_EVENT_DATAS*sizeof(EVENTS__event_data_t)); //fread(EVENTS__event_data, 9, 36, f);
  //RUSS_Mox_Update_();
  memcpy(&MSG__msgs,&savegame.msgs,MAX_MSGS*sizeof(MSG__msgs_t));//fread(MSG__msgs, 18, 100, f);
  memcpy(&MSG__first_msg,&savegame.msgs,MAX_PLAYERS*sizeof(BYTE));//fread(MSG__first_msg, 1, 8, f);
  memcpy(&MSG__max_msgs,&savegame.msgs,MAX_PLAYERS*sizeof(BYTE));//fread(&MSG__max_msgs, 1, 8, f);
  memcpy(&MSG__n_msgs,&savegame.msgs,MAX_PLAYERS*sizeof(BYTE));//fread(MSG__n_msgs, 1, 8, f);
  memcopy_from_savegame(REPORT__displayed_msgs,displayed_msgs);//fread(&REPORT__displayed_msgs, 1, 1, f);
  memcopy_from_savegame(REPORT__displayed_gnn,displayed_gnn);//fread(&REPORT__displayed_gnn, 1, 1, f);
  memcopy_from_savegame(DIPLODEF__COUNCIL_FLAG,council_flag);//fread(&DIPLODEF__COUNCIL_FLAG, sizeof(WORD), 1, f);
  memcopy_from_savegame(DIPLODEF__next_council_meeting_turn,next_council_meeting_turn);//fread(&DIPLODEF__next_council_meeting_turn, sizeof(WORD), 1, f);
  //RUSS_Mox_Update_();
  memcopy_from_savegame(MOX__antaran_resource_level,antaran_resource_level);//fread(&MOX__antaran_resource_level, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__chance_for_antaran_invasion,chance_for_antaran_invasion);//fread(&MOX__chance_for_antaran_invasion, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__disallow_saves_code,disallow_saves_code);//fread(&MOX__disallow_saves_code, sizeof(WORD), 1, f);
  memcopy_from_savegame(EVENTS__last_event_year,last_event_year);//fread(&EVENTS__last_event_year, 4, 1, f);
  memcopy_from_savegame(EVENTS__event_delay_count,event_delay_count);//fread(&EVENTS__event_delay_count, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__antarans,antarans);//fread(&MOX__antarans, 66, 1, f);
  memcopy_from_savegame(MOX__cur_map_scale,cur_map_scale);//fread(&MOX__cur_map_scale, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__cur_map_x,cur_map_x);//fread(&MOX__cur_map_x, sizeof(WORD), 1, f);
  memcopy_from_savegame(MOX__cur_map_y,cur_map_y);//fread(&MOX__cur_map_y, sizeof(WORD), 1, f);
  //fread(&v45, 1000, 1, f);
  
  DoSomeInit();
  random_Set_Random_Seed(savegame.random_seed);
  RUSS_Mox_Update_();
  BILL_Cache_Player_Scientist_Anim_();
  //RUSS_Mox_Update_();
  if ( MOX__NUM_NEBULAS )
	MAPGEN_Load_Nebula_Pictures_();
  MAINSCR_Update_Ship_Icons_In_Between_Turns_();
  if ( MOVEBOX_Moveable_Box_Selected_(2) )
    FLEETPOP_Reload_Fleet_Box_Data_();
  COLCALC_Rebuild_WFI_List_();
  sub_43D290(&savegame.moxset); //0x0043D290 int __cdecl sub_43D290(int a1)
  HAROLD_Confirm_Planet_And_Star_Index_Link_();
  RUSS_Assert_Settings_();
  MAINSCR_Update_Star_Owners_();
  RUSS_Assert_Settings_();
  HAROLD_Load_Leader_Titles_();
    
  for(int i=0;i<MAX_PLAYERS;i++)
  {
	if(savegame.players[i].i8arr[46])  //   if ( *(_WORD *)(MOX__player + v32 + 46) )
    {
		// *(_DWORD *)(MOX__player + v32 + 491) = *(_WORD *)(MOX__player + v32 + 46);
		*((i32*)(&(MOX__player[i].i8arr[491])))=(i32)savegame.players[i].arr[23];
		//*(_WORD *)(MOX__player + v32 + 46) = 0;
		MOX__player[i].arr[23]=0;
    }
  }  
	for(int j=0;j<savegame.num_players;j++)
	{   
      for (int i = 0; i<savegame.num_players;i++){
	   MOX__player[j].i8arr[i+2199] = -1;
	  }      
    }
  
  HAROLD_Restore_Mouse_List_();
  RUSS_Release_All_Blocks_(MOX__screen_seg);
  return 1;
}
//#define random_random_seed (*(ui32*)(0x0057B6F0))
//#define random_dword_57B6FC (*(ui32*)(0x0057B6FC))
ui32 random_random_seed;
ui32 random_dword_57B6FC;
//
ui32 __cdecl random_Set_Random_Seed(ui32 seed)
{  
  random_random_seed = seed;
  return random_random_seed;
}
//
ui32 __cdecl random_set_sub_52D450()
{   
  random_dword_57B6FC = random_random_seed;
  return random_random_seed;
}
ui32 __cdecl random_get_sub_52D460()
{   
  random_random_seed = random_dword_57B6FC;
  return random_dword_57B6FC;
}
ui32 __cdecl random_Get_Random_Seed() //0052D4D0
{
  return random_random_seed;
}
ui32 __cdecl random_Random(ui32 random) //0052D470
{
  ui32 result; // eax@2
  ui32 i; // eax@3

  if ( random )
  {
    i = random_random_seed;
    do
      i = 1103515245 * i + 12345;
    while ( 0xFFFFFFFF / random * random <= i );
    random_random_seed = i;
    result = i / (0xFFFFFFFF / random) + 1;
  }
  else
  {
    result = 1;
  }
  return result;
}

#define memcopy_to_savegame(x,y) memcpy(&savegame.##y,&x,sizeof(savegame.##y));
void __cdecl FILEDEF_Save_Game_(i16 numfile)
{
	
	char filename[MSL];
	savegame_t savegame;
	savegame.random_seed = random_Get_Random_Seed();
	memset(savegame.arr,0,1000);
	HAROLD_Save_Mouse_List_();
	wsprintf(filename,"SAVE%d.GAM",numfile+1);
	RUSS_Assert_Settings_();

	savegame.first = 0xE0; //224
	if ( numfile >= 9 )
		numfile = 9;
	memcpy(&savegame.save_game_description,&MOX__save_game_description+37 * numfile,37);
	memcopy_to_savegame(MOX__stardate,stardate);
	memcopy_to_savegame(MOX__game_type,game_type);
	memcopy_to_savegame(MOX__settings,moxset);
	memcopy_to_savegame(MOX__NUM_COLONIES,num_colonies);
	memcpy(&savegame.colonies,MOX__colony,MAX_COLONIES*sizeof(MOX__colony_t));
	memcopy_to_savegame(MOX__planet_count,planet_count);
	memcpy(&savegame.planets,MOX__planet,MAX_PLANETS*sizeof(MOX__planet_t));
	memcopy_to_savegame(MOX__NUM_STARS,num_stars);
	memcpy(&savegame.stars,MOX__star,MAX_STARS*sizeof(MOX__star_t));     
    memcpy(&savegame.leaders,MOX__leaders,MAX_LEADERS*sizeof(MOX__leader_t));
	memcopy_to_savegame(MOX__NUM_PLAYERS,num_players);
	memcpy(&savegame.players,MOX__player,MAX_PLAYERS*sizeof(MOX__player_t));
	memcopy_to_savegame(MOX__NUM_SHIPS,num_ships);
	memcpy(&savegame.ships,MOX__ship,MAX_SHIPS*sizeof(MOX__ship_t));  
	memcpy(&savegame.moveable_boxes,&MOX__moveable_box,MAX_MOVEABLE_BOXS*sizeof(MOX__moveable_box_t)); 
	memcpy(&savegame.moveable_box_orderes,&MOX__moveable_box_order,MAX_MOVEABLE_BOXS*sizeof(WORD));
	memcopy_to_savegame(MOX__PLAYER_NUM,player_num);
	memcopy_to_savegame(MOX__max_map_scale,max_map_scale);
	memcopy_to_savegame(MOX__max_zoom_count,max_zoom_count);
	memcopy_to_savegame(MOX__star_bg_index,star_bg_index);
	memcopy_to_savegame(MOX__MAP_MAX_X,max_x);
	memcopy_to_savegame(MOX__MAP_MAX_Y,max_y);
	memcopy_to_savegame(MOX__fleet_icon_button_count,fleet_icon_button_count);
	memcpy(&savegame.nebulas,MOX__nebula,MAX_NEBULAS*sizeof(MOX__nebula_t));
	memcopy_to_savegame(MOX__NUM_NEBULAS,num_nebulas);
	memcopy_to_savegame(MOX__bill_savegame,bill_savegame);
	memcopy_to_savegame(REPORT__n_reports,n_reports);
	memcopy_to_savegame(REPORT__reports,reports);
	memcopy_to_savegame(REPORT__done_player_reports,done_player_reports);
	memcopy_to_savegame(REPORT__something_interesting_happened,something_interesting_happened);
	memcopy_to_savegame(REPORT__done_colonize_check,done_colonize_check);
	memcpy(&savegame.event_datas,&EVENTS__event_data,MAX_EVENT_DATAS*sizeof(EVENTS__event_data_t));
	memcpy(&savegame.msgs,&MSG__msgs,MAX_MSGS*sizeof(MSG__msgs_t));
	memcpy(&savegame.msgs,&MSG__first_msg,MAX_PLAYERS*sizeof(BYTE));
	memcpy(&savegame.msgs,&MSG__max_msgs,MAX_PLAYERS*sizeof(BYTE));
	memcpy(&savegame.msgs,&MSG__n_msgs,MAX_PLAYERS*sizeof(BYTE));
	memcopy_to_savegame(REPORT__displayed_msgs,displayed_msgs);
	memcopy_to_savegame(REPORT__displayed_gnn,displayed_gnn);
	memcopy_to_savegame(DIPLODEF__COUNCIL_FLAG,council_flag);
	memcopy_to_savegame(DIPLODEF__next_council_meeting_turn,next_council_meeting_turn);  
	memcopy_to_savegame(MOX__antaran_resource_level,antaran_resource_level);//fread(&MOX__antaran_resource_level, sizeof(WORD), 1, f);
	memcopy_to_savegame(MOX__chance_for_antaran_invasion,chance_for_antaran_invasion);//fread(&MOX__chance_for_antaran_invasion, sizeof(WORD), 1, f);
	memcopy_to_savegame(MOX__disallow_saves_code,disallow_saves_code);//fread(&MOX__disallow_saves_code, sizeof(WORD), 1, f);
	memcopy_to_savegame(EVENTS__last_event_year,last_event_year);//fread(&EVENTS__last_event_year, 4, 1, f);
	memcopy_to_savegame(EVENTS__event_delay_count,event_delay_count);//fread(&EVENTS__event_delay_count, sizeof(WORD), 1, f);
	memcopy_to_savegame(MOX__antarans,antarans);//fread(&MOX__antarans, 66, 1, f);
	memcopy_to_savegame(MOX__cur_map_scale,cur_map_scale);//fread(&MOX__cur_map_scale, sizeof(WORD), 1, f);
	memcopy_to_savegame(MOX__cur_map_x,cur_map_x);//fread(&MOX__cur_map_x, sizeof(WORD), 1, f);
	memcopy_to_savegame(MOX__cur_map_y,cur_map_y);//fread(&MOX__cur_map_y, sizeof(WORD), 1, f);
	if(MOX__NUM_SHIPS>MAX_SHIPS)
	{
		savegame.extension = 1;
		savegame.extension_version = 200;		
		savegame.num_ships=MAX_SHIPS;
	}
	FILE* f = fopen(filename, "wb");
	if(!f)
	{
		char error_message[MSL];
		strcpy(error_message, "Error saving game.\n");//, 0x14u);
		//GENDRAW_Message_Box_(a1, (int)&aErrorSavingGam[20], (int)&v23);
		pLog.Write("Error saving game.\n");
		return;
	}
	size_t written = fwrite(&savegame,sizeof(savegame_t),1,f);
	if(MOX__NUM_SHIPS>MAX_SHIPS)
	{
		memcopy_to_savegame(MOX__NUM_SHIPS,num_ships);

		fwrite(&savegame.extension,sizeof(WORD),1,f);
		fwrite(&savegame.extension_version,sizeof(DWORD),1,f);
		fwrite(&savegame.num_ships,sizeof(WORD),1,f);
		fwrite(&savegame.ships,SHIP_RECORD_SIZE,savegame.num_ships,f);
	}
	fclose(f);
	if(written!=1){
		pLog.Write("Error saving game.\n");
	}
	HAROLD_Restore_Mouse_List_();
    RUSS_Assert_Settings_();
    FILEDEF_Save_Game_Settings_();
}
i8 __cdecl FILEDEF_Save_Game_Settings_() //0043CDB0
{
	RUSS_Assert_Settings_();	
	FILE * f = fopen("MOX.SET", "wb");  
	if (!f)
	{
		return 0;
	}
	i8 res = fwrite(&MOX__settings, sizeof(MOX__set_t), 1, f);
	fclose(f);
	RUSS_Assert_Settings_();		
	return res;
}
void __cdecl FILEDEF_Load_Game_Settings_() //0043CE10
{
  RUSS_Assert_Settings_();
  FILE * f = fopen("MOX.SET", "rb");  
  if (f)
  {
    fread(&MOX__settings, sizeof(MOX__set_t), 1, f);
    fclose(f);
    RUSS_Assert_Settings_();
  }
  HAROLD_Set_Sound_Volume_To_Settings_();
}
//int __cdecl INITGAME_Set_Default_Game_Settings_()

size_t __cdecl msize_hooked(void *ptr)//0x00546F50
{
	return _msize(ptr);
}
void* __cdecl realloc_hooked(void *lpMem, size_t size)//0x00546D80
{
	return realloc(lpMem,size);
}
void *__cdecl malloc_hooked(size_t size)//0x005421A0
{
	void* ptr=0;
	
	ptr = malloc(size);
	//pLog.Printf("malloc ptr=0x%p\t0x%X\t(%d) bytes\n",ptr,size,size);
	
	return ptr;
}
void __cdecl free_hooked(void *ptr)//0x00542270
{
	//pLog.Printf("free   ptr=0x%p\n",ptr);
	free(ptr);	
}
void *__cdecl do_malloc_hooked(size_t size) // 0x0052E9C0
{
  void *ptr; // esi@1
  ptr = //malloc(size);//
	  malloc_in_orion95(size);
  //pLog.Printf("malloc ptr=0x%p\t0x%X\t(%d) bytes\n",ptr,size,size);
  if ( !ptr )
    exit_Exit_With_Message("Null Pointer");      
  return ptr;
}

i16 __cdecl SHIPSTAK_Ship_Has_Special_(i16 nShip, ui16 bit) //0x00460600
{  
  if ( struct_Test_Bit_Field_(MOX__ship[nShip].d.specials, bit)
    && !struct_Test_Bit_Field_(MOX__ship[nShip].damage_flags, bit) )
    return 1; 
  return 0;
}

#define capture_release_flag (*(i16*)(0x0055AFC8)) //used in 2 funcs??
void __cdecl capture_Release_Version() //0520200
{
  capture_release_flag = 1;
}
i16 __cdecl capture_Check_Release_Version()//00520210
{
  return capture_release_flag;
}
i8 __cdecl FILEDEF_Update_Game_Settings_Version_Number_() //0043D300
{  
  FILE * f = fopen("MOX.SET", "rb");
  bool need_init=false;
  if (!f){
	  need_init = true;
  }else{
    fclose(f);
	FILEDEF_Load_Game_Settings_();
    MOX__settings.version = 130;
    i8 saved = FILEDEF_Save_Game_Settings_();
    if(!saved) 
		need_init = true;
  }  
  if (need_init)
    return INITGAME_Set_Default_Game_Settings_();
  return 1;
}
/*
i16 __cdecl fields_Assign_Auto_Function(i32_cdecl_funcname_void_t* pFunc, i16 a2)
{
  __int16 result; // ax@1

  word_55AFDC = 1;
  pFunc_dword_5762F8 = pFunc;
  result = a2;
  if ( a2 <= 0 || a2 >= 10 )
    word_55AFF4 = 2;
  else
    word_55AFF4 = a2;
  return result;
}*/
#define pFunc_dword_55AFD8 (*(i32_cdecl_funcname_void_t*)(0x0055AFD8))
i32_cdecl_funcname_void_t __cdecl fields_Assign_Update_Function(i32_cdecl_funcname_void_t pFunc)//00523E00
{  
  pFunc_dword_55AFD8 = pFunc;
  return pFunc;
}
#define pFont_word_5787B4 (*(i16*)(0x005787B4))
i16 __cdecl fonts_Set_PSTR_Style(i16 font_style)//0x00526D00
{  
  pFont_word_5787B4 = font_style;
  return pFont_word_5787B4;
}

#define MOX__global_screen_seg (*(i32**)(0x0059B5A8))
#define MOX__techname_seg (*(i32**)(0x0059B31C))
#define ALLOC__special_names_seg (*(i32**)(0x00595390))
#define MOX__global_cache_seg (*(i32**)(0x0059B5B8))
#define MOX__global_cache_size (*(i32*)(0x0059BA10))
#define MOX__global_data_seg (*(i32**)(0x00597410))
void __cdecl ALLOC_Allocate_Data_Space_()//0x00500B20
{
  size_t size=0;
  const int MULTI=256;
  MOX__global_screen_seg = allocate_Allocate_Space(1790000 * MULTI);
  MOX__techname_seg = allocate_Allocate_Space(30000 * MULTI);
  ALLOC__special_names_seg = allocate_Allocate_Space(1000 * MULTI);

  size=sizeof(MOX__planet_t)*NEWMAX_PLANETS;
  MOX__planet = (MOX__planet_t*)allocate_Allocate_Space(size);//6120);
  struct_Clear_Structure_(MOX__planet, size);
  
  size=sizeof(MOX__player_t)*NEWMAX_PLAYERS;
  MOX__player = (MOX__player_t*)allocate_Allocate_Space(size);//30024);
  struct_Clear_Structure_(MOX__player, size);//30024u);
  
  size=sizeof(MOX__colony_t)*NEWMAX_COLONIES;
  MOX__colony = (MOX__colony_t*)allocate_Allocate_Space(size);//90250);
  struct_Clear_Structure_(MOX__colony, size);//90250u);

  MOX__nebula_screen_seg = allocate_Allocate_Space(256000 * MULTI);
  MOX__global_cache_seg = buffer_Allocate_Buffer_Space(1525000 * MULTI);
  MOX__global_cache_size = 1525000 * MULTI;
  MOX__global_data_seg = allocate_Allocate_Dos_Space(10240 * MULTI);
  
  size=sizeof(MOX__nebula_t)*NEWMAX_NEBULAS;
  MOX__nebula = (MOX__nebula_t*)allocate_Allocate_Dos_Space(size);//20);
  struct_Clear_Structure_(MOX__nebula, size);
  
  size=sizeof(MOX__star_t)*NEWMAX_STARS;
  MOX__star = (MOX__star_t*)allocate_Allocate_Dos_Space(size);//8136);
  struct_Clear_Structure_(MOX__star, size);
  
  size=sizeof(MOX__ship_t)*NEWMAX_SHIPS;
  MOX__ship = (MOX__ship_t*)allocate_Allocate_Dos_Space(size);//64500);
  struct_Clear_Structure_(MOX__ship, size);
  
  size=sizeof(MOX__leader_t)*NEWMAX_LEADERS;
  MOX__leaders = (MOX__leader_t*)allocate_Allocate_Dos_Space(size);//3953);
  struct_Clear_Structure_(MOX__leaders, size);
}
void no_memory_sub_52E9F0(size_t size)//0x0052E9F0 not hooked yet
{
  char buf[MSL]; 
  wsprintf(buf, "Couldn't Allocate %d Increase Virtual",size);
  exit_Exit_With_Message(buf);
}
i32* allocate_Allocate_Dos_Space(size_t size)//0x0052E8E0
{  
  i32* ptr; // esi@3
  i32* result; // eax@5
    
  if ( size & 1 )
    size = (size & 0xFFFFFFFC) + 4;
  ptr = (i32*)do_malloc_hooked(size + 12);
  if ( !ptr )
    no_memory_sub_52E9F0(size);
  result = ptr + 3;
  ptr[0] = 0;
  ptr[1] = size;
  ptr[2] = 0;
  return result;
}
i32* allocate_Allocate_Space(size_t size)//0x0052E880
{
  i32* ptr; // esi@3
  i32* result; // eax@5
    
  if ( size & 1 )
    size = (size & 0xFFFFFFFC) + 4;
  ptr = allocate_Allocate_Dos_Space(size + 12);
  if ( !ptr )
    no_memory_sub_52E9F0(size);
  result = ptr + 3;
  ptr[0] = 0;
  ptr[1] = size;
  ptr[2] = 0;
  return result;
}
i32* allocate_Allocate_Space_No_Header(size_t size) //0x0052EA60
{
  i32* ptr  = (i32*)do_malloc_hooked(size);
  if ( !ptr )
  {
    ptr = allocate_Allocate_Dos_Space(size);
    if ( !ptr )
      no_memory_sub_52E9F0(size);
  }
  return ptr;
}
ui32 __cdecl random_from_time_sub_52D4E0() //0x0052D4E0
{   
  return random_Set_Random_Seed((ui32)time(0));
}
#define random_dword_59BA08 (*(ui32*)(0x0059BA08))
ui32 __cdecl MOX2_Set_Game_Random_Seed_() //0x00483CC0
{  
  capture_Check_Release_Version();
  if ( random_dword_59BA08 )
    return random_Set_Random_Seed(random_dword_59BA08);
  else
    return random_from_time_sub_52D4E0();  
}
i32 __cdecl video_Set_Refresh_Full() //0x00527C80
{
  video_screen_refresh_flag = 0;
  video_screen_dump_flag = 0;
  return 0;
}
void __cdecl video_Set_Refresh_Stencil() //0x00527CD0
{
  video_screen_refresh_flag = 3;
  video_screen_dump_flag = 0;
}
void __cdecl video_Set_Refresh_Differential()//0x00527D00
{
  video_screen_refresh_flag = 2;
  video_screen_dump_flag = 0;
}
void __cdecl video_Set_Page_Back() //0x005286A0
{
  bRenderBuffer = 2;
  copy_to_on_page_flag = 0;
}

int __cdecl video_Copy_Back_To_Off()//00528700
{  
  RECT r={0,0,g_screen_res_x,g_screen_res_y};  
  return g_lpSurface1->BltFast(0, 0, g_lpSurface2, &r, DDBLTFAST_WAIT);//  16		   
}

int __cdecl video_Copy_Off_To_Back()//005286C0
{
  RECT r={0,0,g_screen_res_x,g_screen_res_y};  
  return g_lpSurface2->BltFast(0, 0, g_lpSurface1, &r, DDBLTFAST_WAIT);//  16		   	  
}
#define palette_byte_558B40 (*(ui8*)(0x00558B40))
#define step_byte_558B44 (*(ui8*)(0x00558B44))

void __cdecl ERIC_Fast_Fade_In_() //0x004BA740
{  
  if ( palette_byte_558B40>=90 )
  {
    video_Set_Refresh_Full();    
    for(i16 i=(ui8)palette_byte_558B40;i>=0;i-=10/*10*/)
    {
      palstore_Darken_Palette_(i);
      palette_byte_558B40 = (ui8)i;
      RUSS_Mox_Update_();
	  //Sleep(10);
    }    
  }
  video_Set_Refresh_Stencil();
}
void __cdecl ERIC_Fast_Fade_Out_()//0x004BA690
{
//  __int16 v0; // bx@2
  bool flag; // [sp+Bh] [bp-1h]@1

  flag = false;
  capture_Check_Release_Version();
  if ( palette_byte_558B40<=10 )
  {
    video_Set_Refresh_Full();
    if ( (i16)((ui8)palette_byte_558B40) <= 100 )
    {
      flag = true;
      for(i16 i=(ui8)palette_byte_558B40;i<=100;i+=(ui8)step_byte_558B44)
      {
        palstore_Darken_Palette_(i);
        palette_byte_558B40 = (ui8)i;
        RUSS_Mox_Update_();
		//Sleep(10);
      }      
    }
    if ( flag )
    {
      video_Set_Page_Back();
      graphics_fill(0, 0, nPrivate::sx-1, nPrivate::sy-1, 0);
      video_Copy_Back_To_Off();
      palette_Clear_Palette_Changes(0, 255);
      MOX2__TOGGLE_PAGES_();
    }
    video_Set_Refresh_Stencil();
  }
}
/*#define byte_57934E ((ui8*)(0x0057934E))
HRESULT __cdecl palstore_Darken_Palette_(int percent) // 0052B850
{
  signed int v1; // edi@1
  int v2; // esi@1
  int v3; // eax@2
  ui8 v5; // [sp+4h] [bp-404h]@2
  ui8 v6; // [sp+5h] [bp-403h]@2
  ui8 v7[6]; // [sp+6h] [bp-402h]@2
  PALETTEENTRY palentry; // [sp+Ch] [bp-3FCh]@3
  //palentry.
  int v1 = 4;
  int v2 = 100 - percent;
  for(int i=4;i<1024;)
  {
    v3 = (ui8)byte_57934E[i + 3];
    i += 4;
    *(&v5 + i) = 4 * v2 * v3 / 100;
    *(&v6 + i) = 4 * v2 * (ui8)byte_57934E[i] / 100;
    v7[i] = 4 * v2 * (ui8)byte_57934E[i + 1] / 100;
  }  
  return g_pPalette->SetEntries(0,1,255,&palentry);	  
}*/
i8 __cdecl FILEDEF_Read_Moo_Custom_Player_Data_()//0x0043D1C0
{
  return 0;
}
#define video_checking_byte_5792FC (*(i8*)(0x005792FC))
i8 __cdecl video_Toggle_Pages()//0x00527EA0
{ 	
  video_checking_byte_5792FC = 1;
  SendMessageA(g_hWnd, WM_USER_UPDATE /*0x401u*/, 0, 0);
  i8 result = video_checking_byte_5792FC;
  while ( video_checking_byte_5792FC == 1 )
    //Sleep(0)
	;
  return result;
}
i8 __cdecl MOX2__TOGGLE_PAGES_()//0x00483C80
{
  RUSS_Mox_Update_();
  return video_Toggle_Pages();
}
#define MOX__mox_font_colors_offset (*(ui8*)(0x0059BA6E))
#define  font_bytes_5A5328 ((ui8*)(0x005A5328)) // [8]; //local, but return address
ui8* MISC_Font_Colors2_(int a1, __int16 type, __int16 colors, __int16 palette) //0x0049DDB0
{  
  ui8* result; // eax@12
  memset(font_bytes_5A5328,0,8);  
  DWORD v4=0;
  v4=MAKELONG(MAKEWORD(palette,0),MAKEWORD(palette,palette));
  switch ( type )
  {
    case 0:
    case 1:
      font_bytes_5A5328[0] = (ui8)colors;
      /*LOBYTE(v4) = palette;
      BYTE1(v4) = palette;
      v5 = v4 << 16;
      LOWORD(v5) = v4;
      *(_DWORD *)&font_bytes_5A5328[1] = v5;
      *(_WORD *)&font_bytes_5A5328[5] = v4;*/
	  font_bytes_5A5328[1] = (ui8)palette;
	  font_bytes_5A5328[2] = (ui8)0;
	  font_bytes_5A5328[3] = (ui8)palette;
	  font_bytes_5A5328[4] = (ui8)palette;
	  font_bytes_5A5328[5] = (ui8)palette;
	  font_bytes_5A5328[6] = (ui8)palette;
      font_bytes_5A5328[7] = (ui8)palette;
      break;
    case 2:
    case 3:
      font_bytes_5A5328[0] = (ui8)colors;
      /*LOBYTE(v4) = palette;
      BYTE1(v4) = palette;
      v6 = v4 << 16;
      LOWORD(v6) = v4;
      *(_DWORD *)&font_bytes_5A5328[1] = v6;
      *(_WORD *)&font_bytes_5A5328[5] = v4;*/
	  font_bytes_5A5328[1] = (ui8)palette;
	  font_bytes_5A5328[2] = (ui8)0;
	  font_bytes_5A5328[3] = (ui8)palette;
	  font_bytes_5A5328[4] = (ui8)palette;
	  font_bytes_5A5328[5] = (ui8)palette;
	  font_bytes_5A5328[6] = (ui8)palette;
      font_bytes_5A5328[7] = (ui8)palette;
      if ( MOX__mox_font_colors_offset )
      {
        font_bytes_5A5328[2] = MOX__mox_font_colors_offset;
      }
      else
      {
        i16 n = colors + 1;
        if ( n >= palette )
          n = palette;
        font_bytes_5A5328[2] = (ui8)n;
      }
      break;
    case 4:
      font_bytes_5A5328[0] = (ui8)colors;
     /* BYTE1(a1) = palette;
      LOBYTE(a1) = palette;
      v8 = a1 << 16;
      LOWORD(v8) = a1;
      *(_DWORD *)&font_bytes_5A5328[1] = v8;
      *(_WORD *)&font_bytes_5A5328[5] = a1;*/
	  font_bytes_5A5328[1] = (ui8)palette;
	  font_bytes_5A5328[2] = (ui8)0;
	  font_bytes_5A5328[3] = (ui8)palette;
	  font_bytes_5A5328[4] = (ui8)palette;
	  font_bytes_5A5328[5] = (ui8)a1;
	  font_bytes_5A5328[6] = (ui8)0;
	  // *(WORD *)&font_bytes_5A5328[5] = a1;
      font_bytes_5A5328[7] = (ui8)palette;
      if ( MOX__mox_font_colors_offset )
        font_bytes_5A5328[2] = MOX__mox_font_colors_offset;
      else
        font_bytes_5A5328[2] = palette - 1;
      break;
    case 5:
      font_bytes_5A5328[1] = (ui8)colors;
	  font_bytes_5A5328[2] = (ui8)palette - 1;
      font_bytes_5A5328[3] = (ui8)palette - 1;
      font_bytes_5A5328[4] = (ui8)palette - 1;
      font_bytes_5A5328[5] = (ui8)palette;
      font_bytes_5A5328[6] = (ui8)palette;
      font_bytes_5A5328[7] = (ui8)palette;      
      break;
    default:
      break;
  }
  result = font_bytes_5A5328;
  MOX__mox_font_colors_offset = 0;
  return result;
}
i16 __cdecl get_type_color_sub_4F1E30()
{
  signed __int16 result; // ax@2
//  signed __int16 v1; // [sp+2h] [bp-2h]@0

  switch ( MOX__cur_map_scale )
  {
    case 10:
      result = 3;
      break;
    case 15:
      result = 2;
      break;
    case 20:
      result = 2;
      break;
    case 30:
      result = 1;
      break;
    default:
      result = 1;
      break;
  }
  return result;
}
//void __cdecl fonts_Print(int a1, int a2, char* pstr) //0x00525910
//{
//  fonts_Print_Display(a2, a1, a1, a2, pstr, 0);
//}
void RUSS_PFMT_(int a1, int a2, const char *format, ...)//0x004D7850 //was empty, i guess it was for debug
{
  va_list args;
  char buf[MSL];
  va_start ( args, format ); 
  vsprintf(buf,format,args);
  va_end(args);
  pLog.Printf("RUSS_PFMT_ %d %d %s\n",a1,a2,buf);  
}
//MAPGEN_Universe_Generation_(v5, galaxy_size, a3);
#define MOX__previous_screen (*(i8*)(0x00597AD0))
#define byte_5A6378 (*(i8*)(0x005A6378))
//MOX2_Main = (MOX2_Main_t)
#define MOX2__skip_intro (*(i8*)(0x005662B4)) //used in parseCMDline, mox2_main
#define TEN_byte_55B1C0 (*(i8*)(0x0055B1C0))
#define MOX2__quick_start (*(i8*)(0x005662AC))
#define MOX2__pan_main_menu_panel (*(i8*)(0x005A6379))
#define MOX__mouse_list_arrow (*(i32*)(0x00558D10))
#define MOX__refresh_mode (*(i8*)(0x0059B5A4))

#define MOX__temp_string ((char*)(0x00596230))
i32	MOX2_Main(void* param)//0x00483470
{
  signed int result; // eax@5
  /*int v1; // eax@8
  int v2; // [sp+Ch] [bp-9Ch]@8
  int v3; // [sp+10h] [bp-98h]@8
  char v4; // [sp+14h] [bp-94h]@5
  char v5; // [sp+88h] [bp-20h]@1*/

  MOX__previous_screen = -1;//-1;
  i16 a=20;//word_59C458 = 20; local
  i16 b=20;//word_5A1830 = 20; local
  byte_5A6378 = -1; //-1 not loacl
  i16 c=1;//word_59D6C2 = 1; //local
  i8 d=1;//byte_5A49F0 = 1; //local
  capture_Release_Version();
  c=1;//word_59D6C2 = 1;
  a=10;//word_59C458 = 10;
  b=20;//word_5A1830 = 20;
  d=1;//byte_5A49F0 = 1;
  char buf[MSL];
  i32* MOX__sound_buffer = allocate_Allocate_Space(0x64000);//new int[0x64000];//allocate_Allocate_Space(0x64000); //local
  if ( DIR("MOX.SET", buf) )
  {
    FILEDEF_Load_Game_Settings_();
    if ( MOX__settings.version != 130 )
      FILEDEF_Update_Game_Settings_Version_Number_();
  }
  else
  {
    INITGAME_Set_Default_Game_Settings_();
  }
  MOX2_Set_Mox_Language_();
  char fontfile[MSL];
  HAROLD_Get_Font_File_(fontfile);
  sound_Set_Audio_Buffer((i32)MOX__sound_buffer);
  result = init_Init_Drivers(4, fontfile);
  if ( !result )
  {
	  return 0;
  }
    MOX2_Set_Mox_Alt_Path_();
    //check_ten_sub_4D7D30();
    fields_Assign_Update_Function((i32_cdecl_funcname_void_t)&RUSS_Mox_Update_);
	HANDLE updateThreadHandle = (HANDLE)_beginthread(&UpdateThread, 0xFFFF, 0);
    fonts_Load_Palette(5, 0, 255);
    mouse_Set_Mouse_List(&MOX__mouse_list_arrow, 1);
    fonts_Set_PSTR_Style(2);
    MOX__game_type = 0;
    HAROLD_Set_Sound_Volume_To_Settings_();
    ALLOC_Allocate_Data_Space_();
    MOX2__pan_main_menu_panel = 1;

    if ( !MOX2__skip_intro )
    {
      play_intro_sub_425A70();
      fields_Assign_Update_Function((i32_cdecl_funcname_void_t)&RUSS_Mox_Update_);
      fonts_Load_Palette(5, 0, 255);
      mouse_Set_Mouse_List(&MOX__mouse_list_arrow, 1);
    }
    JIM_Play_Background_Music_();
    RUSS_Mox_Update_();
    MOX2_Set_Game_Random_Seed_();
    MOX__refresh_mode = 0;
    video_Set_Refresh_Full();
    MOX__mox_font_colors_offset = 117;
    ui8 * v1 = MISC_Font_Colors2_(1, 4, 211, 220);//usercall
    //v2 = *(_DWORD *)v1;
    //v3 = *(_DWORD *)(v1 + 4);
    fonts_Set_Font_Style(4, (i32)v1); //??
    video_Set_Page_Off();
	
//	char str[MSL];
	char *pstr = "Loading Master of Orion II, please wait....";
	char *pnull=0;
	switch ( MOX__settings.config_language )
    {
      case 1:
        pstr = "Master of Orion II wird geladen, bitte warten....";//, 0x30u);
        //v10 = *(_WORD *)".";
        //pnull = &aMasterOfOrionI[50];
        break;
      case 2:
        pstr = "Chargement de Master of Orion II en cours, veuillez patienter....";//, 0x40u);
        //v13 = *(_WORD *)".";
        //v2 = &aChargementDeMa[66];
        break;
      case 3:
        pstr = "Cargando Master of Orion II, espera, por favor...";//, 0x30u);
        //v10 = *(_WORD *)".";
        //v2 = &aCargandoMaster[50];
        break;
      case 4:
        pstr = "Caricamento di Master of Orion II. Attendere, prego...";//, 0x34u);
        //v11 = *(_WORD *)"..";
        //v12 = aCaricamentoDiM[54];
        //v2 = &aCaricamentoDiM[55];
        break;
      default:
        //pstr = "Loading Master of Orion II, please wait....";//, 0x2Cu);
        //v2 = &aLoadingMasterO[44];
        break;
    }
	
	
	fonts_Print(5, 10, pstr);//00525910 fonts_Print
	fonts_Print(5, 30, CmdLine);//00525910 fonts_Print
    MOX2__TOGGLE_PAGES_();
	
    LOADER_Load_Pictures_();
    MOX2_Load_Mox_Global_Strings_();
	Sleep(1000);
    RUSS_Mox_Update_();
    MOX__temp_string[0] = 0;
	
	DWORD addr;
//	ui8 data[2];
	struct Limits	limits2[]=
	{
		0x00524607, nPrivate::sx-1,2,
		0x0052460C, nPrivate::sy-1,2,
		0x005246B0, nPrivate::sx-1,2,
		0x005246B5, nPrivate::sy-1,2,
		NULL,NULL,NULL
	};
	PatchData(limits2);
	if ( MOX2__quick_start && FILEDEF_Read_Moo_Custom_Player_Data_() )
    {		
      //i16 v3 = MOX__settings.field_D7;
		//004B73DD  E8 2E 5F 07 00
		//
		//DWORD
			addr=0x004B73D2;
		//ui8 data[2];
		/*data[0]=0xEB; data[1]=0x5F;
		nPatcher::PatchData((void*)addr,(void*)data,2);
		addr=0x004B7441;
		data[0]=0x12;
		nPatcher::PatchData((void*)addr,(void*)data,1);
		addr=0x004B7448;
		data[0]=0x10;
		nPatcher::PatchData((void*)addr,(void*)data,1);*/
		INITGAME_Init_New_Game_(MOX__settings.game_difficulty, MOX__settings.galaxy_size, MOX__settings.galaxy_age, MOX__settings.num_players);
      MOX__current_screen = scr_Reports_Screen;//39;
    }
    else
    {
      MOX__current_screen = scr_Main_Menu_Screen;//10;
    }
    KEN_Ken_Init_();
    //sub_4D7D80(a1, (int)v2);
    MOX2_Screen_Control_();	
    //sub_4D7DA0();
	  
//	char str_bye[MSL];
	pstr = "Thanks for playing Master of Orion ][";
    switch ( MOX__settings.config_language )
    {
      case 1:
        pstr = "Danke, da- Sie Master of Orion II gewôhlt haben.";//, 0x30u);
        //v5 = "";
        //v4 = &byte_5A6380[48];
        //goto LABEL_24;
		break;
      case 2:
		pstr ="Merci de jouer ð Master of Orion II";
        //memcpy(byte_5A6380, "Merci de jouer ð Master of Orion II", 0x24u);
        break;
      case 3:
		pstr = "Gracias por jugar con Master of Orion II";
        //memcpy(byte_5A6380, "Gracias por jugar con Master of Orion II", 0x28u);
        //v5 = "";
        //v4 = &byte_5A6380[40];
        //goto LABEL_24;
		break;
      case 4:
		pstr =   "Grazie per aver giocato a Master of Orion II !";
        //memcpy(byte_5A6380, "Grazie per aver giocato a Master of Orion II !", 0x2Eu);
        //v5 = "";
        //v4 = &byte_5A6380[46];
//LABEL_24:
        //*v4 = *v5;
        break;
      default:
        //memcpy(byte_5A6380, "Thanks for playing Master of Orion ][", 0x26u);
		
        break;
    }
  exit_Exit_With_Message(pstr); 
  return 0;
}
#define path_alt_byte_55AE00 ((char*)(0x0055AE00))
i32 __cdecl set_path_sub_51D0F0(const char *path)//0x0051D0F0
{  
  strcpy(path_alt_byte_55AE00,path);
  int size = strlen(path_alt_byte_55AE00);
  assert(size<=50); //path_alt_byte_55AE00 has 50 bytes size
  if(size>0 && path_alt_byte_55AE00[size-1]!='\\'){
	  path_alt_byte_55AE00[size]='\\';
	  path_alt_byte_55AE00[++size]=0;
  }
  return size;
}
void __cdecl MOX2_Set_Mox_Alt_Path_()//00483CE0
{
  char buf[MSL];
  char *pstr="ORIONCD.INI not found. Please Re-Install Orion ][.";
  if ( !DIR("orioncd.ini", buf) )
  {   
    switch ( MOX__settings.config_language )
    {
    case 1:
        pstr="ORIONCD.INI nicht gefunden. Bitte Orion II neu installieren.";
        break;
    case 2:
        pstr="ORIONCD.INI introuvable. Veuillez rÂinstaller Orion II.";
        break;
    case 3:
        pstr="ORIONCD.INI no se ha encontrado. Vuelve a instalar Orion ][.";
        break;
	case 4:
		pstr="Non trovo il file ORIONCD.INI. Bisogna reinstallare Orion II.";
    default:          
        break;
    }
    exit_Exit_With_Message(pstr);
    return;    
  }
  FILE* f = fopen("orioncd.ini", "rt");
  fgets(buf, 512, f);
  fclose(f);
  int size=strlen(buf);
  while(size>0 && 
	  (buf[size-1]=='\n'||buf[size-1]=='\r'||buf[size-1]=='\\')
	  )size--;
  buf[size]='\\';
  buf[++size]=0;      
  set_path_sub_51D0F0(buf);  
  char alt_path[MSL];
  wsprintf(alt_path, "%sdiplomat.lbx",buf);
  if ( !DIR("diplomat.lbx", buf) && !DIR(alt_path, buf) )
  {   
	  pstr = "Cannot locate all game files.  Please insert CD or re-install.";
      switch ( MOX__settings.config_language )
      {
        case 1:
          pstr = "Nicht alle Spieldateien auffindbar. Bitte CD einlegen oder neu installieren.";
          break;
        case 2:
          pstr = "Echec de la localisation de tous les fichiers de jeu. Veuillez insÂrer le CD ou rÂinstaller.";
          break;
        case 3:
          pstr = "No se puede localizar todos los ficheros del juego.  Por favor mete el CD-ROM o vuelve a instalar.";
          break;
		case 4:
			pstr = "Non riesco a trovare qualche file. Bisogna reinserire il CD o reinstallare il gioco.";
			break;
        default:         
            break;
      }    
      exit_Exit_With_Message(pstr);
	  return;
  }
}

//typedef	void	(* ParseCommandLine_t)(const char * szCommandLine);
//ParseCommandLine = (ParseCommandLine_t)0x005276F0;
void ParseCommandLine(const char * szCommandLine)
{
	char buf[MSL];
	strcpy(buf,szCommandLine);
	_strlwr(buf);
	char*p;
	p=strstr(buf, "/skipintro");
	if(p)
	{
		MOX2__skip_intro=1;
	}
	p=strstr(buf, "/quickstart");
	if(p)
	{
		MOX2__quick_start=1;
	}
	p=strstr(buf, "/ten=");
	if(p)
	{
		p[6]=0;
		TEN_byte_55B1C0 = atoi(&p[5]);
	}
}
char* __cdecl HAROLD_Get_Font_File_(char* fontfile) //0x004F24D0
{
  char* str = 0; // al@2
  switch ( MOX__settings.config_language )
  {
    case 1:
      str="fontsg.lbx";      
      break;
    case 2:
      str ="fontsf.lbx";      
      break;
    case 3:
      str = "fontss.lbx";     
      break;
    case 4:
      str = "fontsi.lbx";
    default:    
      str = "fonts.lbx";      
      break;
  }
  return strcpy(fontfile,str);
}
/*
i32 MOX2_Screen_Control_(i16 a1, i16 a2, i16 a3) //00483830
{
  int result; // eax@1
  int v4; // edx@12

  for(;;)
  {
    result = MOX__current_screen;
    MOX__global_esc = 0;
    switch ( MOX__current_screen )
    {
      case scr_Main_Screen:
        MAINSCR_Main_Screen_(a2, a3, 0);
        MOX__previous_screen = scr_Main_Screen;
        break;
      case scr_Colony_Screen:
        COLONY_Colony_Screen_();
        MOX__previous_screen = scr_Colony_Screen;
        break;
      case scr_Design_Screen:
        DESIGN_Design_Screen_(a3, 0);
        MOX__previous_screen = scr_Design_Screen;
        break;
      case scr_Fleet_Screen:
        FLT1_Fleet_Screen_(a2, a3);
        MOX__previous_screen = scr_Fleet_Screen;
        break;
      case scr_Race_Screen:
        sub_49B760();
        MOX__previous_screen = scr_Race_Screen;
        break;
      case scr_screenQuitNow:
        return result;
      case scr_LoadSave_Game_Popup:
        sub_510DB0();
        MOX__previous_screen = scr_LoadSave_Game_Popup
        break;
      case scr_Info_Screen:
        sub_425E90();
        MOX__previous_screen = scr_Info_Screen;
        break;
      case scr_Main_Menu_Screen:
        MAINMENU_Main_Menu_Screen_(a2);
        MOX__previous_screen = scr_Main_Menu_Screen;
        break;
      case scr_Next_Turn:
        NEXTTURN_Next_Turn_(a1, a2, a3);
        MOX__previous_screen = scr_Next_Turn;
        break;
      case scr_Newgame_Screen:
        if ( MOX__game_type )
        {
          MOX__return_screen = scr_Multi_Player_Screen;
          sub_4867A0(a2);
        }
        else
        {
          if ( sub_4867A0(a2) )
          {
            LOWORD(v4) = MOX__settings.field_D7;
            INITGAME_Init_New_Game_(MOX__settings.field_D4, MOX__settings.field_D6, v4, MOX__settings.field_D5);
          }
          else
          {
            MOX__current_screen = scr_Main_Menu_Screen;
          }
        }
        MOX__previous_screen = scr_Newgame_Screen;
        break;
      case scr_Hall_Of_Fame_Screen:
        sub_4E3880(-1);
        MOX__previous_screen = scr_Hall_Of_Fame_Screen;
        MOX__current_screen = scr_Main_Menu_Screen;
        break;
      case scr_Multi_Player_Screen:
        sub_5056F0(a3, 0);
        MOX__previous_screen = scr_Multi_Player_Screen;
        break;
      case scr_Hotseat_Screen:
        HOTPOP_Hotseat_Screen_(a2, a3);
        MOX__previous_screen = scr_Hotseat_Screen;
        break;
      case scr_Hotseat_Select_Player:
        sub_503A50(a2, a3);
        MOX__previous_screen = scr_Hotseat_Select_Player;
        break;
      case scr_Planet_Data_Screen:
        nullsub_32();
        MOX__previous_screen = scr_Planet_Data_Screen;
        break;
      case scr_Colony_Summary_Screen:
        sub_402800(a2, a3);
        MOX__previous_screen = scr_Colony_Summary_Screen;
        break;
      case scr_Start_Net_Screen:
        if ( NETSTART_Start_Net_Screen_(a2, a3, 0) )
          MOX__current_screen = scr_Reports_Screen;
        MOX__previous_screen = scr_Start_Net_Screen;
        break;
      case scr_Join_Net_Screen:
        if ( NETSTART_Join_Net_Screen_(a2, a3, 0) )
          MOX__current_screen = scr_Reports_Screen;
        scr_Join_Net_Screen = scr_Join_Net_Screen;
        break;
      case scr_Build_Queue_Popup:
        sub_50E490(a2, a3);
        MOX__previous_screen = scr_Build_Queue_Popup;
        break;
      case scr_Distance_Between_Stars_Screen:
        sub_4D9D10(a1, a2);
        MOX__previous_screen = scr_Distance_Between_Stars_Screen;
        MOX__current_screen = MOX__return_screen;
        break;
      case scr_Officers_Screen:
        sub_43E9A0(a2, a3, 0);
        MOX__previous_screen = scr_Officers_Screen;
        break;
      case scr_Planet_Colonization_In_Main_Screen:
        MAINSCR2_Planet_Colonization_In_Main_Screen_(a2, a3);
        MOX__previous_screen = scr_Planet_Colonization_In_Main_Screen;
        break;
      case scr_Building_Outposts_In_Main_Screen:
        sub_4D9B40(a2, a3);
        MOX__previous_screen = scr_Building_Outposts_In_Main_Screen;
        break;
      case scr_Planet_Summary_Screen:
        sub_4488E0(a2);
        MOX__previous_screen = scr_Planet_Summary_Screen;
        break;
      case scr_Colony_Landing_Screen:
        COLLAND_Colony_Landing_Screen_(a1, a2, a3);
        MOX__previous_screen = scr_Colony_Landing_Screen;
        break;
      case scr_Unloading_Transports_In_Main_Screen:
        sub_4DA6D0(a2, a3);
        MOX__previous_screen = scr_Unloading_Transports_In_Main_Screen;
        break;
      case scr_Scrap_Freighters:
        sub_4DA420(a2);
        MOX__previous_screen = scr_Scrap_Freighters;
        break;
      case scr_Tech_Change:
        sub_484040();
        MOX__previous_screen = scr_Tech_Change;
        break;
      case scr_Net_Next_Turn:
        NETTURN_Net_Next_Turn_(a2);
        break;
      case scr_Reports_Screen:
        MAINSCR2_Reports_Screen_(a1, a2);
        MOX__previous_screen = scr_Reports_Screen;
        break;
      case scr_Turn_Summary_Popup:
        TURNSUM_Turn_Summary_Popup_();
        MOX__previous_screen = scr_Turn_Summary_Popup;
        break;
      case scr_Load_Net_Screen:
        if ( NETSTART_Load_Net_Screen_(a1, a2, a3, 0) )
          MOX__current_screen = scr_Reports_Screen;
        MOX__previous_screen = scr_Join_Net_Screen;
        break;
      case scr_Show_Command_Points_Screen:
        MAINSCR2_Show_Command_Points_Screen_(a1, a2, 0);
        MOX__previous_screen = scr_Show_Command_Points_Screen;
        break;
      default:
        continue;
    }
  }
}   */
/*
DoSomeInit();
KEN_Ken_Init_();
HAROLD_Harold_Init_();
ERIC_Eric_Init_();
BILL_Bill_Init_();
*/

/*
ADVCIV
AIBUILD
AIDATA
AIDESIGN
AIDUDES
AILEADER
AIMOVE
AIPOWER
AITECH
ALLOC
Allocate
ANTARANS
ASSERT
BILL
BILLHELP
CMBTMIS
COLBLDG
COLCALC
COLDRAW
COLGCBT
COLLAND
COLMOVE
COLONIZE
COLONY
COLSUM
COMBAT
COMBFIND
COMBINIT
CONTACT
COUNCIL
DESIGN
DIP
DIPLOMAC
ERIC
ERICNET
ESTRINGS
EVANHELP
EVENTS
ExFilterRethrow
exit
FAKEBUTS
farload
fields
FIELDSAV
FILEDEF
FindHandler(EHExceptionRecord *,EHRegistrationNode *,
FindHandlerForForeignException(EHExceptionRecord *,EHRegistrationNode *,
FIREPTS
FLEETPOP
FLT1
FLT2
FMTPARA
fonts
GENDRAW
GEO
GetRangeOfTrysToCheck(
GetTickCount
graphics
HACCESS
HAROLD
HOMEGEN
HOTPOP
inet
INFO
init
INITGAME
Initialize
INITSHIP
INVASION
ISC
j
JIM
KEN
line
LIST
LOADER
LOSTSHIP
MAINMENU
MAINPUPS
MAINSCR
MAINSCR2
MAINTAIN
MAPGEN
MISC
mouse
MOVEBOX
MOX2
MSG
MULTPLAY
NAMESTAR
net
netcode
NETMOX
NETSTART
NETTURN
NEXTTURN
NPCDIPLO
nullsub
OCPOLPUP
OFFICER
palette
palstore
PLNTSUM
RACECUST
RACESEL
random
remap
replace
REPORT
RUSS
SCORE
SCROLL
SETTLER
setvesa
SHIPMOVE
SHIPS
SHIPSTAK
simmodem
SMACK
sound
special
SPY
strings
sub
SYS
TECHINIT
TEXTBOX
timer
TRADE
TURNSUM
type
unknown
video
x
*/
i16 __cdecl get_galaxy_size_sub_4F1050()
{
  i16 result = 3; // ax@1

  result = MOX__settings.galaxy_size;
  if ( MOX__NUM_STARS <= 20)
  {
	  result = 0;
  }
  if ( MOX__NUM_STARS > 20 && MOX__NUM_STARS <= 36)
  {
	  result = 1;
  }
  if ( MOX__NUM_STARS > 36 && MOX__NUM_STARS <= 54)
  {
	   result = 2;
  }
  if ( MOX__NUM_STARS > 54 && MOX__NUM_STARS <= 72 )
  {
       result = 3;
  }    
  //result=5;
  return result;
}
#define MOX__zoom_count (*(i16*)(0x0059BFDC))
i16 __cdecl set_map_maxxy_sub_4B6C40()//0x004B6C40
{
  int size;
  size = get_galaxy_size_sub_4F1050();
  //size = 5;    
  MOX__max_zoom_count = size;
  MOX__zoom_count = size;
  if(size==3)size++;  
  int k=5*(2+size);
  MOX__cur_map_scale = k;
  MOX__max_map_scale = k;   
  MOX__MAP_MAX_X = 253*k/5;
  MOX__MAP_MAX_Y = 200*k/5;
  switch ( size )
  {
    case 0:
      /*MOX__MAP_MAX_X = 506;
      MOX__MAP_MAX_Y = 400;
      MOX__cur_map_scale = 10;
      MOX__max_map_scale = 10;      
      MOX__max_zoom_count = 0;
      MOX__zoom_count = 0;*/
      break;
    case 1:
      /*MOX__MAP_MAX_X = 759;
      MOX__MAP_MAX_Y = 600;
      MOX__cur_map_scale = 15;
      MOX__max_map_scale = 15;      
      MOX__max_zoom_count = 1;
      MOX__zoom_count = 1;*/
      break;
    case 2:
      /*MOX__MAP_MAX_X = 1012;
      MOX__MAP_MAX_Y = 800;
      MOX__cur_map_scale = 20;
      MOX__max_map_scale = 20;
      //LOWORD(v0) = 2;
      MOX__zoom_count = 2;
      MOX__max_zoom_count = 2;*/
      break;
    case 3:
      /*MOX__MAP_MAX_X = 1518;
      MOX__MAP_MAX_Y = 1200;	  
      MOX__cur_map_scale = 30;
      MOX__max_map_scale = 30;      
      MOX__max_zoom_count = 3;
      MOX__zoom_count = 3;*/
      break;
    default:
      return size;
  }
  return size;
}
#define word_596CAE (*(i16*)(0x00596CAE))
#define word_596CB6 (*(i16*)(0x00596CB6))
#define word_59643C (*(i16*)(0x0059643C))
#define word_596444 (*(i16*)(0x00596444))
#define word_596EA6 (*(i16*)(0x00596EA6))
#define word_596EA8 (*(i16*)(0x00596EA8))
i16 zoom_scale_sub_4F0880(i16 galaxy_size)
{
  signed __int16 result; // ax@2
  result = MOX__max_map_scale;
  if(galaxy_size==3)galaxy_size++;
  int k=5*(2+galaxy_size);  
  if ( MOX__max_map_scale >= k )
        result = k;  
  if ( k == MOX__max_map_scale )
  {
    word_596CAE = 0;
    word_596CB6 = 0;
    word_59643C = 0;
    word_596444 = 0;
    word_596EA6 = 0;
	word_596EA8 = 0;
    result = MOX__max_map_scale;
  }
  return result;
}
//zoom_x_sub_4CD5D0(v3, v2);
int __cdecl zoom_x_sub_4CD5D0(__int16 a1, __int16 a2)
{
  return 506 * a1 / a2;
}
//zoom_y_sub_4CD5B0(v6, v5);
int __cdecl zoom_y_sub_4CD5B0(__int16 a1, __int16 a2)
{
  return 400 * a1 / a2;
}

i16 HAROLD_Map_Scale_To_Zoom_Level_()//0x004F1090
{
  i16 result = MOX__max_zoom_count; 

  /*if ( MOX__cur_map_scale >= MOX__max_map_scale)
  {
    result = MOX__max_zoom_count;    
  }*/  
  //int k = MOX__cur_map_scale/5 - 2;
  //if(k==4)k=3;
  //if(k<=0)k=0;
  
  if ( MOX__cur_map_scale == 10 )
  {    
    if ( MOX__max_zoom_count >= 0 )
      result = 0;
  }
  if ( MOX__cur_map_scale == 15 )
  {
     if ( MOX__max_zoom_count >= 1 )
        result = 1;
  }
  if ( MOX__cur_map_scale == 20 )
  {
     if ( MOX__max_zoom_count >= 2 )
       result = 2;
  }
  if ( MOX__cur_map_scale == 30 )
  {
     if ( MOX__max_zoom_count >= 3 )
          result = 3;      
  }
  if ( MOX__cur_map_scale == 35 )
  {
     if ( MOX__max_zoom_count >= 4 )
          result = 4;      
  }
  if ( MOX__cur_map_scale == 40 )
  {
     if ( MOX__max_zoom_count >= 4 )
          result = 4;      
  }
  if ( MOX__cur_map_scale == 45 )
  {
     if ( MOX__max_zoom_count >= 5 )
          result = 5;      
  }
  return result;
}
void helper_clean_ship_array()
{
  memset((void *)MOX__ship, 0, sizeof(MOX__ship_t)*NEWMAX_SHIPS);
  for(int i=0;i<NEWMAX_SHIPS;i++)
  {
	  MOX__ship[i].state_of_ship=5; //deleted
	  MOX__ship[i].destination_star=-1;
	  MOX__ship[i].ship_officer=-1;	  
  }  
  MOX__NUM_SHIPS = 0;
}
i32 __cdecl INITGAME_Init_Ships_Array_()//0x00478B30
{
  helper_clean_ship_array();
  return SHIPMOVE_Allocate_New_Ship_Slots_();
}
i32 __cdecl INITGAME_Init_Colonies_Array_()//0x00478D80
{
  //int v0; // edi@1
  //int result; // eax@2

  
  memset((void *)MOX__colony, 0, sizeof(MOX__colony_t)*NEWMAX_COLONIES);
  //*(_WORD *)(v0 + 90248) = 0;
  for(int i=0;i<NEWMAX_COLONIES;i++)
  {
	  MOX__colony[i].planet_id = -1;
	  MOX__colony[i].owner = -1;
	  MOX__colony[i].officer = -1;
    // *(_WORD *)(361 * MOX__NUM_COLONIES + MOX__colony + 2) = -1;
    // *(_BYTE *)(361 * MOX__NUM_COLONIES + MOX__colony) = -1;    
    // *(_WORD *)(361 * MOX__NUM_COLONIES++ + MOX__colony + 4) = -1;
  }  
  MOX__NUM_COLONIES = 0;
  return (i32)MOX__colony;
}
i8 __cdecl MSG_Allocate_Msg_Slots_() //0x004726E0
{
  i32 count=0;
  for(int i=0;i<MOX__NUM_PLAYERS;i++)
  {
	if ( !MOX__player[i].i8arr[36] )
       ++count;      
  }
  
  memset(&MSG__first_msg,0, MAX_PLAYERS);
  memset(&MSG__max_msgs,0, MAX_PLAYERS);
  memset(&MSG__n_msgs,0, MAX_PLAYERS);
  
  int msgs=0;
  int last=0;
  //i8* pmsgs = &MSG__n_msgs;
  for(int i=0;i<MOX__NUM_PLAYERS;i++)
  {
      (&MSG__n_msgs)[0] = MOX__player[i].i8arr[36];
      if ( !MOX__player[i].i8arr[36] )
      {		
        (&MSG__first_msg)[i] = msgs / count;
        (&MSG__max_msgs)[i] = msgs;
		last=msgs;
		msgs += 100;
      }
  }
  return last;
}
/*
int MAPGEN_Universe_Generation_(int font, __int16 galaxy_size, __int16 a3)//usercall
{  

  MISC_Font_Colors2_(font, 4, 119, 121);  
  MISC_Font_Colors2_(font, 4, 119, 124);
  
  RUSS_Mox_Update_();
  clock_t timestart = clock();
  byte_597A88 = galaxy_size;
  byte_5A1780 = a3;
  byte_56CD90 = 0;
  dword_56CD98[0] = 0;
  dword_56CD98[1] = 0;
  RUSS_Mox_Update_();
  i16 gsize = galaxy_size;
  while ( 1 )
  {
    dword_56CDA8 = (int)byte_559688;
    byte_5A4AF4 = 0;
    dword_56CDD0 = 1;
    ++byte_56CD90;
    RUSS_Mox_Update_();
    RUSS_Mox_Update_();
    RUSS_Mox_Update_();
    word_596C5C = 37;
    word_59BA04 = 37;
    word_5A4B18 = 0;
	memset((void *)MOX__planet, 0, sizeof(MOX__planet_t)*NEWMAX_PLANETS);
    RUSS_Mox_Update_();
    dword_56CDB8 = farload_Far_Reload_Data("starname.lbx", 1, MOX__screen_seg, 0, 829, 15);
    RUSS_Mox_Update_();
    dword_56CDA0 = allocate_Allocate_Next_Block(MOX__screen_seg, 3316);
    RUSS_Mox_Update_();
    RUSS_Mox_Update_();
    if ( word_5A182E )
    {
      dword_56CDD0 = (unsigned __int16)word_5A182E;
      v33 = clock();
    }
    RUSS_Mox_Update_();
    sub_4B6D40(dword_56CDA0);
    if ( dword_56CDD0 == 1 )
    {
      if ( (unsigned __int16)sub_4BA680(dword_56CDD0) )
        ERIC_Fast_Fade_In_();
      video_Set_Page_Back();
      RUSS_Mox_Update_();
      v5 = buffer_Buffer_Reload("raceopt.lbx", 0, MOX__global_cache_seg);
      animate_Draw(0, 0, v5);
      RUSS_Mox_Update_();
      video_Copy_Back_To_Off();
      strcpy(MAPGEN__fill_msg, HAROLD_H_Message_(242));
      v6 = bitmap_Get_Bitmap_Size(325, 30);
      MAPGEN__fill_msg_bitmap = allocate_Allocate_Next_Block(MOX__screen_seg, v6);
      bitmap_Create_Blank_Bitmap(325, 30, MAPGEN__fill_msg_bitmap);
      MAPGEN__fill_msg_ctr = 0;
      RUSS_Mox_Update_();
      sub_5256D0(4, (int)&v30);
      fonts_Set_Outline_Color(2, 1);
      sub_526610(0, 0, (int)MAPGEN__fill_msg, MAPGEN__fill_msg_bitmap);
      RUSS_Mox_Update_();
    }
    if ( byte_5589A4 )
    {
      random_Set_Random_Seed(MOX__settings.random_seed);
      byte_5589A4 = 0;
    }
    switch ( v32 )
    {
      case 0:
        MOX__NUM_STARS = 20;
        v27 = 0x50004u;
        break;
      case 1:
        MOX__NUM_STARS = 36;
        v27 = 0x60006u;
        break;
      case 2:
        MOX__NUM_STARS = 54;
        v27 = 0x90006u;
        break;
      case 3:
        MOX__NUM_STARS = 71;
        v27 = 0x90008u;
        break;
      default:
        sprintf(&Text, "Galaxy size %d is undefined");
        x_exit_Exit_With_Message(&Text);
        break;
    }
    set_map_maxxy_sub_4B6C40();
    RUSS_Mox_Update_();
    RUSS_Mox_Update_();
    MOX__planet_count = 0;
    GEN_nebulas_sub_4B61D0(galaxy_size);
    if ( MOX__NUM_NEBULAS )
      sub_4B5D40();
    HAROLD_Get_Scaled_Value_(MOX__MAP_MAX_X);
    HAROLD_Get_Scaled_Value_(MOX__MAP_MAX_Y);
    result = dword_56CDD0;
    v28 = 0;
    if ( dword_56CDD0 <= 0 )
      break;
    while ( 1 )
    {
      if ( dword_56CDD0 != 1 )
        MOX__settings.random_seed = random_Get_Random_Seed();
      byte_5A4B56 = 0;
      if ( word_5A182E )
        GEN_nebulas_sub_4B61D0(galaxy_size);
      dword_56CDBC = 0;
      dword_56CDB0 = 0;
      byte_55899C = 0;
      byte_5589A0 = 0;
      v8 = (void *)MOX__planet;
      MOX__NUM_COLONIES = 0;
      dword_56CDC0[0] = 0;
      dword_56CDC0[1] = 0;
      dword_56CDC0[2] = 0;
      MOX__planet_count = 0;
      memset(v8, 0, 6120u);
      memset((void *)MOX__star, 0, 8136u);
      memset(v36, 0, sizeof(v36));
      v9 = (int)&Text;
      if ( sub_4B7960(font, SHIWORD(v27), v27, a3, 1, 1) )
        break;
      v10 = 0;
      if ( MOX__NUM_STARS > 0 )
      {
        do
        {
          word_56CDB4 = v10;
          RUSS_Mox_Update_();
          BYTE1(font) = HIBYTE(v10);
          v11 = 0;
          v9 = 113 * v10;
          do
          {
            v12 = v11++;
            *(_WORD *)(MOX__star + 2 * v12 + v9 + 84) = -1;
          }
          while ( v11 < 8 );
          v13 = v9 + MOX__star;
          *(_DWORD *)(v9 + MOX__star + 66) = -1;
          *(_DWORD *)(v13 + 70) = -1;
          *(_BYTE *)(MOX__star + v9 + 41) = -1;
          if ( dword_56CDD0 == 1 && v28 < dword_56CDD0 )
          {
            fonts_Set_Font_Style(4, (int)&v30);
            v14 = MAPGEN__fill_msg_bitmap;
            v15 = fonts_Get_String_Width((int)MAPGEN__fill_msg);
            MISC_Draw_Advancing_Fill_Message_(
              4u,
              &MAPGEN__fill_msg_ctr,
              2 * MOX__NUM_STARS,
              (int)&v30,
              (int)&v34,
              (640 - v15) / 2,
              215,
              (int)MAPGEN__fill_msg,
              v14,
              0);
            MOX2__TOGGLE_PAGES_();
          }
          *(_BYTE *)(MOX__star + v9 + 20) = -1;
          v16 = random_Random(0xAu) - 1;
          if ( v16 >= 3 )
          {
            if ( v16 >= 7 )
              *(_BYTE *)(MOX__star + v9 + 19) = 2;
            else
              *(_BYTE *)(MOX__star + v9 + 19) = 1;
          }
          else
          {
            *(_BYTE *)(MOX__star + v9 + 19) = 0;
          }
          *(_BYTE *)(MOX__star + v9 + 21) = random_Random(3u) - 1;
          v17 = 0;
          do
          {
            v18 = v17++;
            *(_WORD *)(MOX__star + 2 * v18 + v9 + 74) = -1;
          }
          while ( v17 < 5 );
          word_5A4B18 = 0;
          if ( sub_4F0A70(v10) )
          {
            v19 = sub_4B6250(v10);
            v25 = v19;
            v36[v10] = v19;
            if ( v19 > 0 )
            {
              v20 = v19;
              do
              {
                RUSS_Mox_Update_();
                sub_4B81C0(v10);
                --v20;
              }
              while ( v20 );
            }
            if ( word_5A182E )
              ++dword_5A4B00[v25];
          }
          if ( *(_BYTE *)(MOX__star + v9 + 22) != 6 && v36[v10] > 0 && v10 > 0 )
            sub_4B6960(v10, (int)&v26, (int)dword_56CDC0);
          ++v10;
          *(_BYTE *)(MOX__star + v9 + 41) = -1;
          ++dword_56CDB0;
        }
        while ( v10 < MOX__NUM_STARS );
      }
      LOBYTE(font) = 0;
      do
      {
        if ( MOX__NUM_NEBULAS )
          sub_4B8050(a3);
        GEO_Initialize_Black_Hole_Blocks_(45);
        if ( GEO_Map_Is_Connected_(v9) )
          LOBYTE(font) = 1;
        else
          sub_4B7960(font, SHIWORD(v27), v27, a3, 0, 0);
      }
      while ( !(_BYTE)font );
      if ( word_5A182E )
      {
        v21 = word_5A182E == 0;
        ++dword_5A4AE0[(unsigned __int8)MOX__NUM_NEBULAS];
        if ( !v21 )
        {
          if ( (unsigned __int16)word_5A182E < 0x64u || !(v28 % (signed int)((unsigned __int16)word_5A182E / 0x64u)) )
          {
            v22 = v29;
            v29 = clock();
            RUSS_PFMT_(0, 0, "                                                                      ");
            RUSS_PFMT_(
              0,
              0,
              "Stat %d  (start: %d)  (diff %d)  (total: %d.%d)",
              v28,
              v33,
              v29 - v22,
              (v29 - v33) / 100,
              (v29 - v33) % 100);
          }
        }
      }
      ++v28;
      result = v28;
      if ( v28 >= dword_56CDD0 )
        return result;
    }
    fonts_Set_Font_Style(4, (int)&v30);
    v23 = MAPGEN__fill_msg_bitmap;
    v24 = fonts_Get_String_Width((int)MAPGEN__fill_msg);
    MISC_Draw_Advancing_Fill_Message_(
      4u,
      &MAPGEN__fill_msg_ctr,
      2 * MOX__NUM_STARS,
      (int)&v30,
      (int)&v34,
      (640 - v24) / 2,
      215,
      (int)MAPGEN__fill_msg,
      v23,
      1);
    MOX2__TOGGLE_PAGES_();
  }
  return result;
}*/
//#define MOX__ship_icon (*(i16**)(0x005A2CE0))
#define MOX__maps_to_generate (*(i8*)(0x0059791C))
/*
i32 __cdecl INITGAME_Init_New_Game_(__int16 a1, __int16 galaxy_size, __int16 a3, __int16 num_players) //0x00478820 
{
  int v4; // ebx@1
  signed __int16 v5; // bp@4
  int v6; // eax@8
  char v8; // [sp+13h] [bp-1h]@1

  v4 = 0;
  v8 = 0;
  MOX__settings.random_seed = random_Get_Random_Seed();
  MOX2_Set_Game_Random_Seed_();
  MOX__stardate = 35000;
  MOX__NUM_PLAYERS = num_players;
  INITGAME_Init_Ships_Array_();
  RUSS_Mox_Update_();
  INITGAME_Init_Colonies_Array_();
  RUSS_Mox_Update_();
  memset(&MOX__antarans, 0, sizeof(MOX__antarans_t));
  MOX__antarans.arr[32] = 0;
  RUSS_Mox_Update_();
  if ( MOX__game_type != 2 )
    MOX__PLAYER_NUM = 0;
  INITGAME_Init_Leaders_();
  RUSS_Mox_Update_();
  INITGAME_Init_Players_();
  RUSS_Mox_Update_();
  BILL_Cache_Player_Scientist_Anim_();
  RUSS_Mox_Update_();
  MSG_Allocate_Msg_Slots_();
  MOX2_Set_Game_Random_Seed_();
  MOX__settings.random_seed = random_Get_Random_Seed();
  do
  {
    //MAPGEN_Universe_Generation_(v4, galaxy_size, a3);
    RUSS_Mox_Update_();	
	v5=0;
    //v5 = HOMEGEN_Generate_Home_Worlds_();
    RUSS_Mox_Update_();
    if ( v5 )
    {
      video_Set_Page_Off();
      graphics_fill(0, 0, nPrivate::sx-1, nPrivate::sy-1, 0);
      video_Toggle_Pages();
      MAPGEN_Enforce_Planet_Max_(250);
      MAPGEN_Set_Black_Hole_Visited_Flags_();
      HAROLD_Confirm_Planet_And_Star_Index_Link_();
      if ( MOX__settings.field_D9 == 2 )
      {
       /* ADVCIV_Advanced_Civilization_Colonies_();
        ADVCIV_Assign_Advanced_Civilization_Starting_Ships_();*/
      /*}
    }
    if ( MOX__maps_to_generate > (signed __int16)v4 )
    {
      v8 = 1;
      MOX__ship_icon[3432] = 33;
      MOX__ship_icon[3433] = 29;
      MOX__ship_icon[3434] = 25;
      MOX__ship_icon[3435] = 23;
      MOX__ship_icon[3436] = 21;
      MOX__ship_icon[3437] = 17;
      v5 = 0;
      HAROLD_Set_All_Visited_Flags_();
      MAINSCR_Update_Star_Owners_();
      video_Set_Page_Off();
      MAINSCR_Draw_Mini_Main_Screen_(0);
      MOX2__TOGGLE_PAGES_();
      v6 = (signed __int16)v4;
      LOWORD(v4) = v4 + 1;
      RUSS_PFMT_(0, 23, "Map:  %d", v6);
      RUSS_PFMT_(0, 24, "Seed: %ld", MOX__settings.random_seed);
      capture_Screen_Flic_Capture();
    }
  }
  while ( !v5 );
  if ( v8 )
    exit_Exit_With_Message("done");
  RUSS_Mox_Update_();
  HOMEGEN_Make_System_Monsters_();
  RUSS_Mox_Update_();
  INITGAME_Make_System_Monsters_Into_Ships_();
  RUSS_Mox_Update_();
  MAPGEN_Generate_Wormhole_Links_();
  RUSS_Mox_Update_();
  if ( MOX__settings.field_D9 == 2 )
  {
    OFFICER_Allocate_Adv_Civ_Game_Officers_();
    RUSS_Mox_Update_();
  }
  MAPGEN_Assign_Marooned_Heroes_();
  RUSS_Mox_Update_();
  RUSS_Mox_Update_();
  GEO_Initialize_Black_Hole_Blocks_(45);
  RUSS_Mox_Update_();
  GEO_Initialize_Star_In_Nebula_Info();
  RUSS_Mox_Update_();
  DoSomeInit();
  KEN_Ken_Init_();
  HAROLD_Harold_Init_();
  ERIC_Eric_Init_();
  BILL_Bill_Init_();
  MAINSCR_Update_Star_Owners_();
  RUSS_Mox_Update_();
  COLONIZE_Allocate_New_Colony_Slots_();
  RUSS_Mox_Update_();
  COLCALC_Twiddle_Initial_Homeworlds_();
  RUSS_Mox_Update_();
  SHIPMOVE_Allocate_New_Ship_Slots_();
  RUSS_Mox_Update_();
  COLCALC_Compute_All_Star_Colony_Stuff_();
  RUSS_Mox_Update_();
  TRADE_Process_Trade_And_Research_Agreements_();
  RUSS_Mox_Update_();
  if ( MOX__settings.settings[7] )
    INITGAME_Set_Initial_Colony_Auto_Selects_();
  RUSS_Mox_Update_();
  REPORT_Initialize_Reports_();
  RUSS_Mox_Update_();
  COLCALC_Init_Surrenders_();
  RUSS_Mox_Update_();
  RUSS_Mox_Update_();
  FILEDEF_Save_Game_(9);
  RUSS_Mox_Update_();
  FILEDEF_Save_Game_Settings_();
  RUSS_Mox_Update_();
  sub_478E20();
  if ( !MOX__game_type )
    FILEDEF_Save_Moo_Custom_Player_Data_();
  return random_Set_Random_Seed(MOX__settings.random_seed);
}*/

i16* __cdecl strings_Swap_Short(i16* a1, i16* a2)//0052CE60
{
  i16* result; // eax@1
  i16 v3; // dx@1

  result = a1;
  v3 = *a1;
  *a1 = *a2;
  *a2 = v3;
  return result;
}
void __cdecl sub_536EE8(i8 flags, i8* data, int beg, int len)
{
  
  int v6; // edx@1
  
  v6 = g_screen_res_x - beg;
  for(int j=0;j<len;j++)
    for (int i = 0; i<=beg; i++ )
      data[i+j*v6] = flags;    
    
}
void __cdecl sub_536F23(int flags, void *data, int beg, int len)
{  
  int v6; // eax@1  
  int v9; // edx@1 
  v6 = (flags << 16) | (flags & 0xFFFF);    
  v9 = g_screen_res_x - 4 * beg;  
  for(int j=0;j<len;j++)
  {
	for(int i=0;i<beg;i++)
		((i32*)data)[i]=v6;
    //memcpy(data, v6, beg);
    data = (i8 *)data + 4 * beg + v9;  
  }  
}
i8 __cdecl sub_527C60()
{
  i8 result = 0; // al@2

  if ( FrameCount & 1 )
  {
    video_Set_Page_Off();
    result = video_Toggle_Pages();
  }
  return result;
}
i8 __cdecl clear_video_sub_52D290()
{
	
  i8 result; // al@1
  fields_Clear_Fields();
  video_Set_Refresh_Full();
  video_Set_Page_Off();
  graphics_fill(0, 0, nPrivate::sx-1, nPrivate::sy-1, 0);
  video_Toggle_Pages();
  video_Set_Page_Off();
  graphics_fill(0, 0, nPrivate::sx-1, nPrivate::sy-1, 0);
  video_Toggle_Pages();
  result = sub_527C60();
  if ( g_pPalette )
  {
    result = g_pPalette->Release();
    g_pPalette = 0;
  }
  return result;
}
#define animate_clipping_flag (*(i16*)(0x0055B0E0))
#define  video_screen_window_x1 (*(i16*)(0x00578DFC))
#define  video_screen_window_y1 (*(i16*)(0x00578DF4))
#define  video_screen_window_x2 (*(i16*)(0x00578E00))
#define  video_screen_window_y2 (*(i16*)(0x00578DF8))
ui32 __cdecl graphics_fill(i16 x1, i16 y1, i16 x2, i16 y2, i16 flags)//00528CC0
{
 // int v5; // edx@20
  int v6=0; // ecx@21
 // int v7; // ebp@21
//  int v8; // edi@21
  int v9; // esi@25
 // int v10; // ebx@25
  int v11; // ST28_4@26
 // char v13; // [sp-10h] [bp-2Ch]@32
  int data; // [sp-Ch] [bp-28h]@32
//  int v15; // [sp-8h] [bp-24h]@32
 // int v16; // [sp-4h] [bp-20h]@32
 // int v17; // [sp+10h] [bp-Ch]@20
 // int v18; // [sp+14h] [bp-8h]@20

  g_screen_res_x = LockAppropriateRenderBuffer();
  if ( x2 < x1 )
    strings_Swap_Short(&x1, &x2);
  if ( y2 < y1 )
    strings_Swap_Short(&y1, &y2);
  if ( animate_clipping_flag == 1 )
  {
    if ( x1 >= video_screen_window_x2 || x2 <= video_screen_window_x1 )
      return UnlockAppropriateRenderBuffer();
    if ( y1 >= video_screen_window_y2 || y2 <= video_screen_window_y1 )
      return UnlockAppropriateRenderBuffer();
    if ( x1 < video_screen_window_x1 )
      x1 = video_screen_window_x1;
    if ( y1 < video_screen_window_y1 )
      y1 = video_screen_window_y1;
    if ( x2 > video_screen_window_x2 )
      x2 = video_screen_window_x2;
    if ( y2 > video_screen_window_y2 )
      y2 = video_screen_window_y2;
  }
  if ( video_screen_refresh_flag >= 2 )
    setvesa_Add_Square_To_Scan_List_(x1, y1, x2, y2);
  int x1_4 = ( i16)(x1 / 4); 
  int x2_4 = ( i16)(x2 / 4);
  int width,height;
  if ( x2_4 == x1_4 )
  {
    height = y2 - y1 + 1;
    width = x2 - x1 + 1;
    data = g_lpSurfaceMem + g_screen_res_x * y1 + x1;  	
  }
  else
  {
    int k1 = 4 - (x1 & 3);
    width = x2_4 - x1_4 - 1;
    int k2 = (x2 & 3) + 1;
    if ( k1 == 4 )
    {
      k1 = 0;
      width = x2_4 - x1_4;
    }
    if ( k2 == 4 )
    {
      k2 = 0;
      ++width;
    }
    v9 = g_screen_res_x * y1;
    height = y2 - y1 + 1;
    if ( v6 )
    {
      v11 = flags;
      sub_536EE8(flags, (i8*)x1 + v9 + g_lpSurfaceMem, k1, y2 - y1 + 1);
      sub_536F23(v11, (void *)(v9 + g_lpSurfaceMem + 4 * x1_4 + 4), width, height);
    }
    else
    {
      if ( width )
        sub_536F23(flags, (void *)(v9 + g_lpSurfaceMem + 4 * x1_4), width, y2 - y1 + 1);
    }
    if ( !k2 )
      return UnlockAppropriateRenderBuffer();    
    width = k2;
    data = v9 + g_lpSurfaceMem + 4 * x2_4;    
  }
  sub_536EE8(flags, (i8*)data, width, height);
  return UnlockAppropriateRenderBuffer();
}


int __stdcall WndProc(HWND hWnd, ui32 Msg, WPARAM wParam, LPARAM lParam)
{  
  WORD KeyState[129];
  KeyState[0]=0;  
  
  switch (Msg){
	case WM_SIZE://added
		//nPrivate::sx=LOWORD(lParam);
		//nPrivate::sy=480*LOWORD(lParam)/640;
	  return 0;
    case WM_ACTIVATEAPP:
    {      
      dword_579328 = wParam;
      break;//return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
	case WM_DESTROY:
      PostQuitMessage(0);
      break;//return DefWindowProc(hWnd, Msg, wParam, lParam);  
	case  WM_USER_UPDATEMOUSE: 	  
      dword_55C08C = 1;
      return 1;
	case WM_USER_UPDATE:
       {
          sub_527ED0();
          if ( dword_55C08C == 1 )
          {
            sub_526F80();
            dword_55C08C = 0;
          }
          return 1;
       }    
    case WM_LBUTTONDOWN:
		sub_526DD0(lParam, HIWORD(lParam), 1);
		return 1;    
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
		sub_526DD0(lParam, HIWORD(lParam), 0);
		return 1;
		//break;
    case WM_RBUTTONDOWN:
		sub_526DD0(lParam, HIWORD(lParam), 2);
		return 1;
    
	case WM_MOUSEMOVE:
        {
          sub_526DD0(lParam, HIWORD(lParam), word_578DE0);
          return 1;
        }
	case WM_MENUSELECT:        
          return 0;
	case WM_SETCURSOR: 
		{
		/*//RECT r;
		POINT p;
		//GetClientRect(hWnd,&r);
		GetCursorPos(&p);
		if(ScreenToClient(hWnd,&p))
		{
			if(p.x>nPrivate::sx-100||p.y>nPrivate::sy-100)
				ShowCursor(TRUE);
			else
				ShowCursor(FALSE);
		}else{
			//SetCursor(0);
			ShowCursor(TRUE);
		}
		//ShowCursor(FALSE);*/
		SetCursor(0);
		return 1;
		//break;
		}
	case WM_SYSCOMMAND:      
        if ( wParam != 0xF100 )
			break; //return DefWindowProcA(hWnd, Msg, wParam, lParam);
        return 0;
	case WM_SYSKEYDOWN:                
        if ( wParam >= 0x41 && wParam <= 0x5A || wParam >= 0x70 && wParam <= 0x79 )
        {
          switch ( wParam )
          {
			case 0x41u:
              KeyState[0] = 0x1E00u;
              break;
            case 0x42u:
              KeyState[0] = 0x3000u;
              break;
            case 0x43u:
              KeyState[0] = 0x2E00u;
              break;
            case 0x44u:
              KeyState[0] = 0x2000u;
              break;
            case 0x45u:
              KeyState[0] = 0x1200u;
              break;
            case 0x46u:
              KeyState[0] = 0x2100u;
              break;
            case 0x47u:
              KeyState[0] = 0x2200u;
              break;
            case 0x48u:
              KeyState[0] = 0x2300u;
              break;
            case 0x49u:
              KeyState[0] = 0x1700u;
              break;
            case 0x4Au:
              KeyState[0] = 0x2400u;
              break;
            case 0x4Bu:
              KeyState[0] = 0x2500u;
              break;
            case 0x4Cu:
              KeyState[0] = 0x2600u;
              break;
            case 0x4Du:
              KeyState[0] = 0x3200u;
              break;
            case 0x4Eu:
              KeyState[0] = 0x3100u;
              break;
            case 0x4Fu:
              KeyState[0] = 0x1800u;
              break;
            case 0x50u:
              KeyState[0] = 0x1900u;
              break;
            case 0x51u:
              KeyState[0] = 0x1000u;
              break;
            case 0x52u:
              KeyState[0] = 0x1300u;
              break;
            case 0x53u:
              KeyState[0] = 0x1F00u;
              break;
            case 0x54u:
              KeyState[0] = 0x1400u;
              break;
            case 0x55u:
              KeyState[0] = 0x1600u;
              break;
            case 0x56u:
              KeyState[0] = 0x2F00u;
              break;
            case 0x57u:
              KeyState[0] = 0x1100u;
              break;
            case 0x58u:
              KeyState[0] = 0x2D00u;
              break;
            case 0x59u:
              KeyState[0] = 0x1500u;
              break;
            case 0x5Au:
              KeyState[0] = 0x2C00u;
              break;
            case 0x70u:
              KeyState[0] = 0x6800u;
              break;
            case 0x71u:
              KeyState[0] = 0x6900u;
              break;
            case 0x72u:
              KeyState[0] = 0x6A00u;
              break;
            case 0x73u:
              KeyState[0] = 0x6B00u;
              break;
            case 0x74u:
              KeyState[0] = 0x6C00u;
              break;
            case 0x75u:
              KeyState[0] = 0x6D00u;
              break;
            case 0x76u:
              KeyState[0] = 0x6E00u;
              break;
            case 0x77u:
              KeyState[0] = 0x6F00u;
              break;
            case 0x78u:
              KeyState[0] = 0x7000u;
              break;
            case 0x79u:
              KeyState[0] = 0x7100u;
              break;
            default:
              break;
          }
          byte_57E120 = 1;
          i8 k = byte_57E124 + 1;
          dword_57E0F8[byte_57E124] = KeyState[0];
          byte_57E124 = k % 10;
        }
        if ( wParam == VK_MENU )
			return 0;
	case WM_KEYDOWN:
    {
      KeyState[0] = 0;
	  i16 key=lParam >> 16;
	  //int wp=wParam;
      if ( GetKeyboardState((PBYTE)&KeyState[1])) 
      {
		//wp=wParam;
        ToAscii(wParam, key, (PBYTE)&KeyState[1], KeyState, 0);
      }
      else
      { 
		//wp=wParam;
        key = KeyState[1] & 0xFF;
      }
      if ( KeyState[0] )
      {
        byte_57E120 = 1;
        KeyState[0] += key << 8;
        i8 k = byte_57E124 + 1;
        dword_57E0F8[byte_57E124] = KeyState[0];
        byte_57E124 = k % 10;        
        dword_589A1C = (lParam & 0xFF0000u) >> 16;
      }
      else
      {
		ui32  v9=0;
        switch ( wParam )
        {
          case 8u:
            v9 = 0xE08u;
            break;
          case 9u:
            v9 = 0xF09u;
            break;
          case 0xDu:
            v9 = 0x1C0Du;
            break;
          case 0x21u:
            v9 = 0x49E0u;
            break;
          case 0x22u:
            v9 = 0x51E0u;
            break;
          case 0x23u:
            v9 = 0x4FE0u;
            break;
          case 0x24u:
            v9 = 0x47E0u;
            break;
          case 0x25u:
            v9 = 0x4BE0u;
            break;
          case 0x26u:
            v9 = 0x48E0u;
            break;
          case 0x27u:
            v9 = 0x4DE0u;
            break;
          case 0x28u:
            v9 = 0x50E0u;
            break;
          case 0x2Du:
            v9 = 0x52E0u;
            break;
          case 0x2Eu:
            v9 = 0x10000u;
            break;
          case 0x6Bu:
            v9 = 0xD2Bu;
            break;
          case 0x6Du:
            v9 = 0xC2Du;
            break;
          case 0x70u:
            v9 = 0x3B00u;
            break;
          case 0x71u:
            v9 = 0x3C00u;
            break;
          case 0x72u:
            v9 = 0x3D00u;
            break;
          case 0x73u:
            v9 = 0x3E00u;
            break;
          case 0x74u:
            v9 = 0x3F00u;
            break;
          case 0x75u:
            v9 = 0x4000u;
            break;
          case 0x76u:
            v9 = 0x4100u;
            break;
          case 0x77u:
            v9 = 0x4200u;
            break;
          case 0x78u:
            v9 = 0x4300u;
            break;
          case 0x79u:
            v9 = 0x4400u;
            break;
          case 0x7Au:
            v9 = 0xFFFF8500u;
            break;
          case 0x7Bu:
            v9 = 0xFFFF8600u;
            break;
          default:
            break;
        }
        if ( v9 )
        {
          byte_57E120 = 1;
          i8 k= byte_57E124 + 1;
          dword_57E0F8[byte_57E124] = v9;
          byte_57E124 = k % 10;          
          dword_589A1C = (lParam & 0xFF0000u) >> 16;
        }
      }
	  break;
      //return DefWindowProcA(hWnd, Msg, wParam, lParam);
    }
    default:
       break;
  }
  return DefWindowProc(hWnd, Msg, wParam, lParam);
}
#define graphics_old_screen_window_x1 (*(i16*)(0x0057A9A4)) 
#define graphics_old_screen_window_y1 (*(i16*)(0x0057A99C)) 
#define graphics_old_screen_window_x2 (*(i16*)(0x0057A9A0)) 
#define graphics_old_screen_window_y2 (*(i16*)(0x0057A998)) 


__int16 __cdecl sub_528C10()
{  
  graphics_old_screen_window_x1 = video_screen_window_x1;  
  graphics_old_screen_window_y1 = video_screen_window_y1;
  graphics_old_screen_window_x2 = video_screen_window_x2;
  graphics_old_screen_window_y2 = video_screen_window_y2;
  return video_screen_window_y2;
}
__int16 __cdecl graphics_Restore_Window()
{
  video_screen_window_x1 = graphics_old_screen_window_x1;  
  video_screen_window_y1 = graphics_old_screen_window_y1;
  video_screen_window_x2 = graphics_old_screen_window_x2;
  video_screen_window_y2 = graphics_old_screen_window_y2;
  return graphics_old_screen_window_y2;
}
i16* __cdecl MISC_Save_Window_Status_(i16* pstatus)
{  
  sub_528C10();  
  *pstatus = animate_clipping_flag;
  return pstatus;
}
void __cdecl animate_Clip_On()
{
  animate_clipping_flag = 1;
}
void __cdecl animate_Clip_Off()
{
  animate_clipping_flag = 0;
}
void __cdecl MISC_Restore_Window_Status_(__int16 status)
{
  graphics_Restore_Window();
  if ( status )
    animate_Clip_On();
  else
    animate_Clip_Off();
}
__int16 __cdecl graphics_Set_Window(__int16 x1, __int16 y1, __int16 x2, __int16 y2) //00528BA0
{
	if(x2==527 && y2==421)
	{
		x2=nPrivate::sx-113;
		y2=nPrivate::sy-59;
	}else
	if(x2==639 && y2==479)
	{
		x2=nPrivate::sx-1;
		y2=nPrivate::sy-1;
	}
  if ( x1 < 0 )
    x1 = 0;  
  if ( y1 < 0 )
    y1 = 0;  
  if ( x2 >= g_screen_res_x )
    x2 = g_screen_res_x - 1;  
  if ( y2 >= g_screen_res_y )
    y2 = g_screen_res_y - 1;
  video_screen_window_x1 = x1;
  video_screen_window_y1 = y1;
  video_screen_window_x2 = x2;
  video_screen_window_y2 = y2;
  return y2;
}

void __cdecl sub_4C6930() //draw
{
  i16 v0; // [sp+2h] [bp-2h]@1
  MISC_Save_Window_Status_(&v0);
  graphics_Set_Window(22, 22, 527, 421);
  animate_Clip_On();
  int w=nPrivate::sx;//640
  int h=nPrivate::sy;//480
  PicFrameHeader* buf = (PicFrameHeader*)buffer_Buffer_Reload("STARBG.LBX", 0, MOX__global_cache_seg);
  animate_Draw(word_596CAE, word_596CB6, buf);
  animate_Draw(word_596CAE - w, word_596CB6, buf);
  animate_Draw(word_596CAE - w, word_596CB6 - h, buf);
  animate_Draw(word_596CAE, word_596CB6 - h, buf);
  PicFrameHeader* buf2 = (PicFrameHeader*)buffer_Buffer_Reload("STARBG.LBX", 1, MOX__global_cache_seg);
  animate_Draw(word_59643C, word_596444, buf2);
  animate_Draw(word_59643C - w, word_596444, buf2);
  animate_Draw(word_59643C - w, word_596444 - h, buf2);
  animate_Draw(word_59643C, word_596444 - h, buf2);
  PicFrameHeader* buf3  = (PicFrameHeader*)buffer_Buffer_Reload("STARBG.LBX", 2, MOX__global_cache_seg);
  animate_Draw(word_596EA8, word_596EA6, buf3);
  animate_Draw(word_596EA8 - w, word_596EA6, buf3);
  animate_Draw(word_596EA8 - w, word_596EA6 - h, buf3);
  animate_Draw(word_596EA8, word_596EA6 - h, buf3);
  MISC_Restore_Window_Status_(v0);
}
#define dword_59706E (*(i32*)(0x0059706E))
#define word_59747A (*(i16*)(0x0059747A))
#define word_597468 (*(i16*)(0x00597468))
#define word_597850 (*(i16*)(0x00597850))
#define word_59B3FA (*(i16*)(0x0059B3FA))
#define word_59785C (*(i16*)(0x0059785C))
#define word_59B3FE (*(i16*)(0x0059B3FE))
i8 __cdecl sub_40BCC0(__int16 a1, __int16 a2)
{
  signed __int16 v2; // di@1
  signed __int16 v3; // bx@1
  signed __int16 v4; // bp@9
  signed __int16 v5; // si@9
  signed __int16 v6; // cx@9
  int v7; // eax@21

  v2 = a1 - 16;
  v3 = a2 - 9;
  if ( (signed __int16)(a1 - 16) < 0 )
    v2 = 0;
  if ( v2 > 49 )
    v2 = 49;
  if ( v3 < 0 )
    v3 = 0;
  if ( v3 > 50 )
    v3 = 50;
  v4 = -160 * (HIWORD(dword_59706E) - v2) + word_597468;
  word_59747A += -160 * (dword_59706E - v3);
  v5 = -80 * (HIWORD(dword_59706E) - v2) + word_59785C;
  word_597850 += -80 * (dword_59706E - v3);
  v6 = -20 * (HIWORD(dword_59706E) - v2) + word_59B3FE;
  word_59B3FA += -20 * (dword_59706E - v3);
  if ( v4 < 0 )
    v4 += nPrivate::sx * (unsigned __int16)(nPrivate::sx-1 - v4) / nPrivate::sx;
  if ( word_59747A < 0 )
    word_59747A += nPrivate::sy * (unsigned __int16)(nPrivate::sy-1 - word_59747A) / nPrivate::sy;
  if ( v5 < 0 )
    v5 += nPrivate::sx * (unsigned __int16)(nPrivate::sx-1 - v5) / nPrivate::sx;
  if ( word_597850 < 0 )
    word_597850 += nPrivate::sy * (unsigned __int16)(nPrivate::sy-1 - word_597850) / nPrivate::sy;
  if ( v6 < 0 )
    v6 += nPrivate::sx * (unsigned __int16)(nPrivate::sx-1 - v6) / nPrivate::sx;
  if ( word_59B3FA < 0 )
    word_59B3FA += nPrivate::sy * (unsigned __int16)(nPrivate::sy-1 - word_59B3FA) / nPrivate::sy;
  word_597468 = v4 % nPrivate::sx;
  word_59747A %= nPrivate::sy;
  word_59785C = v5 % nPrivate::sx;
  word_597850 %= nPrivate::sy;
  word_59B3FE = v6 % nPrivate::sx;
  word_59B3FA %= nPrivate::sy;
  dword_59706E = v2<<16 | (v3 &0xFFFF);
  //todo
  //sub_46A8A0(v2, nPrivate::sy);
  //LOWORD(v7) = word_59D6C0;
  //sub_46AFC0(LOWORD(word_59D6C0));
  return 0;//sub_4CDA90(v3, nPrivate::sx, v2, nPrivate::sy);
}
#define animate_animation_header (*(struct PicFrameHeader**)(0x0057632C))
#define byte_56A998 ((i8*)(0x0056A998))
ui32 __cdecl sub_535E50(i16 x1, i16 y1, i32 pframe)
{
  signed int v3; // ebx@1
  int v4; // esi@1
  int v5; // ecx@5
  int v6; // ecx@7
  signed int v7; // ebp@9
  int v8; // edx@9
  int v9; // ecx@12
  int v10; // esi@12
  int v11; // ecx@12
  int v12; // ebp@16
  int v13; // eax@18
  int v14; // edi@18
  int v15; // ebx@20
  unsigned __int8 v16; // cl@21
  int v17; // ebx@28
  unsigned __int8 v18; // cl@29
  int v20; // [sp+10h] [bp-20h]@12
  //signed int v21; // [sp+14h] [bp-1Ch]@1
  int v22; // [sp+18h] [bp-18h]@9
  int v23; // [sp+18h] [bp-18h]@16
  int v24; // [sp+1Ch] [bp-14h]@9
  int v25; // [sp+20h] [bp-10h]@1
  int v26; // [sp+24h] [bp-Ch]@9
  //signed int v27; // [sp+28h] [bp-8h]@1
  int v28; // [sp+2Ch] [bp-4h]@11

  //v21 = a1;
  i16 y2 = nPrivate::sy-1;
  v4 = pframe + 4;
  //v27 = a2;
  i16 x2 = nPrivate::sx-1;
  g_screen_res_x = LockAppropriateRenderBuffer();
  if ( video_screen_window_x1 > x1 )
    x1 = video_screen_window_x1;
  if ( video_screen_window_y1 > y1 )
    y1 = video_screen_window_y1;
  v5 = animate_animation_header->m_height;
   if ( v5 + x1 > video_screen_window_x2 )
    x2 = video_screen_window_x2;
  v6 = y1 + v5;
  if ( v6 > video_screen_window_y2 )
    y2 = video_screen_window_y2;
  v7 = x1;
  v24 = y1 + *(WORD *)(pframe + 2);
  v8 = g_lpSurfaceMem + g_screen_res_x * v24;
  v26 = v8 + x1;
  v22 = v8 + x1;
  if ( y2 >= v6 )
    y2 = v6;
  v28 = y2;
  while ( v24 <= v28 )
  {
    v9 = *(WORD *)v4;
    v10 = v4 + 2;
    v20 = v9;
    v11 = *(WORD *)v10;
    v4 = v10 + 2;
    if ( v20 )
    {
      if ( y1 > v24 || x2 <= v7 )
      {
        v4 += v20;
      }
      else
      {
        v12 = v11 + v7;
        v23 = v11 + v22;
        if ( x1 > v12 )
        {
          v4 = x1 + v4 - v12;
          v23 = x1 + v23 - v12;
          v20 = v12 + v20 - x1;
          v12 = x1;
        }
        v13 = v4;
        v14 = v23;
        if ( x2 >= v12 + v20 )
        {
          if ( v20 > 0 )
          {
            v17 = v20;
            do
            {
              v18 = *(BYTE *)v13;
              if ( *(BYTE *)v13 )
              {
                if ( v18 >= 240u )
                  v18 = (byte_56A998[256 * v18+v14]);
                *(BYTE *)v14 = v18;
              }
              ++v13;
              ++v14;
              --v17;
            }
            while ( v17 );
          }
        }
        else
        {
          if ( x2 - v12 > 0 )
          {
            v15 = x2 - v12 + 1;
            if ( v15 > 0 )
            {
              do
              {
                v16 = *(BYTE *)v13;
                if ( *(BYTE *)v13 )
                {
                  if ( v16 >= 240u )
                    v16 = (byte_56A998[256 * v16+v14]);
                  *(BYTE *)v14 = v16;
                }
                ++v13;
                ++v14;
                --v15;
              }
              while ( v15 );
            }
          }
        }
        v7 = v20 + v12;
        v4 += v20;
        v22 = v20 + v23;
      }
    }
    else
    {
      v7 = x1;
      v24 += v11;
      v26 += nPrivate::sx * v11;
      v22 = v26;
    }
    if ( v4 & 1 )
      ++v4;
  }
  return UnlockAppropriateRenderBuffer();
}
#define MAINMENU__mainmenu_first_seg (*(i32*)(0x00595C58)

#define MAINMENU__gametype (*(i32*)(0x0056ECE8))
void __cdecl draw_uncompressed_sub_5365B0(ui16 x1, ui16 y1, i32 buf1)
{  
  ui8* pframe=(ui8*)buf1;     
  ui8* data = (ui8*)(x1 + (y1 * nPrivate::sx) + g_lpSurfaceMem);
  for(int i=0;i<animate_animation_header->m_height;i++)
  { 
    for(int j=0;j<animate_animation_header->m_width;j++)
    { 
		if ( pframe[j+animate_animation_header->m_width*i] )
			data[j] = pframe[j+animate_animation_header->m_width*i];
    }
	data += nPrivate::sx;
  }
}
unsigned int __cdecl sub_536280(__int16 x1, __int16 y1, int pframe)
{
  __int16 v3; // dx@2
  int v4; // ecx@5
  __int16 v5; // di@5
  __int16 v6; // si@7
  __int16 v7; // bp@7
  __int16 v8; // si@9
  __int16 v9; // bp@11
  signed int v10; // ebx@11
  int v11; // edi@11
  unsigned __int8 v12; // zf@11
  char v13; // sf@11
  unsigned __int8 v14; // of@11
  int v15; // eax@12
  int v16; // eax@19
  signed int v17; // esi@19
  int v18; // eax@20
  signed __int16 v19; // cx@20
  __int16 v20; // ax@20
  int v21; // edi@21
  int v22; // ebp@24
  int v23; // eax@24
  signed __int16 v24; // dx@26
  __int16 v25; // cx@29
  int v27; // [sp+10h] [bp-28h]@11
  int v28; // [sp+14h] [bp-24h]@11
  int v29; // [sp+14h] [bp-24h]@19
  int v30; // [sp+18h] [bp-20h]@24
  int i; // [sp+18h] [bp-20h]@31
  signed int v32; // [sp+1Ch] [bp-1Ch]@0
  signed int v33; // [sp+20h] [bp-18h]@19
  int v34; // [sp+24h] [bp-14h]@19
  int v35; // [sp+28h] [bp-10h]@19
  int v36; // [sp+2Ch] [bp-Ch]@19
  int v37; // [sp+30h] [bp-8h]@19
  int v38; // [sp+34h] [bp-4h]@19

  g_screen_res_x = LockAppropriateRenderBuffer();
  if ( video_screen_window_x1 > x1 )
  {
    v3 = video_screen_window_x1;
    v32 = video_screen_window_x1 - x1;
  }
  else
  {
   v32 = 0;
    v3 = x1;
  }
  if ( video_screen_window_y1 > y1 )
  {
    v4 = video_screen_window_y1 - y1;
    v5 = video_screen_window_y1;
  }
  else
  {
    v4 = 0;
    v5 = y1;
  }
  v6 = x1 + animate_animation_header->m_width - 1;
  v7 = y1 + animate_animation_header->m_height - 1;
  if ( v6 >= video_screen_window_x2 )
    v6 = video_screen_window_x2;
  v8 = v6 - v3 + 1;
  if ( video_screen_window_y2 <= v7 )
    v7 = video_screen_window_y2;
  v9 = v7 - v5 + 1;
  v28 = *(WORD *)(pframe + 2);
  v4 = (signed __int16)v4;
  v10 = 2;
  v11 = x1 + g_screen_res_x * v5;
  v27 = v11;
  v14 = v28;//OFSUB(v28, (signed __int16)v4);
  v12 = v28 == (signed __int16)v4;
  v13 = v28 - (signed __int16)v4 < 0;
  if ( v28 < (signed __int16)v4 )
  {
    do
    {
      v15 = pframe + 2 * v10;
      if ( *(WORD *)v15 )
        v10 += (*(WORD *)v15 + 1) / 2;
      else
        v28 += *(WORD *)(v15 + 2);
      v10 += 2;
    }
    while ( v28 < v4 );
    v14 = v28;//__OFSUB__(v28, v4);
    v12 = v28 == v4;
    v13 = v28 - v4 < 0;
  }
  if ( !((unsigned __int8)(v13 ^ v14) | v12) )
  {
    v27 = g_screen_res_x * (v28 - v4) + v11;
    v11 += g_screen_res_x * (v28 - v4);
  }
  v29 = v28 - v4;
  v34 = (signed __int16)v32;
  v16 = (signed __int16)v32 + v8;
  v17 = 0;
  v35 = v27 + (signed __int16)v32;
  v37 = v16;
  v33 = v11;
  v36 = v27 + v16;
  v38 = v9;
  while ( v29 < v38 )
  {
    v10 += 2;
    v18 = pframe + 2 * v10 - 4;
    v19 = *(WORD *)v18;
    v20 = *(WORD *)(v18 + 2);
    if ( v19 )
    {
      v11 += v20;
      v22 = 2 * v10;
      v23 = v19;
      v30 = v19 + v11;
      if ( v36 > v11 )
      {
        if ( v35 > v11 )
        {
          v24 = v35 - v11;
          if ( v19 < (signed __int16)(v35 - (WORD)v11) )
            v24 = v19;
          v22 += v24;
          v11 += v24;
        }
        v25 = v36 - v11;
        if ( v11 + (signed __int16)(v36 - (WORD)v11) >= v30 )
          v25 = v30 - v11;
        for ( i = v11 + v25; i > v11; *(BYTE *)(g_lpSurfaceMem + v11 - 1) = *(BYTE *)(pframe + v22 - 1) )
        {
          if ( !v17 )
          {
            v17 = 1;
            v33 = v11;
          }
          ++v22;
          v32 = v11++;
        }
      }
      v10 += (v23 + 1) / 2;
    }
    else
    {
      v21 = v20;
      v29 += v20;
      if ( v17 == 1 )
        sub_528A30(v33 % nPrivate::sx, v33 / nPrivate::sx, v32 % nPrivate::sx);
      v27 += nPrivate::sx * v21;
      v11 = v27;
      v17 = 0;
      v35 = v27 + v34;
      v36 = v27 + v37;
      v33 = v27;
    }
  }
  return UnlockAppropriateRenderBuffer();
}
unsigned int __cdecl sub_524830(__int16 x1, __int16 y1, int buf1)
{ 
  ui8* buf=(ui8*)buf1;
  LockAppropriateRenderBuffer();  
  int y_offset = MAKEWORD(buf[2],buf[3]);  
  i32 data = x1 + nPrivate::sx * (y_offset + y1)+g_lpSurfaceMem;
  i32 data2=data;
   int height = animate_animation_header->m_height;
  int j=4;
  for(int i=y_offset;i<height;)
  {
	int n = MAKEWORD(buf[j],buf[j+1]);
    int k = MAKEWORD(buf[2+j],buf[3+j]);
    if ( n )
    {      
	  j+=4;
	  data+=k;
      for (int m = 0; m < n; ++m )
      {
        if ( buf[j+m] < 0xF0u )
          *(ui8*)(data+m) = buf[j+m];
        else
          *(ui8*)(data+m) = (byte_56A998[256 * buf[j+m]+*(ui8*)(data+m)]);        
      }
      data+=n;
	  j+=n;
	  if (n&1)
		j++;
    }
    else
    {
      if(k==1000)//???
		  break;
      data2 += (k*nPrivate::sx);
	  data=data2;
	  i+=k;	  
	  j+=4;
    }
  }
  return UnlockAppropriateRenderBuffer();
}
int __cdecl sub_524C30(int a1)
{
  int v1; // eax@1
  int v2; // ecx@1
  signed int v3; // esi@2
  int v4; // edx@2
  int v5; // eax@2
  unsigned __int8 v6; // bl@4
  int v7; // edx@4
  int v8; // eax@8
  signed int v10; // [sp+10h] [bp-10h]@1
  int i; // [sp+14h] [bp-Ch]@3
  int v12; // [sp+18h] [bp-8h]@1

  v1 = LockPrimarySurface();
  v2 = 0;
  v12 = v1;
  v10 = 0;
  do
  {
    v3 = v10;
    v4 = *(BYTE *)(a1 + v2);
    v5 = *(BYTE *)(a1 + v2 + 1);
    for ( v2 += 2; v5; v5 = *(BYTE *)(v8 + 1) )
    {
      v3 += v4;
      for ( i = v2 + 4 * v5; v2 < i; ++v2 )
      {
        v6 = *(BYTE *)(a1 + v2);
        v7 = v12 * v3 / nPrivate::sx + v3 % nPrivate::sx;
        if ( v6 < 0xF0u )
          *(BYTE *)(dword_5792F0 + v7) = v6;
        else
          *(BYTE *)(v7 + dword_5792F0) = (byte_56A998[256 * v6+v7 + dword_5792F0]);
        ++v3;
      }
      v8 = a1 + v2;
      v2 += 2;
      v4 = *(BYTE *)v8;
    }
    v10 += 65536;
  }
  while ( v10 < 327680 );
  UnlockPrimarySurface();
  return 0;  
}
#define byte_576330 ((i8*)(0x00576330))
unsigned int __cdecl sub_5250F0(__int16 a1, __int16 a2, int a3)
{
  __int16 v3; // cx@2
  __int16 v4; // dx@5
  __int16 v5; // si@5
  __int16 v6; // bx@7
  __int16 v7; // bp@7
  __int16 v8; // bx@9
  __int16 v9; // bp@11
  signed int v10; // edi@11
  int v11; // esi@11
  int v12; // edx@12
  int v13; // eax@19
  signed __int16 v14; // dx@19
  int v15; // eax@19
  int v16; // ebx@21
  int v17; // eax@21
  signed __int16 v18; // bp@23
  __int16 v19; // dx@26
  int v20; // edx@29
  int v22; // [sp+10h] [bp-20h]@11
  int v23; // [sp+14h] [bp-1Ch]@11
  int v24; // [sp+14h] [bp-1Ch]@18
  int i; // [sp+18h] [bp-18h]@11
  int v26; // [sp+18h] [bp-18h]@18
  __int16 v27; // [sp+1Ch] [bp-14h]@2
  int v28; // [sp+1Ch] [bp-14h]@18
  int v29; // [sp+20h] [bp-10h]@21
  int j; // [sp+20h] [bp-10h]@28
  int v31; // [sp+24h] [bp-Ch]@18
  int v32; // [sp+28h] [bp-8h]@18
  int v33; // [sp+2Ch] [bp-4h]@18
  LockAppropriateRenderBuffer();
  
  if ( a1 < video_screen_window_x1 )
  {
    v27 = video_screen_window_x1 - a1;
    v3 = video_screen_window_x1;
  }
  else
  {
    v27 = 0;
    v3 = a1;
  }
  if ( a2 < video_screen_window_y1 )
  {
    v4 = video_screen_window_y1 - a2;
    v5 = video_screen_window_y1;
  }
  else
  {
    v4 = 0;
    v5 = a2;
  }
  v6 = a1 + animate_animation_header->m_width - 1;
  v7 = animate_animation_header->m_height + a2 - 1;
  if ( video_screen_window_x2 <= v6 )
    v6 = video_screen_window_x2;
  v8 = v6 - v3 + 1;
  if ( video_screen_window_y2 <= v7 )
    v7 = video_screen_window_y2;
  v9 = v7 - v5 + 1;
  v23 = *(WORD *)(a3 + 2);
  v10 = 2;
  v11 = g_screen_res_x * v5 + a1;
  v22 = v11;
  for ( i = v4; v23 < i; v10 += 2 )
  {
    v12 = a3 + 2 * v10;
    if ( *(WORD *)v12 )
      v10 += (*(WORD *)v12 + 1) / 2;
    else
      v23 += *(WORD *)(v12 + 2);
  }
  if ( v23 > i )
  {
    v22 = g_screen_res_x * (v23 - i) + v11;
    v11 += g_screen_res_x * (v23 - i);
  }
  v31 = v27;
  v24 = v23 - i;
  v32 = v27 + v22;
  v26 = v8 + v27;
  v33 = v26 + v22;
  v28 = v9;
  while ( v24 < v28 )
  {
    v13 = a3 + 2 * v10;
    v10 += 2;
    v14 = *(WORD *)v13;
    v15 = *(WORD *)(v13 + 2);
    if ( v14 )
    {
      v11 += v15;
      v16 = 2 * v10;
      v17 = v14;
      v29 = v14 + v11;
      if ( v11 < v33 )
      {
        if ( v11 < v32 )
        {
          v18 = v32 - v11;
          if ( (signed __int16)(v32 - (WORD)v11) > v14 )
            v18 = v14;
          v16 += v18;
          v11 += v18;
        }
        v19 = v33 - v11;
        if ( v11 + (signed __int16)(v33 - (WORD)v11) >= v29 )
          v19 = v29 - v11;
        for ( j = v11 + v19; j > v11; *(BYTE *)(g_lpSurfaceMem + v11 - 1) = byte_576330[v20] )
        {
          v20 = *(BYTE *)(a3 + v16++);
          ++v11;
        }
      }
      v10 += (v17 + 1) / 2;
    }
    else
    {
      v24 += v15;
      v22 += nPrivate::sx * v15;
      v11 = v22;
      v32 = v22 + v31;
      v33 = v22 + v26;
    }
  }
  return UnlockAppropriateRenderBuffer();
}	
unsigned int __cdecl sub_536040(__int16 x1, __int16 y1, int buf)
{
  __int16 v3; // cx@2
  __int16 v4; // dx@5
  __int16 v5; // bx@5
  __int16 v6; // di@7
  __int16 v7; // bp@7
  __int16 v8; // di@9
  __int16 v9; // bp@11
  int v10; // eax@11
  signed int v11; // ebx@11
  int v12; // esi@11
  int v13; // eax@12
  int v14; // eax@19
  signed __int16 v15; // dx@19
  int v16; // eax@19
  int v17; // edi@21
  int v18; // eax@21
  signed __int16 v19; // bp@23
  __int16 v20; // dx@26
  char v21; // dl@29
  int i; // [sp+10h] [bp-20h]@11
  signed int v24; // [sp+14h] [bp-1Ch]@11
  int v25; // [sp+14h] [bp-1Ch]@18
  int v26; // [sp+18h] [bp-18h]@18
  __int16 v27; // [sp+1Ch] [bp-14h]@2
  int v28; // [sp+1Ch] [bp-14h]@18
  int v29; // [sp+20h] [bp-10h]@21
  int j; // [sp+20h] [bp-10h]@28
  int v31; // [sp+24h] [bp-Ch]@18
  int v32; // [sp+28h] [bp-8h]@18
  int v33; // [sp+2Ch] [bp-4h]@18
  LockAppropriateRenderBuffer();
  //g_screen_res_x;
  if ( video_screen_window_x1 > x1 )
  {
    v3 = video_screen_window_x1;
    v27 = video_screen_window_x1 - x1;
  }
  else
  {
    v27 = 0;
    v3 = x1;
  }
  if ( video_screen_window_y1 > y1 )
  {
    v4 = video_screen_window_y1 - y1;
    v5 = video_screen_window_y1;
  }
  else
  {
    v4 = 0;
    v5 = y1;
  }
  v6 = x1 + animate_animation_header->m_width - 1;
  v7 = y1 + animate_animation_header->m_height - 1;
  if ( video_screen_window_x2 <= v6 )
    v6 = video_screen_window_x2;
  v8 = v6 - v3 + 1;
  if ( video_screen_window_y2 <= v7 )
    v7 = video_screen_window_y2;
  v9 = v7 - v5 + 1;
  v24 = *(WORD *)(buf + 2);
  v10 = g_screen_res_x * v5 + x1;
  v11 = 2;
  v12 = v10;
  for ( i = v10; v24 < v4; v11 += 2 )
  {
    v13 = buf + 2 * v11;
    if ( *(WORD *)v13 )
      v11 += (*(WORD *)v13 + 1) / 2;
    else
      v24 += *(WORD *)(v13 + 2);
  }
  if ( v24 > v4 )
  {
    i += g_screen_res_x * (v24 - v4);
    v12 = i;
  }
  v31 = v27;
  v25 = v24 - v4;
  v32 = v27 + i;
  v26 = v8 + v27;
  v33 = v26 + i;
  v28 = v9;
  while ( v25 < v28 )
  {
    v14 = buf + 2 * v11;
    v11 += 2;
    v15 = *(WORD *)v14;
    v16 = *(WORD *)(v14 + 2);
    if ( v15 )
    {
      v12 += v16;
      v17 = 2 * v11;
      v18 = v15;
      v29 = v15 + v12;
      if ( v33 > v12 )
      {
        if ( v32 > v12 )
        {
          v19 = v32 - v12;
          if ( v15 < (signed __int16)(v32 - (WORD)v12) )
            v19 = v15;
          v17 += v19;
          v12 += v19;
        }
        v20 = v33 - v12;
        if ( v12 + (signed __int16)(v33 - (WORD)v12) >= v29 )
          v20 = v29 - v12;
        for ( j = v12 + v20; j > v12; *(BYTE *)(g_lpSurfaceMem + v12 - 1) = v21 )
        {
          v21 = *(BYTE *)(buf + v17++);
          ++v12;
        }
      }
      v11 += (v18 + 1) / 2;
    }
    else
    {
      v25 += v16;
      i += nPrivate::sx * v16;
      v12 = i;
      v32 = i + v31;
      v33 = i + v26;
    }
  }
  return UnlockAppropriateRenderBuffer();
}
#define dword_576328 (*(i32*)(0x00576328))
#define palette_byte_579350 ((i8*)(0x00579350))
int __cdecl animate_Set_Animation_Palette()//00524930
{
  int v0; // eax@1
  int v1; // ecx@1
  int result; // eax@1
  int v3; // esi@1
  i8 *v4; // edx@2
  int v5; // ecx@2
  PicFrameHeader* data=animate_animation_header;
  v0 = (int)&data->i8arr[4 * data->m_nframes + 16];
  dword_576328 = v0;
  v1 = v0;
  result = *(WORD *)v0;
  v3 = *(WORD *)(v1 + 2);
  if ( v3 > 0 )
  {
    v4 = &palette_byte_579350[4 * result];
    v5 = 0;
    do
    {
      result = *(DWORD *)(dword_576328 + v5 + 4);
      if ( *(DWORD *)v4 != result )
      {
        ++result;
        *(DWORD *)v4 = result;
        *v4 = 1;
      }
      v4 += 4;
      v5 += 4;
      --v3;
    }
    while ( v3 );
  }
  return result;
}
#define word_55B0E4 (*(i16*)(0x0055B0E4))
#define MOX__main_screen_seg (*(PicFrameHeader**)(0x0059B400))
const int nBITNONE = 0;
const int nBITNOCOMPRESSION = 0x1;
const int nBITBACKGROUND = 0x4;
const int nBITFUNCCOLOR = 0x8;
const int nBITINSIDERPALETTE = 0x10;
const int nBITMIXFRAMES = 0x20;
__int16 __cdecl animate_Draw(__int16 x1, __int16 y1, struct PicFrameHeader* header)//00524490
{
  __int16 result; // ax@16
  
  animate_animation_header = header;
  if(header==MOX__main_screen_seg)
  {
	  //x1=x1+160;
	  //y1=y1+120;
  }	  
  if ( header->m_nbits & nBITINSIDERPALETTE )
    animate_Set_Animation_Palette();
  i16 flag = 0;
  
  ui8* pframe = &header->i8arr[	 header->m_begoffset[header->m_curframe] ];
  int x2,y2;
  x2 = x1 + header->m_width - 1;
  y2 = y1 + header->m_height - 1;
  if ( animate_clipping_flag == 1 )
  {
	  
    if ( x1 > video_screen_window_x2
      || y1 > video_screen_window_y2
	  || x2 < video_screen_window_x1
	  || y2 < video_screen_window_y1 )
      flag=3;

    if ( x1 < video_screen_window_x1
      || y1 < video_screen_window_y1
      || x2 > video_screen_window_x2
      || y2 > video_screen_window_y2 )
      flag = 1;
  }
  else
  {
    //v5 = y1;
    //x2 = -1;
    //y2 = -1;
  }
  if ( flag==1 )
  {
    if ( word_55B0E4 )
    {
      if ( !(header->m_nbits & 0x3) )
        sub_536280(x1, y1, (i32)pframe);
    }
    else
    {
      if ( video_screen_refresh_flag >= 2 )
        setvesa_Add_Square_To_Scan_List_(x1, y1, x2, y2);
      
      switch ( header->m_nbits & 0x3 )
      {
        case nBITNONE:
          if ( header->m_nbits & nBITFUNCCOLOR )
            sub_535E50(x1, y1, (i32)pframe);
          else
            sub_536040(x1, y1, (i32)pframe);
          break;
        case nBITNOCOMPRESSION:
          if ( header->m_nbits & nBITFUNCCOLOR )
            sub_535C00(x1, y1, (i32)pframe);
          //else
            //sub_535D20(x1, y1, (i32)pframe);
          break;
        case 2:
          exit_Exit_With_Message("Screen Background can't be clipped");
          break;
        case 3:
          exit_Exit_With_Message("Refresh Mask can't be drawn");
          break;
        default:
          break;
      }
    }
  }
  else if(flag==0)
  {
    if ( video_screen_refresh_flag >= 2 )
    {
      //result = animate_animation_header;
	  if ( header->m_width == 0 || header->m_height == 0 )
        return (i16)header;//result;
      setvesa_Add_Square_To_Scan_List_(x1, y1, x2, y2);
    }    
    if ( header->m_nbits & nBITFUNCCOLOR )
    {
      switch ( header->m_nbits & 0x3 )
      {
        case nBITNONE:
          sub_524830(x1, y1, (i32)pframe);
          break;
        case nBITNOCOMPRESSION:
          sub_524990(x1, y1, (i32)pframe);
          break;
        case 2:
          if ( copy_to_on_page_flag == 1 )
            sub_524C30((i32)pframe);
          else
            sub_524BB0((i32)pframe);
          if ( video_screen_refresh_flag >= 2 )
            setvesa_Add_Square_To_Scan_List_(0, 0, nPrivate::sx-1, nPrivate::sy-1);
          break;
        case 3:
          exit_Exit_With_Message("Refresh Mask can't be drawn");
          break;
        default:
          break;
      }
    }
    else
    {
      switch ( header->m_nbits & 3 )
      {
        case nBITNONE:
          LockAppropriateRenderBuffer();
          draw_compressed_sub_536508(x1, y1, (i32)pframe);
          UnlockAppropriateRenderBuffer();
          break;
        case nBITNOCOMPRESSION:
          LockAppropriateRenderBuffer();
          draw_uncompressed_sub_5365B0(x1, y1, (i32)pframe);
          UnlockAppropriateRenderBuffer();
          break;
        case 2:
          if ( copy_to_on_page_flag == 1 )
            sub_524AE0((i32)pframe);
          else
            sub_524A50((i32)pframe);
          if ( video_screen_refresh_flag >= 2 )
            setvesa_Add_Square_To_Scan_List_(0, 0, nPrivate::sx-1, nPrivate::sy-1);
          break;
        case 3:
          exit_Exit_With_Message("Refresh Mask can't be drawn");
          break;
        default:
          break;
      }
    }
  }
  ++header->m_curframe;
  if ( header->m_nbits & nBITMIXFRAMES )
  {
    result = header->m_curframe;	
  }
  else
  {
	  result = header->m_width;    
  }
  if ( header->m_nframes <= header->m_curframe )
    {
      result = header->m_8;
      header->m_curframe = result;
    }
  return result;
}
void __cdecl draw_compressed_sub_536508(unsigned __int16 x1, unsigned __int16 y1, int buf1)
{  
  ui8* buf=(ui8*)buf1; 
  int height = animate_animation_header->m_height; 
  int y_offset = MAKEWORD(buf[2],buf[3]);
  i32 data = x1 + ((y1 + y_offset) * nPrivate::sx) + g_lpSurfaceMem;  
  i32 data2=data;
  int j=4;
  for(int i=y_offset;i<height;)
  {
    int n = MAKEWORD(buf[j],buf[j+1]);
    int k = MAKEWORD(buf[2+j],buf[3+j]);    	
    if ( n )
    {            
      memcpy((void *)(data + k), (const void *)&buf[4+j], n);
	  data+=n+k;
	  j+=n+4;
	  if (n&1)
		j++;
    }
    else
    {      
	  if(k==1000)//???
		  break;
      data2 += (k*nPrivate::sx);
	  data=data2;
	  i+=k;	  
	  j+=4;
    }
  }
}


__int16 __cdecl map_sub_4C7260(__int16 x1, __int16 y1, __int16 scale)//0x004C7260
{
  __int16 v3; // si@1
  __int16 v4; // di@1
  signed __int16 v5; // ax@1
  signed __int16 v6; // dx@3
  __int16 v7; // ax@6
  __int16 v8; // cx@6
  __int16 v9; // si@6
  __int16 v10; // ax@6
  __int16 v11; // dx@9
  __int16 v12; // di@9
  __int16 v13; // bx@9
  __int16 v14; // bp@9
  __int16 result; // ax@11
  i16 sx=nPrivate::sx;
  i16 sy=nPrivate::sy;
  v3 = (x1 - MOX__cur_map_x) / scale;
  v4 = (y1 - MOX__cur_map_y) / scale;
  v5 = -1;
  if ( v3 <= 0 )
    v5 = v3 < 0;
  v6 = -1;
  if ( v4 <= 0 )
    v6 = v4 < 0;
  if ( x1 == MOX__cur_map_x )
  {
    v8 = word_596CAE;
    v9 = word_59643C;
    v10 = word_596EA8;
  }
  else
  {
    v7 = v3 * v5;
    v8 = word_596CAE - v7;
    v9 = word_59643C - 2 * v7;
    v10 = word_596EA8 - 4 * v7;
  }
  if ( y1 == MOX__cur_map_y )
  {
    v12 = word_596CB6;
    v13 = word_596444;
    v14 = word_596EA6;
  }
  else
  {
    v11 = v4 * v6;
    v12 = word_596CB6 - v11;
    v13 = word_596444 - 2 * v11;
    v14 = word_596EA6 - 4 * v11;
  }
  word_596EA8 = (signed __int16)(v10 + ((char)-(v10 < 0) & sx)) % sx;
  word_596EA6 = (signed __int16)(v14 + ((char)-(v14 < 0) & sy)) % sy;
  word_59643C = (signed __int16)(v9 + ((char)-(v9 < 0) & sx)) % sx;
  word_596444 = (signed __int16)(v13 + ((char)-(v13 < 0) & sy)) % sy;
  word_596CAE = (signed __int16)(v8 + ((char)-(v8 < 0) & sx)) % sx;
  result = (signed __int16)(v12 + ((char)-(v12 < 0) & sy)) / sy;
  word_596CB6 = (signed __int16)(v12 + ((char)-(v12 < 0) & sy)) % sy;
  return result;
}
i32 __cdecl HAROLD_Get_Up_Scaled_Value_(i16 a1)//004F0A50
{
  return MOX__cur_map_scale * a1 / 10;
}
#define MOX__draw_main_screen_to_back (*(i8*)(0x00596E98))
__int16 __cdecl MAINSCR_Center_Map_(__int16 x1, __int16 y1) //004C49D0
{
  __int16 v2; // si@1
  __int16 v3; // di@1
  __int16 result; // ax@9
  i16 xx=nPrivate::sx-134; //506
  i16 yy=nPrivate::sy-80; //400
  i16 cx=xx/2; //was 253
  i16 cy=yy/2; //was 200
  v2 = MOX__cur_map_x;
  v3 = MOX__cur_map_y;
  MOX__cur_map_x = x1 - HAROLD_Get_Up_Scaled_Value_(cx);
  MOX__cur_map_y = y1 - HAROLD_Get_Up_Scaled_Value_(cy);
  if ( MOX__cur_map_x < 0 )
    MOX__cur_map_x = 0;
  if ( MOX__MAP_MAX_X - (i16)HAROLD_Get_Up_Scaled_Value_(xx) < MOX__cur_map_x )
    MOX__cur_map_x = MOX__MAP_MAX_X - HAROLD_Get_Up_Scaled_Value_(xx);
  if ( MOX__cur_map_y < 0 )
    MOX__cur_map_y = 0;
  if ( MOX__MAP_MAX_Y - (i16)HAROLD_Get_Up_Scaled_Value_(yy) < MOX__cur_map_y )
    MOX__cur_map_y = MOX__MAP_MAX_Y - HAROLD_Get_Up_Scaled_Value_(yy);
  result = MOX__cur_map_y - v3;
  if ( v2 != MOX__cur_map_x || MOX__cur_map_y != v3 )
    result = map_sub_4C7260(v2, v3, 1);
  MOX__draw_main_screen_to_back = 1;
  return result;
}

void __cdecl MAINSCR_Print_Star_Names_()//004CBA80
{
  __int16 v0; // di@1
  char v1[7*700]; // [sp+9h] [bp-27h]@1
  char v2[3*700]; // [sp+10h] [bp-20h]@3

  *(DWORD *)&v1[3] = 0;
  MISC_Save_Window_Status_((i16*)&(v1[1]));
  v0 = 0;
  graphics_Set_Window(22, 22, 527, 421);
  animate_Clip_On();
  if ( MOX__NUM_STARS > 0 )
  {
    do
    {
      if ( MOX__star[v0].type != 6 )
      {
        sub_4CA430(v0, 0, (int)v1, &v2[0], (int *)&v1[3]);
        if ( v2[0] )
          sub_4CA720(v0, v1[0], (int)&v2[0], *(int *)&v1[3]);
      }
      ++v0;
    }
    while ( v0 < MOX__NUM_STARS );
  }
  MISC_Restore_Window_Status_((__int16 )v1[1]);
}
#define dword_59B464 (*(PicFrameHeader**)(0x0059B464))
#define dword_5A4A9C (*(PicFrameHeader**)(0x005A4A9C))

/*void MAINSCR_Draw_Main_Screen_Filled_(unsigned __int16 a1, __int16 a2, __int16 a3)//004C6550
{
  char v3; // cl@1
  int *v4; // eax@3

  graphics_Reset_Window();
  animate_Clip_Off();
  graphics_fill(0, 0, nPrivate::sx-1, nPrivate::sy-1, 0);
  graphics_Set_Window(0, 0, nPrivate::sx-1, nPrivate::sy-1);
  animate_Clip_On();
  sub_4C6930();
  MOX__main_screen_seg = (PicFrameHeader*)buffer_Buffer_Reload("BUFFER0.LBX", 0, MOX__global_cache_seg);
  animate_Draw(0, 0, MOX__main_screen_seg);
  //MAINSCR_Draw_Wormhole_Links_();
  MAINSCR_Draw_Relocation_Links_(a2);
  v3 = MOX__game_type;
  if ( MOX__game_type != 2 && MOX__game_type != 3 )
  {
    v4 = MOX__global_cache_seg;
  }
  else
  {
    dword_59B464 = (PicFrameHeader*)buffer_Buffer_Reload("BUFFER0.LBX", 13, MOX__global_cache_seg);
    animate_Draw(415, 0, dword_59B464);
    v4 = MOX__global_cache_seg;
    v3 = MOX__game_type;
  }
  if ( v3 == 2 || v3 == 3 || v3 == 1 )
  {
    dword_5A4A9C = (PicFrameHeader*)buffer_Buffer_Reload("BUFFER0.LBX", 13, v4);
    animate_Draw(74, 0, dword_5A4A9C);
  }
  animate_Clip_Off();
  graphics_Reset_Window();
  graphics_Set_Window(22, 22, nPrivate::sx-113/*527/, nPrivate::sy-59/*421/);
  animate_Clip_On();
  sub_4C6880();
  animate_Clip_Off();
  graphics_Reset_Window();
  if ( a3 == 1 )
    video_Copy_Off_To_Back();
  MAINSCR_Print_Star_Names_();
  sub_4CA990(a1, a2);
}*/
#define estrings_lbx_arr_566580 (*(i32*)(0x00566580))
#define ESTRINGS__strings ((i32**)(0x0056B788))
i32 __cdecl ESTRINGS_E_Strings_(__int16 index) // 0x004A0050
{
	char* addr = (char*)(ESTRINGS__strings[index]);
	//pLog.Printf("%d = %s\n",index,addr);
  return (i32)ESTRINGS__strings[index];
}
int __cdecl farload_Farload_Data_Static(char *filename, int a2, int buf, int a4, __int16 a5, __int16 size) //0051C7B0
{
  int res = sub_51CA30(filename, a2, buf, 3, a4, a5, size);
  return res;
}
char __cdecl ESTRINGS_Load_E_Strings_() // 004A0060
{  
  char* estringslbxs[6]={"estrings.lbx","estrGERM.lbx","estrFREN.lbx","estrSPAN.lbx","estrITAL.lbx","estrPOLI.lbx"};
  const char* maintextlbxs[6]={"maintext.lbx","maingerm.lbx","mainfren.lbx","mainspan.lbx","mainital.lbx","mainpoli.lbx"};
  int k=MOX__settings.config_language;
  if(k<0 || k>5)
	  k=0;
  char* buf = (char*)farload_Farload_Data_Static(estringslbxs[k], 0, (int)&estrings_lbx_arr_566580, 0, 1, 21000);
  assert(buf);
  int i = 0;
  int **p;
  for(int i=0;i<812;i++)
  {	
	p=&ESTRINGS__strings[i];
    *p = (i32*)buf;
    buf += strlen((const char *)buf) + 1;
  }  
  if ( (ui32)buf > (unsigned int)ESTRINGS__strings )
    exit_Exit_With_Message("you blew the array eric");
  sub_4A02F0();
    
  p=&ESTRINGS__strings[368];
  *p = (i32*)maintextlbxs[k];
  return 0;
}

i16 __cdecl ship_add_sub_5094A0(__int16 player_id, __int16 a2, __int16 colony_id)
{
  
  char v13[MSL]; // [sp+14h] [bp-1F4h]@10
    
  int star_id = MOX__planet[MOX__colony[colony_id].planet_id].star;
  int i=0;
  for(;i<MOX__NUM_SHIPS || i<NEWMAX_SHIPS;i++)
  {    
    if ( MOX__ship[i].current_owner == player_id
      && (MOX__ship[i].state_of_ship == 5 || i >= MOX__NUM_SHIPS) )
      break;    
  }  
  if ( i >= NEWMAX_SHIPS )
  {
    i = -1;
  }
  else
  {
    if ( i >= MOX__NUM_SHIPS )
      MOX__NUM_SHIPS = i + 1;
    memset((void *)&MOX__ship[i], 0, SHIP_RECORD_SIZE);    
	struct_Copy_Structure_(&MOX__ship[i], &MOX__player[player_id].i8arr[99 * a2 + 0x326], 0x63u);
    MOX__ship[i].current_owner = player_id;
    MOX__ship[i].d.builder = player_id;
    MOX__ship[i].state_of_ship = 0;
    MOX__ship[i].destination_star = star_id;
    
    MOX__ship[i].map_x = MOX__star[star_id].map_x;
    MOX__ship[i].map_y = MOX__star[star_id].map_y;
    MOX__ship[i].ship_officer = 0xFFFF;
    if ( MOX__settings.strategic_combat_flag/*field_D8*/ )
    {      
      sprintf(v13, (char*)ESTRINGS_E_Strings_(42));
      if ( strlen(v13) >= 0xF )
      {        
        sprintf(v13, (char*)ESTRINGS_E_Strings_(72));
        if ( strlen(v13) >= 0xF )
			strcpy(v13, MOX__ship[i].d.name);
      }
	  strcpy((char *)(MOX__ship[i].d.name), v13);      
	  ++MOX__player[player_id].arr[MOX__ship[i].d.size/*ship_type_class*/ + 700];
    }
  }
  return i;
}

__int16 __cdecl HAROLD_N_Player_Ships_(__int16 owner) //0x004F1330
{
  int count=0;  
  for(int i=0;i<NEWMAX_SHIPS;i++)
  {
    if ( HACCESS_Ship_Owner_(i) == owner && MOX__ship[i].state_of_ship < 3 )
      ++count;    
  }  
  return count;
}
i16 __cdecl HAROLD_XY_To_Star_Id_(__int16 x, __int16 y) //0x004F1DC0
{
  int flag = 0;  
    for(int i=0;i<MOX__NUM_STARS;i++)
    { 
      if ( MOX__star[i].map_x == x  && MOX__star[i].map_y == y )
	  {
          return i;
      }      
    }      
  return -1;
}
#define byte_57CCCB (*(ui8*)(0x0057CCCB))
#define dword_57CCCC ((LPDIRECTSOUNDBUFFER*)(0x0057CCCC))
#define buf_sound_lbxunk_57CEB0 ((ui8*)(0x0057CEB0))
ui8 __cdecl sub_52DDC0()
{
	int v0; // ebx@1
	LPLPDIRECTSOUNDBUFFER v1; // esi@1
	DWORD v3; // [sp+8h] [bp-4h]@3

	v0 = 0;
	v1 = dword_57CCCC;
	while ( v1 && *v1 )
	{
		(*v1)->GetStatus(&v3);
		//(*((void (__stdcall **)(IDirectSoundBuffer *, LPDWORD))(*v1)->lpVtbl + 9))(*v1, &v3);
		if ( !(v3 & 1) && !(*((BYTE *)v1 - 1) & 1) )
			return v0;
		v1 += 8;
		++v0;
		if ( (unsigned int)v1 >= (unsigned int)&buf_sound_lbxunk_57CEB0[28] )
			return -2;
	}
	return v0;
}


#define MAINMENU__credit_x (*(i32*)(0x0056ED10))
#define MAINMENU__credit_y (*(i32*)(0x0056ED0C)) 
#define MAINMENU__delay_count (*(i32*)(0x0056ED18))
#define MAINMENU__main_menu_background_seg (*(PicFrameHeader**)(0x0056ECDC))
#define MOX__scanned_field (*(i16*)(0x005A4A9A))
struct menu_t //14 bytes
{
	PicFrameHeader *m_active;
	PicFrameHeader *m_inactive;
	WORD field_id;	
	DWORD m_unk;
};
struct mainmenu_t //210bytes
{
	DWORD m_00;
	menu_t arr[6];
};
#define MAINMENU__mainmenu (*(mainmenu_t**)(0x0056ECF0))
#define byte_56ECF8 (*(i8*)(0x0056ECF8))
#define word_576450 (*(i16*)(0x00576450))
i16 __cdecl sub_526060()
{
  return word_576450;
}
void MAINMENU_Draw_Main_Menu_Screen_(/*int a1, int a2*/) //__usercall int a1, int a2 //0x004E95B0
{
  //int v2; // eax@22
  __int16 v3; // ax@22
  __int16 v4; // [sp-8h] [bp-10h]@4
  int v5; // [sp-4h] [bp-Ch]@4
  int v6; // [sp+0h] [bp-8h]@22
  int v7; // [sp+4h] [bp-4h]@22
  PicFrameHeader *draw=0;
  int y=-1;
  if ( MOX__FULL_DRAW_SCREEN_FLAG )
  {
    video_Set_Page_Off();
    graphics_fill(0, 0, 639, 479, 0);
    animate_Draw(0, 0, MAINMENU__main_menu_background_seg);
    video_Copy_Off_To_Back();
    MOX2__TOGGLE_PAGES_();
    MAINMENU__credit_x = 66;
    MAINMENU__credit_y = 422;
    MAINMENU__delay_count = 10;
    MOX__FULL_DRAW_SCREEN_FLAG = 0;
  }
  if ( MAINMENU__mainmenu->arr[0].field_id == MOX__scanned_field )
  {
	  draw = MAINMENU__mainmenu->arr[0].m_active;
	  y = 172;
  }
  if ( MAINMENU__mainmenu->arr[1].field_id == MOX__scanned_field )
    {
      draw = MAINMENU__mainmenu->arr[1].m_active;
      y = 195;
    }
    
      if (  MAINMENU__mainmenu->arr[2].field_id == MOX__scanned_field )
      {
        draw = MAINMENU__mainmenu->arr[2].m_active;
		y = 217;
      }
      
        if (  MAINMENU__mainmenu->arr[3].field_id == MOX__scanned_field )
        {
          draw = MAINMENU__mainmenu->arr[3].m_active;
      y = 240;
        }
        
          if ( MAINMENU__mainmenu->arr[4].field_id== MOX__scanned_field )
          {
            draw = MAINMENU__mainmenu->arr[4].m_active;
			y = 262;
          }
            if (  MAINMENU__mainmenu->arr[5].field_id == MOX__scanned_field )
			{
           draw = MAINMENU__mainmenu->arr[5].m_active;
		      y = 285;
          }
   if(draw)
	animate_Draw(415, y, draw);

  if ( MAINMENU__mainmenu->arr[0].field_id == (WORD)-1000 )
    animate_Draw(415, 172,  MAINMENU__mainmenu->arr[0].m_inactive);
  if ( MAINMENU__mainmenu->arr[1].field_id == (WORD)-1000 )
    animate_Draw(415, 195, MAINMENU__mainmenu->arr[1].m_inactive);
  if ( byte_56ECF8 )
    menu_credit_sub_4E9780();
  ui8* v2 = MISC_Font_Colors2_(10/*a1*/, 1, 210, 215);
  //v6 = *(DWORD *)v2;
  //v7 = *(DWORD *)(v2 + 4);
  fonts_Set_Font_Style(1, (i32)v2);
  v3 = sub_526060();
  fonts_Print_Centered(517+15, 443 - (v3 + 1+4), "Ver 1.31 patched by loader 0.1");
  /*char buf[MSL];
  wsprintf(buf,"%d",FrameCount);
  fonts_Print(10,10,buf);*/
}

i32 __cdecl RUSS_Mox_Update_(void)
{
 // int v0; // ecx@1
  sound_sub_52E630();
  return net_sub_5156C0(0);
	//return MOX__game_type;
}
#define transprt_tpSending (*(i8*)(0x0055BF30))
void UpdateThread( void * param)
{
	
  while(global_QuitFlag!=1){
	if(pFunc_dword_55AFD8 && pFunc_dword_55AFD8==&RUSS_Mox_Update_)
	{
		
	  //PostMessageA(g_hWnd, WM_USER_UPDATE /*0x401u*/, 0, 0);// added
	//	sound_sub_52E630();//from RUSS_Mox_Update_		
	//	net_sub_5156C0(0);	//from RUSS_Mox_Update_
		
	}
	Sleep(10);
  }
}