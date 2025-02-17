#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <list>
#include <queue>                
#include <string>         
#include <unordered_map>
#include <vector>

namespace caches
{
    template <typename KeyT, typename P = int> class lirs final
    {
        enum Status
        {
            lir    ,    //  lir element
            hirr   ,    //  resident hir element
            hirnr  ,    //  non-resident hir element
        };

        struct list_t
        {
            const size_t cap = 0; 
            std::list<KeyT> list;

            enum CSizes {hsize = 1, lsize = 99 };   //  percentage of hirs and lirs lists sizes 
            
            list_t(const size_t cs) : cap((cs * hsize / 100) + 1) {};   //  ctor for lHirs
            list_t(const size_t cs, const size_t hs) : cap(cs - hs) {}; //  ctor for lLirs
        };

        const size_t cSize = 0;
        std::function<P(KeyT)> slow_get_page;   //  slow func to get data if it isn't in the cache

        list_t lHirs;  //  part of cache for hirs elements (approximately 1% of cache size)
        list_t lLirs;  //  part of cache for lirs elements (approximately 99% of cache size)
        std::list<KeyT> queryQueue; 

        struct mnode_t
        {
            Status status = hirr;
            const P page;   
            
            using listIter = typename std::list<KeyT>::iterator;
            listIter cIt; //  iterator to cache element
            listIter qIt; //  iterator to query queue element

            mnode_t(const P p, listIter q) : page(p), qIt(q) {};
        };
        std::unordered_map<KeyT, mnode_t> hashTable;  
        
        void push_new_request(const KeyT& k);   //  pushes new request to queryQueue and hashTable
        void swap_hir_and_lir(const KeyT& k);   //  swaps last lir  and certain hirr
        
        void rotate_queue_if(const KeyT& k);    //  move certain element to the beginning of the queryQueue 
        void queue_push_front(const KeyT& k);
        void queue_prunning();  // pops all unnecessary hirr and hirhr elements

        void list_pop_back(std::list<KeyT>& list);
        void list_push_front(std::list<KeyT>& list, const KeyT& k);

public :

        template <typename SlowFunc>
        explicit lirs(const size_t sz, SlowFunc func) : cSize(sz), 
                                                        slow_get_page(func),
                                                        lHirs(sz), 
                                                        lLirs(sz, lHirs.cap) {};
        
        bool process_request(const KeyT& k);

        const size_t lhirs_cap() const {return lHirs.cap; };
        const size_t lLirs_cap() const {return lLirs.cap; };
    };
}

template <typename KeyT, typename P> 
bool caches::lirs<KeyT, P>::process_request(const KeyT& k)
{     
    auto htIt = hashTable.find(k);
    if(htIt == hashTable.end())  //  new request
    {
        push_new_request(k);
        htIt = hashTable.find(k);
        assert(htIt != hashTable.end());
        assert(htIt->second.qIt != queryQueue.end());

        if(lLirs.list.size() < lLirs.cap)
        {
            htIt->second.status = lir;
            list_push_front(lLirs.list, k);
            return false;
        }

        if(lHirs.list.size() == lHirs.cap)
            list_pop_back(lHirs.list);
       
        list_push_front(lHirs.list, k);
        return false;
    }

    assert(htIt != hashTable.end());    
    switch(htIt->second.status)
    {
        case lir   :    //  accessing lir element
                        rotate_queue_if(htIt->first);
                        list_push_front(lLirs.list, htIt->first);
                        queue_prunning();
                        return true;
        case hirr  :    //  accessing hir resident element             
                        if(htIt->second.qIt != queryQueue.end()) 
                        {
                            rotate_queue_if(htIt->first);  //  if resident hir element 
                            swap_hir_and_lir(htIt->first); //  was not in query queue
                            queue_prunning();              //  it remains in this status
                        }
                        else
                            queue_push_front(htIt->first);
                        return true;
        case hirnr :    //  accessing hir non-resident element              
                        list_pop_back(lHirs.list);
                        list_push_front(lHirs.list, htIt->first);
                        
                        if(htIt->second.qIt == queryQueue.end())
                        {
                            queue_push_front(htIt->first);
                            return false;
                        }

                        rotate_queue_if(htIt->first);
                        swap_hir_and_lir(htIt->first);
                        queue_prunning();
                        return false;
    }
    return false;
}

