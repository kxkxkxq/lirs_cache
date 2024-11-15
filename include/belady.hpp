#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>

namespace caches
{
    template <typename KeyT, typename T = std::string> class belady final
    {
        size_t                  cap = 0;
        const std::list<KeyT>& requests;   //  list of all requests

        using list_iter = typename std::list<KeyT>::iterator;
        std::list<KeyT>                                   blist;  //  cache list 
        std::unordered_multimap<KeyT, size_t>     query_indexes;    //  to find the distance between requests
        std::unordered_multimap<KeyT, list_iter> cache_elements;   //   to check if an elem is in cache

        auto find_farthest_request(const KeyT& kref);
        void                    fill_query_indexes();
        
public :

        belady(const size_t c, const std::list<KeyT>& r) : cap(c), requests(r) {fill_query_indexes();}; 
        bool process_request(const KeyT &kref);
    };
}

template <typename KeyT, typename T> void caches::belady<KeyT, T>::fill_query_indexes()
{
    size_t indx = 0;
    for(auto i = requests.begin(), e = requests.end(); i != e; ++i, ++indx)
        query_indexes.emplace(*i, indx);
}

template <typename KeyT, typename T> 
bool caches::belady<KeyT, T>::process_request(const KeyT& kref)
{
    auto i_it = query_indexes.find(kref);
    assert(i_it != query_indexes.end());
    auto c_it  = cache_elements.find(kref);

    if(c_it != cache_elements.end())
    {
        query_indexes.erase(i_it);
        return true;
    }
    
    if(blist.size() < cap)
    {
        blist.emplace_front(kref);  //  push front request "kref"
        cache_elements.emplace(kref, blist.begin());
        query_indexes.erase(i_it);

        return false;
    }

    auto fr_it = find_farthest_request(kref);
    if(fr_it != blist.end())
    {
        auto cfr_it = cache_elements.find(*fr_it);
        blist.erase(fr_it); //  pop back fathest request  
        cache_elements.erase(cfr_it);

        blist.emplace_front(kref);  
        cache_elements.emplace(kref, blist.begin());
    }
    return false;
}

template <typename KeyT, typename T> 
auto caches::belady<KeyT, T>::find_farthest_request(const KeyT& kref)
{
    auto kref_it = query_indexes.find(kref);
    assert(kref_it != query_indexes.end());
    size_t offset = kref_it->second;
    
    query_indexes.erase(kref_it);
    if(kref_it == query_indexes.end()) return blist.end(); //  if this request is the only one
        
    size_t max_dist = 0;
    auto fr_it = blist.end();

    for(auto i = blist.begin(), e = blist.end(); i != e; ++i)
    {
        auto i_it = query_indexes.find(*i);
        if(i_it == query_indexes.end()) return i; // if it is the last request with such key

        if((i_it->second - offset) > max_dist)
        {
            max_dist = i_it->second;
            fr_it = i;
        }
    }

    assert(fr_it != blist.end());
    return fr_it;
}