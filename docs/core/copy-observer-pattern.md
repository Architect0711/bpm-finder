---
layout: default
title: CopyObserverPattern
---

# Copy Observer Pattern

A thread-safe implementation of the Observer pattern that handles data copying between components in the BPM finder
application.

## Overview

The Copy Observer Pattern consists of two main components:

- `CopyObservable`: A template class that emits data to registered observers
- `CopyObserver`: A template class that receives and queues data for processing

This implementation is designed for scenarios where data needs to be passed between threads safely, with the observable
running in one thread and observers processing data in their own threads.

## Implementation Details

### CopyObservable<DataType>

A thread-safe observable that:

- Maintains a list of registered observers
- Provides thread-safe subscription mechanism
- Notifies observers by passing data by const reference
- Uses CRTP (Curiously Recurring Template Pattern) style

### CopyObserver<DataType>

A thread-safe observer that:

- Maintains a thread-safe queue of received data
- Uses condition variables for synchronization
- Copies received data into its internal queue
- Allows derived classes to process queued data in their own thread
