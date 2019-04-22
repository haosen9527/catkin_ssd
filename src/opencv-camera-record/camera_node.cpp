#include <ros/ros.h>
#include <stdio.h>
#include "include/xiApiPlusOcv.hpp"
#include <opencv2/opencv.hpp>

#include <opencv2/dnn.hpp>

using namespace cv;
using namespace std;
using namespace cv::dnn;



const char* classNames[] = {"background","husky-robot"};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "camera_node");
  ros::NodeHandle nh;
  ROS_INFO("Hello world!");
  //  String weights = "/home/nuc-m210/env/husky/model/frozen_inference_graph.pb";
  //  String prototxt = "/home/nuc-m210/env/husky/ssd_mobilenet_v1_coco.pbtxt";
  //  dnn::Net net = cv::dnn::readNetFromTensorflow(weights, prototxt);
  try
  {
    // Sample for XIMEA OpenCV
    xiAPIplusCameraOcv cam;

    // Retrieving a handle to the camera device
    printf("Opening first camera...\n");
    cam.OpenFirst();

    // Set exposure
    cam.SetExposureTime(10000); //10000 us = 10 ms
    cam.SetImageDataFormat(XI_RGB24);
//    XI_MONO8                     =0, // 8 bits per pixel
//    XI_MONO16                    =1, // 16 bits per pixel
//    XI_RGB24                     =2, // RGB data format
//    XI_RGB32                     =3, // RGBA data format
//    XI_RGB_PLANAR                =4, // RGB planar data format
//    XI_RAW8                      =5, // 8 bits per pixel raw data from sensor
//    XI_RAW16                     =6, // 16 bits per pixel raw data from sensor
//    XI_FRM_TRANSPORT_DATA        =7, // Data from transport layer (e.g. packed). Format see XI_PRM_TRANSPORT_PIXEL_FORMAT
//    XI_RGB48                     =8, // RGB data format
//    XI_RGB64                     =9, // RGBA data format
//    XI_RGB16_PLANAR              =10, // RGB16 planar data format
//    XI_RAW8X2                    =11, // 8 bits per pixel raw data from sensor(2 components in a row)
//    XI_RAW8X4                    =12, // 8 bits per pixel raw data from sensor(4 components in a row)
//    XI_RAW16X2                   =13, // 16 bits per pixel raw data from sensor(2 components in a row)
//    XI_RAW16X4                   =14, // 16 bits per pixel raw data from sensor(4 components in a row)
//    XI_RAW32                     =15, // 32 bits per pixel raw data from sensor in integer format (LSB first)
//    XI_RAW32FLOAT
    // Note: The default parameters of each camera might be different in different API versions

    printf("Starting acquisition...\n");
    cam.StartAcquisition();

    printf("First pixel value \n");
    XI_IMG_FORMAT format = cam.GetImageDataFormat();


    VideoWriter write;
//    VideoCapture captrue(0);
    string outFlie = "/home/hh/boxes/openCV-tf/output.avi";
    double r = cam.GetFrameRate();
    int w = cam.GetWidth();
    int h = cam.GetHeight();
    cout << "w=" << w <<endl;
    cout << "h=" << h <<endl;
    Size S(w, h);
//    //获得帧率
//    double r = captrue.get(CV_CAP_PROP_FPS);
    //打开视频文件，准备写入
    write.open(outFlie, VideoWriter::fourcc('M','J','P','G'), 30, S);

//    //打开失败
//    if (!captrue.isOpened())
//    {
//      return 1;
//    }

    //#define EXPECTED_IMAGES 40
    while(waitKey(30) != 27)
    {
      Mat cv_mat_image = cam.GetNextImageOcvMat();
      if (format == XI_RAW16 || format == XI_MONO16)
        normalize(cv_mat_image, cv_mat_image, 0, 65536, NORM_MINMAX, -1, Mat()); // 0 - 65536, 16 bit unsigned integer range

      cv::imshow("Image from camera",cv_mat_image);
      write.write(cv_mat_image);
      waitKey(10);
      //      printf("\t%d\n",cv_mat_image.at<unsigned char>(0,0));
    }

    write.release();
    cam.StopAcquisition();
    cam.Close();
    printf("Done\n");

    cvWaitKey(500);
  }
  catch(xiAPIplus_Exception& exp)
  {
    printf("Error:\n");
    exp.PrintError();
#ifdef WIN32
    Sleep(2000);
#endif
    cvWaitKey(2000);
    return -1;
  }
  return 0;
}
//  try
//  {
////yaml_url: "package://mcptam/calibrations/camera1.yaml"
////exposure_time: 30000
////rect_left: 200
////rect_top: 200
////rect_height: 600
////rect_width: 900
//    xiAPIplusCameraOcv cam;
//    cam.OpenFirst();
//    cam.SetExposureTime(30000); //10000 us = 10 ms
//    cam.SetImageDataFormat(XI_RGB24);
//    cam.StartAcquisition();
////    cam.next_image_->GetBytesPerPixel()
//    // Read and convert a frame from the camera
//    Mat cv_mat_image = cam.GetNextImageOcvMat();
//    // Show image on display
//    cv::imshow("Image from camera",cv_mat_image);

//    cam.StopAcquisition();
//    cam.Close();
//  }
//  catch(xiAPIplus_Exception& exp)
//  {
//    exp.PrintError(); // report error if some call fails
//  }
//}
