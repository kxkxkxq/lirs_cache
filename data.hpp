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
        
        void input_data();
        void fill_requests_with_data();
        void printf_input_data();
    };

    inline void print_nhits(const unsigned n) {std::cout << n << '\n'; };
}

template <typename KeyT, typename T> void data::Data<KeyT, T>::input_data()
{
    size_t n = 0;
    std::cin >> csize >> n;
    assert(std::cin.good());
    
    requests.reserve(n);
    
    for(size_t i = 0; i < n; ++i)
    {
        KeyT k;
        std::cin >> k;
        assert(std::cin.good() || std::cin.eof());
 
        const data_t<KeyT, T> d = requests.emplace_back(data_t<KeyT, T>{k});
        assert(d.key == k);
    }
}

template <typename KeyT, typename T> void data::Data<KeyT, T>::printf_input_data()
{
    std::cout << "csize == " << csize << "\nnrequests == " << requests.size() << '\n';
    
    for(auto i = requests.begin(); i != requests.end(); ++i)
    {
        const KeyT &k = *i.key;
        auto is_suitable_key = [&k] (const data_t<KeyT, T>& d) {return d.key == k; };

        auto it = std::find_if(requests.begin(), requests.end(), is_suitable_key);  //write .at() for requests
        std::cout << "requests[" << i << "].key == " << it->key << "\n";
    }
}

#endif