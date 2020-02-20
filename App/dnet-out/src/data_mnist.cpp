/*
 * Created on Wed Feb 19 2020
 *
 * Copyright (c) 2020 Peterson Yuhala, IIUN
 */

#include "data_mnist.h"
#include "dnet_types.h"

//number of classes in mnist dataset
#define NUM_CLASSES 10

/**
 * change byte endianness
 * E.g: 0xAABBCCDD --> 0xDDCCBBAA
 */
uint32_t swap_bytes(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}

/**
 * Author: PYuhala
 * Mnist metadata:
 * There are 4 files after decompressing are: 
 * train-images-idx3-ubyte: training set images (45MB)
 * train-labels-idx1-ubyte: training set labels (60KB)
 * t10k-images-idx3-ubyte:  test set images (7.5MB)
 * t10k-labels-idx1-ubyte:  test set labels (12KB)
 * The training set contains 60000 examples, and the test set 10000 examples.
 */

/**
 * NB: mnist magic numbers:
 * image files: 0x00000803(2051)
 * label files: 0x00000801(2049)
 * ref: http://yann.lecun.com/exdb/mnist/
 */
data load_mnist_images(std::string path)
{

    // Read file
    std::ifstream file(path, std::ios::binary);

    uint32_t magic_num = 0;
    uint32_t num_images = 0;
    uint32_t rows = 0;
    uint32_t cols = 0;
    uint32_t image_size = 0;

    // Read the magic num (file signature) and dataset meta data
    /* if (!file.is_open)
        ERROR(); */
    file.read(reinterpret_cast<char *>(&magic_num), sizeof(magic_num));
    magic_num = swap_bytes(magic_num);

    if (magic_num != 2051)
        throw std::runtime_error("Invalid MNIST image file!");

    file.read((char *)&num_images, sizeof(num_images));
    num_images = swap_bytes(num_images);
    file.read((char *)&rows, sizeof(rows));
    rows = swap_bytes(rows);
    file.read((char *)&cols, sizeof(cols));
    cols = swap_bytes(cols);
    image_size = rows * cols;

    char *pixels = new char[image_size];
    //create data matrices
    data d = {0};
    d.shallow = 0;
    matrix X = make_matrix(num_images, image_size); //images
    //NB: d.y will be filled in the routine that calls this one via load_mnist_labels
    /**
     *        i/j_ _ _
     * X.vals[0]|_|_|_| --> 1 grayscale image
     * X.vals[1]|_|_|_|
     * X.vals[2]|_|_|_|
     */
    for (int i = 0; i < num_images; i++)
    {
        file.read(pixels, image_size);
        //copy byte by byte into X.vals
        for (int j = 0; j < X.cols; j++)
        {
            X.vals[i][j] = (float)pixels[j];
        }
        //memcpy(X.vals[i], pixels, image_size);
    }
    //make X the image data values of d
    d.X = X;
    print_matrix(X);
    file.close();
    return d;
}

matrix load_mnist_labels(std::string path)
{
    // Read file
    std::ifstream file(path, std::ios::binary);

    uint32_t magic_num = 0;
    uint32_t num_labels = 0;
    /* 
    if (!file.is_open)
        ERROR(); */
    // Read the magic num (file signature) and dataset meta data
    file.read((char *)&magic_num, sizeof(magic_num));
    magic_num = swap_bytes(magic_num);

    if (magic_num != 2049)
        throw std::runtime_error("Invalid MNIST label file!");

    file.read((char *)&num_labels, sizeof(num_labels));
    num_labels = swap_bytes(num_labels);

    matrix Y = make_matrix(num_labels, NUM_CLASSES); //labels
    //this matrix will be d.y for the mnist training/test data
    /**
     *        i/j_ _ _
     * Y.vals[0]|0|1|0| --> the corresponding label is the class corresponding to the 1
     * Y.vals[1]|1|0|0|
     * Y.vals[2]|0|1|0|
     */

    /**
     * PYuhala
     * 1 byte (sizeof char) is enough to store numbers from 0 to 9
     */
    char label_class;
    for (int i = 0; i < num_labels; i++)
    {
        //label is an int in [0,9]
        file.read(&label_class, 1);
        //label_class = (int)swap_bytes(label_class);
        Y.vals[i][(int)label_class] = 1;
        std::cout << "Label: " << (int)label_class << std::endl;
    }

    //print_matrix(Y);
    file.close();
    return Y;
}