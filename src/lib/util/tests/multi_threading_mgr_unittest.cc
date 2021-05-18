// Copyright (C) 2019-2021 Internet Systems Consortium, Inc. ("ISC")
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <config.h>

#include <exceptions/exceptions.h>
#include <util/multi_threading_mgr.h>

#include <gtest/gtest.h>

using namespace isc::util;
using namespace isc;

/// @brief Verifies that the default mode is false (MT disabled).
TEST(MultiThreadingMgrTest, defaultMode) {
    // MT should be disabled
    EXPECT_FALSE(MultiThreadingMgr::instance().getMode());
}

/// @brief Verifies that the mode setter works.
TEST(MultiThreadingMgrTest, setMode) {
    // enable MT
    EXPECT_NO_THROW(MultiThreadingMgr::instance().setMode(true));
    // MT should be enabled
    EXPECT_TRUE(MultiThreadingMgr::instance().getMode());
    // disable MT
    EXPECT_NO_THROW(MultiThreadingMgr::instance().setMode(false));
    // MT should be disabled
    EXPECT_FALSE(MultiThreadingMgr::instance().getMode());
}

/// @brief Verifies that accessing the thread pool works.
TEST(MultiThreadingMgrTest, threadPool) {
    // get the thread pool
    EXPECT_NO_THROW(MultiThreadingMgr::instance().getThreadPool());
}

/// @brief Verifies that the thread pool size setter works.
TEST(MultiThreadingMgrTest, threadPoolSize) {
    // default thread count is 0
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
    // set thread count to 16
    EXPECT_NO_THROW(MultiThreadingMgr::instance().setThreadPoolSize(16));
    // thread count should be 16
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 16);
    // set thread count to 0
    EXPECT_NO_THROW(MultiThreadingMgr::instance().setThreadPoolSize(0));
    // thread count should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
}

/// @brief Verifies that the packet queue size setter works.
TEST(MultiThreadingMgrTest, packetQueueSize) {
    // default queue size is 0
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 0);
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPool().getMaxQueueSize(), 0);
    // set queue size to 16
    EXPECT_NO_THROW(MultiThreadingMgr::instance().setPacketQueueSize(16));
    // queue size should be 16
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 16);
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPool().getMaxQueueSize(), 16);
    // set queue size to 0
    EXPECT_NO_THROW(MultiThreadingMgr::instance().setPacketQueueSize(0));
    // queue size should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 0);
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPool().getMaxQueueSize(), 0);
}

/// @brief Verifies that detecting thread count works.
TEST(MultiThreadingMgrTest, detectThreadCount) {
    // detecting thread count should work
    EXPECT_NE(MultiThreadingMgr::detectThreadCount(), 0);
}

/// @brief Verifies that apply settings works.
TEST(MultiThreadingMgrTest, applyConfig) {
    // get the thread pool
    auto& thread_pool = MultiThreadingMgr::instance().getThreadPool();
    // MT should be disabled
    EXPECT_FALSE(MultiThreadingMgr::instance().getMode());
    // default thread count is 0
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
    // thread pool should be stopped
    EXPECT_EQ(thread_pool.size(), 0);
    // enable MT with 16 threads and queue size 256
    EXPECT_NO_THROW(MultiThreadingMgr::instance().apply(true, 16, 256));
    // MT should be enabled
    EXPECT_TRUE(MultiThreadingMgr::instance().getMode());
    // thread count should be 16
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 16);
    // queue size should be 256
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 256);
    // thread pool should be started
    EXPECT_EQ(thread_pool.size(), 16);
    // disable MT
    EXPECT_NO_THROW(MultiThreadingMgr::instance().apply(false, 16, 256));
    // MT should be disabled
    EXPECT_FALSE(MultiThreadingMgr::instance().getMode());
    // thread count should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
    // queue size should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 0);
    // thread pool should be stopped
    EXPECT_EQ(thread_pool.size(), 0);
    // enable MT with auto scaling
    EXPECT_NO_THROW(MultiThreadingMgr::instance().apply(true, 0, 0));
    // MT should be enabled
    EXPECT_TRUE(MultiThreadingMgr::instance().getMode());
    // thread count should be detected automatically
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), MultiThreadingMgr::detectThreadCount());
    // thread pool should be started
    EXPECT_EQ(thread_pool.size(), MultiThreadingMgr::detectThreadCount());
    // disable MT
    EXPECT_NO_THROW(MultiThreadingMgr::instance().apply(false, 0, 0));
    // MT should be disabled
    EXPECT_FALSE(MultiThreadingMgr::instance().getMode());
    // thread count should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
    // thread pool should be stopped
    EXPECT_EQ(thread_pool.size(), 0);
}

