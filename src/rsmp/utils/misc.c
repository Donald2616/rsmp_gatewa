/**
* @file misc.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2023-07-10
* 
* @copyright Copyright (c) 2023
* 
*/
#include "config.h"

#include <stdio.h>
#include <string.h>

#define RSMP_UTILS_MISC_C
#include "misc.h"

/**
 * @ingroup misc
 * @{
 */


/**
 * \brief decode une chaine de paramètres
 *
 * \param param la chaine à décoder
 * \param sep le séparateur
 * \param[out] liste des paramètres décodé
 * \return le nombre de parametre
 */
/*int decodeparam(char *param, char sep, char ***argv)
{
	int argc=1;
		int i = 0;
	char *s = param, *p;

	if (param == NULL || *param =='\0') {
		*argv=NULL;
		return 0;
	}

	// On compte
	while(*s!='\0') {
		if (*s == sep)
			argc++;
		s++;
	}
	s = param;
	if ((*argv = calloc(1, sizeof(**argv)*argc)) != NULL) {
		p = strchr(s, sep);
		(*argv)[i++] = s;
		while(p!=NULL) {
			*p++='\0';
			(*argv)[i++] = p;
			p = strchr(p, sep);
		}
	}
	return argc;
}
*/



/**@}*/