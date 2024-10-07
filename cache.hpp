#ifndef CACHE_HPP
#define CACHE_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <list>
#include <cassert>
#include <string>
#include <algorithm>

namespace caches
{
    enum Status
    {
        lir_,            //lir-block
        hirr_,          //resident hir-block
        hirnr_          //non-resident hir-block
    };

    template <typename KeyT, typename T> struct pnode_t
    {
        //typename std::deque<KeyT>::iterator itq;
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

            void push_front(const KeyT k);
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

        int process_request(const data::data_t<KeyT, T> &dref);
        void process_hashtable_pair(const std::pair< KeyT, pnode_t<KeyT, T>>& h); //i will need such func later
        void push_new_request(const data::data_t<KeyT, T> &dref);
    };

}

template <typename KeyT, typename T> 
int caches::lirs<KeyT, T>::process_request(const data::data_t<KeyT, T>& dref)
{
    if(auto it = hashtable.find(dref.key_); it == hashtable.end())
    {
#if 1
        std::cerr << "          key " << dref.key_ << " not found\n"; 
#endif
        push_new_request(dref);
    }  

#if 1      
    else
        std::cerr << "          key " << dref.key_ << " was found\n";
#endif

    auto ht_it = hashtable.find(dref.key_); //ht_it == hashtable iterator to pair {KeyT, pnode_t}
    assert(ht_it != hashtable.end());

#if 1
    std::cerr << "ht_it->first == " << ht_it->first << "\n";
    std::cerr << "ht_it->second.key_ == " << ht_it->second.key_ << "\n";
#endif

    if(llirs.list_.size() <= llirs.cap)
    {
        ht_it->second.status = lir_;

        if(ht_it->first != *tdeque.begin())
        {
            auto qe = std::find(tdeque.begin(), tdeque.end(), ht_it->first);
            std::rotate(tdeque.begin(), qe, (qe + 1)); 
        }

        llirs.push_front(ht_it->first);
#if 1        
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
    }
    return 0;
}

template <typename KeyT, typename T> 
void caches::lirs<KeyT, T>::push_new_request(const data::data_t<KeyT, T>& dref)
{
    tdeque.push_front(dref.key_);
    auto itp = hashtable.emplace(std::pair{dref.key_, pnode_t<KeyT, T>{dref.key_, dref}}); 
                                                                                            //iterator    == itp->second
    KeyT k = dref.key_;                                                                     //pnode_t.itq == iterator->itq
    auto is_suitable_key = [k] (KeyT key) {return key == k; };

#if 0 // move it somewhere else
    if(lhirs.list_.size() >= lhirs.cap)                 // freeing up space 
    {                                                   // to push front new request
        auto i = hashtable.find(lhirs.list_.front());
        i->second.status = hirnr_;
        lhirs.list_.pop_back();
    }

    lhirs.push_front(k);
#endif

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
}

template <typename KeyT, typename T> void caches::lirs<KeyT, T>::list_t::push_front(const KeyT k)
{
    auto itl = std::find(list_.begin(), list_.end(), k);

    (itl == list_.end()) 
    ? list_.push_front(k) 
    : list_.splice(list_.begin(), list_, itl);
}

#endif

