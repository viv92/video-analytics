#include <iostream>
#include "../../src/utils/caffeTrainer.h"

// ./aiSaacFineTuning /home/shetty4l/Downloads/AITOEVA/aisaac-fork/build/storageDir/data/FaceRecData1/Train /home/shetty4l/Downloads/AITOEVA/aisaac-fork/build/storageDir/caffeFiles/train_val.prototxt /home/shetty4l/Downloads/AITOEVA/aisaac-fork/build/storageDir/caffeFiles/solver.prototxt /home/shetty4l/Downloads/AITOEVA/aisaac-fork/build/storageDir/caffeFiles/bvlc_reference_caffenet.caffemodel /home/shetty4l/Downloads/AITOEVA/aisaac-fork/build/storageDir/fineTuning

int main(int argc, char *argv[]) {
    if (argc != 6) {
       std::cout << "Usage: ./fineTuningTest [dataPath] [templateArchitecturePath] [solverFilePath] [trainedCaffeModelPath] [workingDir]" << std::endl;
       return -1;
	}

    std::cout << "about to call caffe trainer" << std::endl << std::flush;
    aiSaac::CaffeTrainer caffeTrainer(argv[1], argv[2], argv[3], argv[4], argv[5]);
    std::cout << "lets train!! :S" << std::endl;
    caffeTrainer.train();
}
