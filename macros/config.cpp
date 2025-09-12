//original version by rluz@lip.pt, modified by guferr@cbpf.br
//Modified 04/10/2022




#include <bitset>
#include <string>
#include "lvds-memory-manager.cpp"
#include <chrono>

using namespace std;

uint8_t byte_array[104]; //Array that will contain the configuration bytes to be sent to the frontend
int int_array[363];      //Array that will contain the numbers in the configuration file lines

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
    out=write_from_array(start_rst,21,1,rpcs); //Start RST
    if(out)
    {
    	cout<<"Config: Address 21 (Start RST) write error code "<<out<<endl<<flush;
    	return out;
    }
    out=write_from_array(stop_rst,21,1,rpcs);  //Stop RST 
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
    out=write_from_array(byte_array,10,104,rpcs); // sends the configuration bytes to the address 104, a FIFO on the front-end, from which the front-end sends to the ASIC
    if(out)
    {
     	cout<<"Config: Address 10 (send config to FIFO) write error code "<<out<<endl<<flush;
        return out;
    }
    out=write_from_array(a222,1,1,rpcs); // sends the FIFO to the ASIC	
    if(out)
    {
     	cout<<"Config: Address 1 (send config to ASIC) write error code "<<out<<endl<<flush;
        return out;
    }
    usleep(500); //adds delay to wait for the internal transferences between the FIFO and the ASIC
    out=write_from_array(a220,1,1,rpcs);  //   sets the front-end FPGA back to idle
    if(out)
    {
     	cout<<"Config: Address 1 (set FPGA to idle) write error code "<<out<<endl<<flush;
        return out;
    }
    out=reset_analogoutput(rpcs); // resets analog output
    if(out){return out;}
    return out;
}

int check_MAROC(bitset<4> rpcs, int nrpc) //check if MAROC outputs aren't stuck in 1
{
    int out=0;
    uint8_t a0[1]={0};
    uint8_t result[4]={0,0,0,0};
    out=write_from_array(a0,114,1,rpcs); // start counting the amount of triggers in 255 40 MHz cycle interval
    if(out)
    {
     	cout<<"Config: Address 144 (count or_triggers) write error code "<<out<<endl<<flush;
        return out;
    }
    usleep(110000); //delay for counting
    out=read_to_array(result,114,1,rpcs);
    if(out)
    {
	cout<<"Config: Address 114 (check MAROC) read error code "<<out<<endl<<flush;	
        return out;
    }
    if(result[0]==255||result[1]==255||result[2]==255||result[3]==255||result[0]==127||result[1]==127||result[2]==127||result[3]==127)
    {
	return 255;
    }
    else
    {
    	return out;
    }
}



bool make_config_from_file(string filename, bitset<4> rpcs) //This is the first function to be called, this is the one that makes the configuration from the file and sends it
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
			int out=check_MAROC(rpcs, nrpc);
			if(out==0)  //check if MAROC outputs aren't stuck to 1
                        {
        	              cout<<"Config: Configuration sucessfully sent"<<endl<<flush; //message it was sucessful
	                      flag=1;
			}
			else if(out!=255)
			{	
                                cout<<"Config: Failure in configuration sending"<<endl<<flush; //message it failed
                                return flag;
                        }
			else
			{
		                cout<<"Config: MAROC outputs stuck in 0 or 1 for 0.1 s, repeat configuration"<<endl<<flush; //message it failed
                                return flag;
			}
		}
		else
		{
			cout<<"Config: Error: There's a value out of range in line "<<line<<" of the configuration file"<<endl<<flush; //messages if there's a line with wrong value
		}
    }

    return flag; //returns the flag 
}

