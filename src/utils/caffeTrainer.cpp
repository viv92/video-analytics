/*
    Copyright 2016 AITOE
*/

#include "caffeTrainer.h"

aiSaac::CaffeTrainer::CaffeTrainer(std::string trainingDataPath,
    std::string testingDataPath,
    std::string templateArchitecturePath,
    std::string originalCaffemodelPath,
    std::string workingDir,
    std::string newNetworkName,
    std::string templateSolverPath) {

    this->trainingDataPath = trainingDataPath;
    this->testingDataPath = testingDataPath;
    this->templateArchitecturePath = templateArchitecturePath;
    this->templateSolverPath = templateSolverPath;
    this->originalCaffemodelPath = originalCaffemodelPath;
    this->workingDir = workingDir;
    this->newNetworkName = newNetworkName;

    std::string databasePath = this->workingDir + "/database";
    std::system(("mkdir -p " + databasePath).c_str());
    this->trainLmdbPath = databasePath + "/train_lmdb";
    this->validationLmdbPath = databasePath + "/validation_lmdb";
    // std::system(("mkdir " + this->trainLmdbPath).c_str());
    // std::cout << "created train dir" << std::endl << std::flush;
    // std::system(("mkdir " + this->validationLmdbPath).c_str());

    this->newModelFolderPath = this->workingDir + "/model";
    std::system(("mkdir -p " + this->newModelFolderPath).c_str());
    this->labelFilePath = this->newModelFolderPath + "/labels.txt";
    this->meanFilePath = this->newModelFolderPath + "/mean.binaryproto";
    this->networkFilePath = this->newModelFolderPath + "/" + newNetworkName + ".prototxt";
    this->solverFilePath = this->newModelFolderPath + "/solver.prototxt";

    if(this->solverStatePath.empty()) {
        if(this->testingDataPath.empty()) {
            this->createLmdb();
        } else {
            this->createLmdb(this->trainingDataPath, this->testingDataPath);
        }
        this->generateMean();
        this->modifyAndSaveNetwork();
    }

    caffe::SolverParameter solver_param;
    caffe::ReadSolverParamsFromTextFileOrDie(this->templateSolverPath, &solver_param);
    caffe::WriteProtoToTextFile(solver_param, this->solverFilePath.c_str());
    // this->modifyAndSaveSolver();
}

aiSaac::CaffeTrainer::CaffeTrainer(std::string trainingDataPath,
    std::string testingDataPath,
    std::string templateArchitecturePath,
    std::string templateSolverPath,
    std::string originalCaffemodelPath,
    std::string solverStatePath,
    std::string workingDir,
    std::string newNetworkName,
    std::string backpropOptimizationAlgo,
    int maxIterations,
    int testIterationSize,
    int stepSize,
    float baseLr,
    int solverMode,
    double trainingRatio,
    int resizeWidth,
    int resizeHeight,
    bool isColor,
    float gamma) {

    this->trainingDataPath = trainingDataPath;
    this->testingDataPath = testingDataPath;
    this->templateArchitecturePath = templateArchitecturePath;
    this->templateSolverPath = templateSolverPath;
    this->originalCaffemodelPath = originalCaffemodelPath;
    this->solverStatePath = solverStatePath;
    this->workingDir = workingDir;

    this->newNetworkName = newNetworkName;
    this->backpropOptimizationAlgo = backpropOptimizationAlgo;

    std::string databasePath = this->workingDir + "/database";
    std::system(("mkdir -p " + databasePath).c_str());
    this->trainLmdbPath = databasePath + "/train_lmdb";
    this->validationLmdbPath = databasePath + "/validation_lmdb";
    // std::system(("mkdir " + this->trainLmdbPath).c_str());
    // std::cout << "created train dir" << std::endl << std::flush;
    // std::system(("mkdir " + this->validationLmdbPath).c_str());

    this->newModelFolderPath = this->workingDir + "/model";
    std::system(("mkdir -p " + this->newModelFolderPath).c_str());
    this->labelFilePath = this->newModelFolderPath + "/labels.txt";
    this->meanFilePath = this->newModelFolderPath + "/mean.binaryproto";
    this->networkFilePath = this->newModelFolderPath + "/" + newNetworkName + ".prototxt";
    this->solverFilePath = this->newModelFolderPath + "/solver.prototxt";

    this->maxIterations = maxIterations;
    this->testIterationSize = testIterationSize;
    this->stepSize = stepSize;
    this->baseLr = baseLr;
    this->gamma = gamma;
    this->solverMode = solverMode;
    this->isColor = isColor;
    this->trainingRatio = trainingRatio;
    this->resizeWidth = resizeWidth;
    this->resizeHeight = resizeHeight;

    if(this->solverStatePath.empty()) {
        if(this->testingDataPath.empty()) {
            this->createLmdb();
        } else {
            this->createLmdb(this->trainingDataPath, this->testingDataPath);
        }
        this->generateMean();
        this->modifyAndSaveNetwork();
    }

    this->modifyAndSaveSolver();
}

