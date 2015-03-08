/* Copyright (c) 2014 Martin Peres, Steve Dodier-Lazaro

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

#include <sys/socket.h>
#include <stddef.h>
#include <malloc.h>
#include <errno.h>

#include <libwsm.h>
#include "backend.h"
#include <wsm/debug.h>

static struct wsm_client_priv_t *wsm_client_priv(wsm_client_t *wsm_client)
{
	return (struct wsm_client_priv_t*) wsm_client;
}

wsm_t *wsm_create()
{
	return wsm_load_backend();
}

void wsm_destroy(wsm_t *wsm)
{
	wsm_unload_backend(wsm);
}

wsm_client_t *wsm_client_create(wsm_t *wsm, struct wl_client *client)
{
	struct wsm_priv_t *wsm_p = wsm_priv(wsm);
	struct wsm_client_priv_t *client_p;
	pid_t pid;
	uid_t uid;
	gid_t gid;

	if (!wsm)
		return NULL;

	wl_client_get_credentials (client, &pid, &uid, &gid);

	client_p = (struct wsm_client_priv_t *)malloc(sizeof(struct wsm_client_priv_t));
	if (!client_p) {
		DEBUG("Failed to allocate the client structure. Abort.");
		return NULL;
	}

	client_p->wsm_p = wsm_p;
	client_p->info.uid = uid;
	client_p->info.gid = gid;
	client_p->info.pid = pid;
	client_p->info.fullpath = wsm_get_path_from_pid(pid);
	client_p->user = wsm_p->client_create(client_p->info);

	return (wsm_client_t *)client_p;
}

void wsm_client_destroy(wsm_client_t *wsm_client)
{
	struct wsm_client_priv_t *c_p = wsm_client_priv(wsm_client);

	if (!wsm_client)
		return;

	if (c_p->info.fullpath)
		free(c_p->info.fullpath);
	c_p->wsm_p->client_destroy(c_p->user);
	free(c_p);
}

wsm_client_info_t wsm_client_info_get(wsm_client_t *wsm_client)
{
	struct wsm_client_priv_t *c_p = wsm_client_priv(wsm_client);
	struct wsm_client_info_t dummy = {0,};

	if (!wsm_client)
		return dummy;

	return c_p->info;
}

wsm_decision_t wsm_client_get_permission(wsm_client_t *wsm_client, const char *capability, const char *object)
{
	struct wsm_client_priv_t *c_p = wsm_client_priv(wsm_client);

	if (!wsm_client)
		return WSM_DECISION_ERROR;

	return c_p->wsm_p->get_permission(c_p->user, capability, object);
}

char *wsm_client_get_custom_permission(wsm_client_t *wsm_client, const char *capability, const char *object)
{
	struct wsm_client_priv_t *c_p = wsm_client_priv(wsm_client);

	if (!wsm_client)
		return NULL;

	return c_p->wsm_p->get_custom_permission(c_p->user, capability, object);
}
