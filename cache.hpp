#ifndef CACHE_HPP
#define CACHE_HPP

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
    enum Status
    {
        lir    ,          //lir-block
        hirr   ,          //resident hir-block
        hirnr             //non-resident hir-block
    };

    enum Cachehit
    {
        miss   ,
        hit        
    };

    template <typename KeyT, typename T> struct pnode_t
    {
        const data::data_t<KeyT, T>& itd;
        int status = hirr;
        KeyT key;
        
        pnode_t(const KeyT k, const data::data_t<KeyT, T>& d) : key(k), itd(d) {};
    };

    template <typename KeyT, typename T> class lirs
    {
        size_t csize = 0;
        
        struct list_t
        {
            size_t cap = 0; 
            std::list<KeyT> list;

            enum Csizes {hsize = 1, lsize = 99 }; //percentage of hirs and lirs lists sizes 
            
            list_t(const size_t cs) : cap((cs * hsize / 100) + 1) {};
            list_t(const size_t cs, const size_t hs) : cap(cs - hs) {};
            
            void push_front(const KeyT k);      //push or lift list element
        };

        list_t lhirs;      //part of cache for hirs elements (approximately 1% of cache size)
        list_t llirs;      //part of cache for lirs elements (approximately 99% of cache size)
        std::unordered_multimap<KeyT, pnode_t<KeyT, T>> hashtable;
        std::deque<KeyT> tdeque;
        
   
    public:

        lirs(const size_t sz) : csize(sz), lhirs(sz), llirs(sz, lhirs.cap) {};

        int  process_request(const data::data_t<KeyT, T> &dref);
        auto push_new_request(const data::data_t<KeyT, T> &dref);
        
        void rotate_deque_if(const KeyT k);
        void deque_prunning();
        
        void swap_hir_and_lir(const KeyT hir_elem);                                     // swaps last lir  and certain hirr
        void pop_back_lhirs_element();
    };

}

template <typename KeyT, typename T> 
int caches::lirs<KeyT, T>::process_request(const data::data_t<KeyT, T>& dref)
{  
    if(auto it = hashtable.find(dref.key); it == hashtable.end())
    {
        auto ht_it = push_new_request(dref);

        if(llirs.list.size() < llirs.cap)
        {
            ht_it->second.status = lir;
            rotate_deque_if(ht_it->first);
            llirs.push_front(ht_it->first);

            return miss;
        }

        if(lhirs.list.size() == lhirs.cap)
            pop_back_lhirs_element();

        lhirs.push_front(ht_it->first);

        return miss;
    }


    auto ht_it = hashtable.find(dref.key); //ht_it == hashtable iterator to pair {KeyT, pnode_t}
    assert(ht_it != hashtable.end());
    
    switch(ht_it->second.status)
    {
        case lir   : //accessing lir element
                        rotate_deque_if(ht_it->first);
                        llirs.push_front(ht_it->first);
                        deque_prunning();

                        return hit;

        case hirr  : //accessing hir resident element
                        rotate_deque_if(ht_it->first);
                        
                        if(auto itq = std::find(tdeque.begin(), tdeque.end(), ht_it->first); itq != tdeque.end())
                        {
                            swap_hir_and_lir(ht_it->first);
                            deque_prunning();
                        }
                        else
                            tdeque.push_front(ht_it->first); 

                        return hit;  

        case hirnr : //accessing hir non-resident element
                        pop_back_lhirs_element();
                        lhirs.push_front(ht_it->first);
                        
                        swap_hir_and_lir(ht_it->first);
                        deque_prunning();

                        return miss;
    }
}

template <typename KeyT, typename T> 
auto caches::lirs<KeyT, T>::push_new_request(const data::data_t<KeyT, T>& dref)
{
    tdeque.push_front(dref.key);
    auto itp = hashtable.emplace(std::pair{dref.key, pnode_t<KeyT, T>{dref.key, dref}}); 

    return  itp;
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::list_t::push_front(const KeyT k)
{
    auto itl = std::find(list.begin(), list.end(), k);

    (itl == list.end())                                               
    ? list.push_front(k) 
    : list.splice(list.begin(), list, itl);
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::pop_back_lhirs_element()
{
    auto ht_it = hashtable.find(lhirs.list.back());
    ht_it->second.status = hirnr;
    lhirs.list.pop_back();
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::rotate_deque_if(const KeyT k)
{
    if(k != tdeque.front())
    {
        auto qe = std::find(tdeque.begin(), tdeque.end(), k);
        std::rotate(tdeque.begin(), qe, (qe + 1)); 
    }
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::deque_prunning()
{
    for(auto itq = tdeque.begin(); ; ++itq)
    {
        auto ht_it = hashtable.find(*itq);

        switch(ht_it->second.status)
        {
            case lir   :
                            return;

            case hirnr :
                            hashtable.erase(*itq);
                            assert(hashtable.find(*itq) == hashtable.end());   

            case hirr  :    
                            tdeque.pop_back();
                            break;
        }
    }
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::swap_hir_and_lir(const KeyT hir_elem)
{
    auto ht_ith = hashtable.find(hir_elem);
    auto ht_itl = hashtable.find(llirs.list.back());
    
    ht_ith->second.status = lir;
    ht_itl->second.status = hirr;

    auto itl = llirs.list.rbegin();
    auto ith = std::find(lhirs.list.begin(), lhirs.list.end(), hir_elem);

    std::iter_swap(ith, itl);
}

#endif