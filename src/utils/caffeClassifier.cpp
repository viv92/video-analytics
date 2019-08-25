/*
    Copyright 2016 AITOE
*/

#include "caffeClassifier.h"

static bool PairCompare(const std::pair<float, int>& lhs, const std::pair<float, int>& rhs) {
    return lhs.first > rhs.first;
}

/* Return the indices of the top N values of vector v. */
static std::vector<int> Argmax(const std::vector<float>& v, int N) {
    std::vector<std::pair<float, int>> pairs;
    for (size_t i = 0; i < v.size(); ++i) {
        pairs.push_back(std::make_pair(v[i], static_cast<int>(i)));
    }
    std::partial_sort(pairs.begin(), pairs.begin() + N, pairs.end(), PairCompare);

    std::vector<int> result;
    for (int i = 0; i < N; ++i) {
        result.push_back(pairs[i].second);
    }
    return result;
}

// aiSaac::CaffeClassifier::CaffeClassifier(const std::string &network_file,
//     const std::string &trained_file) {
// #ifdef CPU_ONLY
//     caffe::Caffe::set_mode(caffe::Caffe::CPU);
// #else
//     caffe::Caffe::set_mode(caffe::Caffe::GPU);
// #endif
//
//     /* Load the network. */
//     // codeReview(Anurag): What is TEST
//     this->net_.reset(new caffe::Net<float>(network_file, caffe::TEST));
//     this->net_->CopyTrainedLayersFrom(trained_file);
//
//     //CHECK_EQ(net_->num_inputs(),1) << "Network should have exactly one input.";
//     //CHECK_EQ(net_->num_outputs(),1) << "Network should have exactly one output.";
//
//     caffe::Blob<float>* input_layer = this->net_->input_blobs()[0];
//     this->num_channels_ = input_layer->channels();
//     //CHECK(num_channels_ == 3 || num_channels_ == 1)
//     //    << "Input layer should have 1 or 3 channels.";
//     this->input_geometry_ = cv::Size(input_layer->width(), input_layer->height());
//     std::cout << "initialized caffe classifier" << std::endl;
//
//     // codeReview(Anurag): is this useful to train things?
//     /* Load the binaryproto mean file. */
// }
aiSaac::CaffeClassifier::CaffeClassifier() {
    this->empty = true;
}

aiSaac::CaffeClassifier::CaffeClassifier(const std::string& network_file,
    const std::string& trained_file,
    const std::string& mean_file,
    const std::string& label_file) {

#ifdef CPU_ONLY
   caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
   caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

    /* Load the network. */
    // codeReview(Anurag): What is TEST
    this->net_.reset(new caffe::Net<float>(network_file, caffe::TEST));
    this->net_->CopyTrainedLayersFrom(trained_file);

    //CHECK_EQ(net_->num_inputs(),1) << "Network should have exactly one input.";
    //CHECK_EQ(net_->num_outputs(),1) << "Network should have exactly one output.";

    caffe::Blob<float>* input_layer = this->net_->input_blobs()[0];
    this->num_channels_ = input_layer->channels();
    //CHECK(num_channels_ == 3 || num_channels_ == 1)
    //    << "Input layer should have 1 or 3 channels.";
    this->input_geometry_ = cv::Size(input_layer->width(), input_layer->height());
    std::cout << "initialized caffe classifier" << std::endl;

    if (!mean_file.empty() ) {
        /* Load the binaryproto mean file. */
        this->SetMean(mean_file);
    }

    if (!label_file.empty()) {
        /* Load labels. */
        std::ifstream labels(label_file.c_str());
        CHECK(labels) << "Unable to open labels file " << label_file;
        std::string line;
        while (std::getline(labels, line)) {
            labels_.push_back(std::string(line));
        }

        caffe::Blob<float>* output_layer = this->net_->output_blobs()[0];
        CHECK_EQ(labels_.size(), output_layer->channels())
            << "Number of labels is different from the output layer dimension.";
    }
}

std::vector<std::pair<std::string, float>> aiSaac::CaffeClassifier::Classify(const cv::Mat& img,
    int N) {
    std::vector<float> output = this->Predict(img);

    N = std::min<int>(labels_.size(), N);
    std::vector<int> maxN = Argmax(output, N);
    std::vector<std::pair<std::string, float>> predictions;
    for (int i = 0; i < N; ++i) {
        int idx = maxN[i];
        std::cout << "CLASS: " << labels_[idx] << " PROB: " << output[idx] << std::endl << std::flush;
        predictions.push_back(std::make_pair(labels_[idx], output[idx]));
    }

    return predictions;
}

/* Load the mean file in binaryproto format. */
void aiSaac::CaffeClassifier::SetMean(const std::string& mean_file) {
    caffe::BlobProto blob_proto;
    caffe::ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

    /* Convert from BlobProto to Blob<float> */
    caffe::Blob<float> mean_blob;
    mean_blob.FromProto(blob_proto);
    CHECK_EQ(mean_blob.channels(), num_channels_)
        << "Number of channels of mean file doesn't match input layer.";

    /* The format of the mean file is planar 32-bit float BGR or grayscale. */
    std::vector<cv::Mat> channels;
    float* data = mean_blob.mutable_cpu_data();
    for (int i = 0; i < num_channels_; i++) {
        /* Extract an individual channel. */
        cv::Mat channel(mean_blob.height(), mean_blob.width(), CV_32FC1, data);
        channels.push_back(channel);
        data += mean_blob.height() * mean_blob.width();
    }

    /* Merge the separate channels into a single image. */
    cv::Mat mean;
    cv::merge(channels, mean);

    /* Compute the global mean pixel value and create a mean image
    * filled with this value. */
    cv::Scalar channel_mean = cv::mean(mean);
    this->mean_ = cv::Mat(input_geometry_, mean.type(), channel_mean);
}

