#ifndef MOBILENETCONFIG_H
#define MOBILENETCONFIG_H
#include <iostream>
#include <string>

struct mobileNetConfig
{
  float alpha =1.0;
  float dropout = 0.0001;
  int depth_multiplier = 1;
  bool include_top = true;
  std::string weights = "imagenet";
  int num_classes = 1000;

  //image size
  int imageSize = 224;
};




#endif // MOBILENETCONFIG_H
