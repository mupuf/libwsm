#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>

#define DEBUG(format, args...) do { \
if (getenv("LIBWSM_DEBUG") && strcmp(getenv("LIBWSM_DEBUG"), "1") == 0) \
	fprintf(stderr, "libWSN: "format, ##args); \
} while(0);

#endif // DEBUG_H
