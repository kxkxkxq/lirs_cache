#ifndef BELADY_HPP
#define BELADY_HPP

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
    enum Cachehit
    {
        miss,
        hit
    };

    template <typename KeyT, typename T> class belady
    {
        size_t cap = 0;
        
        std::deque<data::data_t<KeyT, T>> tdeque;
        std::list<KeyT> blist;
        std::unordered_multimap<KeyT, size_t> hashtable;

        void fill_tdeque_and_hashtable(const std::vector<data::data_t<KeyT, T>>& rqsts);
        auto find_farthest_request(const KeyT& req);
        void rotate_blist_if(const KeyT& req);

public:

        belady(const size_t c, const std::vector<data::data_t<KeyT, T>>& rqsts) : cap(c) {fill_tdeque_and_hashtable(rqsts); }; 
        int proccess_request(const data::data_t<KeyT, T> &dref);
    };
}

template <typename KeyT, typename T> 
void caches::belady<KeyT, T>::fill_tdeque_and_hashtable(const std::vector<data::data_t<KeyT, T>>& rqsts)
{
    size_t m = 0;
    for(auto i = rqsts.begin(), e = rqsts.end(); i != e; ++i, ++m)
    {
        tdeque.push_front(*i);
        hashtable.emplace(std::pair{i->key, m});
    }
}

template <typename KeyT, typename T> 
int caches::belady<KeyT, T>::proccess_request(const data::data_t<KeyT, T> &dref)
{
    auto ht_it = hashtable.find(dref.key);
    assert(ht_it != hashtable.end());
    auto b_it = std::find(blist.begin(), blist.end(), dref.key);

    if(b_it == blist.end())
    {
        if(blist.size() < cap) 
        {
            blist.push_front(ht_it->first);
            hashtable.erase(ht_it);
            return miss;
        }

        auto de_it = find_farthest_request(ht_it->first);
        
        blist.erase(de_it);
        blist.push_front(ht_it->first);
        hashtable.erase(ht_it);

        return miss;

    }

    else
    {
        rotate_blist_if(ht_it->first);
        hashtable.erase(ht_it);
        return hit;
    }

    return 0;
}

template <typename KeyT, typename T> auto caches::belady<KeyT, T>::find_farthest_request(const KeyT& req)
{
    auto it = hashtable.find(req);
    assert(it != hashtable.end());
    size_t offset = it->second;
    
    size_t max_dist = 0;
    auto m_it = blist.end();

    for(auto i = blist.begin(), e = blist.end(); i != e; ++i)
    {
        auto ht_it = hashtable.find(*i);
        if(hashtable.count(*i) == 0) return i;

        if((ht_it->second - offset) > max_dist)
        {
            max_dist = ht_it->second;
            m_it = i;
        }
    }

    assert(m_it != blist.end());
    return m_it;
}

template <typename KeyT, typename T> void caches::belady<KeyT, T>::rotate_blist_if(const KeyT& req)
{
    if(req != blist.front())
    {
        auto it = std::find(blist.begin(), blist.end(), req);
        std::rotate(blist.begin(), it, std::next(it, 1));
    }
}
#endif