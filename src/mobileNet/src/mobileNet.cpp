#include "/home/micros/catkin_ssd/src/mobileNet/include/mobileNet/mobileNet.h"


mobileNet::mobileNet()
{
  //float alpha=1.0,int depthMultiplier=1,float dropout=0.001,bool include_top = true,std::string weights = "imagenet",int numClesses = 100

}
mobileNet::~mobileNet()
{

}
mobileNet* mobileNet::preprocessInput()
{
  return this;
}
mobileNet* mobileNet::convBlock(Scope scope, Input inputs, int filters,
                                float alpha, std::vector<int> kernel, std::vector<int> strides)
{
  auto x = tensorflow::ops::Pad(scope.WithOpName("conv1_pad"),inputs,{{1,1},{1,1}});

  filters = int(filters * alpha);
  Tensor filter = Tensor(DT_FLOAT,{3,3,3,filters});
  filter.flat<float>().setRandom();

  auto conv_output = ops::Conv2D(scope.WithOpName("conv1"),x,filter,strides,"SAME");
  this->BatchNorm(scope,conv_output);
  this->relu6(scope,BN_output[0],"conv1_relu");
  convBlock_output = relu_output;

  return this;
}
mobileNet* mobileNet::depthwiseConvBlock(Scope scope, Input inputs, int pointwise_conv_filters,
                                         float alpha, int depth_multiplier, std::vector<int> strides, int block_id)
{
  pointwise_conv_filters = int(pointwise_conv_filters*alpha);

  auto weight = Variable(scope,{3,3,3,depth_multiplier},DT_FLOAT);

  auto ZeroPadding2D = tensorflow::ops::Pad(scope,inputs,{{1,1},{1,1}});
  auto Depthwiseconv = tensorflow::ops::DepthwiseConv2dNative(scope,ZeroPadding2D,weight,{1,strides[0],strides[1],1},"SAME");
  this->BatchNorm(scope,Depthwiseconv);
  this->relu6(scope,BN_output[0],"relu_depth");

  Tensor filter = Tensor(DT_FLOAT,{3,3,3,pointwise_conv_filters});
  filter.flat<float>().setRandom();

  auto conv2D = tensorflow::ops::Conv2D(scope,relu_output,filter,strides,"SAME");
//  TF_CHECK_OK(session.Run({conv2D},nullptr))
  this->BatchNorm(scope,conv2D);
  this->relu6(scope,BN_output[0],"depthwiseConvBlock");

  return this;
}
mobileNet* mobileNet::relu6(Scope scope, Input inputs,std::string name )
{
  relu_output = tensorflow::ops::Relu6(scope.WithOpName(name),inputs);
  return this;
}
mobileNet* mobileNet::BatchNorm(Scope scope, Input inputs)
{
  Tensor scalev = Tensor(DT_FLOAT,{4});
  scalev.flat<float>().setValues({1,1,1,1});
  auto scale = tensorflow::ops::Variable(scope,{4},DT_FLOAT);
  auto assgin_scale = tensorflow::ops::Assign(scope,scale,scalev);
  Tensor offsetv = Tensor(DT_FLOAT,{4});
  offsetv.flat<float>().setValues({1,1,1,1});
  auto offset = tensorflow::ops::Variable(scope,{4},DT_FLOAT);
  auto assgin_offset = tensorflow::ops::Assign(scope,offset,offsetv);

  auto mean = tensorflow::ops::RandomUniform(scope,{4},DT_FLOAT);
  auto var = tensorflow::ops::RandomUniform(scope,{4},DT_FLOAT);

  TF_CHECK_OK(session.Run({assgin_scale,assgin_offset},nullptr));

  auto BN = tensorflow::ops::FusedBatchNorm(scope,inputs,scale,offset,mean,var);

  TF_CHECK_OK(session.Run({BN.y,BN.batch_mean,BN.batch_variance},&BN_output));

  return this;

}
mobileNet* mobileNet::network(Scope scope,float alpha =1.0,int depth_multiplier=1,
                              float dropout =0.001,bool include_top =true,std::string weights= "imagenet",
                              int classes = 1000,std::string pooling="none")
{
  //image data rows/cols channels?
  //image size [128 or 160 or 192 or 224]==default size
  Tensor imageData = Tensor(DT_FLOAT,{224,224,3});
  imageData.flat<float>().setRandom();

  this->convBlock(scope,imageData,32,alpha,{3,3},{2,2})
      ->depthwiseConvBlock(scope,convBlock_output,64,alpha,depth_multiplier,{1,1,1,1},1)
      ->depthwiseConvBlock(scope,relu_output,128,alpha,depth_multiplier,{1,2,2,1},2)
      ->depthwiseConvBlock(scope,relu_output,128,alpha,depth_multiplier,{1,1,1,1},3)
      ->depthwiseConvBlock(scope,relu_output,256,alpha,depth_multiplier,{1,2,2,1},4)
      ->depthwiseConvBlock(scope,relu_output,256,alpha,depth_multiplier,{1,1,1,1},5)
      ->depthwiseConvBlock(scope,relu_output,512,alpha,depth_multiplier,{1,2,2,1},6)
      ->depthwiseConvBlock(scope,relu_output,512,alpha,depth_multiplier,{1,1,1,1},7)
      ->depthwiseConvBlock(scope,relu_output,512,alpha,depth_multiplier,{1,1,1,1},8)
      ->depthwiseConvBlock(scope,relu_output,512,alpha,depth_multiplier,{1,1,1,1},9)
      ->depthwiseConvBlock(scope,relu_output,512,alpha,depth_multiplier,{1,1,1,1},10)
      ->depthwiseConvBlock(scope,relu_output,512,alpha,depth_multiplier,{1,1,1,1},11)

      ->depthwiseConvBlock(scope,relu_output,1024,alpha,depth_multiplier,{1,2,2,1},12)
      ->depthwiseConvBlock(scope,relu_output,1024,alpha,depth_multiplier,{1,1,1,1},13);
  if(include_top)
  {
    //shape
    int shape;
  }
  auto shape_tensor =Reshape(scope,tensorflow::ops::AvgPool(scope,relu_output,{1,224,224,1},{1,1,1,1},"SAME"),{1,1,int(1024*alpha)});
  //dorpout
  auto conv2D_temp = tensorflow::ops::Conv2D(scope,shape_tensor,classes,{1,1,1,1},"SAME");
  auto softMax = tensorflow::ops::Softmax(scope,conv2D_temp);
  auto reshape = tensorflow::ops::Reshape(scope,softMax,{classes,1});
  Tensor pool;
  if(pooling=="avg")
  {
    pool = tensorflow::ops::AvgPool(scope,reshape,{1,224,224,1},{1,1,1,1},"SAME");
  }
  else
  {
    pool = tensorflow::ops::MaxPool(scope,reshape,{1,224,224,1},{1,1,1,1},"SAME");
  }

  return this;
}

//int main(int argc, char *argv[])
//{
//  return 0;
//}
