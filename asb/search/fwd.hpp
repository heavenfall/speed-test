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

#ifndef ASB_SEARCH_FWD_HPP
#define ASB_SEARCH_FWD_HPP

#include <inx/inx.hpp>
#include <geo/Point.hpp>

// enables/disables redblack tree, rb-tree is much faster
#ifndef ASB_DISABLE_REDBLACK_TREE
#if !defined(ASB_ENABLE_REDBLACK_TREE)
#define ASB_ENABLE_REDBLACK_TREE
#endif
#endif

// #define ASB_ENABLE_EXPANDER_DFS

namespace asb::search {

class BasicQueue;
class Search;
class SearchGrid;

} // namespace asb::search

#endif // ASB_SEARCH_FWD_HPP