aiSaac::CaffeTrainer::~CaffeTrainer() {}

void aiSaac::CaffeTrainer::createLmdb() {
    DIR *dataDir;
    struct dirent *direntDataDir;
    DIR *classPath;
    struct dirent *direntClassPath;
    this->dataFilePath = this->trainingDataPath;
    std::cout << "about to start looping through data folder" << std::endl << std::flush;
    dataDir = opendir(this->dataFilePath.c_str());
    std::string classFolder;
    std::string className;
    std::string classImagePath;

    int label = 0;

    std::ofstream labelFile;
    labelFile.open(this->labelFilePath);

    boost::scoped_ptr<caffe::db::DB> trainDb(caffe::db::GetDB("lmdb"));
    trainDb->Open(this->trainLmdbPath, caffe::db::NEW);
    boost::scoped_ptr<caffe::db::Transaction> trainDbTxn(trainDb->NewTransaction());

    boost::scoped_ptr<caffe::db::DB> validationDb(caffe::db::GetDB("lmdb"));
    validationDb->Open(this->validationLmdbPath, caffe::db::NEW);
    boost::scoped_ptr<caffe::db::Transaction> validationDbTxn(validationDb->NewTransaction());

    caffe::Datum datum;

    int trainCount = 0;
    int validationCount = 0;
    this->numberOfClasses = 0;

    while((direntDataDir = readdir(dataDir)) != NULL) {
        classFolder = this->dataFilePath;
        if (direntDataDir->d_type == DT_DIR
            && strcmp(direntDataDir->d_name, ".")
            && strcmp(direntDataDir->d_name, "..")) {
            std::vector<std::pair<std::string, int>> dataList;
            classFolder = classFolder + "/" + direntDataDir->d_name;
            // className = direntDataDir->d_name;
            labelFile << direntDataDir->d_name << "\n";
            this->numberOfClasses++;
            classPath = opendir(classFolder.c_str());
            while ((direntClassPath = readdir(classPath)) != NULL) {
                if (direntClassPath->d_type == DT_REG) {
                    std::string fileName(direntClassPath->d_name);
                    if (fileName.substr(fileName.find_last_of(".") + 1) != "DS_Store") {
                        classImagePath = classFolder + "/" + fileName;
                        dataList.push_back(std::make_pair(classImagePath, label));
                        std::cout << classImagePath << " : " << label << std::endl << std::flush;
                    } else {
                        std::cout << "Ignoring: " << fileName << std::endl;
                    }
                }
            }
            if (dataList.size() == 1) {
                this->trainingList.push_back(dataList[0]);
            } else {
                int i;
                for (i = 0; i < (int) (this->trainingRatio * dataList.size()); i++) {
                    this->trainingList.push_back(dataList[i]);
                    // std::cout << "retreiving encoding: " << std::endl;
                    std::string fn = dataList[i].first;
                    size_t p = fn.rfind('.');
                    std::string enc = fn.substr(p);
                    std::transform(enc.begin(), enc.end(), enc.begin(), ::tolower);
                    bool status = caffe::ReadImageToDatum(dataList[i].first,
                        dataList[i].second,
                        this->resizeHeight,
                        this->resizeWidth,
                        this->isColor,
                        enc,
                        &datum);
                    if (status == false) {
                        continue;
                    }

                    std::string key_str = caffe::format_int(i, 8)
                        + "_"
                        + dataList[i].first;

                    // Put in db
                    std::string out;
                    CHECK(datum.SerializeToString(&out));
                    trainDbTxn->Put(key_str, out);

                    if (++trainCount % 1000 == 0) {
                      // Commit db
                      trainDbTxn->Commit();
                      trainDbTxn.reset(trainDb->NewTransaction());
                      std::cout << "Processed " << trainCount << " files." << std::endl;
                    }
                }

                if (trainCount % 1000 != 0) {
                    trainDbTxn->Commit();
                    std::cout << "Processed " << trainCount << " files." << std::endl;
                }

                int j = i;
                for (; j < dataList.size(); j++) {
                    this->validationList.push_back(dataList[j]);
                    std::cout << "retreiving encoding: " << std::endl;
                    std::string fn = dataList[j].first;
                    size_t p = fn.rfind('.');
                    std::string enc = fn.substr(p);
                    std::transform(enc.begin(), enc.end(), enc.begin(), ::tolower);
                    bool status = caffe::ReadImageToDatum(dataList[j].first,
                        dataList[j].second,
                        this->resizeHeight,
                        this->resizeWidth,
                        this->isColor,
                        enc,
                        &datum);
                    if (status == false) {
                        continue;
                    }

                    std::string key_str = caffe::format_int(i, 8)
                        + "_"
                        + dataList[j].first;

                    // Put in db
                    std::string out;
                    CHECK(datum.SerializeToString(&out));
                    validationDbTxn->Put(key_str, out);

                    if (++validationCount % 1000 == 0) {
                      // Commit db
                      validationDbTxn->Commit();
                      validationDbTxn.reset(validationDb->NewTransaction());
                      std::cout << "Processed " << validationCount << " files." << std::endl;
                    }
                }

                if (validationCount % 1000 != 0) {
                    validationDbTxn->Commit();
                    std::cout << "Processed " << validationCount << " files." << std::endl;
                }
            }
            label++;
        }
    }
}

