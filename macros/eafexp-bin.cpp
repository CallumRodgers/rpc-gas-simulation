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
#include <vector>

using namespace std;



uint8_t translate(bitset<64> maroc_chs,uint8_t* table)
{
		int count=0;
		while(count<64) //get the number of the bit (MAROC channel) that had a 1
		{
				if(maroc_chs[count])
				{
						break;
				}
				count++;
		}
		uint8_t pad=*(table+count);	//get pad number
		return pad;
}


int get_hod_id(char* hod_id, int hod_size, int nchannels,ifstream& reduced_hod)
{
	ulong hodcurrpos=reduced_hod.tellg(); 
	reduced_hod.seekg(hodcurrpos+hod_size*8*nchannels+12);
	reduced_hod.read(hod_id,4);
	if(reduced_hod.eof()) //tried to read after end of file
	{	
		return 1;			  
	}
	reduced_hod.seekg(hodcurrpos);
	return 0;
}


int main(int argc, char* argv[])
{
	if(argc!=6)
	{
		cout<<"this program must receive exactly 5 arguments: name of translators files for channel 1 through 4, acquisition date in YYYY_MM_DD format"<<endl<<flush;
		return 255;
	}
	ifstream map1(argv[1], std::ios_base::in); //open file with mapping between MAROC channel (line number - 1) and pad (value on that line) for channel 1.
	ifstream map2(argv[2], std::ios_base::in); //same for channel 2
	ifstream map3(argv[3], std::ios_base::in); //same for channel 3
	ifstream map4(argv[4], std::ios_base::in); //same for channel 4

	uint8_t translator1[64];					  //create array for MAROC channel to pad translator for channel 1
	uint8_t translator2[64];					  //create array for MAROC channel to pad translator for channel 2
	uint8_t translator3[64];					  //create array for MAROC channel to pad translator for channel 3
	uint8_t translator4[64];					  //create array for MAROC channel to pad translator for channel 4
	int pad;								  // pad number variable
	for(int i=0; i<64 ; i++)				  // read translator files into arrays
	{
  		map1>>pad;
		translator1[i]=pad;
  		map2>>pad;
		translator2[i]=pad;
  		map3>>pad;
		translator3[i]=pad;
  		map4>>pad;
		translator4[i]=pad;
	}	
	uint8_t readarray1[1024];					 //arrays to read the FIFOS from the frontends of each channel
	uint8_t readarray2[1024];
	uint8_t readarray3[1024];
	uint8_t readarray4[1024];
 
	string acq_date(argv[5]);
	string folder="/data/DATA_"+acq_date+"/";
	string hod_reduced_name="HOD_EVT_"+acq_date+".bin";
	string processed_name="EAFEXP_"+acq_date+".bin";
	string reduced_path=folder+"reduced_bins/";
	string processed_path=folder+"processed_files/";
	
	ifstream reduced_hod(reduced_path+hod_reduced_name, ios::binary); //open reduced binary file
	ofstream processed(processed_path+processed_name, ios_base::binary); //create or rewrite processed output file


	uint8_t hod_beginbin1;
	uint8_t hod_beginbin2;
	uint8_t hod_beginbin3;
	uint8_t hod_beginbin4;
	uint8_t selected;						
	uint16_t lts_id;
	uint32_t trigger_counter;
	uint32_t of_counter;
	uint8_t hod_size;
	uint64_t time_ms;
	uint32_t hod_id;

	reduced_hod.read((char*)&selected,1);   
	reduced_hod.read((char*)&hod_beginbin1,1); 
	reduced_hod.read((char*)&hod_beginbin2,1); 
	reduced_hod.read((char*)&hod_beginbin3,1); 
	reduced_hod.read((char*)&hod_beginbin4,1);  
	reduced_hod.read((char*)&hod_size,1);   
	cout<<int(selected)<<endl<<int(hod_beginbin3)<<endl<<int(hod_size)<<endl<<flush;


	bitset<4> channels(selected);					   //converts the 4 bits from a byte to a bitset
	ulong numev=0;	
	uint8_t nchannels=channels[0]+channels[1]+channels[2]+channels[3]; //number of enabled channels

	int nobt=0;
	while(true) //loop over all events
	{	
		if(get_hod_id((char*)&hod_id, hod_size, nchannels, reduced_hod))
		{
			break;
		}
		if(channels[0]){reduced_hod.read((char*)readarray1,hod_size*8);} //read fifos from each enabled channel
		if(channels[1]){reduced_hod.read((char*)readarray2,hod_size*8);}
		if(channels[2]){reduced_hod.read((char*)readarray3,hod_size*8);}
		if(channels[3]){reduced_hod.read((char*)readarray4,hod_size*8);}
		reduced_hod.read((char*)&time_ms,8);
		reduced_hod.read((char*)&trigger_counter,4); 
		reduced_hod.read((char*)&lts_id,2);
		reduced_hod.read((char*)&of_counter,4);	 
		uint64_t pads1=0;   //64 bit variables that will hold the 64 discriminated MAROC outputs (pads) at each FIFO position
		uint64_t pads2=0;
		uint64_t pads3=0;
		uint64_t pads4=0;		
		for(int i=0;i<hod_size;i++) //make a bitwise OR for every FIFO position in the window and count how many 1's total there was per pad
		{
			uint64_t ipad1=(*(uint64_t*)(readarray1+i*8));
			uint64_t ipad2=(*(uint64_t*)(readarray2+i*8));
			uint64_t ipad3=(*(uint64_t*)(readarray3+i*8));
			uint64_t ipad4=(*(uint64_t*)(readarray4+i*8));
			pads1|=ipad1;
			pads2|=ipad2;
			pads3|=ipad3;
			pads4|=ipad4; 
		}
		//check if each enabled channel had 1s in only one MAROC channel (one pad);
		bitset<64> bitspads1(pads1); //create bitset variables for each
		bitset<64> bitspads2(pads2);
		bitset<64> bitspads3(pads3);
		bitset<64> bitspads4(pads4);
		int nbitspads1=0;			//create bit counter
		int nbitspads2=0;
		int nbitspads3=0;
		int nbitspads4=0;
		for(int b=0;b<64;b++)
		{
			nbitspads1+=bitspads1[b];
			nbitspads2+=bitspads2[b];
			nbitspads3+=bitspads3[b];
			nbitspads4+=bitspads4[b];
		}
                numev++;
                if(numev%1731==0) //print progress at every 1731 events 
                {
                    cout<<numev<<" events processed\r"<<flush;
                }

		//if for each enabled channel there was 1s in only one pad in this event
		if((nbitspads1==1||!channels[0])&&(nbitspads2==1||!channels[1])&&(nbitspads3==1||!channels[2])&&(nbitspads4==1||!channels[3]))
		{
			nobt++;
			if(channels[0]) //if channel 1 was enabled
			{
				uint8_t pad1=translate(bitspads1,translator1); //transalte MAROC channels to pads and convert to string
				processed.write((char*)&pad1,1); //save on file
			}
			if(channels[1]) //if channel 1 was enabled
			{
				uint8_t pad2=translate(bitspads2,translator2); //transalte MAROC channels to pads and convert to string
				processed.write((char*)&pad2,1); //save on file
			}
			if(channels[2]) //if channel 1 was enabled
			{
				uint8_t pad3=translate(bitspads3,translator3); //transalte MAROC channels to pads and convert to string
				processed.write((char*)&pad3,1); //save on file
			}
			if(channels[3]) //if channel 1 was enabled
			{
				uint8_t pad4=translate(bitspads4,translator4); //transalte MAROC channels to pads and convert to string
				processed.write((char*)&pad4,1); //save on file
			}
		}
	}
	cout<<nobt<<endl<<flush;
	reduced_hod.close();
	processed.close();
	cout<<"finished."<<endl<<flush;
}
