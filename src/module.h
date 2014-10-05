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
typedef const char* (*_getModuleName)(void);

/** Returns the name of the module */
typedef const char* (*_getModuleName)(void);

/** Version of libwsm's ABI used by the module */
typedef unsigned int (*_getABIVersion)(void);

struct wsm_priv_t
{
	wsm_t base;

	/* priv */
	void *dlhandle;
	void *user;

	_ctor ctor;
	_dtor dtor;
	_getModuleName getModuleName;
	_getABIVersion getABIVersion;
};

wsm_t *wsm_load_module(void);

#endif
