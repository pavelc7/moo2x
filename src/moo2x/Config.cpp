#include	"stdafx.h"

ConfigReader::ConfigReader()
{
}

ConfigReader::~ConfigReader()
{
}

bool	ConfigReader::RegisterVariable(const std::string & varName, CfgVarType varType, LPVOID lpVariable)
{
	ConfigItems_t::iterator iter = m_Items.find(varName);
	if (iter != m_Items.end())
		return false;

	ConfigItem	item;
	item.m_Name = varName;
	item.m_Type = varType;
	item.m_Variable = lpVariable;
	m_Items.insert(std::make_pair(varName, item));

	return true;
}

bool	ConfigReader::ReadConfig()
{
	FILE * fp = fopen("moo2x.cfg", "r");
	if (fp == NULL)
		return false;
	
	char	buf[MSL];

	while (!feof(fp))
	{
		fgets(buf, MSL, fp);

		char * c = buf;
		// Skip spaces
		while (*c != '\0' && isspace(*c))
			++c;

		if (*c == '#' || *c == '\0')
			continue;

		// Get name
		char * name = c;
		while (*c != '\0' && !isspace(*c))
			++c;
		
		if (*c == '\0')
			continue;

		// Got name
		*c = '\0';
		++c;

		// Skip spaces
		while (*c != '\0' && isspace(*c))
			++c;
		if (*c == '\0')
			continue;

		char * value = c;

		// Trim return from the end of the line
		char * t = c + strlen(c) - 1;
		while (t >= c && (*t == '\n' || *t == '\r'))
			--t;
		++t;

		if (t >= c)
			*t = '\0';

		std::string	vname(name);

		ConfigItems_t::iterator iter = m_Items.find(vname);
		if (iter != m_Items.end())
		{
			ConfigItem & item = iter->second;

			switch (item.m_Type)
			{
			case CfgVarType_Byte:
				{
					ui8 * pVar = (byte *)item.m_Variable;
					*pVar = atoi(value);
				}
				break;
			case CfgVarType_Short:
				{
					i16 * pVar = (short *)item.m_Variable;
					*pVar = atoi(value);
				}
				break;
			case CfgVarType_Int:
				{
					i32 * pVar = (int *)item.m_Variable;
					*pVar = atoi(value);
				}
				break;
			case CfgVarType_Float:
				{
					float * pVar = (float *)item.m_Variable;
					*pVar = (float)atof(value);
				}
				break;
			case CfgVarType_String:
				{
					std::string * pVar = (std::string *)item.m_Variable;
					*pVar = value;
				}
				break;
			case CfgVarType_Bool:
				{
					bool * pVar = (bool *)item.m_Variable;
					*pVar = atoi(value) != 0;
				}
				break;
			}
		}
	}

	fclose(fp);

	return true;
}

namespace	nConfig
{
	// Config variables
	bool	Render_FullScreen = false;

	bool	Debug_Breakpoint = true;

	bool	Debug_Console = false;

	// Config reader
	bool	Initialize()
	{
		ConfigReader	reader;
		reader.RegisterVariable("render.fullscreen", CfgVarType_Bool, &Render_FullScreen);
		reader.RegisterVariable("debug.breakpoint", CfgVarType_Bool, &Debug_Breakpoint);
		reader.RegisterVariable("debug.console", CfgVarType_Bool, &Debug_Console);

		return reader.ReadConfig();
	}

}