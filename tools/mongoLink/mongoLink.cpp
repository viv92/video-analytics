//
// Created by aitoe on 10/2/17.
//

#include "mongoLink.h"

mongocxx::instance inst{};
mongocxx::client conn{mongocxx::uri{}}; // Localhost
//mongocxx::client conn{mongocxx::uri{"mongodb://aitoe:Krishna108@13.82.88.62:27017/aiSaac"}}; // AZURE
using bsoncxx::stdx::string_view;


auto collectionMotion = conn["aiSaac"]["motion"];
auto collectionHead = conn["aiSaac"]["head"];
auto collectionHuman = conn["aiSaac"]["human"];
auto collectionFace = conn["aiSaac"]["face"];
auto collectionAge = conn["aiSaac"]["age"];
auto collectionGender = conn["aiSaac"]["gender"];
auto collectionEmotion = conn["aiSaac"]["emotion"];
auto collectionText = conn["aiSaac"]["text"];
auto collectionObject = conn["aiSaac"]["object"];
auto collectionScene = conn["aiSaac"]["scene"];
auto collectionSummary = conn["aiSaac"]["summary"];
auto collectionResults = conn["aiSaac"]["resultsTable"];


mongoLink::mongoLink(int procFPS){
	this->procFPS=procFPS;
}

void mongoLink::motionMONGO(
    std::vector<aiSaac::BlobObject> blobContainer,
    int frameNumber,
    std::string resultTableId) {
    // std::cout<<"Entered Object Analytics Mongo"<<std::endl;
    for (int i = 0; i < blobContainer.size(); i++) {
        // std::cout<<"Insie Loop"<<std::endl;
        bool foundFlag=false;
        bsoncxx::builder::stream::document init_filter;
        init_filter << "ID"
                    << blobContainer[i].ID
                    << "type"
                    << "motion"
                    << "parentID"
                    << resultTableId;

        // std::cout<<"Init Filter"<<std::endl;
        auto cursor = collectionMotion.find(init_filter.view());
        int count=0;
        for (auto&& doc : cursor) {
            count+=1;
            if(count>0) {
                    count=0;
                    foundFlag=true;
                    break;
            }
        }
        if(foundFlag==false) {
            // std::cout<<"Not Found"<<std::endl;
            if (blobContainer[i].lastRectangle.area() > 0) {
                // std::cout<<"IN loop 2"<<std::endl;
                bsoncxx::builder::stream::document basic_builder{};
                basic_builder
                    << "ID" << blobContainer[i].ID
                    << "type" << "motion"
                    << "parentID"
                    << resultTableId
                    << "startTime" << ((double)blobContainer[i].firstFrameNumber)/
                                      ((double)this->procFPS)
                    << "endTime"   << ((double)blobContainer[i].lastFrameNumber)/
                                      ((double)this->procFPS)
                    << "rectangles"
                    << bsoncxx::builder::stream::open_array
                    << bsoncxx::builder::stream::open_document
                    << "x" << blobContainer[i].lastRectangle.x
                    << "y" << blobContainer[i].lastRectangle.y
                    << "height" << blobContainer[i].lastRectangle.height
                    << "width" << blobContainer[i].lastRectangle.width
                    << "rectTime" << ((double)frameNumber)/
                                     ((double)this->procFPS)
                    << bsoncxx::builder::stream::close_document
                    << bsoncxx::builder::stream::close_array;
                // std::cout<<"After Basic Builder"<<std::endl;
                bsoncxx::document::value basic_doc{basic_builder.extract()};
                auto retUnderId = collectionMotion.insert_one(std::move(basic_doc));
                bsoncxx::oid oid = retUnderId->inserted_id().get_oid().value;
                std::string JobID = oid.to_string();

                // std::cout << "JOB ID: "<<JobID<<std::endl;

                bsoncxx::builder::stream::document filter_builderResult, update_builderResult;
                try {
                    filter_builderResult << "_id" << bsoncxx::oid(resultTableId);
                    // std::cout<<bsoncxx::oid(resultTableId)<<std::endl;
                }

                catch (...) {
                    std::cout<<"caught"<<std::endl;
                }
                update_builderResult
                    << "$push"
                    << bsoncxx::builder::stream::open_document
                    << "motion"
                    << bsoncxx::oid(JobID)
                    << bsoncxx::builder::stream::close_document;


                auto cursorResults = collectionResults.find(filter_builderResult.view());
                int count=0;
                for (auto&& docRes : cursorResults) {
                    // std::cout << bsoncxx::to_json(docRes) << "\n";
                }

                collectionResults.update_one(filter_builderResult.view(), update_builderResult.view());
            }
        }
        else {
            // std::cout<<"In else loop"<<std::endl;
            bsoncxx::builder::stream::document filter_builder, update_builder1, update_builder2;
            if(blobContainer[i].lastFrameNumber == frameNumber) {
                // std::cout<<"in if else"<<std::endl;
                if (blobContainer[i].lastRectangle.area() > 0) {
                    // std::cout<<"push"<<std::endl;
                    filter_builder << "ID"
                                   << blobContainer[i].ID
                                   << "type"
                                   << "motion";
                    update_builder1 << "$set"
                                    << bsoncxx::builder::stream::open_document
                                    << "endTime"
                                    << ((double)blobContainer[i].lastFrameNumber)/
                                       ((double)this->procFPS)
                                    << bsoncxx::builder::stream::close_document;
                    update_builder2
                                    << "$push"
                                    << bsoncxx::builder::stream::open_document
                                    << "rectangles"
                                    << bsoncxx::builder::stream::open_document
                                    << "x" << blobContainer[i].lastRectangle.x
                                    << "y" << blobContainer[i].lastRectangle.y
                                    << "height" << blobContainer[i].lastRectangle.height
                                    << "width" << blobContainer[i].lastRectangle.width
                                    << "rectTime" << ((double)frameNumber)/
                                                     ((double)this->procFPS)
                                    << bsoncxx::builder::stream::close_document
                                    << bsoncxx::builder::stream::close_document ;

                    collectionMotion.update_one(filter_builder.view(), update_builder1.view());
                    collectionMotion.update_one(filter_builder.view(), update_builder2.view());
                }
            }
        }
    }
}

