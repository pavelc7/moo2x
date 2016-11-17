#pragma once

enum	CfgVarType
{
	CfgVarType_Byte = 0,
	CfgVarType_Short,
	CfgVarType_Int,
	CfgVarType_Float,
	CfgVarType_String,
	CfgVarType_Bool
};

class	ConfigReader
{
public:
	ConfigReader();
	~ConfigReader();

	bool	RegisterVariable(const std::string & varName, CfgVarType varType, LPVOID lpVariable);

	bool	ReadConfig();
private:
	struct	ConfigItem
	{
		std::string	m_Name;
		CfgVarType	m_Type;
		void*		m_Variable;
	};
	typedef	std::map<std::string, ConfigItem>	ConfigItems_t;
	ConfigItems_t	m_Items;
};

namespace	nConfig
{
	// Config variables
	extern	bool	Render_FullScreen;
	
	extern	bool	Debug_Breakpoint;
	extern bool Debug_Console;
	// Initializer
	bool	Initialize();
}