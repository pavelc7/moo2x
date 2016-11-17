#pragma once

#include	<stdio.h>
#define LOGMEM (0x01)
class	Log
{
public:
		Log();
		~Log();

		void	Deinitialize();

		void	Write(const char * szLog);
		void	Printf(const char * szFormat, ...);
		void	vPrintf(const char * szFormat, va_list args);
		void	Printf_level(int logging_level,const char * szFormat, ...);
		void	Dump(const LPVOID buffer, DWORD bufLen);

		void	SetEnable(bool enable);
		static int level_off;
private:
		FILE	*	m_fp;
private:
		void	DoOpen();
};

extern	Log		pLog;