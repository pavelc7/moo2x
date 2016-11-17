#pragma once

#include	<stdio.h>

class	Log
{
public:
		Log();
		~Log();

		void	Deinitialize();

		void	Write(const char * szLog);
		void	Printf(const char * szFormat, ...);
		void	Dump(const LPVOID buffer, DWORD bufLen);

		void	SetEnable(bool enable);
private:
		FILE	*	m_fp;
private:
		void	DoOpen();
};

extern	Log		pLog;