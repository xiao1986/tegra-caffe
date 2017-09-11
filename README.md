# tegra-caffe
@Building and Running
@{

This sample uses Caffe to classify the kinds of objects that appear in
the camera stream.

### Prerequisites ###
* You have followed Steps 1-3 in @ref mmapi_build.
* You have a working USB camera module.

### To build and run

1. Set up the Caffe environment.

   a) Install packages from APT with the following commands:

        $ sudo add-apt-repository universe
        $ sudo add-apt-repository multiverse
        $ sudo apt-get update
        $ sudo apt-get install libboost-all-dev libprotobuf-dev libleveldb-dev libsnappy-dev
        $ sudo apt-get install libhdf5-serial-dev protobuf-compiler libgflags-dev libgoogle-glog-dev
        $ sudo apt-get install liblmdb-dev libblas-dev libatlas-base-dev

    b) Download Caffe source package from the following website:

        https://github.com/BVLC/caffe

       And copy the package to `$HOME` directory on the target board
       with the following command:

           $ mkdir -pv $HOME/Work/caffe
           $ cp caffe-master.zip $HOME/Work/caffe/
           $ cd $HOME/Work/caffe/ && unzip caffe-master.zip

    c) Build Caffe source with the following commands:

        $ cd $HOME/Work/caffe/caffe-master
       Locate and edit `Makefile.config.example`.

       Uncomment the following line to enable cuDNN acceleration:

            USE_CUDNN := 1

        Add two lines to CUDA architecture setting:

            -gencode arch=compute_53,code=sm_53 \
            -gencode arch=compute_62,code=sm_62
       And modify the following two lines. Then save and close the file.

            INCLUDE_DIRS := $(PYTHON_INCLUDE) /usr/local/include /usr/include/hdf5/serial
            LIBRARY_DIRS := $(PYTHON_LIB) /usr/local/lib /usr/lib /usr/lib/aarch64-linux-gnu/hdf5/serial
       Then enter:

            $ cp Makefile.config.example Makefile.config
            $ make -j4

       The library libcaffe.so is generated in the `build/lib` directory.

2. Compile the OpenCV consumer library. This library is needed for Caffe and
   must be compiled on the target board.

       $ cd 11_camera_object_identification/opencv_consumer_lib

   Check the Makefile to ensure the following variables are set correctly:

        CUDA_DIR:=/usr/local/cuda
        CAFFE_DIR:=$HOME/Work/caffe/caffe-master
   Then enter:

        $ make

    This command generates the library `libopencv_consumer.so` in the current directory.

3. Download the Caffe model binaries with the following commands:

       $ sudo apt-get install python-pip
       $ sudo pip install pyyaml six
       $ cd $HOME/Work/caffe/caffe-master
       $ ./scripts/download_model_binary.py  models/bvlc_reference_caffenet/

    Get the `ImageNet` labels file with the following command:

        $ ./data/ilsvrc12/get_ilsvrc_aux.sh

4. Build and run the sample with the following commands:

       $ cd 11_camera_object_identification
       $ make
       $ export LD_LIBRARY_PATH=$HOME/Work/caffe/caffe-master/build/lib:/usr/local/cuda/lib64
       $ ./camera_v4l2_cuda -d /dev/video0 -s 640x480 -f YUYV -n 30
@}
![image](https://github.com/xiao1986/tegra-caffe/blob/master/tegra-caffe.gif)
