#ifndef __BERKELEYFUNCTIONS__
#define __BERKELEYFUNCTIONS__

#include<db.h>
#include<windows.h>
#include<stdio.h>
#include<string.h>
#include<db.h>
#pragma comment(lib,"libdb48.lib")

	void putArticle(DB** dbp);
	void getArticle(DB** dbp);
	void closeDb(DB** dbp);
	void createDb(DB** dbp, HANDLE** memoryHandle);


#endif