#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <assert.h>
//16*1024*1024
#define		size	   16*1024*1024

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

/* Partition function for quicksort. Takes middle element as pivot, places pivot
 element in correct position, and places all elements smaller than pivot to the
 left of the pivot and all greater elements to right of pivot. **/
int partition(int *array, int start, int end) {
    int pivot, mid, i, j;
    mid = (start + end)/2;
    pivot = array[mid];
    i = start - 1;
    
    for (j = start; j < end; j++) {
        /* Current element is smaller than pivot. */
        if (array[j] < pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    
    swap(&array[i + 1], &array[mid]);
    return i + 1;
}

void quickSort(int *array, int start, int end){
    // you quick sort function goes here
    if (start < end) {
        int p = partition(array, start, end);
        quickSort(array, start, p);
        quickSort(array, p + 1, end);
    }
   
}

int main(void){
	int i, j, tmp;
	struct timespec start, stop; 
	double exe_time;
	srand(time(NULL)); 
	int * m = (int *) malloc (sizeof(int)*size);
	for(i=0; i<size; i++){
		m[i]=size-i;
	}
	
	if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
	////////**********Your code goes here***************//
    quickSort(m, 0, size);
		
			
	///////******************************////
	
	if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
	exe_time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
	
	for(i=0;i<16;i++) printf("%d ", m[i]);
	printf("\nExecution time = %f sec\n",  exe_time);
    /*
    for (i = 0; i < size - 1; i++) {
        assert(m[i] <= m[i + 1]);
    }
    printf("Quicksort works\n");
     */
}	
