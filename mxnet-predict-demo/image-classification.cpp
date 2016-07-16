
/*!
 *  Copyright (c) 2016
 * \file image_classification-predict.cpp
 * \brief C++ predict example of mxnet
 */

//
//  File: image-classification-predict.cpp
//  This is a simple predictor which shows
//  how to use c api for image classfication
//  It uses opencv for image reading
//  Created by wangdong on 2016.6
//  Thanks to : pertusa, caprice-j, sofiawu, tqchen, piiswrong,liuxiao
//
#include "image-classification.h"
#include <mxnet-cpp/MxNetCpp.h>


CNN_predict::CNN_predict()
{
    out=0;
    dev_type = 1;// 1: cpu, 2: gpu
    dev_id = 0;  // arbitrary.
    num_input_nodes = 1;  // 1 for feedforward

    // Image size and Model_Input_Iamge_Channels
    Model_Input_Iamge_Width = 80;
    Model_Input_Iamge_Height = 80;
    Model_Input_Iamge_Channels = 3;
    isUsingMeanInputImage=false;
}

CNN_predict::CNN_predict(int Input_Iamge_Width,int Input_Iamge_Height,int Input_Iamge_Channels,bool isUsingMean)
{
    out=0;
    dev_type = 1;// 1: cpu, 2: gpu
    dev_id = 0;  // arbitrary.
    num_input_nodes = 1;  // 1 for feedforward

    // Image size and Model_Input_Iamge_Channels
    Model_Input_Iamge_Width = Input_Iamge_Width;
    Model_Input_Iamge_Height = Input_Iamge_Height;
    Model_Input_Iamge_Channels = Input_Iamge_Channels;
    isUsingMeanInputImage=isUsingMean;
}

CNN_predict::~CNN_predict()
{
    delete json_data;
    delete param_data;
    delete mean_nd_buf;
    MXPredFree(out);
}


void CNN_predict::GetInputFile(cv::Mat inputIamge, mx_float* image_data,
                const int Model_Input_Iamge_Channels, const cv::Size resize_size,bool isUsingMeanImg) {
    // Read all kinds of file into a BGR color 3 channels image

    if (inputIamge.empty()) {
        return;
    }


    cv::Mat im;

    cv::resize(inputIamge, im, resize_size,0,0,cv::INTER_NEAREST);

    // Better to be read from a mean.nb file

    int size = im.rows * im.cols * Model_Input_Iamge_Channels;
    //特别注意这里的彩色图像的RGB三通道顺序，opencv是BGR，而训练读入都是 RGB
    mx_float* ptr_image_r = image_data;
    mx_float* ptr_image_g = image_data + size / 3;
    mx_float* ptr_image_b = image_data + size / 3 * 2;

    if(isUsingMeanImg)
    {
        NDListHandle nd_hnd;

        mx_uint nd_index = 0;
        mx_uint nd_len;
        const mx_uint* nd_shape = 0;
        const char* nd_key = 0;
        const mx_float* nd_data = 0;
        mx_uint nd_ndim = 0;

        MXNDListCreate((const char*)mean_nd_buf->GetBuffer(),
                       mean_nd_buf->GetLength(),
                       &nd_hnd, &nd_len);

        MXNDListGet(nd_hnd, nd_index, &nd_key, &nd_data, &nd_shape, &nd_ndim);

        const mx_float* ptr_mean_r = nd_data;
        const mx_float* ptr_mean_g = nd_data + size / 3;
        const mx_float* ptr_mean_b = nd_data + size / 3 * 2;
        cv::Mat mean_image_show(im.size(),CV_8UC3,cv::Scalar::all(0));
        for (int i = 0; i < im.rows; i++) {
            uchar* data = im.ptr<uchar>(i);
            uchar* data_mean = mean_image_show.ptr<uchar>(i);
            for (int j = 0; j < im.cols; j++) {
                *data_mean++ = static_cast<uchar>(*nd_data++);
                *data_mean++ = static_cast<uchar>(*nd_data++);
                *data_mean++ = static_cast<uchar>(*nd_data++);
                mx_float b = (static_cast<mx_float>(*data++) - *ptr_mean_b++)/256.;
                mx_float g = (static_cast<mx_float>(*data++)- *ptr_mean_g++)/256. ;
                mx_float r = (static_cast<mx_float>(*data++) - *ptr_mean_r++)/256.;

                *ptr_image_r++ = r;
                *ptr_image_g++ = g;
                *ptr_image_b++ = b;

            }
        }
        cv::namedWindow("mean_image_show",cv::WINDOW_NORMAL);

        imshow("mean_image_show",mean_image_show);
    }
    else
    {
        cv::Mat show(im.size(),CV_8UC3,cv::Scalar::all(0));
        for (int i = 0; i < im.rows; i++) {
            uchar* data = im.ptr<uchar>(i);
            uchar* data_show = show.ptr<uchar>(i);

            for (int j = 0; j < im.cols; j++) {
                if (Model_Input_Iamge_Channels > 1)
                {
                    mx_float b = static_cast<mx_float>(((*data++)-127.)/256.) /*- mean*/;
                    mx_float g = static_cast<mx_float>(((*data++)-127.)/256.)/*- mean*/;
                    *ptr_image_g++ = g;
                    *data_show++=b*256;
                    *data_show++=g*256;
                    *ptr_image_b++ = b;
                }

                mx_float r = static_cast<mx_float>(((*data++)-127.)/256.)/*- mean*/;
                *ptr_image_r++ = r;
                *data_show++=r*256;

            }
        }
        cv::namedWindow("show",cv::WINDOW_NORMAL);
        cv::imshow("show",show);
      }
}

