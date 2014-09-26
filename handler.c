/*
	Copyright (C) 2013 - 2014 CurlyMo, 2014 BAPSystems design

	Pilight handler handler.c
   modified: 12-08-2014 BAP 
   modified: 14-09-2014 BAP motionsensors added  
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#include "pilight.h"
#include "common.h"
#include "settings.h"
#include "log.h"
#include "options.h"
#include "socket.h"
#include "ssdp.h"
#include "gc.h"

static int main_loop = 1;
static int sockfd = 0;

typedef enum {
	WELCOME,
	IDENTIFY,
	REJECT,
	RECEIVE
} steps_t;

struct devtranslate {
	const int id;
	const int unit;
	const char *stateon;
	const char *stateoff;	
	const char *location;
	const char *name;
};

const char stateopendeur[] = "Open";
const char statecloseddeur[] = "Dicht";
const char statelichtaan[] = "Aan";
const char statelichtuit[] = "Uit";
const char bijkeuken[] = "Bijkeuken";
const char tuindeur[] = "Tuindeur";
const char buitenlicht[] = "Buitenlicht";
const char bijkeukenlicht[] = "Bijkeukenlicht";
const char keuken[] = "Keuken";
const char keukendeur[] = "Keukendeur";
const char keukenlicht[] = "Keukenlicht";
const char keukendimmer[] = "Keukendimmer";
const char huiskamer[] = "Huiskamer";
const char huiskamerdeur[] = "Huiskamerdeur";
const char tv[] = "Televisie";
const char audio[] = "Audio";
const char leeslamp1[] = "Leeslamp-1";
const char leeslamp2[] = "Leeslamp-2";
const char eethoeklicht[] = "Eethoeklicht";
const char zithoeklicht[] = "Zithoeklicht";
const char hal[] = "Hal";
const char voordeur[] = "Voordeur";
const char hallicht[] = "Hallicht";
const char bel[] = "Bel";
const char statebelaan[] = "Gaat";
const char statebeluit[] = "Gaat niet";
const char overlooplicht[] = "Overlooplicht";
const char toilet[] = "Toilet";
const char toiletlicht[] = "Toiletlicht";
const char toiletdeur[] = "Toiletdeur";
const char overloop[] = "Overloop";
const char badkamer[] = "Badkamer";
const char badkamerlicht[] = "Badkamerlicht";
const char badkamerdeur[] = "Badkamerdeur";
const char slaapkamer[] = "Slaapkamer";
const char slaapkamerlicht[] = "Slaapkamerlicht";
const char slaapkamerdeur[] = "Slaapkamerdeur";
const char logeerkamer[] = "Logeerkamer";
const char logeerkamerlicht[] = "Logeerkamerlicht";
const char logeerkamerdeur[] = "Logeerkamerdeur";
const char studeerkamer[] = "Studeerkamer";
const char studeerkamerlicht[] = "Studeerkamerlicht";
const char studeerkamerdeur[] = "Studeerkamerdeur";
const char zolder[] = "Zolder";
const char zolderlicht[] = "Zolderlicht";
const char werkkamer[] = "Werkkamer";
const char werkkamerlicht[] = "Werkkamerlicht";
const char werkkamerlicht2[] = "Bureaulamp";
const char werkkamerdeur[] = "Werkkamerdeur";
const char statebeweging[] = "motion";
const char stategeenbeweging[] = "  none";
const char bewegingsensor1[] = "bewegingsensor 1";
const char bewegingsensor2[] = "bewegingsensor 2";

#define MAXKAKUDEVS	36

const struct devtranslate table[MAXKAKUDEVS] = {																// index1
	{ 12564786, 9, stateopendeur, statecloseddeur, bijkeuken, tuindeur },            //  00
	{ 9783342, 10, statelichtaan, statelichtuit, bijkeuken, buitenlicht },           //  01
	{ 9783342, 11, statelichtaan, statelichtuit, bijkeuken, bijkeukenlicht },			//  02
	{ 12991322, 9, stateopendeur, statecloseddeur, keuken, keukendeur },					//  03
	{ 10971658, 10, statelichtaan, statelichtuit, keuken, keukenlicht },					//  04
	{ 10971658, 11, statelichtaan, statelichtuit, keuken, keukendimmer },				//  05
	{ 2, 0, statelichtaan, statelichtuit, huiskamer, tv },									//  06
	{ 3801158, 0, statelichtaan, statelichtuit, huiskamer,  audio },						//  07	
	{ 0, 0, statelichtaan, statelichtuit, huiskamer, leeslamp1 },							//  08
	{ 1, 0, statelichtaan, statelichtuit, huiskamer, leeslamp2 },							//  09
	{ 11461082, 9, statelichtaan, statelichtuit, huiskamer, eethoeklicht },				//  10
	{ 11223898, 9, statelichtaan, statelichtuit, huiskamer, zithoeklicht },				//  11
	{ 12564894, 9, stateopendeur, statecloseddeur, huiskamer, huiskamerdeur },			//  12
	{ 12990902, 9, stateopendeur, statecloseddeur, hal, voordeur },						//  13
	{ 9787534, 11, statelichtaan, statelichtuit, hal, hallicht },							//  14
	{ 11081294, 1, statebelaan, statebeluit, hal, bel },										//  15
	{ 9787534, 10, statelichtaan, statelichtuit, hal, overlooplicht },					//  16
	{ 10711386, 9, statelichtaan, statelichtuit, toilet, toiletlicht },					//  17
	{ 12990958, 9, stateopendeur, statecloseddeur, toilet, toiletdeur },					//  18
	{ 11207438, 9, statelichtaan, statelichtuit, overloop, overlooplicht },				//  19
	{ 10703366, 9, statelichtaan, statelichtuit, overloop, badkamerlicht },				//  20
	{ 879672, 9, stateopendeur, statecloseddeur, badkamer, badkamerdeur },				//  21
	{ 11211686, 9, statelichtaan, statelichtuit, slaapkamer, slaapkamerlicht },		//  22
	{ 11225598, 9, statelichtaan, statelichtuit, slaapkamer, slaapkamerlicht },		//  23
	{ 865344, 9, stateopendeur, statecloseddeur, slaapkamer, slaapkamerdeur },			//  24
	{ 9698734, 9, statelichtaan, statelichtuit, logeerkamer, logeerkamerlicht },		//  25	
	{ 8619662, 9, stateopendeur, statecloseddeur, logeerkamer, logeerkamerdeur },		//  26
	{ 10706734, 9, statelichtaan, statelichtuit, studeerkamer, studeerkamerlicht },	//  27	
	{ 8619562, 9, stateopendeur, statecloseddeur, studeerkamer, studeerkamerdeur },	//  28			
	{ 10122818, 9, statelichtaan, statelichtuit, zolder, zolderlicht },					//  29
	{ 9541494, 9, statebeweging, stategeenbeweging, zolder, bewegingsensor2 },       //  30		
	{ 10710574, 9, statelichtaan, statelichtuit, overloop, zolderlicht },				//  31
	{ 10709846, 9, statelichtaan, statelichtuit, werkkamer, werkkamerlicht },			//  32
	{ 3819430, 0, statelichtaan, statelichtuit, werkkamer, werkkamerlicht2 },			//  33
	{ 620048, 9, stateopendeur, statecloseddeur, werkkamer, werkkamerdeur },			//  34
	{ 9943962, 9, statebeweging, stategeenbeweging, werkkamer, bewegingsensor1 }     //	 35	
};


int main_gc(void) {
	main_loop = 0;
	if(sockfd > 0) {
		socket_write(sockfd, "HEART");
	}
	sfree((void *)&progname);
	return 0;
}


int main(int argc, char **argv) {
	gc_attach(main_gc);

	/* Catch all exit signals for gc */
	gc_catch();

	log_shell_enable();
	log_file_disable();

	log_level_set(LOG_NOTICE);

	progname = malloc(16);
	if(!progname) {
		logprintf(LOG_ERR, "out of memory");
		exit(EXIT_FAILURE);
	}
	strcpy(progname, "pilight-handler");
	struct options_t *options = NULL;
	struct ssdp_list_t *ssdp_list = NULL;

	JsonNode *json = NULL;

	char *server = NULL;
	unsigned short port = 0;

   char *recvBuff = NULL;
	char *message = NULL;
	char *args = NULL;
	steps_t steps = WELCOME;

	options_add(&options, 'H', "help", OPTION_NO_VALUE, 0, JSON_NULL, NULL, NULL);
	options_add(&options, 'V', "version", OPTION_NO_VALUE, 0, JSON_NULL, NULL, NULL);
	options_add(&options, 'S', "server", OPTION_HAS_VALUE, 0, JSON_NULL, NULL, "^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]).){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
	options_add(&options, 'P', "port", OPTION_HAS_VALUE, 0, JSON_NULL, NULL, "[0-9]{1,4}");

	/* Store all CLI arguments for later usage
	   and also check if the CLI arguments where
	   used correctly by the user. This will also
	   fill all necessary values in the options struct */
	while(1) {
		int c;
		c = options_parse(&options, argc, argv, 1, &args);
		if(c == -1)
			break;
		if(c == -2)
			c = 'H';
		switch(c) {
			case 'H':
				printf("\t -H --help\t\t\tdisplay this message\n");
				printf("\t -V --version\t\t\tdisplay version\n");
				printf("\t -S --server=x.x.x.x\t\tconnect to server address\n");
				printf("\t -P --port=xxxx\t\t\tconnect to server port\n");
				exit(EXIT_SUCCESS);
			break;
			case 'V':
				printf("%s %s\n", progname, VERSION);
				exit(EXIT_SUCCESS);
			break;
			case 'S':
				server = realloc(server, strlen(args)+1);
				memset(server, '\0', strlen(args)+1);
				if(!server) {
					logprintf(LOG_ERR, "out of memory");
					exit(EXIT_FAILURE);
				}
				strcpy(server, args);
			break;
			case 'P':
				port = (unsigned short)atoi(args);
			break;
			default:
				printf("Usage: %s -l location -d device\n", progname);
				exit(EXIT_SUCCESS);
			break;
		}
	}
	options_delete(options);

	if(server && port > 0) {
		if((sockfd = socket_connect(server, port)) == -1) {
			logprintf(LOG_ERR, "could not connect to pilight-daemon");
			return EXIT_FAILURE;
		}
	} else if(ssdp_seek(&ssdp_list) == -1) {
		logprintf(LOG_ERR, "no pilight ssdp connections found");
		goto close;
	} else {
		if((sockfd = socket_connect(ssdp_list->ip, ssdp_list->port)) == -1) {
			logprintf(LOG_ERR, "could not connect to pilight-daemon");
			goto close;
		}
	}
	if(ssdp_list) {
		ssdp_free(ssdp_list);
	}
	if(server) {
		sfree((void *)&server);
	}
	
	while(main_loop) {
		if(steps > WELCOME) {
			if((recvBuff = socket_read(sockfd)) == NULL) {
				goto close;
			}
		}
		switch(steps) {
			case WELCOME:
				socket_write(sockfd, "{\"message\":\"client receiver\"}");
				steps=IDENTIFY;
			break;
			case IDENTIFY:
				//extract the message
				json = json_decode(recvBuff);
				json_find_string(json, "message", &message);
				if(strcmp(message, "accept client") == 0) {
					steps=RECEIVE;
				} else if(strcmp(message, "reject client") == 0) {
					steps=REJECT;
				}
				//cleanup
				json_delete(json);
				sfree((void *)&recvBuff);
				json = NULL;
				message = NULL;
				recvBuff = NULL;
			break;
			case RECEIVE: {
				char *line = strtok(recvBuff, "\n");
				//for each line
				while(line) {
					int id = -1;
					int unit = -1;
//					int repeats = -1;
					struct JsonNode *code;
					char *state = NULL;
					char *uid = NULL;
					char *origin = NULL;
					char *protocol = NULL;
					struct timeval tp;
					struct tm *tm;							
					int index1; 
					double itmp = 0;		
											
					json = json_decode(line);
					if((code = json_find_member(json, "code"))) {					
						if (json_find_number(code, "id", &itmp) == 0) {
							id = (int)round(itmp);							
							if(json_find_number(code, "unit", &itmp) == 0) {
								unit = (int)round(itmp);
								if(json_find_string(code, "state", &state) == 0) {
									if(json_find_string(json, "uuid", &uid) == 0) {
										;
									}
									json_find_string(json, "origin", &origin);
									if (strcmp(origin, "sender") == 0)
									{
										json_find_number(json, "repeat", &itmp);
//										repeats = (int)round(itmp);
									}	
									else {
										json_find_number(json, "repeats", &itmp);	
//										repeats = (int)round(itmp);										
									}
									json_find_string(json, "protocol", &protocol);	

									for (index1 = 0; index1 < MAXKAKUDEVS; index1++)
									{
										if (table[index1].id == id && 	table[index1].unit == unit) {
											gettimeofday(&tp, NULL);												
											break;
										}
									}
									if (index1 <	MAXKAKUDEVS) { // found
										printf("%s in %s %s ", table[index1].name, table[index1].location, state);
										if((tm = localtime(&tp.tv_sec)) != NULL) {
											printf("at %d-%02d-%02d %d:%02d:%02d\n", (tm->tm_year + 1900), tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);											
										}
										else {
											printf("\n");	
										}										
									}
								}	
							}	
						}
					}	
					json_delete(json);
					line = strtok(NULL,"\n");
				}
				sfree((void *)&recvBuff);
				sfree((void *)&line);
				recvBuff = NULL;
			} break;
			case REJECT:
			default:
				goto close;
			break;
		}
	}
close:
	if(sockfd > 0) {
		socket_close(sockfd);
	}
	if(recvBuff) {
		sfree((void *)&recvBuff);
	}
	options_gc();
	log_shell_disable();
	log_gc();
	return EXIT_SUCCESS;
}
