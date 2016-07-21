template <class T> void bucketSort(vector<T> & data,
    unsigned int tablesize, unsigned int (*f)(const T &))
{
// 	CFRONT error ??
    unsigned int max = data.length();
    hashTree<T> table(tablesize, f);

    // put each element into the appropriate bucket
    for (unsigned int i = 0; i < max; i++)
        table.add(x[i]);

    // now put elements back into vector
    hashTreeIterator<T> itr(table);
    int j = 0;

    for (itr.init(); ! itr; itr++)
        x[j++] = itr();
}


// bucket sort (version #2)

template <class T> void bucketSort(vector<T> & data,
    unsigned int (*f)(const T &))
{
    const int tablesize = 31;
    unsigned int max = data.length();
    orderedList<T>  buckets[tablesize];

    // put each element into the appropriate bucket
    for (unsigned int i = 0; i < max; i++)
        buckets[f(data[i])].add(data[i]);

    // now put elements back into vector
    int j = 0;

    for (int i = 0; i < tablesize; i++)
    {
        // empty the elements from the bucket
        listIterator<T> itr(buckets[i]);

        for (itr.init(); ! itr; itr++)
            data[j++] = itr();
    }
}


