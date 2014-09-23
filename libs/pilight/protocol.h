/*
	Copyright (C) 2013 CurlyMo

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

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <pthread.h>

#include "options.h"
#include "threads.h"
#include "hardware.h"
#include "json.h"

typedef enum {
	INTERNAL = -2,
	PROC = -1,
	RAW = 0,
	SWITCH,
	DIMMER,
	WEATHER,
	RELAY,
	SCREEN,
	CONTACT,
	PENDINGSW,
	DATETIME,
	XBMC,
	LIRC,
	WEBCAM
} devtype_t;

typedef struct protocol_devices_t {
	char *id;
	char *desc;
	struct protocol_devices_t *next;
} protocol_devices_t;

typedef struct protocol_plslen_t {
	int length;
	struct protocol_plslen_t *next;
} protocol_plslen_t;

typedef struct protocol_threads_t {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_mutexattr_t attr;
	JsonNode *param;
	struct protocol_threads_t *next;
} protocol_threads_t;

typedef struct protocol_t {
	char *id;
	int header;
	int pulse;
	struct protocol_plslen_t *plslen;
	int footer;
	int rawlen;
	int minrawlen;
	int maxrawlen;
	int binlen;
	short txrpt;
	short rxrpt;
	short multipleId;
	short config;
	unsigned short lsb;
	struct options_t *options;
	JsonNode *message;

	int repeats;
	unsigned long first;
	unsigned long second;

	int bit;
	int recording;
	int raw[255];
	int code[255];
	int pCode[255];
	int binary[128]; // Max. the half the raw length

	hwtype_t hwtype;
	devtype_t devtype;
	struct protocol_devices_t *devices;
	struct protocol_threads_t *threads;

	void (*parseRaw)(void);
	void (*parseCode)(void);
	void (*parseBinary)(void);
	int (*createCode)(JsonNode *code);
	int (*checkValues)(JsonNode *code);
	struct threadqueue_t *(*initDev)(JsonNode *device);
	void (*printHelp)(void);
	void (*gc)(void);
	void (*threadGC)(void);
} protocol_t;

typedef struct protocols_t {
	struct protocol_t *listener;
	char *name;
	struct protocols_t *next;
} protocols_t;

struct protocols_t *protocols;

#define ID		pCode[254]
#define UNIT	code[254]
#define STATE	raw[254]
#define ALL		binary[127]

void protocol_init(void);
struct protocol_threads_t *protocol_thread_init(protocol_t *proto, struct JsonNode *param);
int protocol_thread_wait(struct protocol_threads_t *node, int interval, int *nrloops);
void protocol_thread_free(protocol_t *proto);
void protocol_thread_stop(protocol_t *proto);
void protocol_set_id(protocol_t *proto, const char *id);
void protocol_plslen_add(protocol_t *proto, int plslen);
void protocol_register(protocol_t **proto);
void protocol_device_add(protocol_t *proto, const char *id, const char *desc);
int protocol_device_exists(protocol_t *proto, const char *id);
int protocol_gc(void);

#endif
