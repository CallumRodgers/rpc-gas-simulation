//original version by rluz@lip.pt, modified by guferr@cbpf.br
//Modified 04/10/2022


#include <bitset>
#include <string>
#include "lvds-memory-manager.cpp"
#include <chrono>

using namespace std;




uint8_t byte_array[104]; //Array that will contain the configuration bytes to be sent to the frontend
int int_array[363];      //Array that will contain the numbers in the configuration file lines


int insist_write(uint8_t* array, uint8_t address, uint16_t nbytes,bitset<4>channels)
{
        int fail=0;
        int out=1;
        while(out!=0 && fail<10)
        {
                out=write_from_array(array,address,nbytes,channels); 
                fail++;
        }
		if(fail>=10){return 1;}
        return 0;
}

int insist_read(uint8_t* array, uint8_t address, uint16_t nbytes,bitset<4>channels)
{
        int fail=0;
        int out=1;
        while(out!=0 && fail<10)
        {
                out=read_to_array(array,address,nbytes,channels);
                fail++;
        }
        if(fail>=10){return 1;}
        return 0;
}



uint8_t build_byte(int b0,int l0,int b1,int l1,int b2,int l2,int b3,int l3,int b4,int l4,int b5,int l5,int b6,int l6,int b7,int l7) //builds a byte, bit by bit, secifying which bit in the byte comes from which bit from which number in the configuration file. if b0=3 and l0=2, for example, this means that the bit 0 of this byte will be the bit 3 from the number in line (where bit 0 means the LSB). 
{
    bitset<8> byte;          //declares bitset object byte
    byte[0]=bitset<10>(int_array[l0-1])[b0];  //sets its bits, one by one. Bitset 10 is used here once the largest number in the configuration file uses 10 bits.
    byte[1]=bitset<10>(int_array[l1-1])[b1];
    byte[2]=bitset<10>(int_array[l2-1])[b2];
    byte[3]=bitset<10>(int_array[l3-1])[b3];
    byte[4]=bitset<10>(int_array[l4-1])[b4];
    byte[5]=bitset<10>(int_array[l5-1])[b5];
    byte[6]=bitset<10>(int_array[l6-1])[b6];
    byte[7]=bitset<10>(int_array[l7-1])[b7];
    return byte.to_ulong(); //the conversion to ulong is necessary, because there's no direct conversion from bitset to either int or char 
 }

int check_conf() //checks if all values are inside the allowable range. Returns the value of the first line to fail if there's something wrong, 0 if nothing is wrong.
{
	int i=0;
	for(i=0;i<3;i++)     //checks the first 3 lines, all 1 bit
	{
		if(int_array[i]>1 or int_array[i]<0)
		{
			return i+1;
	    }
	}
	if(int_array[3]>1023 or int_array[3]<0) //checks the 4th line, 10 bits
    {
			return 4;
	}
	if(int_array[4]>1023 or int_array[4]<0) //checks the 5th line, 10 bits
    {
			return 5;
	}
	for(i=5;i<171;i++) //checks lines 6 to 171, all 1 bit
	{
		if(int_array[i]>1 or int_array[i]<0) 
		{
			return i+1;
	    }
	}	
	for(i=171;i<235;i++) //checks lines 172 to 235, all 8 bit
	{
		if(int_array[i]>255 or int_array[i]<0) 
		{
			return i+1;
	    }
	}	
	for(i=235;i<363;i++) //checks lines 236 to 363, all 1 bit
	{
		if(int_array[i]>1 or int_array[i]<0)
		{
			return i+1;
	    }
	}	
	return 0;
}



