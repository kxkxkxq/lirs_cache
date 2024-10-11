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
         lir_            //lir-block
        ,hirr_          //resident hir-block
        ,hirnr_          //non-resident hir-block
    };

    enum Cachehit
    {
         miss_
        ,hit_        
    };

    template <typename KeyT, typename T> struct pnode_t
    {
        const data::data_t<KeyT, T>& itd;
        int status = hirr_;
        KeyT key_;
        
        pnode_t(const KeyT k, const data::data_t<KeyT, T>& d) : key_(k), itd(d) {};
    };

    template <typename KeyT, typename T> class lirs
    {
        enum Csizes {hsize = 1, lsize = 99 }; //percentage of hirs and lirs lists sizes 
        size_t csize = 0;
        
        struct list_t
        {
            size_t cap = 0; 
            std::list<KeyT> list_;

            void push_front(const KeyT k);      //push or lift list element
        };

        list_t lhirs;      //part of cache for hirs elements (approximately 1% of cache size)
        list_t llirs;      //part of cache for lirs elements (approximately 99% of cache size)
        std::unordered_multimap<KeyT, pnode_t<KeyT, T>> hashtable;
        std::deque<KeyT> tdeque;
        
   
    public:

        lirs(const size_t sz) : csize(sz)
        {   
            const size_t hs = (sz * hsize / 100) + 1;
            const size_t ls = sz - hs;

            lhirs.cap = hs;
            llirs.cap = ls;

#if 0
            std::cerr << "      cacehs::lirs::lirs() : hirs size == " << hs << "\n";
            std::cerr << "      caches::lirs::lirs() : lirs size == " << ls << "\n";
            std::cerr << "      caches::lirs.size == " << csize << "\n";
            
            std::cerr << "      llist.size_ == " << llirs.cap << "\n";
            std::cerr << "      lhirs.size_ == " << lhirs.cap << "\n";
#endif

        };

        int  process_request(const data::data_t<KeyT, T> &dref);
        auto push_new_request(const data::data_t<KeyT, T> &dref);
        
        void rotate_deque_if(const KeyT k);
        void deque_prunning();
        
        void swap_hir_and_lir(const KeyT hir_elem);                                     // swaps last lir_  and certain hirr_
        void pop_back_lhirs_element();
    };

}

template <typename KeyT, typename T> 
int caches::lirs<KeyT, T>::process_request(const data::data_t<KeyT, T>& dref)
{
    if(auto it = hashtable.find(dref.key_); it == hashtable.end())
    {
        auto ht_it = push_new_request(dref);

        if(llirs.list_.size() < llirs.cap)
        {
            ht_it->second.status = lir_;
            rotate_deque_if(ht_it->first);
            llirs.push_front(ht_it->first);

#if 0        
            std::cerr << "      lirs list : ";
            for(auto i = llirs.list_.begin(); i != llirs.list_.end(); ++i)
                std::cerr << *i << " ";
            std::cerr << "\n";

            std::cerr << "      hirs list : ";
            for(auto i = lhirs.list_.begin(); i != lhirs.list_.end(); ++i)
                std::cerr << *i << " ";
            std::cerr << "\n";

            std::cerr << "      tdeque after rotation : ";
            for(auto i = tdeque.begin(); i != tdeque.end(); ++i)
                std::cerr << *i << " ";
            std::cerr << "\n\n";
#endif
            return miss_;
        }

        if(lhirs.list_.size() == lhirs.cap)
            pop_back_lhirs_element();

        lhirs.push_front(ht_it->first);

        return miss_;
    }


    auto ht_it = hashtable.find(dref.key_); //ht_it == hashtable iterator to pair {KeyT, pnode_t}
    assert(ht_it != hashtable.end());

#if 0
    std::cerr << "ht_it->first == " << ht_it->first << "\n";
    std::cerr << "ht_it->second.key_ == " << ht_it->second.key_ << "\n";
#endif
    
    switch(ht_it->second.status)
    {
        case lir_   : //accessing lir element
                        rotate_deque_if(ht_it->first);
                        llirs.push_front(ht_it->first);
                        deque_prunning();

                        return hit_;

        case hirr_  : //accessing hir resident element
                        rotate_deque_if(ht_it->first);
                        
                        if(auto itq = std::find(tdeque.begin(), tdeque.end(), ht_it->first); itq != tdeque.end())
                        {
                            swap_hir_and_lir(ht_it->first);
                            deque_prunning();
                        }
                        else
                            tdeque.push_front(ht_it->first); 

                        return hit_;  

        case hirnr_ : //accessing hir non-resident element
                        pop_back_lhirs_element();
                        swap_hir_and_lir(ht_it->first);
                        deque_prunning();

                        return miss_;
    }
    return 0;
}

template <typename KeyT, typename T> 
auto caches::lirs<KeyT, T>::push_new_request(const data::data_t<KeyT, T>& dref)
{
    tdeque.push_front(dref.key_);
    auto itp = hashtable.emplace(std::pair{dref.key_, pnode_t<KeyT, T>{dref.key_, dref}}); 

#if 0
    std::cerr << "\n---------------------------------------------------------------------------\n\n";
    std::cerr << "      lirs::push_new_request() : pnode_t.key_ == " << itp->second.key_ << '\n';
    
    std::cerr << "          pnode_t.status == " << itp->second.status << "\n";
    std::cerr << "          request == " << dref.key_ << "\n";
    std::cerr << "          &pnode_t == " << &itp->second << "\n\n";

    std::cerr << "          tdeque->front() == " << tdeque.front() << "\n";
    std::cerr << "          &pnode_t.itd == " << &itp->second.itd << "\n";         

    std::cerr << "      hashtable pair{KeyT, pnode_t<KeyT, T>} : ";
    std::cerr << "{KeyT elem == " << itp->first << ", pnode_t<KeyT, T>.KeyT elem == " << itp->second.key_ << "}\n\n";

    std::cerr << "      dqueue : ";
    for(auto i = tdeque.begin(); i != tdeque.end(); ++i)
        std::cerr << *i << " ";
    std::cerr << "\n";
    std::cerr << "      hashtable size == " << hashtable.size() << "\n";
#endif

    return  itp;
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::list_t::push_front(const KeyT k)
{
    auto itl = std::find(list_.begin(), list_.end(), k);

    (itl == list_.end())                                               
    ? list_.push_front(k) 
    : list_.splice(list_.begin(), list_, itl);
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::pop_back_lhirs_element()
{
    auto ht_it = hashtable.find(lhirs.list_.back());
    ht_it->second.status = hirnr_;
    lhirs.list_.pop_back();
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
            case lir_   :
                            return;

            case hirnr_ :
                            hashtable.erase(*itq);   

            case hirr_  :    
                            tdeque.pop_back();
                            break;
        }
    }
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::swap_hir_and_lir(const KeyT hir_elem)
{
    auto ht_ith = hashtable.find(hir_elem);
    auto ht_itl = hashtable.find(llirs.list_.front());
    
    ht_ith->second.status = lir_;
    ht_itl->second.status = hirr_;

    auto itl = llirs.list_.rbegin();
    auto ith = std::find(lhirs.list_.begin(), lhirs.list_.end(), hir_elem);
    
    std::iter_swap(ith, itl);
    lhirs.push_front(*itl); //in order to *itl become lhirs.list_.front()
}

#endif

