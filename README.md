# parallel-Merge-Sort-cpp
Multithreading merge sort using c++

# Usage

```bash
g++ -c main.cpp
g++ -c mergeSort.cpp
g++ -o msp main.o mergeSort.o
./msp
```

# Result

Take $9964674$ microseconds sorting a 16GiB `unsigned` array 

in 32 threads, 14 times faster than `std::sort()` 

which take about $1388802321$ microseconds.

![telegram-cloud-document-5-6235757476831038115](https://github.com/PandoraEartha/parallel-Merge-Sort-cpp/assets/98176983/34f7bff8-578e-479b-9121-159e91ebe682)