/// @brief Verifies that the critical section flag works.
TEST(MultiThreadingMgrTest, criticalSectionFlag) {
    // get the thread pool
    auto& thread_pool = MultiThreadingMgr::instance().getThreadPool();
    // MT should be disabled
    EXPECT_FALSE(MultiThreadingMgr::instance().getMode());
    // critical section should be disabled
    EXPECT_FALSE(MultiThreadingMgr::instance().isInCriticalSection());
    // thread count should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
    // thread pool should be stopped
    EXPECT_EQ(thread_pool.size(), 0);
    // exit critical section
    EXPECT_THROW(MultiThreadingMgr::instance().exitCriticalSection(), InvalidOperation);
    // critical section should be disabled
    EXPECT_FALSE(MultiThreadingMgr::instance().isInCriticalSection());
    // enter critical section
    EXPECT_NO_THROW(MultiThreadingMgr::instance().enterCriticalSection());
    // critical section should be enabled
    EXPECT_TRUE(MultiThreadingMgr::instance().isInCriticalSection());
    // enable MT with 16 threads and queue size 256
    EXPECT_NO_THROW(MultiThreadingMgr::instance().apply(true, 16, 256));
    // MT should be enabled
    EXPECT_TRUE(MultiThreadingMgr::instance().getMode());
    // thread count should be 16
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 16);
    // queue size should be 256
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 256);
    // thread pool should be stopped
    EXPECT_EQ(thread_pool.size(), 0);
    // exit critical section
    EXPECT_NO_THROW(MultiThreadingMgr::instance().exitCriticalSection());
    // critical section should be disabled
    EXPECT_FALSE(MultiThreadingMgr::instance().isInCriticalSection());
    // exit critical section
    EXPECT_THROW(MultiThreadingMgr::instance().exitCriticalSection(), InvalidOperation);
    // critical section should be disabled
    EXPECT_FALSE(MultiThreadingMgr::instance().isInCriticalSection());
    // disable MT
    EXPECT_NO_THROW(MultiThreadingMgr::instance().apply(false, 0, 0));
    // MT should be disabled
    EXPECT_FALSE(MultiThreadingMgr::instance().getMode());
    // thread count should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
    // queue size should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 0);
    // thread pool should be stopped
    EXPECT_EQ(thread_pool.size(), 0);
}

