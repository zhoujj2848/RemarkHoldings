/********************************************************
时间：2017/12/1
作者：周建军
公司：Remark Holdings
题目：在同一个线程内随机生成一个长度为12的包含字母和数字的字符串，并对这个字符串进行升序排列

*********************************************************/

#include <stdio.h>
#include <string.h>
#include <iostream> 

#include <pthread.h>
#include <unistd.h>

#include <stdlib.h>    

#define msleep(x) usleep((x*1000))
#define REMARK_OK   (0)
#define REMARK_ERR  (-1)

bool g_stop = false;
bool g_printf = true;
long g_count = 0;

using namespace std;

void getRandStr(string &str,int len)
{
	char source_str[64] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	char ss[2];
	
	//srand((unsigned int)time((time_t *)NULL));
	
	for(int i=0;i<len;i++)
	{
		sprintf(ss,"%c",source_str[(rand()%62)]);
		str.append(ss);
	}
}

int sortStr(char *s)
{
	int i,j,k,l,t;
	char old_str[13] = {0};

	if(s == NULL)
	{
		printf("sortStr func error,char *s is null\n");
		return REMARK_ERR;
	}

	strcpy(old_str,s);
	
 	for(l=0;*(s+l);l++); //获取字符串长度
 	for(i=0;i<l-1;i++)   //从第一个字符开始依次找出剩下字符里面最小的字符 
 						 //然后按askii码从小到大排列起 冒泡排序 
	{
		k=i;

		for(j=i+1;j<l;j++)
		{
			if(*(s+j)<*(s+k))
			{
				k=j;
			}
		}

		t=*(s+i);
		*(s+i)=*(s+k);
		*(s+k)=t;

	}

	if(g_printf)
	{
		printf("%s -> %s\n",old_str,s);
	}

	return REMARK_OK;

}


void *work(void *args)  
{  
	string str = "";
    while(!g_stop)
    {  
    	str = "";
		getRandStr(str,12);
		sortStr((char*)str.c_str());
		g_count++;
        //msleep(1);

		
    }  

    return NULL;  
}  
 

int main(int argc,char** argv)
{
	pthread_t t_work;
	int second = 0;
	int run_time = 10;
	
	if(argv[1] == NULL)
	{
		printf("请输入运行时间 单位为秒\n");
		getchar();
		exit(1);
	}
	else
	{
		run_time = atoi(argv[1]);
	}

	if(argv[2] != NULL)
	{
		int print = atoi(argv[2]);
		if(print)
		{
			g_printf = true; 
		}
		else
		{
			g_printf = false;
		}
	}

    pthread_create(&t_work, NULL, work,NULL);    
	
    while(1)
    {
		second++;

		if(second >= run_time)
		{
			g_stop = true;
			break;
		}
	
    	sleep(1);
    }

	pthread_join(t_work, NULL);/*等待进程t_work结束*/

	printf("***************************************\ntime:%ds\nprintf:%s\nprocess:%ld\nper:%ld\n***************************************\n",run_time,g_printf?"yes":"no",g_count,g_count/run_time);

	getchar();
    return 0;  
}

