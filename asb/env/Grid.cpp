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
