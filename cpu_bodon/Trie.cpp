/***************************************************************************
                          Trie.cpp  -  description
                             -------------------
    begin                : cs dec 26 2002
    copyright            : (C) 2002 by Ferenc Bodon
    email                : bodon@cs.bme.hu
 ***************************************************************************/


#include "Trie.hpp"
#include <cstdlib>
#include <iostream>
#include <algorithm>

bool Edge_point_less(const Edge edge, const itemtype label)
{
   return edge.label < label;
}

/**
  \param an_itemset The given itemset.
  \param item_it This iterator shows the element of the basket 
                 that has to be processed.
  \return NULL, if the itemset is not included, otherwise the trie, 
                that represents the itemset.
*/

const Trie* Trie::is_included( const set<itemtype>& an_itemset, 
			       set<itemtype>::const_iterator item_it ) const
{
   if( item_it == an_itemset.end() ) return this;
   else
   {
      vector<Edge>::const_iterator it_edgevector = 
	 lower_bound(edgevector.begin(), edgevector.end(), 
		     *item_it, Edge_point_less);
      if( it_edgevector != edgevector.end() && 
	  (*it_edgevector).label == *item_it )
         return (*it_edgevector).subtrie->is_included( an_itemset, ++item_it );
      else return NULL;
   }
}

/**
  \param basket the given basket
  \param it_basket *it_basket lead to the actual Trie. 
                    Only items following this item in the basket 
                    need to be considered
  \param counter_incr The number times this basket occurs
*/
void Trie::find_candidate( 
   vector<itemtype>::const_iterator it_basket_upper_bound,
   vector<itemtype>::const_iterator it_basket, 
   const countertype counter_incr )
{
   if( edgevector.empty() ) 
      counter += counter_incr;
   else
   {
   	
         vector<Edge>::iterator it_edge = edgevector.begin();
         while( it_edge != edgevector.end() && 
		it_basket != it_basket_upper_bound )
         {
            if( (*it_edge).label < *it_basket) ++it_edge;
            else if( (*it_edge).label > *it_basket) ++it_basket;
            else
            {
               (*it_edge).subtrie->find_candidate( it_basket_upper_bound + 1, 
						   it_basket + 1, 
						   counter_incr);
               ++it_edge;
               ++it_basket;
            }
	 }
   }
}

/**
  \param min_occurrence The occurence threshold
*/
void Trie::delete_infrequent( const double min_occurrence )
{
   vector<Edge>::iterator itEdge = edgevector.begin();
   while(itEdge != edgevector.end() )
   {
      if( (*itEdge).subtrie->edgevector.empty() )
      {
	 if( (*itEdge).subtrie->counter < min_occurrence )
	 {
	    delete (*itEdge).subtrie;
	    itEdge = edgevector.erase(itEdge);
	 }
	 else ++itEdge;
      }
      else
      {
	 (*itEdge).subtrie->delete_infrequent( min_occurrence );

	 if( (*itEdge).subtrie->edgevector.empty() )
	 {
	    delete (*itEdge).subtrie;
	    itEdge = edgevector.erase(itEdge);
	 }
	 else ++itEdge;
      }
   }
}


Trie::~Trie()
{
   for( vector<Edge>::iterator itEdge = edgevector.begin();
	itEdge != edgevector.end(); ++itEdge )
      delete (*itEdge).subtrie;
}


/**
  \param item The label of the new edge
  \param counter The initial counter of the new state
 */
void Trie::add_empty_state( const itemtype item, const countertype counter )
{
   Edge temp_edge;
   temp_edge.label = item;
   temp_edge.subtrie = new Trie( counter );
   edgevector.push_back(temp_edge);
}
