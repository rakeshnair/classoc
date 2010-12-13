#include"Trie.h"

void
Trie::add(const vector<unsigned>& items, const unsigned index)
{
   assert(items.size() > 1);
   const unsigned rIndex = items[0];
   TrieNode* node = root[rIndex]; 
   if (node == 0) {
      node = new TrieNode();
      root[rIndex] = node;
   } 
      
   for (unsigned i = 1; i < items.size(); ++i) {
      assert(node);
      NodeMap& nMap = node->data;
      NodeMap::iterator itr = nMap.find(items[i]);
      if (itr == nMap.end()) {
         Val v;
         v.index = index;
         nMap[items[i]] = v;
         assert(i == items.size() - 1);
         return;
      } else {
         node = (*itr).second.child; 
         if (node == 0) {
             node = new TrieNode();
             (*itr).second.child = node;
         }
      } 
   }
   assert(0);
}

void
Trie::getSubSets(const vector<unsigned> &set, vector<vector<unsigned> >& subsets)
{

    const unsigned size = set.size() - 1;
    int max = 1 << set.size();
    for(int i = 0; i < max; i++) {
        vector<unsigned> subset;
        int j = i;
        int index = 0;
        while (j > 0) {
            if((j & 1) > 0)
                subset.push_back(set[index]);
            j >>= 1;
            index++;
        }
        if (subset.size() == size)
            subsets.push_back(subset);
    }
}

bool
Trie::checkSupportFromTrie(const vector<unsigned>& set, const bool* count)
{
    assert(set.size() > 1 );
    const TrieNode* node = getRootNode(set[0]);
    if (!node) return false;
    for (unsigned i = 1; i < set.size(); ++i) {
        const NodeMap& nMap = node->data;
        NodeMap::const_iterator itr = nMap.find(set[i]);
        if (itr == nMap.end()) return false;
        if (i == (set.size() - 1)) {
            if (count[set[i]] == 0)  return false; 
            return true;
        }   
        node = (*itr).second.child;
        if (!node) return false;
    }
    assert(0);
}

bool
Trie::checkSupport(const vector<unsigned>& prefix, const bool* count)
{
    vector<vector<unsigned> > subsets;
    getSubSets(prefix, subsets);  
    assert(subsets.size()); 
    for (unsigned i = 0; i < subsets.size(); ++i) 
    {
        if (!checkSupportFromTrie(subsets[i], count))
            return false; 
    }
    return true;
}
