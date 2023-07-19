#ifndef __CJSON_TEST_H__
#define __CJSON_TEST_H__

#define exit_FAILURE 1

#include "cJSON.h"

/* Used by some code below as an example datatype. */
struct record
{
    const char *precision;
    double lat;
    double lon;
    const char *address;
    const char *city;
    const char *state;
    const char *zip;
    const char *country;
};

//void create_objects(void);
int print_preallocated(cJSON *root);
void exit2(int status);

#endif