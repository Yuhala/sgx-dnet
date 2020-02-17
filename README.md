## Summary
- `sgx-dnet` is a port of the machine learning library [Darknet](http://pjreddie.com/darknet) into Intel SGX. 
- This library can be used to securely train and test neural networks models following the Darknet API. 
- `sgx-dnet` source is separated into two main parts: `dnet-in` (Enclave/dnet-in) and `dnet-out` (App/dnet-out). `dnet-in`contains all the necessary API to do secure training and inference within the enclave runtime, without any I/O functionality or system calls, and the `dnet-out` acts as a support library for the `dnet-in` and complements any unsupported enclave functionality.
- Both parts communicate via ecalls and ocalls when/where necessary. 
- We redefine unsupported I/O calls like `fread, fwrite` etc, which act as wrapper functions for ocalls which invoke the corresponding `libc` routines in the untrusted runtime. These wrappers are mainly used for checkpointing and reading weights to/from disk respectively.
- For performance reasons, other I/O functionality like reading training data is performed completely outside; developers using `sgx-dnet` should design their code in such a way that (unsecure)input data is read into untrusted memory before proceeding into the enclave for training and inference. This style minimizes unecessary and expensive I/O during training or inference. For scenarios where training or inference data is to be kept private, the above mentioned wrapper functions could be exploited.
- OPenCV functionality and video processing are not supported as of now. Future versions may take those into account.
- Checkpointing weights to disk is TODO..
 

## Training a model
- To train a model, add a routine in `App.cpp` similar to the example cifar trainer: `train_cifar`. 
- Create a corresponding trainer routine in the trusted side which will be called via an ecall.
- Read and parse the model config file into a `list` data structure.
- Read the training data into the global `training_data` object/variable.
- Perform an ecall with the list and training data objects; the secure training routine is performed within the ecall.
- Modify `Trainer.edl` or `Enclave.edl` accordingly if you need to add more e/ocalls.
- See the example trainer with the cifar model for more inspiration on how to train and test other models.

## Testing the model
- It is assumed here that your trained model is still in enclave memory.
- To test the model after training, add a routine in `App.cpp` which takes the test data as input.
- Add a test routine in the enclave/trusted section which performs inference on a trained `network` object.
- Perform an ecall into the enclave runtime with the test data, and run your test routine within the enclave.
- To test the example cifar trainer, download and copy the cifar training data into `App/dnet-out/data/cifar`, modify the cifar config file in `App/dnet-out/cfg/cifar.cfg` accordingly, build and run the project.

## Doing inference
- By providing a weight file with the corresponding labels, you can perform inference on a pre-trained network model.
- The example tiny darknet application shows how to classify images using a pre-trained network.
- The trained model weights can be obtained via: `wget https://pjreddie.com/media/files/tiny.weights`. Copy these weights to the `Apps/dnet-out/backup` folder and modify the corresponding path to the weights file in [trainer.c](Enclave/dnet-in/train/trainer.c). Equally modify the paths to the corresponding config files and test image in [App.cpp](App/App.cpp).
- Run the `test_tiny` routine in the main function.
- Sample top5 prediction on the `giraffe` image: ![giraffe predictions](App/dnet-out/data/giraffe.jpg)

## Debug hints
- The `sgx-gdb` debug tool is recommended for debugging your enclave application.
- In case you have "strange" `seg faults`, your neural network may be too large to fit in the enclave heap.
- Try increasing the enclave heap size i.e the `HeapMaxSize` parameter in the enclave config file. It is `4GB` (0x100000000) by default in this project.

## Note
- All CUDA functionality is disabled/stripped off in `dnet-in` because GPUs do not have access to enclave memory.
- `dnet-in` contributes approximately `2MB` to the total enclave size after build.

For questions or issues regarding `sgx-dnet` please contact me: `petersonyuhala@gmail.com`. 

![Darknet Logo](http://pjreddie.com/media/files/darknet-black-small.png)

# Darknet #
Darknet is an open source neural network framework written in C and CUDA. It is fast, easy to install, and supports CPU and GPU computation.

For more information see the [Darknet project website](http://pjreddie.com/darknet).


