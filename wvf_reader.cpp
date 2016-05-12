#include<string>
#include<map>
#include<iostream>
using namespace std;
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#pragma pack(1)
struct WVFHeaderInfo
{
 char icc[4];
 int version;
 char copyright[64];
 int total_file;
 int have_file_list;
};
#pragma pack()

struct PathHashInfo
{
 unsigned int hash0;
 unsigned int hash1;
 unsigned int hash2;
 
 bool operator < (const PathHashInfo& a) const
 {
   if(hash0 < a.hash0)return true;
   if(hash0 > a.hash0)return false;
   if(hash1 < a.hash1)return true;
   if(hash1 > a.hash1)return false;
   return hash2 < a.hash2;
 }
 bool operator == (const PathHashInfo& a) const
 {
   return (a.hash0 == hash0) &&(a.hash1 == hash1) && (a.hash2 ==  hash2);
 }
}; 


class RemoteVersionReader
{
public:
   RemoteVersionReader(const std::string& file_name)
   {
 	FILE* remote_file = 0;
	remote_file =fopen(file_name.c_str(),"rb");
 	if(NULL != remote_file)
	{
 	  WVFHeaderInfo head_info;
	  fread(&head_info,sizeof(head_info),1,remote_file);
	 if(strcmp(head_info.icc,"WVF")==0)
	 {
	    printf("%s\n",head_info.copyright);
	    printf("%s\n",head_info.icc);
	    printf("version is %d\n",head_info.version);
	    printf("total file num: %d\n",head_info.total_file);	 
	    printf("have_file_list: %d\n",head_info.have_file_list);
	    PathHashInfo info;
	    unsigned int version;
	    for(int i=0; i < head_info.total_file; i++)
	    {
		fread(&info,sizeof(PathHashInfo),1,remote_file);
 		fread(&version,sizeof(unsigned int),1,remote_file);
 	        printf("%d:%d:%d----%d\n",info.hash0,info.hash1,info.hash2,version);
	    }
 	    bool m_is_contain_file_name = ((head_info.have_file_list == 0) ? false : true);
	    if(m_is_contain_file_name)
	    {
		char buf[256];
		memset(buf,0,sizeof(buf));
 		for(int i = 0; i < head_info.total_file; i++)
		{
		 fread(buf,256,1,remote_file);
		 printf("%d-> %s\n",i,buf);
		}
 	    }
	  }
	}
   }
};

int main(int argc,char* argv[])
{
 if(argc < 2)
 {
  printf("use a wvf file for a param\n");
  exit(-1);
 }
 std::string filename(argv[1]);
 
RemoteVersionReader* reader = new RemoteVersionReader(filename);

 return 0;
}
