#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <cstring>

#define ANSI_COLOR_RED     "\033[;31m"
#define ANSI_COLOR_GREEN   "\033[;32m"
#define ANSI_COLOR_RESET   "\033[0m"
using namespace std;

key_t IPC_key;
void* sharedMemory;
int sharedMemoryID;
double* prices;
map<string,int>indexOf;
map<int,vector<double>>prevValues;
string COMMODITIES[11]= {"ALUMINUM  ", "COPPER    ", "COTTON    ", "CRUDEOIL  ", "GOLD      ","LEAD      ", "MENTHAOIL ", "NATURALGAS", "NICKEL    ", "SILVER    ", "ZINC      "};
void initializeMap(){
    indexOf["ALUMINUM"]=0;
    indexOf["COPPER"]=1;
    indexOf["COTTON"]=2;
    indexOf["CRUDEOIL"]=3;
    indexOf["GOLD"]=4;
    indexOf["LEAD"]=5;
    indexOf["MENTHAOIL"]=6;
    indexOf["NATURALGAS"]=7;
    indexOf["NICKEL"]=8;
    indexOf["SILVER"]=9;
    indexOf["ZINC"]=10;
}
void init(){
    /*
        The ftok() function uses the identity of the file named by the
       given pathname (which must refer to an existing, accessible file)
       and the least significant 8 bits of proj_id (which must be
       nonzero) to generate a key_t type System V IPC key, suitable for
       use with msgget(2), semget(2), or shmget(2).

       The resulting value is the same for all pathnames that name the
       same file, when the same value of proj_id is used.  The value
       returned should be different when the (simultaneously existing)
       files or the project IDs differ.
    */
    IPC_key = ftok("interprocesscommunication",65); //generate IPC key to create shared memory

    //unique number identify the shared sement
    //size of shared segment
    //permisions on shared segement
    sharedMemoryID = shmget(IPC_key,1024,0666|IPC_CREAT); //return a unique number used in shmat 
    sharedMemory= shmat(sharedMemoryID,NULL,0);
    //address where to attach segment aka NULL, takes a suitable address to attach segment to process's address space
    prices = (double *) sharedMemory;
    for(int i=0;i<11;i++)
        prices[i]=0;
}

void consume(){
    
    while(true){
        sleep(0.5);
        for(int i=0;i<11;i++){
            printf("\033[1;1H|");
            double currentPrice = prices[i];
            if(prevValues[i].size()==0){//aka empty
                if(currentPrice>0.00001){
                    printf("\033[%d;%dH",(4+i),14);
                    printf(ANSI_COLOR_GREEN);
                    printf("%7.2lf ↑",currentPrice);
                    printf(ANSI_COLOR_RESET);
                    prevValues[i].push_back(currentPrice);
                }
            }else{
                double previousValue = prevValues[i][prevValues[i].size()-1];
                printf("\033[%d;%dH",(4+i),14);
                if(currentPrice-previousValue>0.001){
                    printf(ANSI_COLOR_GREEN);
                    printf("%7.2lf ↑",currentPrice);
                    printf(ANSI_COLOR_RESET);
                    prevValues[i].push_back(currentPrice);
                }else if (previousValue-currentPrice>0.001){
                    printf(ANSI_COLOR_RED);
                    printf("%7.2lf ↓",currentPrice);
                    printf(ANSI_COLOR_RESET);
                    prevValues[i].push_back(currentPrice);
                }else{
                    printf("%7.2lf -",currentPrice);
                }
                if(prevValues[i].size()>5)
                    prevValues[i].erase(prevValues[i].begin());
                
            }
        }
    }
}
int main(){
    printf("\e[1;1H\e[2J");
    initializeMap();
    init();
    cout<<"+-------------------------------------+"<<endl;
    cout<<"| Currency   | Price  | AvgPrice |"<<endl;
    cout<<"+-------------------------------------+"<<endl;
    for(int i=0;i<11;i++){
        cout<<"| "<<COMMODITIES[i];
        printf("|%7.2lf  |%7.2lf  |\n",0.0,0.0);
    }
    consume();
    return 0;
}