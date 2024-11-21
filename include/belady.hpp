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
        size_t cap = 0;
    
        using list_iter = typename std::list<KeyT>::iterator;
        std::unordered_map<KeyT, list_iter> cache_elements;
        std::unordered_map<KeyT, std::deque<size_t>> query_indexes;
        std::list<KeyT> cache_list;

        void delete_farthest_request(const KeyT& k);  //  finds and deletes element that will not be uset for a long time
        void fill_query_indexes(const std::list<KeyT>& r);

        void erase_element(const KeyT& k);
        KeyT& emplace_element(const KeyT& k);

public :

        belady(const size_t c, const std::list<KeyT>& r) : cap(c) {fill_query_indexes(r); };
        bool process_request(const KeyT& k);
    };
}

template <typename KeyT, typename T> 
void caches::belady<KeyT, T>::fill_query_indexes(const std::list<KeyT>& r)
{
    size_t indx = 0;
    for(auto i = r.begin(), e = r.end(); i != e; ++i, ++indx)
    {
        auto i_it = query_indexes.find(*i);
    
        if(i_it == query_indexes.end())
            query_indexes.emplace(*i, std::deque<size_t>{indx});
        else
            i_it->second.emplace_back(indx);
    }  
}

template <typename KeyT, typename T> 
bool caches::belady<KeyT, T>::process_request(const KeyT& k)
{
    auto i_it = query_indexes.find(k);
    assert(i_it != query_indexes.end());
    i_it->second.erase(i_it->second.begin());

    auto c_it = cache_elements.find(k);
    if(c_it == cache_elements.end())
    {
        if(i_it->second.empty()) return false;
    
        if(cache_list.size() < cap)
        {
            emplace_element(k);
            return false;
        }    
        assert(cache_list.size() == cap);
        
        delete_farthest_request(k);
        emplace_element(k);
        return false;
    }

    return true;
}

template <typename KeyT, typename T> 
void caches::belady<KeyT, T>::delete_farthest_request(const KeyT& k)
{
    auto i_it = query_indexes.find(k);
    assert(i_it != query_indexes.end());

    size_t max_dist = 0;
    auto fr_it = query_indexes.end();

    //  find farthest request
    for(auto i = cache_list.begin(), e = cache_list.end(); i != e; ++i) 
    {
        auto i_it = query_indexes.find(*i);
        assert(i_it != query_indexes.end());
        
        if(i_it->second.empty()) 
        {
            fr_it = i_it;
            break;
        }
        
        if(i_it->second.front() > max_dist)
        {
            fr_it = i_it;
            max_dist = fr_it->second.front();
        }
    }
    //  delete it
    erase_element(fr_it->first);
}

template <typename KeyT, typename T> 
void caches::belady<KeyT, T>::erase_element(const KeyT& k)
{
    auto c_it = cache_elements.find(k);
    assert(c_it != cache_elements.end());

    cache_list.erase(c_it->second);
    cache_elements.erase(c_it);
}

template <typename KeyT, typename T> 
KeyT& caches::belady<KeyT, T>::emplace_element(const KeyT& k)
{
    cache_list.emplace_front(k);
    cache_elements.emplace(k, cache_list.begin());
    return cache_list.front();
}