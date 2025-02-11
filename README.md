# caches
Here are presented two implementations of caching aigorithms - [LIRS caching algorithm](https://en.wikipedia.org/wiki/LIRS_caching_algorithm) 
and the [Belady algorithm](https://en.wikipedia.org/wiki/Cache_replacement_policies)

## How to install
use 
```bush
git clone git@github:.com:kxkxkxq/caches.git
``` 
and then 
```bush
cd ./caches/
```

## How to build
use 
```bush
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
``` 
and then 
```bush
cmake --build build
```

## How to run
to run LIRS algorithm use 
```bush
./build/source/lirs
```
to run Belady's algorithm use 
```bush
./build/source/belady
```
to run tests use 
```bush
./build/tests/test
```
