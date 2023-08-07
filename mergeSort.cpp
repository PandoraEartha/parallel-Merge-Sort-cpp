#include <iostream>
#include <pthread.h>
#include <cstring>
#include <algorithm>

unsigned threadI=0;
unsigned* threadFinishCount;
unsigned allThreadFinishCount=0;

pthread_mutex_t getThreadID;
pthread_mutex_t putThreadFinish;
pthread_mutex_t allThreadFinish;

class Parameter{
    public:
        unsigned* toSort;
        unsigned* vector1;
        unsigned* vector2;
        long long unsigned int length;
        long long unsigned int actualLength;
        unsigned threadNumber;
        bool move2to1Class;
};

Parameter parameters;

bool stageCheck(const unsigned* vector,const long long unsigned int* myWorkingRange){
    for(long long unsigned int index=myWorkingRange[0];(index+1)<myWorkingRange[1];index++){
        if(vector[index]>vector[index+1]){
            std::cout<<"CHECK NOT PASS: "<<index<<std::endl;
            return false;
        }
    }
    return true;
}

void* mergeSortThread(){
    long long unsigned int length=parameters.length;
    long long unsigned int actualLength=parameters.actualLength;
    unsigned threadNumber=parameters.threadNumber;
    unsigned* toSort=parameters.toSort;
    unsigned* vector1=parameters.vector1;
    unsigned* vector2=parameters.vector2;

    pthread_mutex_lock(&getThreadID);
    unsigned myThreadID=threadI;
    threadI++;
    pthread_mutex_unlock(&getThreadID);

    long long unsigned int myWorkingRange[2];
    myWorkingRange[0]=myThreadID*length/threadNumber;
    myWorkingRange[1]=(myThreadID+1)*length/threadNumber;

    memset((void*)(vector1+myWorkingRange[0]),255,sizeof(long long unsigned int)*(myWorkingRange[1]-myWorkingRange[0]));
    for(long long unsigned int copyIndex=myWorkingRange[0];copyIndex<myWorkingRange[1];copyIndex++){
        if(copyIndex<actualLength){
            vector2[copyIndex]=toSort[copyIndex];
        }else{
            vector2[copyIndex]=~0;
        }
    }
    long long unsigned int span=1LLU;
    bool move2to1=true;
    long long unsigned int index1=myWorkingRange[0];
    long long unsigned int operationCount1;
    long long unsigned int index2=myWorkingRange[0];
    long long unsigned int operationCount2;
    long long unsigned int indexToPut=myWorkingRange[0];

    // STAGE 1
    unsigned layerIndex=0;
    while((length/span)>threadNumber){
        for(long long unsigned int index=myWorkingRange[0];index<myWorkingRange[1];index+=(span<<1)){
            index1=index;
            index2=index+span;
            indexToPut=index;
            operationCount1=0;
            operationCount2=0;
            while((operationCount1<span)&&(operationCount2<span)){
                if(move2to1){
                    if(vector2[index1]>vector2[index2]){
                        vector1[indexToPut]=vector2[index2];
                        operationCount2++;
                        index2++;
                    }else{
                        vector1[indexToPut]=vector2[index1];
                        operationCount1++;
                        index1++;
                    }
                }else{
                    if(vector1[index1]>vector1[index2]){
                        vector2[indexToPut]=vector1[index2];
                        operationCount2++;
                        index2++;
                    }else{
                        vector2[indexToPut]=vector1[index1];
                        operationCount1++;
                        index1++;
                    }
                }
                indexToPut++;
            }
            if(operationCount1==span){
                while((operationCount1+operationCount2)<(span<<1)){
                    if(move2to1){
                        vector1[indexToPut]=vector2[index2];
                    }else{
                        vector2[indexToPut]=vector1[index2];
                    }
                    index2++;
                    operationCount2++;
                    indexToPut++;
                }
            }else{
                while((operationCount1+operationCount2)<(span<<1)){
                    if(move2to1){
                        vector1[indexToPut]=vector2[index1];
                    }else{
                        vector2[indexToPut]=vector1[index1];
                    }
                    index1++;
                    operationCount1++;
                    indexToPut++;
                }
            }
        }
        move2to1=!move2to1;
        span=span<<1;
        layerIndex++;
        pthread_mutex_lock(&putThreadFinish);
        threadFinishCount[layerIndex]++;
        pthread_mutex_unlock(&putThreadFinish);
    }
    bool minThread;
    if(myThreadID&1){
        minThread=false;
    }else{
        minThread=true;
    }
//    if(!move2to1){
//        if(!(stageCheck(vector1,myWorkingRange))){
//            std::cout<<"STAGE NOT PASS: "<<myThreadID<<std::endl;
//            return NULL;
//        }
//    }else{
//        if(!(stageCheck(vector2,myWorkingRange))){
//            std::cout<<"STAGE NOT PASS: "<<myThreadID<<std::endl;
//            return NULL;
//        }
//    }
    while(threadFinishCount[layerIndex]!=threadNumber){
    }

    // STAGE 2
    long long unsigned int indexStart;
    while((length/span)>myThreadID){
        long long unsigned int operationCount=0;
        if(minThread){
            // MIN MERGE
            indexStart=span*myThreadID;
            index1=indexStart;
            index2=indexStart+span;
            indexToPut=indexStart;
            while(operationCount<=span){
                if(move2to1){
                    if(vector2[index1]<vector2[index2]){
                        vector1[indexToPut]=vector2[index1];
                        index1++;
                    }else{
                        vector1[indexToPut]=vector2[index2];
                        index2++;
                    }
                }else{
                    if(vector1[index1]<vector1[index2]){
                        vector2[indexToPut]=vector1[index1];
                        index1++;
                    }else{
                        vector2[indexToPut]=vector1[index2];
                        index2++;
                    }
                }
                indexToPut++;
                operationCount++;
            }
        }else{
            // MAX MERGE
            indexStart=span*myThreadID;
            index1=indexStart-1;
            index2=indexStart+span-1;
            indexToPut=indexStart+span-1;
            while(operationCount<=span){
                if(move2to1){
                    if(vector2[index1]>vector2[index2]){
                        vector1[indexToPut]=vector2[index1];
                        index1--;
                    }else{
                        vector1[indexToPut]=vector2[index2];
                        index2--;
                    }
                }else{
                    if(vector1[index1]>vector1[index2]){
                        vector2[indexToPut]=vector1[index1];
                        index1--;
                    }else{
                        vector2[indexToPut]=vector1[index2];
                        index2--;
                    }
                }
                indexToPut--;
                operationCount++;
            }
        }

        move2to1=!move2to1;
        layerIndex++;
        pthread_mutex_lock(&putThreadFinish);
        threadFinishCount[layerIndex]++;
        pthread_mutex_unlock(&putThreadFinish);
        while(threadFinishCount[layerIndex]!=(length/span)){
        }
        span=span<<1;
        if(span==length){
            break;
        }
    }
    if(!myThreadID){
        parameters.move2to1Class=move2to1;
    }
    pthread_mutex_lock(&allThreadFinish);
    allThreadFinishCount++;
    pthread_mutex_unlock(&allThreadFinish);
    while(allThreadFinishCount!=threadNumber){
    }
    move2to1=parameters.move2to1Class;
    if(!move2to1){
        for(long long unsigned int index=myWorkingRange[0];index<myWorkingRange[1];index++){
            toSort[index]=vector1[index];
            if(index==actualLength){
                break;
            }
        }
    }else{
        for(long long unsigned int index=myWorkingRange[0];index<myWorkingRange[1];index++){
            toSort[index]=vector2[index];
            if(index==actualLength){
                break;
            }
        }
    }
    return NULL;
}


