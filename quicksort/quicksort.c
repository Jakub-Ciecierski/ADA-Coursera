#include <stdio.h>
#include <stdlib.h>

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


#define LINE_SIZE 256
#define ARRAY_SIZE 10000

#define DEBUG_MODE 0

/*********************/
/**** DECLERATIONS ***/
/*********************/

int* read_array(char* filename);

void quicksort(int* A, int l, int r, int (*choose_pivot)(int*, int ,int), int* comparison_count);
int partition(int* A, int l, int r, int p);

int choose_pivot_first(int* A, int l, int r);
int choose_pivot_last(int* A, int l, int r);
int choose_pivot_median_of_three(int* A, int l, int r);

void swap(int* a, int* b);

/*********************/
/***** ALGORITHM *****/
/*********************/

/**
 * Input: 
 * 		- Array A.
 * 		- l, r are the indices indicating the 
 * 		left and right bound of A respectively.
 * 		- Function pointer to choose_pivot function.
 * 
 * Ouput: 
 * 		- Sortet array A
 * 
 * Algorithm Description:
 * 
 * 		1) Base Case: if n == 1 then return
 * 		2) Choose pivot p
 * 		3) Partition A around p into two parts
 * 		4) recursive call for first part
 * 		5) recursive call for second part
 * 
 * */
void quicksort(int* A, int l, int r, int (*choose_pivot)(int*, int ,int), int* comparison_count)
{
	// The pivot index
	int p;
	// The pivot index after partition
	int i;
	// Array length
	int n;
	int j;

	n = (r - l + 1);
	
	*comparison_count += n > 0 ? n - 1 : 0;
	
	if(DEBUG_MODE) fprintf(stderr, "\n************************************\n");
	if(DEBUG_MODE) fprintf(stderr, "\nQuickSort(), Array size: %d\n", n);
	
	/// 1) BASE CASE
	if(n <= 1) return;
	
	if(DEBUG_MODE)
	{
		fprintf(stderr, "Before choose_pivot: l = %d, r = %d\n", l, r);
		for(j = l; j <= r; j++)
			fprintf(stderr, "%d, ", A[j]);
		fprintf(stderr, "\n");
	}
	 	
	/// 2) CHOOSE PIVOT
	p = (*choose_pivot)(A, l, r);
	
	/// 3) PARTITION
	i = partition(A, l, r, p);
	
	if(DEBUG_MODE)
	{
		fprintf(stderr, "After choose_pivot = %d\n", p);
		for(j = l; j <= r; j++)
			fprintf(stderr, "%d, ", A[j]);
		fprintf(stderr, "\n");
	}
	
	if(DEBUG_MODE) fprintf(stderr, "After parition(), i = %d\n", i);
	
	/// 4), 5) RECURSION
	quicksort(A, l, i-1, choose_pivot, comparison_count);
	quicksort(A, i+1, r, choose_pivot, comparison_count);

	return;
}

/**
 * Input: 
 * 		- Array A,
 * 		- Indices l and r, the left bound 
 * 		and right bound of A respectively,
 * 		- Pivot index p.
 * Output:
 * 		- Paritioned Array around p such that all 
 * 		elements on the left of p are smaller than p
 * 		and elements on the right of p are greater than p.
 * 		- Index of pivot A[p] in newly partitioned array
 * Note:
 * 		After partition, pivot p is in sorted position.
 * */
int partition(int* A, int l, int r, int p)
{
	int i, j;

	// Pivot must be first element
	if(A[p] != A[l]) {
		if(DEBUG_MODE) fprintf(stderr, "Before swap: A[p] = %d, A[%d] = %d\n", A[p], l, A[l]);
		swap(&(A[p]), &(A[l]));
		if(DEBUG_MODE) fprintf(stderr, "After swap: A[p] = %d, A[%d] = %d\n", A[p], l, A[l]);
	}
	
	// First non pivot element
	i = l + 1;
	for(j = l + 1; j <= r; j++)
	{
		if(A[j] <= A[l])
		{
			swap(&(A[j]), &(A[i]));
			i++;
		}
	}
	swap(&(A[l]), &(A[i - 1]));
	
	return i - 1;
}

/**
 * Chooses the first element of A as a pivot
 * */
int choose_pivot_first(int* A, int l, int r)
{
	int p;
	
	p = l;
	
	return p;
}

/**
 * Chooses the last element of A as a pivot
 * */
int choose_pivot_last(int* A, int l, int r)
{
	int p;
	
	p = r;
	
	return p;
}

/**
 * Considers first, middle and last elements.
 * Chooses the median of these three as pivot
 * */
