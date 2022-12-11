#include <iostream>
#include <random>
#include <iomanip>      // std::setprecision
#include <sys/types.h>
#include <sys/shm.h>
#include <bits/stdc++.h>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>


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
    int sharedMemoryID = shmget(IPC_key,bufferSize+12+32+32+32,0666);
    void* sharedMemory= shmat(sharedMemoryID,NULL,0);
    int* currentSize = (int *) sharedMemory+4;
    int *currentItem = (int *) sharedMemory+8;
    sem_t* empty = (sem_t *) sharedMemory+12; //number of empty slots
    sem_t* full = (sem_t *) sharedMemory+12+32;//number of full slots
    sem_t* mutex = (sem_t *) sharedMemory+12+32+32;
    
    pair<int,double>*array = (pair<int,double>* )sharedMemory+12+32+32+32;

    while(true){
        double number = normalDistribution(randomVariablegenerator);
        struct timespec start;
        time_t now = system_clock::to_time_t(system_clock::now());
        std::cerr<<"["<<put_time(localtime(&now),"%M/%d/%Y %H:%M:%S")<<"] "<<commodityName<<": generating a new value "<<setprecision(2)<<fixed<< number<<endl;;
        sem_wait(empty);
        std::cerr<<"trying to get mutex on shared buffer"<<endl;
        sem_wait(mutex);
        std::cerr<<"placing "<<number<<" on shared buffer"<<endl;
        array[*currentSize].first= commodityIndex;
        array[*currentSize].second=number;
        *currentSize = (*currentSize+1)%bufferSize;
        sem_post(mutex);
        sem_post(full);
        std::cerr<<"sleeping for "<<timeOut<<" ms"<<endl;
        sleep(timeOut/1000);
    }
    
    return 0;
}