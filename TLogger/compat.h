#pragma once

#define PRG_NAME	"TLogger"
#define PRG_VERSION	"1.00"
#define PRG_RELDATE	"2014/05/20"
#define PRG_AUTHOR	"luke"
#define PRG_VER		PRG_NAME " " PRG_VERSION " " PRG_RELDATE " by " PRG_AUTHOR

#define PRG_CONFIG_FILE		"tlogger.cfg"
#define APPID				"TLOGGER"


/*

1.00	2014/05/20

*/

// common definition

#define   WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include  <windows.h>
#include "Config.h"

#define LOG_NORMAL	0
#define LOG_WARN	1
#define LOG_ERROR	2

extern CConfig config;

