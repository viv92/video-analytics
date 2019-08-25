#include <array>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include "../../../tools/videoSources/fileStreamer.h"
#include "../../../tools/videoSources/aiVideoStreamer.h"
#include "../../../tools/logging.h"
#include "../../../src/aiSaac.h"
#include <ctime>
#include <fstream>

/*variables for parsing*/
//a bounding box
typedef std::array<int,4> BB;
//an object = object id + its bounding box
typedef std::pair<int,BB> OBJ;
//a frame = frame number + all objects in it
typedef std::pair< int,std::vector<OBJ> > GTFRAME;
//a container for all frames in video
std::vector<GTFRAME> gtFrameContainer;
//number of unique ground truth blobs in video
int numUniqObj = 0;

void parseXML(char* xmlPath)
{
    BB bb;
    OBJ obj;
    std::vector<OBJ> allobj;
    GTFRAME gtframe;
    std::string tmp;
    rapidxml::file<> xmlFile(xmlPath); // Default template is char
    rapidxml::xml_document<> doc;
    doc.parse<0>(xmlFile.data());
    int objid, fnum, i;
    std::string frameNum, objID;
    rapidxml::xml_node<> *objNode;
    rapidxml::xml_node<> *boxNode;
    rapidxml::xml_node<> *frameNode = doc.first_node()->first_node();
    while(frameNode) {
        frameNum = frameNode->first_attribute("number")->value();
        fnum = stoi(frameNum);
        //std::cout << "framenum = " << fnum << std::endl;
        objNode = frameNode->first_node()->first_node();
        while(objNode) {
            objID = objNode->first_attribute("id")->value();
            objid = stoi(objID);
            if(numUniqObj < objid)
                numUniqObj = objid;
            //std::cout << "objID = " << objid << "\t";
            boxNode = objNode->first_node("box");
            i=0;
            for (rapidxml::xml_attribute<> *attr = boxNode->first_attribute();
         attr; attr = attr->next_attribute()) {
                //allobj[objid][i] = attr->value();
                //std::cout << allobj[objid][i] << " ";
                tmp = attr->value();
                bb[i] = stoi(tmp);
                i++;
            }
            obj.first = objid;
            obj.second = bb;
            allobj.push_back(obj);

            //std::cout << std::endl;
            objNode = objNode->next_sibling();
        }

        gtframe.first = fnum;
        gtframe.second = allobj;
        gtFrameContainer.push_back(gtframe);

        allobj.clear();
        frameNode = frameNode->next_sibling();
    }
}

