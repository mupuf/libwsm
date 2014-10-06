/*
Wayland Security Module
Copyright (C) 2014 Martin Peres & Steve Dodier-Lazaro

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <sys/types.h>
#include <malloc.h>

#include <libwsm.h>

struct wsm_default_t{
	/* */
};

struct wsm_default_client_t{
	/* */
};

void *ctor(void)
{
	void *user = malloc(sizeof(struct wsm_default_t));

	/* init */

	return user;
}

void dtor(void *user)
{
	free(user);
}

const char* getModuleName()
{
	return "default";
}

unsigned int getABIVersion()
{
	return 1;
}

void *client_new(wsm_client_info_t info)
{
	void *user = malloc(sizeof(struct wsm_default_client_t));

	/* init */

	return user;
}

void client_free(void *user)
{
	free(user);
}
