/*
 * Created on Wed Feb 15 2020
 *
 * Copyright (c) 2020 Peterson Yuhala, IIUN
 */


#ifndef _APP_H_
#define _APP_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "sgx_error.h" /* sgx_status_t */
#include "sgx_eid.h"   /* sgx_enclave_id_t */

#include "dnet-out/src/dnet_ocalls.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define TOKEN_FILENAME "enclave.token"
#define ENCLAVE_FILENAME "enclave.signed.so"
#define NUM_THREADS 3 //number of worker threads

//extern sgx_enclave_id_t global_eid;    /* global enclave id */

#if defined(__cplusplus)

extern "C"
{
#endif

    void train_cifar(char *cfgfile);
    void test_cifar(char *cfgfile);
    void train_mnist(char *cfgfile);
    void test_mnist(char *cfgfile);
    void test_tiny(char *cfgfile);

#if defined(__cplusplus)
}
#endif

#endif /* !_APP_H_ */
