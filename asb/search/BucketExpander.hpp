/*
MIT License

Copyright (c) 2023 Ryan Hechenberger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ASB_SEARCH_BUCKETEXPANDER_HPP
#define ASB_SEARCH_BUCKETEXPANDER_HPP

#include "fwd.hpp"
#include "Node.hpp"
#include "SearchGrid.hpp"
#include <vector>
#include <array>
#include <bit>

namespace asb::search {

class BucketExpander
{
public:
	static constexpr uint16_t BUCKET_ORDER = 0b101'010'100'011'001u;
	static constexpr uint16_t BALANCED_ORDER = 0b001'101'011'010'100u;
	void setup(SearchGrid& grid);
	void setupSearch(SearchId sid, NodeDesc target);

	BucketPush& getBucket(dist_type rel_f) noexcept
	{
		return m_bucketLists[f_hash(rel_f)];
	}

	bool expandStartNode(dist_type fvalue, NodeId s);
	bool expandNode(dist_type fvalue, const NodeId* bucket, int size);
	// will read of memory bucket[0..size-1], expanding all nodes inside
	// returns true if target was discovered (ends when finding target)
#ifndef ASB_ENABLE_EXPANDER_DFS
	bool expandBucket(dist_type fvalue, const NodeId* bucket, int size);
#else
	bool expandBucket(dist_type fvalue);
#endif

	// special function that converts f into m_bucketLists id
	constexpr static int f_hash(dist_type f) noexcept
	{
		assert(f == 0 || f == 5857864376 || f == 20000000000 || f == 8284271248 || f == 14142135624 || f == 28284271248);
		/*
		14286848
		0 0b0 0
		5857864376 0b100000000000000000 1
		20000000000 0b100100000000000000000 2
		8284271248 0b110000100000000000000000 3
		14142135624 0b110010100000000000000000 4
		28284271248 0b110110100000000000000000 5
		*/
		return std::popcount(static_cast<dist_type>(f) & static_cast<dist_type>(14286848ull));
	}

	BucketPushArray& getBucketLists() noexcept { return m_bucketLists; }
	const BucketPushArray& getBucketLists() const noexcept { return m_bucketLists; }

protected:
	BucketPushArray m_bucketLists;
#ifndef ASB_ENABLE_EXPANDER_DFS
	BucketPush m_bucketLoop;
#endif
	SearchGrid* m_grid;
	SearchId m_sid;
	NodeDesc m_target;
};

} // namespace asb::search

#endif // ASB_SEARCH_BUCKETEXPANDER_HPP
