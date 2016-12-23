#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wmissing-declarations"
#  if defined __clang__ || defined __APPLE__
#    pragma GCC diagnostic ignored "-Wmissing-prototypes"
#    pragma GCC diagnostic ignored "-Wextra"
#  endif
#endif

#ifndef TEST_PRECOMP_HPP
#define TEST_PRECOMP_HPP

#define GTEST_DONT_DEFINE_FAIL      0
#define GTEST_DONT_DEFINE_SUCCEED   0
#define GTEST_DONT_DEFINE_ASSERT_EQ 0
#define GTEST_DONT_DEFINE_ASSERT_NE 0
#define GTEST_DONT_DEFINE_ASSERT_LE 0
#define GTEST_DONT_DEFINE_ASSERT_LT 0
#define GTEST_DONT_DEFINE_ASSERT_GE 0
#define GTEST_DONT_DEFINE_ASSERT_GT 0
#define GTEST_DONT_DEFINE_TEST      0

#include "opencv2/ts/ts_gtest.h"

#define IVX_HIDE_INFO_WARNINGS
#include "ivx.hpp"
#include "ivx_lib_debug.hpp"

#endif // TEST_PRECOMP_HPP
