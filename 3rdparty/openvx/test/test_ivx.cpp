/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "test_precomp.hpp"

using namespace cv;
using namespace std;


/*
 * Comparing OpenVX based implementation with the main one
*/

#ifndef IMPLEMENT_PARAM_CLASS
#define IMPLEMENT_PARAM_CLASS(name, type) \
    class name \
    { \
    public: \
        name ( type arg = type ()) : val_(arg) {} \
        operator type () const {return val_;} \
    private: \
        type val_; \
    }; \
    inline void PrintTo( name param, std::ostream* os) \
    { \
        *os << #name <<  "(" << testing::PrintToString(static_cast< type >(param)) << ")"; \
    }
#endif // IMPLEMENT_PARAM_CLASS

IMPLEMENT_PARAM_CLASS(ImagePath, string)
IMPLEMENT_PARAM_CLASS(ApertureSize, int)
IMPLEMENT_PARAM_CLASS(L2gradient, bool)

PARAM_TEST_CASE(CannyVX, ImagePath, ApertureSize, L2gradient)
{
    string imgPath;
    int kSize;
    bool useL2;
    Mat src, dst;

    virtual void SetUp()
    {
        imgPath = GET_PARAM(0);
        kSize = GET_PARAM(1);
        useL2 = GET_PARAM(2);
    }

    void loadImage()
    {
        src = cv::imread(cvtest::TS::ptr()->get_data_path() + imgPath, IMREAD_GRAYSCALE);
        ASSERT_FALSE(src.empty()) << "cann't load image: " << imgPath;
    }
};

TEST_P(CannyVX, Accuracy)
{
    if(haveOpenVX())
    {
        loadImage();

        setUseOpenVX(false);
        Mat canny;
        cv::Canny(src, canny, 100, 150, 3);

        setUseOpenVX(true);
        Mat cannyVX;
        cv::Canny(src, cannyVX, 100, 150, 3);

        // 'smart' diff check (excluding isolated pixels)
        Mat diff, diff1;
        absdiff(canny, cannyVX, diff);
        boxFilter(diff, diff1, -1, Size(3,3));
        const int minPixelsAroud = 3; // empirical number
        diff1 = diff1 > 255/9 * minPixelsAroud;
        erode(diff1, diff1, Mat());
        double error = cv::norm(diff1, NORM_L1) / 255;
        const int maxError = std::min(10, diff.size().area()/100); // empirical number
        if(error > maxError)
        {
            string outPath =
                    string("CannyVX-diff-") +
                    imgPath + '-' +
                    'k' + char(kSize+'0') + '-' +
                    (useL2 ? "l2" : "l1");
            std::replace(outPath.begin(), outPath.end(), '/', '_');
            std::replace(outPath.begin(), outPath.end(), '\\', '_');
            std::replace(outPath.begin(), outPath.end(), '.', '_');
            imwrite(outPath+".png", diff);
        }
        ASSERT_LE(error, maxError);

    }
}

    INSTANTIATE_TEST_CASE_P(
                ImgProc, CannyVX,
                testing::Combine(
                    testing::Values(
                        string("shared/baboon.png"),
                        string("shared/fruits.png"),
                        string("shared/lena.png"),
                        string("shared/pic1.png"),
                        string("shared/pic3.png"),
                        string("shared/pic5.png"),
                        string("shared/pic6.png")
                    ),
                    testing::Values(ApertureSize(3), ApertureSize(5)),
                    testing::Values(L2gradient(false), L2gradient(true))
                )
    );

/* End of file. */
