#include <iostream>
#include <random>
#include <iomanip>      // std::setprecision
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <chrono>
#include "MySemaphore.h"



using namespace std;
using namespace std::chrono;

map<string,int>commodityToIndex;
void initializeMap(){
    commodityToIndex["ALUMINUM"]=0;
    commodityToIndex["COPPER"]=1;
    commodityToIndex["COTTON"]=2;
    commodityToIndex["CRUDEOIL"]=3;
    commodityToIndex["GOLD"]=4;
    commodityToIndex["LEAD"]=5;
    commodityToIndex["MENTHAOIL"]=6;
    commodityToIndex["NATURALGAS"]=7;
    commodityToIndex["NICKEL"]=8;
    commodityToIndex["SILVER"]=9;
    commodityToIndex["ZINC"]=10;
}

int main(int argc, char** argv){
    initializeMap();
    string commodityName = argv[1];
    double commodityMean = stoi(argv[2]);
    double standardDeviation = stoi(argv[3]);
    double timeOut = stoi(argv[4]);
    int bufferSize = stoi(argv[5]);
    default_random_engine randomVariablegenerator;
    normal_distribution<double> normalDistribution(commodityMean,standardDeviation);
    int commodityIndex = commodityToIndex[commodityName];

    key_t IPC_key = ftok("interprocesscommunication",65);
    int sharedMemoryID = shmget(IPC_key,(bufferSize*16)+8,0666);
    void* sharedMemory= shmat(sharedMemoryID,NULL,0);
    int* currentSize = (int *) sharedMemory;
    int *currentItem = (int *) sharedMemory+4;

    struct sembuf sem_op;
    int semaphoreSetId =semget(IPC_key,3,0666);
    pair<int,double>*array = (pair<int,double>* )sharedMemory+8;

    while(true){
        double number = normalDistribution(randomVariablegenerator);
        struct timespec start;
        time_t now = system_clock::to_time_t(system_clock::now());
        std::cerr<<"["<<put_time(localtime(&now),"%M/%d/%Y %H:%M:%S")<<"] "<<commodityName<<": generating a new value "<<setprecision(2)<<fixed<< number<<endl;;
        sem_op = waitSemaphore(1);
        semop(semaphoreSetId,&sem_op,1);
        std::cerr<<"trying to get mutex on shared buffer"<<endl;
        sem_op = waitSemaphore(0);
        semop(semaphoreSetId,&sem_op,1);
        std::cerr<<"placing "<<number<<" on shared buffer"<<endl;
        array[*currentSize].first= commodityIndex;
        array[*currentSize].second=number;
        *currentSize = (*currentSize+1)%bufferSize;
        sem_op = signalSemaphore(0);
        semop(semaphoreSetId,&sem_op,1);
        sem_op = signalSemaphore(2);
        semop(semaphoreSetId,&sem_op,1);
        std::cerr<<"sleeping for "<<timeOut<<" ms"<<endl;
        usleep(timeOut*1000);
    }
    
    return 0;
}