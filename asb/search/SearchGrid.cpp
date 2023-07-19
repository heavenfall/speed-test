#include "SearchGrid.hpp"

namespace asb::search {

SearchGrid::SearchGrid()
{ }

void SearchGrid::setup(const std::vector<bool>& bits, int width, int height)
{
	env::Grid::setup(bits, width, height);
	m_nodes.resize(getWidth() * getHeight());
	auto* node_data = m_nodes.data();
	for (int i = 0, y = 0; y < getHeight(); ++y)
	for (int x = 0; x < getWidth(); ++i, ++x, ++node_data) {
		node_data->id = NodeId{static_cast<uint32_t>(i)};
		node_data->p = geo::Point(x, y);
	}
}

} // namespace asb::search
