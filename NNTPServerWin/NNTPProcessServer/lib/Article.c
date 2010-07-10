/*
 * Article.c
 *
 *  Created on: May 7, 2010
 *      Author: Barrios, Nahuel.
 */

#include <windows.h>
#include <stdio.h>
#include "Article.h"

char* getArticleIDAsString(stArticle anArticle){
	char elString[12];
	ZeroMemory(elString,12);
	sprintf(elString, "%d", anArticle.uiArticleID);
	return elString;
}