void mongoLink::faceAnalyticsMONGO(
    std::vector<aiSaac::FaceBlob> blobContainer,
    int frameNumber,
    std::string resultTableId,
    std::string resultTableFeature) {
    // std::cout<<"Entered Face Analytics Mongo"<<std::endl;
    for (int i = 0; i < blobContainer.size(); i++) {
        // std::cout<<"Insie Loop"<<std::endl;
        bool foundFlag=false;
        bsoncxx::builder::stream::document init_filter;
        init_filter << "ID"
                    << blobContainer[i].ID
                    << "type"
                    << "faceAnalytics"
                    << "parentID"
                    << resultTableId;

        // std::cout<<"Init Filter"<<std::endl;
        auto cursor = collectionFace.find(init_filter.view());
        int count=0;
        for (auto&& doc : cursor) {
            count+=1;
            if(count>0) {
                    count=0;
                    foundFlag=true;
                    break;
            }
        }
        if(foundFlag==false) {
            // std::cout<<"Not Found"<<std::endl;
            if (blobContainer[i].lastRectangle.area() > 0) {
                // std::cout<<"IN loop 2"<<std::endl;
                bsoncxx::builder::stream::document basic_builder{};
                basic_builder
                    << "ID" << blobContainer[i].ID
                    << "type" << "faceAnalytics"
                    << "parentID"
                    << resultTableId
                    << "labels"
                    << bsoncxx::builder::stream::open_array << blobContainer[i].label
                    << bsoncxx::builder::stream::close_array
                    << "startTime" << ((double)blobContainer[i].firstFrameNumber)/
                                      ((double)this->procFPS)
                    << "endTime"   << ((double)blobContainer[i].lastFrameNumber)/
                                      ((double)this->procFPS)
                    << "rectangles"
                    << bsoncxx::builder::stream::open_array
                    << bsoncxx::builder::stream::open_document
                    << "x" << blobContainer[i].lastRectangle.x
                    << "y" << blobContainer[i].lastRectangle.y
                    << "height" << blobContainer[i].lastRectangle.height
                    << "width" << blobContainer[i].lastRectangle.width
                    << "rectTime" << ((double)frameNumber)/
                                     ((double)this->procFPS)
                    << bsoncxx::builder::stream::close_document
                    << bsoncxx::builder::stream::close_array;
                // std::cout<<"After Basic Builder"<<std::endl;
                bsoncxx::document::value basic_doc{basic_builder.extract()};
                auto retUnderId = collectionFace.insert_one(std::move(basic_doc));
                bsoncxx::oid oid = retUnderId->inserted_id().get_oid().value;
                std::string JobID = oid.to_string();

                std::cout << "JOB ID: "<<JobID<<std::endl;

                bsoncxx::builder::stream::document filter_builderResult, update_builderResult;
                try {
                    filter_builderResult << "_id" << bsoncxx::oid(resultTableId);
                    // std::cout<<bsoncxx::oid(resultTableId)<<std::endl;
                }

                catch (...) {
                    std::cout<<"caught"<<std::endl;
                }
                update_builderResult
                    << "$push"
                    << bsoncxx::builder::stream::open_document
                    << resultTableFeature
                    << bsoncxx::oid(JobID)
                    << bsoncxx::builder::stream::close_document;


                auto cursorResults = collectionResults.find(filter_builderResult.view());
                int count=0;
                for (auto&& docRes : cursorResults) {
                    // std::cout << bsoncxx::to_json(docRes) << "\n";
                }

                collectionResults.update_one(filter_builderResult.view(), update_builderResult.view());
            }
        }
        else {
            // std::cout<<"In else loop"<<std::endl;
            bsoncxx::builder::stream::document filter_builder, update_builder1, update_builder2, update_builder3;
            if(blobContainer[i].lastFrameNumber == frameNumber) {
                // std::cout<<"in if else"<<std::endl;
                if (blobContainer[i].lastRectangle.area() > 0) {
                    // std::cout<<"push"<<std::endl;
                    filter_builder << "ID"
                                   << blobContainer[i].ID
                                   << "type"
                                   << "faceAnalytics";
                    update_builder1 << "$set"
                                    << bsoncxx::builder::stream::open_document
                                    << "endTime"
                                    << ((double)blobContainer[i].lastFrameNumber)/
                                       ((double)this->procFPS)
                                    << bsoncxx::builder::stream::close_document;
                    update_builder2
                                    << "$push"
                                    << bsoncxx::builder::stream::open_document
                                    << "rectangles"
                                    << bsoncxx::builder::stream::open_document
                                    << "x" << blobContainer[i].lastRectangle.x
                                    << "y" << blobContainer[i].lastRectangle.y
                                    << "height" << blobContainer[i].lastRectangle.height
                                    << "width" << blobContainer[i].lastRectangle.width
                                    << "rectTime" << ((double)frameNumber)/
                                                     ((double)this->procFPS)
                                    << bsoncxx::builder::stream::close_document
                                    << bsoncxx::builder::stream::close_document ;

                    update_builder3
                                    << "$push"
                                    << bsoncxx::builder::stream::open_document
                                    << "labels"
                                    << blobContainer[i].label
                                    << bsoncxx::builder::stream::close_document;

                    collectionFace.update_one(filter_builder.view(), update_builder1.view());
                    collectionFace.update_one(filter_builder.view(), update_builder2.view());
                    collectionFace.update_one(filter_builder.view(), update_builder3.view());
                }
            }
        }
    }
}

