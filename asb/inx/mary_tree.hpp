#ifndef GMLIB_MARY_TREE_HPP_INCLUDED
#define GMLIB_MARY_TREE_HPP_INCLUDED

#include "inx.hpp"

namespace inx {

template <size_t M> 
class mary_tree_base;
template <size_t M>
struct mary_tree_node;

template <size_t M>
struct mary_tree_node
{
	static_assert(M >= 2, "M must be greater than or equal to 2");
	constexpr static size_t node_width() noexcept { return M; }
	using self = mary_tree_node<M>;
	using node_type = self;

	bool is_leaf() const noexcept
	{
		return Helper_leaf<>(std::make_index_sequence<M>()) == 0;
	}
	template <size_t... I>
	intptr_t Helper_leaf(std::index_sequence<I...>) const noexcept
	{
		return (reinterpret_cast<intptr_t>(m_nData.children[I]) | ...);
	}

	size_t degree() const noexcept
	{
		return Helper_deg(std::make_index_sequence<M>());
	}
	template <size_t... I>
	void* Helper_deg(std::index_sequence<I...>) const noexcept
	{
		return (static_cast<size_t>(m_nData.children[I] != nullptr) + ...);
	}

	bool is_root() const noexcept
	{
		return m_nData.parent == nullptr;
	}

	bool has_child(const node_type& child) const noexcept
	{
		return std::apply([c=&child](auto*... cld) { return ( (cld == c) || ...); }, m_nData.children);
	}

	size_t child_index(const node_type& child) const noexcept
	{
		return std::find(m_nData.children.cbegin(), m_nData.children.cend(), &child) - m_nData.children.cbegin();
	}

	bool children_connected() const noexcept
	{
		return std::apply([t=this](auto*... cld) { return ( (cld == nullptr || cld->m_nData.parent == t) && ...); }, m_nData.children);
	}

	node_type* connect_child(node_type& child, size_t i) noexcept
	{
		assert(i < node_width());
		child.m_nData.parent = this;
		return std::exchange(m_nData.children[i], &child);
	}
	node_type* connect_none(size_t i) noexcept
	{
		assert(i < node_width());
		return std::exchange(m_nData.children[i], nullptr);
	}
	node_type* connect_child_auto(node_type* child, size_t i) noexcept
	{
		assert(i < node_width());
		if (child != nullptr) {
			child->m_nData.parent = this;
		}
		return std::exchange(m_nData.children[i], child);
	}
	node_type* make_root() noexcept
	{
		return std::exchange(m_nData.parent, nullptr);
	}

	node_type* parent() noexcept { return m_nData.parent; }
	const node_type* parent() const noexcept { return m_nData.parent; }

	node_type* child(size_t i) noexcept { assert(i < node_width()); return m_nData.children[i]; }
	const node_type* child(size_t i) const noexcept { assert(i < node_width()); return m_nData.children[i]; }

	auto& children() noexcept { return m_nData.children; }
	const auto& children() const noexcept { return m_nData.children; }

	struct NodeData {
		node_type* parent;
		std::array<node_type*, node_width()> children;
	} m_nData;
};

template <size_t M> 
class mary_tree_base
{
public:
	static_assert(M >= 2, "M must be greater than or equal to 2");
	using self = mary_tree_base<M>;
	using value_type = mary_tree_node<M>;

	mary_tree_base() noexcept : m_root(nullptr), m_size(0) { }
	mary_tree_base(const self&) = delete;
	mary_tree_base(self&&) = default;

	value_type& root() noexcept { return *m_root; }
	const value_type& root() const noexcept { return *m_root; }

	size_t size() const noexcept { return m_size; }
	bool empty() const noexcept { assert((m_root == nullptr) == (m_size == 0)); return m_root == nullptr; }

protected:
	value_type* m_root;
	size_t m_size;
};

} // namespace inx::alg

#endif // GMLIB_MARY_TREE_HPP_INCLUDED