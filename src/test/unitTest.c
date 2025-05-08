
#include <signal.h>
#include <pthread.h>

#include "test/unitTest.h"
#include "print-data.h"

#include "test/test-rsmp-messages.h"
#include "test/test-rsmp-sxl.h"

#include "test/test_disp_all_sxx.h"

// #include "test-auth.h"
// #include "test-cipher.h"

#define QUOTE_EXAMPLE_TEST_0 "Hello World -- anonymous"
#define QUOTE_EXAMPLE QUOTE_EXAMPLE_TEST_0

// clé ayant servi à la dérivation pour en générer une nouvelle
// suivant l'exemple test-0.txt
#define KEYPASS_HEX_EXAMPLE_TEST_0 "736974655f7349645f41756e206c6f67696e5dec58503c7f9dcaed3fcfa4dd001ac4132d7a0cb2e6eed148736974655f7349645f42"
#define KEYPASS_HEX_EXAMPLE KEYPASS_HEX_EXAMPLE_TEST_0



int unitTest(int *Stop, int testCase)
{
     _log_level_set(LOG_LEVEL_ALL);
     
	switch (testCase)
	{
        
    case 1: 
        test_rsmp_messages();
        break;
    case 2:
            _log_info("test_rsmp_sxl");
    sleep(1);
        test_rsmp_sxl();
    exit(0);
    case 3:
        test_disp_s10();
        break;
    case 4:
        test_disp_s20();
        break;
    case 5:
        test_disp_s30();
        break;
    case 6:
        test_disp_s40();
        break;

    
    
    
    // case 100:
    //     test_auth(0, NULL);
    //     break;
    // case 101:
    //     test_cipher(0, NULL);
    //     break;

	default:
		_log_debug("testCase = %d not found", testCase);
        return -1;
	}

    return 0;
}