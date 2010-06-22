/*
 * Article.h
 *
 *  Created on: May 7, 2010
 *      Author: Barrios, Nahuel.
 */

#ifndef ARTICLE_H_
#define ARTICLE_H_

/**
 * Estructura que referencia a un Articulo de noticia.
 */
typedef struct stArticle {
    unsigned int uiArticleID;	/*	ID de la noticia.	*/
    char* sNewsgroup;			/*	El grupo al que pertenece la noticia.	*/
    char* sHead;				/*	El header de la noticia.	*/
    char* sBody;				/*	El body de la noticia	*/

} stArticle;

char* getArticleIDAsString(stArticle anArticle);

#endif /* ARTICLE_H_ */
