// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

// Copyright (C) 2016, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.

/*
Tests for C++ wrappers over OpenVX 1.x C API
Details: TBD
*/

#include "precomp.hpp"

using namespace ivx;

TEST(Context, create_assign)
{
    Context c1 = Context::create();
    ASSERT_FALSE(!c1);
    ASSERT_EQ(c1.version(), VX_VERSION);

    Context c2(c1);
    ASSERT_GT(c2.refsNum(), 0u);

    Context c3;
    c3 = c2;
    ASSERT_GT(c3.kernelTable().size(), 0u);
}

TEST(Context, reset_refcount)
{
    Context c1 = Context::create();
    ASSERT_FALSE(!c1);

    Context c2;
    ASSERT_TRUE(!c2);
    c2.reset();
    ASSERT_TRUE(!c2);
    c2 = c2;
    ASSERT_TRUE(!c2);
    c2.detach();
    ASSERT_TRUE(!c2);
    c2.reset(c1, false);
    ASSERT_FALSE(!c2);
    c2.detach();
    ASSERT_TRUE(!c2);
    c2.reset(c1, false);
    ASSERT_EQ(c1.refsNum(), c2.refsNum());
    c2.reset(c2.get<Context>(), false);
    ASSERT_FALSE(!c2);
    c2.reset((vx_context)c2, false);
    ASSERT_FALSE(!c2);
    c2 = c2;
    ASSERT_FALSE(!c2);

    Context c3;
    c3.reset(c2, false);
    ASSERT_EQ(c3.refsNum(), c1.refsNum());
    c3.reset(c1, false);
    ASSERT_EQ(c3.refsNum(), c2.refsNum());
    c3.detach();
    ASSERT_EQ(c1.refsNum(), c2.refsNum());

    ASSERT_EQ(c1.version(), VX_VERSION);
}

/* End of file. */
