#include <math.h>
#include <hls_math.h>

void compare(float Q1, float Q2, float Q3, float Q4,
		float Q5, float Q6, float Q7, float Q8,
		float Q9, float Q10, float Q11,
		unsigned int *maxQ);


int main(int argc, char **argv)
{

	unsigned int maxQ ;

//	float Q[] = {0.3, 0.9, 0.99, 0.33, 0.67, 0.33, 0.88, 0.9, 0.12, 0.4, 0.77};
//	compare(Q[0], Q[1], Q[2], Q[3],
//			Q[4], Q[5], Q[6], Q[7],
//			Q[8], Q[9], Q[10], &maxQ);
//	printf("Result: %d \r\n", maxQ);

//	float Q[] = {0.3, 0.9, 0.93, 0.33, 0.67, 0.33, 0.88, 0.9, 0.12, 0.9, 0.77};
//	compare(Q[0], Q[1], Q[2], Q[3],
//			Q[4], Q[5], Q[6], Q[7],
//			Q[8], Q[9], Q[10], &maxQ);
//	printf("Result: %d \r\n", maxQ);

	float Q[] = {0.3, 1.9, 1.93, 1.89, 0.1, 1.9315, 0.88, 1.9387, 0.9, 0.12, 1.91} ;
	compare(Q[0], Q[1], Q[2], Q[3],
			Q[4], Q[5], Q[6], Q[7],
			Q[8], Q[9], Q[10], &maxQ);
	printf("Result: %u\r\n", (unsigned int)maxQ);

	return 0;
}
