/*
 * Article.c
 *
 *  Created on: May 7, 2010
 *      Author: Barrios, Nahuel.
 */

#include <stdio.h>
#include "Article.h"

char* getArticleIDAsString(stArticle anArticle){
	char* elString;
	sprintf(elString, "%d", anArticle.uiArticleID);
	return elString;
}