std::vector<float> aiSaac::CaffeClassifier::getLayerOutput(std::string layer) {
    caffe::shared_ptr<caffe::Blob<float>> output_layer = this->net_->blob_by_name(layer);
    const float *begin = output_layer->cpu_data();
    const float *end = begin + output_layer->channels();

    return std::vector<float>(begin, end);;
}
// codeReview(Suyash): additional parameter added to print feature extraction time
std::vector<float> aiSaac::CaffeClassifier::Predict(const cv::Mat &img, std::string layer, const bool &verbose) {
#ifdef CPU_ONLY
   caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
   caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif
    //cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE);// Create a window for display.
    //cv::imshow( "Display window", img );
    //cv::waitKey(10);
    //std::cout << "Rows = " << img.rows << " columns = " << img.cols << std::endl;
    //std::cout << "Inside predict"  << this->net_->input_blobs().size() << std::endl;
    caffe::Blob<float>* input_layer = this->net_->input_blobs()[0];
    //std::cout << "Get input layer" << std::endl;
    //std::cout << "Num chanels" << this->num_channels_ << std::endl;
    //std::cout << "input geometry.height" << this->input_geometry_.height << std::endl;
    //std::cout << "input geometry.wirdth" << this->input_geometry_.width << std::endl;
    input_layer->Reshape(1, this->num_channels_,
        this->input_geometry_.height,
        this->input_geometry_.width);
    //std::cout << "Reshaping " << std::endl;
    this->net_->Reshape();
    //std::cout << "Reshaped " << std::endl;

    std::vector<cv::Mat> input_channels;
    //std::cout << "wrapping input layer" << std::endl;
    this->WrapInputLayer(&input_channels);
    //std::cout << "Wrapped now preprocessing" << std::endl;

    this->Preprocess(img, &input_channels); //convert img to caffe input
    //std::cout << "Preprocessed" << std::endl;
    caffe::Timer tmr;
    tmr.Start();
    this->net_->Forward(); // Caffe functions
    tmr.Stop();
    if (verbose == true) {
        std::cout << "Feature extracton time = " << tmr.Seconds() << std::endl;
    }

    /* Copy the output layer to std::std::std::vector */
    if (!layer.empty()) {
        // caffe::shared_ptr<caffe::Blob<float>> output_layer = this->net_->blob_by_name(layer);
        // const float *begin = output_layer->cpu_data();
        // const float *end = begin + output_layer->channels();
        //
        // return std::vector<float>(begin, end);
        return this->getLayerOutput(layer);
    }

    caffe::Blob<float> *output_layer = net_->output_blobs()[0];
    const float *begin = output_layer->cpu_data();
    const float *end = begin + output_layer->channels();

    return std::vector<float>(begin, end);

    // debug
    // std::vector<std::string> tmpnames = this->net_->blob_names();
    // for (int i = 0; i < tmpnames.size(); i++) {
    //     std::cout << tmpnames[i] << " " << std::endl;
    // }

    // std::vector<float> temp = std::vector<float>(begin, end);
    /*std::cout<<std::endl<<"Feature:"<<std::endl;
    for (std::vector<float>::const_iterator i = temp.begin(); i != temp.end(); ++i) {
        std::cout << *i << ' ';
    }*/
}

void aiSaac::CaffeClassifier::Preprocess(const cv::Mat &img, std::vector<cv::Mat> *input_channels) {
    cv::Mat sample;
    if (img.channels() == 3 && this->num_channels_ == 1) {
        cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
    } else if (img.channels() == 4 && this->num_channels_ == 1) {
        cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
    } else if (img.channels() == 4 && this->num_channels_ == 3) {
        cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
    } else if (img.channels() == 1 && this->num_channels_ == 3) {
        cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
    } else {
        sample = img;
    }

    cv::Mat sample_resized;
    if (sample.size() != this->input_geometry_) {
        cv::resize(sample, sample_resized, this->input_geometry_);
    } else {
        sample_resized = sample;
    }

    cv::Mat sample_float;
    if (this->num_channels_ == 3) {
        sample_resized.convertTo(sample_float, CV_32FC3);
    } else {
        sample_resized.convertTo(sample_float, CV_32FC1);
    }

    // cv::Mat sample_normalized;
    // subtract(sample_float, mean_, sample_normalized);
    cv::split(sample_float, *input_channels);

    CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
        == this->net_->input_blobs()[0]->cpu_data())
        << "Input channels are not wrapping the input layer of the network. ";
}

void aiSaac::CaffeClassifier::WrapInputLayer(std::vector<cv::Mat> *input_channels) {
    caffe::Blob<float>* input_layer = this->net_->input_blobs()[0];

    int width = input_layer->width();
    int height= input_layer->height();
    float* input_data = input_layer->mutable_cpu_data();
    for (int i = 0; i < input_layer->channels();++i) {
        cv::Mat channel(height, width, CV_32FC1, input_data);
        input_channels->push_back(channel);
        input_data += width * height;
    }
}

bool aiSaac::CaffeClassifier::isEmpty(){
    return this->empty;
}
