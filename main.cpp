#include <random>
#include <iostream>
#include <sys/time.h>
#include "mergeSort.h"

int main(){
    std::random_device rd;
    std::mt19937  gen(rd());

    unsigned* toSort=(unsigned*)malloc(1LLU<<34);
    unsigned* toSort2=(unsigned*)malloc(1LLU<<34);
    std::cout<<toSort<<std::endl;
    for(long long unsigned int index=0;index<(1LLU<<32);index++){
        *(toSort+index)=gen();
        *(toSort2+index)=*(toSort+index);
    }

    std::cout<<"SORT BEGIN"<<std::endl;

    struct timeval start,end;
    gettimeofday(&start,NULL);
    mergeSort(toSort,1LLU<<32,32);
    gettimeofday(&end,NULL);
    std::cout<<"CONSUME ";
    std::cout<<(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec)<<" MICROSECOND"<<std::endl;

    for(long long unsigned int index=0;(index+1)<(1LLU<<32);index++){
        if(toSort[index]>toSort[index+1]){
            std::cout<<"WRONG RANGE AT "<<index<<" AND "<<index+1<<": "<<toSort[index]<<", "<<toSort[index+1]
                     <<std::endl;
            return 1;
        }
    }
    std::cout<<"CHECK PASS"<<std::endl;
    gettimeofday(&start,NULL);
    std::sort(toSort2,toSort2+(1LLU<<32));
    gettimeofday(&end,NULL);
    std::cout<<"CONSUME ";
    std::cout<<(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec)<<" MICROSECOND"<<std::endl;

    return 0;
}