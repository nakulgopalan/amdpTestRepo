//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose

# include <vector.h>

template <class T> void treeSort(vector<T> & v)
{
    // declare an search tree of the correct type
    avlTree<T> sorter;

    // copy the entire vector into the tree
    vectorIterator<T> vitr(v);

    for (vitr.init(); ! vitr; ++vitr)
        sorter.add(vitr());

    // now copy the values back into the array
    int i = 0;
    searchTreeIterator<T> itr(sorter);

    for (itr.init(); ! itr; ++itr)
        v[i++] = itr();
}
