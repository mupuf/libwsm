#include <libwsm.h>

#undef NDEBUG
#include <assert.h>
#include "unix_socket.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNIX_SOCKET_PATH "./socket"

int main(int argc, char **argv)
{
	int sfd, cfd, scfd;
	wsm_client_info_t info;

	sfd = unix_socket_server_create(UNIX_SOCKET_PATH); assert(sfd > 0);
	cfd = unix_socket_client_connect(UNIX_SOCKET_PATH); assert(cfd > 0);
	scfd = unix_socket_server_accept(sfd); assert(cfd > 0);

	wsm_t *wsm = wsm_create();
	assert(wsm);

	wsm_client_t *wsm_client = wsm_client_create(wsm, scfd);
	assert(wsm_client);

	info = wsm_client_info_get(wsm_client);

	assert(info.fd == scfd);
	assert(info.pid == getpid());
	assert(info.uid == getuid());
	assert(info.gid == getgid());

	if (argv[0][0] == '/') {
		assert(info.fullpath);
		assert(strcmp(info.fullpath, argv[0]) == 0);
	} else
		fprintf(stderr, "Impossible to run the 'fullpath' check. "
				"Run the test with an absolute path.\n");

	char *permission = wsm_client_get_custom_permission(wsm_client, "_TESTING_CAPABILITY", NULL);
	assert(permission);
	free(permission);

	wsm_decision_t dec = wsm_client_get_permission(wsm_client, "_WESTON_FULLSCREEN", NULL);
	assert(dec != WSM_DECISION_ERROR);

	wsm_client_destroy(wsm_client);
	wsm_destroy(wsm);

	close(scfd);
	close(cfd);
	close(sfd);

	return 0;
}
