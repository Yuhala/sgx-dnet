/*
 * Created on Fri Feb 14 2020
 *
 * Copyright (c) 2020 Peterson Yuhala, IIUN
 */

#include "dnet_ocalls.h"

//File pointer used for reading/writing checkpoint-weight files
FILE *fp = NULL;
char *weights_file = "/path/to/weightsfile";

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

// 0 for read: 1 for write
void ocall_open_file(const char *filename, flag oflag)
{
    if (!fp) //fp == NULL
    {
        switch (oflag)
        {
        case O_RDONLY:
            fp = fopen(filename, "rb");
            printf("opened file in read only mode\n");
            break;
        case O_WRONLY:
            fp = fopen(filename, "wb");
            printf("opened file in write only mode\n");
            break;
        case O_RDPLUS:
            fp = fopen(filename, "r+");
            break;
        case O_WRPLUS:
            fp = fopen(filename, "w+");
            break;
        default:
            //nothing to do
        }
    }
    else
    {
        printf("problem with file pointer..\n");
    }
}

/**
 * Only one file can be manipulated at any point in time
 * from within enclave. So we have no ambiguity at the level
 * of which file pointer to close..
 */
void ocall_close_file()
{
    if (fp) //fp != NULL
    {
        fclose(fp);
        fp = NULL;
    }
}

void ocall_fread(void *ptr, size_t size, size_t nmemb)
{
    if (fp)
    {
        fread(ptr, size, nmemb, fp);
    }
    else
    {
        printf("Corrupt file pointer..\n");
        abort();
    }
}

void ocall_fwrite(void *ptr, size_t size, size_t nmemb)
{
    if (fp)
    {
        fwrite(ptr, size, nmemb, fp);
    }
    else
    {
        printf("Corrupt file pointer..\n");
        abort();
    }
}