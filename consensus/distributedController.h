#ifndef DC_H
#define DC_H

struct package{
    float c[3];
    float cost;
};

class DController
{
    private:
    unsigned char lowerLimit = 0;
    unsigned char higherLimit = 100;
    int index = 0;
    float d[3] = {0,0,0};
    float d_avg[3] = {0,0,0};
    float y[3] = {0,0,0};
    float k[3] = {0,0,0};
    float n=0;
    float m=0;
    float c[3]={0,0,0};
    float o=0;
    float l=0;


    public:
    void setIndex(int);
    void setExternalIllumination(float);
    void setLowerReference(float);
    void setIlluminationCosts(float);
    void setGains(float*);
    void setD(float*);
    void setDAvg(float*);
    void setY(float*);
    bool checkFeasibility(float*);
    float evaluateCost(float*, float);
    void iterateConsensus(float, package &);
    float getD(int);
    void resetConsensus();
    void teste();
};
#endif 
