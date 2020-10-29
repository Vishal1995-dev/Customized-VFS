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
	int Permission;
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
	int Mode;
	PINODE ptr;
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
		Obj_UFDT.ufdt[i]=NULL;
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

int GetFDFromName(char *name)
{
	int i = 0;
	while(i<MAXFILES)
	{
		if(Obj_UFDT.ufdt[i] != NULL)
		{
			if(strcmp((Obj_UFDT.ufdt[i]->ptr->File_name),name)==0)
				{
					break;
				}
		}
		i++;
	}
	if(i == 50)
	{
		return -1;
	}
	else
	{
		return i;
	}
}

PINODE Get_Inode(char * name)
{
	PINODE temp = Head;
	int i = 0;
	
	if(name == NULL)
	{
		return NULL;
	}
	while(temp!= NULL)
	{
		if(strcmp(name,temp->File_name) == 0)
		{
			break;
		}
		temp = temp->next;
	}
	return temp;
}

int CreateFile(char *name,int permission)
{
	if(name==NULL)
	{
		return -1;
	}
	PINODE temp = Get_Inode(name);
	if(temp!=NULL)
	{
		return -3;
	}
	int i = 0;
	temp=Head;
	while(temp->Link_Count!=0 && i<MAXFILES)
	{
		i+=1;
		temp=temp->next;
	}
	if(i==MAXFILES)
	{
		return -2;
	}
	else
	{
		strcpy(temp->File_name,name);
		temp->Link_Count=1;
		temp->Ref_Count=1;
		temp->File_Type=1;
		temp->Permission=permission;
		temp->Data=(char *) malloc (1024);
		
		PFILETABLE ptable =(PFILETABLE)malloc(sizeof(FILETABLE));
		ptable->Read_Offset=0;
		ptable->Write_Offset=0;
		ptable->Count=1;
		ptable->Mode=1;
		ptable->ptr=temp;
		
		Obj_Super.Free_Inodes-=1;
		i=0;
		while(Obj_UFDT.ufdt[i]!=NULL)
		{
			i++;
		}
		Obj_UFDT.ufdt[i]=ptable;
		return i;
	}
}

void LS()
{
	PINODE temp = Head;
	while(temp->Link_Count!=0)
	{
		printf("%s\n",temp->File_name);
		temp=temp->next;
	}
}

int Fstat(int fd)
{
	PFILETABLE ptable=Obj_UFDT.ufdt[fd];
	if(ptable==NULL)
	{
		return -2;
	}
	else
	{
		PINODE temp = NULL;
		temp=ptable->ptr;
		printf("Name : %s\n",temp->File_name);
		printf("File Size : %d\n",temp->File_Size);
		printf("File Type : %d\n",temp->File_Type);
		printf("Actual File Size : %d\n",temp->Actual_FileSize);
		printf("Link Count : %d\n",temp->Link_Count);
		return 0;
	}
}

int Stat(char *name)
{
	PINODE temp = Get_Inode(name);
	if(temp==NULL)
	{
		return -2;
	}
	printf("Name : %s\n",temp->File_name);
	printf("File Size : %d\n",temp->File_Size);
	printf("File Type : %d\n",temp->File_Type);
	printf("Actual File Size : %d\n",temp->Actual_FileSize);
	printf("Link Count : %d\n",temp->Link_Count);
	return 0;
}


int main()
{
	//Variable declaration
	char str[80];
	char command[4][80];
	int count=0,ret=0;
	
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
			if(strcmp(command[0],"ls") == 0)
			{
				LS();
			}
			else if(strcmp(command[0],"help")==0)
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
			if(strcmp(command[0],"stat") == 0)
			{
				ret = Stat(command[1]);
				if(ret == -1)
					printf("ERROR : Incorrect parameters\n");
				if(ret == -2)
					printf("ERROR : There is no such file\n");
				continue;
			}
			else if(strcmp(command[0],"fstat") == 0)
			{
				ret = Fstat(atoi(command[1]));
				if(ret == -1)
					printf("ERROR : Incorrect parameters\n");
				if(ret == -2)
					printf("ERROR : There is no such file\n");
				continue;
			}
			else if(strcmp(command[0],"man")==0)
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
			if(strcmp(command[0],"create") == 0)
			{
				ret = CreateFile(command[1],atoi(command[2]));
				if(ret >= 0)
					printf("File is successfully created with file descriptor : %d\n",ret);
				if(ret == -1)
					printf("ERROR : Incorrect parameters\n");
				if(ret == -2)
					printf("ERROR : There is no inodes\n");
				if(ret == -3)
					printf("ERROR : File already exists\n");
				if(ret == -4)
					printf("ERROR : Memory allocation failure\n");
				continue;
			}
			else
			{
				printf("Command not found\n");
			}
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

