# cv2PYNQ
这是一个在PYNQ平台上加速OpenCV图像处理算法的Python扩展包。这个库目前实现了某几个特定的图像处理算法的硬件加速，可以在16ms内处理完1080p的灰度图的滤波算法。

目前已经实现的算法列表：
- Sobel: 3x3; 5x5
- Scharr
- Laplacian: ksize = 1; 3; 5
- blur: ksize = 3
- GaussinBlur: ksize = 3
- erode: ksize = 3
- dilate: ksize = 3
- Canny 

## 安装
取决于Pynq软件版本的不同，安装方式各有不同。

首先需要打开PYNQ-Z1/Z2板卡上的Linux命令行界面，然后根据不同版本输入如下安装命令：
> >= PYNQ v2.3
```
sudo pip3 install -e .
```
> <= PYNQ v2.2
```
sudo pip3.6 install -e . 
```
运行完安装脚本之后就可以在Jupyter界面看到`cv2PYNQ`的文件夹

## 运行Sobel滤波
在`cv2PYNQ`文件夹中有一个Sobel滤波算法的notebook，跟着其中的步骤做就可以了。
