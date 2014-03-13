/**
 * Code for thinning a binary image using Zhang-Suen algorithm.
 */
#include <com_example_opt_course_ex_OpCourseEx.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

extern "C" {
/**
 * Perform one thinning iteration of Zhang-Suen algorithm
 *
 * @param  im    Binary image with range = 0-1
 * @param  iter  0=even, 1=odd
 */
void thinningIteration(cv::Mat& im, int iter)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);

            int A  = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
                     (p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
                     (p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
                     (p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
            int B  = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
            int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
            int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                marker.at<uchar>(i,j) = 1;
        }
    }

    im &= ~marker;
}

/**
 * Perform one thinning iteration of Guo-Hall algorithm
 *
 * @param  im    Binary image with range = 0-1
 * @param  iter  0=even, 1=odd
 */
void thinningGuoHallIteration(cv::Mat& im, int iter)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows; i++)
    {
        for (int j = 1; j < im.cols; j++)
        {
            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);

            int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
                     (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
            int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
            int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
            int N  = N1 < N2 ? N1 : N2;
            int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

            if (C == 1 && (N >= 2 && N <= 3) & m == 0)
                marker.at<uchar>(i,j) = 1;
        }
    }

    im &= ~marker;
}


/**
 * Function for thinning the given binary image
 *
 * @param  im  Binary image with range = 0-255
 * @param  useGuoHall Use Guo-Hall or Zhang-Suen algorithm
 */
JNIEXPORT double JNICALL Java_com_example_opt_1course_1ex_OpCourseEx_thinning(JNIEnv*, jobject, jlong addrMat,
		jlong useGHull)
{
	double tStart = (double)getTickCount();
	bool useGuoHall = (bool)useGHull;
	cv::Mat& im = *(cv::Mat*)addrMat;
    im /= 255;

    cv::Mat prev = cv::Mat::zeros(im.size(), CV_8UC1);
    cv::Mat diff;

    do {
        if (useGuoHall)
        {
            thinningGuoHallIteration(im, 0);
            thinningGuoHallIteration(im, 1);
        }
        else
        {
            thinningIteration(im, 0);
            thinningIteration(im, 1);
        }
        cv::absdiff(im, prev, diff);
        im.copyTo(prev);
    }
    while (cv::countNonZero(diff) > 0);

    im *= 255;
    return ((double)getTickCount()-tStart)/(double)getTickFrequency();
}

}
/**
 * This is an example on how to call the thinning function above.
 */
//int main()
//{
//    cv::Mat src = cv::imread("test_image.png");
//    if (src.empty())
//        return -1;
//
//    cv::Mat bw;
//    cv::cvtColor(src, bw, CV_BGR2GRAY);
//    cv::threshold(bw, bw, 10, 255, CV_THRESH_BINARY);
//
//    thinning(bw);
//
//    cv::imshow("src", src);
//    cv::imshow("dst", bw);
//    cv::waitKey(0);
//
//    return 0;
//}
