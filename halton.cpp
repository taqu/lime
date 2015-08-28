#include <fstream>
#include <math.h>


float halton(int index, int prime)
{
    float result = 0.0f;
    float f = 1.0f / prime;
    int i = index;
    while(0 < i) {
        result = result + f * (i % prime);
        i = (int)floor((double)i/prime);
        f = f / prime;
    }
    return result;
}

//float halton_next(float prev, int prime)
//{
//    float r = 1.0f - prev - 0.000001f;
//    float f = 1.0f/prime;
//    if(f < r) {
//        return prev + f;
//    } else {
//        float h = f;
//        float hh;
//        do {
//            hh = h;
//            h *= f;
//        } while(h >= r);
//        return prev + hh + h - 1.0f;
//    }
//}


void generateHaltonSequenceAll(int base0, int base1, int maxNum)
{
    std::ofstream file("halton.txt", std::ios::binary);
    if(!file.is_open()){
        return;
    }

    int step = 4;
    float x0, y0, x1, y1;
    for(int i = 0; i < maxNum; i+=step) {
#if 0
        x0 = halton(i, base0);
        y0 = halton(i, base1);
        x0 = x0 - 0.5f; // 2.0f * x0 - 1.0f;
        y0 = y0 - 0.5f; // 2.0f * y0 - 1.0f;

        x1 = halton(i + 1, base0);
        y1 = halton(i + 1, base1);
        x1 = x1 - 0.5f; // 2.0f * x1 - 1.0f;
        y1 = y1 - 0.5f; // 2.0f * y1 - 1.0f;
        file << "{" << x0 << ", " << y0 << ", " << x1 << ", " << y1 << "}," << std::endl;
#endif
        file << "{";
        for(int j=0; j<step; ++j){
            float x = halton(i+j, base1);
            x = static_cast<float>(6.28318530717958647692*x);
            file << x << ", ";
        }
        file << "}," << std::endl;
    }
    file.close();
}


int main(int argc, char** argv)
{
    generateHaltonSequenceAll(2, 3, 256);
    return 0;
}
