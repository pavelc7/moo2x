#pragma once

namespace	nPatcher
{
	bool	CreateJmp(void * targetAddr, void * funcAddr);

	bool	PatchData(void * targetAddr, void * patchData, ui32 patchDataLen);
}
