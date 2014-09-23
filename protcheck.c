/*
	Copyright (C) 2014 - bennybap

	This file is part of pilight.

    pilight is free software: you can redistribute it and/or modify it under the
	terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later
	version.

    pilight is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with pilight. If not, see	<http://www.gnu.org/licenses/>
*/

#include "protocol.h"

struct devtranslate {
	const int id;
	const int unit;
	const char *protname;
};

#define MAXKAKUDEVS	36

const char arctechContact[] = "arctech_contact";
const char arctechSwitch[] = "arctech_switch";
const char arctech_dimmer[] = "arctech_dimmer";
const char motion_sensor[] = "motion_sensor";
const char arctechSwitchOld[] = "arctech_switch_old";

const struct devtranslate table[MAXKAKUDEVS] = {	//	 index
	{ 12564786, 9, 	arctechContact 	},        	//  00
	{ 9783342, 	10, 	arctechSwitch 		},          //  01
	{ 9783342, 	11, 	arctechSwitch 		},				//  02
	{ 12991322, 9, 	arctechContact 	},				//  03
	{ 10971658, 10, 	arctechSwitch 		},				//  04
	{ 10971658, 11, 	arctech_dimmer 	},				//  05
	{ 2, 			0, 	arctechSwitchOld 	},				//  06
	{ 3801158, 	0, 	arctechSwitch 		},				//  07	
	{ 0, 			0, 	arctechSwitchOld 	},				//  08
	{ 1, 			0, 	arctechSwitchOld 	},				//  09
	{ 11223898, 9, 	arctechSwitch 		},				//  10	
	{ 11461082, 9, 	arctechSwitch 		},				//  11
	{ 12564894, 9, 	arctechContact 	},				//  12
	{ 12990902, 9, 	arctechContact 	},				//  13
	{ 11081294, 1, 	arctechContact 	},				//  14	
	{ 9787534, 11, 	arctechSwitch 		},				//  15
	{ 12990958, 9, 	arctechContact 	},				//  16	
	{ 10711386, 9, 	arctechSwitch 		},				//  17	
	{ 11207438, 9, 	arctechSwitch 		},				//  18	
	{ 9787534, 	10, 	arctechSwitch 		},				//  19
	{ 879672, 	9, 	arctechContact 	},				//  20
	{ 10703366, 9, 	arctechSwitch 		},				//  21
	{ 865344, 	9, 	arctechContact 	},				//  22
	{ 11225598, 9, 	arctechSwitch 		},				//  23	
	{ 11211686, 9, 	arctechSwitch 		},				//  24
	{ 8619662, 	9, 	arctechContact 	},				//  25
	{ 9698734, 	9, 	arctech_dimmer 	},				//  26	
	{ 8619562, 	9, 	arctechContact 	},				//  27	
	{ 10706734, 9, 	arctechSwitch 		},				//  28	
	{ 10122818, 9, 	arctech_dimmer 	},				//  29
	{ 9943962, 	9, 	motion_sensor 		},     		//	 30	
	{ 3819430, 	0, 	arctechSwitch 		},				//  31	
	{ 10709846, 9, 	arctechSwitch 		},				//  32	
	{ 10710574, 9, 	arctechSwitch 		},				//  33	
	{ 620048, 	9, 	arctechContact 	},				//  34	
	{ 9541494, 	9, 	motion_sensor 		}       		//  35	
};


int CheckProtocol(struct protocol_t *protocol)
{
	int error = -1, index;
	
	for (index = 0; index < MAXKAKUDEVS; index++) {
		if ((protocol->ID == table[index].id) && (protocol->UNIT == table[index].unit)) {
			if (strcmp(protocol->id, table[index].protname) == 0) {
				error = 0;	
			}
			break;
		}	
	}
	if ((error == -1) && protocol->message) {
		json_delete(protocol->message);
		protocol->message = NULL;
	}
	return(error);
}