// LoadSynsets
// Code from : https://github.com/pertusa/mxnet_predict_cc/blob/master/mxnet_predict.cc
std::vector<std::string> CNN_predict::LoadSynset(const char *filename) {
    std::ifstream fi(filename);

    if ( !fi.is_open() ) {
        std::cerr << "Error opening file " << filename << std::endl;
        assert(false);
    }

    std::vector<std::string> output;

    std::string synset, lemma;
    while ( fi >> synset ) {
        getline(fi, lemma);
        output.push_back(lemma);
    }

    fi.close();

    return output;
}

int CNN_predict::PrintOutputResult(const std::vector<float>& data) {
    if (data.size() != synset.size()) {
        std::cerr << "Result data and synset size does not match!" << std::endl;
    }

    float best_accuracy = 0.0;
    int best_idx = 0;

    for ( int i = 0; i < static_cast<int>(data.size()); i++ ) {
        qDebug ("Accuracy[%d] = %.8f\n", i, data[i]);

        if ( data[i] > best_accuracy ) {
            best_accuracy = data[i];
            best_idx = i;
        }
    }

    qDebug ("Best Result:  id = %d, accuracy = %.8f\n",
    best_idx, best_accuracy);
    return best_idx;
}



bool CNN_predict::loadModel(std::string json_file_path,std::string params_file_path,std::string mean_nd_file_path)
{
    // Models path for your model, you have to modify it
    json_data=new BufferFile (json_file_path);
    param_data=new BufferFile (params_file_path);
    mean_nd_buf = new BufferFile(mean_nd_file_path);
    if(false == json_data->isGetBufferFile || false == param_data->isGetBufferFile)
    {
        return false;
    }
    const char* input_key[1] = {"data"};
    const char** input_keys= input_key;

    const mx_uint input_shape_indptr[2] = { 0, 4 };
    // ( trained_width, trained_height, channel, num)
//    const mx_uint input_shape_data[4] = { 1,
//                                        static_cast<mx_uint>(Model_Input_Iamge_Channels),
//                                        static_cast<mx_uint>(Model_Input_Iamge_Width),
//                                        static_cast<mx_uint>(Model_Input_Iamge_Height) };
    const mx_uint input_shape_data[4] = { 1,
                                        static_cast<mx_uint>(Model_Input_Iamge_Channels),
                                        static_cast<mx_uint>(Model_Input_Iamge_Height),
                                          static_cast<mx_uint>(Model_Input_Iamge_Width) };
//    const mx_uint input_shape_data[4] = { static_cast<mx_uint>(Model_Input_Iamge_Width),
//                                          static_cast<mx_uint>(Model_Input_Iamge_Height),
//                                          static_cast<mx_uint>(Model_Input_Iamge_Channels),
//                                           1};
    //-- Create Predictor
    MXPredCreate((const char*)json_data->GetBuffer(),
                 (const char*)param_data->GetBuffer(),
                 static_cast<size_t>(param_data->GetLength()),
                 dev_type,
                 dev_id,
                 num_input_nodes,
                 input_keys,
                 input_shape_indptr,
                 input_shape_data,
                 &out);
  return true;
}

int CNN_predict::predictImageClassfication(cv::Mat inputIamge) {
    if (inputIamge.empty()) {
        return 0;
    }




//     PredictorHandle outcopy=malloc(sizeof(&out));
//     memcpy(outcopy,out,sizeof(&out));

    // Just a big enough memory 1000x1000x3
    unsigned int image_size = Model_Input_Iamge_Width * Model_Input_Iamge_Height * Model_Input_Iamge_Channels;
    std::vector<mx_float> image_data = std::vector<mx_float>(image_size);

    //-- Read Mean Data
    GetInputFile(inputIamge, image_data.data(), Model_Input_Iamge_Channels,
                cv::Size(Model_Input_Iamge_Width, Model_Input_Iamge_Height),isUsingMeanInputImage);

    //-- Set Input Image
    MXPredSetInput(out, "data", image_data.data(), image_size);

    //-- Do Predict Forward
    MXPredForward(out);

    mx_uint output_index = 0;

    mx_uint *shape = 0;
    mx_uint shape_len;

    //-- Get Output Result
    MXPredGetOutputShape(out, output_index, &shape, &shape_len);

    size_t size = 1;
    for (mx_uint i = 0; i < shape_len; ++i) size *= shape[i];

    std::vector<float> data(size);

    MXPredGetOutput(out, output_index, &(data[0]), size);

    // Release Predictor
    //MXPredFree(out);

    // Synset path for your model, you have to modify it


    //-- Print Output Data

    int result = PrintOutputResult(data);
    return result;
}