void aiSaac::CaffeTrainer::createLmdb(std::string trainingDataPath, std::string testingDataPath) {
    DIR *dataDir;
    struct dirent *direntDataDir;
    DIR *classPath;
    struct dirent *direntClassPath;

    dataDir = opendir(trainingDataPath.c_str());

    int label,i = 0;
    std::string classFolder;
    std::string className;
    std::string classImagePath;
    std::map<std::string, int> trainingStringToIntLabels;

    std::ofstream labelFile;
    labelFile.open(this->labelFilePath);

    caffe::Datum datum;

    int trainCount = 0;
    int validationCount = 0;
    this->numberOfClasses = 0;

    while((direntDataDir = readdir(dataDir)) != NULL) {
        if(direntDataDir->d_type == DT_DIR && strcmp(direntDataDir->d_name,"..") && strcmp(direntDataDir->d_name,".")){
            // cout << direntDataDir->d_name<<endl;
            trainingStringToIntLabels.insert(std::make_pair(direntDataDir->d_name, i++));
        }
    }

    boost::scoped_ptr<caffe::db::DB> trainDb(caffe::db::GetDB("lmdb"));
    trainDb->Open(this->trainLmdbPath, caffe::db::NEW);
    boost::scoped_ptr<caffe::db::Transaction> trainDbTxn(trainDb->NewTransaction());

    std::cout << "Converting training data to lmdb" << std::endl;

    dataDir = opendir(trainingDataPath.c_str());
    while((direntDataDir = readdir(dataDir)) != NULL) {
        classFolder = trainingDataPath;
        if(direntDataDir->d_type == DT_DIR && strcmp(direntDataDir->d_name,"..") && strcmp(direntDataDir->d_name,".")){
            className = direntDataDir->d_name;
            labelFile << className << "\n";
            this->numberOfClasses++;
            classFolder = classFolder + "/" + direntDataDir->d_name;
            classPath = opendir(classFolder.c_str());
            std::vector<std::pair<std::string, int>> trainingData;
            while((direntClassPath = readdir(classPath)) != NULL) {
                if((direntClassPath->d_type == DT_REG) && (strcmp(direntClassPath->d_name,"_DS_Store"))){
                    classImagePath = classFolder + "/" + direntClassPath->d_name;
                    label = trainingStringToIntLabels.find(className)->second;
                    trainingData.push_back(std::make_pair(classImagePath, label));
                    std::cout << classImagePath << " : " << label << std::endl << std::flush;
                }else {
                    std::cout<<"Ignoring" << " : " << direntClassPath->d_name << std::endl << std::flush;
                }
            }
            //TODO: BUG? WHY IF THERE IS ONLY ONE DATA POINT, IT IS BEING IGNORED?
            if(trainingData.size() == 1) {
                //std::cout << "Training data size is 1~" << std::endl;
                this->trainingList.push_back(trainingData[0]);
            }else {
                for (int i = 0; i < trainingData.size(); i++) {
                    this->trainingList.push_back(trainingData[i]);
                    // std::cout << "retreiving encoding: " << std::endl;
                    std::string fn = trainingData[i].first;
                    size_t p = fn.rfind('.');
                    std::string enc = fn.substr(p);
                    std::transform(enc.begin(), enc.end(), enc.begin(), ::tolower);
                    bool status = caffe::ReadImageToDatum(trainingData[i].first,
                        trainingData[i].second,
                        this->resizeHeight,
                        this->resizeWidth,
                        this->isColor,
                        enc,
                        &datum);
                        //std::cout << "Status = " << status << std::endl;
                    if (status == false) {
                        continue;
                    }

                    std::string key_str = caffe::format_int(i, 8)
                        + "_"
                        + trainingData[i].first;

                    // Put in db
                    std::string out;
                    CHECK(datum.SerializeToString(&out));
                    trainDbTxn->Put(key_str, out);
                    //std::cout << "Stored " << key_str << "in train_db" << std::endl;


                    if (++trainCount % 1000 == 0) {
                      // Commit db
                      trainDbTxn->Commit();
                      //std::cout << "Transaction commited for train db" << std::endl;
                      trainDbTxn.reset(trainDb->NewTransaction());
                      std::cout << "Processed " << trainCount << " files." << std::endl;
                    }
                }

                if (trainCount % 1000 != 0) {
                    trainDbTxn->Commit();
                    //std::cout << "Transaction committed from other if for train db" << std::endl;
                    std::cout << "Processed " << trainCount << " files." << std::endl;
                }
            }
        }
    }

    boost::scoped_ptr<caffe::db::DB> validationDb(caffe::db::GetDB("lmdb"));
    validationDb->Open(this->validationLmdbPath, caffe::db::NEW);
    boost::scoped_ptr<caffe::db::Transaction> validationDbTxn(validationDb->NewTransaction());

    std::cout << "Converting testing data to lmdb" << std::endl;

    dataDir = opendir(testingDataPath.c_str());
    while((direntDataDir = readdir(dataDir)) != NULL) {
        classFolder = testingDataPath;
        if(direntDataDir->d_type == DT_DIR && strcmp(direntDataDir->d_name,"..") && strcmp(direntDataDir->d_name,".")){
            className = direntDataDir->d_name;
            classFolder = classFolder + "/" + direntDataDir->d_name;
            classPath = opendir(classFolder.c_str());
            std::vector<std::pair<std::string, int>> testingData;
            while((direntClassPath = readdir(classPath)) != NULL) {
                if((direntClassPath->d_type == DT_REG) && (strcmp(direntClassPath->d_name,"_DS_Store"))){
                    classImagePath = classFolder + "/" + direntClassPath->d_name;
                    label = trainingStringToIntLabels.find(className)->second;
                    testingData.push_back(std::make_pair(classImagePath, label));
                    std::cout << classImagePath << " : " << label << std::endl << std::flush;
                }else {
                    std::cout<<"Ignoring" << " : " << direntClassPath->d_name << std::endl << std::flush;
                }
            }
            if(testingData.size() == 1) {
                this->validationList.push_back(testingData[0]);
            }else {
                for (int i = 0; i < testingData.size(); i++) {
                    this->validationList.push_back(testingData[i]);
                    // std::cout << "retreiving encoding: " << std::endl;
                    std::string fn = testingData[i].first;
                    size_t p = fn.rfind('.');
                    std::string enc = fn.substr(p);
                    std::transform(enc.begin(), enc.end(), enc.begin(), ::tolower);
                    bool status = caffe::ReadImageToDatum(testingData[i].first,
                        testingData[i].second,
                        this->resizeHeight,
                        this->resizeWidth,
                        this->isColor,
                        enc,
                        &datum);
                    if (status == false) {
                        continue;
                    }

                    std::string key_str = caffe::format_int(i, 8)
                        + "_"
                        + testingData[i].first;

                    // Put in db
                    std::string out;
                    CHECK(datum.SerializeToString(&out));
                    validationDbTxn->Put(key_str, out);
                    //std::cout << "Stored " << key_str << "in test_db" << std::endl;

                    if (++validationCount % 1000 == 0) {
                      // Commit db
                      validationDbTxn->Commit();
                      //std::cout << "Transaction commited for test db" << std::endl;
                      validationDbTxn.reset(trainDb->NewTransaction());
                      std::cout << "Processed " << validationCount << " files." << std::endl;
                    }
                }

                if (validationCount % 1000 != 0) {
                    validationDbTxn->Commit();
                    //std::cout << "Transaction commited for test db from other if" << std::endl;
                    std::cout << "Processed " << validationCount << " files." << std::endl;
                }
            }
        }
    }
}

