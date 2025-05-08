/**
* @file bcd.h
* @author Yann Cohen (yann.cohen@svme.eu)
* @brief
* @version 0.1
* @date 2023-11-28
* 
* @copyright Copyright (c) 2023
* 
*/
#ifndef CRYPT_BCD_H
#define CRYPT_BCD_H
/**
 * @defgroup bcd 
 * @ingroup cipher
 * @brief Encodage bcd
 * 
 *
    char const *txt = "Oh la belle bleue !";
    size_t len = strlen(txt);
    char *bcd = bcd_encode(txt, len);
    size_t lbis;
    char *aze = bcd_decode(bcd, &lbis);
    printf("txt: '%s' (%lu)\nbdc: '%s' (%lu)\naze: '%s' (%lu)\n", txt, len, bcd, strlen(bcd), aze, lbis);
    printf("strcmp:%d\n", strcmp(txt, aze));
    free(bcd);
    free(aze);
    bcd = bcd_encode(txt, len);
    aze = bcd_decode(bcd, &lbis);
    printf("txt: '%s' (%lu)\nbdc: '%s' (%lu)\naze: '%s' (%lu)\n", txt, len, bcd, strlen(bcd), aze, lbis);
    printf("strcmp:%d\n", strcmp(txt, aze));

txt: 'Oh la belle bleue !' (19)
bdc: '4F68206C612062656C6C6520626C6575652021' (38)
aze: 'Oh la belle bleue !' (19)
strcmp:0
txt: 'Oh la belle bleue !' (19)
bdc: '4F:68:20:6C:61:20:62:65:6C:6C:65:20:62:6C:65:75:65:20:21' (56)
aze: 'Oh la belle bleue !' (19)
strcmp:0
 * 
 * 
 * @{
 */

/**
 * @brief encode un buffer en binary coded decimal
 * 
 * Le texte encodé aura une taille double du buffer original si
 * aucun séparateur utilisé ou bien triple si un séparateur est
 * défini.
 * 
 * \remark la sortie est allouée dynamiquement, il appartient à
 *         l'appelant de la libérer...
 * 
 * @param buf  buffer à encoder 
 * @param len  longueur du buffer (efficace)
 * @param sep  le séparateur à applique si !=0, 
 *             __NE DOIT PAS ÊTRE UN CARACTÈRE HEXADÉCIMAL__
 * @return char* la chaine de caractère de l'encodage qui intègre 
 *               un terminateur de chaîne, si Ok
 */
char *bcd_encode_sep(char const *buf, size_t len, char sep);

/**
 * @brief Encode sans séparateur
 * 
 * cf. bcd_encode_sep()
 * 
 * @param b  buffer à encoder 
 * @param l  longueur du buffer (efficace)
 */
#define bcd_encode(b, l) bcd_encode_sep((b), (l), '\0')
/**
 * @brief 
 * 
 * @brief Encode avec séparateur '`:`'
 * 
 * cf. bcd_encode_sep()
 * 
 * @param b  buffer à encoder 
 * @param l  longueur du buffer (efficace)
 */
#define bcd_encode(b, l) bcd_encode_sep((b), (l), NULL)
// \todo il y a un loup avec bcd encode. laquelle des deux fonctions est la bonne ?

/**
 * @brief decode une chaîne de caractère bdc
 * 
 * \remark Détecte si il existe un séparateur d'octet ou non.
 * 
 * @param[in]  buf    buffer (chaîne de caractère avec un terminateur) 
 *                    à décoder.
 * @param[out] len    longeur décodée
 * 
 * @return char* la chaine décodé si OK, NULL si échec
 */
char *bcd_decode(char const *buf, size_t *len);


/**@}*/
#endif //CRYPT_BCD_H