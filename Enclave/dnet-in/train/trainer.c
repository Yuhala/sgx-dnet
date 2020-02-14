
//#include "Enclave.h"
//#include "Enclave_t.h"
#include "dnet_sgx_utils.h"
#include "darknet.h"
#include "trainer.h"

void ecall_trainer(list *sections, data *training_data, int pmem)
{
    
    ocall_open_file("file.txt");
    printf("Created and opened file.txt\n");
    char c[] = "enclave file i/o test";
    fwrite(c,strlen(c)+1,1,0);
    char buffer[100];
    ocall_close_file();//dont have fseek ocall so I close and reopen for now :-)
    ocall_open_file("file.txt");

     /* Read and display data */
   fread(buffer, strlen(c)+1, 1, 0);
   printf("%s\n", buffer);
    ocall_close_file();

    return;

    train_cifar(sections, training_data, pmem);
}

/**
 * Training algorithms for different models
 */

void train_cifar(list *sections, data *training_data, int pmem)
{
    //TODO: pointer checks
    printf("In ecall_trainer..\n"); //minimize the number of prints when doing benchmarking as they perform expensive enclave transitions..

    network *net = create_net_in(sections);
    printf("done creating network in enclave...\n");

    srand(12345);
    float avg_loss = -1;
    printf("Learning Rate: %g, Momentum: %g, Decay: %g\n", net->learning_rate, net->momentum, net->decay);
    char **labels = {"airplane", "automobile", "bird", "cat", "deer", "dog", "frog", "horse", "ship", "truck"};
    int classes = 10;
    int N = 50000;
    int epoch = (*net->seen) / N;
    data train = *training_data;
    printf("Max batches: %d\n", net->max_batches);

    while (get_current_batch(net) < net->max_batches || net->max_batches == 0)
    {

        float loss = train_network_sgd(net, train, 1);
        if (avg_loss == -1)
            avg_loss = loss;
        avg_loss = avg_loss * .95 + loss * .05;
        printf("Batch num: %ld, Seen: %.3f: Loss: %f, Avg loss: %f avg, L. rate: %f rate,Images seen: %ld \n",
               get_current_batch(net), (float)(*net->seen) / N, loss, avg_loss, get_current_rate(net), *net->seen);
        if (*net->seen / N > epoch)
        {
            //epoch = *net->seen / N;
            //printf("Epoch: %d\n", epoch);
            //char buff[256];
            //sprintf(buff, "%s/%s_%d.weights",backup_directory,base, epoch);
            //save_weights(net, buff);
        }
        if (get_current_batch(net) % 100 == 0)
        {
            //run net on test data and note accuracy

            // char buff[256];
            // sprintf(buff, "%s/%s.backup",backup_directory,base);
            // save_weights(net, buff);
            printf("modulo 100 is 0..save weights here..\n");
        }
    }

    //char buff[256];
    //sprintf(buff, "%s/%s.weights", backup_directory, base);
    //save_weights(net, buff);

    //free_network(net);
    //TODO
    //print training terminated

    //free_data(train);
}
void ecall_tester(network *net, data *test_data, int pmem)
{
    //do pointer checks
    printf("ecall_tester..\n");

    /*  network *net = load_network(filename, weightfile, 0);
    //srand(time(0));

    //clock_t time;
    float avg_acc = 0;
    float avg_top5 = 0;
    data test = load_cifar10_data("data/cifar/test_batch.bin");

    //time=clock();

    float *acc = network_accuracies(net, test, 2);
    avg_acc += acc[0];
    avg_top5 += acc[1];
    printf("top1: %f, xx seconds, %d images\n", avg_acc, test.X.rows);
    free_data(test); */
}

/**
 * Author: Peterson Yuhala
 * Knowledge distillation involves training a smaller network with 
 * a larger network.
 * So we pass the weights as param
 * I do not need this in the enclave for my proof of concept ml application
 * Nevertheless this functionality could be easily ported into the enclave
 */
/* void train_cifar_distill(char *cfgfile, char *weightfile)
{
    srand(time(0));
    float avg_loss = -1;
    char *base = basecfg(cfgfile);
    printf("%s\n", base);
    network *net = load_network(cfgfile, weightfile, 0);
    printf("Learning Rate: %g, Momentum: %g, Decay: %g\n", net->learning_rate, net->momentum, net->decay);

    char *backup_directory = "/home/ubuntu/peterson/sgx-dnet/backup/";
    int classes = 10;
    int N = 50000;

    char **labels = {"airplane","automobile","bird","cat","deer","dog","frog","horse","ship","truck"};
    int epoch = (*net->seen)/N;

    data train = load_all_cifar10();
    matrix soft = csv_to_matrix("results/ensemble.csv");

    float weight = .9;
    scale_matrix(soft, weight);
    scale_matrix(train.y, 1. - weight);
    matrix_add_matrix(soft, train.y);

    while(get_current_batch(net) < net->max_batches || net->max_batches == 0){
        clock_t time=clock();

        float loss = train_network_sgd(net, train, 1);
        if(avg_loss == -1) avg_loss = loss;
        avg_loss = avg_loss*.95 + loss*.05;
        printf("%ld, %.3f: %f, %f avg, %f rate, %lf seconds, %ld images\n", get_current_batch(net), (float)(*net->seen)/N, loss, avg_loss, get_current_rate(net), sec(clock()-time), *net->seen);
        if(*net->seen/N > epoch){
            epoch = *net->seen/N;
            char buff[256];
            sprintf(buff, "%s/%s_%d.weights",backup_directory,base, epoch);
            save_weights(net, buff);
        }
        if(get_current_batch(net)%100 == 0){
            char buff[256];
            sprintf(buff, "%s/%s.backup",backup_directory,base);
            save_weights(net, buff);
        }
    }
    char buff[256];
    sprintf(buff, "%s/%s.weights", backup_directory, base);
    save_weights(net, buff);

    free_network(net);
    free_ptrs((void**)labels, classes);
    free(base);
    free_data(train);
} */

