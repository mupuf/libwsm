/*
Wayland Security Module - Default Backend
Copyright (C) 2014 Martin Peres & Steve Dodier-Lazaro

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	02110-1301	USA
*/

#include <sys/types.h>
#include <malloc.h>

#include <libwsm.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <wayland-util.h>
#include "config-parser.h"

#include "debug.h"

#define TMP_PREFIX					 	"../data/"
#define WSM_DEFAULT_POLICY_DIR			TMP_PREFIX"security/wsm/default"
#define WSM_DEFAULT_POLICY_PER_USER_DIR	TMP_PREFIX"security/wsm/per-user/default"

#define WSM_DEFAULT_DEFAULT_PATH		"*"
#define WSM_DEFAULT_DEFAULT_UID			-1

#define WSM_SCREENSHOT "WSM_SCREENSHOT"
#define WSM_SCREENSHARING "WSM_SCREENSHARING"
#define WSM_VIRTUAL_KEYBOARD "WSM_VIRTUAL_KEYBOARD"
#define WSM_VIRTUAL_POINTING "WSM_VIRTUAL_POINTING"
#define WSM_GLOBAL_KEYBOARD_SEQUENCE "WSM_GLOBAL_KEYBOARD_SEQUENCE"
#define WSM_FORWARD_RESERVED_KEYBOARD_SEQUENCE "WSM_FORWARD_RESERVED_KEYBOARD_SEQUENCE"
#define WSM_CLIPBOARD_COPY "WSM_CLIPBOARD_COPY"
#define WSM_CLIPBOARD_PASTE "WSM_CLIPBOARD_PASTE"


struct wsm_default_t {
	/* Name of the current compositor */
	char *compositor_name;

	/* List of policies for each application and UID */
	struct wl_list app_policies;
};

static struct wsm_default_t *_wsm_default_global = NULL;

struct wsm_default_client_t{
	/* Executable */
	wsm_client_info_t		*parent;
	char			 		*exe_path;
	pid_t			 		 pid;
	uid_t			 		 euid;
	struct weston_config	*policy;
};

struct wsm_app_policy_t {
	unsigned int		 	 version;
	char					*exe_path;
	signed long				 uid;		/* Can be -1 for the default UID */
	struct weston_config	*config;
	struct wl_list		 	 link;
};

struct wsm_cap_t {
	const char	 	*cap_name;
	struct wl_list	 objects; /* wsm_objperm_t */
	struct wl_list	 link;
};

struct wsm_objperm_t {
	const char	 	*object;
	const char	 	*permission;
	struct wl_list	 link;
};

struct wsm_default_client_t *wsm_default_client_lookup(int socket);
struct wsm_default_client_t *wsm_default_client_new(int socket);
void wsm_default_client_delete(struct wsm_default_client_t *app);
void wsm_default_client_change_permission(struct wsm_default_client_t *app, const char *cap_name, void *object, void *permission);

struct wsm_app_policy_t *wsm_app_policy_new(struct wsm_default_t *global, const char *path, const signed long uid, short * const existed);
void wsm_app_policy_register(struct wsm_default_t *global, struct wsm_app_policy_t *policy);
void wsm_app_policy_free(struct wsm_app_policy_t *policy);
struct wsm_app_policy_t *wsm_app_policy_lookup(struct wsm_default_t *global, const char *exe_path, const signed long uid);

int _string_starts_with(const char *str, const char *prefix)
{
	size_t pre_len = strlen(prefix);
	size_t str_len = strlen(str);

	if (pre_len > str_len)
		return 0;
	else
		return (strncmp(prefix, str, pre_len) == 0);
}

struct wsm_app_policy_t *wsm_app_policy_new(struct wsm_default_t *global, const char *path, const signed long uid, short * const existed)
{
	if (path == NULL)
		return NULL;

	// Factorising for later, will set to 1 when a policy is found to exist
	if (existed)
		*existed=0;

	struct weston_config *config;
	struct weston_config_section *section;
	char *exe_path = NULL;

	config = weston_config_parse(path);
	if (config != NULL) {
		DEBUG("Default Backend: wsm_app_policy_new: Using config file '%s'\n", weston_config_get_full_path(config));
	} else {
		DEBUG("Default Backend: wsm_app_policy_new: Could not parse policy file '%s'.\n", path);
	}
	section = weston_config_get_section(config, "Wayland Security Entry", NULL, NULL);
	weston_config_section_get_string(section, "Exec", &exe_path, NULL);

	if (!exe_path) {
		DEBUG("Default Backend: wsm_app_policy_new: Policy file '%s' is missing an executable path and will be discarded.\n", path);
		weston_config_destroy(config);
		return NULL;
	}

	struct wsm_app_policy_t *existing;

