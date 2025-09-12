#include "config.cpp" //cointains configuration software, which in turn contains the LVDS communication and memory manager
#include <chrono>
#include <ctime>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>


using namespace std;

string zeropad(int number) // add 0s to make all numbers between 0 and 99 two-digit
{
	if(number<10)
	{
		return "0"+to_string(number);	
	}
	else 
	{
		return to_string(number);	
	}
}

string make_evt_name() //Prepare the name of the event file based on date and time
{
	struct tm *timestamp;				
	time_t currdate;								 
	time(&currdate);
	timestamp = localtime(&currdate); 
	string second=zeropad(timestamp->tm_sec);
	string minute=zeropad(timestamp->tm_min);
	string hour=zeropad(timestamp->tm_hour);
	string day=zeropad(timestamp->tm_mday);
	string month=zeropad(1+timestamp->tm_mon);
	string year=to_string(1900+timestamp->tm_year);
	string name="HOD_"+year+"_"+month+"_"+day+"_"+hour+minute+second+".bin";
	return name;
}

string make_rate_name() //Prepare the name of the binary rate file based on date and time
{
	struct tm *timestamp;				
	time_t currdate;								 
	time(&currdate);
	timestamp = localtime(&currdate); 
	string second=zeropad(timestamp->tm_sec);
	string minute=zeropad(timestamp->tm_min);
	string hour=zeropad(timestamp->tm_hour);
	string day=zeropad(timestamp->tm_mday);
	string month=zeropad(1+timestamp->tm_mon);
	string year=to_string(1900+timestamp->tm_year);
	string name="RATE_"+year+"_"+month+"_"+day+"_"+hour+minute+second+".bin";
	return name;
}

string make_acq_name() //Prepare the new name to rename an old acquisition file (current one is always ACQ.txt)
{
	struct tm *timestamp;				
	time_t currdate;								 
	time(&currdate);
	timestamp = localtime(&currdate); 
	string second=zeropad(timestamp->tm_sec);
	string minute=zeropad(timestamp->tm_min);
	string hour=zeropad(timestamp->tm_hour);
	string day=zeropad(timestamp->tm_mday);
	string month=zeropad(1+timestamp->tm_mon);
	string year=to_string(1900+timestamp->tm_year);
	string name="ACQ_backed_up_in_"+year+"_"+month+"_"+day+"_"+hour+minute+second+".txt";
	return name;
}


void checkorcreatedir(string path) //Verifies if a directory exist, if not create. Fails if there's something that isn't a directory in the path.
{
	struct stat st;
 	bool exists=stat(path.c_str(), &st);
	if(!exists) //it exists
	{
		if(!S_ISDIR(st.st_mode))
		{
			cout<<"Something in the path "<<path<<" exists, but isn't a directory."<<endl<<flush;
			exit(1);
		}
	}
	else
	{
		const int dir_err = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (dir_err==-1)
		{
			cout<<"Error creating the directory "<<path<<endl<<flush;
			exit(1);
		}
	}
}

void backupacq(string dirname) //Checks if there's an ACQ.txt file in the path and renames it if there is.
{
	struct stat st;
	string oldpath="/data/"+dirname+"/ACQ.txt";
 	bool exists=stat(oldpath.c_str(), &st);
	if(!exists) //it exists
	{
		string newname=make_acq_name();
		string newpath="/data/"+dirname+"/"+newname;
		if (rename(oldpath.c_str(), newpath.c_str()))
   		{
        		cout<<"Error renaming old ACQ.txt file found in /data/"<<dirname<<" to the new name "<<newname<<endl<<flush;
        		exit(1);
    		}	
	}
}
 

void checkdirtree(string dirname) //Checks the existence and consistency of the directory tree of the acquisiton folder, creates non-existing folders if necessary
{
	string rootpath="/data/"+dirname;
	checkorcreatedir(rootpath);
	string rawpath=rootpath+"/HOD_RAW";
	checkorcreatedir(rawpath);	
	string reducedpath=rootpath+"/reduced_bins";
	checkorcreatedir(reducedpath);	
	string processedpath=rootpath+"/processed_files";
	checkorcreatedir(processedpath);
	string ratepath=rootpath+"/HOD_RATE";
	checkorcreatedir(ratepath);
}



