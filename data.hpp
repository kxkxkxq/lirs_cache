#ifndef DATA_HPP
#define DATA_HPP

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

namespace data
{
    template <typename KeyT, typename T> struct data_t
    {
        KeyT key_;  
        T rqst_;    

        data_t(const KeyT key, const T rqst) : key_(key), rqst_(rqst) {};
        data_t(const KeyT key)               : key_(key) {};
    };
    
    template <typename KeyT, typename T> struct Data
    {
        std::vector<data_t<KeyT, T>> requests; 
    public:
        
        unsigned csize = 0, nrequests = 0;
        
        void input_data();
        void fill_requests_with_data();
        void printf_input_data();
    };

    inline void print_nhits(const unsigned n) {std::cout << n << '\n'; };
}

template <typename KeyT, typename T> void data::Data<KeyT, T>::input_data()
{
    std::cin >> csize >> nrequests;
    requests.reserve(nrequests);
    
    for(size_t i = 0; i < nrequests; ++i)
    {
        KeyT key;
        std::cin >> key;
        requests.emplace_back(data_t<KeyT, T>{key});
    }
}

template <typename KeyT, typename T> void data::Data<KeyT, T>::printf_input_data()
{
    std::cout << "csize == " << csize << "\nnrequests == " << nrequests << '\n';
    
    for(unsigned i = 0; i < nrequests; ++i)
    {
        data_t<KeyT, T> data = requests[i];
        KeyT k = data.key_;
        auto is_suitable_key = [k] (data_t<KeyT, T>& d) {return d.key_ == k; };

        auto it = std::find_if(requests.begin(), requests.end(), is_suitable_key);  //write .at() for requests
        std::cerr << "requests[" << i << "].key == " << it->key_ << "\n";
    }
}

#endif