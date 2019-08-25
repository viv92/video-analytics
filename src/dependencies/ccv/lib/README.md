![Darknet Logo](http://pjreddie.com/media/files/darknet-black-small.png)

#Darknet#
Darknet is an open source neural network framework written in C and CUDA. It is fast, easy to install, and supports CPU and GPU computation.

For more information see the [Darknet project website](http://pjreddie.com/darknet).

For questions or issues please use the [Google Group](https://groups.google.com/forum/#!forum/darknet).

Steps to build
1) Make
2) Run the following commands:
ar rcs libdarknet.a obj/gemm.o obj/utils.o obj/cuda.o obj/convolutional_layer.o obj/list.o obj/image.o obj/activations.o obj/im2col.o obj/col2im.o obj/blas.o obj/crop_layer.o obj/dropout_layer.o obj/maxpool_layer.o obj/softmax_layer.o obj/data.o obj/matrix.o obj/network.o obj/connected_layer.o obj/cost_layer.o obj/parser.o obj/option_list.o obj/detection_layer.o obj/captcha.o obj/route_layer.o obj/writing.o obj/box.o obj/nightmare.o obj/normalization_layer.o obj/avgpool_layer.o obj/coco.o obj/dice.o obj/yolo.o obj/detector.o obj/layer.o obj/compare.o obj/classifier.o obj/local_layer.o obj/swag.o obj/shortcut_layer.o obj/activation_layer.o obj/rnn_layer.o obj/gru_layer.o obj/rnn.o obj/rnn_vid.o obj/crnn_layer.o obj/demo.o obj/tag.o obj/cifar.o obj/go.o obj/batchnorm_layer.o obj/art.o obj/region_layer.o obj/reorg_layer.o obj/super.o obj/voxel.o obj/tree.o 

To test if it worked:
1) Uncomment main from videodarknet.cpp
2) Run:
g++ -std=c++0x `pkg-config --cflags opencv` `pkg-config --libs opencv` -DOPENCV -L. -Wall -Wfatal-errors -Ofast  src/videodarknet.cpp -lm -pthread  -ldarknet -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_objdetect -lopencv_features2d -o darknetVideo
3) If no errors, comment main again
