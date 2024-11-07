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
    template <typename KeyT, typename T> class lirs
    {
        enum Status
        {
            lir    ,          //lir-block
            hirr   ,          //resident hir-block
            hirnr             //non-resident hir-block
        };

        struct pnode_t
        {
            const data::data_t<KeyT, T>& itd;
            int status = hirr;
            KeyT key;
        
        pnode_t(const KeyT k, const data::data_t<KeyT, T>& d) : key(k), itd(d) {};
        };

        struct list_t
        {
            size_t cap = 0; 
            std::list<KeyT> list;

            enum Csizes {hsize = 1, lsize = 99 }; //percentage of hirs and lirs lists sizes 
            
            list_t(const size_t cs) : cap((cs * hsize / 100) + 1) {};
            list_t(const size_t cs, const size_t hs) : cap(cs - hs) {};
            
            void push_front(const KeyT k);      //push or lift list element
            
            void print_list() const
            {
                for(auto i = list.begin(), e = list.end(); i != e; ++i)
                    std::cout << *i << " ";
                std::cout << "\n";
            };
        };

        size_t csize = 0;

        list_t lhirs;      //part of cache for hirs elements (approximately 1% of cache size)
        list_t llirs;      //part of cache for lirs elements (approximately 99% of cache size)
        std::unordered_multimap<KeyT, pnode_t> hashtable;
        std::deque<KeyT> tdeque;

        bool process_single_cache(const data::data_t<KeyT, T> &dref);   //case for "degenerate" cache
        auto push_new_request(const data::data_t<KeyT, T> &dref);
        
        void rotate_deque_if(const KeyT k);
        void deque_prunning();
        
        void swap_hir_and_lir(const KeyT hir_elem);                                     // swaps last lir  and certain hirr
        void pop_back_lhirs_element();

    public:
        
        lirs(const size_t sz) : csize(sz), lhirs(sz), llirs(sz, lhirs.cap) {};
        bool process_request(const data::data_t<KeyT, T> &dref);
        
        const size_t lhirs_cap() const {return lhirs.cap; };
        const size_t llirs_cap() const {return llirs.cap; };
    };
}

template <typename KeyT, typename T> 
bool caches::lirs<KeyT, T>::process_request(const data::data_t<KeyT, T>& dref)
{  
    if(csize == 1)
        return process_single_cache(dref);
    
    if(auto it = hashtable.find(dref.key); it == hashtable.end())
    {
        auto ht_it = push_new_request(dref);

        if(llirs.list.size() < llirs.cap)
        {
            ht_it->second.status = lir;
            rotate_deque_if(ht_it->first);
            llirs.push_front(ht_it->first);

            return false;
        }

        if(lhirs.list.size() == lhirs.cap)
            pop_back_lhirs_element();

        lhirs.push_front(ht_it->first);

        return false;
    }

    auto ht_it = hashtable.find(dref.key); //ht_it == hashtable iterator to pair {KeyT, pnode_t}
    assert(ht_it != hashtable.end());
    
    switch(ht_it->second.status)
    {
        case lir   : //accessing lir element
                        rotate_deque_if(ht_it->first);
                        llirs.push_front(ht_it->first);
                        deque_prunning();

                        return true;

        case hirr  : //accessing hir resident element
                        rotate_deque_if(ht_it->first);
                        
                        if(auto itq = std::find(tdeque.begin(), tdeque.end(), ht_it->first); itq != tdeque.end())
                        {
                            swap_hir_and_lir(ht_it->first);
                            deque_prunning();
                        }
                        else
                            tdeque.push_front(ht_it->first); 

                        return true;  

        case hirnr : //accessing hir non-resident element
                        pop_back_lhirs_element();
                        lhirs.push_front(ht_it->first);
                        
                        swap_hir_and_lir(ht_it->first);
                        deque_prunning();

                        return false;
    }

    return 0;
}

template <typename KeyT, typename T> 
auto caches::lirs<KeyT, T>::push_new_request(const data::data_t<KeyT, T>& dref)
{
    tdeque.push_front(dref.key);
    pnode_t p{dref.key, dref};
    auto itp = hashtable.insert(std::pair{dref.key, p});

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
    for(auto itq = tdeque.rbegin(), e = tdeque.rend(); itq != e; ++itq) //there was "itq = tdeque.begin()"
    {
        auto ht_it = hashtable.find(*itq);
        assert(ht_it != hashtable.end());

        switch(ht_it->second.status)
        {
            case lir   :
                            return;

            case hirnr :  
                            tdeque.pop_back();
                            break;
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

    llirs.push_front(hir_elem);
}

template <typename KeyT, typename T> 
bool caches::lirs<KeyT, T>::process_single_cache(const data::data_t<KeyT, T>& dref)
{
    if(hashtable.find(dref.key) == hashtable.end())
    {
        if(lhirs.list.size() == 0)
        {
            auto ht_it = push_new_request(dref);
            lhirs.push_front(ht_it->first);
            return false;
        }

        hashtable.erase(dref.key);
        lhirs.list.pop_back();

        auto ht_it = push_new_request(dref);
        lhirs.push_front(ht_it->first);
        return false;
    }

    return true;
}

#endif