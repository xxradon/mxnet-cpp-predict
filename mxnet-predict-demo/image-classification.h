#ifndef IMAGE_CLASS_H
#define IMAGE_CLASS_H
#include <stdio.h>

// Path for c_predict_api

#include <mxnet/c_predict_api.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Read file to buffer
class BufferFile {
 public :
    std::string file_path_;
    int length_;
    char* buffer_;
    bool isGetBufferFile;

    explicit BufferFile(std::string file_path)
    :file_path_(file_path) {

        std::ifstream ifs(file_path.c_str(), std::ios::in | std::ios::binary);
        if (!ifs) {
            isGetBufferFile=false;
        }
        else
        {
            isGetBufferFile=true;
        }


        ifs.seekg(0, std::ios::end);
        length_ = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::cout << file_path.c_str() << " ... "<< length_ << " bytes\n";

        buffer_ = new char[sizeof(char) * length_];
        ifs.read(buffer_, length_);
        ifs.close();
    }

    int GetLength() {
        return length_;
    }
    char* GetBuffer() {
        return buffer_;
    }

    ~BufferFile() {
        delete[] buffer_;
        buffer_ = NULL;
    }
};

class CNN_predict
{
public:
    CNN_predict();
    CNN_predict(int Input_Iamge_Width,int Input_Iamge_Height,int Input_Iamge_Channels,bool isUsingMean);
    ~CNN_predict();
    //json_file_path_ 模型框架结构文件路径
    //params_file_path 权重参数文件路径
    //mean_nd_file_path 均值文件路径
    bool loadModel(std::string json_file_path,std::string params_file_path,std::string mean_nd_file_path);
    int predictImageClassfication(cv::Mat inputIamge);
private:
    void GetInputFile(cv::Mat inputIamge, mx_float* image_data,
                    const int channels, const cv::Size resize_size,bool isUsingMeanImg);
    std::vector<std::string> LoadSynset(const char *filename);
    int PrintOutputResult(const std::vector<float>& data);
private:
     PredictorHandle out;  // alias for void *
     BufferFile *json_data;
     BufferFile *param_data;
     //-- Read Mean Data
     BufferFile *mean_nd_buf;

     NDListHandle nd_hnd;


public:
     // Parameters
     int dev_type ;  // 1: cpu, 2: gpu
     int dev_id ;  // arbitrary.
     mx_uint num_input_nodes;  // 1 for feedforward

     // Image size and channels
     int Model_Input_Iamge_Width;
     int Model_Input_Iamge_Height;
     int Model_Input_Iamge_Channels;
     bool isUsingMeanInputImage;
     std::vector<std::string> synset;

};



#endif // IMAGE_CLASS_H
