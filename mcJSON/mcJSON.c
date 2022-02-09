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

unsigned char *getCurrentPointer(printbuffer *buffer, size_t needed)
{
    if (buffer == NULL || buffer->offset > buffer->length)
    {
        return NULL;
    }
    needed += buffer->offset;
    if (needed < buffer->length)
    {
        return buffer->buffer + buffer->offset;
    }
    //大于最大长度，重新分配内存
    return NULL;
}

mcJSON_Bool printNumber(int *number, printbuffer *buffer)
{
    if (buffer == NULL)
    {
        return false;
    }
    unsigned char strNumber[10] = {0};
    int length = sprintf(strNumber, "%d", *number);
    unsigned char *current_print = getCurrentPointer(buffer, length);
    if (current_print == NULL)
    {
        return false;
    }
    for (int i = 0; i < length; i++)
    {
        *current_print++ = strNumber[i];
    }
    buffer->offset += length;
    return true;
}

mcJSON_Bool printStr(unsigned char *str, printbuffer *buffer, mcJSON_Bool iskey)
{
    if (str == NULL || buffer == NULL)
    {
        return false;
    }
    size_t needed = 0;
    if (iskey)
    {
        needed = 3;
    }
    else
    {
        needed = 2;
    }

    unsigned char *current_print = getCurrentPointer(buffer, needed);
    if (current_print == NULL)
    {
        return false;
    }
    *current_print++ = '\"';
    while (*str)
    {
        *current_print++ = *str++;
        buffer->offset++;
    }

    *current_print++ = '\"';
    if (iskey)
    {
        *current_print++ = ':';
    }

    buffer->offset += needed;
    return true;
}

mcJSON_Bool printObject(mcJSON *item, printbuffer *buffer)
{
    //为了防止内存溢出，添加需要的空间大小
    size_t needed = 0;
    unsigned char *current_print = buffer->buffer;
    item = item->child;
    if (current_print != NULL)
    {
        needed = buffer->format ? 2 : 1;
        if (!buffer->format)
        {
            current_print = getCurrentPointer(buffer, needed);
            if (current_print == NULL)
            {
                return false;
            }

            *current_print++ = '{';
        }
        buffer->offset += needed;
        while (item != NULL)
        {
            if (printStr(item->key, buffer, true))
            {
                printValue(item, buffer);
                //判断是否打印逗号
                if (item->next != NULL)
                {
                    //获取偏移过后的指针
                    needed = 1;
                    current_print = getCurrentPointer(buffer, needed);
                    if (current_print == NULL)
                    {
                        return false;
                    }
                    *current_print++ = ',';
                    buffer->offset += needed;
                }
            }

            item = item->next;
        }
        needed = 2;
        current_print = getCurrentPointer(buffer, needed);
        if (current_print == NULL)
        {
            return false;
        }
        *current_print++ = '}';
        *current_print++ = '\0';
        buffer->offset += needed;
        return true;
    }
    return false;
}

mcJSON_Bool printValue(mcJSON *obj, printbuffer *buffer)
{
    //判断不同数据类型
    switch (obj->type)
    {
    case mcJSON_String:
        return printStr(obj->stringValue, buffer, false);
        break;
    case mcJSON_Object:
        return printObject(obj, buffer);
        break;
    case mcJSON_Number:
        return printNumber(&(obj->intValue), buffer);
        break;
    default:
        break;
    }
    return false;
}

char *print(mcJSON *obj)
{
    printbuffer buffer;
    buffer.offset = 0;
    buffer.length = 512;
    buffer.format = 0;
    buffer.buffer = malloc(sizeof(mcJSON) * buffer.length);
    if (!printValue(obj, &buffer))
    {
        goto fail;
    }
    //重新分配内存，不占用太多空间
    char *printed = realloc(buffer.buffer, buffer.offset + 1);
    buffer.buffer = NULL;
    return printed;
fail:
    if (buffer.buffer != NULL)
    {
        free(buffer.buffer);
    }
    if (printed != NULL)
    {
        free(printed);
    }
}

int main()
{
    mcJSON *mcjson_test = createObjectItem();
    addStringToObject(mcjson_test, "name", "Lili");
    addNumberToObject(mcjson_test, "age", 11);
    addStringToObject(mcjson_test, "address", "beijing");
    addNumberToObject(mcjson_test, "high", 190);
    //   deleteItem(mcjson_test);
    //   print(mcjson_test);
    // mcJSON *obj = getObjectItem(mcjson_test,"name");
    char *printed = print(mcjson_test);
    printf("%s", printed);
    return 0;
}