void make_bytes_from_ints()   //builds the 104-byte array with the 829 configuration bits, packing the bits in the order they need to be packed so the front-end FPGA may send it to ASIC in the correct order. Check the bit config order in the bottom of this program to understand its logic.
{

	//block 1 


	byte_array[0]=build_byte(0,1,0,2,0,3,9,5,8,5,7,5,6,5,5,5);  
	byte_array[1]=build_byte(4,5,3,5,2,5,1,5,0,5,9,4,8,4,7,4);
	byte_array[2]=build_byte(6,4,5,4,4,4,3,4,2,4,1,4,0,4,0,6);
	byte_array[3]=build_byte(0,7,0,8,0,9,0,137,0,73,0,136,0,72,0,135);


	//block 2

	int i=4; 
	for(int line=68;line>8;line-=4)   
	{
		byte_array[i]=build_byte(0,line+3,0,line+66,0,line+2,0,line+65,0,line+1,0,line+64,0,line,0,line+63);   
		i++;
	}


	//block 3 	

	byte_array[19]=build_byte(0,11,0,74,0,10,0,138,0,139,0,140,0,141,0,142); 
	byte_array[20]=build_byte(0,143,0,144,0,145,0,146,0,147,0,148,0,149,0,150);
	byte_array[21]=build_byte(0,151,0,152,0,153,0,154,0,155,0,156,0,157,0,158);
	byte_array[22]=build_byte(0,159,0,160,0,161,0,162,0,163,0,164,0,165,0,166);
	byte_array[23]=build_byte(0,167,0,168,0,169,0,170,0,171,0,299,7,235,6,235);	


	//block 4

	i=24;
	int line=235;
	while(true) 
	{
		
		byte_array[i]=build_byte(5,line,4,line,3,line,2,line,1,line,0,line,0,line+63,7,line-1);
		i++; line--;
		byte_array[i]=build_byte(6,line,5,line,4,line,3,line,2,line,1,line,0,line,0,line+63);
		i++; line--;
		byte_array[i]=build_byte(7,line,6,line,5,line,4,line,3,line,2,line,1,line,0,line);
		i++; line--;
		byte_array[i]=build_byte(0,line+64,7,line,6,line,5,line,4,line,3,line,2,line,1,line);
		i++; line--;
		byte_array[i]=build_byte(0,line+1,0,line+64,7,line,6,line,5,line,4,line,3,line,2,line);
		i++; line--;
		byte_array[i]=build_byte(1,line+1,0,line+1,0,line+64,7,line,6,line,5,line,4,line,3,line);
		i++; line--;
		byte_array[i]=build_byte(2,line+1,1,line+1,0,line+1,0,line+64,7,line,6,line,5,line,4,line);
		i++; line--;
		byte_array[i]=build_byte(3,line+1,2,line+1,1,line+1,0,line+1,0,line+64,7,line,6,line,5,line);
		if(i==94)
		{
			break;
		}
		i++; line--;
		byte_array[i]=build_byte(4,line+1,3,line+1,2,line+1,1,line+1,0,line+1,0,line+64,7,line,6,line);
		i++;
	}


	//block 5

	byte_array[95]=build_byte(4,172,3,172,2,172,1,172,0,172,0,363,0,362,0,361);

	//block 6

	i=96;  
	for(int line=360;line>304;line-=8)   
	{
		byte_array[i]=build_byte(0,line,0,line-1,0,line-2,0,line-3,0,line-4,0,line-5,0,line-6,0,line-7);   
		i++;
	}
	
	//block 7

	byte_array[103]=build_byte(0,304,0,303,0,302,0,301,0,300,0,300,0,300,0,300); //(the last 3 are futile)
}


bool read_config_from_file(string filename)       //reads all the lines in the configuration file and fills the integer array int_array with the numbers in each line
{
    int i=0, inter=0; bool flag=1;
    ifstream file(filename);                         //creates the data stream to read the file and opens the file
    if(!file)                                        //check if opening failed or not
    {
        cout<<"Config: Error. File could not be opened."<<endl<<flush;
        flag=0;                                         //flag that indicates error
    }
    else
    {
        while(file>>inter) //loop that reads each line, converts them into integers and writes them into the array.
        {
	   	int_array[i]=inter;
		i++;
        }
    }
    file.close();   //closes the file
    return flag;  //returns the flag
}

int reset_analogoutput(bitset<4> rpcs) //Resets the analog output
{
    int out=0;
    uint8_t start_rst[1]={0x0F}, stop_rst[1]={0xF0}; 
    out=insist_write(start_rst,21,1,rpcs); //Start RST
    if(out)
    {
    	cout<<"Config: Address 21 (Start RST) write error code "<<out<<endl<<flush;
    	return out;
    }
    out=insist_write(stop_rst,21,1,rpcs);  //Stop RST 
    if(out)
    {
	cout<<"Config: Address 21 (Stop RST) write error code "<<out<<endl<<flush;
	return out;
    }
    return out;
}

