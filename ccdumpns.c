#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


struct dns{
char domain[100];
char ip[20];
};

struct dns record[]={
{"google.com","142.12.10.09"},
{"wipro.com","132.156.12.12"},
{"youtube.com","112.166.09.05"}
};

void to_lower(char* data)
{
    for(int i=0;i<strlen(data);i++)
    {
        data[i]=tolower(data[i]);
    }
}
void rem(char *data)
{
    char *p;
    if((p=strchr(data,'\n'))!=NULL)
    {
        *p='\0';
    }
    //return pos;
}
char *resolve(char *data)
{
    to_lower(data);
    rem(data);
    int num=sizeof(record)/sizeof(record[0]);
    for(int i=0;i<num;i++)
    {
        if(strcmp(record[i].domain,data)==0)
        {
            return record[i].ip;
        }
    }
}
int main()
{
    char buf[100];
    printf("Enter the host name:");
        scanf("%s",buf);
        buf[100-1]='\0';
        char *ip_a=resolve(buf);
    printf("IP address: %s",ip_a);
    return 0;

}