template <typename KeyT, typename P>
void caches::lirs<KeyT, P>::push_new_request(const KeyT& k)
{
    P page = slow_get_page(k);
    
    queryQueue.push_front(k);
    hashTable.emplace(k, std::move(mnode_t{slow_get_page(k), queryQueue.begin()}));
                                                             
    auto i = hashTable.find(k);
    assert(i != hashTable.end());
    assert(*i->second.qIt == queryQueue.front());
    i->second.cIt = lHirs.list.end();  //  cIt == lHirs.list.end() means
                                       //  there's no key k in cache 
}

template <typename KeyT, typename P> 
void caches::lirs<KeyT, P>::queue_push_front(const KeyT& k)
{
    auto htIt = hashTable.find(k);
    assert(htIt != hashTable.end());
    assert(htIt->second.qIt == queryQueue.end());

    queryQueue.push_front(htIt->first);
    htIt->second.qIt = queryQueue.begin(); 
    assert(*htIt->second.qIt == k);
}

template <typename KeyT, typename P> 
void caches::lirs<KeyT, P>::list_push_front(std::list<KeyT>& list, const KeyT& k)
{
    auto htIt = hashTable.find(k);
    assert(htIt != hashTable.end());

    if(htIt->second.cIt == lHirs.list.end())  
    {                                           
        list.push_front(k);
        htIt->second.cIt = list.begin();
        assert(htIt->second.cIt != list.end());
    }
    else
        list.splice(list.begin(), list, htIt->second.cIt);
}

template <typename KeyT, typename P> 
void caches::lirs<KeyT, P>::list_pop_back(std::list<KeyT>& list)
{
    auto htIt = hashTable.find(list.back());
    assert(htIt != hashTable.end());
    assert(htIt->second.cIt != lHirs.list.end());

    htIt->second.status = hirnr;
    list.pop_back();
    htIt->second.cIt = lHirs.list.end();  
}

template <typename KeyT, typename P> void caches::lirs<KeyT, P>::rotate_queue_if(const KeyT& k)
{
    if(k != queryQueue.front())
    {
        auto htIt = hashTable.find(k);
        assert(htIt != hashTable.end());
        assert(htIt->second.qIt != queryQueue.end());
        queryQueue.splice(queryQueue.begin(), queryQueue, htIt->second.qIt); 
    }
}

template <typename KeyT, typename P> void caches::lirs<KeyT, P>::queue_prunning()
{
    for(auto i : queryQueue) 
    {
        auto htIt = hashTable.find(i);
        assert(htIt != hashTable.end());
        assert(htIt->first == i);
        assert(htIt->second.qIt != queryQueue.end());

        switch(htIt->second.status)
        {
            case lir   :
                            return;
            case hirr  :    
            case hirnr :  
                            queryQueue.pop_back();
                            htIt->second.qIt = queryQueue.end();
                            break;
        }
    }
}

template <typename KeyT, typename P> void caches::lirs<KeyT, P>::swap_hir_and_lir(const KeyT& k)
{
    auto htIth = hashTable.find(k);
    assert(!lLirs.list.empty());
    auto htItl = hashTable.find(lLirs.list.back());

    assert(htIth != hashTable.end());
    assert(htItl != hashTable.end());
    assert(htIth->second.cIt != lHirs.list.end());
    assert(htItl->second.cIt != lHirs.list.end());
    
    htIth->second.status = lir;
    htItl->second.status = hirr;

    lLirs.list.splice(lLirs.list.begin(), lHirs.list, htIth->second.cIt);
    lHirs.list.splice(lHirs.list.begin(), lLirs.list, htItl->second.cIt);
}