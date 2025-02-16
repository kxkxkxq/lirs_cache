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
        const size_t capacity = 0;
    
        using listIter = typename std::list<KeyT>::iterator;
        std::unordered_map<KeyT, listIter> cacheElements;
        std::unordered_map<KeyT, std::deque<size_t>> queryIndexes;
        std::list<KeyT> cacheList;

        template <typename Iter>
        void fill_query_indexes(const Iter begin, const Iter end);
        
        void erase_element(const KeyT& k);
        KeyT& emplace_element(const KeyT& k);
        void delete_farthest_request(const KeyT& k);  //  finds and deletes element that will not be uset for a long time
        
public :

        template <typename Iter> 
        belady(const size_t cap, const Iter begin, const Iter end) : capacity(cap) 
        {
            fill_query_indexes(begin, end);
        };

        bool process_request(const KeyT& k);
    };
}

template <typename KeyT, typename T> template <typename Iter> 
void caches::belady<KeyT, T>::fill_query_indexes(const Iter begin, const Iter end)
{
    size_t indx = 0;
    for(auto i = begin, e = end; i != e; ++i)
    {
        auto iIt = queryIndexes.find(*i);
    
        if(iIt == queryIndexes.end())
            queryIndexes.emplace(*i, std::deque<size_t>{indx});
        else
            iIt->second.emplace_back(indx);
        
        ++indx;
    }  
}

template <typename KeyT, typename T> 
bool caches::belady<KeyT, T>::process_request(const KeyT& k)
{
    auto iIt = queryIndexes.find(k);
    assert(iIt != queryIndexes.end());
    iIt->second.erase(iIt->second.begin());

    auto cIt = cacheElements.find(k);
    if(cIt == cacheElements.end())
    {
        if(iIt->second.empty()) return false;
    
        if(cacheList.size() < capacity)
        {
            emplace_element(k);
            return false;
        }    
        assert(cacheList.size() == capacity);
        
        delete_farthest_request(k);
        emplace_element(k);
        return false;
    }

    return true;
}

template <typename KeyT, typename T> 
void caches::belady<KeyT, T>::delete_farthest_request(const KeyT& k)
{
    auto iIt = queryIndexes.find(k);
    assert(iIt != queryIndexes.end());

    size_t maxDist = 0;
    auto frIt = queryIndexes.end();

    //  find farthest request
    for(auto i : cacheList) 
    {
        auto iIt = queryIndexes.find(i);
        assert(iIt != queryIndexes.end());
        
        if(iIt->second.empty()) 
        {
            frIt = iIt;
            break;
        }
        
        if(iIt->second.front() > maxDist)
        {
            frIt = iIt;
            maxDist = frIt->second.front();
        }
    }
    //  delete it
    erase_element(frIt->first);
}

template <typename KeyT, typename T> 
void caches::belady<KeyT, T>::erase_element(const KeyT& k)
{
    auto cIt = cacheElements.find(k);
    assert(cIt != cacheElements.end());

    cacheList.erase(cIt->second);
 cacheElements.erase(cIt);
}

template <typename KeyT, typename T> 
KeyT& caches::template belady<KeyT, T>::emplace_element(const KeyT& k)
{
    cacheList.emplace_front(k);
 cacheElements.emplace(k, cacheList.begin());
    return cacheList.front();
}