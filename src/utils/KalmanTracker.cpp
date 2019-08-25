///////////////////////////////////////////////////////////////////////////////
// KalmanTracker.cpp: KalmanTracker Class Implementation Declaration

#include "KalmanTracker.h"


int KalmanTracker::kf_count = 0;


// initialize Kalman filter
void KalmanTracker::init_kf(StateType stateMat)
{
	std::cout << "init_kf KalmanTracker" << std::endl << std::flush;
	int stateNum = 7;
	int measureNum = 4;
	kf = KalmanFilter(stateNum, measureNum, 0);

	measurement = Mat::zeros(measureNum, 1, CV_32F);

	/*float the_matrix[stateNum][stateNum] = {{1, 0, 0, 0, 1, 0, 0},
																					{0, 1, 0, 0, 0, 1, 0},
																					{0, 0, 1, 0, 0, 0, 1},
																					{0, 0, 0, 1, 0, 0, 0},
																					{0, 0, 0, 0, 1, 0, 0},
																					{0, 0, 0, 0, 0, 1, 0},
																					{0, 0, 0, 0, 0, 0, 1}};
	kf.transitionMatrix = Mat(stateNum, stateNum, CV_32F, the_matrix);*/

	kf.transitionMatrix = (Mat_<float>(stateNum, stateNum) <<
		1, 0, 0, 0, 1, 0, 0,
		0, 1, 0, 0, 0, 1, 0,
		0, 0, 1, 0, 0, 0, 1,
		0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 0, 0, 1);

	setIdentity(kf.measurementMatrix);
	setIdentity(kf.processNoiseCov, Scalar::all(1e-2));
	setIdentity(kf.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(kf.errorCovPost, Scalar::all(1));

	// initialize state vector with bounding box in [cx,cy,s,r] style
	kf.statePost.at<float>(0, 0) = stateMat.x + stateMat.width / 2;
	std::cout << "statePost[0,0] = " << kf.statePost.at<float>(0, 0) << std::endl << std::flush;
	kf.statePost.at<float>(1, 0) = stateMat.y + stateMat.height / 2;
	std::cout << "statePost[1,0] = " << kf.statePost.at<float>(1, 0) << std::endl << std::flush;
	kf.statePost.at<float>(2, 0) = stateMat.area();
	std::cout << "statePost[2,0] = " << kf.statePost.at<float>(2, 0) << std::endl << std::flush;
	kf.statePost.at<float>(3, 0) = stateMat.width / stateMat.height;
	std::cout << "statePost[3,0] = " << kf.statePost.at<float>(3, 0) << std::endl << std::flush;

	std::cout << "------statePost-----" << std::endl << std::flush;
	std::cout << "statePost.rows:" << kf.statePost.rows << " statePost.cols:" << kf.statePost.cols << std::endl << std::flush;
	for (int i=0; i<kf.statePost.rows; i++) {
		for (int j=0; j<kf.statePost.cols; j++) {
			std::cout << kf.statePost.at<float>(i,j) << " ";
		}
		std::cout << std::endl << std::flush;
	}
	std::cout << "---------------------" << std::endl << std::flush;

}


// Predict the estimated bounding box.
StateType KalmanTracker::predict()
{
	std::cout << "kalman predict Matrix:" << std::endl << std::flush;
	// predict
	Mat p = kf.predict();
	int r = p.rows;
	int c = p.cols;
	for (int i=0; i<r; i++) {
		for (int j=0; j<c; j++) {
			std::cout << p.at<float>(i,j) << " ";
		}
		std::cout << std::endl << std::flush;
	}
	m_age += 1;

	if (m_time_since_update > 0)
		m_hit_streak = 0;
	m_time_since_update += 1;

	StateType predictBox = get_rect_xysr(p.at<float>(0, 0), p.at<float>(1, 0), p.at<float>(2, 0), p.at<float>(3, 0));

	m_history.push_back(predictBox);
	return m_history.back();
}


// Update the state vector with observed bounding box.
void KalmanTracker::update(StateType stateMat)
{
	m_time_since_update = 0;
	m_history.clear();
	m_hits += 1;
	m_hit_streak += 1;

	// measurement
	measurement.at<float>(0, 0) = stateMat.x + stateMat.width / 2;
	measurement.at<float>(1, 0) = stateMat.y + stateMat.height / 2;
	measurement.at<float>(2, 0) = stateMat.area();
	measurement.at<float>(3, 0) = stateMat.width / stateMat.height;

	// update
	kf.correct(measurement);
}


// Return the current state vector
StateType KalmanTracker::get_state()
{
	Mat s = kf.statePost;
	return get_rect_xysr(s.at<float>(0, 0), s.at<float>(1, 0), s.at<float>(2, 0), s.at<float>(3, 0));
}


// Convert bounding box from [cx,cy,s,r] to [x,y,w,h] style.
StateType KalmanTracker::get_rect_xysr(float cx, float cy, float s, float r)
{
	float w = sqrt(s * r);
	float h = s / w;
	float x = (cx - w / 2);
	float y = (cy - h / 2);

	if (x < 0 && cx > 0)
		x = 0;
	if (y < 0 && cy > 0)
		y = 0;

	std::cout << "in get rect" << std::endl << std::flush;
	std::cout << "x:" << x << " y:" << y << " w:" << w << " h:" << h << std::endl << std::flush;
	return StateType(x, y, w, h);
}



/*
// --------------------------------------------------------------------
// Kalman Filter Demonstrating, a 2-d ball demo
// --------------------------------------------------------------------

const int winHeight = 600;
const int winWidth = 800;

Point mousePosition = Point(winWidth >> 1, winHeight >> 1);

// mouse event callback
void mouseEvent(int event, int x, int y, int flags, void *param)
{
	if (event == CV_EVENT_MOUSEMOVE) {
		mousePosition = Point(x, y);
	}
}

void TestKF();

void main()
{
	TestKF();
}


void TestKF()
{
	int stateNum = 4;
	int measureNum = 2;
	KalmanFilter kf = KalmanFilter(stateNum, measureNum, 0);

	// initialization
	Mat processNoise(stateNum, 1, CV_32F);
	Mat measurement = Mat::zeros(measureNum, 1, CV_32F);

	kf.transitionMatrix = *(Mat_<float>(stateNum, stateNum) <<
		1, 0, 1, 0,
		0, 1, 0, 1,
		0, 0, 1, 0,
		0, 0, 0, 1);

	setIdentity(kf.measurementMatrix);
	setIdentity(kf.processNoiseCov, Scalar::all(1e-2));
	setIdentity(kf.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(kf.errorCovPost, Scalar::all(1));

	randn(kf.statePost, Scalar::all(0), Scalar::all(winHeight));

	namedWindow("Kalman");
	setMouseCallback("Kalman", mouseEvent);
	Mat img(winHeight, winWidth, CV_8UC3);

	while (1)
	{
		// predict
		Mat prediction = kf.predict();
		Point predictPt = Point(prediction.at<float>(0, 0), prediction.at<float>(1, 0));

		// generate measurement
		Point statePt = mousePosition;
		measurement.at<float>(0, 0) = statePt.x;
		measurement.at<float>(1, 0) = statePt.y;

		// update
		kf.correct(measurement);

		// visualization
		img.setTo(Scalar(255, 255, 255));
		circle(img, predictPt, 8, CV_RGB(0, 255, 0), -1); // predicted point as green
		circle(img, statePt, 8, CV_RGB(255, 0, 0), -1); // current position as red

		imshow("Kalman", img);
		char code = (char)waitKey(100);
		if (code == 27 || code == 'q' || code == 'Q')
			break;
	}
	destroyWindow("Kalman");
}
*/
