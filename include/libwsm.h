/*
Copyright (c) 2014 Martin Peres

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef LIBWSM_H
#define LIBWSM_H
//

/**
 * \file libwsm.h
 * \author MùPùF - Martin Peres (martin dot peres at free dot fr)
 * \date 05-10-2014
 */

#include <sys/types.h>

#define WSM_SOFT_ALLOW "soft-allow"
#define WSM_SOFT_DENY "soft-deny"
#define WSM_ALLOW "allow"
#define WSM_DENY "deny"

typedef struct wsm_t {} wsm_t;
typedef struct wsm_client_t {} wsm_client_t;
typedef struct wsm_client_info_t {
	int fd;
	pid_t pid;
	uid_t uid;
	gid_t gid;
	char *fullpath; /* TODO: Check for ABI problems */
} wsm_client_info_t;

wsm_t *wsm_init();
void wsm_fini(wsm_t *wsm);

wsm_client_t *wsm_client_new(wsm_t *wsm, int client_fd);
void wsm_client_free(wsm_client_t *wsm_client);

wsm_client_info_t wsm_client_info_get(wsm_client_t *wsm_client);

char *wsm_client_get_permission(wsm_client_t *wsm_client, const char *capability, const char *object);

#endif