void mongoLink::ageAnalyticsMONGO(
    std::vector<aiSaac::FaceBlob> blobContainer,
    int frameNumber,
    std::string resultTableId) {
	// std::cout<<"Entered Face Analytics Mongo"<<std::endl;
	for (int i = 0; i < blobContainer.size(); i++) {
		// std::cout<<"Insie Loop"<<std::endl;
		bool foundFlag=false;
		bsoncxx::builder::stream::document init_filter;
		init_filter << "ID"
		            << blobContainer[i].ID
					<< "type"
					<< "ageAnalytics"
					<< "parentID"
					<< resultTableId;

		// std::cout<<"Init Filter"<<std::endl;
		auto cursor = collectionAge.find(init_filter.view());
		int count=0;
		for (auto&& doc : cursor) {
			count+=1;
			if(count>0) {
				count=0;
				foundFlag=true;
				break;
			}
		}
		if(foundFlag==false) {
			// std::cout<<"Not Found"<<std::endl;
			if (blobContainer[i].lastRectangle.area() > 0
                && blobContainer[i].age != "unassigned") {
				// std::cout<<"IN loop 2"<<std::endl;
				bsoncxx::builder::stream::document basic_builder{};
				basic_builder
						<< "ID" << blobContainer[i].ID
						<< "type" << "ageAnalytics"
						<< "parentID"
						<< resultTableId
						<< "labels"
						<< bsoncxx::builder::stream::open_array << blobContainer[i].age
						<< bsoncxx::builder::stream::close_array
						<< "startTime" << ((double)blobContainer[i].firstFrameNumber)/
						                  ((double)this->procFPS)
						<< "endTime"   << ((double)blobContainer[i].lastFrameNumber)/
						                  ((double)this->procFPS)
						<< "rectangles"
						<< bsoncxx::builder::stream::open_array
						<< bsoncxx::builder::stream::open_document
						<< "x" << blobContainer[i].lastRectangle.x
						<< "y" << blobContainer[i].lastRectangle.y
						<< "height" << blobContainer[i].lastRectangle.height
						<< "width" << blobContainer[i].lastRectangle.width
						<< "rectTime" << ((double)frameNumber)/
						                 ((double)this->procFPS)
						<< bsoncxx::builder::stream::close_document
						<< bsoncxx::builder::stream::close_array;
				// std::cout<<"After Basic Builder"<<std::endl;
				bsoncxx::document::value basic_doc{basic_builder.extract()};
				auto retUnderId = collectionAge.insert_one(std::move(basic_doc));
				bsoncxx::oid oid = retUnderId->inserted_id().get_oid().value;
				std::string JobID = oid.to_string();

				std::cout << "JOB ID: "<<JobID<<std::endl;

				bsoncxx::builder::stream::document filter_builderResult, update_builderResult;
				try {
					filter_builderResult << "_id" << bsoncxx::oid(resultTableId);
					// std::cout<<bsoncxx::oid(resultTableId)<<std::endl;
				}

				catch (...) {
					std::cout<<"caught"<<std::endl;
				}
				update_builderResult
						<< "$push"
			            << bsoncxx::builder::stream::open_document
			            << "age"
			            << bsoncxx::oid(JobID)
			            << bsoncxx::builder::stream::close_document;


				auto cursorResults = collectionResults.find(filter_builderResult.view());
				int count=0;
				for (auto&& docRes : cursorResults) {
					// std::cout << bsoncxx::to_json(docRes) << "\n";
				}

				collectionResults.update_one(filter_builderResult.view(), update_builderResult.view());
			}
		}
		else {
			// std::cout<<"In else loop"<<std::endl;
			bsoncxx::builder::stream::document filter_builder, update_builder1, update_builder2, update_builder3;
			if(blobContainer[i].lastFrameNumber == frameNumber) {
				// std::cout<<"in if else"<<std::endl;
				if (blobContainer[i].lastRectangle.area() > 0
                    && blobContainer[i].age != "unassigned") {
					// std::cout<<"push"<<std::endl;
					filter_builder << "ID"
					               << blobContainer[i].ID
					               << "type"
					               << "ageAnalytics";
					update_builder1 << "$set"
					                << bsoncxx::builder::stream::open_document
					                << "endTime"
					                << ((double)blobContainer[i].lastFrameNumber)/
					                   ((double)this->procFPS)
					                << bsoncxx::builder::stream::close_document;
					update_builder2
							<< "$push"
							<< bsoncxx::builder::stream::open_document
							<< "rectangles"
							<< bsoncxx::builder::stream::open_document
							<< "x" << blobContainer[i].lastRectangle.x
							<< "y" << blobContainer[i].lastRectangle.y
							<< "height" << blobContainer[i].lastRectangle.height
							<< "width" << blobContainer[i].lastRectangle.width
							<< "rectTime" << ((double)frameNumber)/
							                 ((double)this->procFPS)
							<< bsoncxx::builder::stream::close_document
							<< bsoncxx::builder::stream::close_document ;

					update_builder3
							<< "$push"
							<< bsoncxx::builder::stream::open_document
							<< "labels"
					        << blobContainer[i].age
							<< bsoncxx::builder::stream::close_document;

					collectionAge.update_one(filter_builder.view(), update_builder1.view());
					collectionAge.update_one(filter_builder.view(), update_builder2.view());
					collectionAge.update_one(filter_builder.view(), update_builder3.view());
				}
			}
		}
	}
}

void mongoLink::emotionAnalyticsMONGO(
    std::vector<aiSaac::FaceBlob> blobContainer,
    int frameNumber,
    std::string resultTableId) {
    // std::cout<<"Entered Face Analytics Mongo"<<std::endl;
    for (int i = 0; i < blobContainer.size(); i++) {
        // std::cout<<"Insie Loop"<<std::endl;
        bool foundFlag=false;
        bsoncxx::builder::stream::document init_filter;
        init_filter << "ID"
                    << blobContainer[i].ID
                    << "type"
                    << "emotionAnalytics"
                    << "parentID"
                    << resultTableId;

        // std::cout<<"Init Filter"<<std::endl;
        auto cursor = collectionEmotion.find(init_filter.view());
        int count=0;
        for (auto&& doc : cursor) {
            count+=1;
            if(count>0) {
                count=0;
                foundFlag=true;
                break;
            }
        }
        if(foundFlag==false) {
            // std::cout<<"Not Found"<<std::endl;
            if (blobContainer[i].lastRectangle.area() > 0
                && blobContainer[i].emotion != "unassigned") {
                // std::cout<<"IN loop 2"<<std::endl;
                bsoncxx::builder::stream::document basic_builder{};
                basic_builder
                        << "ID" << blobContainer[i].ID
                        << "type" << "emotionAnalytics"
                        << "parentID"
                        << resultTableId
                        << "labels"
                        << bsoncxx::builder::stream::open_array << blobContainer[i].emotion
                        << bsoncxx::builder::stream::close_array
                        << "startTime" << ((double)blobContainer[i].firstFrameNumber)/
                                          ((double)this->procFPS)
                        << "endTime"   << ((double)blobContainer[i].lastFrameNumber)/
                                          ((double)this->procFPS)
                        << "rectangles"
                        << bsoncxx::builder::stream::open_array
                        << bsoncxx::builder::stream::open_document
                        << "x" << blobContainer[i].lastRectangle.x
                        << "y" << blobContainer[i].lastRectangle.y
                        << "height" << blobContainer[i].lastRectangle.height
                        << "width" << blobContainer[i].lastRectangle.width
                        << "rectTime" << ((double)frameNumber)/
                                         ((double)this->procFPS)
                        << bsoncxx::builder::stream::close_document
                        << bsoncxx::builder::stream::close_array;
                // std::cout<<"After Basic Builder"<<std::endl;
                bsoncxx::document::value basic_doc{basic_builder.extract()};
                auto retUnderId = collectionEmotion.insert_one(std::move(basic_doc));
                bsoncxx::oid oid = retUnderId->inserted_id().get_oid().value;
                std::string JobID = oid.to_string();

                std::cout << "JOB ID: "<<JobID<<std::endl;

                bsoncxx::builder::stream::document filter_builderResult, update_builderResult;
                try {
                    filter_builderResult << "_id" << bsoncxx::oid(resultTableId);
                    // std::cout<<bsoncxx::oid(resultTableId)<<std::endl;
                }
                catch (...) {
                    std::cout<<"caught"<<std::endl;
                }


                update_builderResult
                        << "$push"
                        << bsoncxx::builder::stream::open_document
                        << "emotion"
                        << bsoncxx::oid(JobID)
                        << bsoncxx::builder::stream::close_document;


                auto cursorResults = collectionResults.find(filter_builderResult.view());
                int count=0;
                for (auto&& docRes : cursorResults) {
                    // std::cout << bsoncxx::to_json(docRes) << "\n";
                }

                collectionResults.update_one(filter_builderResult.view(), update_builderResult.view());
            }
        }
        else {
            // std::cout<<"In else loop"<<std::endl;
            bsoncxx::builder::stream::document filter_builder, update_builder1, update_builder2, update_builder3;
            if(blobContainer[i].lastFrameNumber == frameNumber) {
                // std::cout<<"in if else"<<std::endl;
                if (blobContainer[i].lastRectangle.area() > 0
                    && blobContainer[i].emotion != "unassigned") {
                    // std::cout<<"push"<<std::endl;
                    filter_builder << "ID"
                                   << blobContainer[i].ID
                                   << "type"
                                   << "emotionAnalytics";
                    update_builder1 << "$set"
                                    << bsoncxx::builder::stream::open_document
                                    << "endTime"
                                    << ((double)blobContainer[i].lastFrameNumber)/
                                       ((double)this->procFPS)
                                    << bsoncxx::builder::stream::close_document;
                    update_builder2
                            << "$push"
                            << bsoncxx::builder::stream::open_document
                            << "rectangles"
                            << bsoncxx::builder::stream::open_document
                            << "x" << blobContainer[i].lastRectangle.x
                            << "y" << blobContainer[i].lastRectangle.y
                            << "height" << blobContainer[i].lastRectangle.height
                            << "width" << blobContainer[i].lastRectangle.width
                            << "rectTime" << ((double)frameNumber)/
                                             ((double)this->procFPS)
                            << bsoncxx::builder::stream::close_document
                            << bsoncxx::builder::stream::close_document ;

                    update_builder3
                            << "$push"
                            << bsoncxx::builder::stream::open_document
                            << "labels"
                            << blobContainer[i].emotion
                            << bsoncxx::builder::stream::close_document;

                    collectionEmotion.update_one(filter_builder.view(), update_builder1.view());
                    collectionEmotion.update_one(filter_builder.view(), update_builder2.view());
                    collectionEmotion.update_one(filter_builder.view(), update_builder3.view());
                }
            }
        }
    }
}

