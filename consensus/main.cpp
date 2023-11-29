#include <iostream>
#include "distributedController.h"

using namespace std;

int main(int argc, char** argv){
    DController dcont1;
    DController dcont2;
    DController dcont3;

    int maxIterations = atoi(argv[1]);

    float c1 = 1;
    float k1[3] = {0.06566, 0.00203, 0.00622};
    float c2 = 1;
    float k2[3] = {0.00109, 0.00849, 0.00345};
    float c3 = 1;
    float k3[3] = {0.00470, 0.00504, 0.01469};
    dcont1.setIndex(0);
    dcont2.setIndex(1);
    dcont3.setIndex(2);

    dcont1.setIlluminationCosts(c1);
    dcont2.setIlluminationCosts(c2);
    dcont3.setIlluminationCosts(c3);
    dcont1.setGains(k1);
    dcont2.setGains(k2); 
    dcont3.setGains(k3);    
    dcont1.setExternalIllumination(0.57);
    dcont2.setExternalIllumination(0.52);
    dcont3.setExternalIllumination(1.29);
    dcont1.setLowerReference(10);
    dcont2.setLowerReference(10);
    dcont3.setLowerReference(10);
    package package1;
    package package2;
    package package3;
    package1.c[0] = 0;
    package1.c[1] = 0;
    package2.c[0] = 0;
    package2.c[1] = 0;
    package3.c[0] = 0;
    package3.c[1] = 0;
    package1.cost = 0;
    package2.cost = 0;
    package3.cost = 0;

    float d_avg[3] = {0,0,0};
    float y1[3] = {0,0,0};
    float y2[3] = {0,0,0};
    float y3[3] = {0,0,0};
    
    for (int i = 0; i < maxIterations; i++ ) {
        dcont1.iterateConsensus(0.07, package1);
        dcont2.iterateConsensus(0.07, package2);
        dcont3.iterateConsensus(0.07, package3);

        dcont1.setD(package1.c);
        dcont2.setD(package2.c);
        dcont3.setD(package3.c);
        
        for(int i = 0; i<3; i++){
            d_avg[i] = (package1.c[i] + package2.c[i]+package3.c[i])/3;
        }
        
        dcont1.setDAvg(d_avg);
        dcont2.setDAvg(d_avg);
        dcont3.setDAvg(d_avg);

        for(int i = 0; i<3; i++){
            y1[i] += 0.07 * (package1.c[i] - d_avg[i]);
            y2[i] += 0.07 * (package2.c[i] - d_avg[i]);
            y3[i] += 0.07 * (package3.c[i] - d_avg[i]);
        }
        dcont1.teste();
        
        dcont1.setY(y1);
        dcont2.setY(y2);
        dcont3.setY(y3);        
    }
}