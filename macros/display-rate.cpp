#include <iomanip>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <unistd.h>
#include <fstream>
#include <bitset>

using namespace std;

int main(int argc, char* argv[])
{
        if(argc!=3)
        {
                cout<<"this program must receive exactly 2 arguments (name of input reduced binary, name of translator file)"<<endl<<flush;
                return 255;
 	}
        ifstream map(argv[2], std::ios_base::in); //open file with mapping between MAROC channel (line number - 1) and pad (value on that line).
        uint8_t translator[64];                               //create array for MAROC channel to pad translator
        int pad;                                                                  // pad number variable
        for(int i=0; i<64 ; i++)                                  // read translator file into array
        {
               map>>pad;
               translator[i]=pad;
        }
        ifstream rate(argv[1], std::ios::binary); //open the rate bin
	uint16_t readarray[64];
	uint16_t padrate[64];
	rate.read((char*)&readarray[0],128);
	rate.close();
	for(int i=0; i<64 ; i++) 
        {
		pad = translator[i];
		padrate[pad-1]=readarray[i];
	}
	for(int i=0 ; i<64 ; i+=8) 
        {
		cout<<(int)padrate[63-i]<<'\t'<<(int)padrate[62-i]<<'\t'<<(int)padrate[61-i]<<'\t'<<(int)padrate[60-i]<<'\t'<<(int)padrate[59-i]<<'\t'<<(int)padrate[58-i]<<'\t'<<(int)padrate[57-i]<<'\t'<<(int)padrate[56-i]<<endl<<flush;
	}	
}
