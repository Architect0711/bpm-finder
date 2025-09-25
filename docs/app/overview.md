# app

The entire app logic is encapsulated into `BpmFinderApp.cpp`, which takes all its dependencies via constructor
injection. It is constructed by the `BpmFinderAppFactory.cpp`, which builds different versions of the app for production
and testing. This allows us to test every module independently. 