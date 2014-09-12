/*
  Copyright (C) 2014 CurlyMo & lvdp & bennybap

  This file is part of pilight.

    pilight is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

    pilight is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with pilight. If not, see <http://www.gnu.org/licenses/>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../pilight.h"
#include "common.h"
#include "dso.h"
#include "log.h"
#include "protocol.h"
#include "hardware.h"
#include "binary.h"
#include "gc.h"
#include "motion_sensor.h"

static void motionSensorCreateMessage(int id, int unit, int state, int all) {
	motion_sensor->message = json_mkobject();
	json_append_member(motion_sensor->message, "id", json_mknumber(id));
	if(all == 1) {
		json_append_member(motion_sensor->message, "all", json_mknumber(all));
	} else {
		json_append_member(motion_sensor->message, "unit", json_mknumber(unit));
	}

	if(state == 1) {
		json_append_member(motion_sensor->message, "state", json_mkstring("motion"));
	} else {
		json_append_member(motion_sensor->message, "state", json_mkstring("  none"));
	}
}

static void motionSensorParseBinary(void) {
	int unit = binToDecRev(motion_sensor->binary, 28, 31);
	int state = motion_sensor->binary[27];
	int all = motion_sensor->binary[26];
	int id = binToDecRev(motion_sensor->binary, 0, 25);

	motionSensorCreateMessage(id, unit, state, all);
}

#ifndef MODULE
__attribute__((weak))
#endif
void motionSensorInit(void) {

	protocol_register(&motion_sensor);
	protocol_set_id(motion_sensor, "motion_sensor");
	protocol_device_add(motion_sensor, "motion_sensor", "KlikAanKlikUit AWST-6000");
	protocol_plslen_add(motion_sensor, 279);

	motion_sensor->devtype = SWITCH;
	motion_sensor->hwtype = RF433;
	motion_sensor->pulse = 4;
	motion_sensor->rawlen = 132;
	motion_sensor->lsb = 3;

	options_add(&motion_sensor->options, 'u', "unit", OPTION_HAS_VALUE, CONFIG_ID, JSON_NUMBER, NULL, "^([0-9]{1}|[1][0-5])$");
	options_add(&motion_sensor->options, 'i', "id", OPTION_HAS_VALUE, CONFIG_ID, JSON_NUMBER, NULL, "^([0-9]{1,7}|[1-5][0-9]{7}|6([0-6][0-9]{6}|7(0[0-9]{5}|10([0-7][0-9]{3}|8([0-7][0-9]{2}|8([0-5][0-9]|6[0-3]))))))$");
	options_add(&motion_sensor->options, 't', "motion", OPTION_NO_VALUE, CONFIG_STATE, JSON_STRING, NULL, NULL);
	options_add(&motion_sensor->options, 'f', "  none", OPTION_NO_VALUE, CONFIG_STATE, JSON_STRING, NULL, NULL);

	options_add(&motion_sensor->options, 'a', "all", OPTION_HAS_VALUE, CONFIG_SETTING, JSON_NUMBER, (void *)0, "^[10]{1}$");
	options_add(&motion_sensor->options, 0, "gui-readonly", OPTION_HAS_VALUE, CONFIG_SETTING, JSON_NUMBER, (void *)0, "^[10]{1}$");

	motion_sensor->parseBinary=&motionSensorParseBinary;
}

#ifdef MODULE
void compatibility(struct module_t *module) {
	module->name = "motion_sensor";
	module->version = "1.0";
	module->reqversion = "5.0";
	module->reqcommit = NULL;
}

void init(void) {
	motionSensorInit();
}
#endif