void mongoLink::genderAnalyticsMONGO(
    std::vector<aiSaac::FaceBlob> blobContainer,
    int frameNumber,
    std::string resultTableId) {
	// std::cout<<"Entered Face Analytics Mongo"<<std::endl;
	for (int i = 0; i < blobContainer.size(); i++) {
		// std::cout<<"Insie Loop"<<std::endl;
		bool foundFlag=false;
		bsoncxx::builder::stream::document init_filter;
		init_filter << "ID"
		            << blobContainer[i].ID
					<< "type"
					<< "genderAnalytics"
					<< "parentID"
					<< resultTableId;

		// std::cout<<"Init Filter"<<std::endl;
		auto cursor = collectionGender.find(init_filter.view());
		int count=0;
		for (auto&& doc : cursor) {
			count+=1;
			if(count>0) {
				count=0;
				foundFlag=true;
				break;
			}
		}
		if(foundFlag==false) {
			// std::cout<<"Not Found"<<std::endl;
			if (blobContainer[i].lastRectangle.area() > 0
                && blobContainer[i].gender != "unassigned") {
				// std::cout<<"IN loop 2"<<std::endl;
				bsoncxx::builder::stream::document basic_builder{};
				basic_builder
						<< "ID" << blobContainer[i].ID
						<< "type" << "genderAnalytics"
						<< "parentID"
						<< resultTableId
						<< "labels"
						<< bsoncxx::builder::stream::open_array << blobContainer[i].gender
						<< bsoncxx::builder::stream::close_array
						<< "startTime" << ((double)blobContainer[i].firstFrameNumber)/
						                  ((double)this->procFPS)
						<< "endTime"   << ((double)blobContainer[i].lastFrameNumber)/
						                  ((double)this->procFPS)
						<< "rectangles"
						<< bsoncxx::builder::stream::open_array
						<< bsoncxx::builder::stream::open_document
						<< "x" << blobContainer[i].lastRectangle.x
						<< "y" << blobContainer[i].lastRectangle.y
						<< "height" << blobContainer[i].lastRectangle.height
						<< "width" << blobContainer[i].lastRectangle.width
						<< "rectTime" << ((double)frameNumber)/
						                 ((double)this->procFPS)
						<< bsoncxx::builder::stream::close_document
						<< bsoncxx::builder::stream::close_array;
				// std::cout<<"After Basic Builder"<<std::endl;
				bsoncxx::document::value basic_doc{basic_builder.extract()};
				auto retUnderId = collectionGender.insert_one(std::move(basic_doc));
				bsoncxx::oid oid = retUnderId->inserted_id().get_oid().value;
				std::string JobID = oid.to_string();

				std::cout << "JOB ID: "<<JobID<<std::endl;

				bsoncxx::builder::stream::document filter_builderResult, update_builderResult;
				try {
					filter_builderResult << "_id" << bsoncxx::oid(resultTableId);
					// std::cout<<bsoncxx::oid(resultTableId)<<std::endl;
				}

				catch (...) {
					std::cout<<"caught"<<std::endl;
				}
				update_builderResult
						<< "$push"
			            << bsoncxx::builder::stream::open_document
						<< "gender"
			            << bsoncxx::oid(JobID)
			            << bsoncxx::builder::stream::close_document;


				auto cursorResults = collectionResults.find(filter_builderResult.view());
				int count=0;
				for (auto&& docRes : cursorResults) {
					// std::cout << bsoncxx::to_json(docRes) << "\n";
				}

				collectionResults.update_one(filter_builderResult.view(), update_builderResult.view());
			}
		}
		else {
			// std::cout<<"In else loop"<<std::endl;
			bsoncxx::builder::stream::document filter_builder, update_builder1, update_builder2, update_builder3;
			if(blobContainer[i].lastFrameNumber == frameNumber) {
				// std::cout<<"in if else"<<std::endl;
				if (blobContainer[i].lastRectangle.area() > 0
                    && blobContainer[i].gender != "unassigned") {
					// std::cout<<"push"<<std::endl;
					filter_builder << "ID"
					               << blobContainer[i].ID
					               << "type"
					               << "genderAnalytics";
					update_builder1 << "$set"
					                << bsoncxx::builder::stream::open_document
					                << "endTime"
					                << ((double)blobContainer[i].lastFrameNumber)/
					                   ((double)this->procFPS)
					                << bsoncxx::builder::stream::close_document;
					update_builder2
							<< "$push"
							<< bsoncxx::builder::stream::open_document
							<< "rectangles"
							<< bsoncxx::builder::stream::open_document
							<< "x" << blobContainer[i].lastRectangle.x
							<< "y" << blobContainer[i].lastRectangle.y
							<< "height" << blobContainer[i].lastRectangle.height
							<< "width" << blobContainer[i].lastRectangle.width
							<< "rectTime" << ((double)frameNumber)/
							                 ((double)this->procFPS)
							<< bsoncxx::builder::stream::close_document
							<< bsoncxx::builder::stream::close_document ;

					update_builder3
							<< "$push"
							<< bsoncxx::builder::stream::open_document
							<< "labels"
					        << blobContainer[i].gender
							<< bsoncxx::builder::stream::close_document;

					collectionGender.update_one(filter_builder.view(), update_builder1.view());
					collectionGender.update_one(filter_builder.view(), update_builder2.view());
					collectionGender.update_one(filter_builder.view(), update_builder3.view());
				}
			}
		}
	}
}