	if ((existing = wsm_app_policy_lookup(global, exe_path, uid)) != NULL)
	{
		DEBUG("Default Backend: wsm_app_policy_new: Found an existing policy for '%s;%ld'.\n", exe_path, uid);
		if (existed)
			*existed=1;

		weston_config_destroy(config);
		free(exe_path);
		return existing;
	} else {
		DEBUG("Default Backend: wsm_app_policy_new: Created a new policy for '%s;%ld'.\n", exe_path, uid);
		struct wsm_app_policy_t *policy = malloc(sizeof(struct wsm_app_policy_t));
		policy->version = (const unsigned int) 1;
		policy->exe_path = exe_path;
		policy->uid = uid;
		policy->config = config;

		wsm_app_policy_register(global, policy);

		return policy;
	}
}

void wsm_app_policy_register(struct wsm_default_t *global, struct wsm_app_policy_t *policy)
{
	wl_list_insert(&global->app_policies, &policy->link);
}

void wsm_app_policy_free(struct wsm_app_policy_t *policy)
{
	if (!policy)
		return;

	free(policy->exe_path);
	weston_config_destroy(policy->config);
	free(policy);
}

struct wsm_app_policy_t *wsm_app_policy_lookup(struct wsm_default_t *global, const char *exe_path, const signed long uid)
{
	if(!exe_path || (uid < 0 && uid != WSM_DEFAULT_DEFAULT_UID))
		return NULL;

	struct wsm_app_policy_t *policy;

	wl_list_for_each(policy, &global->app_policies, link) {
		if (strcmp(policy->exe_path, exe_path) == 0 && policy->uid == uid)
			return policy;
	}

	return NULL;
}

static int _filter_uid(const struct dirent *dir)
{
	if (dir == NULL)
		return 0;

	if (dir->d_type != DT_DIR)
		return 0;

	if (dir->d_name[0] == '.')
		return 0;

	errno = 0;
	long int uid = strtol(dir->d_name, 0, 10);
	if (errno)
	{
		DEBUG("Default Backend: Error when scanning for a user policy directory: folder name does not look like an UID (%s).", strerror(errno));
		return 0;
	} else
		return uid > 0;
}

static int _filter_all_files(const struct dirent *dir)
{
	if (dir == NULL)
		return 0;

	if (dir->d_type != DT_REG)
		return 0;

		return 1;
}

int scan_policy_folder(struct wsm_default_t *global, const char *path, const signed long uid)
{
	int created_files = 0;

	if (uid != WSM_DEFAULT_DEFAULT_UID) {
		DEBUG("Default Backend: Scanning directory '%s' for user '%ld' policies...\n", path, uid);
	} else {
		DEBUG("Default Backend: Scanning directory '%s' for policies...\n", path);
	}

	struct dirent **namelist = NULL;
	int nb_files = scandir(path, &namelist, _filter_all_files, alphasort);

	if (nb_files == -1 && errno != ENOENT) {
		DEBUG("Default Backend: Error when scanning a policy directory");
	} else {
		DEBUG("Default Backend: %d %s found in '%s'.\n", nb_files, (nb_files!=1? "policies":"policy"), path);
	}

	if (nb_files > 0) {
		size_t pathlen = strlen(path);

		int i=0;
		for(i=0; i<nb_files; ++i) {
			struct dirent *ent = namelist[i];
			char *ini_path = malloc(pathlen + strlen(ent->d_name) + 2);
			sprintf(ini_path, "%s/%s", path, ent->d_name);

			short existed;
			struct wsm_app_policy_t *policy = wsm_app_policy_new(global, ini_path, uid, &existed);
			if (policy && !existed)
				++created_files;

			free(ini_path);
			free(namelist[i]);
		}
		free(namelist);
	}

	return created_files;
}

static void _free_policy_list(struct wsm_default_t *global)
{
	struct wsm_app_policy_t *pol, *next_pol;

	if (global == NULL)
		return;

	wl_list_for_each_safe(pol, next_pol, &global->app_policies, link) {
		wsm_app_policy_free(pol);
	}
}

