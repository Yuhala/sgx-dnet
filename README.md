## Summary
- `sgx-dnet` is a port of the machine learning library [Darknet](http://pjreddie.com/darknet) into Intel SGX. 
- This library can be used to securely train and test neural networks models following the Darknet API. 
- `sgx-dnet` source is separated into two main parts: `dnet-in` (Enclave/dnet-in) and `dnet-out` (App/dnet-out). The former contains the necessary API to do secure training and inference within the enclave runtime, without any I/O functionality or system calls, and the latter is the full Darknet library which acts as a support library for the former and complements any unsupported enclave functionality.
- Both parts communicate via ecalls and ocalls when/where necessary. 
- Checkpointing weights to disk is TODO..
 

## Training a model
- To train a model, add a routine in `App.cpp` similar to the example cifar trainer: `train_cifar`. 
- Create a corresponding trainer routine in the trusted side which will be called via an ecall.
- Read and parse the model config file into a `list` data structure.
- Read the training data into the global `training_data` object/variable.
- Perform an ecall with the list and training data objects; the secure training routine is performed within the ecall.
- See the example trainer with the cifar model for more inspiration on how to train and test other models.

## Testing the model
- It is assumed here that your trained model is still in enclave memory.
- To test the model after training, add a routine in `App.cpp` which takes the test data as input.
- Add a test routine in the enclave/trusted section which performs inference on a trained `network` object.
- Perform an ecall into the enclave runtime with the test data, and run your test routine within the enclave.

## Debug hints
- The `sgx-gdb` debug tool is recommended for debugging your enclave application.
- In case you have "strange" `seg faults`, your neural network may be too large to fit in the enclave heap.
- Try increasing the enclave size i.e the `HeapMaxSize` parameter in the enclave config file. It is `2GB` (0x80000000) by default.


![Darknet Logo](http://pjreddie.com/media/files/darknet-black-small.png)

# Darknet #
Darknet is an open source neural network framework written in C and CUDA. It is fast, easy to install, and supports CPU and GPU computation.

For more information see the [Darknet project website](http://pjreddie.com/darknet).

For questions or issues please use the [Google Group](https://groups.google.com/forum/#!forum/darknet).