void aiSaac::CaffeTrainer::generateMean() {
    boost::scoped_ptr<caffe::db::DB> db(caffe::db::GetDB("lmdb"));
    db->Open(this->trainLmdbPath, caffe::db::READ);
    boost::scoped_ptr<caffe::db::Cursor> cursor(db->NewCursor());

    caffe::BlobProto sum_blob;
    int count = 0;
    // load first datum
    caffe::Datum datum;
    datum.ParseFromString(cursor->value());

    if (caffe::DecodeDatumNative(&datum)) {
      std::cout << "Decoding Datum" << std::endl;
    }

    sum_blob.set_num(1);
    sum_blob.set_channels(datum.channels());
    sum_blob.set_height(datum.height());
    sum_blob.set_width(datum.width());
    const int data_size = datum.channels() * datum.height() * datum.width();
    int size_in_datum = std::max<int>(datum.data().size(), datum.float_data_size());
    for (int i = 0; i < size_in_datum; ++i) {
        sum_blob.add_data(0.);
    }
    std::cout << "Starting Iteration" << std::endl;
    while (cursor->valid()) {
        caffe::Datum datum;
        datum.ParseFromString(cursor->value());
        caffe::DecodeDatumNative(&datum);

        const std::string& data = datum.data();
        size_in_datum = std::max<int>(datum.data().size(), datum.float_data_size());
        CHECK_EQ(size_in_datum, data_size) << "Incorrect data field size " << size_in_datum;
        if (data.size() != 0) {
            CHECK_EQ(data.size(), size_in_datum);
            for (int i = 0; i < size_in_datum; ++i) {
                sum_blob.set_data(i, sum_blob.data(i) + (uint8_t)data[i]);
            }
        } else {
            CHECK_EQ(datum.float_data_size(), size_in_datum);
            for (int i = 0; i < size_in_datum; ++i) {
                sum_blob.set_data(i, sum_blob.data(i) +
                    static_cast<float>(datum.float_data(i)));
            }
        }
        ++count;
        if (count % 10000 == 0) {
            std::cout << "Processed " << count << " files." << std::endl;
        }
        cursor->Next();
    }

    if (count % 10000 != 0) {
        std::cout << "Processed " << count << " files." << std::endl;
    }
    for (int i = 0; i < sum_blob.data_size(); ++i) {
        sum_blob.set_data(i, sum_blob.data(i) / count);
    }

    std::cout << "Write to " << this->meanFilePath << std::endl;
    WriteProtoToBinaryFile(sum_blob, this->meanFilePath);

    const int channels = sum_blob.channels();
    const int dim = sum_blob.height() * sum_blob.width();
    std::vector<float> mean_values(channels, 0.0);
    std::cout << "Number of channels: " << channels << std::endl;
    for (int c = 0; c < channels; ++c) {
        for (int i = 0; i < dim; ++i) {
            mean_values[c] += sum_blob.data(dim * c + i);
        }
        std::cout << "mean_value channel [" << c << "]:" << mean_values[c] / dim << std::endl;
    }
}

