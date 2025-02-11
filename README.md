# caches
Here are presented two implementations of caching aigorithms - [LIRS caching algorithm](https://en.wikipedia.org/wiki/LIRS_caching_algorithm) 
and the [Belady algorithm](https://en.wikipedia.org/wiki/Cache_replacement_policies)

## How to install
create a copy of the repository '''git clone git@github:.com:kxkxkxq/caches.git''' 
go '''cd ./caches/'''

## Build
use '''cmake -DCMAKE_BUILD_TYPE=Release -S . -B build''' and then '''cmake --build build'''

## Run
use '''./build/source/lirs''' to run LIRS algorithm \
use '''./build/source/belady''' to run Belady's algorithm \
use '''./build/tests/test''' to run tests
