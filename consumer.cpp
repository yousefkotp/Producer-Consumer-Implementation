#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <cstring>
#include "MySemaphore.h"
#include <signal.h>

#define ANSI_COLOR_RED     "\033[;31m"
#define ANSI_COLOR_BLUE   "\033[34m"
#define ANSI_COLOR_GREEN   "\033[;32m"
#define ANSI_COLOR_RESET   "\033[0m"
using namespace std;

key_t IPC_key;
void* sharedMemory;
int sharedMemoryID;
map<string,int>indexOf;
map<int,vector<double>>prevValues;
map<int,double>prevAverageValues;
pair<int,double>* prices;
int bufferSize;
int* currentItem;
int* currentSize;
int semaphoreSetId;

string COMMODITIES[11]= {"ALUMINUM  ", "COPPER    ", "COTTON    ", "CRUDEOIL  ", "GOLD      ","LEAD      ", "MENTHAOIL ", "NATURALGAS", "NICKEL    ", "SILVER    ", "ZINC      "};

void updateCommodityPrice(int commodityIndex,double currentPrice){
    printf("\033[1;1H");
    if(prevValues[commodityIndex].size()==0){//aka empty
        printf("\033[%d;%dH",(4+commodityIndex),14);
        printf(ANSI_COLOR_GREEN);
        printf("%7.2lf ↑|",currentPrice);
        printf(ANSI_COLOR_RESET);
        prevValues[commodityIndex].push_back(currentPrice);
    }else{
        double previousValue = prevValues[commodityIndex][prevValues[commodityIndex].size()-1];
        printf("\033[%d;%dH",(4+commodityIndex),14);
        if(currentPrice-previousValue>0.001){
            printf(ANSI_COLOR_GREEN);
            printf("%7.2lf ↑",currentPrice);
            printf(ANSI_COLOR_RESET);
        }else if (previousValue-currentPrice>0.001){
            printf(ANSI_COLOR_RED);
            printf("%7.2lf ↓",currentPrice);
            printf(ANSI_COLOR_RESET);
        }else{
            printf("%7.2lf -",currentPrice);
        }
        cout<<"|";
        prevValues[commodityIndex].push_back(currentPrice);
        if(prevValues[commodityIndex].size()>5)
            prevValues[commodityIndex].erase(prevValues[commodityIndex].begin());
    }
    int sum =0,j=0;
    for(j=0;j<5 && j<prevValues[commodityIndex].size();j++){
        sum+=prevValues[commodityIndex][j];
    }
    double average = sum*1.0/j;
    if(prevAverageValues[commodityIndex]<0.0001){
        printf(ANSI_COLOR_GREEN);
        printf("%7.2lf ↑",average);
        printf(ANSI_COLOR_RESET);
    }else{
        double prevAverageValue = prevAverageValues[commodityIndex];
        if(average-prevAverageValue>0.0001){
            printf(ANSI_COLOR_GREEN);
            printf("%7.2lf ↑",average);
            printf(ANSI_COLOR_RESET);
        }else if(prevAverageValue-average>0.0001){
            printf(ANSI_COLOR_RED);
            printf("%7.2lf ↓",average);
            printf(ANSI_COLOR_RESET);
        }else{
            printf("%7.2lf -",average);
        }
    }
    prevAverageValues[commodityIndex]=average;
}


void consume(){
    IPC_key = ftok("interprocesscommunication",65);
    sharedMemoryID = shmget(IPC_key,(bufferSize*16)+8,0666|IPC_CREAT); 
    sharedMemory= shmat(sharedMemoryID,NULL,0);
    memset(sharedMemory,0,(bufferSize*16)+8);

    currentSize = (int *) sharedMemory;
    currentItem= (int *) sharedMemory+4;

    prices = (pair<int,double>*)sharedMemory+8;

    struct sembuf sem_op;
    semaphoreSetId = semget(IPC_key,3,0666 | IPC_CREAT );
    semctl(semaphoreSetId,0,SETVAL,1); //mutex at index 0
    semctl(semaphoreSetId,1,SETVAL,bufferSize); // empty at index 1
    semctl(semaphoreSetId,2,SETVAL,0); //full at index 2

    while(true){
        sem_op = waitSemaphore(2);
        semop(semaphoreSetId,&sem_op,1);
        sem_op = waitSemaphore(0);
        semop(semaphoreSetId,&sem_op,1);
        cout<<"|"<<endl;
        pair<int,double>p = prices[*currentItem];
        *currentItem = (*currentItem+1)%bufferSize;
        int commodityIndex = p.first;
        double commodityPrice = p.second;
        updateCommodityPrice(commodityIndex,commodityPrice);
        sem_op = signalSemaphore(0);
        semop(semaphoreSetId,&sem_op,1);
        sem_op = signalSemaphore(1);
        semop(semaphoreSetId,&sem_op,1);
    }
}   
void signalHandler(int signum){
    shmdt(sharedMemory);
    shmctl(sharedMemoryID,IPC_RMID,NULL);
    exit(signum);
}

int main(int argc, char** argv){
    signal(SIGINT,signalHandler);
    bufferSize= stoi(argv[1]);
    printf("\e[1;1H\e[2J");
    cout<<"+-------------------------------------+"<<endl;
    cout<<"| Currency  |  Price  | AvgPrice |"<<endl;
    cout<<"+-------------------------------------+"<<endl;
    for(int i=0;i<11;i++){
        cout<<"| "<<COMMODITIES[i];
        printf("|");
        printf(ANSI_COLOR_BLUE);
        printf("%7.2lf  ",0.0);
        printf(ANSI_COLOR_RESET);
        printf("|");
        printf(ANSI_COLOR_BLUE);
        printf("%7.2lf  ",0.0);
        printf(ANSI_COLOR_RESET);
        printf("|\n");
    }
    cout<<"+-------------------------------------+"<<endl;
    consume();
    
    return 0;
}