void aiSaac::CaffeTrainer::modifyAndSaveNetwork() {
    caffe::NetParameter param;
    caffe::ReadNetParamsFromTextFileOrDie(this->templateArchitecturePath, &param);

    // std::cout << "done done done" << std::endl;
    param.clear_name();
    param.set_name(this->newNetworkName);

    std::cout << "layer size: " << param.layer_size() << std::endl;
    std::cout << "layers size: " << param.layers_size() << std::endl;

    for (int i = 0; i < param.layer_size(); i++) {
        caffe::LayerParameter *layerParam = param.mutable_layer(i);
        for (int j = 0; j < layerParam->include_size(); j++) {
            if (!std::strcmp(layerParam->name().c_str(), "data")) {
                if (layerParam->include(j).phase() == 0) {
                    std::cout << "got it training: " << layerParam->name() << std::endl;
                    if (layerParam->has_transform_param()) {
                        if (layerParam->transform_param().has_mean_file()) {
                            std::cout << "current mean file" << layerParam->transform_param().mean_file() << std::endl;
                            layerParam->mutable_transform_param()->set_mean_file(this->meanFilePath);
                            std::cout << "new mean file" << layerParam->transform_param().mean_file() << std::endl;
                        } else if (layerParam->transform_param().mean_value_size()) {
                            std::cout << "mean values found!" << std::endl;
                            layerParam->mutable_transform_param()->clear_mean_value();
                            layerParam->mutable_transform_param()->set_mean_file(this->meanFilePath);
                            std::cout << "new mean file" << layerParam->transform_param().mean_file() << std::endl;
                        }
                    }
                    if (layerParam->has_data_param()
                        && layerParam->data_param().has_source()) {
                        std::cout << "current data file" << layerParam->data_param().source() << std::endl;
                        layerParam->mutable_data_param()->set_source(this->trainLmdbPath);
                        std::cout << "new data file" << layerParam->data_param().source() << std::endl;
                    } else {
                        std::cout << "no data or source param" << std::endl;
                    }
                } else {
                    std::cout << "this is testing" << layerParam->name() << std::endl;
                    if (layerParam->has_transform_param()) {
                        if (layerParam->transform_param().has_mean_file()) {
                            std::cout << "current mean file" << layerParam->transform_param().mean_file() << std::endl;
                            layerParam->mutable_transform_param()->set_mean_file(this->meanFilePath);
                            std::cout << "new mean file" << layerParam->transform_param().mean_file() << std::endl;
                        } else if (layerParam->transform_param().mean_value_size()) {
                            std::cout << "mean values found!" << std::endl;
                            layerParam->mutable_transform_param()->clear_mean_value();
                            layerParam->mutable_transform_param()->set_mean_file(this->meanFilePath);
                            std::cout << "new mean file" << layerParam->transform_param().mean_file() << std::endl;
                        }
                    }

                    if (layerParam->has_data_param()
                        && layerParam->data_param().has_source()) {
                        std::cout << "current data file" << layerParam->data_param().source() << std::endl;
                        layerParam->mutable_data_param()->set_source(this->validationLmdbPath);
                        std::cout << "new data file" << layerParam->data_param().source() << std::endl;
                    } else {
                        std::cout << "no data or source param" << std::endl;
                    }
                }
            }
        }

        if (!std::strcmp(layerParam->type().c_str(), "InnerProduct") &&
            (!std::strcmp(param.layer(i + 1).type().c_str(), "SoftmaxWithLoss") ||
            !std::strcmp(param.layer(i + 1).type().c_str(), "Accuracy"))) {
            std::cout << "current output" << layerParam->inner_product_param().num_output()
                << std::endl;
            layerParam->mutable_inner_product_param()->set_num_output(this->numberOfClasses);
            std::cout << "new output" << layerParam->inner_product_param().num_output()
                << std::endl;

            std::cout << "current name: " << layerParam->name() << std::endl;
            std::string prevName = layerParam->name();
            std::string newName = this->newNetworkName + "_" + std::to_string(i) + layerParam->name();
            std::cout << this->newNetworkName + "_" + std::to_string(i) << std::endl;
            std::cout << layerParam->name() << std::endl;

            layerParam->clear_name();
            layerParam->set_name(newName);
            std::cout << "new name: " << layerParam->name() << std::endl;

            for (int j = i; j < param.layer_size(); j++) {
                caffe::LayerParameter *layerParam2 = param.mutable_layer(j);
                for (int k = 0; k < layerParam2->top_size(); k++) {
                    if (!std::strcmp(layerParam2->top(k).c_str(), prevName.c_str())) {
                        std::cout << "found prevName in layer: " << layerParam2->name() << std::endl;
                        layerParam2->set_top(k, newName);
                    }
                }
                for (int k = 0; k < layerParam2->bottom_size(); k++) {
                    // std::cout << "bottom: " << layerParam2->bottom(k) << std::endl;
                    if (!std::strcmp(layerParam2->bottom(k).c_str(), prevName.c_str())) {
                        std::cout << "found prevName in layer: " << layerParam2->name() << std::endl;
                        layerParam2->set_bottom(k, newName);
                    }
                }
            }
        }
    }
    caffe::WriteProtoToTextFile(param, this->networkFilePath.c_str());
}

