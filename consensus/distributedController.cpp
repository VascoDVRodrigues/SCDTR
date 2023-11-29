#include "distributedController.h"
#include <iostream>
using namespace std;

void DController::setIndex(int i){
    index = i;
}
void DController::setExternalIllumination(float ext){
    o = ext;
}
void DController::setLowerReference(float ilu){
    l = ilu;
    DController::resetConsensus();
}
void DController::setIlluminationCosts(float cost){
    for(int i = 0; i<3; i++){
        (i == index)? c[i] = cost: c[i]=0; 
    }
}
void DController::setGains(float* vector){
    for(int i = 0; i<3; i++){
        k[i] = vector[i] * 40.96;
        n+=k[i]*k[i];
    }
    m = n-(k[index]*k[index]);
}
void DController::setD(float* vector){
    for(int i = 0; i<3; i++){
        d[i] = vector[i];
    }
}
void DController::setDAvg(float* vector){
    for(int i = 0; i<3; i++){
        d_avg[i] = vector[i];
    }
}
void DController::setY(float* vector){
    for(int i = 0; i<3; i++){
        y[i] = vector[i];
    }
}
bool DController::checkFeasibility(float* d){
    float tolerance = 0.001;
    float boundary = 0.0;
    if(d[index]<float(lowerLimit)-tolerance) return false; 
    if(d[index]>float(higherLimit) + tolerance) return false; 
    for (int i = 0; i < 3; i++)
    {
        boundary+=d[i]*k[i];
    }
    if(boundary<l-o-tolerance) return false; 
    return true;
}
float DController::evaluateCost(float* d, float rho){
    float cost = 0.0;
    float sqr_norm = 0.0;
    for (int i = 0; i < 3; i++)
    {
        cost += c[i]* d[i] + y[i] * (d[i] - d_avg[i]);
    }
    for (int i = 0; i < 3; i++)
    {
        sqr_norm += (d[i] - d_avg[i]) * (d[i] - d_avg[i]);
    }
    return cost+rho/2*sqr_norm;
}
void DController::iterateConsensus(float rho, package &package){
    float d_best[3] = {-1, -1, -1};
    float cost_best = 1000000.0;
    float z[3];
    float d_u[3];
    float cost = cost_best;
    //Unconstrained solution
    for (int i = 0; i < 3; i++){
        z[i] = rho * d_avg[i] - y[i] - c[i];
        d_u[i] = z[i]/rho;
    }
    if(DController::checkFeasibility(d_u)){
        cost = DController::evaluateCost(d_u, rho);
        if(cost<cost_best){
            cost_best = cost;
            for (int i = 0; i < 3; i++){d_best[i] = d_u[i];}
        }
    }

    //Linear boundary
    float var = 0.0;
    for (int i = 0; i < 3; i++){
        var += z[i] * k[i];
    }
    for (int i = 0; i < 3; i++){
        d_u[i] = z[i]/rho-k[i]/n*(o - l+var/rho);
    }
    if(DController::checkFeasibility(d_u)){

        cost = DController::evaluateCost(d_u, rho);
        if(cost<cost_best){
            cost_best = cost;
            for (int i = 0; i < 3; i++){d_best[i] = d_u[i];}
        }
    }

    //Lower boundary
    for (int i = 0; i < 3; i++){
        if(i == index){
            d_u[i] = 0;
        }else{
            d_u[i] = z[i]/rho;
        } 
    }
    if(DController::checkFeasibility(d_u)){

        cost = DController::evaluateCost(d_u, rho);
        if(cost<cost_best){
            cost_best = cost;
            for (int i = 0; i < 3; i++){d_best[i] = d_u[i];}
        }
    }
    //Higher boundary
    for (int i = 0; i < 3; i++){
        if(i == index){
            d_u[i] = higherLimit;
        }else{
            d_u[i] = z[i]/rho;
        } 
    }
    if(DController::checkFeasibility(d_u)){

        cost = DController::evaluateCost(d_u, rho);
        if(cost<cost_best){
            cost_best = cost;
            for (int i = 0; i < 3; i++){d_best[i] = d_u[i];}
        }
    }
    //Linear and lower boundary
    for (int i = 0; i < 3; i++){
        if(i == index){
            d_u[i] = 0;
        }else{
            d_u[i] = z[i]/rho - k[i]/m * (o - l) + (1/rho/m)*k[i]*(k[index]*z[index]-var);
        } 
    }
    if(DController::checkFeasibility(d_u)){
        cost = DController::evaluateCost(d_u, rho);
        if(cost<cost_best){
            cost_best = cost;
            for (int i = 0; i < 3; i++){d_best[i] = d_u[i];}
        }
    }

    //Linear and higher boundary
    for (int i = 0; i < 3; i++){
        if(i == index){
            d_u[i] = higherLimit;
        }else{
            d_u[i] = z[i]/rho - k[i]/m * (o - l + higherLimit * k[index]) + (1/rho/m)*k[i]*(k[index]*z[index]-var);
        } 
    }
    if(DController::checkFeasibility(d_u)){
        cost = DController::evaluateCost(d_u, rho);
        if(cost<cost_best){
            cost_best = cost;
            for (int i = 0; i < 3; i++){d_best[i] = d_u[i];}
        }
    }

    for (int i = 0; i < 3; i++){
        package.c[i] = d_best[i];
        
    }
    package.cost = cost_best;
}
float DController::getD(int ind){
    return d[ind];
}

void DController::resetConsensus(){
    for(int i = 0; i<3; i++){
        d[i] = 0;
        d_avg[i] = 0;
        y[i] = 0;
    }
}
void DController::teste(){
    cout << "index: " << index <<endl;
    cout << "d: " << d[0] << " "<<  d[1] <<  " "<<  d[2] << endl;
    cout << "d_avg: " << d_avg[0] << " "<<  d_avg[1]<< " "<<  d_avg[2] << endl;
    cout << "y: " << y[0] << " "<<  y[1] << " "<<  y[2] << endl;
    cout << "k: " << k[0] << " "<<  k[1] << " "<<  k[2] << endl;
    cout << "c: " << c[0] << " "<<  c[1] << " "<<  c[2] << endl;
    cout << "n: " << n <<endl;
    cout << "m: " << m <<endl;
    cout << "o: " << o <<endl;
    cout << "l: " << l <<endl;
}