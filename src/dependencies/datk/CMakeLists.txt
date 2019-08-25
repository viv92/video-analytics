	cmake_minimum_required(VERSION 2.8)
project("DATK")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4 -std=c++0x")

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fopenmp")
    set(CMAKE_EXE_LINKER_FLAGS "-s")
endif()

# build libDATK.a
add_library(datk
# optimization/contFunctions
	src/optimization/contFunctions/ContinuousFunctions.cc
	src/optimization/contFunctions/L2LogisticLoss.cc
	src/optimization/contFunctions/L2SmoothSVMLoss.cc
	src/optimization/contFunctions/L2SmoothSVRLoss.cc
	src/optimization/contFunctions/L2LeastSquaresLoss.cc
	src/optimization/contFunctions/L2ProbitLoss.cc
	src/optimization/contFunctions/L2HuberSVMLoss.cc
	src/optimization/contFunctions/L2HingeSVMLoss.cc
	src/optimization/contFunctions/L2HingeSVRLoss.cc
	src/optimization/contFunctions/L1LogisticLoss.cc
	src/optimization/contFunctions/L1SmoothSVMLoss.cc
	src/optimization/contFunctions/L1LeastSquaresLoss.cc
	src/optimization/contFunctions/L1ProbitLoss.cc
	src/optimization/contFunctions/L1HuberSVMLoss.cc
# optimization/contAlgorithms
	src/optimization/contAlgorithms/gd.cc
	src/optimization/contAlgorithms/gdLineSearch.cc
	src/optimization/contAlgorithms/gdBarzilaiBorwein.cc
	src/optimization/contAlgorithms/gdNesterov.cc
	src/optimization/contAlgorithms/cg.cc
	src/optimization/contAlgorithms/lbfgsMin.cc
	src/optimization/contAlgorithms/lbfgsMinOwl.cc
	src/optimization/contAlgorithms/tron.cc
	src/optimization/contAlgorithms/sgd.cc
	src/optimization/contAlgorithms/sgdDecayingLearningRate.cc
	src/optimization/contAlgorithms/sgdLineSearch.cc
	src/optimization/contAlgorithms/sgdAdagrad.cc
	src/optimization/contAlgorithms/sgdRegularizedDualAveragingAdagrad.cc
	src/optimization/contAlgorithms/sgdRegularizedDualAveraging.cc
	src/optimization/contAlgorithms/sgdStochasticAverageGradient.cc
# specialized optimization/contAlgorithms
	src/optimization/contAlgorithms/specialized/L1LRPrimal.cc
	src/optimization/contAlgorithms/specialized/SVCDual.cc
	src/optimization/contAlgorithms/specialized/SVRDual.cc
# optimization/discFunctions
	src/optimization/discFunctions/SetFunctions.cc
	src/optimization/discFunctions/SubmodularFunctions.cc
	src/optimization/discFunctions/FeatureBasedFunctions.cc
	src/optimization/discFunctions/FeatureBasedFunctionsDense.cc
	src/optimization/discFunctions/FacilityLocation.cc
	src/optimization/discFunctions/FacilityLocationSparse.cc
	src/optimization/discFunctions/SaturateCoverage.cc
	src/optimization/discFunctions/GraphCutFunctions.cc
	src/optimization/discFunctions/ModularFunctions.cc
	src/optimization/discFunctions/DisparitySum.cc
	src/optimization/discFunctions/DisparityMin.cc
	src/optimization/discFunctions/MMR.cc
	src/optimization/discFunctions/ProbabilisticSetCover.cc
	src/optimization/discFunctions/SetCover.cc
# optimization/discAlgorithms
	src/optimization/discAlgorithms/increment.cc
	src/optimization/discAlgorithms/bidirGreedyMax.cc
	src/optimization/discAlgorithms/lazyGreedyMax.cc
  src/optimization/discAlgorithms/lazyGreedyMaxOrdering.cc
	src/optimization/discAlgorithms/lazyGreedyMaxKnapsack.cc
	src/optimization/discAlgorithms/lazyGreedySC.cc
	src/optimization/discAlgorithms/revlazyGreedyMax.cc
	src/optimization/discAlgorithms/naiveGreedyMax.cc
  src/optimization/discAlgorithms/naiveGreedyMaxOrdering.cc
	src/optimization/discAlgorithms/streamGreedy.cc
	src/optimization/discAlgorithms/localSearchMax.cc
	src/optimization/discAlgorithms/dataPartitionGreedyUniform.cc
	src/optimization/discAlgorithms/distributedGreedy.cc
	src/optimization/discAlgorithms/lazierThanLazy.cc
	src/optimization/discAlgorithms/sieveStreamGreed.cc
 	src/optimization/discAlgorithms/secretaryGreedy.cc
	src/optimization/discAlgorithms/mminCC.cc
	src/optimization/discAlgorithms/mminSCSC.cc
	src/optimization/discAlgorithms/mminSCSK.cc

# classifiers
	src/machinelearning/Classifiers.cc
	src/machinelearning/logisticRegression/L1LogisticRegression.cc
	src/machinelearning/logisticRegression/L2LogisticRegression.cc
	src/machinelearning/SVM/L1SmoothSVM.cc
	src/machinelearning/SVM/L2SmoothSVM.cc
	src/machinelearning/SVM/L2HingeSVM.cc

# data representation
	src/representation/Matrix.cc
	src/representation/Set.cc
	src/representation/VectorOperations.cc
	src/representation/MatrixOperations.cc
	src/representation/SparseFeature.cc
	src/representation/DenseFeature.cc
	src/representation/FileIO.cc
#utils
	src/utils/arguments.cc
	src/utils/error.cc
	)

