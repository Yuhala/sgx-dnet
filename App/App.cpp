
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <thread>

#include <sgx_urts.h>
#include "App.h"
#include "Enclave_u.h"
#include "ErrorSupport.h"


/* For romulus */
#define MAX_PATH FILENAME_MAX

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;
static int stack_val = 10;


/* Darknet variables */
data training_data, test_data;
network *net_out;
#define CIFAR_CFG_FILE "/home/ubuntu/peterson/sgx-dnet-romulus/App/dnet-out/cfg/cifar.cfg"
//#define CIFAR_CFG_FILE "App/dnet-out/cfg/cifar.cfg"

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

/* Thread function --> only for testing purposes */
void thread_func()
{
    size_t tid = std::hash<std::thread::id>()(std::this_thread::get_id());
    printf("Thread ID: %d\n", tid);
    stack_val++; // implement mutex/atomic..just for testing anyway
    //ecall_nvram_worker(global_eid, stack_val, tid);
    //ecall_tester(global_eid,NULL,NULL,0);
}

/**
 * Train cifar network in the enclave:
 * We first create a network object in the untrusted side (net_out)
 * by parsing the config file (NB: we can't read/write files in the enclave)
 * This network is then passed to the enclave runtime via an ecall
 * and used as a template to create the secure network in the enclave
 */
void train_cifar(char *cfgfile, char *weightfile)
{
    char *base = basecfg(cfgfile);
    printf("%s\n", base);
    //I don't save weights on disk yet --> NULL weightfile
    //For nvram weights, tests are done within the enclave
    net_out = load_network(cfgfile, NULL, 0);
    list *sections = read_cfg(cfgfile);

    //Load training data
    training_data = load_all_cifar10();
    /**
     * The enclave will create a secure network struct in enclave memory
     * and do a deep copy of net_out...
     */
    ecall_trainer(global_eid, sections, &training_data, 0);
    printf("Training complete..\n");
    free_data(training_data);
}

void test_cifar(char *cfgfile, char *weightfile)
{
    //TODO
}

/* Initialize the enclave:
 *   Call sgx_create_enclave to initialize an enclave instance
 */
int initialize_enclave(void)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;

    /* Call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, NULL, NULL, &global_eid, NULL);
    if (ret != SGX_SUCCESS)
    {
        print_error_message(ret);
        return -1;
    }

    return 0;
}

/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    sgx_status_t ret;

    /* Initialize the enclave */
    if (initialize_enclave() < 0)
    {
        printf("Enter a character before exit ...\n");
        getchar();
        return -1;
    }

    //Create NUM_THRREADS threads
    //std::thread trd[NUM_THREADS];
    
    
    train_cifar(CIFAR_CFG_FILE, NULL);
    /*  
    for (int i = 0; i < NUM_THREADS; i++)
    {
        trd[i] = std::thread(thread_func);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        trd[i].join();
    } */

    //Destroy enclave
    sgx_destroy_enclave(global_eid);
    return 0;
}