int send_config(bitset<4> rpcs)           //sends the configuration bytes to the front end
{
    int out=0; 
    uint8_t a222[1]={222}, a220[1]={220}; 
    out=insist_write(byte_array,10,104,rpcs); // sends the configuration bytes to the address 104, a FIFO on the front-end, from which the front-end sends to the ASIC
    if(out)
    {
     	cout<<"Config: Address 10 (send config to FIFO) write error code "<<out<<endl<<flush;
        return out;
    }
    out=insist_write(a222,1,1,rpcs); // sends the FIFO to the ASIC	
    if(out)
    {
     	cout<<"Config: Address 1 (send config to ASIC) write error code "<<out<<endl<<flush;
        return out;
    }
    usleep(500); //adds delay to wait for the internal transferences between the FIFO and the ASIC
    out=insist_write(a220,1,1,rpcs);  //   sets the front-end FPGA back to idle
    if(out)
    {
     	cout<<"Config: Address 1 (set FPGA to idle) write error code "<<out<<endl<<flush;
        return out;
    }
    out=reset_analogoutput(rpcs); // resets analog output
    if(out){return out;}
    return out;
}

bool make_config_from_file(bitset<4> rpcs, string filename) //This is the first function to be called, this is the one that makes the configuration from the file and sends it
{
    bool flag=0;	 //flag to indicate success or lack thereof
    int line=0; 	 //line variable to store line with improper value, if any
    int nrpc=rpcs[0]+rpcs[1]+rpcs[2]+rpcs[3];
    if(read_config_from_file(filename)) //calls read_config_from_file and checks if it had success
    {
		line=check_conf(); 			 //calls the function to check if all values are inside the allowable range
		if(not line) //checks if line is zero (it should be, if no line is out of range), otherwise there's an error
		{
			make_bytes_from_ints(); 					  //if it had success, build the byte array from the numbers in the conf file
			if(send_config(rpcs))  //send the configuration bytes to the front end.
			{
				cout<<"Config: Failure in configuration sending"<<endl<<flush; //message it failed
				return flag;
			}
			usleep(1000); //delay to ensure MAROC has time to reset
		}
		else
		{
			cout<<"Config: Error: There's a value out of range in line "<<line<<" of the configuration file"<<endl<<flush; //messages if there's a line with wrong value
		}
    }
    return flag; //returns the flag 
}


bool make_conf_file(string name, string namedefault, int gain) 
{
	bool flag=0;
	ofstream tempconf(name);
	ifstream defaultconf(namedefault); 
	if(!defaultconf) //check if opening failed or not
	{
        	cout<<"Config: Error. Default config file could not be opened."<<endl<<flush;
        	flag=1;                                         //flag that indicates error
    	}
    	else
	{
		int i=0;
		int val=0;
       		while(defaultconf>>val) //loop that reads each line, converts them into integers and writes them into the array.
        	{
			if(i>=171 && i<235)
			{
				tempconf<<gain<<endl;
			}
			else
			{
				tempconf<<val<<endl;
			}
			i++;
        	}
    	}
   	defaultconf.close();   //closes the file
	tempconf.close();
    	return flag;  //returns the flag   
}


bool make_def_conf_file(string name, string namedefault, int* gains, int rpc) 
{
	bool flag=0;
	ofstream tempconf(name);
	ifstream defaultconf(namedefault); 
    if(!defaultconf) //check if opening failed or not
    {
        cout<<"Config: Error. Default config file could not be opened."<<endl<<flush;
        flag=1;                                         //flag that indicates error
    }
    else
    {
		int i=0;
		int val=0;
        while(defaultconf>>val) //loop that reads each line, converts them into integers and writes them into the array.
        {
			if(i>=171 && i<235)
			{
				tempconf<<gains[rpc*64+i-171]<<endl;
			}
			else
			{
				tempconf<<val<<endl;
			}
			i++;
        }
    }
    defaultconf.close();   //closes the file
	tempconf.close();
    return flag;  //returns the flag   
}


int main(int argc, char* argv[])
{
    uint16_t readarray[256]={0};
	int gains[4][64]={0};
	int  nrpc[4]={0};
	map_memory();
	string rpc(argv[1]);
	string defaultconf(argv[2]);
	int correctgain=strtoul(argv[3],NULL,10);
	bitset<4> rpcs(rpc);
	for(int i=0; i<4; i++) 	
	{
		for(int k=0; k<64; k++) 	
		{
			gains[i][k]=correctgain;
		}
	}
	int r=0;
	for(int i=1;i<16;i*=2)
	{
		bitset<4> irpc(i);
		if(rpcs[r])
		{
			if(make_def_conf_file("conf"+irpc.to_string()+".txt",defaultconf,&gains[0][0],r)){return 0;}
		}
		r++;
	}
	r=0;
	for(int i=1;i<16;i*=2)
	{	
		bitset<4> irpc(i);
		if(rpcs[r]){make_config_from_file(irpc,"conf"+irpc.to_string()+".txt");}
		r++;
	}
	cout<<"finished"<<endl<<flush;
}




