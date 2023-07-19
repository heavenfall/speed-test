#include "Grid.hpp"

namespace asb::env {

Grid::Grid()
{ }

void Grid::setup(const std::vector<bool> &bits, int width, int height)
{
	m_width = static_cast<uint32_t>(width) + 2*PADDING;
	m_height = static_cast<uint32_t>(height) + 2*PADDING;
#ifdef ABS_USE_HORI_VERT_GRID
	m_table[0].setup(m_width, m_height);
	m_table[1].setup(m_height, m_width);
#else
	m_table.setup(m_width, m_height);
#endif
	width += static_cast<int>(PADDING);
	height += static_cast<int>(PADDING);
	for (int i = 0, y = static_cast<int>(PADDING); y < height; ++y)
	for (int x = static_cast<int>(PADDING); x < width; ++x) {
		if (bits[i++]) {
#ifdef ABS_USE_HORI_VERT_GRID
			m_table[0].bit_or(x, y, 1);
			m_table[1].bit_or(y, x, 1);
#else
			m_table.bit_or(x, y, 1);
#endif
		}
	}
	assert(m_table.getRowBits() >= m_width);
	m_width = m_table.getRowBits();
}

} // namespace asb::env
