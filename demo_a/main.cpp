/********************************************************
时间：2017/11/30
作者：周建军
公司：Remark Holdings
题目：编写一个支持泛型的线程安全的FIFO阻塞队列，采用生产者和消费者的模型，一个线程随机生产一个长度为12的
包含字母和数字的字符串放入队列，另一个线程从队列中读取字符并按字母顺序升序排列

*********************************************************/

#include "block_queue.h"
#include <stdio.h>
#include <string.h>
#include <iostream> 

#include <pthread.h>
#include <unistd.h>

#define msleep(x) usleep((x*1000))

BlockQueue<string> *g_queue = NULL; 
bool g_stop = false;
bool g_printf = true;


void getRandStr(string &str,int number)
{
	char source_str[64] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	char ss[2];
	
	//srand((unsigned int)time((time_t *)NULL));
	
	for(int i=0;i<number;i++)
	{
		sprintf(ss,"%c",source_str[(rand()%62)]);
		str.append(ss);
	}
}

void sortStr(char *s)
{
	int i,j,k,l,t;
	char old_str[13] = {0};

	if(s == NULL)
	{
		return;
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
		printf("%s -> %s\n",old_str,s);     //printf 线程安全
	}

}



#if 1
void *productor(void *args)  
{  
	string str = "";
	long count = 0;

	BlockQueue<string> *queue = (BlockQueue<string> *)args;
	
    while(!g_stop)
    {  
    	str = "";
		getRandStr(str,12);
        queue->push(str); 
		count++;
        //msleep(1);

		
    }  

	queue->releaseCond();
	queue->m_create_count = count;

    return NULL;  
}  
  
void *consumer(void* args)  
{  
	long count = 0;
	BlockQueue<string> *queue = (BlockQueue<string> *)args;
	
    while(!g_stop)  
    {  
        string str;
        queue->pop(str); 
		count++;
		sortStr((char*)str.c_str());

    }  

	queue->releaseCond();
	queue->m_process_count = count;

    return NULL;  
}  
#endif

int main(int argc,char** argv)
{
	pthread_t t_p;
	pthread_t t_c;
	string str="";
	int second = 0;
	int run_time = 10;
	int queue_size = 1000;
	
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

	if(argv[2] == NULL)
	{
		printf("请输入队列数组大小\n");
		getchar();
		exit(1);
	}
	else
	{
		queue_size = atoi(argv[2]);
	}

	if(argv[3] != NULL)
	{
		int print = atoi(argv[3]);
		if(print)
		{
			g_printf = true; 
		}
		else
		{
			g_printf = false;
		}
	}

	g_queue = new BlockQueue<string>(queue_size);

    pthread_create(&t_p, NULL, productor, (void*)g_queue);    
    pthread_create(&t_c, NULL, consumer, (void*)g_queue); 
	
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

	pthread_join(t_p, NULL);/*等待进程t_p结束*/
	pthread_join(t_c, NULL);/*等待进程t_c结束*/ 

	printf("***************************************\ntime:%ds\nqueue size:%d\nprintf:%s\ncreate:%ld\nprocess:%ld\nper:%ld\n***************************************\n",run_time,queue_size,g_printf?"yes":"no",g_queue->m_create_count,g_queue->m_process_count,g_queue->m_process_count/run_time);

	getchar();
    return 0;  
}

