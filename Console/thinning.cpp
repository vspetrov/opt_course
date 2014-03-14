/**
 * Code for thinning a binary image using Zhang-Suen algorithm.
 */
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
using namespace std;
using namespace cv;

enum{
    THINNING_ALG_DEFAULT,
    THINNING_ALG_GUO_HALL,
    THINNING_ALG_OPTIMIZED
};

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

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0){
            	marker.at<uchar>(i,j)=1;
            }
        }
    }

    im &= ~marker;
}

bool thinningIterationOpt(cv::Mat& im, int iter)
{
    int step = im.step1();
    bool anyChanges = false;
    uchar p2,p3,p4,p5,p6,p7,p8,p9;
    uchar A, B, m1, m2;
    uchar *r;
    assert(im.isContinuous());
    for (int i = 1; i < im.rows-1; i++)
    {
    	r = im.ptr(i-1);
        for (int j = 1; j < im.cols-1; j++)
        {
            if (r[j+step] == 0) {
                goto prev_check;
            }
            p2 = r[j];//im.at<uchar>(i-1, j);
            p3 = r[j+1];//im.at<uchar>(i-1, j+1);
            p9 = r[j-1];//im.at<uchar>(i-1, j-1);

            p4 = r[step+j+1];//.at<uchar>(i, j+1);
            p8 = r[step+j-1];//im.at<uchar>(i, j-1);

            p5 = r[2*step+j+1];//im.at<uchar>(i+1, j+1);
            p6 = r[2*step+j];//im.at<uchar>(i+1, j);
            p7 = r[2*step+j-1];//im.at<uchar>(i+1, j-1);

            m1 = iter == 0 ? (p2 & p4 & p6) : (p2 & p4 & p8);
            if (m1 > 0) goto prev_check;
            m2 = iter == 0 ? (p4 & p6 & p8) : (p2 & p6 & p8);
            if (m2 > 0) goto prev_check;
            A  = (p2 == 0 && p3 > 0) + (p3 == 0 && p4 > 0) +
                (p4 == 0 && p5 > 0) + (p5 == 0 && p6 > 0) +
                (p6 == 0 && p7 > 0) + (p7 == 0 && p8 > 0) +
                (p8 == 0 && p9 > 0) + (p9 == 0 && p2 > 0);

            if (A  != 1) {
                goto prev_check;
            }
            B  = (p2  > 0) + (p3  > 0) + (p4  > 0)
                + (p5  > 0) + (p6  > 0) + (p7  > 0)
                + (p8  > 0) + (p9  > 0);


            if (B >= 2 && B <= 6){
                r[step+j] = 254;
                anyChanges = true;
            }
        prev_check:
            if (r[j-1] == 254) r[j-1]=0;
            // if (r[j] == 254 && j == im.cols-2) r[j] = 0;
        }
    }

    r  = im.ptr(im.rows-2);

    for (int i=1; i<im.cols-1; i++){
        if (r[i] == 254) r[i] = 0;
    }

    r = im.ptr(0)+im.cols-2;
    for (int i=1; i<im.rows-1; i++){
        if (r[step*i] == 254){
            r[step*i] = 0;
        }
    }


    return anyChanges;
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
void thinning(cv::Mat &im, int algorithm)
{
    im /= 255;
    cv::Mat prev = cv::Mat::zeros(im.size(), CV_8UC1);
    cv::Mat diff;

    do{
        if (algorithm == THINNING_ALG_GUO_HALL)
        {
            thinningGuoHallIteration(im, 0);
            thinningGuoHallIteration(im, 1);
        }else{
            thinningIteration(im, 0);
            thinningIteration(im, 1);
        }
        cv::absdiff(im, prev, diff);
        im.copyTo(prev);
    }
    while (cv::countNonZero(diff) > 0);

    im *= 255;
}

void thinningOpt(cv::Mat &im)
{
    bool anyChanges = true;
    do{
        bool change1 = thinningIterationOpt(im, 0);
        bool change2 = thinningIterationOpt(im, 1);
        anyChanges = change1 | change2;
    }
    while (anyChanges);
}

/**
 * This is an example on how to call the thinning function above.
 */
int main(int argc, char **argv)
{
    char *filename = argv[1];
    int iterations = atoi(argv[2]);
    int showRst = atoi(argv[3]);
    cv::Mat src = cv::imread(filename);
    if (src.empty())
        return -1;

    cv::Mat bw;
    cv::cvtColor(src, bw, CV_BGR2GRAY);
    cv::threshold(bw, bw, 125, 255, CV_THRESH_BINARY);

    cv::Mat bw_backup = bw.clone();

    cv::Mat thinning_default_rst = bw.clone();
    thinning(thinning_default_rst,THINNING_ALG_DEFAULT);

    cv::Mat thinning_opt_rst = bw.clone();
    thinningOpt(thinning_opt_rst);
    bool correctness = countNonZero(thinning_default_rst != thinning_opt_rst) == 0;

    double defMinTime=1e10, defMaxTime=0, defAvTime = 0;
    for (int i=0; i<iterations; i++){
        bw = bw_backup.clone();
        double tstart = (double)getTickCount();
        thinning(bw,THINNING_ALG_DEFAULT);
        double elapsed = ((double)getTickCount() - tstart)/
            (double)getTickFrequency();
        if (elapsed < defMinTime) defMinTime = elapsed;
        if (elapsed > defMaxTime) defMaxTime = elapsed;
        defAvTime += elapsed;
    }
    defAvTime /= iterations;

    double optMinTime=1e10, optMaxTime=0, optAvTime = 0;
    for (int i=0; i<iterations; i++){
        bw = bw_backup.clone();
        double tstart = (double)getTickCount();
        thinningOpt(bw);
        double elapsed = ((double)getTickCount() - tstart)/
            (double)getTickFrequency();
        if (elapsed < optMinTime) optMinTime = elapsed;
        if (elapsed > optMaxTime) optMaxTime = elapsed;
        optAvTime += elapsed;
    }
    optAvTime /= iterations;
    printf("%s\n",filename);
    printf("Corr: %d | Lat(sec) [Min,Max,Av]: || Def [%9.5f:%9.5f:%9.5f] || Opt [%9.5f:%9.5f:%9.5f] || AvSpeedUp %5.2fx\n",
           correctness,defMinTime,defMaxTime,defAvTime,
           optMinTime,optMaxTime,optAvTime,
           defAvTime/optAvTime);
    printf("-------------------------------------------------------\n");
    fflush(stdout);

    if (showRst){
        imshow("original",bw_backup);
        imshow("thinned",bw);
        waitKey(0);
    }


    return 0;
}
