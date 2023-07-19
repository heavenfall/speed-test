#ifndef ASB_SEARCH_SEARCHGRID_HPP
#define ASB_SEARCH_SEARCHGRID_HPP

#include "fwd.hpp"
#include <env/Grid.hpp>
#include "Node.hpp"

namespace asb::search {

class SearchGrid : public env::Grid
{
public:
	SearchGrid();

	void setup(const std::vector<bool>& bits, int width, int height);
	std::vector<Node>& getNodes() noexcept { return m_nodes; }

protected:
	std::vector<Node> m_nodes;
};

} // namespace asb::search

#endif // ASB_SEARCH_SEARCHGRID_HPP
