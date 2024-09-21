#ifndef CACHE_HPP
#define CACHE_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <list>

namespace lirscache
{
    template <typename U> struct page_t
    {
        U key;
        std::string* name = nullptr;
    };

    template <typename U> class Cache
    {
        std::unordered_multimap<U, page_t<U>>* hashtable;
        std::deque<U>* udeque;
        std::list<U>* ulist;
   
    public:
      
        unsigned csize = 0;

        using udata = typename data::Data<U>;
        int process_request(const U req);
        
        Cache(unsigned sz) : csize(sz) 
        {
            hashtable = new std::unordered_multimap<U, page_t<U>>;
            udeque = new std::deque<U>;
            ulist = new std::list<U>;
        };
        
        ~Cache()
        {
            delete ulist;
            delete udeque;
            delete hashtable;
        };
    };

}

#endif