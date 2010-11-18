/***************************************************************************
                          Trie.hpp  -  description
                             -------------------
    begin                : cs dec 26 2002
    copyright            : (C) 2002 by Ferenc Bodon
    email                : bodon@cs.bme.hu
 ***************************************************************************/

#ifndef Trie_HPP
#define Trie_HPP

/**
  *@author Ferenc Bodon
  */

#include "common.hpp"
#include <vector>
#include <set>

using namespace std;

class Apriori_Trie;
class Trie;

/** This struct represent an edge of a Trie.

An edge has a label, and an edge points to a subtrie.
*/
struct Edge
{
   itemtype label;
   Trie* subtrie;
};

/** This class represent a general Trie.

   We can regard the trie as a recursive data structure. It has a root 
   node and a list of (sub)trie. We can reach a subtree by a 
   labeled edge (link). Since the root of the trie represents an itemset 
   the counter stands for the occurrence. For the sake of fast traversal 
   we also store the length of the maximal path starting from the root, 
   and the edges are stored ordered according to their label.
*/

class Trie
{
friend class Apriori_Trie;

public:
  /**
     \param init_counter The initial counter of the new trie
   */
   Trie( const countertype init_counter ):counter(init_counter){}

   /// It decides whether the given itemset is included in the trie or not.
   const Trie* is_included( const set<itemtype>& an_itemset, 
			    set<itemtype>::const_iterator item_it ) const;

   /// Increases the counter for those itemsets that is 
   /// contained by the given basket.
   void find_candidate( vector<itemtype>::const_iterator it_basket_upper_bound,
			   vector<itemtype>::const_iterator it_basket, 
			const countertype counter_incr );

   /// Deletes the tries that represent infrequent itemsets.
   void delete_infrequent( const double min_occurrence );

   ~Trie();

private:

   /// Adds an empty state to the trie
   void add_empty_state( const itemtype item, 
			 const countertype init_counter=0 );

public:
   // No public members

private:

   /// counter stores the occurrence of the itemset represented by the Trie
   countertype counter;

   /** edgevector stores the edges of the root the trie.
     *
     * edgevector is always sorted!
     */
   vector<Edge> edgevector;
   
};


#endif
