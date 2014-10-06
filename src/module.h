/* Copyright (c) 2014 Martin Peres

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

#ifndef MODULE_H
#define MODULE_H

#include <libwsm.h>

/**
 * \file module.h
 * \author Martin Peres
 * \date 05-10-2014
 */

/** Init the module */
typedef void *(*_ctor)(void);

/** free the module */
typedef void (*_dtor)(void *);

/** Returns the name of the module */
typedef const char* (*_get_module_name)(void);

/** Version of libwsm's ABI used by the module */
typedef unsigned int (*_get_ABI_version)(void);

/** Allocate a new client */
typedef void * (*_client_new)(wsm_client_info_t info);

/** Free a client */
typedef void * (*_client_free)(void *user);

struct wsm_priv_t
{
	wsm_t base;

	/* priv */
	void *dlhandle;
	void *user;

	_ctor ctor;
	_dtor dtor;
	_get_module_name get_module_name;
	_get_ABI_version get_ABI_version;

	_client_new client_new;
	_client_free client_free;
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
wsm_t *wsm_load_module(void);
void wsm_unload_module(wsm_t *wsm);

#endif
