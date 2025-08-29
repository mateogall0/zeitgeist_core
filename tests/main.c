#include "test_utils.h"
#include "server/static.h"

int main()
{
    int _test_counter = 0;
    set_log_requests(false);

    TESTS;

    printf("\nAll tests passed!\n");
    return (0);
}
