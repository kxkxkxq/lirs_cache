#ifndef DATA_HPP
#define DATA_HPP

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

namespace data
{
    template <typename KeyT, typename T> struct data_t
    {
        KeyT key;  
        T rqst;    

        data_t(const KeyT &k, const T &r) : key(k), rqst(r) {};
        data_t(const KeyT &k)               : key(k) {};
    };
    
    template <typename KeyT, typename T> struct Data
    {
        std::vector<data_t<KeyT, T>> requests; 
        size_t csize = 0;
        
        bool input_data();
    };
}

template <typename KeyT, typename T> bool data::Data<KeyT, T>::input_data()
{
    size_t n = 0;
    std::cin >> csize >> n;

    if(!std::cin.good())
    {
        std::cerr << "Input error\n";
        return false;
    }

    requests.reserve(n);
    
    for(size_t i = 0; i < n; ++i)
    {
        KeyT k;
        std::cin >> k;

        if((!std::cin.good()) || (!std::cin.eof()))
        {
            std::cerr << "Input error\n";
            return false;
        }
 
        requests.emplace_back(data_t<KeyT, T>{k});
    }
    return true;
}

#endif