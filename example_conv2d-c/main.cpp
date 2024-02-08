#define batchSize 2
#define numInputChannels 16
#define numOutputChannels 8
#define stride 2
#define kernelSize 3
#define inputHeight 50
#define inputWidth 100
#define outputHeight 24 // adjust it if you change other parameters
#define outputWidth 49 // adjust it if you change other parameters
#define EPS 1e-6
#include <iostream>
#include <fstream>
#include <stdlib.h>

void conv2dNoPaddingNoBias(float inBuffer[batchSize][numInputChannels][inputHeight][inputWidth],
    float wBuffer[numOutputChannels][numInputChannels][kernelSize][kernelSize],
    float outBuffer[batchSize][numOutputChannels][outputHeight][outputWidth]){
    float sum;
    for (int b = 0; b < batchSize; b++){
        for (int co = 0; co < numOutputChannels; co++){
            for (int h = 0; h < outputHeight; h++){
                for (int w = 0; w < outputWidth; w++){
                    sum = 0;
                    for (int ci = 0; ci < numInputChannels; ci++){
                        for (int k0 = 0; k0 < kernelSize; k0++){
                            for (int k1 = 0; k1 < kernelSize; k1++){
                                sum += inBuffer[b][ci][h * stride + k0][w * stride + k1] * wBuffer[co][ci][k0][k1];
                            }
                        }
                    }
                    outBuffer[b][co][h][w] = sum;
                }
            }
        }
    }
}


void validateOutput(float out[batchSize][numOutputChannels][outputHeight][outputWidth],
    float outGolden[batchSize][numOutputChannels][outputHeight][outputWidth]){
    for (int b = 0; b < batchSize; b++){
        for (int co = 0; co < numOutputChannels; co++){
            for (int h = 0; h < outputHeight; h++){
                for (int w = 0; w < outputWidth; w++){
                    if (abs(outGolden[b][co][h][w] - out[b][co][h][w]) > EPS ){
                        std::cout << "output[" << b << ","<< co << "," << h << "," << w << "]: " << outGolden[b][co][h][w] << " != " << out[b][co][h][w] << std::endl;
                        return;
                    }
                }
            }
        }
    }
    std::cout << "PASSED" << std::endl;
    return;
}

int main(){
    float inBuffer[batchSize][numInputChannels][inputHeight][inputWidth];
    float wBuffer[numOutputChannels][numInputChannels][kernelSize][kernelSize];
    float outBuffer[batchSize][numOutputChannels][outputHeight][outputWidth];
    float outBufferGolden[batchSize][numOutputChannels][outputHeight][outputWidth];
    std::ifstream ifs_in("input.bin", std::ios::in | std::ios::binary);
    ifs_in.read((char*)(***inBuffer), batchSize * numInputChannels * inputHeight * inputWidth * sizeof(float));
    std::ifstream ifs_param("weight.bin", std::ios::in | std::ios::binary);
    ifs_param.read((char*)(***wBuffer), numOutputChannels * numInputChannels * kernelSize * kernelSize * sizeof(float));
    conv2dNoPaddingNoBias(inBuffer, wBuffer, outBuffer);    
    // std::ofstream ofs_out("output_c.bin", std::ios::out | std::ios::binary);
    // ofs_out.write((char*)(***outBuffer), batchSize * numOutputChannels * outputHeight * outputWidth * sizeof(float) );
    std::ifstream ofs_out_golden("output_torch.bin", std::ios::out | std::ios::binary);
    ofs_out_golden.read((char*)(***outBufferGolden), batchSize * numOutputChannels * outputHeight * outputWidth * sizeof(float) );
    validateOutput(outBuffer, outBufferGolden);
}
