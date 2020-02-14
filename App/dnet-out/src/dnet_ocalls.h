#ifndef DNET_OCALLS_H
#define DNET_OCALLS_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Enclave_u.h"
#include "darknet.h"

#if defined(__cplusplus)

extern "C"
{
#endif

    void ocall_free_sec(section *sec);
    void ocall_free_list(list *list);
    void ocall_print_string(const char *str);

#if defined(__cplusplus)
}
#endif

#endif /* DNET_OCALLS_H */
