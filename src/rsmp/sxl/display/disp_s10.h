#ifndef DISP_S10_H
#define DISP_S10_H

/**
 * @file disp_s10.h
 * @brief Structures liées au message DISP/S10 - Description des capacités de l'afficheur
 */

typedef struct {
    unsigned int width;
    unsigned int height;
} SizeWebObject;

typedef struct {
    char predefinedGroupId[64];
    char supportedEnum[10][32]; // Max 10 valeurs par groupe
    unsigned int enumCount;
} PredefinedGroupObject;

typedef struct {
    char displayType[32];
    char templateType[32];
    unsigned int minDisplayTime;
    unsigned int maxObjectIdSize;
    unsigned int maxSupportedTemplate;
    unsigned int maxSupportedZone;
    unsigned int maxSupportedSequence;
    unsigned int maxSupportedAnimation;
    unsigned int maxSupportedAvailableMsg;
    unsigned int maxSupportedDisplayPool;
    unsigned int maxScrollSpeed;
    SizeWebObject size;
    int textDisplayable; // booléen (0 = false, 1 = true)
    PredefinedGroupObject* predefinedGroups;
    unsigned int predefinedGroupCount;
} DispS10StatusResponse;

#endif // DISP_S10_H
