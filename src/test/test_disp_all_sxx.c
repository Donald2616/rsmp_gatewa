#include <stdio.h>
#include <string.h>
#include "rsmp/sxl/display/disp_s10.h"
#include "rsmp/sxl/display/disp_s20.h"
#include "rsmp/sxl/display/disp_s30.h"
#include "rsmp/sxl/display/disp_s40.h"
#include "rsmp/sxl/display/disp_json_generic.h"

/*
int main() {
    DispS10StatusResponse res = {
        .displayType = "Mixed",
        .templateType = "Static",
        .minDisplayTime = 1000,
        .maxObjectIdSize = 16,
        .maxSupportedTemplate = 2,
        .maxSupportedZone = 5,
        .maxSupportedSequence = 5,
        .maxSupportedAnimation = 5,
        .maxSupportedAvailableMsg = 5,
        .maxSupportedDisplayPool = 255,
        .maxScrollSpeed = 15,
        .size = { .width = 340, .height = 80 },
        .textDisplayable = 1,
        .predefinedGroupCount = 1
    };

    static PredefinedGroupObject groups[1];
    strcpy(groups[0].predefinedGroupId, "PDEF_FLASH");
    groups[0].enumCount = 2;
    strcpy(groups[0].supportedEnum[0], "OFF");
    strcpy(groups[0].supportedEnum[1], "FLASH_ON");

    res.predefinedGroups = groups;

    struct json_object* json = encode_disp_s10_status_response(&res);
    printf("%s\n", json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY));
    json_object_put(json);

    return 0;
}
    */

void test_disp_s10(void)
{

    DispS10StatusResponse res = {
        .displayType = "Mixed",
        .templateType = "Static",
        .minDisplayTime = 1000,
        .maxObjectIdSize = 16,
        .maxSupportedTemplate = 2,
        .maxSupportedZone = 5,
        .maxSupportedSequence = 5,
        .maxSupportedAnimation = 5,
        .maxSupportedAvailableMsg = 5,
        .maxSupportedDisplayPool = 255,
        .maxScrollSpeed = 15,
        .size = { .width = 340, .height = 80 },
        .textDisplayable = 1,
        .predefinedGroupCount = 1
    };

    static PredefinedGroupObject groups[1];
    strcpy(groups[0].predefinedGroupId, "PDEF_FLASH");
    groups[0].enumCount = 2;
    strcpy(groups[0].supportedEnum[0], "OFF");
    strcpy(groups[0].supportedEnum[1], "FLASH_ON");

    res.predefinedGroups = groups;

    struct json_object* json = encode_disp_s10_status_response(&res);
    printf("%s\n", json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY));
    json_object_put(json);

}

void test_disp_s20(void){

    // === ALLOCATION de la réponse DISP/S20 ===
    DispS20StatusResponse response;
    response.templateCount = 1;
    response.templates = calloc(response.templateCount, sizeof(TemplateObject));

    TemplateObject* tpl = &response.templates[0];
    strncpy(tpl->templateId, "TEMPLATE_OLD_SCHOOL", sizeof(tpl->templateId));
    tpl->readonly = true;
    tpl->zoneCount = 1;
    tpl->zones = calloc(tpl->zoneCount, sizeof(ZoneObject));

    // === Définition d’une zone ===
    ZoneObject* zone = &tpl->zones[0];
    strncpy(zone->zoneId, "LIGNE_1", sizeof(zone->zoneId));
    zone->colorCount = 1;
    strncpy(zone->supportedColors[0], "yellow", sizeof(zone->supportedColors[0]));

    zone->position.x = 1;
    zone->position.y = 1;
    zone->position.z = 0;

    strncpy(zone->zoneType, "Text", sizeof(zone->zoneType));

    // === Définition d’options de texte ===
    zone->textOptions = calloc(1, sizeof(TextOptions));
    strncpy(zone->textOptions->type, "TextLine", sizeof(zone->textOptions->type));
    strncpy(zone->textOptions->textColor, "yellow", sizeof(zone->textOptions->textColor));
    strncpy(zone->textOptions->backgroundColor, "black", sizeof(zone->textOptions->backgroundColor));
    strncpy(zone->textOptions->font, "5x7", sizeof(zone->textOptions->font));
    strncpy(zone->textOptions->horizontalAlign, "Center", sizeof(zone->textOptions->horizontalAlign));
    strncpy(zone->textOptions->verticalAlign, "Middle", sizeof(zone->textOptions->verticalAlign));
    strncpy(zone->textOptions->orientation, "LeftRightTopBottom", sizeof(zone->textOptions->orientation));

    // === SERIALISATION JSON ===
    struct json_object* j = encode_disp_s20_status_response(&response);
    if (j) {
        printf("========== DISP/S20 JSON ==========\n");
        printf("%s\n", json_object_to_json_string_ext(j, JSON_C_TO_STRING_PRETTY));
        json_object_put(j); // Libération JSON
    }

    // === LIBÉRATION MÉMOIRE ===
    free(zone->textOptions);
    free(tpl->zones);
    free(response.templates);
}


void test_disp_s30(void){

    DispS30StatusResponse response;
    response.msgIdCount = 2;
    response.msgIdList = malloc(response.msgIdCount * sizeof(char*));
    response.msgIdList[0] = strdup("HELLO_WORLD");
    response.msgIdList[1] = strdup("SECOND_MESSAGE");

    struct json_object* json = encode_disp_s30_status_response(&response);
    printf("JSON généré :\n%s\n", json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY));

    const char* json_str = json_object_to_json_string(json);
    DispS30StatusResponse decoded;
    decode_disp_s30_status_response(json_str, &decoded);

    printf("Message décodé :\n");
    for (size_t i = 0; i < decoded.msgIdCount; ++i) {
        printf("- %s\n", decoded.msgIdList[i]);
    }

    free_disp_s30_status_response(&response);
    free_disp_s30_status_response(&decoded);
    json_object_put(json);
}

void test_disp_s40(void){

// Création d'une structure de test
    DispS40StatusResponse response;
    response.displayPoolCount = 1;
    response.displayPoolList = calloc(1, sizeof(DisplayPoolElementObject));

    strcpy(response.displayPoolList[0].msgId, "HELLO_WORLD");
    response.displayPoolList[0].maxWaitTime = 0;
    response.displayPoolList[0].displayDuration = 0;
    response.displayPoolList[0].displayPriority = 100;
    strcpy(response.displayPoolList[0].dynamicData, "");

    // Encodage JSON
    struct json_object* j = encode_disp_s40_status_response(&response);
    const char* json_str = json_object_to_json_string_ext(j, JSON_C_TO_STRING_PRETTY);
    printf("JSON généré :\n%s\n", json_str);

    // Décodage
    DispS40StatusResponse decoded;
    decode_disp_s40_status_response(json_str, &decoded);
    printf("Message décodé : %s\n", decoded.displayPoolList[0].msgId);

    // Nettoyage
    free_disp_s40_status_response(&response);
    free_disp_s40_status_response(&decoded);
    json_object_put(j);

}