#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
using namespace std;
using namespace cv;
void collectCardsFromImages(vector<Mat>& cardsImages, vector< string>& cardsNames, vector<Mat>& cardsDescriptors, vector<vector<KeyPoint>>& cardsKeypoints) {

	string card_names[] = {"6-krest.jpg", "7-byben.jpg", "8-krest.jpg", "dama-byben.jpg", "korol-winny.jpg", "tys-chervi.jpg"};

for (const auto& name : card_names) {
    Mat card = imread(name);
    resize(card, card, Size(130, 220));
    cardsImages.push_back(card);
    cardsNames.push_back(name);
}
	Ptr<ORB> detector = ORB::create();

	for (int i = 0; i < cardsImages.size(); i++) {
		Mat dis;
		vector<KeyPoint> keys;
		detector->detectAndCompute(cardsImages[i], noArray(), keys, dis);
		cardsKeypoints.push_back(keys);
		cardsDescriptors.push_back(dis);
	}
}

void affineCastImage(RotatedRect& rect, Mat& image, Mat& resoult) {

	Mat rotatedMatrix, rotatedImage;
	rotatedMatrix = getRotationMatrix2D(rect.center, rect.angle, 1.0);
	warpAffine(image, rotatedImage, rotatedMatrix, image.size(), INTER_CUBIC);
	getRectSubPix(rotatedImage, rect.size, rect.center, resoult);

	rotate(resoult, resoult, ROTATE_180);

	if (resoult.size[0] < resoult.size[1]) {
		rotate(resoult, resoult, ROTATE_90_CLOCKWISE);
	}
}

void detectCard(string& cardName, Mat& card, vector<Mat>& cardsImages, vector<string>& cardsNames, vector<Mat>& cardsDescriptors, vector<vector<KeyPoint>>& cardsKeypoints) {

	Mat cardDescriptors;
	vector<KeyPoint> cardKeypoints;
	Ptr<ORB> detector = ORB::create();
	Ptr<BFMatcher> matcher = BFMatcher::create();
	detector->detectAndCompute(card, noArray(), cardKeypoints, cardDescriptors);

	if (cardDescriptors.empty()) {
		cardName = "";
		return;
	}

	int maxI = -1;
	int maxCount = 0;

	for (int i = 0; i < cardsImages.size(); i++) {

		if (cardsDescriptors[i].empty()) {
			continue;
		}

		vector<vector<DMatch>> knn_matches;

		matcher->knnMatch(cardsDescriptors[i], cardDescriptors, knn_matches, 3);

		vector<DMatch> correct;

		for (size_t i = 0; i < knn_matches.size(); i++) {
			if (knn_matches[i][0].distance < 0.75 * knn_matches[i][1].distance) {
				correct.push_back(knn_matches[i][0]);
			}
		}

		if (maxCount < correct.size()) {
			maxCount = static_cast<int>(correct.size());
			maxI = i;
		}
	}

	if (maxI == -1) {
		cardName = "";
	}
	else {
		size_t lastindex = cardsNames[maxI].find_last_of(".");
		cardName = cardsNames[maxI].substr(0, lastindex);
	}
}

static void drawRotateRect(Mat& image, RotatedRect& box) {

	Point2f boxPoints[4];
	box.points(boxPoints);

	for (int j = 0; j < 4; j++) {
		line(image, boxPoints[j], boxPoints[(j + 1) % 4], Scalar(0, 255, 0), 4, LINE_AA);
	}
}

void findCardsOnPhoto(Mat& image, vector<Mat>& cardsImages, vector<string>& cardsNames, vector<Mat>& cardsDescriptors,
	vector<vector<KeyPoint>>& cardsKeypoints) {
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	Mat blur, thresh, canny;
	GaussianBlur(image, blur, Size(15, 15), 3);
	Canny(blur, canny, 130, 130);
	findContours(canny, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	for (const auto& contour : contours) {
		vector<Point> contoursPoly;

		approxPolyDP(contour, contoursPoly, 1, true);
		RotatedRect cardRect = minAreaRect(contoursPoly);

		if (cardRect.size.width < 100 || cardRect.size.height < 100) {
			continue;
		}

		Mat card;
		string cardName;
		affineCastImage(cardRect, image, card);

		detectCard(cardName, card, cardsImages, cardsNames, cardsDescriptors, cardsKeypoints);

		if (cardName != "") {
			drawRotateRect(image, cardRect);
			// �������� ����� ����� �� 20 ��������
			Point textOrigin(cardRect.center.x - 20, cardRect.center.y);
			putText(image, cardName, textOrigin, 1, 2, Scalar(0, 0, 255), 2);

		}
	}
}

int main() {

	vector<Mat> cardsImages;
	vector<string> cardsNames;
	vector<Mat> cardsDescriptors;
	vector<vector<KeyPoint>> cardsKeypoints;

	Mat image = imread("all.jpg");

	collectCardsFromImages(cardsImages, cardsNames, cardsDescriptors, cardsKeypoints);

	findCardsOnPhoto(image, cardsImages, cardsNames, cardsDescriptors, cardsKeypoints);

	imshow("Res", image);
	waitKey(0);

	imwrite("result.jpg", image);

	return 0;
}