void mongoLink::humanAnalyticsMONGO(
    std::vector<aiSaac::HumanBlob> blobContainer,
    int frameNumber,
    std::string resultTableId) {
	// std::cout<<"Entered Human Analytics Mongo"<<std::endl;
	for (int i = 0; i < blobContainer.size(); i++) {
		// std::cout<<"Insie Loop"<<std::endl;
		bool foundFlag=false;
		bsoncxx::builder::stream::document init_filter;
		init_filter << "ID"
		            << blobContainer[i].ID
					<< "type"
					<< "humanAnalytics"
					<< "parentID"
					<< resultTableId;

		// std::cout<<"Init Filter"<<std::endl;
		auto cursor = collectionHuman.find(init_filter.view());
		int count=0;
		for (auto&& doc : cursor) {
			count+=1;
			if(count>0) {
				count=0;
				foundFlag=true;
				break;
			}
		}
		if(foundFlag==false) {
			// std::cout<<"Not Found"<<std::endl;
			if (blobContainer[i].lastRectangle.area() > 0) {
				// codeReview(Pratik): Add a dedicated metaData field in humanBlob.h and make corresponding changes in humanAnalytics, mongoLink and serialLink.
				
                std::string metaData = "person " + blobContainer[i].shirtColor;
				// std::cout<<"IN loop 2"<<std::endl;
				bsoncxx::builder::stream::document basic_builder{};
				basic_builder
						<< "ID" << blobContainer[i].ID
						<< "type" << "humanAnalytics"
						<< "parentID"
						<< resultTableId
						<< "shirtColor"
						<< bsoncxx::builder::stream::open_array << blobContainer[i].shirtColor
						<< bsoncxx::builder::stream::close_array
                        << "metaData"
						<< bsoncxx::builder::stream::open_array << metaData
						<< bsoncxx::builder::stream::close_array
						<< "startTime" << ((double)blobContainer[i].firstFrameNumber)/
						                  ((double)this->procFPS)
						<< "endTime"   << ((double)blobContainer[i].lastFrameNumber)/
						                  ((double)this->procFPS)
                        << "rectangles"
						<< bsoncxx::builder::stream::open_array
						<< bsoncxx::builder::stream::open_document
						<< "x" << blobContainer[i].lastRectangle.x
						<< "y" << blobContainer[i].lastRectangle.y
						<< "height" << blobContainer[i].lastRectangle.height
						<< "width" << blobContainer[i].lastRectangle.width
						<< "rectTime" << ((double)frameNumber)/
						                 ((double)this->procFPS)
						<< bsoncxx::builder::stream::close_document
						<< bsoncxx::builder::stream::close_array;
				// std::cout<<"After Basic Builder"<<std::endl;
				bsoncxx::document::value basic_doc{basic_builder.extract()};
				auto retUnderId = collectionHuman.insert_one(std::move(basic_doc));
				bsoncxx::oid oid = retUnderId->inserted_id().get_oid().value;
				std::string JobID = oid.to_string();

				// std::cout << "JOB ID: "<<JobID<<std::endl;

				bsoncxx::builder::stream::document filter_builderResult, update_builderResult;
				try {
					filter_builderResult << "_id" << bsoncxx::oid(resultTableId);
					// std::cout<<bsoncxx::oid(resultTableId)<<std::endl;
				}

				catch (...) {
					std::cout<<"caught"<<std::endl;
				}
				update_builderResult
						<< "$push"
			            << bsoncxx::builder::stream::open_document
			            << "human"
			            << bsoncxx::oid(JobID)
			            << bsoncxx::builder::stream::close_document;


				auto cursorResults = collectionResults.find(filter_builderResult.view());
				int count=0;
				for (auto&& docRes : cursorResults) {
					// std::cout << bsoncxx::to_json(docRes) << "\n";
				}

				collectionResults.update_one(filter_builderResult.view(), update_builderResult.view());
			}
		}
		else {
			// std::cout<<"In else loop"<<std::endl;
			bsoncxx::builder::stream::document filter_builder,
                update_builder1,
                update_builder2,
                update_builder3,
                update_builder4;
			if(blobContainer[i].lastFrameNumber == frameNumber) {
				// std::cout<<"in if else"<<std::endl;
				if (blobContainer[i].lastRectangle.area() > 0) {
					// std::cout<<"push"<<std::endl;
					filter_builder << "ID"
					               << blobContainer[i].ID
					               << "type"
					               << "humanAnalytics";
					update_builder1 << "$set"
					                << bsoncxx::builder::stream::open_document
					                << "endTime"
					                << ((double)blobContainer[i].lastFrameNumber)/
					                   ((double)this->procFPS)
					                << bsoncxx::builder::stream::close_document;
					update_builder2
							<< "$push"
							<< bsoncxx::builder::stream::open_document
							<< "rectangles"
							<< bsoncxx::builder::stream::open_document
							<< "x" << blobContainer[i].lastRectangle.x
							<< "y" << blobContainer[i].lastRectangle.y
							<< "height" << blobContainer[i].lastRectangle.height
							<< "width" << blobContainer[i].lastRectangle.width
							<< "rectTime" << ((double)frameNumber)/
							                 ((double)this->procFPS)
							<< bsoncxx::builder::stream::close_document
							<< bsoncxx::builder::stream::close_document ;

					update_builder3
							<< "$push"
							<< bsoncxx::builder::stream::open_document
							<< "shirtColor"
					        << blobContainer[i].shirtColor
							<< bsoncxx::builder::stream::close_document;
                    std::string metaData = "person " + blobContainer[i].shirtColor;
                    update_builder4
							<< "$push"
							<< bsoncxx::builder::stream::open_document
							<< "metaData"
					        << metaData
							<< bsoncxx::builder::stream::close_document;

                    collectionHuman.update_one(filter_builder.view(), update_builder1.view());
					collectionHuman.update_one(filter_builder.view(), update_builder2.view());
					collectionHuman.update_one(filter_builder.view(), update_builder3.view());
                    collectionHuman.update_one(filter_builder.view(), update_builder4.view());
				}
			}
		}
	}
}

