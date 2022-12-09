#include <iostream>
#include <random>
#include <iomanip>      // std::setprecision
using namespace std;


int main(int argc, char** argv){
    string commodityName = argv[1];
    double commodityMean = stoi(argv[2]);
    double standardDeviation = stoi(argv[3]);
    double timeOut = stoi(argv[4]);
    default_random_engine randomVariablegenerator;
    normal_distribution<double> normalDistribution(commodityMean,standardDeviation);
    double number = normalDistribution(randomVariablegenerator);
    return 0;
}