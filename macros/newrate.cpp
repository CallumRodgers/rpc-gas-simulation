#include "config.cpp"
#include <chrono>

int insist_read(uint8_t* array, uint8_t address, uint16_t nbytes,bitset<4>channels)
{
        int fail=0;
        int out=1;
        while(out!=0 && fail<10)
        {
                out=read_to_array(array,address,nbytes,channels); //read the status of the FIFOS until obtain success
                fail++;
        }
	if(fail>=10){return 1;}
        return 0;
}



int main(int argc, char* argv[])
{
        if(argc==2)
        {		
		uint8_t readarray[128]={0};
                map_memory();
		ofstream rate("rate.bin", std::ios::binary);
                string rpc(argv[1]);
                bitset<4> rpcs(rpc);
		insist_read(readarray,75,128,rpcs);
		//read_to_file(rate,75,128,rpcs);
		rate.write((char*)readarray,128);
		rate.close();
                unmap_memory();
                return 0;
        }
	else
	{
                cout<<"esse script deve receber exatamente 1 argumento (rpcs selecionadas)"<<endl<<flush;
        }
}









