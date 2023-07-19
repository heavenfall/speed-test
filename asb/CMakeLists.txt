cmake_minimum_required(VERSION 3.13)

target_include_directories(run PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_sources(run PRIVATE env/Grid.cpp search/BucketQueue.cpp search/BucketExpander.cpp search/Search.cpp search/SearchGrid.cpp)