void mongoLink::objectAnalyticsMONGO(
    std::vector<aiSaac::ObjectBlob> blobContainer,
    int frameNumber,
    std::string resultTableId) {
	// std::cout<<"Entered Object Analytics Mongo"<<std::endl;
	for (int i = 0; i < blobContainer.size(); i++) {
		// std::cout<<"Insie Loop"<<std::endl;
		bool foundFlag=false;
		bsoncxx::builder::stream::document init_filter;
		init_filter << "ID"
		            << blobContainer[i].ID
					<< "type"
					<< "objectAnalytics"
					<< "parentID"
					<< resultTableId;

		// std::cout<<"Init Filter"<<std::endl;
		auto cursor = collectionObject.find(init_filter.view());
		int count=0;
		for (auto&& doc : cursor) {
			count+=1;
			if(count>0) {
				count=0;
				foundFlag=true;
				break;
			}
		}
		if(foundFlag==false) {
			// std::cout<<"Not Found"<<std::endl;
			if (blobContainer[i].lastRectangle.area() > 0) {
				// std::cout<<"IN loop 2"<<std::endl;
				bsoncxx::builder::stream::document basic_builder{};
				basic_builder
						<< "ID" << blobContainer[i].ID
						<< "type" << "objectAnalytics"
						<< "parentID"
						<< resultTableId
						<< "labels"
						<< bsoncxx::builder::stream::open_array << blobContainer[i].label
						<< bsoncxx::builder::stream::close_array
						<< "startTime" << ((double)blobContainer[i].firstFrameNumber)/
						                  ((double)this->procFPS)
						<< "endTime"   << ((double)blobContainer[i].lastFrameNumber)/
						                  ((double)this->procFPS)
                        << "metaData"
                        << bsoncxx::builder::stream::open_array << blobContainer[i].metaData
						<< bsoncxx::builder::stream::close_array
						<< "rectangles"
						<< bsoncxx::builder::stream::open_array
						<< bsoncxx::builder::stream::open_document
						<< "x" << blobContainer[i].lastRectangle.x
						<< "y" << blobContainer[i].lastRectangle.y
						<< "height" << blobContainer[i].lastRectangle.height
						<< "width" << blobContainer[i].lastRectangle.width
						<< "rectTime" << ((double)frameNumber)/
						                 ((double)this->procFPS)
						<< bsoncxx::builder::stream::close_document
						<< bsoncxx::builder::stream::close_array;
				// std::cout<<"After Basic Builder"<<std::endl;
				bsoncxx::document::value basic_doc{basic_builder.extract()};
				auto retUnderId = collectionObject.insert_one(std::move(basic_doc));
				bsoncxx::oid oid = retUnderId->inserted_id().get_oid().value;
				std::string JobID = oid.to_string();

				// std::cout << "JOB ID: "<<JobID<<std::endl;

				bsoncxx::builder::stream::document filter_builderResult, update_builderResult;
				try {
					filter_builderResult << "_id" << bsoncxx::oid(resultTableId);
					// std::cout<<bsoncxx::oid(resultTableId)<<std::endl;
				}

				catch (...) {
					std::cout<<"caught"<<std::endl;
				}
				update_builderResult
						<< "$push"
			            << bsoncxx::builder::stream::open_document
			            << "object"
			            << bsoncxx::oid(JobID)
			            << bsoncxx::builder::stream::close_document;


				auto cursorResults = collectionResults.find(filter_builderResult.view());
				int count=0;
				for (auto&& docRes : cursorResults) {
					// std::cout << bsoncxx::to_json(docRes) << "\n";
				}

				collectionResults.update_one(filter_builderResult.view(), update_builderResult.view());
			}
		}
		else {
			// std::cout<<"In else loop"<<std::endl;
			bsoncxx::builder::stream::document filter_builder,
                update_builder1,
                update_builder2,
                update_builder3,
                update_builder4;
			if(blobContainer[i].lastFrameNumber == frameNumber) {
				// std::cout<<"in if else"<<std::endl;
				if (blobContainer[i].lastRectangle.area() > 0) {
					// std::cout<<"push"<<std::endl;
					filter_builder << "ID"
					               << blobContainer[i].ID
					               << "type"
					               << "objectAnalytics";
					update_builder1 << "$set"
					                << bsoncxx::builder::stream::open_document
					                << "endTime"
					                << ((double)blobContainer[i].lastFrameNumber)/
					                   ((double)this->procFPS)
					                << bsoncxx::builder::stream::close_document;
					update_builder2
							<< "$push"
							<< bsoncxx::builder::stream::open_document
							<< "rectangles"
							<< bsoncxx::builder::stream::open_document
							<< "x" << blobContainer[i].lastRectangle.x
							<< "y" << blobContainer[i].lastRectangle.y
							<< "height" << blobContainer[i].lastRectangle.height
							<< "width" << blobContainer[i].lastRectangle.width
							<< "rectTime" << ((double)frameNumber)/
							                 ((double)this->procFPS)
							<< bsoncxx::builder::stream::close_document
							<< bsoncxx::builder::stream::close_document ;

					update_builder3
							<< "$push"
							<< bsoncxx::builder::stream::open_document
							<< "labels"
					        << blobContainer[i].label
							<< bsoncxx::builder::stream::close_document;

                    update_builder4
							<< "$push"
							<< bsoncxx::builder::stream::open_document
							<< "metaData"
					        << blobContainer[i].metaData
							<< bsoncxx::builder::stream::close_document;

					collectionObject.update_one(filter_builder.view(), update_builder1.view());
					collectionObject.update_one(filter_builder.view(), update_builder2.view());
					collectionObject.update_one(filter_builder.view(), update_builder3.view());
                    collectionObject.update_one(filter_builder.view(), update_builder4.view());
				}
			}
		}
	}
}

