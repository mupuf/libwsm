#include <libwsm.h>

#undef NDEBUG
#include <assert.h>
#include "unix_socket.h"

#define UNIX_SOCKET_PATH "./socket"

int main(int argc, char **argv)
{
	int sfd, cfd;

	/* Init a connection */
	sfd = unix_socket_server_create(UNIX_SOCKET_PATH); assert(sfd > 0);
	assert(unix_socket_client_connect(UNIX_SOCKET_PATH) > 0);
	cfd = unix_socket_server_accept(sfd); assert(cfd > 0);

	wsm_t *wsm = wsm_init();
	assert(wsm);

	wsm_client_t *wsm_client = wsm_client_new(wsm, cfd);
	assert(wsm_client);

	wsm_client_free(wsm_client);
	wsm_fini(wsm);

	return 0;
}
