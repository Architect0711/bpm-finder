//
// Created by Robert on 2025-10-06.
//

#include <gtest/gtest.h>
#include "../../src/core/Sink.h"
#include "../../src/core/Observable.h"
#include <string>
#include <vector>
#include <thread>
#include <chrono>

using namespace bpmfinder::core;

/**
 * @class TestSink
 * @brief A test implementation of Sink that collects processed data in a vector.
 *
 * TestSink provides a concrete implementation of the abstract Sink class for testing purposes.
 * It stores all processed data items in a thread-safe vector, allowing external verification
 * of the sink's behavior.
 */
class TestSink : public Sink<std::string>
{
private:
    std::vector<std::string> processed_data_;
    mutable std::mutex data_mutex_;

public:
    TestSink() = default;

    /**
     * @brief Gets a copy of all processed data.
     * @return Vector containing all processed data items.
     */
    std::vector<std::string> GetProcessedData() const
    {
        std::lock_guard<std::mutex> lock(data_mutex_);
        return processed_data_;
    }

    /**
     * @brief Gets the count of processed items.
     * @return Number of items that have been processed.
     */
    size_t GetProcessedCount() const
    {
        std::lock_guard<std::mutex> lock(data_mutex_);
        return processed_data_.size();
    }

    /**
     * @brief Clears all processed data.
     */
    void ClearProcessedData()
    {
        std::lock_guard<std::mutex> lock(data_mutex_);
        processed_data_.clear();
    }

protected:
    void Process(std::string data) override
    {
        std::lock_guard<std::mutex> lock(data_mutex_);
        processed_data_.push_back(std::move(data));
    }
};

/**
 * @class TestObservable
 * @brief A specialized observable class for testing sink interactions.
 */
class TestObservable : public Observable<std::string>
{
public:
    void EmitData(const std::string& data)
    {
        Notify(data);
    }
};

// ============================================================================
// Test Fixture
// ============================================================================

class SinkTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        sink_ = std::make_unique<TestSink>();
    }

    void TearDown() override
    {
        if (sink_ && sink_->IsRunning())
        {
            sink_->Stop();
        }
        sink_.reset();
    }

    /**
     * @brief Helper function to wait for a condition with timeout.
     * @param condition The condition to wait for.
     * @param timeout_ms Maximum time to wait in milliseconds.
     * @return true if condition was met, false if timeout occurred.
     */
    bool WaitForCondition(const std::function<bool()>& condition, int timeout_ms = 1000)
    {
        auto start = std::chrono::steady_clock::now();
        while (!condition())
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start).count();
            if (elapsed > timeout_ms)
            {
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return true;
    }

    std::unique_ptr<TestSink> sink_;
};

// ============================================================================
// Basic Functionality Tests
// ============================================================================

TEST_F(SinkTests, WhenSinkCreated_ThenNotRunning)
{
    EXPECT_FALSE(sink_->IsRunning());
}

TEST_F(SinkTests, WhenSinkStarted_ThenIsRunning)
{
    sink_->Start();
    EXPECT_TRUE(sink_->IsRunning());
}

TEST_F(SinkTests, WhenSinkStopped_ThenNotRunning)
{
    sink_->Start();
    ASSERT_TRUE(sink_->IsRunning());

    sink_->Stop();
    EXPECT_FALSE(sink_->IsRunning());
}

TEST_F(SinkTests, WhenStartCalledTwice_ThenNoError)
{
    sink_->Start();
    ASSERT_TRUE(sink_->IsRunning());

    // Should not throw or cause issues
    sink_->Start();
    EXPECT_TRUE(sink_->IsRunning());
}

TEST_F(SinkTests, WhenStopCalledTwice_ThenNoError)
{
    sink_->Start();
    sink_->Stop();
    ASSERT_FALSE(sink_->IsRunning());

    // Should not throw or cause issues
    sink_->Stop();
    EXPECT_FALSE(sink_->IsRunning());
}

// ============================================================================
// Data Processing Tests
// ============================================================================

TEST_F(SinkTests, WhenDataPushed_ThenDataProcessed)
{
    // -------------------- Arrange --------------------
    sink_->Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Give worker thread time to start

    // -------------------- Act ------------------------
    sink_->PushData("test_data");

    // -------------------- Assert ---------------------
    bool success = WaitForCondition([this]()
    {
        return sink_->GetProcessedCount() == 1;
    });

    ASSERT_TRUE(success) << "Timeout waiting for data to be processed";

    auto processed = sink_->GetProcessedData();
    ASSERT_EQ(processed.size(), 1);
    EXPECT_EQ(processed[0], "test_data");
}

TEST_F(SinkTests, WhenMultipleDataPushed_ThenAllDataProcessedInOrder)
{
    // -------------------- Arrange --------------------
    sink_->Start();

    // -------------------- Act ------------------------
    sink_->PushData("data1");
    sink_->PushData("data2");
    sink_->PushData("data3");

    // -------------------- Assert ---------------------
    bool success = WaitForCondition([this]()
    {
        return sink_->GetProcessedCount() == 3;
    });

    ASSERT_TRUE(success) << "Timeout waiting for data to be processed";

    auto processed = sink_->GetProcessedData();
    ASSERT_EQ(processed.size(), 3);
    EXPECT_EQ(processed[0], "data1");
    EXPECT_EQ(processed[1], "data2");
    EXPECT_EQ(processed[2], "data3");
}

