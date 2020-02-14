/*
 * Created on Fri Feb 14 2020
 *
 * Copyright (c) 2020 Peterson Yuhala, IIUN
 */

#include "dnet_ocalls.h"


//File pointers used for reading/writing checkpoint-weight files
FILE *read_fp = NULL;
FILE *write_fp = NULL;

void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate
     * the input string to prevent buffer overflow.
     */
    printf("%s", str);
}


/* Free section in untrusted memory*/
void ocall_free_sec(section *sec)
{
    //printf("Freeing section in ocall..\n");
    free_section(sec);
}

void ocall_free_list(list *list)
{
    free_list(list);
}

