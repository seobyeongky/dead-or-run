#pragma once

#include "../internal.h"

#define ERROR_COCREATEINSTANCE -1;
#define ERROR_UPNP_NOT_FOUNDED -2;
#define ERROR_PORTMAPPING_FAILED -3;
#define ERROR_ALERY_BINDED -4

int SetPortForwarding(WCHAR *localIP, WCHAR *description, int internalPort, int externalPort);