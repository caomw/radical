/******************************************************************************
 * Copyright (c) 2016 Sergey Alexandrov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include "test.h"

#include <radical/exceptions.h>
#include <radical/radiometric_response.h>

BOOST_AUTO_TEST_CASE(MatConstructor) {
  // Invalid initialization, should throw
  cv::Mat m;
  BOOST_CHECK_THROW(radical::RadiometricResponse rr(m), radical::RadiometricResponseException);
  m.create(1, 100, CV_32FC1);
  BOOST_CHECK_THROW(radical::RadiometricResponse rr(m), radical::RadiometricResponseException);
  m.create(1, 256, CV_32FC1);
  BOOST_CHECK_THROW(radical::RadiometricResponse rr(m), radical::RadiometricResponseException);
  m.create(1, 256, CV_8UC3);
  BOOST_CHECK_THROW(radical::RadiometricResponse rr(m), radical::RadiometricResponseException);
  // Valid initialization
  m.create(1, 256, CV_32FC3);
  m.setTo(1.0f);
  BOOST_CHECK_NO_THROW(radical::RadiometricResponse rr1(m, radical::RadiometricResponse::ChannelOrder::RGB));
  BOOST_CHECK_NO_THROW(radical::RadiometricResponse rr2(m, radical::RadiometricResponse::ChannelOrder::BGR));
}

BOOST_AUTO_TEST_CASE(LoadConstructor) {
  // Invalid initialization, should throw
  BOOST_CHECK_THROW(radical::RadiometricResponse rr(getTestFilename("file_that_does_not_exist.txt")),
                    radical::SerializationException);
  BOOST_CHECK_THROW(radical::RadiometricResponse rr(getTestFilename("radiometric_response_empty.txt")),
                    radical::SerializationException);
  BOOST_CHECK_THROW(radical::RadiometricResponse rr(getTestFilename("radiometric_response_invalid.txt")),
                    radical::SerializationException);
  BOOST_CHECK_NO_THROW(radical::RadiometricResponse rr(getTestFilename("radiometric_response_constant.txt")));
  BOOST_CHECK_NO_THROW(radical::RadiometricResponse rr(getTestFilename("radiometric_response_identity.txt")));
}

BOOST_AUTO_TEST_CASE(DirectMapPixel) {
  radical::RadiometricResponse rr(getTestFilename("radiometric_response_identity.txt"));
  BOOST_CHECK_EQUAL(rr.directMap(cv::Vec3f(0, 0, 0)), cv::Vec3b(0, 0, 0));
  BOOST_CHECK_EQUAL(rr.directMap(cv::Vec3f(100, 200, 255)), cv::Vec3b(100, 200, 255));
}

BOOST_AUTO_TEST_CASE(DirectMapImage) {
  radical::RadiometricResponse rr(getTestFilename("radiometric_response_identity.txt"));
  cv::Mat E(256, 1, CV_32FC3);
  cv::Mat I_expected(256, 1, CV_8UC3);
  for (int j = 0; j < 256; ++j) {
    E.at<cv::Vec3f>(0, j) = cv::Vec3f(j, j + 10, 255 - j);
    I_expected.at<cv::Vec3b>(0, j) = cv::Vec3b(j, std::min(255, j + 10), 255 - j);
  }
  cv::Mat I;
  rr.directMap(E, I);
  BOOST_CHECK_EQUAL_MAT(I, I_expected, cv::Vec3b);
}
