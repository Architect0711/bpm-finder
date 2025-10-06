---
layout: default
title: Sink
---

# Sink

The sink is a base class for a piece of the dsp chain that does not emit any values. It consumes values using the
observer pattern and does something with them, but it will not be an observable, only an observer. Most common use cases
will be to write data into a file or to send it to the gui for displaying.

## Lifecycle

```mermaid
sequenceDiagram
    participant Main as Main Thread
    participant Sink as Sink Object
    participant Future as Future<void>
    participant Worker as Worker Thread
    participant Promise as Promise<void>
    participant Mutex as Mutex
    participant CV as Condition Variable
    participant Queue as Data Queue
    Note over Main, Queue: === STARTUP PHASE ===
    Main ->> Sink: Start()
    Sink ->> Sink: Set running_ = true
    Sink ->> Promise: Create promise
    Sink ->> Future: Get future from promise
    Sink ->> Worker: Create & launch worker thread
    Note over Worker: Thread starts executing
    Worker ->> Mutex: Lock mutex
    activate Mutex
    Worker ->> Promise: set_value()
    Note over Promise: Signal "I'm ready!"
    Promise -->> Future: Notify future
    Note over Worker: Thread is now waiting<br/>for data or stop signal
    Future -->> Main: ready_future.wait() returns
    Main ->> Main: Start() returns
    Note over Main: Sink is now running
    Note over Main, Queue: === DATA PROCESSING PHASE ===
    Main ->> Sink: PushData("data1")
    Sink ->> Mutex: Lock mutex
    activate Mutex
    Sink ->> Queue: queue_.push("data1")
    Sink ->> Mutex: Unlock mutex
    deactivate Mutex
    Sink ->> CV: notify_one()
    Note over CV: Wake up worker thread
    CV -->> Worker: Wake up from cv_.wait()
    Note over Worker: Predicate returns true<br/>(queue not empty)
    Worker ->> Queue: Check !queue_.empty()
    Queue -->> Worker: true
    Worker ->> Queue: data = queue_.front()
    Worker ->> Queue: queue_.pop()
    Worker ->> Mutex: Unlock mutex
    deactivate Mutex
    Note over Mutex: Lock released during processing
    Worker ->> Worker: Process(data)
    activate Worker
    Note over Worker: Virtual method<br/>does actual work
    deactivate Worker
    Worker ->> Mutex: Lock mutex
    activate Mutex
    Worker ->> Queue: Check !queue_.empty()
    Queue -->> Worker: false (no more data)
    Note over Worker: Back to cv_.wait()
    Worker ->> CV: wait(lock, predicate)
    Note over Worker: Thread sleeps,<br/>mutex released atomically
    Note over Main, Queue: === CONCURRENT DATA PUSH ===
    Main ->> Sink: PushData("data2")
    Sink ->> Mutex: Lock mutex
    activate Mutex
    Sink ->> Queue: queue_.push("data2")
    Sink ->> Mutex: Unlock mutex
    deactivate Mutex
    Sink ->> CV: notify_one()
    CV -->> Worker: Wake up
    Note over Worker: Predicate true,<br/>reacquires mutex
    Worker ->> Queue: Process "data2"...
    Note over Worker: (same flow as before)
    Note over Main, Queue: === SHUTDOWN PHASE ===
    Main ->> Sink: Stop()
    Sink ->> Sink: Set running_ = false
    Sink ->> CV: notify_one()
    Note over CV: Wake up worker to check<br/>running_ flag
    CV -->> Worker: Wake up from cv_.wait()
    Worker ->> Worker: Check predicate
    Note over Worker: !running_ is true
    Worker ->> Queue: Check queue_.empty()
    Queue -->> Worker: true (all processed)
    Worker ->> Worker: Break from loop
    Worker ->> Mutex: Unlock mutex (automatic)
    deactivate Mutex
    Note over Worker: Thread function exits
    Worker -->> Sink: Thread terminates
    Sink ->> Worker: thread_.join()
    Note over Sink: Wait for thread<br/>to fully terminate
    Worker -->> Sink: Join complete
    Sink ->> Main: Stop() returns
    Note over Main, Queue: === CLEANUP COMPLETE ===
```

### Worker Thread State Machine

```mermaid
stateDiagram-v2
    [*] --> Created: Start() called
    Created --> Signaling: Worker thread starts
    Signaling --> Waiting: Promise set_value()<br/>Main thread unblocked
    Waiting --> Processing: Data available in queue<br/>(CV notified)
    Processing --> Waiting: Queue empty<br/>(enter cv_.wait)
    Processing --> Processing: More data in queue
    Waiting --> Stopping: running_ = false<br/>(CV notified by Stop())
    Processing --> Stopping: running_ = false
    Stopping --> [*]: Thread exits<br/>join() completes
    note right of Waiting
        Mutex is released atomically
        while waiting on CV
    end note
    note right of Processing
        Mutex is explicitly unlocked
        during Process() call
    end note
```
