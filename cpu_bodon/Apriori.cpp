/***************************************************************************
                          apriori.cpp  -  description
                             -------------------
    begin                : cs dec 26 2002
    copyright            : (C) 2002 by Ferenc Bodon
    email                : bodon@cs.bme.hu
 ***************************************************************************/

#include "Apriori.hpp"
#include <iostream>
#include <vector>
#include <set>
#include <cmath>   //because of the ceil function

using namespace std;

/**
  \param candidate_size The size of the candidate whose support 
                        has top be determined.
*/
void Apriori::support( const itemtype& candidate_size )
{
   set<itemtype> basket;
   vector<itemtype> basket_v;
   if( store_input )
   {
      if (candidate_size == 2)
      {
         while( input_output_manager.read_in_a_line( basket ) )
         {
            input_output_manager.basket_recode( basket, basket_v );
            if (basket_v.size()>1) reduced_baskets[basket_v]++;
         }
      }
      for (map<vector<itemtype>, countertype>::iterator it = 
	      reduced_baskets.begin(); it!=reduced_baskets.end();it++)
         apriori_trie->find_candidate(it->first,candidate_size,it->second);
   }
   else while( input_output_manager.read_in_a_line( basket ) )
   {
      input_output_manager.basket_recode(basket, basket_v);
          apriori_trie->find_candidate(basket_v,candidate_size);
   }
}

/**
  \param min_supp The relative support threshold
  \param min_conf The confidence threshold for association rules. 
                  If min_conf=0 no association rules will be extraced.
  \param quiet If quiet = true then no system messages will be written 
               during the process.
  \param size_threshold Frequent itemsets above this threshold 
                        do not need to be found.
*/
void Apriori::APRIORI_alg( const double min_supp, 
			   const bool quiet, 
			   const unsigned int size_threshold )
{
   countertype basket_number;   
   if(!quiet) cout<<endl<<"\t\tFinding frequent itemsets..."<<endl<<endl;
   itemtype candidate_size=1;
   if(!quiet)
   {
      cout<<endl<<"Determining the support of the items";
      cout<<" and deleting infrequent ones!"<<endl;
   }
   vector<countertype> support_of_items;
   basket_number = input_output_manager.find_frequent_items( 
      min_supp, support_of_items );
   input_output_manager<< "Frequent 0-itemsets:\nitemset (occurrence)\n";
   input_output_manager<< "{} ("<< basket_number << ")\n";
   input_output_manager<< "Frequent " << 1;
   input_output_manager<< "-itemsets:\nitemset (occurrence)\n";
   set<itemtype> temp_set;
   for(vector<countertype>::size_type index = 0;
      index < support_of_items.size(); index++)
   {
      temp_set.insert(index);
      input_output_manager.write_out_basket_and_counter( 
	 temp_set, support_of_items[index] );
      temp_set.erase(temp_set.begin());
   }

   apriori_trie = new Apriori_Trie( basket_number );
   apriori_trie->insert_frequent_items( support_of_items );

//   apriori_trie->show_content();
//   getchar();
   double min_supp_abs = min_supp * (basket_number - 0.5);
   cout << "ABS" << min_supp_abs << endl;
//   apriori_trie->show_content();
//   getchar();
   candidate_size++;
   {
      cout<<endl<<"Genarating "<<candidate_size;
      cout<<"-itemset candidates!"<<endl;
   }
   apriori_trie->candidate_generation(candidate_size-1,
				      input_output_manager);
//   apriori_trie->show_content();
//   getchar();
   while( apriori_trie->is_there_any_candidate() )
   {
      input_output_manager.rewind();
      if(!quiet)
      {
	 cout<<"Determining the support of the "<<candidate_size;
	 cout<<"-itemset candidates!"<<endl;
      }
      support( candidate_size );
//      apriori_trie->show_content();
//      getchar();
      if(!quiet) cout<<"Deleting infrequent itemsets!"<<endl;
      apriori_trie->delete_infrequent(min_supp_abs, candidate_size);
//      apriori_trie->show_content();
//      getchar();
      if (candidate_size == size_threshold )
      {
	 if(!quiet) cout<<"Size threshold is reached!"<<endl;
	 break;
      }
      candidate_size++;
      {
	 cout<<endl<<"Genarating "<<candidate_size;
	 cout<<"-itemset candidates!"<<endl;
      }
      input_output_manager<< "Frequent " << candidate_size-1;
      input_output_manager<< "-itemsets:\nitemset (occurrence)\n";
      apriori_trie->candidate_generation(candidate_size-1,
					 input_output_manager);
//      apriori_trie->show_content_preorder();
//      getchar();
   }
   if(!quiet) cout<<endl<<"Mining is done!"<<endl;
}
