#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include "mssd_node/image_sensor.h"
#include "mssd_node/test.h"

#include <stdio.h>

#include <iostream>

using namespace cv;
using namespace cv::dnn;
using namespace std;

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


Mat frame;
ros::Publisher pub;
image_transport::Publisher pub2;
void chatterCallback(const mssd_node::image_sensor::ConstPtr &msg)
{
  sensor_msgs::ImagePtr rosimg(new sensor_msgs::Image(msg->data));
  frame = cv_bridge::toCvShare(rosimg, "bgr8")->image;

  cout<<msg->image_name<<endl;

  Size frame_size = frame.size();
  cout << "frame_size = " << frame_size << "frame_size.width = " << frame_size.width << "frame_size.height = " << frame_size.height <<endl;

  //frame=cv_bridge::toCvShare(msg, "bgr8")->image;
  sensor_msgs::ImagePtr msg_send = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();

  std::cout<<"********************************************"<<endl;

  String weights = "/home/micros/ImageTool/src/opencv-tf-mssd/data/frozen_inference_graph.pb";
  String prototxt = "/home/micros/ImageTool/src/opencv-tf-mssd/data/ssd_mobilenet_v1_coco.pbtxt";
  dnn::Net net = cv::dnn::readNetFromTensorflow(weights, prototxt);

  // xiAPIplusCameraOcv cam;
  float confidenceThreshold=0.5;
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

  cv::Mat blob = cv::dnn::blobFromImage(frame,1./255,Size(300,300));

  //Sets the input to the network
  net.setInput(blob);

  Mat output = net.forward();
  Mat detectionMat(output.size[2], output.size[3], CV_32F, output.ptr<float>());
  frame = frame(crop);
  vector<double> layersTimings;
  double freq = getTickFrequency() / 1000;
  double time = net.getPerfProfile(layersTimings) / freq;
  ostringstream ss;
  ss << "detection time: " << time << " ms";
  putText(frame, ss.str(), Point(20, 20), 0, 0.5, Scalar(0, 0, 255));

  //check confidence
  //vector<float> confidence_list;
  int check = 0;
  float confidence_sum=0;
  int i = 0;

  std::cout<<"detectionMat.rows:"<<detectionMat.rows<<std::endl;
  std::cout<<"detectionMat.cols:"<<detectionMat.cols<<std::endl;
  for (i = 0; i < detectionMat.rows; i++)
  {
    float confidence = detectionMat.at<float>(i, 2);
    if (confidence > confidenceThreshold) {
      size_t objectClass = (size_t)(detectionMat.at<float>(i, 1));
      int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
      int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
      int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
      int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);
      ostringstream ss;
      ss << confidence;
      // confidence_list.push_back(confidence);
      std::cout<<confidence<<std::endl;
      confidence_sum+=confidence;
      if(confidence<0.7)
      {
        check = 1;
      }
      String conf(ss.str());
      Rect object((int)xLeftBottom, (int)yLeftBottom, (int)(xRightTop - xLeftBottom), (int)(yRightTop - yLeftBottom));
      rectangle(frame, object, Scalar(0, 255, 0),2);
      String label = String(classNames[objectClass]) + ": " + conf;
      std::cout<<label<<std::endl;
      int baseLine = 0;
      Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
      rectangle(frame, Rect(Point(xLeftBottom, yLeftBottom - labelSize.height), Size(labelSize.width, labelSize.height + baseLine)), Scalar(0, 255, 0), CV_FILLED);
      putText(frame, label, Point(xLeftBottom, yLeftBottom), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
    }
  }

  //publish accuracy low image(average_low or someone_low?)
  mssd_node::image_sensor msgs;
  msgs.image_name = msg->image_name;
  std::cout<<"confidence_sum : "<<confidence_sum<<std::endl;

  //average<0.8 or someone<0.7
  if(check==1 /*|| ((confidence_sum/i)<0.8)*/)
  {
    //cv::Mat image = cv::imread(parser.get<String>("image"), CV_LOAD_IMAGE_COLOR);
    //cv::Mat image = cv_bridge::toCvShare(msg, "bgr8")->image;
    sensor_msgs::Image msg_data ;
    cv_bridge::CvImage(std_msgs::Header(), "bgr8", cv_bridge::toCvShare(msg_send, "bgr8")->image).toImageMsg(msg_data);
    msgs.data= msg_data;
    pub.publish(msgs);
  }
  else
  {
    //this image class aversge >0.8|| all>0.7 flag
    std::cout<<"######## detection OK ########"<<std::endl;

  }
  sensor_msgs::Image msg_view ;
  cv_bridge::CvImage(std_msgs::Header(), "bgr8", cv_bridge::toCvShare(rosimg, "bgr8")->image).toImageMsg(msg_view);
  pub2.publish(msg_view);


  string savePath ="/home/micros/catkin_ssd/detection/"+msg->image_name;
  //save
  imwrite(savePath,cv_bridge::toCvShare(rosimg, "bgr8")->image);
  //imshow("kWinName", frame);
  //waitKey(0);


  //imshow("kWinName", frame);
  //waitKey(0);
}



int main(int argc, char **argv)
{
  ros::init(argc, argv, "accuracy_low_image");
  ros::NodeHandle nh;
  pub = nh.advertise<mssd_node::image_sensor>("accuracy_low_image", 10);

  //pub2 = nh.advertise("accuracy_low_image_1", 10);

  image_transport::ImageTransport it(nh);
  pub2 = it.advertise("accuracy_result", 1);

  ros::Subscriber sub = nh.subscribe("/recogImage", 10, chatterCallback);
  ros::spin();

  return 0;


}


