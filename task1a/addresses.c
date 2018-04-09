#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int addr5;
int addr6;

int foo();
void point_at(void *p);
void print_arr();
void task1d();
int main (int argc, char** argv){
    int addr2;
    int addr3;
    char* yos="ree";
    int * addr4 = (int*)(malloc(50));
    printf("- &addr2: %p  print p&\n",&addr2);
    printf("- &addr3: %p\n",&addr3);
    printf("- foo: %p   print the addresses in memo of foo\n",foo);
    printf("- &addr5: %p\n",&addr5);
    printf("- &addr6: %p\n",&addr6);

	point_at(&addr5);
	print_arr();
	task1d();
	
    printf("- &addr6: %p\n",&addr6);
    printf("- yos: %p\n",yos);
    printf("- addr4: %p   heap?\n",addr4);
    printf("- &addr4: %p   stack?\n",&addr4);
    return 0;
}

void task1d(){
	int iarray[] = {0x01234567,0x89ABCDEF,0x13579BDF};
	char carray[] = {'a','b','c'};
	int* iarrayPtr=iarray;
	char* carrayPtr=carray;
	int i;
printf("----------------------------------\n");
printf("\n");	
for(i=0;i<3;i++){
		printf("iarray[%d]= %p  ",i,iarrayPtr[i]);
}
printf("\n");
for(i=0;i<3;i++)
{
	printf("carray[%d]= %c  ",i,*carrayPtr);
	++carrayPtr;	
}
printf("\n----------------------------------\n");
}


int foo(){
    return -1;
}

void point_at(void *p){
    int local;
	static int addr0 = 2;
    static int addr1;


    long dist1 = (size_t)&addr6 - (size_t)p;
    long dist2 = (size_t)&local - (size_t)p;
    long dist3 = (size_t)&foo - (size_t)p;
    
    printf("dist1: (size_t)&addr6 - (size_t)p: %ld\n",dist1);
    printf("dist2: (size_t)&local - (size_t)p: %ld\n",dist2);
    printf("dist3: (size_t)&foo - (size_t)p:  %x\n",dist3);
	
	printf("- addr0: %p\n", & addr0);
    printf("- addr1: %p\n",&addr1);
}

void print_arr(){
	int iarray[3];
	int iarray2[10];
	char carray[4];
/*
	printf("-------------------------------------------------------\n");
	printf("- &iarray: %p\n",&iarray);
	printf("- iarray: %d\n",*iarray);
	printf("- &iarray[1]: %p \n",&iarray[1]);
	printf("- iarray[1]: %p \n",iarray[1]);
	printf("- &iarray[2]: %p \n",&iarray[2]);
	printf("- iarray[2]: %p \n",iarray[2]);
	printf("- &iarray+1: %p\n",&iarray+1);
	printf("-------------------------------------------------------\n");
	printf("- &carray: %p\n",&carray);
	printf("- carray: %c\n",*carray);
	printf("- &carray[1]: %p \n",&carray[1]);
	printf("- &carray[1]+1: %p \n",&carray[1]+1);
	printf("- carray[1]: %p \n",carray[1]);
	printf("- &carray[2]: %p \n",&carray[2]);
	printf("- carray[2]: %p \n",carray[2]);
	printf("- &carray+1: %p\n",&carray+1);
	printf("sizeof char:%d\n",sizeof(char));
	printf("sizeof int:%d\n",sizeof(int));
*/
	printf("------------SHAPIRA DEFECT------------------------\n");
	printf("- &iarray: %p\n",&iarray);
	printf("- &iarray+1: %p\n",&iarray+1);
	printf("- &iarray2  : %p\n",&iarray2);
	printf("- &iarray2+1: %p\n",&iarray2+1);
	printf("- &iarray2+2: %p\n",&iarray2+2);
	printf("dist: %d,\n",(size_t)(&iarray2+1)-(size_t)(&iarray2));
	printf("dist: %d,\n",(size_t)(&iarray2+2)-(size_t)(&iarray2));
	printf("- &carray: %p\n",&carray);
	printf("- &carray+1: %p\n",&carray+1);
	printf("- &carray+2: %p\n",&carray+2);
	printf("------------SHAPIRA DEFECT------------------------\n");

}

