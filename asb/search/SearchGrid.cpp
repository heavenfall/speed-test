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
