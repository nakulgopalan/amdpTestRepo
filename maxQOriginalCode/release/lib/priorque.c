# include <vector.h>

//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose

template <class T> void buildHeap(
    vector<T> & data, int heapsize, int position)
{
    // rebuild the heap
    T value = data[position];

    while (position < heapsize)
    {
        // replace position with the smaller of the
        // two children, or the last element
        unsigned int childpos = position * 2 + 1;

        if (childpos < heapsize)
        {
            if ((childpos + 1 < heapsize) &&
                data[childpos + 1] < data[childpos])
                childpos += 1;

            // inv: childpos is smaller of two children
            if (value < data[childpos])
            {
                // found right location
                data[position] = value;
                return;
            }
            else
            {
                data[position] = data[childpos];
                position = childpos;
                // recur and keep moving down
            }
        }
        else
        {
            // no children
            data[position] = value;
            return;
        }
    }
}

//----------------------------------------------------------------------
//	function: heapSort
//----------------------------------------------------------------------

template <class T> void heapSort(vector<T> & data)
{
    // sort the vector argument using a heap algorithm
    int max = data.length();

    // first build the initial heap
    int i;
    for (i = max / 2; i >= 0; i--)
        buildHeap(data, max, i);

    // now swap the smallest element to the last position
    for (i = max - 1; i > 0; i--)
    {
        swap(data, i, 0);

        // and rebuild the heap again
        buildHeap(data, i, 0);
    }
}
