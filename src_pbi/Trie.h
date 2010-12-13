#ifndef TRIE_H
#define TRIE_H
#include<map>
#include<vector>
#include<assert.h>
using namespace std;

class Val;

typedef map<int, Val> NodeMap;
class TrieNode
{
public:
	map<int, Val> data;
};

class Val
{
public:
   Val()
   {
      child = 0;
   }
   ~Val()
   {
       delete child;
   }    
   unsigned index; 
   TrieNode* child;
};


class Trie
{

public:
	Trie(const unsigned nItems)
    {
        level = 1;
        root.resize(nItems); 
        for (unsigned i = 0; i < nItems; ++i) {
            root[i] = 0;
        }
    }    
    ~Trie()
    {
        for (unsigned i = 0; i < root.size(); ++i) {
            delete root[i]; 
        }  
    }
    void add(const vector<unsigned>& items, const unsigned index);
    void incrLevel() { ++level; }
    const TrieNode* getRootNode(const unsigned i)
    {
        return root[i];
    }
    bool checkSupport(const vector<unsigned>& prefix, const bool* count);
    bool checkSupportFromTrie(const vector<unsigned>& set, const bool* count);  
    void getSubSets(const vector<unsigned> &set, vector<vector<unsigned> >& subsets);  

 
private:
    unsigned level;
	vector<TrieNode*> root;


};

void traverse(const NodeMap& nMap,
             vector<unsigned>& prefix,
             vector<vector<unsigned > >&  kItemSet);


#endif