void mongoLink::sceneAnalyticsMONGO(std::vector<aiSaac::SceneBlob> blobContainer,
    int frameNumber,
    std::string resultTableId) {
	// std::cout<<"Entered Scene Analytics Mongo"<<std::endl;
	for (int i = 0; i < blobContainer.size(); i++) {
		// std::cout<<"Insie Loop"<<std::endl;
		bool foundFlag=false;
		bsoncxx::builder::stream::document init_filter;
		init_filter << "ID"
		            << blobContainer[i].ID
					<< "type"
					<< "sceneAnalytics"
					<< "parentID"
					<< resultTableId;

		// std::cout<<"Init Filter"<<std::endl;
		auto cursor = collectionScene.find(init_filter.view());
		int count=0;
		for (auto&& doc : cursor) {
			count+=1;
			if(count>0) {
				count=0;
				foundFlag=true;
				break;
			}
		}
		if(foundFlag==false) {
			// std::cout<<"Not Found"<<std::endl;
			// std::cout<<"IN loop 2"<<std::endl;
			bsoncxx::builder::stream::document basic_builder{};
			basic_builder
					<< "ID" << blobContainer[i].ID
					<< "type" << "sceneAnalytics"
					<< "parentID"
					<< resultTableId
					<< "labels"
					<< bsoncxx::builder::stream::open_array << blobContainer[i].label
					<< bsoncxx::builder::stream::close_array
					<< "startTime" << ((double)blobContainer[i].firstFrameNumber)/
					                  ((double)this->procFPS)
					<< "endTime"   << ((double)blobContainer[i].lastFrameNumber)/
					                  ((double)this->procFPS);

			// std::cout<<"After Basic Builder"<<std::endl;
			bsoncxx::document::value basic_doc{basic_builder.extract()};
			auto retUnderId = collectionScene.insert_one(std::move(basic_doc));
			bsoncxx::oid oid = retUnderId->inserted_id().get_oid().value;
			std::string JobID = oid.to_string();

			// std::cout << "JOB ID: "<<JobID<<std::endl;

			bsoncxx::builder::stream::document filter_builderResult, update_builderResult;
			try {
				filter_builderResult << "_id" << bsoncxx::oid(resultTableId);
				// std::cout<<bsoncxx::oid(resultTableId)<<std::endl;
			}

			catch (...) {
				std::cout<<"caught"<<std::endl;
			}
			update_builderResult
					<< "$push"
		            << bsoncxx::builder::stream::open_document
		            << "scene"
		            << bsoncxx::oid(JobID)
		            << bsoncxx::builder::stream::close_document;


			auto cursorResults = collectionResults.find(filter_builderResult.view());
			int count=0;
			for (auto&& docRes : cursorResults) {
				// std::cout << bsoncxx::to_json(docRes) << "\n";
			}

			collectionResults.update_one(filter_builderResult.view(), update_builderResult.view());
		}
		else {
			// std::cout<<"In else loop"<<std::endl;
			bsoncxx::builder::stream::document filter_builder, update_builder1, update_builder2;
			if(blobContainer[i].lastFrameNumber == frameNumber) {
				// std::cout<<"in if else"<<std::endl;
				// std::cout<<"push"<<std::endl;
				filter_builder << "ID"
				               << blobContainer[i].ID
				               << "type"
				               << "sceneAnalytics";
				update_builder1 << "$set"
				                << bsoncxx::builder::stream::open_document
				                << "endTime"
				                << ((double)blobContainer[i].lastFrameNumber)/
				                   ((double)this->procFPS)
				                << bsoncxx::builder::stream::close_document;
				update_builder2
						<< "$push"
						<< bsoncxx::builder::stream::open_document
						<< "labels"
				        << blobContainer[i].label
						<< bsoncxx::builder::stream::close_document;

				collectionScene.update_one(filter_builder.view(), update_builder1.view());
				collectionScene.update_one(filter_builder.view(), update_builder2.view());
			}
		}
	}
}

