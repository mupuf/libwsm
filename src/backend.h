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

#ifndef BACKEND_H
#define BACKEND_H

#include <libwsm.h>

/**
 * \file backend.h
 * \author Martin Peres
 * \author Steve Dodier-Lazaro
 * \date 2014-10-05
 */

/** Init the backend */
typedef void *(*_ctor)(const char *);

/** free the backend */
typedef void (*_dtor)(void *);

/** Returns the name of the backend */
typedef const char* (*_get_backend_name)(void);

/** Version of libwsm's ABI used by the backend */
typedef unsigned int (*_get_ABI_version)(void);

/** Allocate a new client */
typedef void * (*_client_create)(wsm_client_info_t info);

/** Free a client */
typedef void * (*_client_destroy)(void *user);

/** Make a decision for a given client, capability and object */
typedef wsm_decision_t (*_get_permission)(void *user, const char *capability, const char *object);

/** Make a decision, supporting custom answers */
typedef char * (*_get_custom_permission)(void *user, const char *capability, const char *object);

struct wsm_priv_t
{
	wsm_t base;

	/* priv */
	void *dlhandle;
	void *user;

	_ctor ctor;
	_dtor dtor;
	_get_backend_name get_backend_name;
	_get_ABI_version get_ABI_version;

	_client_create client_create;
	_client_destroy client_destroy;

	_get_permission get_permission;
	_get_custom_permission get_custom_permission;
};

struct wsm_client_priv_t
{
	wsm_client_t base;

	/* priv */
	struct wsm_priv_t *wsm_p;
	wsm_client_info_t info;
	void *user;
};

struct wsm_priv_t *wsm_priv(wsm_t *wsm);

char *wsm_get_path_from_pid(const pid_t pid);
wsm_t *wsm_load_backend(void);
void wsm_unload_backend(wsm_t *wsm);

#endif
