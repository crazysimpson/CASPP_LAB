#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct block{
	int valid;
	int tag;
	int time;
} Block;

typedef struct Set{
	int s_size;
	Block *blocks;
} Set;

typedef struct cache{
	int s;
	int E;
	int b;
	Set *sets;
} Cache;

void collectcache(Cache *c);
void errorinfo();
void initcache(Cache *c, int s, int E, int b);
void simulate(Cache *c, char *file, int flag, int *Hit, int *Miss, int *Evic);
int LRU(Set * set);

int main(int argc, char *argv[])
{
	char * file = NULL;
	int s=0;
	int E=0;
	int b=0;
	int flag = 0;
	int hit=0;
	int miss=0;
	int evic=0;

	Cache c;

	printf("%d\n", argc);
	for(int i=1; i<argc; i++)
	{
		if(strcmp(argv[i], "-s") == 0){
			s = atoi(argv[++i]);
			if(s<=0)
			{
				errorinfo();
				return 0;
			}
			continue;
		}
		if(strcmp(argv[i], "-v") == 0){
			flag=1;
			continue;	
		}
		if(strcmp(argv[i], "-h") == 0){
			errorinfo();
			return 0;
		}
		if(strcmp(argv[i], "-E") == 0){
			E = atoi(argv[++i]);
			if(E<=0){
				errorinfo();
				return 0;
			}
			continue;
		}
		if(strcmp(argv[i], "-b") == 0){
			b = atoi(argv[++i]);
			if(b<=0){
				errorinfo();
				return 0;
			}
			continue;
		}
		if(strcmp(argv[i], "-t") == 0 ){
			if((i+1)==argc || *argv[i+1]=='-'){
				errorinfo();
				return 0;
			}
			file = argv[++i];	
		}
	}
	printf("s:%d, E:%d, b:%d, file: %s\n", s,E,b, file);
	initcache(&c, s, E, b);
	simulate(&c, file, flag, &hit, &miss, &evic);
	collectcache(&c);
	printSummary(hit, miss, evic);
	return 0;
}

void collectcache(Cache *c){
	free(c->sets->blocks);
	free(c->sets);
	return;
}

void initcache( Cache *c, int s, int E, int b){
		c->s=s;
		c->E=E;
		c->b=b;
		c->sets =(Set*) malloc(sizeof(Set)*(2<<s));
		for(int i=0; i< (2<<s);i++)
		{	(c->sets[i]).s_size=0;
			(c->sets+i)->blocks=(Block*)malloc(sizeof(Block)*E);
			for(int p=0;p<E;p++){
				((c->sets+i)->blocks+p)->valid=0;
				((c->sets+i)->blocks+p)->time=0;
				((c->sets+i)->blocks+p)->tag=0;	
			}
		}
}

int LRU(Set *set){
	int result=0;
	int flag=0;
	for(int i=0; i<set->s_size; i++){
		if(set->blocks[i].time > flag)
		{
			flag = set->blocks[i].time;
			result = i;
		}
	}
	return result;
}

void simulate(Cache *c, char *file, int flag, int *Hit, int * Miss, int *Evic)
{
	char line[100];
	int len=100;
	FILE * read = fopen(file, "r");
	char * ins;
	char * add;
	//char* ss;
	unsigned long addr;
	
	Set * selected_set;
	
	
	while(fgets(line, len, read) != NULL){
		ins=strtok(line, " ,");
		add = strtok(NULL, " ,");
		//ss = strtok(NULL, " ,");
		addr =(unsigned long) strtol(add,NULL ,16);
		//strcat(line, ",");
		strcat(line, add);
		strcat(line, ",");
//		strcat(line, ss);
		selected_set = &c->sets[(addr<<(64 - c->b - c->s))>>(64-c->s)];
		int i=0;
		int find=0;
		
		if((*ins) == 'I')
			continue;

		for(i=0; i<c->E; i++){
			if(selected_set->blocks[i].valid==0){
				continue;
			}else if(selected_set->blocks[i].tag==(addr>>(c->b+c->s))){
				find=1;
				selected_set->blocks[i].time = 1;
				if((*ins)=='M'){
					(*Hit)+=2;
					strcat(line, " hit hit");
				}else{
					(*Hit)++;
					strcat(line, " hit");
				}
					
			}else{
				selected_set->blocks[i].time++;
			}
		}
		if(find==0 && i>=c->E){
			int evic=0;
			(*Miss)++;
			strcat(line, " miss");
			if(selected_set->s_size<c->E){
				
				selected_set->blocks[selected_set->s_size].tag = addr>>(c->b+c->s);
				selected_set->blocks[selected_set->s_size].time = 1;
				selected_set->blocks[selected_set->s_size].valid=1;
				selected_set->s_size++;
			}else{
				(*Evic)++;
				strcat(line, " eviction");
				 evic = LRU(selected_set);
				selected_set->blocks[evic].tag = addr>>(c->b+c->s);
				selected_set->blocks[evic].time = 1;

			}
			if((*ins)=='M'){
				strcat(line, " hit");
				(*Hit)++;
			}
		}
		if(flag == 1){
			printf("%s\n", line);
		}
		
	}	
}


void errorinfo(){
	printf("example: ./csim -v -s 4 -E 1 -b 4 -t traces/yi.trace\n");
	printf("example: ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
}
