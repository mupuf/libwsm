#include <libwsm.h>

#undef NDEBUG
#include <assert.h>

int main(int argc, char **argv)
{
	wsm_t *wsm = wsm_init();
	assert(wsm);

	wsm_client_t *wsm_client = wsm_client_new(wsm, 3);
	assert(wsm_client);

	wsm_client_free(wsm_client);
	wsm_fini(wsm);

	return 0;
}
