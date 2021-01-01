#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "math.h"
#include <time.h>
#include <stdlib.h>
#include "xil_cache.h"
#include "xil_types.h"
#include "xtime_l.h"
#include <sleep.h>

#include "xlite_to_stream.h"
#include "xcompare.h"

#define A 4
#define S 11

XLite_to_stream ls ;
XCompare compare ;

int ls_init(XLite_to_stream *ptr){

	XLite_to_stream_Config *cfgPtr;
    int status;

    cfgPtr = XLite_to_stream_LookupConfig(XPAR_LITE_TO_STREAM_DEVICE_ID);
    if (!cfgPtr) {
        print("ERROR: Lookup failed.\n\r");
        return XST_FAILURE;
    }

    status = XLite_to_stream_CfgInitialize(ptr, cfgPtr);
    if (status != XST_SUCCESS) {
        print("ERROR: Could not initialize.\n\r");
        return XST_FAILURE;
    }

    return status;
}


int compare_init(XCompare *ptr){

	XCompare_Config *cfgPtr;
    int status;

    cfgPtr = XCompare_LookupConfig(XPAR_COMPARE_DEVICE_ID);
    if (!cfgPtr) {
        print("ERROR: Lookup failed.\n\r");
        return XST_FAILURE;
    }

    status = XCompare_CfgInitialize(ptr, cfgPtr);
    if (status != XST_SUCCESS) {
        print("ERROR: Could not initialize.\n\r");
        return XST_FAILURE;
    }

    return status;
}

void runAlgorithm();

int main()
{
    init_platform();

    int status;

    status = ls_init(&ls);
    status = compare_init(&compare);

    runAlgorithm();

    cleanup_platform();
    return 0;
}

void runAlgorithm(){

     double probs[A] = {0.1, 0.3, 0.5, 0.7} ;
    // double probs[A] = {0.65, 0.75, 0.85, 0.95} ;
	// double probs[A] = {0.0, 0.2, 0.9, 0.8} ;
	// double probs[A] = {0.4, 0.5, 0.3, 0.7} ;
	// double probs[A] = {0.4, 0.0, 0.0, 0.7} ;
	// double probs[A] = {0.1, 0.3, 0.5, 0.7} ;
	// double probs[A] = {0.51, 0.52, 0.53, 0.54} ;
	// double probs[A] = {0.11, 0.21, 0.31, 0.41} ;
	// double probs[A] = {0.38, 0.24, 0.77, 0.65} ;
	// double probs[A] = {0.7, 0.4, 0.2, 0.1} ;

    unsigned int n = 0 ;

    unsigned int X1 = 1, X2 = 1, X3 = 1, X4 = 1, T1 = 1, T2 = 1, T3 = 1, T4 = 1 ;

    unsigned int subset[S] = {3,5,6,7,9,10,11,12,13,14,15} ;
    unsigned int K = 2;

    unsigned int beta, b0, b1, b2, b3, bv0, bv1, bv2, bv3 ;
	unsigned int selected, arms, vacant, occupied ;
	unsigned int tau = 0 ;
    double r = 0;

    uint32_t inform = 0 ;

	XLite_to_stream_Set_inform_V(&ls,inform) ;

	XTime tprocessorStart, tprocessorEnd ;
	XTime_GetTime(&tprocessorStart);

    for(int i=1; i<=10000 ; i++){

    	if(i<=11)
    		selected = i;
    	else
     		selected = XCompare_Get_maxQ(&compare);

    	selected-- ;

    	r = (double) rand()/ (double) RAND_MAX;

    	arms = 0 ;
    	vacant = 0 ;

    	beta = subset[selected] ;

		b0 = beta&1 ;
		b1 = (beta>>1)&1 ;
		b2 = (beta>>2)&1 ;
		b3 = (beta>>3)&1 ;

		bv0 = b0 && (r < probs[0]) ;
		bv1 = b1 && (r < probs[1]) ;
		bv2 = b2 && (r < probs[2]) ;
		bv3 = b3 && (r < probs[3]) ;

		arms = b0 + b1 + b2 + b3 ;
		vacant = bv0 + bv1 + bv2 + bv3 ;

    	occupied = arms - vacant ;

        if(occupied <= K)
        {
        	X1 += bv0 ;
        	X2 += bv1 ;
        	X3 += bv2 ;
        	X4 += bv3 ;

        	T1 += b0 ;
        	T2 += b1 ;
        	T3 += b2 ;
        	T4 += b3 ;
        }
        else
        	vacant = 0 ;

        tau += vacant ;

		inform = 1 ;

		inform = (((((((((inform<<4)|beta)<<1)|bv3)<<1)|bv2)<<1)|bv1)<<1)|bv0 ;

		XLite_to_stream_Set_inform_V(&ls,inform) ;

    	n += 1 ;
    }

    XTime_GetTime(&tprocessorEnd);

    printf("X1 = %u		T1 = %u\n", X1, T1) ;
    printf("X2 = %u		T2 = %u\n", X2, T2) ;
    printf("X3 = %u		T3 = %u\n", X3, T3) ;
    printf("X4 = %u		T4 = %u\n", X4, T4) ;

    printf("Learned Probabilities:\n") ;

	printf("P1 = %f\r\n", (float)X1/T1) ;
	printf("P2 = %f\r\n", (float)X2/T2) ;
	printf("P3 = %f\r\n", (float)X3/T3) ;
	printf("P4 = %f\r\n", (float)X4/T4) ;

    printf("N = %u \r\n", n) ;

    printf("Total Reward: %u", tau) ;

    printf("\nPS took %.2f us.\n\n", 1.0 * (tprocessorEnd - tprocessorStart) / (COUNTS_PER_SECOND/1000000));

}
