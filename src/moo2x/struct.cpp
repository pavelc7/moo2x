#include	"stdafx.h"

int		struct_Test_Bit_Field_(const ui8 * pbStruct, ui16 bit)
{
	return ((pbStruct[bit >> 3] >> (bit & 7)) & 1) != 0;
}

void	struct_Set_Bit_Field_(ui8 * pbStruct, ui16 bit)
{
	pbStruct[bit >> 3] |= (1 << (bit & 7));
}

void	struct_Clear_Bit_Field_(ui8 * pbStruct, ui16 bit)
{
	pbStruct[bit >> 3] &= ~(1 << (bit & 7));
}

void	struct_Clear_Structure_(void * pbStruct, ui32 length)
{
	if (pbStruct != NULL)
		memset(pbStruct, 0, length);
}

void	struct_Copy_Structure_(void * pbTarget, const void * pbSource, ui32 length)
{
	if (pbTarget != NULL && pbSource != NULL)
		memcpy(pbTarget, pbSource, length);
}
