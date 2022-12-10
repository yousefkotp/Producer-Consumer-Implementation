#include <iostream>
#include <random>
#include <iomanip>      // std::setprecision
#include <sys/types.h>
#include <bits/stdc++.h>
#include <sys/shm.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
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
    void* sharedMemory;
    sharedMemory= shmat(sharedMemoryID,NULL,0);
    double* array = (double *) sharedMemory;

    
    while(true){
        double number = normalDistribution(randomVariablegenerator);
        array[commodityIndex]=number;
        cout<<array[commodityIndex]<<endl;
        sleep(timeOut/1000);
    }
    
    return 0;
}