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

#ifndef ASB_SEARCH_SEARCH_HPP
#define ASB_SEARCH_SEARCH_HPP

#include "fwd.hpp"
#include "Types.hpp"
#include "Node.hpp"
#ifdef ASB_ENABLE_REDBLACK_TREE
#include "RedBlackQueue.hpp"
#else
#include "BucketQueue.hpp"
#endif
#include "BucketExpander.hpp"
#include "SearchGrid.hpp"

namespace asb::search {

class Search
{
public:
	Search();

	void setup(SearchGrid& l_grid);
	void search(geo::Point s, geo::Point t);
	const std::vector<geo::Point>& get_path() const noexcept { return m_path; }

	env::Grid* get_grid() noexcept { return m_grid; }
	const env::Grid* get_grid() const noexcept { return m_grid; }
	auto& get_queue() noexcept { return m_queue; }
	const auto& get_queue() const noexcept { return m_queue; }
	BucketExpander& get_expansion() noexcept { return m_expander; }
	const BucketExpander& get_expansion() const noexcept { return m_expander; }

	uint32_t getNodeId(geo::Point p) const noexcept
	{
		assert(m_grid != nullptr && static_cast<uint32_t>(p.x) < m_grid->getTable().getWidth() && static_cast<uint32_t>(p.y) < m_grid->getTable().getHeight());
		return static_cast<uint32_t>(m_grid->getTable().bit_index(p.x, p.y).id);
	}

protected:
	SearchId m_sid;
	SearchGrid* m_grid;
#ifdef ASB_ENABLE_REDBLACK_TREE
	RedBlackQueue m_queue;
#else
	BucketQueue m_queue;
#endif
	BucketExpander m_expander;
	std::vector<geo::Point> m_path;
};

} // namespace asb::search

#endif // ASB_SEARCH_SEARCH_HPP
