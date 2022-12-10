#include <iostream>
#include <random>
#include <iomanip>      // std::setprecision
#include <sys/types.h>
#include <sys/shm.h>
#include <bits/stdc++.h>
#include <semaphore.h>
#include <unistd.h>


using namespace std;

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
void initializeSharedMemory(){

}

int main(int argc, char** argv){
    initializeMap();
    string commodityName = argv[1];
    double commodityMean = stoi(argv[2]);
    double standardDeviation = stoi(argv[3]);
    double timeOut = stoi(argv[4]);

    default_random_engine randomVariablegenerator;
    normal_distribution<double> normalDistribution(commodityMean,standardDeviation);
    int commodityIndex = commodityToIndex[commodityName];

    key_t IPC_key = ftok("interprocesscommunication",65);
    int sharedMemoryID = shmget(IPC_key,1024,0666);
    void* sharedMemory= shmat(sharedMemoryID,NULL,0);
    int* bufferSize = (int *) sharedMemory;
    int* currentSize = (int *) sharedMemory+4;
    int *currentItem = (int *) sharedMemory+8;
    sem_t* empty = (sem_t *) sharedMemory+12; //number of empty slots
    sem_t* full = (sem_t *) sharedMemory+12+32;//number of full slots
    sem_t* mutex = (sem_t *) sharedMemory+12+32+32;
    
    pair<int,double>*array = (pair<int,double>* )sharedMemory+12+32+32+32;

    while(true){
        double number = normalDistribution(randomVariablegenerator);
        cout<<"waiting"<<endl;
        sem_wait(empty);
        cout<<"Passed empty"<<endl;
        sem_wait(mutex);
        cout<<"passed mutex"<<endl;
        array[*currentSize].first= commodityIndex;
        array[*currentSize].second=number;
        *currentSize = (*currentSize+1)%*bufferSize;
        sem_post(mutex);
        sem_post(full);
        cout<<*bufferSize<<endl;
        sleep(timeOut/1000);
    }
    
    return 0;
}