void new_evt_file(ofstream& binary, ofstream& aux,string dirname, string name, unsigned long ev_count, unsigned long trigger_count, uint16_t lts_id, uint32_t overflow_counter, uint64_t time) //creates a new raw event file, that holds at most 50 k events.
{
	binary.open(("/data/"+dirname+"/HOD_RAW/"+name), std::ios::binary);	
	string line="FO\t"+name+"\t"+to_string(ev_count)+"\t"+to_string(trigger_count)+"\t"+to_string(lts_id)+"\t"+to_string(overflow_counter)+"\t"+to_string(time)+"\n";
	//cout<<line<<flush;
	aux.write(line.data(),line.size());
	aux.flush();
}

void new_rate_file(ofstream& binary, ofstream& aux,string dirname, string name) //creates new rate file
{
	binary.open(("/data/"+dirname+"/HOD_RATE/"+name), std::ios::binary);	
	string line="RT\t"+name+"\n";
	aux.write(line.data(),line.size());
	aux.flush();
}

void close_file(ofstream& binary, ofstream& aux,string name, unsigned long ev_count, unsigned long trigger_count, uint16_t lts_id, uint32_t overflow_counter,uint64_t time) //Close a raw event file
{
	binary.close();
	binary.clear();
	string line="FC\t"+name+"\t"+to_string(ev_count)+"\t"+to_string(trigger_count)+"\t"+to_string(lts_id)+"\t"+to_string(overflow_counter)+"\t"+to_string(time)+"\n";
	//cout<<line<<flush;
	aux.write(line.data(),line.size());
	aux.flush();
}

void overflow(ofstream& aux, uint32_t overflow_counter, unsigned long ev_count, unsigned long trigger_count, uint16_t oldid, uint16_t newid) //registers an LTS ID overflow
{
	struct tm *timestamp;				
	time_t currdate;								 
	time(&currdate);
	timestamp = localtime(&currdate); 
	string second=zeropad(timestamp->tm_sec);
	string minute=zeropad(timestamp->tm_min);
	string hour=zeropad(timestamp->tm_hour);
	string day=zeropad(timestamp->tm_mday);
	string month=zeropad(1+timestamp->tm_mon);
	string year=to_string(1900+timestamp->tm_year);
	string line="OF\t"+to_string(overflow_counter)+"\t"+year+"\t"+month+"\t"+day+"\t"+hour+"\t"+minute+"\t"+second+"\t"+to_string(ev_count)+"\t"+to_string(trigger_count)+"\t"+to_string(oldid)+"\t"+to_string(newid)+"\n";
	//cout<<line<<flush;
	aux.write(line.data(),line.size());
	aux.flush();
}

int dumpfifos(uint8_t* readarray, uint8_t* verify, bitset<4>ports, int verbose) //Dump the FIFOS from the enabled front-ends in case of error
{
	int fail=0;
	if(verbose){cout<<"dumping, initial status: "<<(int)verify[0]<<"\t"<<(int)verify[1]<<"\t"<<(int)verify[2]<<"\t"<<(int)verify[3]<<"\t"<<endl<<flush;}
        int out=1;
	while(out||(verify[0]!=231&&ports[0])||(verify[1]!=231&&ports[1])||(verify[2]!=231&&ports[2])||(verify[3]!=231&&ports[3])) //read until dumping all FIFOS
	{
		out=1;
                while(out!=0 && fail<20)
                {
			out=read_to_array(readarray,32,1024,ports); //dump any remaining FIFOS
			fail++;
		}
                if(fail>=20){return 1;}
		out=1;
		while(out!=0 && fail<20)
		{
			out=fixed_read_to_array(verify,31,1,ports); //read the status of the FIFOS until obtain success
			fail++;
		}
		if(fail>=20){return 1;}
	}
	return 0;
}

int getfifostat(uint8_t* verify, bitset<4>ports) //Get the status of the FIFO from frontends
{
	int fail=0;
	while(true)
	{
		int out=1;
		while(out!=0 && fail<20)
		{
			out=fixed_read_to_array(verify,31,1,ports); //read the status of the FIFOS until obtain success
			fail++;
		}	
		if(fail>=20){return 1;}
		//if(verify[0]==255||verify[2]==255||verify[3]==255){cout<<"invalid trigger found"<<endl<<flush;}
		if((verify[0]==24||verify[0]==231||verify[0]==255||!ports[0])&&(verify[1]==24||verify[1]==231||verify[1]==255||!ports[1])&&(verify[2]==24||verify[2]==231||verify[2]==255||!ports[2])&&(verify[3]==24||verify[3]==231||verify[3]==255||!ports[3]))
		{
			return 0;
		}
	}
}

