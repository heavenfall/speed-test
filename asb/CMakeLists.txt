cmake_minimum_required(VERSION 3.13)

target_sources(asb PRIVATE
	env/Grid.cpp
	search/BucketExpander.cpp
	search/BucketQueue.cpp
	search/RedBlackQueue.cpp
	search/Search.cpp
	search/SearchGrid.cpp

	env/WarthogGrid.cpp
	search/WarthogBucketExpander.cpp
	search/WarthogSearch.cpp
	search/WarthogSearchGrid.cpp
)
