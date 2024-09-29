#ifndef CACHE_HPP
#define CACHE_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <list>
#include <cassert>
#include <string>

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
        typename std::deque<KeyT>::iterator itq;
        const data::data_t<KeyT, T>& itd;
        int status = hirr_;
        KeyT key_;
        
        pnode_t(const KeyT k, const data::data_t<KeyT, T>& d) : key_(k), itd(d) {};
    };

    template <typename KeyT, typename T> class lirs
    {
        std::unordered_multimap<KeyT, pnode_t<KeyT, T>> hashtable; //we need to write : hashtable::insert({key, page_});
        std::deque<KeyT> tdeque;
        std::list<KeyT> tlist;
   
    public:
      
        unsigned csize = 0;

        int process_request(const data::data_t<KeyT, T> &dref);
        void process_hashtable_pair(const std::pair< KeyT, pnode_t<KeyT, T>>& h); //i will need such func later

        lirs(const unsigned sz) : csize(sz) {};
    };

}

template <typename KeyT, typename T> 
int caches::lirs<KeyT, T>::process_request(const data::data_t<KeyT, T>& dref)
{
    tdeque.push_front(dref.key_);
    pnode_t<KeyT, T> *ppage = new pnode_t<KeyT, T> {dref.key_, dref};

    KeyT k = dref.key_;
    auto is_suitable_key = [k] (KeyT key) {return key == k; };
    ppage->itq = find_if(tdeque.begin(), tdeque.end(), is_suitable_key);

#if 1
    std::cerr << "\n---------------------------------------------------------------------------\n";
    std::cerr << "\n      lirs::process_request() : ppage->key_ == " << ppage->key_ << '\n';
    std::cerr << "          request == " << dref.key_ << "\n";
    std::cerr << "          &pnode_t == " << ppage << "\n";

    std::cerr << "          tdeque->front() == " << tdeque.front() << "\n";
    const data::data_t<KeyT, T> d = ppage->itd;
    std::cerr << "          &page.itd == " << &d << "\n";
    std::cerr << "          page.itq == " << *ppage->itq << "\n";         
#endif

    hashtable.insert({ppage->key_, *ppage});

#if 1
    std::cerr << "\n        inserted pair{" << ppage->key_ << ", " << ppage << "}\n";
    std::cerr << "      hashtable pair{KeyT, pnode_t<KeyT, T>} : ";
    
    auto it = hashtable.find(ppage->key_);
    std::cerr << "{pair.first == " << it->first << ", pair.second.key == " << it->second.key_ << "}\n";
#endif

    return 0;
}

#endif