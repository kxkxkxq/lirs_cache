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
            inv         //  invalid starus state  
        };

        struct list_t
        {
            size_t cap = 0; 
            std::list<KeyT> list;

            enum Csizes {hsize = 1, lsize = 99 };   //  percentage of hirs and lirs lists sizes 
            
            list_t(const size_t cs) : cap((cs * hsize / 100) + 1) {};
            list_t(const size_t cs, const size_t hs) : cap(cs - hs) {};
        };

        size_t          csize = 0;
        list_t              lhirs;  //  part of cache for hirs elements (approximately 1% of cache size)
        list_t              llirs;  //  part of cache for lirs elements (approximately 99% of cache size)
        std::list<KeyT>    tdeque;

        using list_iter = typename std::list<KeyT>::iterator;
       
        std::unordered_map<KeyT, list_iter>    deque_check;
        std::unordered_map<KeyT, list_iter>    cache_check;
        std::unordered_map<KeyT, Status>    request_status;

        void push_new_request(const KeyT& k);
        void swap_hir_and_lir(const KeyT& k);   //  swaps last lir  and certain hirr
        
        void deque_pop_back();
        void deque_push_front(const KeyT& k);
        void rotate_deque_if(const KeyT& k);
        void deque_prunning();

        void list_pop_back(std::list<KeyT>& list);
        void list_push_front(std::list<KeyT>& list, const KeyT& k);

public :

        lirs(const size_t sz) : csize(sz), lhirs(sz), llirs(sz, lhirs.cap) {};
        bool process_request(const KeyT& k);

        void print_all();
        const size_t lhirs_cap() const {return lhirs.cap; };
        const size_t llirs_cap() const {return llirs.cap; };
    };
}

template <typename KeyT, typename T> 
bool caches::lirs<KeyT, T>::process_request(const KeyT& k)
{
    static int f = 0;
    auto rs_it = request_status.find(k);
    if(rs_it == request_status.end())
    {
        push_new_request(k);
        auto rs_it = request_status.find(k);

        if(llirs.list.size() < llirs.cap)
        {
            rs_it->second = lir;
            list_push_front(llirs.list, k);
            return false;
        }

        if(lhirs.list.size() == lhirs.cap)
            list_pop_back(lhirs.list);

        list_push_front(lhirs.list, k);
        return false;
    }
    
    assert(rs_it != request_status.end());
    if(rs_it->second == inv)
    {
        rs_it->second = hirr;
        deque_push_front(k);

        if(lhirs.list.size() == lhirs.cap)
            list_pop_back(lhirs.list);

        list_push_front(lhirs.list, k);
        return false;
    }

    switch(rs_it->second)
    {
        case lir   : //accessing lir element
                        rotate_deque_if(rs_it->first);
                        list_push_front(llirs.list, rs_it->first);
                        deque_prunning();
                        
                        return true;

        case hirr  : //accessing hir resident element
                        
                            
                        if(deque_check.find(rs_it->first) != deque_check.end())
                        {
                            rotate_deque_if(rs_it->first);
                            swap_hir_and_lir(rs_it->first);
                            deque_prunning();
                        }
                        else
                            deque_push_front(rs_it->first);

                        return true;  

        case hirnr : //accessing hir non-resident element
                        rotate_deque_if(rs_it->first);
                        list_pop_back(lhirs.list);
                        list_push_front(lhirs.list, rs_it->first);
                        
                        swap_hir_and_lir(rs_it->first);
                        deque_prunning();

                        return false;
    }
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::push_new_request(const KeyT& k)
{
    deque_push_front(k);
    request_status.emplace(k, hirr);
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::rotate_deque_if(const KeyT& k)
{
    if(k != tdeque.front())
    {
        auto qc_it = deque_check.find(k);
        tdeque.splice(tdeque.begin(), tdeque, qc_it->second);
    }
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::list_push_front(std::list<KeyT>& list, const KeyT& k)
{
    auto cc_it = cache_check.find(k);

    if(cc_it != cache_check.end())
    {
        if(cc_it->second == list.end())
        {
            auto rs_it = request_status.find(k);
            rs_it->second = hirr;
            list.emplace_front(k);
            cc_it->second = list.begin();
        }
        else
            list.splice(list.begin(), list, cc_it->second);
    }
    else
    {
        list.emplace_front(k);
        cache_check.emplace(k, list.begin());
    }
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::list_pop_back(std::list<KeyT>& list)
{
    auto rs_it = request_status.find(list.back());
    rs_it->second = hirnr;
    
    auto cc_it = cache_check.find(list.back());
    list.erase(cc_it->second);
    cc_it->second = list.end();  
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::deque_prunning()
{
    for(auto q_it = tdeque.rbegin(), e = tdeque.rend(); q_it != e; )  //  there was "itq = tdeque.begin()"
    {
        auto rs_it = request_status.find(*q_it);
        assert(rs_it != request_status.end());

        switch(rs_it->second)
        {
            case lir   :
                            return;

            case hirr  :    
                            deque_pop_back();
                            break;
            case hirnr :  
                            if(rs_it->second != inv)
                            {
                                rs_it->second = inv;
                                deque_pop_back();
                                cache_check.erase(rs_it->first);
                                break;
                            }
                            else   
                                request_status.erase(rs_it);
                                break;
        }
    }
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::deque_pop_back()
{
    auto qc_it = deque_check.find(tdeque.back());
    tdeque.erase(qc_it->second);
    deque_check.erase(qc_it);
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::deque_push_front(const KeyT& k)
{
    tdeque.emplace_front(k);
    deque_check.emplace(k, tdeque.begin());
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::swap_hir_and_lir(const KeyT& k)
{
    auto rs_ith = request_status.find(k);
    auto rs_itl = request_status.find(llirs.list.back());
    
    rs_ith->second = lir;
    rs_itl->second = hirr;

    auto cc_ith = cache_check.find(k);
    auto cc_itl = cache_check.find(llirs.list.back());

    llirs.list.splice(llirs.list.begin(), lhirs.list, cc_ith->second);
    lhirs.list.splice(lhirs.list.begin(), llirs.list, cc_itl->second);
}
