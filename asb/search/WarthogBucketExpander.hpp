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

#ifndef ASB_SEARCH_WARTHOGBUCKETEXPANDER_HPP
#define ASB_SEARCH_WARTHOGBUCKETEXPANDER_HPP

#include "fwd.hpp"
#include "BucketExpander.hpp"
#include "WarthogSearchGrid.hpp"

namespace asb::search {

class WarthogBucketExpander : public BucketExpander
{
public:
	static constexpr uint16_t BUCKET_ORDER = 0b101'010'100'011'001u;
	static constexpr uint16_t BALANCED_ORDER = 0b001'101'011'010'100u;
	void setup(WarthogSearchGrid& grid);
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

	static uint32_t get3x3(WarthogSearchGrid::table& T, uint32_t p)
	{
		uint8_t r[3];
		T.get_neighbours(p, r);
		return uint32_t{r[0]} | (uint32_t{r[1]} << 3) | (uint32_t{r[2]} << 6);
	}
};

} // namespace asb::search

#endif // ASB_SEARCH_WARTHOGBUCKETEXPANDER_HPP
