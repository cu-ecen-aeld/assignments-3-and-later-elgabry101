#include "stdio.h"
#include "syslog.h"

int main(int argc,char* argv[])
{
	openlog(NULL,0,LOG_USER);
	if(argc!=3)
	{
		syslog(LOG_ERR,"Invalid number of arguments %d",argc);
		return 1;
	}
	char* file=argv[1];
	char* str=argv[2];
	printf("Writing %s to %s",str,file);
	FILE*fptr;
	fptr=fopen(file,"w");
	if(fptr==NULL){
		perror("couldnt open file");
		syslog(LOG_ERR,"Could not open file");
		return 2;
		}
	fprintf(fptr,str);
	fclose(fptr);
	return 0;
}
