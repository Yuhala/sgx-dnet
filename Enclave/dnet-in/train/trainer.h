
#ifndef TRAINER_IN_H
#define TRAINER_IN_H


#if defined(__cplusplus)
extern "C"
{
#endif

void train_cifar(list *sections, data *training_data, int pmem);

#if defined(__cplusplus)
}
#endif



#endif