int choose_pivot_median_of_three(int* A, int l, int r)
{
	int mid_index;
	int first, mid, last;
	int p;
	int n;

	n = (r - l + 1);
	
	// odd
	if(n%2 != 0)
		mid_index = n/2;
	else
		mid_index = n/2 - 1;
	mid_index += l;
	
	first = A[l];
	mid = A[mid_index];
	last = A[r];

	if((first > mid && first < last) ||
		(first < mid && first > last))
		p = l;
	else if((mid > first && mid < last) ||
			(mid < first && mid > last))
		p = mid_index;
	else
		p = r;

	if(DEBUG_MODE) fprintf(stderr, "first: %d, mid: %d, mid_index: %d, last: %d. p = %d\n",
								first, mid, mid_index, last, A[p]);
	
	return p;
}

/**
 * Swaps two integers
 * */
void swap(int* a, int* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
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
		
		if(DEBUG_MODE) 
			fprintf(stderr, "Read %d-th element = %d, from line: %s\n", i, A[i], line);
		
		i++;
	}
	
	return A;
}

/*********************/
/**** TEST CASES *****/
/*********************/

void test_case_file(int (*choose_pivot)(int*, int ,int))
{
	int* A;
	float hostTime;
    double startTime, stopTime;
    int comparison_count;
    
	fprintf(stdout, "Running Test Case from File\n");

	A = read_array("QuickSort.txt");
	
	startTime = second();
	
	comparison_count = 0;
	quicksort(A, 0, ARRAY_SIZE - 1, choose_pivot, &comparison_count);
	
	stopTime = second();
    hostTime = (stopTime - startTime) * 1000;

	fprintf(stdout, "Computation Time: %lf ms\n", hostTime);
	
	fprintf(stderr, "\nComparison count: %d\n", comparison_count);

	free(A);
}

void test_case_small(int (*choose_pivot)(int*, int ,int))
{
	int i;
	int comparison_count;
	
	int A[6] = {6,5,4,3,2,1};
	
	fprintf(stdout, "Running Small Case\n");
	
	comparison_count = 0;
	quicksort(A, 0, 6 - 1, choose_pivot, &comparison_count);
	
	for(i = 0; i < 6; i++)
	{
		fprintf(stderr, "%d, ", A[i]);
	}
	fprintf(stderr, "\nComparison count: %d\n", comparison_count);
}

void test_case_medium(int (*choose_pivot)(int*, int ,int))
{
	int i;
	int comparison_count;

	int A[15] = {6, 1, 9, 2, 12, 156, 22, 66, 11, 6, 5, 4, 3, 2, 1};
	
	fprintf(stdout, "Running Small Case\n");

	comparison_count = 0;
	quicksort(A, 0, 15 - 1, choose_pivot, &comparison_count);
	
	for(i = 0; i < 15; i++)
	{
		fprintf(stderr, "%d, ", A[i]);
	}
	fprintf(stderr, "\nComparison count: %d\n", comparison_count);
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
			fprintf(stdout, "\n***************************\n");
			fprintf(stdout, "choose_pivot_first\n");
			test_case_file(choose_pivot_first);
			
			fprintf(stdout, "\n***************************\n");
			fprintf(stdout, "choose_pivot_last\n");
			test_case_file(choose_pivot_last);
			
			fprintf(stdout, "\n***************************\n");
			fprintf(stdout, "choose_pivot_median_of_three\n");
			test_case_file(choose_pivot_median_of_three);
			break;
		case 1:
			fprintf(stdout, "\n\n\n***************************\n\n\n");
			fprintf(stdout, "choose_pivot_first\n");
			test_case_small(choose_pivot_first);
			
			fprintf(stdout, "\n\n\n***************************\n\n\n");
			fprintf(stdout, "choose_pivot_last\n");
			test_case_small(choose_pivot_last);
			
			fprintf(stdout, "\n\n\n***************************\n\n\n");
			fprintf(stdout, "choose_pivot_median_of_three\n");
			test_case_small(choose_pivot_median_of_three);
			break;
		case 2:
			fprintf(stdout, "\n\n\n***************************\n\n\n");
			fprintf(stdout, "choose_pivot_first\n");
			test_case_medium(choose_pivot_first);
			
			fprintf(stdout, "\n\n\n***************************\n\n\n");
			fprintf(stdout, "choose_pivot_last\n");
			test_case_medium(choose_pivot_last);
			
			fprintf(stdout, "\n\n\n***************************\n\n\n");
			fprintf(stdout, "choose_pivot_median_of_three\n");
			test_case_medium(choose_pivot_median_of_three);
			break;
		default:
			break;
	}

	return EXIT_SUCCESS;
}
