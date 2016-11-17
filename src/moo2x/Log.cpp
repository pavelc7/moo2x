#include	"stdafx.h"
#include	<time.h>

Log	pLog;

Log::Log()
{
	m_fp = NULL;
}

Log::~Log()
{
	Deinitialize();
}

void	Log::Deinitialize()
{
	if (m_fp != NULL)
	{
		fclose(m_fp);
		m_fp = NULL;
	}
}

void	Log::DoOpen()
{
	if (m_fp != NULL)
	{
		fclose(m_fp);
		m_fp = NULL;
	}

	m_fp = fopen("moo2x_log.txt", "a+");
	if (m_fp)
		fseek(m_fp, 0, SEEK_END);
}

void	Log::SetEnable(bool enable)
{
	if (enable)
	{
		DoOpen();
	} else
	{
		if (m_fp != NULL)
		{
			fclose(m_fp);
			m_fp = NULL;
		}
	}
}

void	Log::Write(const char * szLog)
{
	if (m_fp == NULL)
		return;

	char buffer[MSL];
	time_t ptime = time(NULL);
	struct tm *loctime = localtime(&ptime);;
	strftime(buffer, MSL, "[%d/%m/%y %H:%M:%S]", loctime);
	fprintf(m_fp, buffer);

	fprintf(m_fp, "%s", szLog);
	fflush(m_fp);
}

void	Log::Printf(const char * szFormat, ...)
{
	if (m_fp == NULL)
		return;

	char buffer[MSL];
	time_t ptime = time(NULL);
	struct tm *loctime = localtime(&ptime);;
	strftime(buffer, MSL, "[%d/%m/%y %H:%M:%S] ", loctime);
	fprintf(m_fp, buffer);

	va_list vaList;
	va_start(vaList, szFormat);
	vfprintf(m_fp, szFormat, vaList);
	fflush(m_fp);
	va_end(vaList);
}

void	Log::Dump(const LPVOID b, DWORD packLen)
{
	if (m_fp == NULL)
		return;

	char	buf [MSL] = "  0 ";
	char	buf2[MSL] = "    ";
	char	temp[MSL];

	const BYTE * pack = (BYTE *)b;

	Printf("Data Dump (%d = %x)\n", packLen, packLen);
	Write("Row   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F\n");

	for (DWORD i = 0; i < packLen; i++)
	{
		if (i != 0 && i % 16 == 0)
		{
			Printf("%s\n", buf);
			Printf("%s\n", buf2);

			sprintf(buf, "%3x ", i / 16);
			sprintf(buf2, "    ");
		}

		sprintf(temp, "%3x ", pack[i]);
		strcat(buf, temp);

		if (pack[i] >= 30 && pack[i] < 128)
		{
			sprintf(temp, "%3c ", pack[i]);
			strcat(buf2, temp);
		} else
		{
			sprintf(temp, "    ", pack[i]);
			strcat(buf2, temp);
		}
	}

	Printf("%s\n", buf);
	Printf("%s\n\n", buf2);
}