void mergeSort(unsigned* toSort,long long unsigned int length,unsigned threadNumber){
    long long unsigned int lengthPower2=1LLU;
    while(!(lengthPower2&length)){
        lengthPower2=lengthPower2<<1;
    }
    if(lengthPower2^length){
        lengthPower2=1LLU<<63;
        while(!(lengthPower2&length)){
            lengthPower2=lengthPower2>>1;
        }
        lengthPower2=lengthPower2<<1;
    }
    unsigned moveCount=0;
    for(long long unsigned int bit1=1LLU;(bit1^lengthPower2);bit1=bit1<<1){
        moveCount++;
    }
    threadFinishCount=(unsigned*)malloc(sizeof(unsigned)*moveCount);
    memset(threadFinishCount,0,sizeof(unsigned)*moveCount);

    unsigned threadNumberInFunction;
    if((lengthPower2>>10)>>5){
        unsigned threadNumberSelect[11]={1,2,4,8,16,32,64,128,256,512,1024};
        // indexTNS, index of threadNumberSelect
        for(unsigned eachThreadNumber: threadNumberSelect){
            if(eachThreadNumber>threadNumber){
                break;
            }
            threadNumberInFunction=eachThreadNumber;
        }
    }else{
        threadNumberInFunction=1;
    }

    unsigned* vector1=(unsigned*)malloc(sizeof(unsigned)*lengthPower2);
    unsigned* vector2=(unsigned*)malloc(sizeof(unsigned)*lengthPower2);

    parameters.toSort=toSort;
    parameters.threadNumber=threadNumberInFunction;
    parameters.length=lengthPower2;
    parameters.actualLength=length;
    parameters.vector1=vector1;
    parameters.vector2=vector2;

    pthread_mutex_init(&getThreadID,NULL);
    pthread_mutex_init(&putThreadFinish,NULL);
    pthread_mutex_init(&allThreadFinish,NULL);

    pthread_t* threadPool=(pthread_t*)malloc(sizeof(pthread_t)*threadNumberInFunction);
    for(unsigned threadIndex=0;threadIndex<threadNumberInFunction;threadIndex++){
        pthread_t currentThread;
        pthread_create(&currentThread,NULL,reinterpret_cast<void*(*)(void*)>(mergeSortThread),NULL);
        threadPool[threadIndex]=currentThread;
    }

    for(unsigned threadIndex=0;threadIndex<threadNumberInFunction;threadIndex++){
        pthread_join(threadPool[threadIndex],NULL);
    }
    pthread_mutex_destroy(&getThreadID);
    pthread_mutex_destroy(&putThreadFinish);
    pthread_mutex_destroy(&allThreadFinish);
    free(parameters.vector1);
    free(parameters.vector2);
}

