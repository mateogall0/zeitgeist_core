#include "test_utils.h"
#include "server/static.h"


int main()
{
    set_log_requests(false);

    TESTS;

    printf("\nAll tests passed!\n");
    return (0);
}
