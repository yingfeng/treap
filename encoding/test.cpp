#include "pfor_coding.h"
#include "coding_factory.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int compressedSize(string fileName, coding* myCoding, int groupSize){
    ifstream fin(fileName.c_str());
    int totalSize = 0;
    int count = 0;
    unsigned *values = new unsigned[groupSize];
    unsigned* compressed = new unsigned[groupSize];
    int index = 0;
    string token;
    while(true){
        fin >> token;
        if(fin.eof()){
            break;
        }
        count ++;
        values[index] = atoi(token.c_str()); 
        index ++;
        if(index == groupSize){
            totalSize += myCoding->Compression(values, compressed, groupSize);
            index = 0;
        }
    }
    totalSize += myCoding->Compression(values, compressed, index);
    fin.close();
    delete [] values;
    delete [] compressed;
    cout << 32 * totalSize << ":" << count << ":" << totalSize * 32/(double)count << endl;
}

int main(int argc, char* args[]){
    int codingType = atoi(args[1]);     
    string fileName = args[2];
    int groupSize = 1024 * 1024;

    coding_factory factory;
    coding* myCoding = factory.get_coder(codingType);
    compressedSize(fileName, myCoding, groupSize); 
}