static int _init_policy_list(struct wsm_default_t *global)
{
	if(!global)
		return -1;

	wl_list_init(&global->app_policies);
	int total_policies = 0;

	DEBUG("Default Backend: Scanning user directories for per-user policies...\n");
	struct dirent **namelist = NULL;
	int nb_users = scandir(WSM_DEFAULT_POLICY_PER_USER_DIR, &namelist, _filter_uid, alphasort);
	if (nb_users == -1 && errno != ENOENT) {
		DEBUG("Default Backend: An error occurred when scanning user directories in '%s'.\n", WSM_DEFAULT_POLICY_PER_USER_DIR);
	} else
		DEBUG("Default Backend: %d %s found.\n", nb_users, (nb_users!=1? "user directories":"user directory"));

	if (nb_users > 0) {	
		size_t pathlen = strlen(WSM_DEFAULT_POLICY_PER_USER_DIR);
		int i;
		for (i=0; i<nb_users; ++i) {
			struct dirent *ent = namelist[i];
			char *folder_path = malloc(pathlen + strlen(ent->d_name) + 2);
			sprintf(folder_path, "%s/%s", WSM_DEFAULT_POLICY_PER_USER_DIR, ent->d_name);
			long uid = strtol(ent->d_name, 0, 10);
		
			int nb_policies = scan_policy_folder(global, folder_path, uid);
			if (nb_policies < 0) {
				DEBUG("Default Backend: An error occurred when looking for policies in '%s'.\n", folder_path);
			} else {
				total_policies += nb_policies;
			}

			free(folder_path);
			free(namelist[i]);
		}
		free(namelist);
	}

	DEBUG("Default Backend: Scanning the default policy directory...\n");
	int nb_policies = scan_policy_folder(global, WSM_DEFAULT_POLICY_DIR, WSM_DEFAULT_DEFAULT_UID);
	if (nb_policies < 0) {
		DEBUG("Default Backend: An error occurred when looking for policies in '%s'.\n", WSM_DEFAULT_POLICY_DIR);
	} else {
		total_policies += nb_policies;
	}

	DEBUG("Default Backend: %d %s were loaded in total.\n", total_policies, (total_policies!=1? "policies":"policy"));

	return total_policies;
}

void dtor(void *p_global)
{
	struct wsm_default_t *global = p_global;

	if(!global) {
		DEBUG("Default Backend: dtor: libwsm attempted to have me delete my internal data by passing a NULL pointer. This is a bug, please report it to the libwsm developers.\n");
		return;
	}

	_free_policy_list(global);
	free(global->compositor_name);
	free(global);
}

void *ctor(const char *compositor_name)
{
	struct wsm_default_t *global = malloc(sizeof(struct wsm_default_t));
	if(!global)
		return NULL;

	if (!_init_policy_list(global)) {
		DEBUG("Default Backend: ctor: could not find any policy file, please check you installed them at the right location.\nI expected to find default policies at '%s' and per-user policies at '%s' (in folders named after users' UIDs).\n", WSM_DEFAULT_POLICY_DIR, WSM_DEFAULT_POLICY_PER_USER_DIR);
		free(global);
		return NULL;
	}

	global->compositor_name = strdup(compositor_name);

	if(_wsm_default_global)
		dtor(_wsm_default_global);
	_wsm_default_global = global;

	return (void *)global;
}

const char* get_backend_name()
{
	return "default";
}

unsigned int get_ABI_version()
{
	return 1;
}

void *client_new(wsm_client_info_t info)
{
	if(!_wsm_default_global) {
		DEBUG("Default Backend: client_new: libwsm attempted to have the default backend initialise a policy for a new client but it is not initialised or has been deleted. This is a bug, please report it to the libwsm developers.\n");
		return NULL;
	}

	if (info.fullpath == NULL || info.uid < 0 || info.pid <= 0) {
		DEBUG("Default Backend: client_new: the default backend was asked to create a new client with invalid information. This should be a bug in libwsm. Path:'%s'\tUID:%d\tPID:%d.\n", info.fullpath, info.uid, info.pid);
		return NULL;
	}

	struct wsm_default_t		*global	= _wsm_default_global;
	struct wsm_default_client_t	*client	= NULL;
	struct wsm_app_policy_t		*pol	= NULL;

	client = malloc(sizeof(struct wsm_default_client_t));
	if (!client) {
		DEBUG("Default Backend: client_new: ran out of memory whilst creating a new client. Aborting.\n");
		return NULL;
	}

	pol = wsm_app_policy_lookup(global, info.fullpath, info.uid);
	if (!pol)
		pol = wsm_app_policy_lookup(global, info.fullpath, WSM_DEFAULT_DEFAULT_UID);
	if (!pol)
		pol = wsm_app_policy_lookup(global, WSM_DEFAULT_DEFAULT_PATH, info.uid);
	if (!pol)
		pol = wsm_app_policy_lookup(global, WSM_DEFAULT_DEFAULT_PATH, WSM_DEFAULT_DEFAULT_UID);

	if (!pol) {
		DEBUG("Default Backend: No policy could be found for client '%s\tUID:%d\tPID:%d', this is probably a bug in the backend or a mistake in your system configuration.\n", info.fullpath, info.uid, info.pid);
		free(client);
		return NULL;
	}

	client->exe_path = strndup(info.fullpath, PATH_MAX);
	client->pid = info.pid;
	client->euid = info.uid;
	client->policy = wsm_weston_config_copy(pol->config);

	return (void *) client;
}

void client_free(void *client)
{
	free((struct wsm_default_client_t *)client);
}
