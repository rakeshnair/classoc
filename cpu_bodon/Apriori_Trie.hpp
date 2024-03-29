/***************************************************************************
                          Apriori_Trie.hpp  -  description
                             -------------------
    begin                : cs dec 26 2002
    copyright            : (C) 2002 by Ferenc Bodon
    email                : bodon@cs.bme.hu
 ***************************************************************************/

#ifndef Apriori_Trie_HPP
#define Apriori_Trie_HPP

/**
  *@author Ferenc Bodon
  */

#include "Trie.hpp"
#include "Input_Output_Manager.hpp"
#include <fstream>
#include <set>
#include <vector>
#include <cstdio>
using namespace std;


/** Apriori_Trie (or prefix-tree) is a tree-based datastructure.

   Apriori_Trie is a special tree designed to provide efficient methods 
   for the apriori algorithm. It mostly uses a regular trie except 
   when there exist faster solution. For example for storing one and two 
   itemset candidate where a simple vector and array gives better performance.
   Apriori_Trie extends the functions provided by the regular trie with 
   a candidate generation process.

*/
class Apriori_Trie
{
public:

/**
  \param counter_of_emptyset The support of the empty set, 
                             i.e. the number of transactions.
*/
   Apriori_Trie( const countertype counter_of_emptyset ):
      main_trie(counter_of_emptyset){}
   
   /// Insert the frequent items and their counters into the trie;
   void insert_frequent_items(const vector<countertype>& counters );
   
      /** Generates candidates 

      Generates candidates and writes frequent itemset 
      that are obtained in the previous iteration to disk.
      */
   void candidate_generation( const itemtype& frequent_size,
			      Input_Output_Manager& input_output_manager );

   /// Increases the counter of those candidates that are contained 
   /// by the given basket.
   void find_candidate( const vector<itemtype>& basket, 
			const itemtype candidate_size, 
			const countertype counter=1 );

   /// Deletes unfrequent itemsets.
   void delete_infrequent( const double min_occurrence, 
			   const itemtype candidate_size );


   /// Returns true if trie is not empty
   bool is_there_any_candidate() const
   {
      return !main_trie.edgevector.empty();
   }

   ~Apriori_Trie(){}

protected:

   /// Decides if all subset of an itemset is contained in the Apriori_Trie
   bool is_all_subset_frequent( const set<itemtype>& maybe_candidate ) const;

   /// Generates candidate of size two
   void candidate_generation_two();

   /// Generates candidate of size more than two
   void candidate_generation_assist( 
      Trie* trie, 
      set<itemtype>& maybe_candidate, 
      Input_Output_Manager& input_output_manager);

   /// Increases the counter for those itempairs that are in the given basket.
   void find_candidate_two( const vector<itemtype>& basket, 
			    const countertype counter=1 );

   /// Deletes the Tries that represent infrequent itemsets of size 2.
   void delete_infrequent_two( const double min_occurrence );

private:
   // No private methods

public:
   // No public members

protected:
   /// Trie to store the candidates.
   Trie main_trie;

   /**  temp_counter_array stores the occurences of the itempairs
     *
     * We can use a simple array to determine the support of itemset 
     * of size two. This requires less memory than the trie-based supportcount.
     * temp_counter_array[i][j-i] stores the occurence of the itempair (i,j).
     */
   vector< vector<countertype> > temp_counter_array;
};

#endif
