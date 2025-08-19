#ifndef ZTEST_FIXTURES_H
#define ZTEST_FIXTURES_H

#include "zeitgeist_client.h"

#define SETUP_CLIENT_QUEUE () \
	init_client_payload_queue(128)
#define TEARDOWN_CLIENT_QUEUE_ () \
	destroy_client_payload_queue();



#endif
