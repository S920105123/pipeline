#include <fstream>
#include <cstdio>
#include <string>
#include <cstring>

extern int cycle;
static char err_buf[256];
static bool error_collector[5];
FILE *ferr;
std::string err_str[5] = {
	"Write $0 Error\n",
	"Address Overflow\n",
	"Misalignment Error\n",
	"Overwrite HI-LO registers\n",
	"Number Overflow\n"
};

void error(int type)
{
	error_collector[type]=true;
}

void output_error()
{
	int len=0;
	for (int i=0;i<sizeof(error_collector);i++) {
		if (error_collector[i]) {
			len+=sprintf(err_buf+len,"In cycle %d: %s", cycle, err_str[i].c_str());
			error_collector[i]=false;
		}
	}
	fwrite(err_buf,1,len,ferr);
}

void init_error()
{
	/* Initialize */
	//ferr.open("error_dump.rpt",std::ios_base::out);
	ferr=fopen("error_dump.rpt","wb");
	memset(error_collector,false,sizeof(error_collector));
}