/* void test_cifar_multi(char *filename, char *weightfile)
{
    network *net = load_network(filename, weightfile, 0);
    set_batch_network(net, 1);
    srand(time(0));

    float avg_acc = 0;
    data test = load_cifar10_data("data/cifar/cifar/test_batch.bin");

    int i;
    for(i = 0; i < test.X.rows; ++i){
        image im = float_to_image(32, 32, 3, test.X.vals[i]);

        float pred[10] = {0};

        float *p = network_predict(net, im.data);
        axpy_cpu(10, 1, p, 1, pred, 1);
        flip_image(im);
        p = network_predict(net, im.data);
        axpy_cpu(10, 1, p, 1, pred, 1);

        int index = max_index(pred, 10);
        int class = max_index(test.y.vals[i], 10);
        if(index == class) avg_acc += 1;
        free_image(im);
        printf("%4d: %.2f%%\n", i, 100.*avg_acc/(i+1));
    }
} */

/* void extract_cifar()
{
char *labels[] = {"airplane","automobile","bird","cat","deer","dog","frog","horse","ship","truck"};
    int i;
    data train = load_all_cifar10();
    data test = load_cifar10_data("data/cifar/cifar-10-batches-bin/test_batch.bin");
    for(i = 0; i < train.X.rows; ++i){
        image im = float_to_image(32, 32, 3, train.X.vals[i]);
        int class = max_index(train.y.vals[i], 10);
        char buff[256];
        sprintf(buff, "data/cifar/train/%d_%s",i,labels[class]);
        save_image_options(im, buff, PNG, 0);
    }
    for(i = 0; i < test.X.rows; ++i){
        image im = float_to_image(32, 32, 3, test.X.vals[i]);
        int class = max_index(test.y.vals[i], 10);
        char buff[256];
        sprintf(buff, "data/cifar/test/%d_%s",i,labels[class]);
        save_image_options(im, buff, PNG, 0);
    }
} */

/* void test_cifar_csv(char *filename, char *weightfile)
{
    network *net = load_network(filename, weightfile, 0);
    srand(time(0));

    data test = load_cifar10_data("data/cifar/cifar-10-batches-bin/test_batch.bin");

    matrix pred = network_predict_data(net, test);

    int i;
    for(i = 0; i < test.X.rows; ++i){
        image im = float_to_image(32, 32, 3, test.X.vals[i]);
        flip_image(im);
    }
    matrix pred2 = network_predict_data(net, test);
    scale_matrix(pred, .5);
    scale_matrix(pred2, .5);
    matrix_add_matrix(pred2, pred);

    matrix_to_csv(pred);
    fprintf(stderr, "Accuracy: %f\n", matrix_topk_accuracy(test.y, pred, 1));
    free_data(test);
} */

/* void test_cifar_csvtrain(char *cfg, char *weights)
{
    network *net = load_network(cfg, weights, 0);
    srand(time(0));

    data test = load_all_cifar10();

    matrix pred = network_predict_data(net, test);

    int i;
    for(i = 0; i < test.X.rows; ++i){
        image im = float_to_image(32, 32, 3, test.X.vals[i]);
        flip_image(im);
    }
    matrix pred2 = network_predict_data(net, test);
    scale_matrix(pred, .5);
    scale_matrix(pred2, .5);
    matrix_add_matrix(pred2, pred);

    matrix_to_csv(pred);
    fprintf(stderr, "Accuracy: %f\n", matrix_topk_accuracy(test.y, pred, 1));
    free_data(test);
}

void eval_cifar_csv()
{
    data test = load_cifar10_data("data/cifar/cifar-10-batches-bin/test_batch.bin");

    matrix pred = csv_to_matrix("results/combined.csv");
    fprintf(stderr, "%d %d\n", pred.rows, pred.cols);

    fprintf(stderr, "Accuracy: %f\n", matrix_topk_accuracy(test.y, pred, 1));
    free_data(test);
    free_matrix(pred);
} */

/* void run_cifar(int argc, char **argv)
{
    if(argc < 4){
        fprintf(stderr, "usage: %s %s [train/test/valid] [cfg] [weights (optional)]\n", argv[0], argv[1]);
        return;
    }

    char *cfg = argv[3];
    //if we have 5 args initialize weights else 0
    char *weights = (argc > 4) ? argv[4] : 0;
    if(0==strcmp(argv[2], "train")) train_cifar(cfg, weights);
    else if(0==strcmp(argv[2], "extract")) extract_cifar();
    else if(0==strcmp(argv[2], "distill")) train_cifar_distill(cfg, weights);
    else if(0==strcmp(argv[2], "test")) test_cifar(cfg, weights);
    else if(0==strcmp(argv[2], "multi")) test_cifar_multi(cfg, weights);
    else if(0==strcmp(argv[2], "csv")) test_cifar_csv(cfg, weights);
    else if(0==strcmp(argv[2], "csvtrain")) test_cifar_csvtrain(cfg, weights);
    else if(0==strcmp(argv[2], "eval")) eval_cifar_csv();
} */
