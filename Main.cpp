#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

#define MAXFILES 100
#define FILESIZE 1024;

struct SuperBlock
{
	int Total_Inodes;
	int Free_Inodes;
}Obj_Super;

struct inode
{
	char File_name[50];
	int Inode_number;
	int File_Size;
	int Actual_FileSize;	
	int Link_Count;
	int Ref_Count;
	int File_Type;
	char *Data;
	struct inode *next;
};

typedef struct inode INODE;
typedef struct inode * PINODE;
typedef struct inode ** PPINODE;

struct FileTable
{
	int Read_Offset;
	int Write_Offset;
	int Count;
	PINODE ptr;
	int Mode;
};

typedef FileTable FILETABLE;
typedef FileTable * PFILETABLE;

struct UFDT
{
	PFILETABLE ufdt[MAXFILES];
}Obj_UFDT;

PINODE Head = NULL; 	//Global pointer to inode

void CreateDILB()
{
	//Create linked list of Inodes
	int i=1;
	PINODE newn = NULL;
	PINODE temp = Head;
	while(i<=MAXFILES)	//Loop iterates 100 times
	{
		newn = (PINODE) malloc (sizeof(INODE));
		newn->Inode_number=i;
		newn->File_Size=FILESIZE;
		newn->Actual_FileSize=0;	
		newn->Link_Count=0;
		newn->Ref_Count=0;
		newn->File_Type=0;
		newn->Data=NULL;
		newn->next=NULL;
		
		if(Head == NULL)	//First inode		
		{
			Head=newn;
			temp=Head;
		}
		else			//Node second to last
		{
			temp->next=newn;
			temp=temp->next;
		}
		i++;
	}
}

void CreateSuperBlock()
{
	Obj_Super.Total_Inodes=MAXFILES;
	Obj_Super.Free_Inodes=MAXFILES;
}

void CreateUFDT()
{
	int i=0;
	for(i=0;i<MAXFILES;i++)
	{
		Obj_UFDT.ufdt[i]==NULL;
	}
}

void SetEnvironment()
{
	CreateDILB();
	CreateSuperBlock();
	CreateUFDT();
}

void DisplayHelp()
{
	printf("------------------------------------\n");
	printf("open: Used to open existing file\n");
	printf("read: Used to read existing file\n");
	printf("write: Used to write into existing file\n");
	printf("creat: Used to create new file\n");
	printf("------------------------------------\n");
}

void ManPage(char *str)
{
	if(strcmp(str,"open")==0)
	{
		printf("Desciiption : Used to open file\n");
		printf("Usage : open file_name mode\n");
	}
	else if(strcmp(str,"close")==0)
	{
		printf("Desciiption : Used to close file\n");
		printf("Usage : close file_name\n");	
	}
	else if(strcmp(str,"ls")==0)
	{
		printf("Desciiption : List all file\n");
		printf("Usage : ls\n");	
	}
	else
	{
		printf("Man page not found\n");
	}
}

int main()
{
	//Variable declaration
	char str[80];
	char command[4][80];
	int count=0;
	
	system("clear");
	printf("Customized Virtual File System\n");
	
	SetEnvironment();	//Create DILB
	
	while(1)
	{
		printf("Vishal's VFS :> ");
		fgets(str,80,stdin);	//Accept command
		fflush(stdin);
		
		count=sscanf(str,"%s %s %s %s",command[0],command[1],command[2],command[3]); //Break command into tokens
		if(count==1)
		{
			if(strcmp(command[0],"help")==0)
			{
				DisplayHelp();
			}
			else if(strcmp(command[0],"clear")==0)
			{
				system("clear");
			}
			else if(strcmp(command[0],"exit")==0)
			{
				printf("Thank you\n");
				system("clear");
				break;
			}
			else
			{
				printf("Command not found\n");
			}
		}
		else if(count==2)
		{
			if(strcmp(command[0],"man")==0)
			{
				ManPage(command[1]);
			}
			else
			{
				printf("Command not found\n");
			}
		}
		else if(count==3)
		{
		
		}
		else if(count==4)
		{
		
		}
		else
		{
			printf("Bad command\n");
		}
	}
	return 0;
}