int insist_read(uint8_t* array, uint8_t address, uint16_t nbytes,bitset<4>ports) //Insist on reading frontends until obtaining success, reads the data from the frontends in each enabled port, in increasing order. Array must be n_enabled_ports*nbytes big.
{
	int fail=0;
	int out=1;
	while(out!=0 && fail<10)
	{
		out=read_to_array(array,address,nbytes,ports); //read the status of the FIFOS until obtain success
		fail++;
	}	
	if(fail>=10){return 1;}
	return 0;
}

int insist_fixed_read(uint8_t* array, uint8_t address, uint16_t nbytes,bitset<4>ports) //Same as the previous but reading the data from each port to a fixed position in the array, independently of disabled ports. Array must always be 4*nbytes big. 
{
	int fail=0;
	int out=1;
	while(out!=0 && fail<10)
	{
		out=fixed_read_to_array(array,address,nbytes,ports); //read the status of the FIFOS until obtain success
		fail++;
	}	
	if(fail>=10){return 1;}
	return 0;
}

int insist_write(uint8_t* array, uint8_t address, uint16_t nbytes,bitset<4>ports) //Same as the insist read but for writing. 
{
	int fail=0;
	int out=1;
	while(out!=0 && fail<10)
	{
		out=write_from_array(array,address,nbytes,ports); //read the status of the FIFOS until obtain success
		fail++;
	}	
	if(fail>=10){return 1;}
	return 0;
}