/// @brief Verifies that the critical section works.
TEST(MultiThreadingMgrTest, criticalSection) {
    // get the thread pool instance
    auto& thread_pool = MultiThreadingMgr::instance().getThreadPool();
    // thread pool should be stopped
    EXPECT_EQ(thread_pool.size(), 0);
    // apply multi-threading configuration with 16 threads and queue size 256
    MultiThreadingMgr::instance().apply(true, 16, 256);
    // thread count should match
    EXPECT_EQ(thread_pool.size(), 16);
    // thread count should be 16
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 16);
    // queue size should be 256
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 256);
    // use scope to test constructor and destructor
    {
        MultiThreadingCriticalSection cs;
        // thread pool should be stopped
        EXPECT_EQ(thread_pool.size(), 0);
        // thread count should be 16
        EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 16);
        // queue size should be 256
        EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 256);
        // use scope to test constructor and destructor
        {
            MultiThreadingCriticalSection inner_cs;
            // thread pool should be stopped
            EXPECT_EQ(thread_pool.size(), 0);
            // thread count should be 16
            EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 16);
            // queue size should be 256
            EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 256);
        }
        // thread pool should be stopped
        EXPECT_EQ(thread_pool.size(), 0);
        // thread count should be 16
        EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 16);
        // queue size should be 256
        EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 256);
    }
    // thread count should match
    EXPECT_EQ(thread_pool.size(), 16);
    // thread count should be 16
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 16);
    // queue size should be 256
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 256);
    // use scope to test constructor and destructor
    {
        MultiThreadingCriticalSection cs;
        // thread pool should be stopped
        EXPECT_EQ(thread_pool.size(), 0);
        // thread count should be 16
        EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 16);
        // queue size should be 256
        EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 256);
        // apply multi-threading configuration with 64 threads and queue size 4
        MultiThreadingMgr::instance().apply(true, 64, 4);
        // thread pool should be stopped
        EXPECT_EQ(thread_pool.size(), 0);
        // thread count should be 64
        EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 64);
        // queue size should be 4
        EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 4);
    }
    // thread count should match
    EXPECT_EQ(thread_pool.size(), 64);
    // thread count should be 64
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 64);
    // queue size should be 4
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 4);
    // use scope to test constructor and destructor
    {
        MultiThreadingCriticalSection cs;
        // thread pool should be stopped
        EXPECT_EQ(thread_pool.size(), 0);
        // thread count should be 64
        EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 64);
        // queue size should be 4
        EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 4);
        // apply multi-threading configuration with 0 threads
        MultiThreadingMgr::instance().apply(false, 64, 256);
        // thread pool should be stopped
        EXPECT_EQ(thread_pool.size(), 0);
        // thread count should be 0
        EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
        // queue size should be 0
        EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 0);
    }
    // thread count should match
    EXPECT_EQ(thread_pool.size(), 0);
    // thread count should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
    // queue size should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 0);
    // use scope to test constructor and destructor
    {
        MultiThreadingCriticalSection cs;
        // thread pool should be stopped
        EXPECT_EQ(thread_pool.size(), 0);
        // thread count should be 0
        EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
        // queue size should be 0
        EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 0);
        // use scope to test constructor and destructor
        {
            MultiThreadingCriticalSection inner_cs;
            // thread pool should be stopped
            EXPECT_EQ(thread_pool.size(), 0);
            // thread count should be 0
            EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
            // queue size should be 0
            EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 0);
        }
        // thread pool should be stopped
        EXPECT_EQ(thread_pool.size(), 0);
        // thread count should be 0
        EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
        // queue size should be 0
        EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 0);
    }
    // thread count should match
    EXPECT_EQ(thread_pool.size(), 0);
    // thread count should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 0);
    // queue size should be 0
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 0);
    // use scope to test constructor and destructor
    {
        MultiThreadingCriticalSection cs;
        // thread pool should be stopped
        EXPECT_EQ(thread_pool.size(), 0);
        // apply multi-threading configuration with 64 threads
        MultiThreadingMgr::instance().apply(true, 64, 256);
        // thread pool should be stopped
        EXPECT_EQ(thread_pool.size(), 0);
        // thread count should be 64
        EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 64);
        // queue size should be 256
        EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 256);
    }
    // thread count should match
    EXPECT_EQ(thread_pool.size(), 64);
    // thread count should be 64
    EXPECT_EQ(MultiThreadingMgr::instance().getThreadPoolSize(), 64);
    // queue size should be 256
    EXPECT_EQ(MultiThreadingMgr::instance().getPacketQueueSize(), 256);
    // apply multi-threading configuration with 0 threads
    MultiThreadingMgr::instance().apply(false, 0, 0);
}

/// @brief Test fixture for exercised CriticalSection callbacks.
class CriticalSectionCallbackTest : public ::testing::Test {
public:
    /// @brief Constructor.
    CriticalSectionCallbackTest() {}

    /// @brief A callback that adds the value, 1, to invocations lists.
    void one() {
        invocations_.push_back(1);
    }

    /// @brief A callback that adds the value, 2, to invocations lists.
    void two() {
        invocations_.push_back(2);
    }

    /// @brief A callback that adds the value, 3, to invocations lists.
    void three() {
        invocations_.push_back(3);
    }

