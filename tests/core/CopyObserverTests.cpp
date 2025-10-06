//
// Created by Robert on 2025-10-05.
//

#include <gtest/gtest.h>
#include "../src/core/CopyObservable.h"
#include "../src/core/CopyObserver.h"
#include <string>

using namespace bpmfinder::core;

/**
 * @class TestCopyObservable
 * @brief A specialized observable class for testing observer interactions.
 *
 * TestCopyObservable inherits from the CopyObservable class and provides a mechanism
 * to emit string data to subscribed observers. It facilitates the testing and
 * verification of the observable-observer notification system.
 *
 * This class allows for the creation and dispatch of data via the `CreateData` method,
 * which notifies all subscribed observers, ensuring thread-safe communication and proper
 * propagation of updates in the observer design pattern.
 */
class TestCopyObservable : public CopyObservable<std::string>
{
public:
    void CreateData(const std::string& data)
    {
        Notify(data);
    }
};

/**
 * @class TestCopyObserver
 * @brief Specialized observer class used for testing observable-observer interactions.
 *
 * TestCopyObserver extends the CopyObserver class with additional functionality to enable testing
 * of the Observer design pattern. It provides access to internal synchronization primitives
 * and the data queue to facilitate validation of data reception mechanisms.
 *
 * This class supports testing scenarios where thread-safe access is crucial, allowing
 * inspection of internal state and coordination via a mutex and condition variable.
 */
class TestCopyObserver : public CopyObserver<std::string>
{
public:
    std::mutex& GetMutex() { return mtx_; }
    std::condition_variable& GetConditionVariable() { return cv_; }
    std::queue<std::string>& GetQueue() { return queue_; }
};

TEST(CopyObserverTests, WhenObservableCreatesData_ThenObserverReceivesIt)
{
    // -------------------- Arrange --------------------
    TestCopyObservable observable;
    TestCopyObserver observer;
    observable.Subscribe(&observer);

    // -------------------- Act ------------------------
    observable.CreateData("test_data");

    // -------------------- Assert ---------------------
    std::unique_lock lock(observer.GetMutex());
    ASSERT_FALSE(observer.GetQueue().empty());
    EXPECT_EQ(observer.GetQueue().front(), "test_data");
}

TEST(CopyObserverTests, WhenMultipleDataCreated_ThenAllDataReceived)
{
    // -------------------- Arrange --------------------
    TestCopyObservable observable;
    TestCopyObserver observer;
    observable.Subscribe(&observer);

    // -------------------- Act ------------------------
    observable.CreateData("data1");
    observable.CreateData("data2");

    // -------------------- Assert ---------------------
    std::unique_lock lock(observer.GetMutex());
    ASSERT_EQ(observer.GetQueue().size(), 2);
    EXPECT_EQ(observer.GetQueue().front(), "data1");
    observer.GetQueue().pop();
    EXPECT_EQ(observer.GetQueue().front(), "data2");
}
