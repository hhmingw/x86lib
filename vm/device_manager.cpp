/**
Copyright (c) 2007 - 2009 Jordan "Earlz/hckr83" Earls  <http://www.Earlz.biz.tm>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.
   
THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

This file is part of the x86Lib project.
**/
#include <iostream>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <x86lib.h>


namespace x86Lib{
//The lack of indentation for namespaces is intentional...
using namespace std;

MemorySystem::MemorySystem()
{
	locked=0;
}
/**Because we must resize the list with realloc, we use malloc through out this for safety.**/
void MemorySystem::Add(uint32_t low,uint32_t high,MemoryDevice *memdev){
	DeviceRange_t device;
	
	/* Check For Overlapping Addresses */
	for(unsigned int i = 0; i < memorySystemVector.size(); i++)
	{
		device = memorySystemVector[i];
		
		if( device.high <= high && 
		    device.low >= low )
		{
			printf("System_excp\n");
			fflush(stdout);
			throw new System_excp();
		}
	}

	device.high = high;
	device.low = low;
	device.memdev = memdev;
	
	/* Place Device in Memory System Vector */
	memorySystemVector.push_back( device );
}

void MemorySystem::Remove(uint32_t low,uint32_t high)
{
	
}
	
void MemorySystem::Remove(MemoryDevice *memdev)
{
}
void MemorySystem::Read(uint32_t address,int size,void *b)
{
	uint8_t* buffer=(uint8_t*)b;
	DeviceRange_t device;
	
	if(size == 0)
	{
		return;
	}
		
	size--;
		
	for(unsigned int i = 0; i < memorySystemVector.size(); i++)
	{
		device = memorySystemVector[i];
		
		if(device.low <= address && 
		   device.high >= address)
		{
			//we found a region matching.
			if( (address + size) > device.high )
			{
				//One device range will not cover the whole request.
				device.memdev->Read(address, device.high - address + 1, buffer);
				size -= device.high - address;
				
				buffer += device.high - address + 1; //bug?
				
				address += device.high - address + 1;
				Read( address, size+1 , buffer );
			}
			else
			{
				//correct absolute address to a relative one
				address-=device.low;
				device.memdev->Read(address, size + 1, buffer);
			}
			return;
		}
	}
	throw Mem_excp(address);
}



void MemorySystem::Write(uint32_t address,int size,void *b)
{
	uint8_t *buffer=(uint8_t*)b;
	DeviceRange_t device;
		
	if( size==0 )
	{
		return;
	}
		
	size--;
		
	for( unsigned int i = 0; i < memorySystemVector.size(); i++ )
	{
		device = memorySystemVector[i];
		
		if(device.low <= address && 
		   device.high >= address)
		{
			//we found a region matching.
			if( (address + size) > device.high)
			{
				//One device range will not cover the whole request.
				device.memdev->Write(device.high - address, device.high - address + 1, buffer);
				
				size -= device.high - address;
				
				buffer += device.high - address;

				address += device.high - address + 1;
				
				Write(address , size + 1, buffer);			
			}
			else
			{
				//correct absolute address to a relative one
				address-=device.low;
				device.memdev->Write(address,size+1,buffer);
			}
			return;
		}
	}
	throw Mem_excp(address);
}

int  MemorySystem::RangeFree(uint32_t low,uint32_t high)
{
	return 1;
}



PortSystem::PortSystem(){
	count=0;
	//list=NULL;
}
void PortSystem::Add(uint16_t low,uint16_t high,PortDevice *portdev){
	int i;
	for(i=0;i<count;i++){
		if(list[i].high<=high && list[i].low>=low){
			throw new System_excp();
		}
	}
	if(count==0){
		list=(DeviceRange_t*)malloc(sizeof(DeviceRange_t)*1);
		//count++;
	}else{
		void *t=list;
		list=(DeviceRange_t*)realloc(list,sizeof(DeviceRange_t)*count+1);
		if(list==NULL){
			list=(DeviceRange_t*)t; //restore old pointer so we can free it
			throw Default_excp(__FILE__,__FUNCTION__,__LINE__);
		}
		//count++;
	}
	list[count].high=high;
	list[count].low=low;
	list[count].portdev=portdev;
	count++;
}



void PortSystem::Read(uint16_t address,int size,void *buffer){
	int i;
	if(size==0){return;}
	size=size-1;
	for(i=0;i<count;i++){
		if(list[i].low<=address && list[i].high>=address){
			//we found a region matching.
			list[i].portdev->Read(address,size+1,buffer);
			return;
		}
	}
	throw Mem_excp(address);
}



void PortSystem::Write(uint16_t address,int size,void *buffer){
	if(size==0){return;}
	for(int i=0;i<count;i++){
		if(list[i].low<=address && list[i].high>=address){
            list[i].portdev->Write(address,size,buffer);
			return;
		}
	}
	throw Mem_excp(address);
}

void Remove(uint16_t low,uint16_t high){}
void Remove(PortDevice *portdev){}
int RangeFree(uint32_t low,uint32_t high){return 1;}




};