void aiSaac::CaffeTrainer::modifyAndSaveSolver() {
    caffe::SolverParameter solver_param;
    caffe::ReadSolverParamsFromTextFileOrDie(this->templateSolverPath, &solver_param);

    std::cout << "current net: " << solver_param.net() << std::endl;
    solver_param.set_net(this->networkFilePath);
    std::cout << "new net: " << solver_param.net() << std::endl;

    std::cout << "current optimization algorithm: " << solver_param.type() << std::endl;
    solver_param.set_type(this->backpropOptimizationAlgo);
    std::cout << "new optimization algorithm: " << solver_param.type() << std::endl;

    std::cout << "current max iterations: " << solver_param.max_iter() << std::endl;
    solver_param.set_max_iter(this->maxIterations);
    std::cout << "new max iterations: " << solver_param.max_iter() << std::endl;

    if(!this->solverStatePath.empty()) {
        std::cout << "current snapshot_prefix: " << solver_param.snapshot_prefix() << std::endl;
        solver_param.set_snapshot_prefix(this->newModelFolderPath + "/" + solver_param.snapshot_prefix());
        std::cout << "new snapshot_prefix: " << solver_param.snapshot_prefix() << std::endl;
    }

    for (int i = 0; i < solver_param.test_iter_size(); i++) {
        std::cout << "current test iter " + std::to_string(i) + ": "
            << solver_param.test_iter(i) << std::endl;
        solver_param.set_test_iter(i, this->testIterationSize);
        std::cout << "current test iter " + std::to_string(i) + ": "
            << solver_param.test_iter(i) << std::endl;
    }

    std::cout << "current test interval: " << solver_param.test_interval() << std::endl;
    solver_param.set_test_interval(this->testIterationSize);
    std::cout << "new test interval: " << solver_param.test_interval() << std::endl;

    std::cout << "current stepsize: " << solver_param.stepsize() << std::endl;
    solver_param.set_stepsize(this->stepSize);
    std::cout << "new stepsize: " << solver_param.stepsize() << std::endl;

    std::cout << "current base_lr: " << solver_param.base_lr() << std::endl;
    solver_param.set_base_lr(this->baseLr);
    std::cout << "new base_lr: " << solver_param.base_lr() << std::endl;

    std::cout << "current gamma: " << solver_param.gamma() << std::endl;
    solver_param.set_gamma(this->gamma);
    std::cout << "new gamma: " << solver_param.gamma() << std::endl;

    std::cout << "current solver mode: " <<  solver_param.solver_mode() << std::endl;
    if (this->solverMode) {
        solver_param.set_solver_mode(caffe::SolverParameter_SolverMode_GPU);
    } else {
        solver_param.set_solver_mode(caffe::SolverParameter_SolverMode_CPU);
    }

    std::cout << "new solver mode: " <<  solver_param.solver_mode() << std::endl;

    caffe::WriteProtoToTextFile(solver_param, this->solverFilePath.c_str());
}

