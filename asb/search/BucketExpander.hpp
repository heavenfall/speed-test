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
	void setup(SearchGrid& grid);
	void setupSearch(SearchId sid, NodeDesc target);

	BucketPush& getBucket(dist_type rel_f) noexcept
	{
		return m_bucketLists[f_hash(rel_f)];
	}

	bool expandStartNode(dist_type fvalue, NodeId s);
	// will read of memory bucket[0..size-1], expanding all nodes inside
	// returns true if target was discovered (ends when finding target)
	bool expandBucket(dist_type fvalue, const NodeId* bucket, int size);

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
	BucketPush m_bucketLoop;
	SearchGrid* m_grid;
	SearchId m_sid;
	NodeDesc m_target;
};

} // namespace asb::search

#endif // ASB_SEARCH_BUCKETEXPANDER_HPP