TEST_F(SinkTests, WhenDataPushedBeforeStart_ThenDataProcessedAfterStart)
{
    // -------------------- Arrange --------------------
    sink_->PushData("early_data");

    // -------------------- Act ------------------------
    sink_->Start();

    // -------------------- Assert ---------------------
    bool success = WaitForCondition([this]()
    {
        return sink_->GetProcessedCount() == 1;
    });

    ASSERT_TRUE(success) << "Timeout waiting for data to be processed";

    auto processed = sink_->GetProcessedData();
    ASSERT_EQ(processed.size(), 1);
    EXPECT_EQ(processed[0], "early_data");
}

TEST_F(SinkTests, WhenSinkStopped_ThenPendingDataStillProcessed)
{
    // -------------------- Arrange --------------------
    sink_->Start();

    // Push multiple items
    for (int i = 0; i < 10; ++i)
    {
        sink_->PushData("data_" + std::to_string(i));
    }

    // Give it a moment to start processing
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // -------------------- Act ------------------------
    sink_->Stop();

    // -------------------- Assert ---------------------
    // All data should be processed
    auto processed = sink_->GetProcessedData();
    EXPECT_EQ(processed.size(), 10);

    // Verify order
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(processed[i], "data_" + std::to_string(i));
    }
}

// ============================================================================
// Observable Integration Tests
// ============================================================================

TEST_F(SinkTests, WhenConnectedToObservable_ThenDataFromObservableIsProcessed)
{
    // -------------------- Arrange --------------------
    TestObservable observable;
    observable.Subscribe(sink_.get());
    sink_->Start();

    // -------------------- Act ------------------------
    observable.EmitData("observable_data");

    // -------------------- Assert ---------------------
    bool success = WaitForCondition([this]()
    {
        return sink_->GetProcessedCount() == 1;
    });

    ASSERT_TRUE(success) << "Timeout waiting for data to be processed";

    auto processed = sink_->GetProcessedData();
    ASSERT_EQ(processed.size(), 1);
    EXPECT_EQ(processed[0], "observable_data");
}

TEST_F(SinkTests, WhenMultipleDataFromObservable_ThenAllProcessedInOrder)
{
    // -------------------- Arrange --------------------
    TestObservable observable;
    observable.Subscribe(sink_.get());
    sink_->Start();

    // -------------------- Act ------------------------
    for (int i = 0; i < 5; ++i)
    {
        observable.EmitData("item_" + std::to_string(i));
    }

    // -------------------- Assert ---------------------
    bool success = WaitForCondition([this]()
    {
        return sink_->GetProcessedCount() == 5;
    });

    ASSERT_TRUE(success) << "Timeout waiting for data to be processed";

    auto processed = sink_->GetProcessedData();
    ASSERT_EQ(processed.size(), 5);
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(processed[i], "item_" + std::to_string(i));
    }
}

// ============================================================================
// Stress Tests
// ============================================================================

TEST_F(SinkTests, WhenHighVolumeDataPushed_ThenAllDataProcessed)
{
    // -------------------- Arrange --------------------
    const size_t data_count = 1000;
    sink_->Start();

    // -------------------- Act ------------------------
    for (size_t i = 0; i < data_count; ++i)
    {
        sink_->PushData("data_" + std::to_string(i));
    }

    // -------------------- Assert ---------------------
    bool success = WaitForCondition([this, data_count]()
    {
        return sink_->GetProcessedCount() == data_count;
    }, 5000); // Longer timeout for high volume

    ASSERT_TRUE(success) << "Timeout waiting for all data to be processed. "
        << "Processed: " << sink_->GetProcessedCount()
        << "/" << data_count;

    auto processed = sink_->GetProcessedData();
    EXPECT_EQ(processed.size(), data_count);
}

TEST_F(SinkTests, WhenConcurrentDataPush_ThenAllDataProcessed)
{
    // -------------------- Arrange --------------------
    const size_t threads_count = 10;
    const size_t items_per_thread = 100;
    const size_t total_items = threads_count * items_per_thread;

    sink_->Start();
    std::vector<std::thread> threads;

    // -------------------- Act ------------------------
    for (size_t t = 0; t < threads_count; ++t)
    {
        threads.emplace_back([this, t, items_per_thread]()
        {
            for (size_t i = 0; i < items_per_thread; ++i)
            {
                sink_->PushData("thread_" + std::to_string(t) + "_item_" + std::to_string(i));
            }
        });
    }

    // Wait for all threads to finish pushing
    for (auto& thread : threads)
    {
        thread.join();
    }

    // -------------------- Assert ---------------------
    bool success = WaitForCondition([this, total_items]()
    {
        return sink_->GetProcessedCount() == total_items;
    }, 5000);

    ASSERT_TRUE(success) << "Timeout waiting for all data to be processed. "
        << "Processed: " << sink_->GetProcessedCount()
                         << "/" << total_items;

    EXPECT_EQ(sink_->GetProcessedData().size(), total_items);
}
