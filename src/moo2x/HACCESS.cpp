#include	"stdafx.h"

ui16 HACCESS_Ship_Owner_(i16 ship_id)
{
	ui16	flag = 0;

	char	b[512];
	sprintf(b, "%d %d", sizeof(MOX__ship_t), sizeof(MOX__ship_weapon_t));

	if (ship_id >= 0)
	{
		if (ship_id < NEWMAX_SHIPS)
		{
			i8	owner = MOX__ship[ship_id].current_owner;
			if (owner >= 0)
			{
				if (owner <= 15)
				{
					return owner;
				} else
				{
					flag = 4;
				}
			} else
			{
				flag = 3;
			}
		} else
		{
			flag = 2;
		}
	} else
	{
		flag = 1;
	}

	if (flag != 0)
	{
		char	buf[MSL];

		sprintf(buf, "Memory Corruption! val == %d, ship_id == %d, owner == %d stardate = %ld\n", 
						flag, 
						ship_id,
						MOX__ship[ship_id].current_owner,
						MOX__stardate
						);

		exit_Exit_With_Message(buf);
	}

	return 0;
}

