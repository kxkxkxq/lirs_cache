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
    auto itp = hashtable.emplace(std::pair{dref.key_, pnode_t<KeyT, T>{dref.key_, dref}}); 
                                                                                            //iterator    == itp->second
    KeyT k = dref.key_;                                                                     //pnode_t.itq == iterator->itq
    auto is_suitable_key = [k] (KeyT key) {return key == k; };
    itp->second.itq = find_if(tdeque.begin(), tdeque.end(), is_suitable_key);

#if 1
    std::cerr << "\n---------------------------------------------------------------------------\n\n";
    std::cerr << "      lirs::process_request() : pnode_t.key_ == " << itp->second.key_ << '\n';
    std::cerr << "          request == " << dref.key_ << "\n";
    std::cerr << "          &pnode_t == " << &itp->second << "\n\n";

    std::cerr << "          tdeque->front() == " << tdeque.front() << "\n";
    std::cerr << "          &pnode_t.itd == " << &itp->second.itd << "\n";
    std::cerr << "          pnode_t.itq == " << *itp->second.itq << "\n";         

    std::cerr << "      hashtable pair{KeyT, pnode_t<KeyT, T>} : ";
    std::cerr << "{KeyT elem == " << itp->first << ", pnode_t<KeyT, T>.KeyT elem == " << itp->second.key_ << "}\n";
#endif

    return 0;
}

#endif