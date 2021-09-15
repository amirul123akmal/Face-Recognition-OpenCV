#include <vector>
#include <opencv2/opencv.hpp>
#include <future>

#pragma warning(disable: 4305)

void process(std::vector<cv::Rect> *datas, cv::Mat face, cv::Size sizing, cv::Mat frame, float threshold, float k, cv::Size offset)
{
	std::vector<cv::Rect> datas{};
	cv::Mat result;
	cv::Point pt1, pt2;
	cv::resize(face, face, sizing);
	cv::matchTemplate(frame, face, result, cv::TM_CCOEFF_NORMED);
	for (int i = 0; i < result.cols; i++)
	{
		for (int j = 0; j < result.rows; j++)
		{
			if (result.at<float>(j, i) >= threshold) {
				pt1.x = i; pt1.y = j;
				pt2.x = i + (offset.width * k); pt2.y = j + (offset.height * k);
				datas->push_back(cv::Rect(pt1, pt2));
				//cv::rectangle(frame, pt1, pt2, cv::Scalar(0, 255, 255), 2);
				break;
			}
		}
	}
}

int main()
{
	cv::Mat frame;
	cv::Mat face = cv::imread("res/2.jpg", cv::IMREAD_UNCHANGED);

	float threshold = 0.65;
	float size_threshold = 0.7;
	int ori_size_x = face.size[0];
	int ori_size_h = face.size[1];

	std::vector<cv::Rect> datas{};

	cv::VideoCapture camera(0);
	cv::Size sizing;
	cv::Size offset(228, 228);

	while (1)
	{
		datas.clear();
		camera.read(frame);
		for (float k = size_threshold; k < 1; k += 0.1)
		{
			sizing.width = ori_size_x * k;
			sizing.height = ori_size_h * k;
			std::async(std::launch::async, process, &datas, face, sizing, frame, threshold, size_threshold, offset);
		}

		cv::groupRectangles(datas, 1, 0.2);
		for (cv::Rect collected : datas)
		{
			cv::rectangle(frame, collected, cv::Scalar(0, 255, 255), 2);
		}

		cv::imshow("Hello", frame);
		if (cv::waitKey(5) >= 0)
		{
			break;
		}
	}
}