int main(int argc, char* argv[])
{
	if(argc==9)
	{		
		//array declarations that don't depend on pointers nor any other previous variable
 
		uint8_t frontend_trig_num[1]={0};	 //variable with the trig number to be set on the frontends (0 for using central unit trigger)
		uint8_t sttm_on[1]={0x0F};			 //variable to start frontend state machines
		uint8_t sttm_off[1]={0xF0};			 //variable to stop frontend state machines
		uint8_t verify[4];					 //array to save fixed-position 1-byte data read from frontends
		uint8_t readarray[4096]={0};		 //array to save FIFOS (1024 bytes each) from frontends
		uint32_t overflow_counter=0;		 //counter 
		uint16_t oldID=0;					 //stores previously-read LTS ID
		ulong trigger_count=0;	//creates trigger counter (store total amount of triggers sent to the front-ends, independently of success in saving events)
		ulong old_trigger_count=0;
		ulong lts_err=0;		//counts LTS reading errors
		ulong ftend_err=0;		//counts frontend reading errors
		ofstream outfile;		//declaration of the output file variable
		ofstream ratefile;		//declaration of the rate file
		string evt_file_name;	//declare event file name string
		string rate_file_name;  //declares rate file name string
		//order dependent part

		map_memory();						//maps process space memory to physical memory and enable using the pointers inherited from lvds-memory-manager
		string chs(argv[1]);				//string with enabled interface ports specification (4 bits written with chars, LSB is interface port 1).
		bitset<4> ports(chs);			//converts the 4 bits from a string of chars to a bitset
		uint8_t nports=ports[0]+ports[1]+ports[2]+ports[3]; //number of enabled ports
		uint16_t logic=strtoul(argv[2],NULL,16);	//convert trigger logic hex input into 16 bit integer
		uint8_t verbose=strtoul(argv[8],NULL,10);
		uint8_t mode=strtoul(argv[3],NULL,16);		//convert trigger logic hex input into 16 bit integer
		*trigger_logic_bits=logic;			//sets the central unit trigger logic
		*trigger_mode=mode;					//sets the central unit trigger mode 
		*trigger_enable=0;					//disables central unit trigger output
		ulong numev=strtoul(argv[4],NULL,10);			//convert number of events into unsigned long
		int latency= strtoul(argv[5],NULL,10);			//get the RATE reading latency (in minutes)
		bool havelts=(bool)strtoul(argv[6],NULL,10);	//Get the LTS usage flag
		string dirname(argv[7]);				//get the acquisition directory name (it will be in /data/ but the path isn't needed, only the name)
		checkdirtree(dirname);					//checks the directory tree, create directories if nonexistent
		backupacq(dirname);						//checks and backs up any previously existent ACQ.txt file, if acquisition was launched on an existent folder
		ofstream auxfile("/data/"+dirname+"/ACQ.txt", ios_base::out); //creates new acquisition file.
		string selected(argv[1]);				//strings to save in the acquisition file, the parameters of the acquisition
		string tlogic(argv[2]);					
		string tmode(argv[3]);
		string number_ev(argv[4]);
		string ratelat(argv[5]);
		string ltsflag(argv[6]);
		selected+="\n";
		number_ev+="\n";
		tlogic+="\n";
		tmode+="\n";
		ratelat+="\n";
		ltsflag+="\n";
		auxfile.write(selected.data(),selected.size());		//save the selected ports in aux file 
		auxfile.write(number_ev.data(),number_ev.size());	//save the number of requested events in aux file
		auxfile.write(tlogic.data(),tlogic.size());		//save the trigger logic in aux file
		auxfile.write(tmode.data(),tmode.size());		//save the trigger mode in aux file
		auxfile.write(ratelat.data(),ratelat.size());	//save the rate latency in aux file
		auxfile.write(ltsflag.data(),ltsflag.size());	//save the lts usage flag in aux file
		bool closed=1; //boolean set everytime a new file must be created to store events (in the beginning and after closing a previous file)
		rate_file_name=make_rate_name(); //make the name for the rate file
		new_rate_file(ratefile,auxfile,dirname,rate_file_name);
		int fail; 		//variable used to store return values of read/write operations
	
		//The hit FSM must be started to ensure the FIFOs can be emptied

		fail=insist_write(frontend_trig_num,90,1,ports);			//set the frontends in selected ports to the specified trigger number (0)
		if(fail){cout<<"Address 90 (set ftend trigger) write failed"<<endl<<flush;return 1;}
		fail=insist_write(sttm_on,33,1,ports);						//start the hit state machine in the frontends in the enabled ports 
		if(fail){cout<<"Address 33 (start hit stt machine) write failed "<<endl<<flush;return 1;}

		//make sure all FIFOS are empty in all front-ends

		fail=getfifostat(verify,ports);
		if(fail){cout<<"failed getting fifo stat\n"<<(int)verify[0]<<"\t"<<(int)verify[1]<<"\t"<<(int)verify[2]<<"\t"<<(int)verify[3]<<"\t"<<flush;return 1;}	
		fail=dumpfifos(readarray,verify,ports,verbose);  
		if(fail){cout<<"failed dumping fifos"<<flush;return 1;}	  
		fail=insist_write(sttm_off,33,1,ports);	  //stop the state machine so it wont get the triggers while booting up the lts_id reading mechanism
		if(fail){cout<<"Address 33 (stop hit stt machine) write failed "<<endl<<flush;return 1;}



	

		//boot up lts_id reading mechanism if lts is being used

		int success_count=0;

		while(success_count<3 && havelts) //loop taking lts_IDs until there's a sequence of 3 consecutive valid IDs
		{
			cout<<*lts_id<<endl<<flush;
			*trigger_enable=1;	  //enables central unit trigger
			usleep(100);
			if(*trigger_read!=0)
			{
				*trigger_enable=0;	//enables central unit trigger
				if((((oldID>(uint16_t)*lts_id)&&(oldID>65435)&&((uint16_t)*lts_id<100))||(((uint16_t)*lts_id>oldID)&&((uint16_t)*lts_id<(oldID+100))))) //checks if the new ID is coherent with the last successfully received ID
				{
					oldID=*lts_id;
					success_count++;
				}
				else //if it isn't, try again with the next ID	
				{
					oldID=*lts_id;
					success_count=0;
				}		

			}
		}	

		//start hit FSMs on frontends
		fail=insist_write(sttm_on,33,1,ports);						//start the hit state machine in the frontends in the enabled ports 
		if(fail){cout<<"Address 33 (start hit stt machine) write failed "<<endl<<flush;return 1;}
		int out;
		struct tm *timestamp;				//creates structure for timestamp 
		time_t currdate;								 
		time(&currdate);
		timestamp = localtime(&currdate);	//saves local date and time to timestamp variables
		uint8_t tsarray[6]={timestamp->tm_hour,timestamp->tm_min,timestamp->tm_sec,timestamp->tm_mday,timestamp->tm_mon,timestamp->tm_year}; //creates array with start timestamp variables for this file
		uint64_t last_time_ms=-latency*60000+chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	        auto live_time_us=-chrono::steady_clock::now().time_since_epoch();
       		auto begin_time_us=-live_time_us;
		*trigger_enable=1;
		for(ulong i=0;i<numev || numev==0;) //if zero events were required, loop eternally, otherwise loop for the required number of events
		{
			if(*trigger_read!=0)	//check trigger variable to see if it's different from zero, in which case the Central Unit already sent the trigger response back to the front-ends
			{
                		live_time_us+=chrono::steady_clock::now().time_since_epoch();
				trigger_count++; //increment trigger variable
				uint64_t time_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
				if(time_ms-last_time_ms>=latency*60000) //read background rates from frontends if the required interval has passed
				{
					last_time_ms=time_ms;
					fail=insist_read(readarray,75,128,ports);
					if(fail){cout<<"failed reading rates"<<flush;return 1;} 
					ratefile.write((char*)&time_ms,8);	 //write the unix time in millisseconds 
					ratefile.write((char*)&readarray,128*nports); //write the background rates from the frontends in each enabled port in the file
				}

				fail=getfifostat(verify,ports); //get the status of the fifos to see if they got the trigger repsonse sent back by the Central Unit to them
				if(fail){cout<<"failed getting fifo stat"<<flush;;return 1;} 
				if((verify[0]==24||!ports[0])&&(verify[1]==24||!ports[1])&&(verify[2]==24||!ports[2])&&(verify[3]==24||!ports[3])) //if all enable FIFOS got the trigger response from the Central Unit correctly
				{
					out=read_to_array(readarray,32,1024,ports);	//read the fifos from the frontends on selected ports to the central unit memory and save them on the file
					if(out!=0) //if there was an error reading, the data must be discarded because the FIFOs were already empty. But first, it's necessary to get their status and dump any remaining FIFOs
					{
						if(verbose){cout<<"failed reading arrays from fifos, code "<<out<<endl<<flush;}
						fail=getfifostat(verify,ports); //get their status again
						if(fail){cout<<"failed getting fifo stat"<<flush;return 1;} 
						out=1; //set it back to 1 to inform the next conditional there was an error, to dump the FIFOs
					}
				}
				else 
				{
					if(verbose){cout<<"fifo stat read properly, but not all were ready"<<endl<<flush;}
					out=1; //not all were ready, and the ones that were ready will need to be dumped
				}
				if(out!=0)	//either some wasn't ready or all were ready but there was a read error			
				{	
					dumpfifos(readarray,verify,ports,verbose); //dump any remaining fifos
					if(havelts) //check if LTS is enabled
					{
						usleep(5); //ensure LTS_ID came, once that, in case of error, the system might be too fast for not reading the FIFOS
					}
					ftend_err++; 
					if(havelts) //reads, count LTS ID overflows and register overflows in acquisition file, if enabled
					{
						if((((oldID>(uint16_t)*lts_id)&&(oldID>65435)&&((uint16_t)*lts_id<100))||(((uint16_t)*lts_id>oldID)&&((uint16_t)*lts_id<(oldID+100))))) //Check if the new ID makes sense, either being at most 100 bigger than the last, or smaller in case of an overflow, but both the old and new values being close to 0 in this case
						{
							if(oldID>(uint16_t)*lts_id) //Check if the new ID is smaller than the old, in which case, it's an overflow
							{
								overflow_counter++;
								overflow(auxfile,overflow_counter,i,trigger_count,oldID,*lts_id); //register overflow in acquisition file			 
							}
							oldID=*lts_id;
						}
						else //If the new ID doesn't make sense, consider it an error and don't update the oldID variable
						{
							lts_err++; 
							//cout<<"E_lts"<<endl<<flush;
						}	
					}
				        if((trigger_count-old_trigger_count)>1000)
                                	{
                                        	cout<<"1000 thousand failed acquires, resetting front-ends state machines"<<endl<<flush;
						fail=insist_write(sttm_off,33,1,ports);
				                if(fail){cout<<"Address 33 (start hit stt machine) write failed "<<endl<<flush;return 1;}  
						usleep(1000);
						fail=insist_write(sttm_on,33,1,ports);                                     
				                if(fail){cout<<"Address 33 (start hit stt machine) write failed "<<endl<<flush;return 1;}  
						dumpfifos(readarray,verify,ports,verbose);
						usleep(1000);
						old_trigger_count=trigger_count;
	                                }
					*trigger_enable=0; //resets trigger
                    			live_time_us-=chrono::steady_clock::now().time_since_epoch();
					*trigger_enable=1;
				}
				else //all the FIFOs were ready and were succesfully read
				{
					if(!havelts||((((oldID>(uint16_t)*lts_id)&&(oldID>65435)&&((uint16_t)*lts_id<100))||(((uint16_t)*lts_id>oldID)&&((uint16_t)*lts_id<(oldID+100))))))//same thing as before, but also proceeding if LTS is disabled
					{
						if(havelts && oldID>(uint16_t)*lts_id) 
						{
							overflow_counter++;
							overflow(auxfile,overflow_counter,i,trigger_count,oldID,*lts_id);									
						} 
						if(closed) //check if there's need to open a new event file, make one if necessary
						{
							evt_file_name=make_evt_name();
							new_evt_file(outfile,auxfile,dirname,evt_file_name,i,trigger_count-1,*lts_id,overflow_counter,time_ms);
							closed=0;
						}
						oldID=*lts_id*havelts; //keeps oldID in zero if not using LTS
						outfile.write((char*)readarray,1024*nports); //write the FIFOS
						outfile.write((char*)&time_ms,8);			 //write UNIX time in millisseconds
						outfile.write((char*)&trigger_count,4); 	 //write total trigger counter
						outfile.write((char*)&oldID,2);				 //write LTS ID
						outfile.write((char*)&overflow_counter,4); 	 //write overflow counter
						outfile.flush();				 //flush file
						old_trigger_count=trigger_count;
						*trigger_enable=0; //resets trigger
						i++;					//if there was no error, increment event counter and print the event number if it reached a multiple of 100
						if((i%1000)==0)
						{
							cout<<i<<endl<<flush; //prints event number, 1000 by 1000
						}
						if((i%50000)==0 || (i==numev && numev!=0)) //closes acquisition file at every 50k events
						{
							close_file(outfile,auxfile,evt_file_name,(i-1),trigger_count-1,*lts_id,overflow_counter,time_ms);
							closed=1;
						}
						if(i<numev || numev==0)  //re-eanble trigger only if total events weren't reached.
						{
                            				live_time_us-=chrono::steady_clock::now().time_since_epoch();
							*trigger_enable=1;
						}
					}
					else //only satisfied if LTS was enabled AND there was an error
					{
						lts_err++;
						//cout<<oldID<<" "<<(int)*lts_id<<" E_lts"<<endl<<flush;
						*trigger_enable=0;
                        			live_time_us-=chrono::steady_clock::now().time_since_epoch();
						*trigger_enable=1;
					}
				}
			}
		}
	        //live_time_us+=chrono::steady_clock::now().time_since_epoch();
        	int64_t live_us=chrono::duration_cast<std::chrono::microseconds>(live_time_us).count();
	        int64_t tot_us=chrono::duration_cast<std::chrono::microseconds>(chrono::steady_clock::now().time_since_epoch()-begin_time_us).count();
		time(&currdate);
		timestamp = localtime(&currdate);	//saves local date and time to timestamp variables
		cout<<numev<<endl<<flush;
		uint8_t tearray[6]={timestamp->tm_hour,timestamp->tm_min,timestamp->tm_sec,timestamp->tm_mday,timestamp->tm_mon,timestamp->tm_year}; //creates array with start timestamp variables for this file 
		cout<<"total number of triggers (including those with subsequent read failures): "<<trigger_count<<endl<<flush; 
		cout<<"aquisition began at "<<(int)tsarray[0]<<":"<<(int)tsarray[1]<<":"<<(int)tsarray[2]<<"	"<<(int)tsarray[3]<<"/"<<(int)tsarray[4]+1<<"/"<<(int)tsarray[5]+1900<<endl<<flush;
		cout<<"aquisition ended at "<<(int)tearray[0]<<":"<<(int)tearray[1]<<":"<<(int)tearray[2]<<"	"<<(int)tearray[3]<<"/"<<(int)tearray[4]+1<<"/"<<(int)tearray[5]+1900<<endl<<flush;
        cout<<"total time was "<<tot_us<<" microseconds, and live time was "<<live_us<<" microseconds"<<endl<<flush;
		fail=insist_write(sttm_off,33,1,ports);						//start the hit state machine in the frontends in the enabled ports 
		if(fail){cout<<"Address 33 (stop hit stt machine) write failed "<<endl<<flush;return 1;}		
		unmap_memory();			//unmaps memory
		return 0;				//returns 0 (everything was successful) 
	}
	else
	{
		cout<<"This program must receive exactly 8 arguments (enabled interface ports, trigger logic, trigger mode, number of events (0=infinite), rate measurement interval (in minutes), lts usage (0=ignore lts, 1=use lts), output directory name (which should be in /data/, will be created if non-existent)and verbose status (0=quiet,1=verbose)"<<endl<<flush;
		return 1;
	}
}
