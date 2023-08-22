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