/********************************************************************************
 
This block explains each bit in each byte in the byte_array, which is sent to the front end.

Additional information can be found in the config_explain.pdf file packed together with config.txt, and on the MAROC3 datasheet.

Each byte here has its bits explained, beginning by the LSB, on the left, and moving to the MSB, on the right:
 
           LSB----------------------------------------------MSB
          bit 0, bit 1, bit 2, bit 3, bit 4, bit 5, bit 6, bit 7
 

Here, little-endian order is being adopted, meaning that "bit 0" is the LSB of a number, and higher bits are closer to MSB.

Note that the serial transmission sends LSB first, but the ASIC uses big-endian order, meaning that the MSB of the values 
to be sent to ASIC must come first (closer to LSB) in the configuration bytes.

Additionally, the order of many parameters were re-arranged for convenience in the configuration file, 
hence the relationship between the bits in byte_array and the lines don't always follow a clear pattern.

Single bit lines are reffered solely by the line number alone.

-------------Block 1 (no pattern)

byte 0: lines 1 to 3 + bits 9 to 5 bits from line 5        
byte 1: bits 4 to 0 from line 5 + bits 9 to 7 from line 4    
byte 2: bits 6 to 0 from line 4 plus line 6                  
byte 3: lines 7 to 9 + line 137 + line 73 + line 136 + line 72 + line 135


-------------Block 2 (patterned)

byte 4: line 71 + line 134 + line 70 + line 133 + line 69 + line 132 + line 68 + line 131
byte 5: line 67 + line 130 + line 66 + line 129 + line 65 + line 128 + line 64 + line 127
byte 6: line 63 + line 126 + line 62 + line 125 + line 61 + line 124 + line 60 + line 123
byte 7: line 59 + line 122 + line 58 + line 121 + line 57 + line 120 + line 56 + line 119
byte 8: line 55 + line 118 + line 54 + line 117 + line 53 + line 116 + line 52 + line 115
byte 9: line 51 + line 114 + line 50 + line 113 + line 49 + line 112 + line 48 + line 111
byte 10: line 47 + line 110 + line 46 + line 109 + line 45 + line 108 + line 44 + line 107
byte 11: line 43 + line 106 + line 42 + line 105 + line 41 + line 104 + line 40 + line 103
byte 12: line 39 + line 102 + line 38 + line 101 + line 37 + line 100 + line 36 + line 99
byte 13: line 35 + line 98 + line 34 + line 97 + line 33 + line 96 + line 32 + line 95
byte 14: line 31 + line 94 + line 30 + line 93 + line 29 + line 92 + line 28 + line 91
byte 15: line 27 + line 90 + line 26 + line 89 + line 25 + line 88 + line 24 + line 87
byte 16: line 23 + line 86 + line 22 + line 85 + line 21 + line 84 + line 20 + line 83
byte 17: line 19 + line 82 + line 18 + line 81 + line 17 + line 80 + line 16 + line 79
byte 18: line 15 + line 78 + line 14 + line 77 + line 13 + line 76 + line 12 + line 75


-------------Block 3 (no significant pattern)

byte 19: line 11 + line 74 + line 10 + line 138 to 142
byte 20 :lines 143 to 150	                               
byte 21 :lines 151 to 158	
byte 22 :lines 159 to 166
byte 23: lines 167 to 171 + line 299 + bits 7 and 6 from line 235    


-------------Block 4 (patterned)

byte 24: bits 5 to 0 from line 235 + line 298 + bit 7 from line 234
byte 25: bits 6 to 0 from line 234 + line 297

byte 26: bits 7 to 0 from line 233 
byte 27: line 296 + bits 7 to 1 from line 232
byte 28: bit 0 from line 232 + line 295 + bits 7 to 2 from line 231
byte 29: bits 1 to 0 from line 231 + line 294 + bits 7 to 3 from line 230
byte 30: bits 2 to 0 from line 230 + line 293 + bits 7 to 4 from line 229
byte 31: bits 3 to 0 from line 229 + line 292 + bits 7 to 5 from line 228
byte 32: bits 4 to 0 from line 228 + line 291 + bits 7 to 6 from line 227
byte 33: bits 5 to 0 from line 227 + line 290 + bit 7 from line 226
byte 34: bits 6 to 0 from line 226 + line 289

byte 35: bits 7 to 0 from line 225
byte 36: line 288 + bits 7 to 1 from line 224
byte 37: bit 0 from line 224 + line 287 + bits 7 to 2 from line 223
byte 38: bits 1 to 0 from line 223 + line 286 + bits 7 to 3 from line 222
byte 39: bits 2 to 0 from line 222 + line 285 + bits 7 to 4 from line 221
byte 40: bits 3 to 0 from line 221 + line 284 + bits 7 to 5 from line 220
byte 41: bits 4 to 0 from line 220 + line 283 + bits 7 to 6 from line 219
byte 42: bits 5 to 0 from line 219 + line 282 + bit 7 from line 218
byte 43: bits 6 to 0 from line 218 + line 281


byte 44: bits 7 to 0 from line 217
byte 45: line 280 + bits 7 to 1 from line 216
byte 46: bit 0 from line 216 + line 279 + bits 7 to 2 from line 215
byte 47: bits 1 to 0 from line 215 + line 278 + bits 7 to 3 from line 214
byte 48: bits 2 to 0 from line 214 + line 277 + bits 7 to 4 from line 213
byte 49: bits 3 to 0 from line 213 + line 276 + bits 7 to 5 from line 212
byte 50: bits 4 to 0 from line 212 + line 275 + bits 7 to 6 from line 211
byte 51: bits 5 to 0 from line 211 + line 274 + bit 7 from line 210
byte 52: bits 6 to 0 from line 210 + line 273

byte 53: bits 7 to 0 from line 209
byte 54: line 272 + bits 7 to 1 from line 208
byte 55: bit 0 from line 208 + line 271 + bits 7 to 2 from line 207
byte 56: bits 1 to 0 from line 207 + line 270 + bits 7 to 3 from line 206
byte 57: bits 2 to 0 from line 206 + line 269 + bits 7 to 4 from line 205
byte 58: bits 3 to 0 from line 205 + line 268 + bits 7 to 5 from line 204
byte 59: bits 4 to 0 from line 204 + line 267 + bits 7 to 6 from line 203
byte 60: bits 5 to 0 from line 203 + line 266 + bit 7 from line 202
byte 61: bits 6 to 0 from line 202 + line 265


byte 62: bits 7 to 0 from line 201
byte 63: line 264 + bits 7 to 1 from line 200
byte 64: bit 0 from line 200 + line 263 + bits 7 to 2 from line 199
byte 65: bits 1 to 0 from line 199 + line 262 + bits 7 to 3 from line 198
byte 66: bits 2 to 0 from line 198 + line 261 + bits 7 to 4 from line 197
byte 67: bits 3 to 0 from line 197 + line 260 + bits 7 to 5 from line 196
byte 68: bits 4 to 0 from line 196 + line 259 + bits 7 to 6 from line 195
byte 69: bits 5 to 0 from line 195 + line 258 + bit 7 from line 194
byte 70: bits 6 to 0 from line 194 + line 257

byte 71: bits 7 to 0 from line 193
byte 72: line 256 + bits 7 to 1 from line 192
byte 73: bit 0 from line 192 + line 255 + bits 7 to 2 from line 191
byte 74: bits 1 to 0 from line 191 + line 254 + bits 7 to 3 from line 190
byte 75: bits 2 to 0 from line 190 + line 253 + bits 7 to 4 from line 189
byte 76: bits 3 to 0 from line 189 + line 252 + bits 7 to 5 from line 188
byte 77: bits 4 to 0 from line 188 + line 251 + bits 7 to 6 from line 187
byte 78: bits 5 to 0 from line 187 + line 250 + bit 7 from line 186
byte 79: bits 6 to 0 from line 186 + line 249


byte 80: bits 7 to 0 from line 185
byte 81: line 248 + bits 7 to 1 from line 184
byte 82: bit 0 from line 184 + line 247 + bits 7 to 2 from line 183
byte 83: bits 1 to 0 from line 183 + line 246 + bits 7 to 3 from line 182
byte 84: bits 2 to 0 from line 182 + line 245 + bits 7 to 4 from line 181
byte 85: bits 3 to 0 from line 181 + line 244 + bits 7 to 5 from line 180
byte 86: bits 4 to 0 from line 180 + line 243 + bits 7 to 6 from line 179
byte 87: bits 5 to 0 from line 179 + line 242 + bit 7 from line 178
byte 88: bits 6 to 0 from line 178 + line 241


byte 89: bits 7 to 0 from line 177
byte 90: line 240 + bits 7 to 1 from line 176
byte 91: bit 0 from line 176 + line 239 + bits 7 to 2 from line 175
byte 92: bits 1 to 0 from line 175 + line 238 + bits 7 to 3 from line 174
byte 93: bits 2 to 0 from line 174 + line 237 + bits 7 to 4 from line 173
byte 94: bits 3 to 0 from line 173 + line 236 + bits 7 to 5 from line 172


-------------Block 5 (no pattern)


byte 95: bits 4 to 0 from line 172 + lines 363 to 361


-------------Block 6 (patterned)

byte 96: lines 360 to 353
byte 97: lines 352 to 345
byte 98: lines 344 to 337
byte 99: lines 336 to 329
byte 100: lines 328 to 321
byte 101: lines 320 to 313
byte 102: lines 312 to 305

-------------Block 7  (no pattern)

byte 103: lines 304 to 300 + 3 futile bits 


/********************************************************************************/