######################################### TESTS ########################################
add_executable(TestL2LogisticLoss test/TestL2LogisticLoss.cc)
target_link_libraries(TestL2LogisticLoss datk)

add_executable(TestL2LogisticLossArgs test/TestL2LogisticLossArgs.cc)
target_link_libraries(TestL2LogisticLossArgs datk)

add_executable(TestL1LogisticLoss test/TestL1LogisticLoss.cc)
target_link_libraries(TestL1LogisticLoss datk)

add_executable(TestL2LeastSquaresLoss test/TestL2LeastSquaresLoss.cc)
target_link_libraries(TestL2LeastSquaresLoss datk)

add_executable(TestL1LeastSquaresLoss test/TestL1LeastSquaresLoss.cc)
target_link_libraries(TestL1LeastSquaresLoss datk)

add_executable(TestL2HuberSVMLoss test/TestL2HuberSVMLoss.cc)
target_link_libraries(TestL2HuberSVMLoss datk)

add_executable(TestL1HuberSVMLoss test/TestL1HuberSVMLoss.cc)
target_link_libraries(TestL1HuberSVMLoss datk)

add_executable(TestL2ProbitLoss test/TestL2ProbitLoss.cc)
target_link_libraries(TestL2ProbitLoss datk)

add_executable(TestL1ProbitLoss test/TestL1ProbitLoss.cc)
target_link_libraries(TestL1ProbitLoss datk)

add_executable(TestL2SmoothSVMLoss test/TestL2SmoothSVMLoss.cc)
target_link_libraries(TestL2SmoothSVMLoss datk)

add_executable(TestL2SmoothSVRLoss test/TestL2SmoothSVRLoss.cc)
target_link_libraries(TestL2SmoothSVRLoss datk)

add_executable(TestL1SmoothSVMLoss test/TestL1SmoothSVMLoss.cc)
target_link_libraries(TestL1SmoothSVMLoss datk)

add_executable(TestL2HingeSVMLoss test/TestL2HingeSVMLoss.cc)
target_link_libraries(TestL2HingeSVMLoss datk)

add_executable(TestL2HingeSVRLoss test/TestL2HingeSVRLoss.cc)
target_link_libraries(TestL2HingeSVRLoss datk)

add_executable(TestSubmodularFunctions test/TestSubmodularFunctions.cc)
target_link_libraries(TestSubmodularFunctions datk)

add_executable(TestClassification test/TestClassification.cc)
target_link_libraries(TestClassification datk)

add_executable(TestClassificationLibSVM test/TestClassificationLibSVM.cc)
target_link_libraries(TestClassificationLibSVM datk)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "DATK: The Data Analytics ToolKit")
SET(CPACK_PACKAGE_VENDOR "Summar Inc.")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
IF(WIN32 AND NOT UNIX)
  SET(CPACK_NSIS_HELP_LINK "http:\\www.summar.com/SMTK")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\www.summar.com")
  SET(CPACK_NSIS_CONTACT "rkiyer@uw.edu")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)

# 	src/optimization/contFunctions/SumContinuousFunctions.cc
#	src/optimization/contFunctions/SimpleFunctions.cc
#	src/optimization/contFunctions/LogisticLoss.cc
#	src/optimization/contFunctions/SmoothSVMLoss.cc
#	src/optimization/contFunctions/SmoothSVRLoss.cc
#	src/optimization/contFunctions/LeastSquaresLoss.cc
#	src/optimization/contFunctions/ProbitLoss.cc
#	src/optimization/contFunctions/HuberSVMLoss.cc
#	src/optimization/contFunctions/HingeSVMLoss.cc
#	src/optimization/contFunctions/HingeSVRLoss.cc
