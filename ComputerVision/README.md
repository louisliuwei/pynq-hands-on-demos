# cv2PYNQ
This Python package accelerates [OpenCV](https://opencv.org/) image filtering functions for the [PYNQ](http://www.pynq.io/) platform.
The library implements a specific set of popular image filters and feature detection algorithms.
The calculation of time-consuming tasks is implemented in the Programmable Logic (PL) of the ZYNQ chip.
cv2PYNQ also includes the Video-Subsystem of the [base](https://github.com/Xilinx/PYNQ) project of PYNQ.
Therefore, the HDMI In and Out interfaces can be used in your application.

The library calculates every filter for gray-channel images with 1080p within 16 ms if the input and output buffers are located in the contiguous memory of the chip. 

Currently accelerated functions:
- Sobel: 3x3; 5x5
- Scharr
- Laplacian: ksize = 1; 3; 5
- blur: ksize = 3
- GaussinBlur: ksize = 3
- erode: ksize = 3
- dilate: ksize = 3
- Canny 

## Install
Install by typing: 
> on PYNQ v2.3
```
sudo pip3 install -e .
```
> on PYNQ v2.2 and earlier
```
sudo pip3.6 install -e . 
```
into the terminal on your Pynq-Z1/Pynq-Z2 board.   

## Run Sobel Demo
Open http://192.168.2.99:9090/notebooks/cv2PYNQ/Xupsh-UserGuide-Sobel.ipynb using your browser.

## More
If you want to contribute to cv2pynq, Read the instructions in https://github.com/wbrueckner/cv2PYNQ-The-project-behind-the-library