//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose

//----------------------------------------------------------------------
//	function: binarySearch
//----------------------------------------------------------------------

template <class VecType, class EleType>
int binarySearch(VecType data, EleType ele, unsigned int max)
{	// perform binary search on a vector

	unsigned int low = 0;
	unsigned int high = max;

	while (low < high) {

		// inv: data[0..low-1] are all lexx than ele
		// data[high..max] are all greater than ele
		unsigned int mid = (low + high) / 2;
		if (data[mid] < ele)
			low = mid + 1;
		else
			high = mid;
		}

	// return element for which data[0..low-1] < ele <= data[low]
	return low;
}

//----------------------------------------------------------------------
//	function: swap
//----------------------------------------------------------------------

template <class T> void swap( vector<T> & data, int i, int j)
{
    // swap the values at positions i and j
    T temp = data[i];
    data[i] = data[j];
    data[j] = temp;
}


//----------------------------------------------------------------------
//	function: bubbleSort
//----------------------------------------------------------------------

template <class T> void bubbleSort(vector<T> & data)
{
    // sort, in place. the argument vector into ascending order
    for (int top = data.length() - 1; top > 0; top--)
    {
        // find the largest element in position 0..top
        // place in data[top]
        for (int i = 0; i < top; i++)
        {
            // inv: data[i] is largest element in 0..i
            if (data[i+1] < data[i])
                swap(data, i+1, i);

            // inv: data[i+1] is largest element in 0..i+1
        }

        // inv: data[top..length-1] is properly ordered
    }
}



//----------------------------------------------------------------------
//	function: selectionSort
//----------------------------------------------------------------------

template <class T> void selectionSort(vector<T> & data)
{
    // sort, in place. the argument vector into ascending order
    for (int top = data.length() - 1; top > 0; top--)
    {
        // find the position of the largest element
        int largeposition = 0;

        for (int j = 1; j <= top; j++)
        {
            // inv: data[largeposition] is largest element
            // in 0..j-1
            if (data[largeposition] < data[j])
                largeposition = j;

            // inv: data[largeposition] is largest element in 0..j
        }

        if (top != largeposition)
            swap(data, top, largeposition);

        // inv: data[top..n] is ordered
    }
}


//----------------------------------------------------------------------
//	function: insertionSort
//----------------------------------------------------------------------

template <class T> void insertionSort(vector<T> & data)
{
    // sort, in place. the argument vector into ascending order
    int n = data.length();

    for (int top = 1; top < n; top++)
    {
        // inv: data[0..top-1] is ordered
        // goal: place element data[top] in the proper position
        for (int j = top - 1; j >= 0 && data[j+1] < data[j]; j--) {
            swap(data, j, j+1);
		}

        // inv: data[0..top] is ordered
    }

    // inv: data[0..n-1] is ordered
}

//----------------------------------------------------------------------
//	partition and quicksort
//----------------------------------------------------------------------

template <class T>
int partition(vector<T> & v, int low, int high, int pivotIndex)
{
	// move the pivot value to the bottom of the vector
	if (pivotIndex != low) 
		swap(v, low, pivotIndex);
	pivotIndex = low;
	low++;

	// invarient:
	//	v[i] for i less than low are less than or equal to pivot
	//	v[i] for i greater than high are greater than pivot

	// move elements into place
	while (low <= high) {
		if (v[low] <= v[pivotIndex])
			low++;
		else if (v[high] > v[pivotIndex])
			high--;
		else
			swap(v, low, high);
		}

	// put pivot back between two groups
	if (high != pivotIndex)
		swap(v, pivotIndex, high);
	return high;
}

template <class T>
T findElement(vector<T> & v, int N, int low, int high)
{
	// first select a pivot
	int pivotIndex = (high + low)/2;

	// partition the vector
	pivotIndex = partition(v, low, high, pivotIndex);

	// see if we are done
	if (pivotIndex == N)
		return v[N];

	// else try again
	if (N < pivotIndex)
		return findElement(v, N, low, pivotIndex);
	else
		return findElement(v, N, pivotIndex, high);
}

template <class T>
void quackSort(vector<T> & v, int low, int high)
{
	// no need to sort a vector of zero or one elements
	if (low >= high)
		return;

	// select the pivot value
	int pivotIndex = (low + high) / 2;

	// partition the vector
	pivotIndex = partition(v, low, high, pivotIndex);

	// sort the two sub arrays
	if (low < pivotIndex)
		quackSort(v, low, pivotIndex - 1);
	if (pivotIndex < high)
		quackSort(v, pivotIndex + 1, high);
}

template <class T>
void quackSort(vector<T> & v)
{
	quackSort(v, 0, (int) v.length() - 1);
}

