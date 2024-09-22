#ifndef CACHE_HPP
#define CACHE_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <list>
#include <cassert>

namespace lirscache
{
    enum STATUS 
    {
        LIR,            //lir-block
        HIR_R,          //resident hir-block
        HIR_NR          //non-resident hir-block
    };

    template <typename U> struct page_t
    {
        U key;
        typename std::deque<U>::iterator itq;
        
        page_t(const U key_) : key(key_) {};
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
        
        Cache(const unsigned sz) : csize(sz) 
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

template <typename U> int lirscache::Cache<U>::process_request(const U req)
{
    page_t<U>* ppage = new page_t<U>{req};

#if 1
    std::cout << "\n      cache::process_request() : page.key == " << ppage->key << '\n';
    std::cerr << "              req == " << req << "\n";
    std::cerr << "              &page_t == " << ppage << "\n";
#endif

    udeque->push_front(ppage->key);
    ppage->itq = udeque->begin();

#if 1
    std::cerr << "      cache::process_request() : udeque->front() == " << udeque->front() << "\n";
    std::cerr << "          page.itq == " << *ppage->itq << "\n";         
#endif

    return 0;
}

#endif