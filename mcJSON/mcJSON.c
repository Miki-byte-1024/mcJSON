#include <stdio.h>
#include "mcJSON.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

char *mcJSON_CopyStr(const char *const value)
{
    char *new_value = (char *)malloc(sizeof(char));
    int length = strlen(value) + sizeof("");
    memcpy(new_value, value, length);
    return new_value;
}

mcJSON *createObjectItem()
{
    mcJSON *pointer = malloc(sizeof(mcJSON));
    if (pointer != NULL)
    {
        memset(pointer, '\0', sizeof(mcJSON));
        pointer->type = mcJSON_Object;
    }
    return pointer;
}

mcJSON *createStringItem(const char *const value)
{
    mcJSON *obj = createObjectItem();
    if (obj)
    {
        obj->type = mcJSON_String;
        char *new_value = mcJSON_CopyStr(value);
        obj->stringValue = new_value;
    }
    return obj;
}

mcJSON *createNumberItem(double value)
{
    mcJSON *obj = createObjectItem();
    if (obj)
    {
        obj->type = mcJSON_Number;
        obj->doubleValue = value;
        if (value >= INT_MAX)
        {
            obj->intValue = INT_MAX;
        }
        else if (value <= INT_MIN)
        {
            obj->intValue = INT_MIN;
        }
        else
        {
            obj->intValue = value;
        }
    }
    return obj;
}

void addNumberToObject(mcJSON *obj, const char *const key, double value)
{
    if (obj == NULL || key == NULL)
    {
        return;
    }
    mcJSON *numberItem = createNumberItem(value);
    if (!addObjToArray(obj, numberItem, key))
    {
        deleteItem(numberItem);
    }
}

mcJSON_Bool addObjToArray(mcJSON *obj, mcJSON *item, const char *const key)
{
    if (key == NULL || obj == NULL || item == NULL)
    {
        return 0;
    }
    //复制一个key
    char *new_key = mcJSON_CopyStr(key);
    if (new_key)
    {
        item->key = new_key;
        mcJSON *tempChild = obj->child;
        //如果child为空，新建一个节点
        if (tempChild == NULL)
        {
            obj->child = item;
            item->prev = item;
            item->next = NULL;
        }
        else
        {
            mcJSON *lastItem = tempChild->prev;
            lastItem->next = item;
            item->prev = lastItem;
            tempChild->prev = item;
            item->next = NULL;
        }
        return 1;
    }
    return 0;
}

void addStringToObject(mcJSON *obj, const char const *key, const char const *value)
{
    //创建一个child key value
    if (obj == NULL || key == NULL || value == NULL)
    {
        return;
    }
    mcJSON *strItem = createStringItem(value);
    addObjToArray(obj, strItem, key);
}

/**
 * 是否可以使用散列表优化
 * key 和 指针组成散列表
 */
mcJSON *getObjectItem(mcJSON *obj, char *key)
{
    obj = obj->child;
    while (obj != NULL)
    {
        if (obj->key != NULL && strcmp(obj->key, key) == 0)
        {
            return obj;
        }
        obj = obj->next;
    }
    return obj;
}

void deleteItem(mcJSON *obj)
{
    while (obj != NULL)
    {
        mcJSON *nextNode = obj->next;
        if (obj->child != NULL)
        {
            deleteItem(obj->child);
        }
        if (obj->key != NULL)
        {
            free(obj->key);
        }
        if (obj->stringValue != NULL)
        {
            free(obj->stringValue);
        }
        free(obj);
        obj = nextNode;
    }
}

unsigned char *getCurrentPointer(printbuffer *buffer)
{
    if (buffer == NULL)
    {
        return NULL;
    }
    // length += buffer->offset;
    return buffer->buffer + buffer->offset;
}
mcJSON_Bool printStr(unsigned char *str, printbuffer *buffer,mcJSON_Bool iskey)
{
    if (str == NULL || buffer == NULL)
    {
        return false;
    }

    unsigned char *current_print = getCurrentPointer(buffer);
    *current_print++ = '\"';
    while(*str){
        *current_print++ = *str++;
        buffer->offset++;
    }
    
    *current_print++ = '\"';
    if (iskey)
    {
        *current_print++ = ':';
        buffer->offset += 3;
    }else{
        buffer->offset += 2;
    }
    
    
    return true;
}
unsigned char *printObject(mcJSON *item, printbuffer *buffer)
{
    unsigned char *current_print = buffer->buffer;
    item = item->child;
    // int length = buffer->format ? 2:1;
    if (current_print != NULL)
    {
        *current_print++ = '{';
        buffer->offset++;
        while (item != NULL)
        {
            //获取偏移过后的指针
            if (printStr(item->key, buffer,true))
            {
                
                printValue(item,buffer);

                //判断是否打印逗号
                if (item->next != NULL)
                {   
                    current_print = getCurrentPointer(buffer);
                    *current_print++ = ',';
                    buffer->offset++;
                }
            }
            item = item->next;
        }
        current_print = getCurrentPointer(buffer);
        *current_print++ = '}';
        *current_print++ = '\0';
    }
    return buffer->buffer;
}

unsigned char *printValue(mcJSON *obj, printbuffer *buffer)
{
    unsigned char *result = NULL;
    //判断不同数据类型
    switch (obj->type)
    {
    case mcJSON_String:
        /* code */

        break;
    case mcJSON_Object:
        result = printObject(obj, buffer);
        break;
    default:
        break;
    }
    return result;
}

char *print(mcJSON *obj)
{
    printbuffer buffer;
    buffer.offset = 0;
    buffer.length = 512;
    buffer.buffer = malloc(sizeof(mcJSON) * buffer.length);
    char *result = (char *)printValue(obj, &buffer);
    return result;
    //头打印
    //打印key
    //打印value
    //尾部打印
}

int main()
{
    mcJSON *mcjson_test = createObjectItem();
    addStringToObject(mcjson_test, "name", "Lili");
    addNumberToObject(mcjson_test, "age", 11);
    //   deleteItem(mcjson_test);
    //   print(mcjson_test);
    // mcJSON *obj = getObjectItem(mcjson_test,"name");
    char* printed = print(mcjson_test);
    printf("%s",printed);

    return 0;
}