void aiSaac::CaffeTrainer::train() {
    caffe::SolverParameter solver_param;
    caffe::ReadSolverParamsFromTextFileOrDie(this->solverFilePath, &solver_param);
    solver_param.mutable_train_state()->set_level(0);  // level flag
    if (this->solverMode) {
        caffe::Caffe::set_mode(caffe::Caffe::GPU);
    } else {
        caffe::Caffe::set_mode(caffe::Caffe::CPU);
    }

    caffe::SignalHandler signal_handler(
        caffe::SolverAction::STOP,  // sigint effect
        caffe::SolverAction::SNAPSHOT);  // sighup effect

    boost::shared_ptr<caffe::Solver<float>>
        solver(caffe::SolverRegistry<float>::CreateSolver(solver_param));

    solver->SetActionFunction(signal_handler.GetActionFunction());
    std::vector<std::string> model_names;

    if(!this->solverStatePath.empty()) {
        LOG(INFO) << "Restoring from snapshot";
        solver->Restore(this->solverStatePath.c_str());
    }else if(!this->originalCaffemodelPath.empty()) {
        boost::split(model_names, this->originalCaffemodelPath, boost::is_any_of(",") );
        for (int i = 0; i < model_names.size(); ++i) {
            LOG(INFO) << "Finetuning from " << model_names[i];
            solver.get()->net()->CopyTrainedLayersFrom(model_names[i]);
            for (int j = 0; j < solver.get()->test_nets().size(); ++j) {
                solver.get()->test_nets()[j]->CopyTrainedLayersFrom(model_names[i]);
            }
        }
    }

    std::cout << "Starting optimization" << std::endl;
    solver->Solve();
    std::cout << "Optimization done!" << std::endl;
}
