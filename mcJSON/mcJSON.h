#define mcJSON_String 1
#define mcJSON_Number 2
#define mcJSON_Array 3
#define mcJSON_Object 4
#define true ((mcJSON_Bool)1)
#define false ((mcJSON_Bool)0)

typedef int mcJSON_Bool;
typedef struct mcJSON
{
    struct mcJSON *prev;
    struct mcJSON *next;
    struct mcJSON *child;
    int type;
    char *stringValue;
    int intValue;
    double doubleValue;
    char *key;
} mcJSON;

typedef struct printbuffer
{
    unsigned char* buffer;
    mcJSON_Bool format;
    //记录偏移量
    int offset;
    //记录最大长度
    int length;
} printbuffer;

mcJSON_Bool printValue(mcJSON *obj, printbuffer *buffer);
void deleteItem(mcJSON *obj);
mcJSON_Bool addObjToArray(mcJSON *obj,mcJSON *item,const char *const key);
mcJSON_Bool printNumber(int *number, printbuffer *buffer);