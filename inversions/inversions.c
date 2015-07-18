#include <stdio.h>
#include <stdlib.h>

#define LINE_SIZE 256
#define ARRAY_SIZE 100000

#if defined(_WIN32)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
static double second(void)
{
    LARGE_INTEGER t;
    static double oofreq;
    static int checkedForHighResTimer;
    static BOOL hasHighResTimer;

    if (!checkedForHighResTimer) {
        hasHighResTimer = QueryPerformanceFrequency(&t);
        oofreq = 1.0 / (double)t.QuadPart;
        checkedForHighResTimer = 1;
    }
    if (hasHighResTimer) {
        QueryPerformanceCounter(&t);
        return (double)t.QuadPart * oofreq;
    }
    else {
        return (double)GetTickCount() / 1000.0;
    }
}
#elif defined(__linux__) || defined(__APPLE__)
#include <stddef.h>
#include <sys/time.h>
static double second(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}
#else
#error unsupported platform
#endif

/*********************/
/***** ALGORITHM *****/
/*********************/

int count_inversions(int* A, int n);
int* merge_sort(int* A, int n, unsigned long* inversions_count);
int* merge(int* A, int size_a, int* B, int size_b, unsigned long* inversions_count);

/**
 * 
 * INVERSIONS COUNT ALGORITHMS
 * 
 * 1) Sort array A with MergeSort:
 * 		1.0) Base case
 * 		1.1) Split array A into two halfs: B and C.				// O(n)
 * 		1.2) Recursively sort each array B and C independendly.	// O(1)
 * 		1.3) Merge the solutions and count Inversions			// O(n)
 * 	
 * Running Time - Worst Case:
 * 		1.1)	O(n)
 * 		1.2)	O(1)
 * 		1.3) 	O(n)
 * 		
 * This rutine is ran log(n) times.
 * Thus Total Running Time = O(n*log(n))
 * 
 * */
int count_inversions(int* A, int n)
{
	fprintf(stderr, "Starting Count Inversions \n");
	
	long inversions_count;
	int i;

	fprintf(stderr, "Input Array A: \n");
	for(i = 0; i < n; i++)
	{
		fprintf(stderr, "A[%d] = %d, ", i, A[i]);
	}
	fprintf(stderr, "\n");
	
	inversions_count = 0;
	
	merge_sort(A, n, &inversions_count);

	return inversions_count;
}

int* merge_sort(int* A, int n, unsigned long* inversions_count)
{
	int i;
	int* B;
	int* C;
	int* MergedArray;
	int size_b, size_c;
	int divider;
	
	/// 1.0) BASE CASE
	if(n == 1){
		// array of size 1 is considered sorted;
		fprintf(stderr, "Array of size 1\n");
		return A;
	}

	/// 1.1) SPLIT
	
	// if n is odd, then make size_c bigger
	divider = n%2;
	
	size_b = n/2;
	size_c = n/2 + divider;
	
	fprintf(stderr, "\nSize_B = %d, Size_c = %d\n", size_b, size_c);
	
	// prepare two subarrays
	B = (int*)malloc(size_b * sizeof(int));
	if(B == NULL){
		fprintf(stderr, "Not enough memory\n");
		exit(EXIT_FAILURE);
	}
	
	C = (int*)malloc(size_c * sizeof(int));
	if(C == NULL){
		fprintf(stderr, "Not enough memory\n");
		exit(EXIT_FAILURE);
	}
	
	fprintf(stderr, "\n\n**** DIVIDING ****\n\n");

	// Print all arrays
	for(i = 0; i < n; i++)
	{
		fprintf(stderr, "A[%d] = %d, ", i, A[i]);
	}
	fprintf(stderr, "\nInto: \n\n");

	for(i = 0; i < size_b; i++)
	{
		B[i] = A[i];
		fprintf(stderr, "B[%d] = %d, ", i, B[i]);
	}
	fprintf(stderr, "\n");
	for(i = size_b; i < n; i++)
	{
		int index = i-size_b;
		C[index] = A[i];
		fprintf(stderr, "C[%d] = %d, ", index, C[index]);
	}
	fprintf(stderr, "\n");
	
	/// 1.2) RECURSION
	fprintf(stderr, "\n\n**** Start of Left Side merge_sort ****\n\n");
	B = merge_sort(B, size_b, inversions_count);
	fprintf(stderr, "\n\n**** Start of Right Side merge_sort ****\n\n");
	C = merge_sort(C, size_c, inversions_count);
	
	/// 1.3) MERGE
	MergedArray = merge(B, size_b, C, size_c, inversions_count);
	
	free(B);
	free(C);
	
	return MergedArray;
}

