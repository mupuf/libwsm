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

#include <sys/types.h>
#include <stddef.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>

#include "module.h"
#include "debug.h"

struct wsm_priv_t *wsm_priv(wsm_t *wsm)
{
	return (struct wsm_priv_t*) wsm;
}

wsm_t *load_backend(const char *dir_path, const char *filename)
{
	char path[PATH_MAX];
	struct wsm_priv_t *w_p;

	//if the filename doesn't have the right prefix then try to load it as a shared object
	if(strncmp(filename, "libwsm_", 7))
		return NULL;

	DEBUG("try loading the backend '%s'\n", filename);

	snprintf(path, PATH_MAX-1, "%s/%s", dir_path, filename);

	/* load the shared object */
	void* dlhandle = dlopen(path, RTLD_NOW);
	if (dlhandle == NULL) {
		DEBUG("failed to dlopen the library '%s': %s\n", filename, dlerror());
		return NULL;
	}

	/* load the symbols */
	w_p = (struct wsm_priv_t *)malloc(sizeof(struct wsm_priv_t));
	w_p->dlhandle = dlhandle;
	w_p->ctor = (_ctor)dlsym(dlhandle, "ctor");
	w_p->dtor = (_dtor)dlsym(dlhandle, "dtor");
	w_p->getModuleName = (_getModuleName)dlsym(dlhandle, "getModuleName");
	w_p->getABIVersion = (_getABIVersion)dlsym(dlhandle, "getABIVersion");

	/* if minimal functions are here, add the lib to available modules */
	if(w_p->ctor == NULL || w_p->dtor == NULL ||
	   w_p->getModuleName == NULL || w_p->getABIVersion == NULL) {
		DEBUG("not all symbols are present, check version numbers\n");
		goto error;
	}

	/* check that the ABI version is no greater than the one from this lib */
	if (w_p->getABIVersion() > 1) {
		DEBUG("wrong ABI version (%i)\n", w_p->getABIVersion());
		goto error;
	}

	w_p->user = w_p->ctor();

	DEBUG("WSM module '%s' got loaded successfully\n", w_p->getModuleName());

	return (wsm_t *)w_p;

error:
	dlclose(dlhandle);
	free(w_p);
	return NULL;

}

wsm_t *wsm_load_module(void)
{
	DIR * backend_dir;
	struct dirent * mydirent;
	wsm_t *wsm = NULL;

	/* TODO: load the module the user actually wants! */

	backend_dir = opendir(DIRECTORY_PATH);
	if(backend_dir == NULL) {
		DEBUG("Could not open the backend directory '"
		      DIRECTORY_PATH"': %s.\n", strerror(errno));
		return NULL;
	}

	while ((mydirent = readdir(backend_dir))!=NULL && wsm == NULL)
		wsm = load_backend(DIRECTORY_PATH, mydirent->d_name);
	closedir(backend_dir);

	return wsm;
}

void wsm_unload_module(wsm_t *wsm)
{
	struct wsm_priv_t *wsm_p = wsm_priv(wsm);

	if (!wsm)
		return;

	wsm_p->dtor(wsm_p->user);
	dlclose(wsm_p->dlhandle);

	free(wsm);
}

