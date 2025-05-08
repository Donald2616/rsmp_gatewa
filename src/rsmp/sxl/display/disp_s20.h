#ifndef DISP_S20_H
#define DISP_S20_H

#include <stdbool.h>

/**
 * @file disp_s20.h
 * @brief Structures liées au message DISP/S20 - Lecture des templates disponibles.
 *
 * Ce fichier contient uniquement la définition des structures nécessaires pour représenter
 * une réponse DISP/S20. L'encodage et le décodage JSON sont gérés dans un fichier générique.
 */

/**
 * @struct TextOptions
 * @brief Options spécifiques à une zone de type "Text".
 */
typedef struct {
    char type[32];             ///< Type : "TextLine" ou "TextArea"
    char textColor[32];       ///< Couleur du texte (ex: "red", "white", etc.)
    char backgroundColor[32]; ///< Couleur de fond (par défaut : "black")
    char font[32];            ///< Nom de la police (ex: "5x7")
    char horizontalAlign[32]; ///< Alignement horizontal : Left, Center, Right
    char verticalAlign[32];   ///< Alignement vertical : Top, Middle, Bottom
    char orientation[32];     ///< Orientation : RightLeftTopBottom, etc.
} TextOptions;

/**
 * @struct GraphicOptions
 * @brief Options pour une zone graphique (zoneType == "Graphic").
 */
typedef struct {
    char negotiation[32]; ///< Mode de redimensionnement : RejectDifferent, Mosaic, etc.
    struct {
        int w; ///< Largeur de la zone
        int h; ///< Hauteur de la zone
    } size;
} GraphicOptions;

/**
 * @struct PredefinedOptions
 * @brief Options pour une zone prédéfinie (zoneType == "Predefined").
 */
typedef struct {
    char predefinedGroupId[32]; ///< Identifiant du groupe prédéfini
    struct {
        int w; ///< Largeur
        int h; ///< Hauteur
    } size;
} PredefinedOptions;

/**
 * @struct CustomOptions
 * @brief Options personnalisées (zoneType == "Custom").
 * @note La structure est vide par défaut, à définir par le fabricant.
 */
typedef struct {
    char dummy[1]; ///< Champ fictif pour éviter une structure vide
} CustomOptions;

/**
 * @struct ZoneObject
 * @brief Représente une zone au sein d’un template.
 */
typedef struct {
    char zoneId[32];                  ///< Identifiant unique de la zone
    char supportedColors[5][16];     ///< Tableau des couleurs supportées
    int colorCount;                  ///< Nombre de couleurs
    struct {
        int x; ///< Position X
        int y; ///< Position Y
        int z; ///< Position Z
    } position;                      ///< Position dans la zone
    char zoneType[32];               ///< Type de zone : Text, Graphic, etc.
    TextOptions* textOptions;        ///< Options texte (si zoneType == "Text")
    GraphicOptions* graphicOptions;  ///< Options graphique (si zoneType == "Graphic")
    PredefinedOptions* predefinedOptions; ///< Options prédéfinies
    CustomOptions* customOptions;    ///< Options personnalisées
} ZoneObject;

/**
 * @struct TemplateObject
 * @brief Représente un template affichable avec ses zones.
 */
typedef struct {
    char templateId[64]; ///< Identifiant du template
    bool readonly;       ///< Indique si le template est modifiable
    ZoneObject* zones;   ///< Tableau de zones
    int zoneCount;       ///< Nombre de zones
} TemplateObject;

/**
 * @struct DispS20StatusResponse
 * @brief Réponse complète du message DISP/S20.
 */
typedef struct {
    TemplateObject* templates; ///< Tableau de templates
    int templateCount;         ///< Nombre de templates
} DispS20StatusResponse;

#endif // DISP_S20_H
