#include <stdio.h>
#include "cJSON.h"

void test1()
{
    cJSON *cjson_test = cJSON_CreateObject();
    cJSON_AddStringToObject(cjson_test, "name", "Miki");
    cJSON_AddNumberToObject(cjson_test, "age", 123);
    cJSON *cjson_skill = cJSON_CreateArray();
    cJSON_AddItemToArray(cjson_skill, cJSON_CreateString("Java"));
    cJSON_AddItemToArray(cjson_skill, cJSON_CreateString("Python"));
    cJSON_AddItemToObject(cjson_test, "skills", cjson_skill);
    cJSON *cjson_address = cJSON_CreateObject();
    cJSON_AddStringToObject(cjson_address, "country", "China");
    cJSON_AddNumberToObject(cjson_address, "zip-code", 111111);
    cJSON_AddItemToObject(cjson_test, "address", cjson_address);
    char *str = cJSON_PrintUnformatted(cjson_test);
    printf("%s", str);
}


void test3()
{
    cJSON *json, *json_value, *json_timestamp;
    char text[] = "{\"value\":1,\"timestamp\":\"2016-11-19T08:50:11\"}";
    json = cJSON_Parse(text);
    json_timestamp = cJSON_GetObjectItem(json, "timestamp");
    if (json_timestamp->type == cJSON_String)
    {
        //get the result in the valuestring
        printf("%s\r\n", json_timestamp->valuestring);
    }

    // char *str = cJSON_Print(json);
    // printf("%s", str);
}

void main(void)
{
    test1();
}