void mongoLink::textAnalyticsMONGO(
    std::vector<aiSaac::TextBlob> blobContainer,
    int frameNumber,
    std::string resultTableId) {
	// std::cout<<"Entered Object Analytics Mongo"<<std::endl;
	for (int i = 0; i < blobContainer.size(); i++) {
		// std::cout<<"Insie Loop"<<std::endl;
		bool foundFlag=false;
		bsoncxx::builder::stream::document init_filter;
		init_filter << "ID"
		            << blobContainer[i].ID
					<< "type"
					<< "textAnalytics"
					<< "parentID"
					<< resultTableId;

		// std::cout<<"Init Filter"<<std::endl;
		auto cursor = collectionText.find(init_filter.view());
		int count=0;
		for (auto&& doc : cursor) {
			count+=1;
			if(count>0) {
				count=0;
				foundFlag=true;
				break;
			}
		}
		if(foundFlag==false) {
			// std::cout<<"Not Found"<<std::endl;
            if (blobContainer[i].lastRectangle.area() > 0) {
                // std::cout<<"IN loop 2"<<std::endl;
                bsoncxx::builder::stream::document basic_builder{};
                basic_builder
                        << "ID" << blobContainer[i].ID
                        << "type" << "textAnalytics"
                        << "parentID"
                        << resultTableId
                        << "labels"
                        << bsoncxx::builder::stream::open_array << blobContainer[i].text
                        << bsoncxx::builder::stream::close_array
                        << "startTime" << ((double)blobContainer[i].firstFrameNumber)/
                                          ((double)this->procFPS)
                        << "endTime"   << ((double)blobContainer[i].lastFrameNumber)/
                                          ((double)this->procFPS)
                        << "rectangles"
                        << bsoncxx::builder::stream::open_array
                        << bsoncxx::builder::stream::open_document
                        << "x" << blobContainer[i].lastRectangle.x
                        << "y" << blobContainer[i].lastRectangle.y
                        << "height" << blobContainer[i].lastRectangle.height
                        << "width" << blobContainer[i].lastRectangle.width
                        << "rectTime" << ((double)frameNumber)/
                                         ((double)this->procFPS)
                        << bsoncxx::builder::stream::close_document
                        << bsoncxx::builder::stream::close_array;
                // std::cout<<"After Basic Builder"<<std::endl;
                bsoncxx::document::value basic_doc{basic_builder.extract()};
                auto retUnderId = collectionText.insert_one(std::move(basic_doc));
                bsoncxx::oid oid = retUnderId->inserted_id().get_oid().value;
                std::string JobID = oid.to_string();

                // std::cout << "JOB ID: "<<JobID<<std::endl;

                bsoncxx::builder::stream::document filter_builderResult, update_builderResult;
                try {
                    filter_builderResult << "_id" << bsoncxx::oid(resultTableId);
                    // std::cout<<bsoncxx::oid(resultTableId)<<std::endl;
                }

                catch (...) {
                    std::cout<<"caught"<<std::endl;
                }
                update_builderResult
                        << "$push"
                        << bsoncxx::builder::stream::open_document
                        << "labels"
                        << bsoncxx::oid(JobID)
                        << bsoncxx::builder::stream::close_document;


                auto cursorResults = collectionResults.find(filter_builderResult.view());
                int count=0;
                for (auto&& docRes : cursorResults) {
                    // std::cout << bsoncxx::to_json(docRes) << "\n";
                }

                collectionResults.update_one(filter_builderResult.view(), update_builderResult.view());
            }
		}
		else {
			// std::cout<<"In else loop"<<std::endl;
			bsoncxx::builder::stream::document filter_builder, update_builder1, update_builder2, update_builder3;
			if(blobContainer[i].lastFrameNumber == frameNumber) {
				// std::cout<<"in if else"<<std::endl;
                if (blobContainer[i].lastRectangle.area() > 0) {
                    // std::cout<<"push"<<std::endl;
                    filter_builder << "ID"
                                   << blobContainer[i].ID
                                   << "type"
                                   << "textAnalytics";
                    update_builder1 << "$set"
                                    << bsoncxx::builder::stream::open_document
                                    << "endTime"
                                    << ((double)blobContainer[i].lastFrameNumber)/
                                       ((double)this->procFPS)
                                    << bsoncxx::builder::stream::close_document;
                    update_builder2
                            << "$push"
                            << bsoncxx::builder::stream::open_document
                            << "rectangles"
                            << bsoncxx::builder::stream::open_document
                            << "x" << blobContainer[i].lastRectangle.x
                            << "y" << blobContainer[i].lastRectangle.y
                            << "height" << blobContainer[i].lastRectangle.height
                            << "width" << blobContainer[i].lastRectangle.width
                            << "rectTime" << ((double)frameNumber)/
                                             ((double)this->procFPS)
                            << bsoncxx::builder::stream::close_document
                            << bsoncxx::builder::stream::close_document ;

                    update_builder3
                            << "$push"
                            << bsoncxx::builder::stream::open_document
                            << "labels"
                            << blobContainer[i].text
                            << bsoncxx::builder::stream::close_document;

                    collectionText.update_one(filter_builder.view(), update_builder1.view());
                    collectionText.update_one(filter_builder.view(), update_builder2.view());
                    collectionText.update_one(filter_builder.view(), update_builder3.view());
                }
			}
		}
	}
}

void mongoLink::summarizationMONGO(
    std::vector<int> orderedKeyPoints,
    aiSaac::AiSaacSettings *aiSaacSettings,
    std::string resultTableId) {
	// std::cout << "inside summarization mongo" << std::endl;
	for (int i = 0; i < orderedKeyPoints.size(); i++) {
		// std::cout<<"Insie Loop"<<std::endl;
		bsoncxx::builder::stream::document init_filter;
		init_filter << "ID"
		            << i
					<< "type"
					<< "summarization"
					<< "parentID"
					<< resultTableId;

		int startTime = orderedKeyPoints[i] * aiSaacSettings->getSummarizationSNPSize();
		int endTime = startTime + aiSaacSettings->getSummarizationSNPSize();
		// std::cout << "building bscon" << std::endl;
		bsoncxx::builder::stream::document basic_builder{};
		basic_builder
				<< "type" << "summarization"
				<< "parentID" << resultTableId
				<< "startTime" << startTime
				<< "endTime" << endTime;

		// std::cout<<"After Basic Builder"<<std::endl;
		bsoncxx::document::value basic_doc{basic_builder.extract()};
		auto retUnderId = collectionSummary.insert_one(std::move(basic_doc));
		bsoncxx::oid oid = retUnderId->inserted_id().get_oid().value;
		std::string JobID = oid.to_string();

		// std::cout << "JOB ID: "<<JobID<<std::endl;

		// std::cout << "before try block" << std::endl;
		bsoncxx::builder::stream::document filter_builderResult, update_builderResult;
		try {
			filter_builderResult << "_id" << bsoncxx::oid(resultTableId);
			// std::cout<<bsoncxx::oid(resultTableId)<<std::endl;
		}catch (...) {
			std::cout<<"caught"<<std::endl;
		}
		// std::cout << "after catch block" << std::endl;
		update_builderResult
				<< "$push"
	            << bsoncxx::builder::stream::open_document
	            << "summary"
	            << bsoncxx::oid(JobID)
	            << bsoncxx::builder::stream::close_document;

        // std::cout << "before finding in collectionResults" << std::endl;
		auto cursorResults = collectionResults.find(filter_builderResult.view());
		for (auto&& docRes : cursorResults) {
			std::cout << "inside cursor for loop" << std::endl;
			// std::cout << bsoncxx::to_json(docRes) << "\n";
		}
		// std::cout << "after for loop and before update" << std::endl;
		collectionResults.update_one(filter_builderResult.view(), update_builderResult.view());
	}
}

void mongoLink::pushvideoLengthAndFrameSize(std::string videoLength, int height, int width, std::string resultTableId) {
	bsoncxx::builder::stream::document filter_builderResult, update_builderResult1, update_builderResult2;
	try {
		filter_builderResult << "_id" << bsoncxx::oid(resultTableId);
		// std::cout<<bsoncxx::oid(resultTableId)<<std::endl;
	}catch (...) {
		std::cout<<"caught"<<std::endl;
	}
	// std::cout << "after catch block" << std::endl;
	update_builderResult1
		<< "$set"
		<< bsoncxx::builder::stream::open_document
		<<"videoLength" << videoLength
		<< bsoncxx::builder::stream::close_document;

	update_builderResult2
		<< "$set"
		<< bsoncxx::builder::stream::open_document
		<< "frameSize"
			<< bsoncxx::builder::stream::open_document
			<< "height" << height
			<< "width" << width
			<< bsoncxx::builder::stream::close_document
		<< bsoncxx::builder::stream::close_document;

	auto cursorResults = collectionResults.find(filter_builderResult.view());
	for (auto&& docRes : cursorResults) {
		std::cout << "inside cursor for loop" << std::endl;
	}

	collectionResults.update_one(filter_builderResult.view(), update_builderResult1.view());
	collectionResults.update_one(filter_builderResult.view(), update_builderResult2.view());
}
