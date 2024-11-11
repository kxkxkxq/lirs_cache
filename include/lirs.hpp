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
            lir    ,          //lir-block
            hirr   ,          //resident hir-block
            hirnr             //non-resident hir-block
        };


        struct list_t
        {
            size_t cap = 0; 
            std::list<KeyT> list;

            enum Csizes {hsize = 1, lsize = 99 }; //percentage of hirs and lirs lists sizes 
            
            list_t(const size_t cs) : cap((cs * hsize / 100) + 1) {};
            list_t(const size_t cs, const size_t hs) : cap(cs - hs) {};
        };

        size_t                                  csize = 0;
        list_t                                  lhirs;      //part of cache for hirs elements (approximately 1% of cache size)
        list_t                                  llirs;      //part of cache for lirs elements (approximately 99% of cache size)
        std::list<KeyT>                         tdeque;
       
        struct pnode_t
        {            
            using list_iter = typename std::list<KeyT>::iterator;
            
            list_iter q_it;
            list_iter l_it;
            int status = hirr;
        
            pnode_t(list_iter q) : q_it(q) {};
        };
       
        std::unordered_multimap<KeyT, pnode_t> hashtable;

        void list_push_front(std::list<KeyT>& list, const KeyT& k);     //push or lift list element
        void list_pop_back(std::list<KeyT>& list);

        bool process_single_cache(const KeyT& kref);   //case for "degenerate" cache
        void push_new_request(const KeyT& kref);
        
        void rotate_deque_if(const KeyT& k);
        void deque_prunning();
        
        void swap_hir_and_lir(const KeyT& hir_elem); 

public:
        
        lirs(const size_t sz) : csize(sz), lhirs(sz), llirs(sz, lhirs.cap) {};
        bool process_request(const KeyT& kref);
        
        const size_t lhirs_cap() const {return lhirs.cap; };
        const size_t llirs_cap() const {return llirs.cap; };
    };
}

template <typename KeyT, typename T> 
bool caches::lirs<KeyT, T>::process_request(const KeyT& kref)
{  
    if(csize == 1)                        
        return process_single_cache(kref);
    
    if(auto it = hashtable.find(kref); it == hashtable.end())
    {
        push_new_request(kref);
        auto ht_it = hashtable.find(kref);
        
        assert(ht_it != hashtable.end());
        if(llirs.list.size() < llirs.cap)
        {
            ht_it->second.status = lir;
            list_push_front(llirs.list, kref);
            assert(ht_it->second.l_it != llirs.list.end());

            return false;
        }

        if(lhirs.list.size() == lhirs.cap)
            list_pop_back(lhirs.list);

        list_push_front(lhirs.list, kref); //ht_it->first
        return false;
    }

    auto ht_it = hashtable.find(kref);
    assert(ht_it != hashtable.end());
    
    rotate_deque_if(ht_it->first);
    switch(ht_it->second.status)
    {
        case lir   : //accessing lir element
                        list_push_front(llirs.list, ht_it->first);
                        deque_prunning();
                        return true;

        case hirr  : //accessing hir resident element
                        if(ht_it->second.q_it != tdeque.end())
                        {
                            swap_hir_and_lir(ht_it->first);
                            deque_prunning();
                        }
                        else
                            tdeque.emplace_front(ht_it->first); 
                        
                        return true;  

        case hirnr : //accessing hir non-resident element
                        list_pop_back(lhirs.list);
                        list_push_front(lhirs.list, ht_it->first);
                        swap_hir_and_lir(ht_it->first);
                        deque_prunning();
                    
                        return false;
    }
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::push_new_request(const KeyT& kref)
{
    tdeque.emplace_front(kref);
    hashtable.emplace(kref, tdeque.begin());
    
    auto ht_it = hashtable.find(kref);
    assert(ht_it != hashtable.end());
    ht_it->second.l_it = lhirs.list.end();
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::rotate_deque_if(const KeyT& k)
{
    if(k != tdeque.front())
    {
        auto ht_it = hashtable.find(k);
        assert(ht_it != hashtable.end());
        tdeque.splice(tdeque.begin(), tdeque, ht_it->second.q_it);
    }
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::list_push_front(std::list<KeyT>& list, const KeyT& k)
{
    auto ht_it = hashtable.find(k);
    assert(ht_it != hashtable.end());
    if(ht_it->second.l_it == lhirs.list.end())
    {
        list.emplace_front(k);
        ht_it->second.l_it = list.begin();
    }
    else
        list.splice(list.begin(), list, ht_it->second.l_it);
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::list_pop_back(std::list<KeyT>& list)
{                                           
    auto ht_it = hashtable.find(list.back());
    assert(ht_it != hashtable.end());
    ht_it->second.status = hirnr;
                       
    list.erase(ht_it->second.l_it);
    ht_it->second.l_it = lhirs.list.end(); 
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::deque_prunning()
{
    for(auto q_it = tdeque.rbegin(), e = tdeque.rend(); q_it != e; ++q_it)
    {
        auto ht_it = hashtable.find(*q_it);
        assert(ht_it != hashtable.end());
    
        switch(ht_it->second.status)
        {
            case lir   :
                            return;
            case hirnr :  
                            tdeque.erase(ht_it->second.q_it);
                            hashtable.erase(*q_it);
                            break;
            case hirr  :     
                            tdeque.erase(ht_it->second.q_it);
                            ht_it->second.q_it = tdeque.end();
                            
                            break;
        }
    }
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::swap_hir_and_lir(const KeyT& hir_elem)
{
    auto hir_it = hashtable.find(hir_elem);
    auto lir_it = hashtable.find(llirs.list.back());
    
    hir_it->second.status = lir;
    lir_it->second.status = hirr;

    auto tmp = std::next(hir_it->second.l_it);
    llirs.list.splice(lir_it->second.l_it, lhirs.list, hir_it->second.l_it);
    lhirs.list.splice(tmp, llirs.list, lir_it->second.l_it);

    list_push_front(llirs.list, hir_elem); 
}

template <typename KeyT, typename T> 
bool caches::lirs<KeyT, T>::process_single_cache(const KeyT& kref)
{
    if(hashtable.find(kref) == hashtable.end())
    {
        if(lhirs.list.size() == 0)
        {                               //  refactor this piece
            push_new_request(kref);
            auto ht_it = hashtable.find(kref);
            list_push_front(lhirs.list, kref);
            return false;
        }

        hashtable.erase(kref);
        list_pop_back(lhirs.list);
        
        push_new_request(kref);
        auto ht_it = hashtable.find(kref);
        list_push_front(lhirs.list, kref);
        return false;
    }

    return true;
}