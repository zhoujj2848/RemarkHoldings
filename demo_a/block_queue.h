/********************************************************
时间：2017/11/30
作者：周建军
公司：Remark Holding
邮件：zhoujj2848@qq.com
说明：支持泛型的阻塞队列，采用环形数组的方式来承载数据

*********************************************************/


#ifndef _BLOCK_QUEUE_H
#define _BLOCK_QUEUE_H

#include <iostream> 
#include <stdio.h>
#include <stdlib.h>    
#include <sys/time.h>

using namespace std;

template<class T>  
class BlockQueue
{
	public:
		BlockQueue(int max_size = 1000)  
        {  
              
            m_max_size = max_size;  
            m_array = new T[max_size];  
            m_size = 0;  
            m_front = -1;  
            m_back = -1;  

			m_create_count = 0;
			m_process_count = 0;

            m_mutex = new pthread_mutex_t;  
            m_create_cond = new pthread_cond_t; 
			m_process_cond = new pthread_cond_t;
            pthread_mutex_init(m_mutex, NULL); 
			
            pthread_cond_init(m_create_cond, NULL);  
			pthread_cond_init(m_process_cond,NULL);
        }  
  
        ~BlockQueue()  
        {  
            pthread_mutex_lock(m_mutex);  
            if(m_array != NULL)  
                delete  m_array;  
            pthread_mutex_unlock(m_mutex); 

			printf("delete 1\n");
  
            pthread_mutex_destroy(m_mutex);  
            pthread_cond_destroy(m_create_cond);  
			pthread_cond_destroy(m_process_cond);  

			printf("delete 2\n");
  
            delete m_mutex;  
            delete m_create_cond; 
			delete m_process_cond;

			printf("delete 3\n");
        }  

		void clear()  
        {  
            pthread_mutex_lock(m_mutex);  
            m_size = 0;  
            m_front = -1;  
            m_back = -1;  
            pthread_mutex_unlock(m_mutex);  
        }  
  
        bool isFull()const  
        {  
        	bool full = false;

            pthread_mutex_lock(m_mutex);  
            if(m_size >= m_max_size)  
            {  
                full = true;
            } 
            pthread_mutex_unlock(m_mutex);  

            return full;  
        }  
  
        bool isEmpty()const  
        {  
        	bool empty =false;
            pthread_mutex_lock(m_mutex);  
            if(!m_size)  
            {  
               empty = true;
            }  
            pthread_mutex_unlock(m_mutex);  
            return empty;  
        }  
          
        bool front(T& value)const  
        {  
            pthread_mutex_lock(m_mutex);  
            if(!m_size)  
            {  
                pthread_mutex_unlock(m_mutex);  
                return false;  
            }  
            value = m_array[m_front];  
            pthread_mutex_unlock(m_mutex);  
            return true;  
        }  
          
        bool back(T& value)const  
        {  
            pthread_mutex_lock(m_mutex);  
            if(!m_size)  
            {  
                pthread_mutex_unlock(m_mutex);  
                return false;  
            }  
            value = m_array[m_back];  
            pthread_mutex_unlock(m_mutex);  
            return true;  
        }  
  
        int size()const  
        {  
            int tmp = 0;  
            pthread_mutex_lock(m_mutex);  
            tmp = m_size;  
            pthread_mutex_unlock(m_mutex);  
            return tmp;  
        }  
  
        int maxSize()const  
        {  
            int tmp = 0;  
            pthread_mutex_lock(m_mutex);  
            tmp = m_max_size;  
            pthread_mutex_unlock(m_mutex);  
            return tmp;  
        }

		void releaseCond()
    	{
    		 pthread_cond_broadcast(m_create_cond);
			 pthread_cond_broadcast(m_process_cond);
    	}
  
        bool push(const T& item)  
        {  		
            pthread_mutex_lock(m_mutex); 
           
            if(m_size >= m_max_size)  
            {  
				if(pthread_cond_wait(m_create_cond, m_mutex) != 0)  //等待消费者取走数据
				{
					pthread_mutex_unlock(m_mutex);
                    return false; 
				}
		
            }  

            m_back = (m_back + 1) % m_max_size;  
            m_array[m_back] = item;  
            m_size++;  
            pthread_cond_broadcast(m_process_cond);  
            pthread_mutex_unlock(m_mutex);  

            return true;  
        }  
  
        bool pop(T& item)  
        { 	
			pthread_mutex_lock(m_mutex);
            if(m_size <= 0)  
            {  
         
                if(pthread_cond_wait(m_process_cond, m_mutex) != 0)  //等待生产者产生新的数据
                {  
                	pthread_mutex_unlock(m_mutex);
                    return false;  
                } 
            }  

            m_front = (m_front + 1) % m_max_size;  
            item = m_array[m_front];  
            m_size--;
			
			if(m_size < m_max_size)
			{
				pthread_cond_broadcast(m_create_cond);  
			}
			
            pthread_mutex_unlock(m_mutex); 
	
            return true;  
        }  

	public:
		long m_create_count;   //统计产生的字符串数
		long m_process_count;  //统计处理的字符串数
  
	private:
		pthread_mutex_t *m_mutex;  
        pthread_cond_t *m_process_cond; 
		pthread_cond_t *m_create_cond;
        T *m_array;  
        int m_size;  
        int m_max_size;  
        int m_front;  
        int m_back;  
};

#endif
