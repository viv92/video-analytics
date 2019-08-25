1. Prerequisites:
a) Install rapidxml: sudo apt-get install librapidxml-dev
b) Add include path for rapidxml in makefile (e.g. RAPIDXMLINCLUDES = -I/home/vivs/aitoe/vivs_aisaac/build/storageDir/data/EvalData/human/rapidxml)
c) Download CAVIAR dataset video "EnterExitCrossingPaths1cor.mpg" and ground thruth xml from http://homepages.inf.ed.ac.uk/rbf/CAVIARDATA1/
d) Store video and xml files under /storageDir/data/EvalData/human/caviar/EnterExitCrossingPaths1/corrview/ 
e) Create a blank csv file to which the results will be written
f) Some params have been extracted out to aisaacSettings.json as they are required to be varied. These are:
    -"dpmFlags": ,
    -"dpmInterval": ,
    -"dpmMinNeighbours": ,
    -"dpmThreshold": ,
    -"icfFlags": ,
    -"icfInterval": ,
    -"icfMinNeighbours": ,
    -"icfStepThrough": ,
    -"icfThreshold": ,



2. Making the program: ("tracking_human.cpp" is the main file)

a) Add make recipe for "/research/MLtesting/HumanAnalytics/tracking_human.cpp", adjusting the paths. For e.g.

SOURCES_MLTEST_HUMAN_ANALYTICS_TRACKING = research/MLtesting/HumanAnalytics/tracking_human.cpp src/utils/dlib/all/*.cpp src/utils/caffeTrainer.cpp src/utils/caffeClassifier.cpp src/utils/darknetClassifier.cpp /home/vivs/aitoe/vivs_aisaac/src/utils/colorClassifier.cpp $(AISAAC_ROOT) $(FACE_AISAAC_SOURCES) $(HUMAN_AISAAC_SOURCES) $(CUSTOM_AISAAC_SOURCES) $(MONGO_SOURCES) $(VIDEO_SOURCES) $(LOGGING_SOURCES)
OBJECTS_MLTEST_HUMAN_ANALYTICS_TRACKING = $(SOURCES_MLTEST_HUMAN_ANALYTICS_TRACKING:.cpp = .o)
EXECUTABLE_MLTEST_HUMAN_ANALYTICS_TRACKING = mltestHumanTrack
$(EXECUTABLE_MLTEST_HUMAN_ANALYTICS_TRACKING):
	$(CC) $(RAPIDXMLINCLUDES) $(OBJECTS_MLTEST_HUMAN_ANALYTICS_TRACKING) $(OPENCVINCLUDES) $(VLCINCLUDES) $(X11INCLUDES) $(CCVINCLUDES) $(GLOGINCLUDES) $(CAFFEINCLUDES) $(BOOSTINCLUDES) $(DARKNETINCLUDES) $(DATKINCLUDES) $(PROTOBUFINCLUDES) $(MONGOINCLUDES) $(OPENCVLIBPATH) $(VLCLIBPATH) $(X11LIBPATH) $(CCVLIBPATH) $(GLOGPATH) $(CAFFELIBPATH) $(BOOSTLIBPATH) $(DARKNETLIBPATH) $(DATKLIBPATH) $(PROTOBUFLIBPATH) $(MONGOLIBPATH) $(OPENCVLIBS) $(VLCLIBS) $(X11LIBS) $(CCVLIBS) $(GLOGLIBS) $(CAFFELIBS) $(BOOSTLIBS) $(DARKNETLIBS) $(DATKLIBS) $(PROTOBUFLIBS) $(MONGOLIBS) $(OTHERLIBS) $(EXTRACAFFE) $(EXTRAMONGO)  $(CUDAINCLUDES) $(CUDALIBS) -o build/$@ 

b) type in terminal: make mltestHumanTrack



3. Running the program

a) Manual
./mltestHumanTrack [xmlFile] [sourcefile] [aiSaacSettingsPath] [outputCSVFile] [algo] [detectThreshold] [trackThreshold] [algoInterval] [algoMinNeighbours] [algoFlags] [algoThreshold] [algoStepThrough]

b)bash script: use humanTrack.sh (attached in mail, not pushed to repo) (requires adjusting the paths)


 
4. NOTES:
a) Any ground truth xml file used should have the following structure:
	
<?xml version="1.0" encoding="UTF-8"?><dataset name="[datasetName]">

    <frame number="0">
        <objectlist>
            <object id="0">
                <box h="79" w="30" xc="145" yc="111"/>
            </object>
            <object id="1">
                <box h="79" w="30" xc="145" yc="111"/>
            </object>
        </objectlist>
    </frame>

    <frame number="1">
        <objectlist>
            <object id="0">
                <box h="79" w="30" xc="145" yc="111"/>
            </object>
            <object id="1">
                <box h="79" w="30" xc="145" yc="111"/>
            </object>
        </objectlist>
    </frame>

</dataset>


b) Currently tested on one video of the CAVIAR dataset only: "EnterExitCrossingPaths1cor.mpg". So some paths might be hardcoded for this video in the main file "tracking_human.cpp"


