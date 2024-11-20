#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <queue>                
#include <string>         
#include <unordered_map>
#include <vector>

namespace caches
{
    template <typename KeyT, typename T = std::string> class lirs final
    {
        enum Status
        {
            lir    ,    //  lir-block
            hirr   ,    //  resident hir-block
            hirnr  ,    //  non-resident hir-block
        };

        struct list_t
        {
            size_t cap = 0; 
            std::list<KeyT> list;

            enum Csizes {hsize = 1, lsize = 99 };   //  percentage of hirs and lirs lists sizes 
            
            list_t(const size_t cs) : cap((cs * hsize / 100) + 1) {};
            list_t(const size_t cs, const size_t hs) : cap(cs - hs) {};
        };

        const size_t csize;
        list_t lhirs;  //  part of cache for hirs elements (approximately 1% of cache size)
        list_t llirs;  //  part of cache for lirs elements (approximately 99% of cache size)
        std::list<KeyT> query_queue; 

        struct mnode_t
        {
            Status status = hirr;
            
            using list_iter = typename std::list<KeyT>::iterator;
            list_iter c_it; //  iterator to cache element
            list_iter q_it; //  iterator to query queue element

            mnode_t(list_iter q) : q_it(q) {};
        };
        std::unordered_map<KeyT, mnode_t> hashtable;  
        
        void push_new_request(const KeyT& k);
        void swap_hir_and_lir(const KeyT& k);   //  swaps last lir  and certain hirr
        
        void rotate_queue_if(const KeyT& k);
        void queue_push_front(const KeyT& k);
        void queue_prunning();  // pops all unnecessary hirr and hirhr elements

        void list_pop_back(std::list<KeyT>& list);
        void list_push_front(std::list<KeyT>& list, const KeyT& k);

public :

        explicit lirs(const size_t sz = 0) : csize(sz), lhirs(sz), llirs(sz, lhirs.cap) {};
        bool process_request(const KeyT& k);

        const size_t lhirs_cap() const {return lhirs.cap; };
        const size_t llirs_cap() const {return llirs.cap; };
    };
}

template <typename KeyT, typename T> 
bool caches::lirs<KeyT, T>::process_request(const KeyT& k)
{     
    auto ht_it = hashtable.find(k);
    if(ht_it == hashtable.end())  //  new request
    {
        push_new_request(k);
        ht_it = hashtable.find(k);
        assert(ht_it != hashtable.end());
        assert(ht_it->second.q_it != query_queue.end());

        if(llirs.list.size() < llirs.cap)
        {
            ht_it->second.status = lir;
            list_push_front(llirs.list, k);
            return false;
        }

        if(lhirs.list.size() == lhirs.cap)
            list_pop_back(lhirs.list);
       
        list_push_front(lhirs.list, k);

        return false;
    }

    assert(ht_it != hashtable.end());    
    switch(ht_it->second.status)
    {
        case lir   :    //  accessing lir element
                        rotate_queue_if(ht_it->first);
                        list_push_front(llirs.list, ht_it->first);
                        queue_prunning();

                        return true;
        case hirr  :    //  accessing hir resident element             
                        if(ht_it->second.q_it != query_queue.end())
                        {
                            rotate_queue_if(ht_it->first);
                            swap_hir_and_lir(ht_it->first);
                            queue_prunning();
                        }
                        else
                            queue_push_front(ht_it->first);

                        return true;
        case hirnr :    //  accessing hir non-resident element              
                        list_pop_back(lhirs.list);
                        list_push_front(lhirs.list, ht_it->first);
                        
                        if(ht_it->second.q_it == query_queue.end())
                        {
                            queue_push_front(ht_it->first);
                            return false;
                        }

                        rotate_queue_if(ht_it->first);
                        swap_hir_and_lir(ht_it->first);
                        queue_prunning();

                        return false;
    }
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::push_new_request(const KeyT& k)
{
    query_queue.push_front(k);
    hashtable.emplace(k, query_queue.begin());
    
    auto i = hashtable.find(k);
    assert(i != hashtable.end());
    assert(*i->second.q_it == query_queue.front());
    i->second.c_it = lhirs.list.end();  //  c_it == lhirs.list.end() means
                                        //  there's no key k in cache 
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::queue_push_front(const KeyT& k)
{
    auto ht_it = hashtable.find(k);
    assert(ht_it != hashtable.end());
    assert(ht_it->second.q_it == query_queue.end());

    query_queue.push_front(ht_it->first);
    ht_it->second.q_it = query_queue.begin(); 
    assert(*ht_it->second.q_it == k);
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::list_push_front(std::list<KeyT>& list, const KeyT& k)
{
    auto ht_it = hashtable.find(k);
    assert(ht_it != hashtable.end());

    if(ht_it->second.c_it == lhirs.list.end())  
    {                                           
        list.push_front(k);
        ht_it->second.c_it = list.begin();
        assert(ht_it->second.c_it != list.end());
    }
    else
        list.splice(list.begin(), list, ht_it->second.c_it);
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::list_pop_back(std::list<KeyT>& list)
{
    auto ht_it = hashtable.find(list.back());
    assert(ht_it != hashtable.end());
    assert(ht_it->second.c_it != lhirs.list.end());

    ht_it->second.status = hirnr;
    list.pop_back();
    ht_it->second.c_it = lhirs.list.end();  
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::rotate_queue_if(const KeyT& k)
{
    if(k != query_queue.front())
    {
        auto ht_it = hashtable.find(k);
        assert(ht_it != hashtable.end());
        assert(ht_it->second.q_it != query_queue.end());
        query_queue.splice(query_queue.begin(), query_queue, ht_it->second.q_it); 
    }
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::queue_prunning()
{
    for(auto i = query_queue.rbegin(), e = query_queue.rend(); i != e;) //there was "itq = tdeque.begin()"
    {
        assert(i != e);
        auto ht_it = hashtable.find(*i);
        assert(ht_it != hashtable.end());
        assert(ht_it->first == *i);
        assert(ht_it->second.q_it != query_queue.end());

        switch(ht_it->second.status)
        {
            case lir   :
                            return;
            case hirr  :    
            case hirnr :  
                            query_queue.pop_back();
                            ht_it->second.q_it = query_queue.end();
                            break;
        }
    }
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::swap_hir_and_lir(const KeyT& k)
{
    auto ht_ith = hashtable.find(k);
    auto ht_itl = hashtable.find(llirs.list.back());

    assert(ht_ith != hashtable.end());
    assert(ht_itl != hashtable.end());
    assert(ht_ith->second.c_it != lhirs.list.end());
    assert(ht_itl->second.c_it != lhirs.list.end());
    
    ht_ith->second.status = lir;
    ht_itl->second.status = hirr;

    llirs.list.splice(llirs.list.begin(), lhirs.list, ht_ith->second.c_it);
    lhirs.list.splice(lhirs.list.begin(), llirs.list, ht_itl->second.c_it);
}
