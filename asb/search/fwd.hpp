#ifndef ASB_SEARCH_FWD_HPP
#define ASB_SEARCH_FWD_HPP

#include <inx/inx.hpp>
#include <geo/Point.hpp>

// enables/disables redblack tree, rb-tree is much faster
#ifndef DISABLE_ASB_ENABLE_REDBLACK_TREE
#if !defined(ASB_ENABLE_REDBLACK_TREE)
#define ASB_ENABLE_REDBLACK_TREE
#endif
#endif

namespace asb::search {

class BasicQueue;
class Search;
class SearchGrid;

} // namespace asb::search

#endif // ASB_SEARCH_FWD_HPP
