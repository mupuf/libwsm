#include <libwsm.h>

int main(int argc, char **argv)
{
	wsm_t *wsm = wsm_init();

	wsm_fini(wsm);

	return 0;
}
