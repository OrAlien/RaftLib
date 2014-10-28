/**
 * readcontainer.tcc - 
 * @author: Jonathan Beard
 * @version: Sun Oct 26 15:51:46 2014
 * 
 * Copyright 2014 Jonathan Beard
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _READCONTAINER_TCC_
#define _READCONTAINER_TCC_  1
#include <iterator>
#include <string>
#include <functional>
#include <map>

#include <list>
#include <vector>
#include <array>
#include <deque>
#include <forward_list>

#include <typeinfo>

#include <raft>

template < class T, std::size_t N = 1 > class read_container : 
   public parallel_k
{

typedef typename std::list< T >::iterator           it_list;
typedef typename std::vector< T >::iterator         it_vect;
typedef typename std::array< T, N >::iterator          it_arr;
typedef typename std::deque< T >::iterator          it_deq;
typedef typename std::forward_list< T >::iterator   it_forlist;

template< class iterator_type > 
   static bool  inc_helper( iterator_type &begin, iterator_type &end, Port &port_list )
   {
      for( auto &port : port_list )
      {
         if( begin == end )
         {
            return( false );
         }
         port.push( (*begin) );
         std::cout << "Sending: " << (*begin) << "\n";
         begin++;
      }
      return( true );
   }

const std::map< std::size_t,
         std::function< bool ( void*, void*, Port& ) > > func_map
            =  {
                  { 
                     typeid( it_list ).hash_code(),
                     [ ]( void *b_ptr, void *e_ptr, Port &port_list )
                     {
                        it_list *begin( reinterpret_cast< it_list* >( b_ptr ) );
                        it_list *end  ( reinterpret_cast< it_list* >( e_ptr ) );
                        return( inc_helper( *begin, *end, port_list ) );
                     }
                  },
                  { 
                     typeid( it_vect ).hash_code(),
                     [ ]( void *b_ptr, void *e_ptr, Port &port_list )
                     {
                        it_vect *begin( reinterpret_cast< it_vect* >( b_ptr ) );
                        it_vect *end  ( reinterpret_cast< it_vect* >( e_ptr ) );
                        return( inc_helper( *begin, *end, port_list ) );
                     }
                  },
                  { 
                     typeid( it_arr ).hash_code(),
                     [ ]( void *b_ptr, void *e_ptr, Port &port_list )
                     {
                        it_arr *begin( reinterpret_cast< it_arr* >( b_ptr ) );
                        it_arr *end  ( reinterpret_cast< it_arr* >( e_ptr ) );
                        return( inc_helper( *begin, *end, port_list ) );
                     }
                  },
                  { 
                     typeid( it_deq ).hash_code(),
                     [ ]( void *b_ptr, void *e_ptr, Port &port_list )
                     {
                        it_deq *begin( reinterpret_cast< it_deq* >( b_ptr ) );
                        it_deq *end  ( reinterpret_cast< it_deq* >( e_ptr ) );
                        return( inc_helper( *begin, *end, port_list ) );
                     }
                  },
                  { 
                     typeid( it_forlist ).hash_code(),
                     [ ]( void *b_ptr, void *e_ptr, Port &port_list )
                     {
                        it_forlist *begin( reinterpret_cast< it_forlist* >( b_ptr ) );
                        it_forlist *end  ( reinterpret_cast< it_forlist* >( e_ptr ) );
                        return( inc_helper( *begin, *end, port_list ) );
                     }
                  }
               };

public:
   template < class iterator_type > 
   read_container( iterator_type begin, iterator_type end ) : it_begin_ptr((void*)&begin),
                                                              it_end_ptr((void*)&end)
   {     
      output.addPort< T >( std::to_string( port_name_index++ ) );

      /** 
       * hacky way of getting the right iterator type for the ptr
       * pehaps change if I can figure out how to do without having
       * to move the constructor template to the class template 
       * param
       */
       auto ret_val( func_map.find( typeid( iterator_type ).hash_code() ) );
       if( ret_val != func_map.end() )
       {
          inc_func = (*ret_val).second; 
       }
       else
       {
          /** TODO, make exception for this **/
          assert( false );
       }
   }

   virtual raft::kstatus run()
   {
      if( inc_func( it_begin_ptr, it_end_ptr, output ) )
      {
         return( raft::stop );
      }
      return( raft::proceed );
   }

protected:
   virtual void addPort()
   {
      output.addPort< T >( std::to_string( port_name_index++ ) );
   }

private:
   std::size_t port_name_index = 0;
   void       *it_begin_ptr    = nullptr;
   void       *it_end_ptr      = nullptr;
   std::function< bool ( void*, void*, Port& ) > inc_func;

};
#endif /* END _READCONTAINER_TCC_ */