/**
 * A and B are sorted arrays
 * */
int* merge(int* A, int size_a, int* B, int size_b, unsigned long* inversions_count)
{	
	int i, j, k;
	int* C;
	int size_c;

	// print both subarrays
	fprintf(stderr, "\n\n**** Merge ****\n\n");

	for(i = 0; i < size_a; i++)
	{
		fprintf(stderr, "A[%d] = %d, ", i, A[i]);
	}
	fprintf(stderr, "\n");
	for(i = 0; i < size_b; i++)
	{
		fprintf(stderr, "B[%d] = %d, ", i, B[i]);
	}
	fprintf(stderr, "\n");

	size_c = size_a + size_b;
	
	C = (int*)malloc(size_c*sizeof(int));
	
	i = 0;
	j = 0;

	// fill entire array C
	for(k = 0; k < size_c; k++)
	{
		// A has been already filled
		if(i >= size_a && j < size_b)
			C[k] = B[j++];
		// B has been already filled
		else if(j >= size_b && i < size_a)
			C[k] = A[i++];

		else if(A[i] < B[j]){
			C[k] = A[i++];
		}
		else{
			C[k] = B[j++];
			(*inversions_count) += size_a - i;
		}
		
		fprintf(stderr, "C[%d] = %d, ", k, C[k]);
	}
	fprintf(stderr, "\n");
	
	return C;
}

/*********************/
/******* UTIL ********/
/*********************/

int* read_array(char* filename)
{
	char line[LINE_SIZE];
	int i;
	int* A;
	FILE* f;
	
	A = (int*)malloc(ARRAY_SIZE * sizeof(int));
	if(A == NULL){
		fprintf(stderr, "Not enough memory\n");
		exit(EXIT_FAILURE);
	}
	
	if((f = fopen(filename, "r+")) == NULL) {
		fprintf(stderr, "Can't open file\n");
		exit(EXIT_FAILURE);
	}
	
	i = 0;
	while(fgets(line, LINE_SIZE, f) != NULL)
	{
		if(i > ARRAY_SIZE - 1){
			fprintf(stderr, "Something wrong: Array should have size: %d but read: %d lines\n", ARRAY_SIZE, i);
			exit(EXIT_FAILURE);
		}
		
		sscanf(line, "%d", &(A[i]));
		//A[i] = atoi(line);
		
		fprintf(stderr, "Read %d-th element = %d, from line: %s\n", i, A[i], line);
		
		i++;
	}
	
	return A;
}

/*********************/
/**** TEST CASES *****/
/*********************/

void test_case_file()
{
	unsigned long inversion_count;
	int* A;
	float hostTime;
    double startTime, stopTime, elapsed;
    
	fprintf(stdout, "Running Test Case from File\n");

	A = read_array("IntegerArray.txt");
	
	startTime = second();
	
	inversion_count = count_inversions(A, ARRAY_SIZE);
	
	stopTime = second();
    hostTime = (stopTime - startTime) * 1000;

	fprintf(stdout, "Computation Time: %lf\n", hostTime);
	
	fprintf(stdout, "Total number of inversions: %lu\n", inversion_count);
	
	free(A);
}

void test_case_small()
{
	unsigned long inversion_count;

	int A[6] = {6,5,4,3,2,1};
	
	fprintf(stdout, "Running Small Case\n");
	
	inversion_count = count_inversions(A, 6);
	
	fprintf(stdout, "Total number of inversions: %lu\n", inversion_count);
}

void test_case_medium()
{
	unsigned long inversion_count;

	int A[15] = {6, 1, 9, 2, 12, 156, 22, 66, 11, 6, 5, 4, 3, 2, 1};
	
	fprintf(stdout, "Running Small Case\n");
	
	inversion_count = count_inversions(A, 15);
	
	fprintf(stdout, "Total number of inversions: %lu\n", inversion_count);
}

void usage(char* app_name)
{
	fprintf(stderr, "USAGE:\n");
	fprintf(stderr, "%s: test_case \n", app_name);
	fprintf(stderr, "test_case is optional \n");
}

int main(int argc, char** argv)
{
	int test_case;
	
	if(argc == 1)
		test_case = 0;
	else{
		test_case = atoi(argv[1]);
		if(test_case < 0){
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	
	switch(test_case)
	{
		case 0:
			test_case_file();
			break;
		case 1:
			test_case_small();
			break;
		case 2:
			test_case_medium();
			break;
		default:
			break;
	}

	return EXIT_SUCCESS;
}
