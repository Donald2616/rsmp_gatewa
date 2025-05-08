/**
* @file bcd.c
* @author Yann Cohen (yann.cohen@svme.eu)
* @brief
* @version 0.1
* @date 2023-11-28
* 
* @copyright Copyright (c) 2023
* 
*/
// #include "config.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define CRYPT_BCD_C
#include "bcd.h"

/**
 * @ingroup bcd
 * @{
 */


/**
 * @brief encode un demi caractère
 * 
 * @param c le carctère dont la partie basse sera encodée
 * @return char le caractère en hexdécimal majuscule
 */
static inline char c2a(char c) 
{
    c &= 0x0F;
    return (c<'\x0A')?('0'+c):('A'+c-0x0a); 
}

/**
 * @brief decode un caractère
 * 
 * @param c      le caractère hexadécimal majuscule ou minuscule
 * @return char  le code associé
 */
static inline char a2c(char c)
{
    if (c>'0'-1 && c<'9'+1) 
        return c - '0';
    else
    if (c>'a'-1 && c<'f'+1)
        return 0x0A + c - 'a';
    else
    if (c>'A'-1 && c<'F'+1)
        return 0x0A + c - 'A';
    return 0;
}

char *bcd_encode_sep(char const *buf, size_t len, char sep)
{
    char *out=NULL;
    if (len>0 && buf)
    {
        size_t max;
        if (sep && !isxdigit(sep))
        {
            max = len*3;
            out = calloc(max, sizeof(*out));
            size_t i=0, j=0;
            for(i=0, j=0; i<max; i++, j++)
            {
                out[i++] = c2a(buf[j]>>4);
                out[i++] = c2a(buf[j]&0x0F);
                out[i] = sep;
            }   
            out[max-1] = '\0';     
        }
        else
        {
            max = len*2;
            out = calloc(max+1, sizeof(*out));
            for(size_t i=0, j=0; i<max; i++, j++)
            {
                out[i++] = c2a(buf[j]>>4);
                out[i] = c2a(buf[j]&0x0F);
            }

        }
    }
    return out;
}

char *bcd_decode(char const *buf, size_t *lout)
{
    char *out = NULL;
    if (buf)
    {
        size_t len = strlen(buf);
        if (len>1 && isxdigit(buf[0]) && isxdigit(buf[1]))
        {
            out = calloc(len/2, sizeof(*out));
            if (out)
            {
                size_t i=0;
                *lout=0;
                while(i<len)
                {
                    out[*lout] = (a2c(buf[i++])<<4) & 0xf0;
                    out[*lout] |= a2c(buf[i++]);
                    (*lout)++;
                    if (isxdigit(buf[i])) continue;
                    i++;
                }
                
            }
        }
    }
    return out;
}




/**@}*/