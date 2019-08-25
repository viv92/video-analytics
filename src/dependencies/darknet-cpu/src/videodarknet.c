#include "network.h"
#include "detection_layer.h"
#include "cost_layer.h"
#include "utils.h"
#include "parser.h"
#include "box.h"
#include "demo.h"
#include "image.h"

char *vocnames[] = {"aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow", "diningtable", "dog", "horse", "motorbike", "person", "pottedplant", "sheep", "sofa", "train", "tvmonitor"};

void videoyolo(char *cfgfile, char *weightfile, char *filename, float thresh)
{
    image *alphabet = load_alphabet();
    network net = parse_network_cfg(cfgfile);
    if(weightfile){
        load_weights(&net, weightfile);
    }
    detection_layer l = net.layers[net.n-1];
    set_batch_network(&net, 1);
    srand(2222222);
    clock_t time;
    char buff[256];
    char *input = buff;
    int j;
    float nms=.4;
    box *boxes = (box*) calloc(l.side*l.side*l.n, sizeof(box));
    float **probs = (float**) calloc(l.side*l.side*l.n, sizeof(float *));
    for(j = 0; j < l.side*l.side*l.n; ++j) probs[j] = (float*) calloc(l.classes, sizeof(float *));
    printf("Video File name is: %s\n", filename);
    CvCapture* cap = cvCaptureFromFile(filename);
    IplImage* frame;
    int frame_number = 0;
    int FPS = 1*30;
    while(1){
        frame = cvQueryFrame(cap);
        if(!frame) break;
        image im = ipl_to_image(frame);
        rgbgr_image(im);
        image sized = resize_image(im, net.w, net.h);
        float *X = sized.data;
        if (frame_number % FPS == 0)
        {
          time=clock();
          network_predict(net, X);
          printf("%s: Predicted in %f seconds.\n", input, sec(clock()-time));
          get_detection_boxes(l, 1, 1, thresh, probs, boxes, 0);
          if (nms) do_nms_sort(boxes, probs, l.side*l.side*l.n, l.classes, nms);
        }
        //draw_detections(im, l.side*l.side*l.n, thresh, boxes, probs, vocnames, alphabet, 20);
        draw_detections(im, l.side*l.side*l.n, thresh, boxes, probs, vocnames, alphabet, 20);
        //save_image(im, "predictions");
        show_image(im, "predictions");
        #ifdef OPENCV
            cvWaitKey(30);
        #endif
        frame_number++;
        free_image(im);
        free_image(sized);
    }
    cvDestroyAllWindows();
}

int main(int argc, char **argv)
{
    char *prefix = find_char_arg(argc, argv, "-prefix", 0);
    float thresh = find_float_arg(argc, argv, "-thresh", .2);
    int cam_index = find_int_arg(argc, argv, "-c", 0);
    int frame_skip = find_int_arg(argc, argv, "-s", 0);
    if(argc < 4){
        fprintf(stderr, "usage: %s %s [train/test/valid] [cfg] [weights (optional)]\n", argv[0], argv[1]);
        return 0;
    }

    char *cfg = argv[3];
    char *weights = (argc > 4) ? argv[4] : 0;
    char *filename = (argc > 5) ? argv[5]: 0;
    videoyolo(cfg, weights, filename, thresh);
    return 0;
}
