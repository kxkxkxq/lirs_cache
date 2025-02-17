#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>

namespace caches
{
    template <typename KeyT, typename P = int> class belady final
    {
        const size_t capacity = 0;
        std::function<P(KeyT)> slow_get_page;   //  slow func to get data if it isn't in the cache

        using listIter = typename std::list<KeyT>::iterator;
        class PNode
        {
            const P page;
            listIter ElemIter;
        
        public:

            PNode(const P p, listIter elem) : page(p), ElemIter(elem) {};
            listIter get_list_iter() {return ElemIter;};
            const P get_page() const noexcept {return page;};
        };
    
        std::unordered_map<KeyT, PNode> cacheElements;
        std::unordered_map<KeyT, std::deque<size_t>> queryIndexes;
        std::list<KeyT> cacheList;

        template <typename Iter>
        void fill_query_indexes(const Iter begin, const Iter end);
        
        void erase_element(const KeyT& k);
        KeyT& emplace_element(const KeyT& k);
        void delete_farthest_request(const KeyT& k);  // finds and deletes element 
                                                      // that will not be uset for a long time
        
public :

        template <typename Iter, typename SlowFunc> 
        belady( const size_t cap, 
                SlowFunc func, 
                const Iter begin, 
                const Iter end) : capacity(cap), slow_get_page(func) 
        { 
            fill_query_indexes(begin, end);
        };

        bool process_request(const KeyT& k);
    };
}

template <typename KeyT, typename P> template <typename Iter> 
void caches::belady<KeyT, P>::fill_query_indexes(const Iter begin, const Iter end)
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

template <typename KeyT, typename P> 
bool caches::belady<KeyT, P>::process_request(const KeyT& k)
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

template <typename KeyT, typename P> 
void caches::belady<KeyT, P>::delete_farthest_request(const KeyT& k)
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

template <typename KeyT, typename P> 
void caches::belady<KeyT, P>::erase_element(const KeyT& k)
{
    auto cIt = cacheElements.find(k);
    assert(cIt != cacheElements.end());

    cacheList.erase(cIt->second.get_list_iter());
    cacheElements.erase(cIt);
}

template <typename KeyT, typename P> 
KeyT& caches::template belady<KeyT, P>::emplace_element(const KeyT& k)
{
    cacheList.emplace_front(k);
    cacheElements.emplace(k, std::move(PNode{slow_get_page(k), cacheList.begin()})); 
    return cacheList.front();
}