    /// @brief A callback that adds the value, 4, to invocations lists.
    void four() {
        invocations_.push_back(4);
    }

    /// @brief Indicates whether or not the DHCP thread pool is running.
    ///
    /// @return True if the pool is running, false otherwise.
    bool isThreadPoolRunning() {
        return (MultiThreadingMgr::instance().getThreadPool().size());
    }

    /// @brief Checks callback invocations over a series of nested
    /// CriticalSections.
    ///
    /// @param entries A vector of the invocation values that should
    /// be present after entry into the outermost CriticalSection.  The
    /// expected values should be in the order the callbacks were added
    /// to the MultiThreadingMgr's list of callbacks.
    /// @param exits A vector of the invocation values that should
    /// be present after exiting the outermost CriticalSection.  The
    /// expected values should be in the order the callbacks were added
    /// to the MultiThreadingMgr's list of callbacks.
    void runCriticalSections(std::vector<int> entries, std::vector<int>exits) {
        // Pool must be running.
        ASSERT_TRUE(isThreadPoolRunning());

        // Clear the invocations list.
        invocations_.clear();

        // Use scope to create nested CriticalSections.
        {
            // Enter a critical section.
            MultiThreadingCriticalSection cs;

            // Thread pool should be stopped.
            ASSERT_FALSE(isThreadPoolRunning());

            if (entries.size()) {
                // We expect entry invocations.
                ASSERT_TRUE(invocations_ == entries);
            } else {
                // We do not expect entry invocations.
                ASSERT_FALSE(invocations_.size());
            }

            // Clear the invocations list.
            invocations_.clear();

            {
                // Enter another CriticalSection.
                MultiThreadingCriticalSection inner_cs;

                // Thread pool should still be stopped
                ASSERT_FALSE(isThreadPoolRunning());

                // We should not have had any callback invocations.
                ASSERT_FALSE(invocations_.size());
            }

            // After exiting inner section, the thread pool should
            // still be stopped
            ASSERT_FALSE(isThreadPoolRunning());

            // We should not have had more callback invocations.
            ASSERT_FALSE(invocations_.size());
        }

        // After exiting the outer section, the thread pool should
        // match the thread count.
        ASSERT_TRUE(isThreadPoolRunning());

        if (exits.size()) {
            // We expect exit invocations.
            ASSERT_TRUE(invocations_ == exits);
        } else {
            // We do not expect exit invocations.
            ASSERT_FALSE(invocations_.size());
        }
    }

    /// @brief A list of values set by callback invocations.
    std::vector<int> invocations_;
};

/// @brief Verifies that the critical section callbacks work.
TEST_F(CriticalSectionCallbackTest, basics) {
    // get the thread pool instance
    auto& thread_pool = MultiThreadingMgr::instance().getThreadPool();
    // thread pool should be stopped
    EXPECT_EQ(thread_pool.size(), 0);

    // Add two sets of CriticalSection call backs.
    MultiThreadingMgr::instance().addCriticalSectionCallbacks("oneAndTwo",
         std::bind(&CriticalSectionCallbackTest::one, this),
         std::bind(&CriticalSectionCallbackTest::two, this));

    MultiThreadingMgr::instance().addCriticalSectionCallbacks("threeAndFour",
         std::bind(&CriticalSectionCallbackTest::three, this),
         std::bind(&CriticalSectionCallbackTest::four, this));

    // Apply multi-threading configuration with 16 threads and queue size 256.
    MultiThreadingMgr::instance().apply(true, 16, 256);

    // Make three passes over nested CriticalSections to ensure
    // callbacks execute at the appropriate times and we can do
    // so repeatedly.
    for (int i = 0; i < 3; ++i) {
        runCriticalSections({1,3}, {2,4});
    }

    // Now remove the first set of callbacks.
    MultiThreadingMgr::instance().removeCriticalSectionCallbacks("oneAndTwo");

    // Retest CriticalSections.
    runCriticalSections({3}, {4});

    // Now remove the remaining callbacks.
    MultiThreadingMgr::instance().removeAllCriticalSectionCallbacks();

    // Retest CriticalSections.
    runCriticalSections({}, {});
}
