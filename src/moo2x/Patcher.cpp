#include	"stdafx.h"

namespace	nPatcher
{
	// Create jump (0xE9xxxxxxxx at targetAddr pointing to funcAddr)
	bool	CreateJmp(void * targetAddr, void * funcAddr)
	{
		DWORD oldProtect;

		if (!VirtualProtect(targetAddr, 5, PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;

		BYTE * temp = (BYTE *)targetAddr;
		*(temp++) = 0xE9;
		*(DWORD *)(temp) = (DWORD)funcAddr - (DWORD)targetAddr - 5;

		VirtualProtect(targetAddr, 5, oldProtect, &oldProtect);

		return true;
	}

	// Patch bytes at targetAddr with data in patchData with patchDataLen length
	bool	PatchData(void * targetAddr, void * patchData, ui32 patchDataLen)
	{
		DWORD oldProtect;

		if (!VirtualProtect(targetAddr, 5, PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;

		memcpy(targetAddr, patchData, patchDataLen);

		VirtualProtect(targetAddr, 5, oldProtect, &oldProtect);

		return true;
	}
}