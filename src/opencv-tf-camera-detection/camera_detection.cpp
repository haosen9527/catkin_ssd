#include <ros/ros.h>
#include <stdio.h>
#include "include/xiApiPlusOcv.hpp"
#include <opencv2/opencv.hpp>

#include <opencv2/dnn.hpp>

using namespace cv;
using namespace std;
using namespace cv::dnn;

const size_t inWidth = 300;
const size_t inHeight = 300;
const float WHRatio = inWidth / (float)inHeight;
const char* classNames[] = { "background",
                             "person",
                             "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat",
                             "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis",
                             "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl",
                             "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch", "potted plant", "bed", "dining table",
                             "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase",
                             "scissors", "teddy bear", "hair drier", "toothbrush"};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "camera_node");
  ros::NodeHandle nh;
  ROS_INFO("Hello world!");
  String weights = "/home/hh/object——detection/tensorflow/models-master/research/object_detection/ssd_model/2016coco/frozen_inference_graph.pb";
  String prototxt = "/home/hh/object——detection/tensorflow/models-master/research/object_detection/ssd_model/ssd_mobilenet_v1_coco.pbtxt";
  dnn::Net net = cv::dnn::readNetFromTensorflow(weights, prototxt);
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
    // Note: The default parameters of each camera might be different in different API versions

    printf("Starting acquisition...\n");
    cam.StartAcquisition();

    printf("First pixel value \n");
    XI_IMG_FORMAT format = cam.GetImageDataFormat();
//#define EXPECTED_IMAGES 40
    while(waitKey(30) != 27)
    {
      Mat cv_mat_image = cam.GetNextImageOcvMat();
      if (format == XI_RAW16 || format == XI_MONO16)
        normalize(cv_mat_image, cv_mat_image, 0, 65536, NORM_MINMAX, -1, Mat()); // 0 - 65536, 16 bit unsigned integer range
      Size frame_size = cv_mat_image.size();
      //  cout << "frame_size = " << frame_size << "frame_size.width = " << frame_size.width << "frame_size.height = " << frame_size.height <<endl;
      Size cropSize;
      if (frame_size.width / (float)frame_size.height > WHRatio)
      {
        cropSize = Size(static_cast<int>(frame_size.height * WHRatio),
                        frame_size.height);
      }else
      {
        cropSize = Size(frame_size.width,
                        static_cast<int>(frame_size.width / WHRatio));
      }
      Rect crop(Point((frame_size.width - cropSize.width) / 2,
                      (frame_size.height - cropSize.height) / 2),
                cropSize);

      cv::Mat blob = cv::dnn::blobFromImage(cv_mat_image,1./255,Size(300,300));
      //cout << "blob size: " << blob.size << endl;

      net.setInput(blob);

      Mat output = net.forward();
//      cout << "output size: " << output.size << endl;

      Mat detectionMat(output.size[2], output.size[3], CV_32F, output.ptr<float>());

//      cv_mat_image = cv_mat_image(crop);
      float confidenceThreshold = 0.50;
      vector<double> layersTimings;
      double freq = getTickFrequency() / 1000;
      double time = net.getPerfProfile(layersTimings) / freq;
      ostringstream ss;
      ss << "detection time: " << time << " ms";
      putText(cv_mat_image, ss.str(), Point(20, 20), 0, 0.5, Scalar(0, 0, 255));

      for (int i = 0; i < detectionMat.rows; i++)
      {
        float confidence = detectionMat.at<float>(i, 2);
        if (confidence > confidenceThreshold) {
          size_t objectClass = (size_t)(detectionMat.at<float>(i, 1));
          int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * cv_mat_image.cols);
          int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * cv_mat_image.rows);
          int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * cv_mat_image.cols);
          int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * cv_mat_image.rows);
          ostringstream ss;
          ss << confidence;
          String conf(ss.str());
          Rect object((int)xLeftBottom, (int)yLeftBottom, (int)(xRightTop - xLeftBottom), (int)(yRightTop - yLeftBottom));
          rectangle(cv_mat_image, object, Scalar(0, 255, 0),2); String label = String(classNames[objectClass]) + ": " + conf;
          int baseLine = 0;
          Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
          rectangle(cv_mat_image, Rect(Point(xLeftBottom, yLeftBottom - labelSize.height), Size(labelSize.width, labelSize.height + baseLine)), Scalar(0, 255, 0), CV_FILLED);
          putText(cv_mat_image, label, Point(xLeftBottom, yLeftBottom), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
        }
      }
      cv::imshow("Image from camera",cv_mat_image);
      waitKey(1);
//      printf("\t%d\n",cv_mat_image.at<unsigned char>(0,0));
    }

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
