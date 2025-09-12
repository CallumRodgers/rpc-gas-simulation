#include "config.cpp" //cointains configuration software, which in turn contains the LVDS communication and memory manager
#include <chrono>
#include <ctime>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>


using namespace std;

int main(int argc, char* argv[])
{
	if(argc==4)
	{		
		map_memory();						//maps process space memory to physical memory and enable using the pointers inherited from lvds-memory-manager
		uint16_t logic=strtoul(argv[1],NULL,16);	//convert trigger logic hex input into 16 bit integer
		uint8_t mode=strtoul(argv[2],NULL,16);		//convert trigger logic hex input into 16 bit integer
		*trigger_logic_bits=logic;			//sets the central unit trigger logic
		*trigger_mode=mode;					//sets the central unit trigger mode 
		*trigger_enable=0;					//disables central unit trigger output
		ulong noftrig=strtoul(argv[3],NULL,10);			//get the RATE reading duration (in minutes)		
		ulong trigger_count=0;	//creates trigger counter (store total amount of triggers sent to the front-ends, independently of success in saving events)   	
		uint64_t last_time_ms=chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
		auto live_time_us=-chrono::steady_clock::now().time_since_epoch();
		auto begin=-live_time_us;
		*trigger_enable=1;
		while(true) 
		{
			if(*trigger_read!=0)	//check trigger variable to see if it's different from zero, in which case the Central Unit already sent the trigger response back to the front-ends
			{
				live_time_us+=chrono::steady_clock::now().time_since_epoch();
				trigger_count++; //increment trigger variable
				if(trigger_count<noftrig)
				{
                			*trigger_enable=0;
					live_time_us-=chrono::steady_clock::now().time_since_epoch();
		                	*trigger_enable=1;
				}
				else 
				{
					break;
				}
			}
		}
		auto dur_us=chrono::steady_clock::now().time_since_epoch()-begin;
		int64_t live_us=chrono::duration_cast<std::chrono::microseconds>(live_time_us).count();
                int64_t time_us=chrono::duration_cast<std::chrono::microseconds>(dur_us).count();

		//uint64_t time_ms=chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();	
//		float duration=(float)(time_ms-last_time_ms)/1000.0; 
		//cout<<((float)duration)<<endl<<flush;
		cout<<"total time was "<<time_us<<" microseconds, and live time was "<<live_us<<" microseconds"<<endl<<flush;
	    	unmap_memory();			//unmaps memory
		return 0;				//returns 0 (everything was successful) 
	}
	else
	{
		cout<<"This program must receive exactly 3 arguments (trigger logic, trigger mode, number of triggers to measure"<<endl<<flush;
		return 1;
	}
}