int main(int argc, char* argv[]) {

    if( argc < 5 ) {
        std::cout << "Usage: ./mltestHumanTrack [xmlFile] [sourcefile] [aiSaacSettingsPath] [outputCSVFile] [algo] [detectThreshold] [trackThreshold] [algoInterval] [algoMinNeighbours] [algoFlags] [algoThreshold] [algoStepThrough] \n";
        return -1;
    }

    //variables for running time of program
    time_t t1,t2;
    t1 = time(NULL);

    //parse gt XML
    parseXML(argv[1]);

    //variables for tracking
    int trackLoss=0; //cumulative tracking loss
    int numSwitches=0; //no. of wrong switches
    int fnTrackLoss=0; //cumulative false negative tracking loss
    double trackThreshold=0.5;
    numUniqObj += 1; //accounting for zero indexing
    std::vector<int> objMap(numUniqObj,-1); //current gt to prediction mapping
    std::vector<int> correctMap(numUniqObj,-1); //correct gt to prediction mapping
    std::vector<int> blobTrackLoss(numUniqObj,0); //blob wise tracking loss


    std::string file = argv[2];
    std::string aiSaacSettingsPath = argv[3];
    std::string outputCSVFile = argv[4];
    std::string algo;

    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsPath);
    if (argc > 5) {
        algo = argv[5];
        aiSaacSettings.setHumanDetectAnalyticsAlgo(algo);
    }
    aiSaac::HumanAnalytics humanAnalytics = aiSaac::HumanAnalytics(&aiSaacSettings);
    if ( file.find("mp4") < file.length() || file.find("mpg") < file.length() ) {
        FileStreamer *fileStreamer = new FileStreamer(file);
        if(!fileStreamer->isStreaming()) {
            debugMsg("Unable to open file for reading");
            return -1;
        } else {
            std::cout << "Found file for reading" << std::endl;
        }

        cv::Mat currentFrame, annotatedFrame;
        int frameNumber = 0;
        int totalProcessedFrameNumber = 0;
        int FPS = fileStreamer->getFileFPS();
        int procFPS = aiSaacSettings.getProcFPS();;
        cv::Size S;
        //variables for rects
        int h,w,xc,yc,pp,objTaken;
        //variables for counting detection results
        int TP=0, FP=0, FN=0;
        double tightness=0;
        //variable for intersection over union area threshold
        double iouAreaThreshold=0.5;
        //variables for algo params with defaults
        int algoInterval=8, algoMinNeighbours=1, algoFlags=0, algoStepThrough=1;
        float algoThreshold=0.5;

        if (argc > 6)
            iouAreaThreshold = stod(argv[6]);
        if (argc > 7)
            trackThreshold = stod(argv[7]);
        if (argc > 8)
            algoInterval = stoi(argv[8]);
        if (argc > 9)
            algoMinNeighbours = stoi(argv[9]);
        if (argc > 10)
            algoFlags = stoi(argv[10]);
        if (argc > 11)
            algoThreshold = stof(argv[11]);
        if (argc > 12)
            algoStepThrough = stoi(argv[12]);
        if (algo == "DPM") {
            aiSaacSettings.setDPM_interval(algoInterval);
            aiSaacSettings.setDPM_min_neighbours(algoMinNeighbours);
            aiSaacSettings.setDPM_flags(algoFlags);
            aiSaacSettings.setDPM_threshold(algoThreshold);
        }
        if (algo == "ICF") {
            aiSaacSettings.setICF_interval(algoInterval);
            aiSaacSettings.setICF_min_neighbours(algoMinNeighbours);
            aiSaacSettings.setICF_flags(algoFlags);
            aiSaacSettings.setICF_threshold(algoThreshold);
            aiSaacSettings.setICF_step_through(algoStepThrough);
        }

        S = fileStreamer->getFileFrameSize();
        if ( procFPS < 1 ) {
            procFPS = 1;
        } else if ( procFPS > FPS ) {
            procFPS = FPS;
        }

        while (true) {
            currentFrame = fileStreamer->getFrame();
            if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                break;
            }
            totalProcessedFrameNumber++;
        }

        fileStreamer = new FileStreamer(file);
        while (true) {
            currentFrame = fileStreamer->getFrame();
            if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                break;
            }
            annotatedFrame = currentFrame;

            //just for verification/visualisation of gt annotations
            for(int j=0; j < gtFrameContainer[frameNumber].second.size(); j++) {
                h = gtFrameContainer[frameNumber].second[j].second[0];
                w = gtFrameContainer[frameNumber].second[j].second[1];
                xc = gtFrameContainer[frameNumber].second[j].second[2];
                yc = gtFrameContainer[frameNumber].second[j].second[3];

                cv::Rect gtRectangle(cv::Point(xc-w/2,yc-h/2),cv::Point(xc+w/2,yc+h/2));
                cv::rectangle(annotatedFrame,
                gtRectangle,
                cv::Scalar(0, 0, 255), 1);
            }

            if (frameNumber % (FPS / procFPS) == 0) {

                //do tracking and predict bb
                humanAnalytics.track(currentFrame, frameNumber);

                //initialise result counts
                int tp = 0; int fp = 0; int fn = 0;

                //get number of predictions in the frame
                int predsize = 0;
                std::vector<int> predIndexMap; //required for track map
                for(int i=0; i < humanAnalytics.blobContainer.size(); i++) {
                    if(humanAnalytics.blobContainer[i].lastFrameNumber == frameNumber) {
                        predsize++;
                        predIndexMap.push_back(humanAnalytics.blobContainer[i].ID);
                    }
                }

                //get number of gt in frame
                int gtsize = gtFrameContainer[frameNumber].second.size();

                //initialise 2D matrix for recording iouArea between all combinations of gt objects and predictions in the frame
                std::vector< std::vector<double> > iouMat(predsize, std::vector<double>(gtsize,0));

                int mappedI = -1; //required for row indexing of iouMat

                //populate iouMat
                for(int i=0; i < humanAnalytics.blobContainer.size(); i++) {
                    if(humanAnalytics.blobContainer[i].lastFrameNumber == frameNumber) {

                        mappedI++;

                        for(int j=0; j < gtFrameContainer[frameNumber].second.size(); j++) {
                            h = gtFrameContainer[frameNumber].second[j].second[0];
                            w = gtFrameContainer[frameNumber].second[j].second[1];
                            xc = gtFrameContainer[frameNumber].second[j].second[2];
                            yc = gtFrameContainer[frameNumber].second[j].second[3];

                            //calculate iouArea
                            cv::Rect gtRectangle(cv::Point(xc-w/2,yc-h/2),cv::Point(xc+w/2,yc+h/2));
                            double intersectionArea = (gtRectangle & humanAnalytics.blobContainer[i].lastRectangle).area();
                            double unionArea = gtRectangle.area() + humanAnalytics.blobContainer[i].lastRectangle.area() - intersectionArea;
                            double iouArea = intersectionArea/unionArea;

                            //record iouArea after thresholding
                            if(iouArea > iouAreaThreshold) {
                                iouMat[mappedI][j] = iouArea;
                            }
                        }
                    }
                }

                //DEBUG cout
                //std::cout << "predsize" << predsize << std::endl;
                //std::cout << "gtsize" << gtsize << std::endl;
                //std::cout << "r = " << iouMat.size() << std::endl;
                //std::cout << "c = " << iouMat[0].size() << std::endl;
                /*std::cout << "iouMat :" << std::endl;
                for(int i=0; i < predsize; i++) {
                    for(int j=0; j < gtsize; j++) {
                        std::cout << iouMat[i][j] << "\t";
                    }
                    std::cout << std::endl;
                }
                std::cout << "--predIDs--\n";
                for(int i=0; i < predsize; i++) {
                    std::cout << predIndexMap[i] << " ";
                }
                std::cout << "\n--gtIDs--\n";
                for(int i=0; i < gtsize; i++) {
                    std::cout << gtFrameContainer[frameNumber].second[i].first << " ";
                }
                std::cout << "\n--objMap--\n";
                for(int i=0; i < numUniqObj; i++) {
                    std::cout << objMap[i] << " ";
                }
                std::cout << "\n--trackloss = " << trackLoss << std::endl;
                std::cout << "\n--wrongSwitches = " << numSwitches << std::endl;
                cv::waitKey(0);*/

                //calculate tp from iouMat
                double sumArea = 0, maxIouArea = 0;
                int rowindex = -1, colindex = -1, gtID, predID;
                while(true) {
                    sumArea = 0;
                    maxIouArea = 0;
                    for(int i=0; i < predsize; i++) {
                        for(int j=0; j < gtsize; j++) {
                            sumArea += iouMat[i][j];
                            if(maxIouArea < iouMat[i][j]) {
                                maxIouArea = iouMat[i][j];
                                rowindex = i;
                                colindex = j;
                            }
                        }
                    }
                    if(sumArea == 0)
                        break;
                    tp++; //tp found
                    std::cout << "tp found!\n";

                    //track mapping
                    gtID = gtFrameContainer[frameNumber].second[colindex].first;
                    predID = predIndexMap[rowindex];

                    if(objMap[gtID] == -1) { //blob predicted for the first time
                        objMap[gtID] = predID;
                        correctMap[gtID] = predID;
                    } else {
                        if(objMap[gtID] != predID) { //switch in prediction to gl mapping
                            if(correctMap[gtID] != predID) { //is a wrong switch
                                numSwitches += 1;
                            } else { //switching back to correct mapping - does not count as wrong switch

                            }
                            objMap[gtID] = predID; //update current mapping after switch
                        } else { //no switch happened
                            //tracking thresholding
                            if(iouMat[rowindex][colindex] < trackThreshold) {
                                trackLoss += 1; //update cumulative tracking loss
                                blobTrackLoss[gtID] += 1; //update blob-wise tracking loss
                            }
                        }
                    }

                    tightness += maxIouArea;
                    for(int k=0; k < predsize; k++) {
                        iouMat[k][colindex] = 0;
                    }
                    for(int k=0; k < gtsize; k++) {
                        iouMat[rowindex][k] = 0;
                    }
                }

                //calculate fp and fn
                fp = predsize - tp;
                fn = gtsize - tp;
                fnTrackLoss += fn; //update false negative tracking loss

                //update global result counts
                TP += tp;
                FP += fp;
                FN += fn;

            }// done processing the frame

            //just a progress head up
            if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
                std::cout << "AISAAC_LOG: Your video is 25% processed." << std::endl;
            } else if (frameNumber == floor(totalProcessedFrameNumber*0.50)) {
                std::cout << "AISAAC_LOG: Your video is 50% processed." << std::endl;
            } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
                std::cout << "AISAAC_LOG: Your video is 75% processed." << std::endl;
            }

            //draw the annotations
            humanAnalytics.annotate(annotatedFrame, frameNumber);
            if (annotatedFrame.data != NULL) {
                cv::imshow("Annotated Humans", annotatedFrame);
                cv::waitKey(10);
                //annotatedVideo.write(annotatedFrame);
            }

            //move to next frame
            frameNumber++;
        }


        std::cout << "AISAAC_LOG: Your video is 100% processed." << std::endl;

        std::cout << "Detection Results: = " << std::endl;
        std::cout << "TP = " << TP << std::endl;
        std::cout << "FP = " << FP << std::endl;
        std::cout << "FN = " << FN << std::endl;
        std::cout << "Tightness = " << tightness/TP << std::endl;
        double precision, recall, f1score;
        precision = (double)TP/(double)(TP+FP);
        recall = (double)TP/(double)(TP+FN);
        f1score = (2*precision*recall)/(precision+recall);
        std::cout << "Precision = " << precision << std::endl;
        std::cout << "Recall = " << recall << std::endl;
        std::cout << "F1score = " << f1score << std::endl;

        std::cout << "Tracking Results: = " << std::endl;
        std::cout << "Tightness = " << tightness/TP << std::endl;
        std::cout << "No. of wrong switches = " << numSwitches << std::endl;
        std::cout << "Cumulative TP Tracking Loss = " << trackLoss << std::endl;
        std::cout << "Cumulative FP Tracking Loss = " << fnTrackLoss << std::endl;

        //write results to csv
        std::string csvRow = "\nEnterExitCrossingPaths1cor.mpg (CAVIAR DATASET),384 x 288,MPEG-1 Video,25 fps,13 sec,Human Analytics,"+to_string(procFPS)+","+algo+",\"algoInterval: "+to_string(algoInterval)+"\nalgoMinNeighbours: "+to_string(algoMinNeighbours)+"\nalgoFlags: "+to_string(algoFlags)+"\nalgoThreshold: "+to_string(algoThreshold)+"\ndetect_iouAreaThreshold: "+to_string(iouAreaThreshold)+"\",\"TP: "+to_string(TP)+"\nFP: "+to_string(FP)+"\nFN: "+to_string(FN)+"\nPrecision: "+to_string(precision)+"\nRecall: "+to_string(recall)+"\nF1score: "+to_string(f1score)+"\",\"HumanAnalytics default:\n((abs(potentialPosition.x - latestPosition.x) < (rawFrame.size().width / 4)) && (abs(potentialPosition.y - latestPosition.y) < (rawFrame.size().height / 4)))\",track_iouAreaThreshold: "+to_string(trackThreshold)+",\"Tightness: "+to_string(tightness)+"\nWrongSwitches: "+to_string(numSwitches)+"\nTP_traking_loss: "+to_string(trackLoss)+"\nFP_tracking_loss: "+to_string(fnTrackLoss)+"\",NA,NA,NA,"+to_string(f1score)+","+to_string(trackLoss);

        std::ofstream outputfile;
        outputfile.open(outputCSVFile, std::ios_base::app);
        outputfile << csvRow;
        outputfile.close();

        t2 = time(NULL);
        std::cout << "Runtime = " << difftime(t2,t1) << std::endl;

    } else {
        debugMsg("Source file not a video of required encoding");
        return -1;
    }

}
