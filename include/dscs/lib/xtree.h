#pragma once
#include <xtree>

namespace dscs {
	/*
		Taken from Microsoft's MSVC STL implementation
		Originally licensed under Apache-2.0 WITH LLVM-exception

		Modifications were made to achieve compatibility with the data
		structures found in Digimon Story Cyber Sleuth Complete Edition.
        This only works in release mode!
	*/

	using namespace std;

	template <class _Traits>
	class _Tree { // ordered red-black tree for map/multimap/set/multiset
	public:
		using value_type = typename _Traits::value_type;
		using allocator_type = typename _Traits::allocator_type;

	protected:
		using _Alty = _Rebind_alloc_t<allocator_type, value_type>;
		using _Alty_traits = allocator_traits<_Alty>;
		using _Node = _Tree_node<value_type, typename _Alty_traits::void_pointer>;
		using _Alnode = _Rebind_alloc_t<allocator_type, _Node>;
		using _Alnode_traits = allocator_traits<_Alnode>;
		using _Nodeptr = typename _Alnode_traits::pointer;

		using _Scary_val = _Tree_val<conditional_t<_Is_simple_alloc_v<_Alnode>, _Tree_simple_types<value_type>,
			_Tree_iter_types<value_type, typename _Alty_traits::size_type, typename _Alty_traits::difference_type,
			typename _Alty_traits::pointer, typename _Alty_traits::const_pointer, value_type&, const value_type&,
			_Nodeptr>>>;

		static constexpr bool _Multi = _Traits::_Multi;

		enum _Redbl { // colors for link to parent
			_Red,
			_Black
		};

	public:
		using key_type = typename _Traits::key_type;
		using value_compare = typename _Traits::value_compare;

		using key_compare = typename _Traits::key_compare;

		using size_type = typename _Alty_traits::size_type;
		using difference_type = typename _Alty_traits::difference_type;
		using pointer = typename _Alty_traits::pointer;
		using const_pointer = typename _Alty_traits::const_pointer;
		using reference = value_type&;
		using const_reference = const value_type&;

		using iterator =
			conditional_t<is_same_v<key_type, value_type>, _Tree_const_iterator<_Scary_val>, _Tree_iterator<_Scary_val>>;
		using const_iterator = _Tree_const_iterator<_Scary_val>;
		using _Unchecked_iterator = conditional_t<is_same_v<key_type, value_type>,
			_Tree_unchecked_const_iterator<_Scary_val>, _Tree_unchecked_iterator<_Scary_val>>;
		using _Unchecked_const_iterator = _Tree_unchecked_const_iterator<_Scary_val>;

		using reverse_iterator = _STD reverse_iterator<iterator>;
		using const_reverse_iterator = _STD reverse_iterator<const_iterator>;

		struct _Copy_tag {
			explicit _Copy_tag() = default;
		};

		struct _Move_tag {
			explicit _Move_tag() = default;
		};

	private:
		void _Move_assign(_Tree& _Right, _Equal_allocators) noexcept(is_nothrow_move_assignable_v<key_compare>) {
			clear();
			_Getcomp() = _Right._Getcomp(); // intentionally copy comparator, see LWG-2227
			_Pocma(_Getal(), _Right._Getal());
			_Swap_val_excluding_comp(_Right);
		}

		void _Move_assign(_Tree& _Right, _Propagate_allocators) {
			if (_Getal() == _Right._Getal()) {
				_Move_assign(_Right, _Equal_allocators{});
			}
			else {
				clear();
				_Getcomp() = _Right._Getcomp(); // intentionally copy comparator, see LWG-2227
				auto&& _Alproxy = _GET_PROXY_ALLOCATOR(_Alnode, _Getal());
				auto&& _Right_alproxy = _GET_PROXY_ALLOCATOR(_Alnode, _Right._Getal());
				_Container_proxy_ptr<_Alty> _Proxy(_Right_alproxy, _Leave_proxy_unbound{});
				const auto _Scary = _Get_scary();
				const auto _Right_scary = _Right._Get_scary();
				const auto _Newhead = _STD exchange(_Right_scary->_Myhead, _Node::_Buyheadnode(_Right._Getal()));
				const auto _Newsize = _STD exchange(_Right_scary->_Mysize, size_type{ 0 });
				_Scary->_Erase_head(_Getal());
				_Pocma(_Getal(), _Right._Getal());
				_Scary->_Myhead = _Newhead;
				_Scary->_Mysize = _Newsize;
				_Proxy._Bind(_Alproxy, _Scary);
				_Scary->_Swap_proxy_and_iterators(*_Right_scary);
			}
		}

		void _Move_assign(_Tree& _Right, _No_propagate_allocators) {
			if (_Getal() == _Right._Getal()) {
				_Move_assign(_Right, _Equal_allocators{});
			}
			else {
				clear();
				_Getcomp() = _Right._Getcomp(); // intentionally copy comparator, see LWG-2227
				_Copy(_Right, _Move_tag{});
			}
		}

	public:
		_Tree& operator=(_Tree&& _Right) noexcept(noexcept(_Move_assign(_Right, _Choose_pocma<_Alnode>{}))) {
			if (this != _STD addressof(_Right)) {
				_Move_assign(_Right, _Choose_pocma<_Alnode>{});
			}

			return *this;
		}

	private:
		void _Swap_val_excluding_comp(_Tree& _Right) { // swap contents (except comparator) with _Right, equal allocators
			const auto _Scary = _Get_scary();
			const auto _Right_scary = _Right._Get_scary();
			_Scary->_Swap_proxy_and_iterators(*_Right_scary);
			_Swap_adl(_Scary->_Myhead, _Right_scary->_Myhead);
			_STD swap(_Scary->_Mysize, _Right_scary->_Mysize);
		}

	protected:
		template <class... _Valtys>
		pair<_Nodeptr, bool> _Emplace(_Valtys&&... _Vals) {
			using _In_place_key_extractor = typename _Traits::template _In_place_key_extractor<_Remove_cvref_t<_Valtys>...>;
			const auto _Scary = _Get_scary();
			_Tree_find_result<_Nodeptr> _Loc;
			_Nodeptr _Inserted;
			if constexpr (!_Multi && _In_place_key_extractor::_Extractable) {
				const auto& _Keyval = _In_place_key_extractor::_Extract(_Vals...);
				_Loc = _Find_lower_bound(_Keyval);
				if (_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
					return { _Loc._Bound, false };
				}

				_Check_grow_by_1();
				_Inserted = _Tree_temp_node<_Alnode>(_Getal(), _Scary->_Myhead, _STD forward<_Valtys>(_Vals)...)._Release();
				// nothrow hereafter
			}
			else {
				_Tree_temp_node<_Alnode> _Newnode(_Getal(), _Scary->_Myhead, _STD forward<_Valtys>(_Vals)...);
				const auto& _Keyval = _Traits::_Kfn(_Newnode._Ptr->_Myval);
				if constexpr (_Multi) { // duplicate check if unique
					_Loc = _Find_upper_bound(_Keyval);
				}
				else {
					_Loc = _Find_lower_bound(_Keyval);
					if (_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
						return { _Loc._Bound, false };
					}
				}

				_Check_grow_by_1();
				// nothrow hereafter
				_Inserted = _Newnode._Release();
			}

			return { _Scary->_Insert_node(_Loc._Location, _Inserted), true };
		}

	public:
		template <class... _Valtys>
		pair<iterator, bool> emplace(_Valtys&&... _Vals) {
			const auto _Result = _Emplace(_STD forward<_Valtys>(_Vals)...);
			return { iterator(_Result.first, _Get_scary()), _Result.second };
		}

	protected:
		template <class... _Valtys>
		_Nodeptr _Emplace_hint(const _Nodeptr _Hint, _Valtys&&... _Vals) {
			using _In_place_key_extractor = typename _Traits::template _In_place_key_extractor<_Remove_cvref_t<_Valtys>...>;
			const auto _Scary = _Get_scary();
			_Tree_find_hint_result<_Nodeptr> _Loc;
			_Nodeptr _Inserted;
			if constexpr (!_Multi && _In_place_key_extractor::_Extractable) {
				_Loc = _Find_hint(_Hint, _In_place_key_extractor::_Extract(_Vals...));
				if (_Loc._Duplicate) {
					return _Loc._Location._Parent;
				}

				_Check_grow_by_1();
				_Inserted = _Tree_temp_node<_Alnode>(_Getal(), _Scary->_Myhead, _STD forward<_Valtys>(_Vals)...)._Release();
				// nothrow hereafter
			}
			else {
				_Tree_temp_node<_Alnode> _Newnode(_Getal(), _Scary->_Myhead, _STD forward<_Valtys>(_Vals)...);
				_Loc = _Find_hint(_Hint, _Traits::_Kfn(_Newnode._Ptr->_Myval));
				if constexpr (!_Multi) {
					if (_Loc._Duplicate) {
						return _Loc._Location._Parent;
					}
				}

				_Check_grow_by_1();
				// nothrow hereafter
				_Inserted = _Newnode._Release();
			}

			return _Scary->_Insert_node(_Loc._Location, _Inserted);
		}

	public:
		template <class... _Valtys>
		iterator emplace_hint(const_iterator _Where, _Valtys&&... _Vals) { // insert value_type(_Val...) at _Where
			const auto _Scary = _Get_scary();
		#if _ITERATOR_DEBUG_LEVEL == 2
			_STL_VERIFY(_Where._Getcont() == _Scary, "map/set insert iterator outside range");
		#endif // _ITERATOR_DEBUG_LEVEL == 2
			return iterator(_Emplace_hint(_Where._Ptr, _STD forward<_Valtys>(_Vals)...), _Scary);
		}

		~_Tree() noexcept {
			const auto _Scary = _Get_scary();
			_Scary->_Erase_head(_Getal());
		#if _ITERATOR_DEBUG_LEVEL != 0 // TRANSITION, ABI
			auto&& _Alproxy = _GET_PROXY_ALLOCATOR(_Alnode, _Getal());
			_Delete_plain_internal(_Alproxy, _Scary->_Myproxy);
		#endif // _ITERATOR_DEBUG_LEVEL != 0
		}

	private:
		void _Copy_assign(const _Tree& _Right, false_type) {
			clear();
			_Getcomp() = _Right._Getcomp();
			_Pocca(_Getal(), _Right._Getal());
			_Copy(_Right, _Copy_tag{});
		}

		void _Copy_assign(const _Tree& _Right, true_type) {
			if (_Getal() == _Right._Getal()) {
				_Copy_assign(_Right, false_type{});
			}
			else {
				clear();
				const auto _Scary = _Get_scary();
				_Scary->_Orphan_all();
				auto& _Al = _Getal();
				auto&& _Alproxy = _GET_PROXY_ALLOCATOR(_Alnode, _Al);
				const auto& _Right_al = _Right._Getal();
				auto&& _Right_alproxy = _GET_PROXY_ALLOCATOR(_Alnode, _Right_al);
				_Container_proxy_ptr<_Alty> _Proxy(_Right_alproxy, _Leave_proxy_unbound{});
				auto _Right_al_non_const = _Right_al;
				auto _Newhead = _Node::_Buyheadnode(_Right_al_non_const);
				_Node::_Freenode0(_Al, _Scary->_Myhead);
				_Pocca(_Al, _Right_al);
				_Scary->_Myhead = _Newhead;
				_Proxy._Bind(_Alproxy, _Scary);
				_Getcomp() = _Right._Getcomp();
				_Copy(_Right, _Copy_tag{});
			}
		}

	public:
		_Tree& operator=(const _Tree& _Right) {
			if (this != _STD addressof(_Right)) {
				_Copy_assign(_Right, _Choose_pocca<_Alnode>{});
			}

			return *this;
		}

		_NODISCARD iterator begin() noexcept {
			const auto _Scary = _Get_scary();
			return iterator(_Scary->_Myhead->_Left, _Scary);
		}

		_NODISCARD const_iterator begin() const noexcept {
			const auto _Scary = _Get_scary();
			return const_iterator(_Scary->_Myhead->_Left, _Scary);
		}

		_NODISCARD iterator end() noexcept {
			const auto _Scary = _Get_scary();
			return iterator(_Scary->_Myhead, _Scary);
		}

		_NODISCARD const_iterator end() const noexcept {
			const auto _Scary = _Get_scary();
			return const_iterator(_Scary->_Myhead, _Scary);
		}

		_Unchecked_iterator _Unchecked_begin() noexcept {
			return _Unchecked_iterator(_Get_scary()->_Myhead->_Left, nullptr);
		}

		_Unchecked_const_iterator _Unchecked_begin() const noexcept {
			return _Unchecked_const_iterator(_Get_scary()->_Myhead->_Left, nullptr);
		}

		_Default_sentinel _Unchecked_end() const noexcept {
			return {};
		}

		_Unchecked_const_iterator _Unchecked_end_iter() const noexcept {
			return _Unchecked_const_iterator(_Get_scary()->_Myhead, nullptr);
		}

		_NODISCARD reverse_iterator rbegin() noexcept {
			return reverse_iterator(end());
		}

		_NODISCARD const_reverse_iterator rbegin() const noexcept {
			return const_reverse_iterator(end());
		}

		_NODISCARD reverse_iterator rend() noexcept {
			return reverse_iterator(begin());
		}

		_NODISCARD const_reverse_iterator rend() const noexcept {
			return const_reverse_iterator(begin());
		}

		_NODISCARD const_iterator cbegin() const noexcept {
			return begin();
		}

		_NODISCARD const_iterator cend() const noexcept {
			return end();
		}

		_NODISCARD const_reverse_iterator crbegin() const noexcept {
			return rbegin();
		}

		_NODISCARD const_reverse_iterator crend() const noexcept {
			return rend();
		}

		_NODISCARD size_type size() const noexcept {
			return _Get_scary()->_Mysize;
		}

		_NODISCARD size_type max_size() const noexcept {
			return (_STD min)(
				static_cast<size_type>((numeric_limits<difference_type>::max)()), _Alnode_traits::max_size(_Getal()));
		}

		_NODISCARD bool empty() const noexcept {
			return size() == 0;
		}

		_NODISCARD allocator_type get_allocator() const noexcept {
			return static_cast<allocator_type>(_Getal());
		}

		_NODISCARD key_compare key_comp() const {
			return _Getcomp();
		}

		_NODISCARD value_compare value_comp() const {
			return value_compare(key_comp());
		}

		template <bool _Multi2 = _Multi, enable_if_t<!_Multi2, int> = 0>
		pair<iterator, bool> insert(const value_type& _Val) {
			const auto _Result = _Emplace(_Val);
			return { iterator(_Result.first, _Get_scary()), _Result.second };
		}

		template <bool _Multi2 = _Multi, enable_if_t<_Multi2, int> = 0>
		iterator insert(const value_type& _Val) {
			return iterator(_Emplace(_Val).first, _Get_scary());
		}

		template <bool _Multi2 = _Multi, enable_if_t<!_Multi2, int> = 0>
		pair<iterator, bool> insert(value_type&& _Val) {
			const auto _Result = _Emplace(_STD move(_Val));
			return { iterator(_Result.first, _Get_scary()), _Result.second };
		}

		template <bool _Multi2 = _Multi, enable_if_t<_Multi2, int> = 0>
		iterator insert(value_type&& _Val) {
			return iterator(_Emplace(_STD move(_Val)).first, _Get_scary());
		}

		iterator insert(const_iterator _Where, const value_type& _Val) {
			const auto _Scary = _Get_scary();
		#if _ITERATOR_DEBUG_LEVEL == 2
			_STL_VERIFY(_Where._Getcont() == _Scary, "map/set insert iterator outside range");
		#endif // _ITERATOR_DEBUG_LEVEL == 2
			return iterator(_Emplace_hint(_Where._Ptr, _Val), _Scary);
		}

		iterator insert(const_iterator _Where, value_type&& _Val) {
			const auto _Scary = _Get_scary();
		#if _ITERATOR_DEBUG_LEVEL == 2
			_STL_VERIFY(_Where._Getcont() == _Scary, "map/set insert iterator outside range");
		#endif // _ITERATOR_DEBUG_LEVEL == 2
			return iterator(_Emplace_hint(_Where._Ptr, _STD move(_Val)), _Scary);
		}

		template <class _Iter>
		void insert(_Iter _First, _Iter _Last) {
			_Adl_verify_range(_First, _Last);
			auto _UFirst = _Get_unwrapped(_First);
			const auto _ULast = _Get_unwrapped(_Last);
			const auto _Myhead = _Get_scary()->_Myhead;
			for (; _UFirst != _ULast; ++_UFirst) {
				_Emplace_hint(_Myhead, *_UFirst);
			}
		}

		void insert(initializer_list<value_type> _Ilist) {
			insert(_Ilist.begin(), _Ilist.end());
		}

	private:
		_Nodeptr _Erase_unchecked(_Unchecked_const_iterator _Where) noexcept {
			const auto _Scary = _Get_scary();
			_Unchecked_const_iterator _Successor = _Where;
			++_Successor; // save successor iterator for return
			_Nodeptr _Erasednode = _Scary->_Extract(_Where); // node to erase
			_Scary->_Orphan_ptr(_Erasednode);
			_Node::_Freenode(_Getal(), _Erasednode); // delete erased node
			return _Successor._Ptr; // return successor nodeptr
		}

		_Nodeptr _Erase_unchecked(_Unchecked_const_iterator _First, _Unchecked_const_iterator _Last) noexcept {
			const auto _Begin = _Unchecked_begin();
			if (_First == _Begin && _Last._Ptr->_Isnil) {
				// erase all
				clear();
				return _Last._Ptr;
			}

			// partial erase, one at a time
			while (_First != _Last) {
				_Erase_unchecked(_First++);
			}

			return _Last._Ptr;
		}

	public:
		template <class _Iter = iterator, enable_if_t<!is_same_v<_Iter, const_iterator>, int> = 0>
		iterator erase(iterator _Where) noexcept /* strengthened */ {
			const auto _Scary = _Get_scary();
		#if _ITERATOR_DEBUG_LEVEL == 2
			_STL_VERIFY(_Where._Getcont() == _Scary, "map/set erase iterator from incorrect container");
			_STL_VERIFY(!_Where._Ptr->_Isnil, "cannot erase map/set end() iterator");
		#endif // _ITERATOR_DEBUG_LEVEL == 2
			return iterator(_Erase_unchecked(_Where._Unwrapped()), _Scary);
		}

		iterator erase(const_iterator _Where) noexcept /* strengthened */ {
			const auto _Scary = _Get_scary();
		#if _ITERATOR_DEBUG_LEVEL == 2
			_STL_VERIFY(_Where._Getcont() == _Scary, "map/set erase iterator from incorrect container");
			_STL_VERIFY(!_Where._Ptr->_Isnil, "cannot erase map/set end() iterator");
		#endif // _ITERATOR_DEBUG_LEVEL == 2
			return iterator(_Erase_unchecked(_Where._Unwrapped()), _Scary);
		}

		iterator erase(const_iterator _First, const_iterator _Last) noexcept /* strengthened */ {
			return iterator(_Erase_unchecked(_First._Unwrapped(), _Last._Unwrapped()), _Get_scary());
		}

		size_type erase(const key_type& _Keyval) noexcept(noexcept(_Eqrange(_Keyval))) /* strengthened */ {
			const auto _Where = _Eqrange(_Keyval);
			const _Unchecked_const_iterator _First(_Where.first, nullptr);
			const _Unchecked_const_iterator _Last(_Where.second, nullptr);
			const auto _Num = static_cast<size_type>(_STD distance(_First, _Last));
			_Erase_unchecked(_First, _Last);
			return _Num;
		}

		void clear() noexcept {
			const auto _Scary = _Get_scary();
			_Scary->_Orphan_ptr(nullptr);
			auto _Head = _Scary->_Myhead;
			_Scary->_Erase_tree(_Getal(), _Head->_Parent);
			_Head->_Parent = _Head;
			_Head->_Left = _Head;
			_Head->_Right = _Head;
			_Scary->_Mysize = 0;
		}

	private:
		template <class _Other>
		_NODISCARD _Nodeptr _Find(const _Other& _Keyval) const {
			const _Tree_find_result<_Nodeptr> _Loc = _Find_lower_bound(_Keyval);
			if (_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
				return _Loc._Bound;
			}

			return _Get_scary()->_Myhead;
		}

	public:
		_NODISCARD iterator find(const key_type& _Keyval) {
			return iterator(_Find(_Keyval), _Get_scary());
		}

		_NODISCARD const_iterator find(const key_type& _Keyval) const {
			return const_iterator(_Find(_Keyval), _Get_scary());
		}

		template <class _Other, class _Mycomp = key_compare, class = typename _Mycomp::is_transparent>
		_NODISCARD iterator find(const _Other& _Keyval) {
			return iterator(_Find(_Keyval), _Get_scary());
		}

		template <class _Other, class _Mycomp = key_compare, class = typename _Mycomp::is_transparent>
		_NODISCARD const_iterator find(const _Other& _Keyval) const {
			return const_iterator(_Find(_Keyval), _Get_scary());
		}

	#if _HAS_CXX20
		_NODISCARD bool contains(const key_type& _Keyval) const {
			return _Lower_bound_duplicate(_Find_lower_bound(_Keyval)._Bound, _Keyval);
		}

		template <class _Other, class _Mycomp = key_compare, class = typename _Mycomp::is_transparent>
		_NODISCARD bool contains(const _Other& _Keyval) const {
			return _Lower_bound_duplicate(_Find_lower_bound(_Keyval)._Bound, _Keyval);
		}
	#endif // _HAS_CXX20

		_NODISCARD size_type count(const key_type& _Keyval) const {
			if constexpr (_Multi) {
				const auto _Ans = _Eqrange(_Keyval);
				return static_cast<size_type>(_STD distance(
					_Unchecked_const_iterator(_Ans.first, nullptr), _Unchecked_const_iterator(_Ans.second, nullptr)));
			}
			else {
				return _Lower_bound_duplicate(_Find_lower_bound(_Keyval)._Bound, _Keyval);
			}
		}

		template <class _Other, class _Mycomp = key_compare, class = typename _Mycomp::is_transparent>
		_NODISCARD size_type count(const _Other& _Keyval) const {
			const auto _Ans = _Eqrange(_Keyval);
			return static_cast<size_type>(_STD distance(
				_Unchecked_const_iterator(_Ans.first, nullptr), _Unchecked_const_iterator(_Ans.second, nullptr)));
		}

		_NODISCARD iterator lower_bound(const key_type& _Keyval) {
			return iterator(_Find_lower_bound(_Keyval)._Bound, _Get_scary());
		}

		_NODISCARD const_iterator lower_bound(const key_type& _Keyval) const {
			return const_iterator(_Find_lower_bound(_Keyval)._Bound, _Get_scary());
		}

		template <class _Other, class _Mycomp = key_compare, class = typename _Mycomp::is_transparent>
		_NODISCARD iterator lower_bound(const _Other& _Keyval) {
			return iterator(_Find_lower_bound(_Keyval)._Bound, _Get_scary());
		}

		template <class _Other, class _Mycomp = key_compare, class = typename _Mycomp::is_transparent>
		_NODISCARD const_iterator lower_bound(const _Other& _Keyval) const {
			return const_iterator(_Find_lower_bound(_Keyval)._Bound, _Get_scary());
		}

		_NODISCARD iterator upper_bound(const key_type& _Keyval) {
			return iterator(_Find_upper_bound(_Keyval)._Bound, _Get_scary());
		}

		_NODISCARD const_iterator upper_bound(const key_type& _Keyval) const {
			return const_iterator(_Find_upper_bound(_Keyval)._Bound, _Get_scary());
		}

		template <class _Other, class _Mycomp = key_compare, class = typename _Mycomp::is_transparent>
		_NODISCARD iterator upper_bound(const _Other& _Keyval) {
			return iterator(_Find_upper_bound(_Keyval)._Bound, _Get_scary());
		}

		template <class _Other, class _Mycomp = key_compare, class = typename _Mycomp::is_transparent>
		_NODISCARD const_iterator upper_bound(const _Other& _Keyval) const {
			return const_iterator(_Find_upper_bound(_Keyval)._Bound, _Get_scary());
		}

		_NODISCARD pair<iterator, iterator> equal_range(const key_type& _Keyval) {
			const auto _Result = _Eqrange(_Keyval);
			const auto _Scary = _Get_scary();
			return { iterator(_Result.first, _Scary), iterator(_Result.second, _Scary) };
		}

		_NODISCARD pair<const_iterator, const_iterator> equal_range(const key_type& _Keyval) const {
			const auto _Result = _Eqrange(_Keyval);
			const auto _Scary = _Get_scary();
			return { const_iterator(_Result.first, _Scary), const_iterator(_Result.second, _Scary) };
		}

		template <class _Other, class _Mycomp = key_compare, class = typename _Mycomp::is_transparent>
		_NODISCARD pair<iterator, iterator> equal_range(const _Other& _Keyval) {
			const auto _Result = _Eqrange(_Keyval);
			const auto _Scary = _Get_scary();
			return { iterator(_Result.first, _Scary), iterator(_Result.second, _Scary) };
		}

		template <class _Other, class _Mycomp = key_compare, class = typename _Mycomp::is_transparent>
		_NODISCARD pair<const_iterator, const_iterator> equal_range(const _Other& _Keyval) const {
			const auto _Result = _Eqrange(_Keyval);
			const auto _Scary = _Get_scary();
			return { const_iterator(_Result.first, _Scary), const_iterator(_Result.second, _Scary) };
		}

		void swap(_Tree& _Right) noexcept(_Is_nothrow_swappable<key_compare>::value) /* strengthened */ {
			if (this != _STD addressof(_Right)) {
				_Pocs(_Getal(), _Right._Getal());
				_Swap_val_excluding_comp(_Right);
				_Swap_adl(_Getcomp(), _Right._Getcomp());
			}
		}

	protected:
		template <class _Keyty>
		_Tree_find_hint_result<_Nodeptr> _Find_hint(const _Nodeptr _Hint, const _Keyty& _Keyval) const {
			const auto& _Comp = _Getcomp();
			const auto _Head = _Get_scary()->_Myhead;
			if constexpr (_Multi) { // insert even if duplicate
				if (_Hint->_Isnil) {
					// insert at end if greater than or equal to last element
					if (_Head->_Parent->_Isnil || !_DEBUG_LT_PRED(_Comp, _Keyval, _Traits::_Kfn(_Head->_Right->_Myval))) {
						return { {_Head->_Right, _Tree_child::_Right}, false };
					}

					// _Hint is end(); it must be closer to the end of equivalents
					return { _Find_upper_bound(_Keyval)._Location, false };
				}

				if (_Hint == _Head->_Left) {
					// insert at beginning if less than or equal to first element
					if (!_DEBUG_LT_PRED(_Comp, _Traits::_Kfn(_Hint->_Myval), _Keyval)) {
						return { {_Hint, _Tree_child::_Left}, false };
					}

					// _Hint is begin(); it must be closer to the beginning of equivalents
					return { _Find_lower_bound(_Keyval)._Location, false };
				}

				if (!_DEBUG_LT_PRED(_Comp, _Traits::_Kfn(_Hint->_Myval), _Keyval)) {
					// _Val <= *_Hint
					const auto _Prev = (--(_Unchecked_const_iterator(_Hint, nullptr)))._Ptr;
					if (!_DEBUG_LT_PRED(_Comp, _Keyval, _Traits::_Kfn(_Prev->_Myval))) {
						// _Val <= *_Hint and _Val >= *prev(_Hint); insert here
						if (_Prev->_Right->_Isnil) {
							return { {_Prev, _Tree_child::_Right}, false };
						}
						else {
							return { {_Hint, _Tree_child::_Left}, false };
						}
					}

					// _Val goes before *_Hint; _Hint must be closer to the end of equivalents
					return { _Find_upper_bound(_Keyval)._Location, false };
				}

				// _Val goes after *_Hint; _Hint must be closer to the beginning of equivalents
				return { _Find_lower_bound(_Keyval)._Location, false };
			}
			else { // insert only if unique
				if (_Hint->_Isnil) { // insert at end if after last element
					// insert at end if greater than last element
					if (_Head->_Parent->_Isnil || _DEBUG_LT_PRED(_Comp, _Traits::_Kfn(_Head->_Right->_Myval), _Keyval)) {
						return { {_Head->_Right, _Tree_child::_Right}, false };
					}
				}
				else if (_Hint == _Head->_Left) {
					// insert at beginning if less than first element
					if (_DEBUG_LT_PRED(_Comp, _Keyval, _Traits::_Kfn(_Hint->_Myval))) {
						return { {_Hint, _Tree_child::_Left}, false };
					}
				}
				else if (_DEBUG_LT_PRED(_Comp, _Keyval, _Traits::_Kfn(_Hint->_Myval))) {
					// _Val < *_Hint
					const auto _Prev = (--(_Unchecked_const_iterator(_Hint, nullptr)))._Ptr;
					if (_DEBUG_LT_PRED(_Comp, _Traits::_Kfn(_Prev->_Myval), _Keyval)) {
						// _Val < *_Hint and _Val > *prev(_Hint); insert here
						if (_Prev->_Right->_Isnil) {
							return { {_Prev, _Tree_child::_Right}, false };
						}
						else {
							return { {_Hint, _Tree_child::_Left}, false };
						}
					}
				}
				else if (_DEBUG_LT_PRED(_Comp, _Traits::_Kfn(_Hint->_Myval), _Keyval)) {
					// _Val > *_Hint
					const auto _Next = (++(_Unchecked_const_iterator(_Hint, nullptr)))._Ptr;
					if (_Next->_Isnil || _DEBUG_LT_PRED(_Comp, _Keyval, _Traits::_Kfn(_Next->_Myval))) {
						// _Val > *_Hint and _Val < *next(_Hint); insert here
						if (_Hint->_Right->_Isnil) {
							return { {_Hint, _Tree_child::_Right}, false };
						}

						return { {_Next, _Tree_child::_Left}, false };
					}
				}
				else {
					// _Val is equivalent to *_Hint; don't insert
					return { {_Hint, _Tree_child::_Right}, true };
				}

				const auto _Loc = _Find_lower_bound(_Keyval);
				if (_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
					return { {_Loc._Bound, _Tree_child::_Unused}, true };
				}

				return { _Loc._Location, false };
			}
		}

		template <class _Keyty>
		_Tree_find_result<_Nodeptr> _Find_upper_bound(const _Keyty& _Keyval) const {
			const auto _Scary = _Get_scary();
			_Tree_find_result<_Nodeptr> _Result{ {_Scary->_Myhead->_Parent, _Tree_child::_Right}, _Scary->_Myhead };
			_Nodeptr _Trynode = _Result._Location._Parent;
			while (!_Trynode->_Isnil) {
				_Result._Location._Parent = _Trynode;
				if (_DEBUG_LT_PRED(_Getcomp(), _Keyval, _Traits::_Kfn(_Trynode->_Myval))) {
					_Result._Location._Child = _Tree_child::_Left;
					_Result._Bound = _Trynode;
					_Trynode = _Trynode->_Left;
				}
				else {
					_Result._Location._Child = _Tree_child::_Right;
					_Trynode = _Trynode->_Right;
				}
			}

			return _Result;
		}

		template <class _Keyty>
		bool _Lower_bound_duplicate(const _Nodeptr _Bound, const _Keyty& _Keyval) const {
			return !_Bound->_Isnil && !_DEBUG_LT_PRED(_Getcomp(), _Keyval, _Traits::_Kfn(_Bound->_Myval));
		}

		template <class _Keyty>
		_Tree_find_result<_Nodeptr> _Find_lower_bound(const _Keyty& _Keyval) const {
			const auto _Scary = _Get_scary();
			_Tree_find_result<_Nodeptr> _Result{ {_Scary->_Myhead->_Parent, _Tree_child::_Right}, _Scary->_Myhead };
			_Nodeptr _Trynode = _Result._Location._Parent;
			while (!_Trynode->_Isnil) {
				_Result._Location._Parent = _Trynode;
				if (_DEBUG_LT_PRED(_Getcomp(), _Traits::_Kfn(_Trynode->_Myval), _Keyval)) {
					_Result._Location._Child = _Tree_child::_Right;
					_Trynode = _Trynode->_Right;
				}
				else {
					_Result._Location._Child = _Tree_child::_Left;
					_Result._Bound = _Trynode;
					_Trynode = _Trynode->_Left;
				}
			}

			return _Result;
		}

		void _Check_grow_by_1() {
			if (max_size() == _Get_scary()->_Mysize) {
				_Throw_tree_length_error();
			}
		}

		template <class _Moveit>
		void _Copy(const _Tree& _Right, _Moveit _Movefl) { // copy or move entire tree from _Right
			const auto _Scary = _Get_scary();
			const auto _Right_scary = _Right._Get_scary();
			_Scary->_Myhead->_Parent = _Copy_nodes(_Right_scary->_Myhead->_Parent, _Scary->_Myhead, _Movefl);
			_Scary->_Mysize = _Right_scary->_Mysize;
			if (!_Scary->_Myhead->_Parent->_Isnil) { // nonempty tree, look for new smallest and largest
				_Scary->_Myhead->_Left = _Scary_val::_Min(_Scary->_Myhead->_Parent);
				_Scary->_Myhead->_Right = _Scary_val::_Max(_Scary->_Myhead->_Parent);
			}
			else { // empty tree, just tidy head pointers
				_Scary->_Myhead->_Left = _Scary->_Myhead;
				_Scary->_Myhead->_Right = _Scary->_Myhead;
			}
		}

		template <class _Ty, class _Is_set>
		_Nodeptr _Copy_or_move(_Ty& _Val, _Copy_tag, _Is_set) { // copy to new node
			return _Buynode(_Val);
		}

		template <class _Ty>
		_Nodeptr _Copy_or_move(_Ty& _Val, _Move_tag, true_type) { // move to new node -- set
			return _Buynode(_STD move(_Val));
		}

		template <class _Ty>
		_Nodeptr _Copy_or_move(_Ty& _Val, _Move_tag, false_type) { // move to new node -- map
			return _Buynode(_STD move(const_cast<key_type&>(_Val.first)), _STD move(_Val.second));
		}

		template <class _Moveit>
		_Nodeptr _Copy_nodes(_Nodeptr _Rootnode, _Nodeptr _Wherenode, _Moveit _Movefl) {
			// copy entire subtree, recursively
			const auto _Scary = _Get_scary();
			_Nodeptr _Newroot = _Scary->_Myhead; // point at nil node

			if (!_Rootnode->_Isnil) { // copy or move a node, then any subtrees
				bool_constant<is_same_v<key_type, value_type>> _Is_set;
				_Nodeptr _Pnode = _Copy_or_move(_Rootnode->_Myval, _Movefl, _Is_set);
				_Pnode->_Parent = _Wherenode;
				_Pnode->_Color = _Rootnode->_Color;
				if (_Newroot->_Isnil) {
					_Newroot = _Pnode; // memorize new root
				}

				_TRY_BEGIN
					_Pnode->_Left = _Copy_nodes(_Rootnode->_Left, _Pnode, _Movefl);
				_Pnode->_Right = _Copy_nodes(_Rootnode->_Right, _Pnode, _Movefl);
				_CATCH_ALL
					_Scary->_Erase_tree_and_orphan(_Getal(), _Newroot); // subtree copy failed, bail out
				_RERAISE;
				_CATCH_END
			}

			return _Newroot; // return newly constructed tree
		}

		template <class _Other>
		pair<_Nodeptr, _Nodeptr> _Eqrange(const _Other& _Keyval) const
			noexcept(_Nothrow_compare<key_compare, key_type, _Other>&& _Nothrow_compare<key_compare, _Other, key_type>) {
			// find range of nodes equivalent to _Keyval
			const auto _Scary = _Get_scary();
			const auto& _Comp = _Getcomp();
			_Nodeptr _Pnode = _Scary->_Myhead->_Parent;
			_Nodeptr _Lonode = _Scary->_Myhead; // end() if search fails
			_Nodeptr _Hinode = _Scary->_Myhead; // end() if search fails

			while (!_Pnode->_Isnil) {
				const auto& _Nodekey = _Traits::_Kfn(_Pnode->_Myval);
				if (_DEBUG_LT_PRED(_Comp, _Nodekey, _Keyval)) {
					_Pnode = _Pnode->_Right; // descend right subtree
				}
				else { // _Pnode not less than _Keyval, remember it
					if (_Hinode->_Isnil && _DEBUG_LT_PRED(_Comp, _Keyval, _Nodekey)) {
						_Hinode = _Pnode; // _Pnode greater, remember it
					}

					_Lonode = _Pnode;
					_Pnode = _Pnode->_Left; // descend left subtree
				}
			}

			_Pnode = _Hinode->_Isnil ? _Scary->_Myhead->_Parent : _Hinode->_Left; // continue scan for upper bound
			while (!_Pnode->_Isnil) {
				if (_DEBUG_LT_PRED(_Getcomp(), _Keyval, _Traits::_Kfn(_Pnode->_Myval))) {
					// _Pnode greater than _Keyval, remember it
					_Hinode = _Pnode;
					_Pnode = _Pnode->_Left; // descend left subtree
				}
				else {
					_Pnode = _Pnode->_Right; // descend right subtree
				}
			}

			return { _Lonode, _Hinode };
		}

	#if _HAS_CXX17
	public:
		using node_type = typename _Traits::node_type;

		node_type extract(const const_iterator _Where) {
			const auto _Scary = _Get_scary();
		#if _ITERATOR_DEBUG_LEVEL == 2
			_STL_VERIFY(_Where._Getcont() == _Scary && !_Where._Ptr->_Isnil, "map/set erase iterator outside range");
		#endif // _ITERATOR_DEBUG_LEVEL == 2

			const auto _Ptr = _Scary->_Extract(_Where._Unwrapped());
			_Scary->_Orphan_ptr(_Ptr);
			return node_type::_Make(_Ptr, _Getal());
		}

		node_type extract(const key_type& _Keyval) {
			const const_iterator _Where = find(_Keyval);
			if (_Where == end()) {
				return node_type{};
			}

			return extract(_Where);
		}

		auto insert(node_type&& _Handle) {
			if (_Handle.empty()) {
				if constexpr (_Multi) {
					return end();
				}
				else {
					return _Insert_return_type<iterator, node_type>{end(), false, {}};
				}
			}

			_Check_node_allocator(_Handle);
			const auto _Scary = _Get_scary();
			const auto _Attempt_node = _Handle._Getptr();
			const auto& _Keyval = _Traits::_Kfn(_Attempt_node->_Myval);
			_Tree_find_result<_Nodeptr> _Loc;
			if constexpr (_Multi) {
				_Loc = _Find_upper_bound(_Keyval);
			}
			else {
				_Loc = _Find_lower_bound(_Keyval);
				if (_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
					return _Insert_return_type<iterator, node_type>{
						iterator(_Loc._Bound, _Scary), false, _STD move(_Handle)};
				}
			}

			_Check_grow_by_1();

			// nothrow hereafter

			_Attempt_node->_Left = _Scary->_Myhead;
			// _Attempt_node->_Parent handled in _Insert_node
			_Attempt_node->_Right = _Scary->_Myhead;
			_Attempt_node->_Color = _Red;

			const auto _Inserted = _Scary->_Insert_node(_Loc._Location, _Handle._Release());
			if constexpr (_Multi) {
				return iterator(_Inserted, _Scary);
			}
			else {
				return _Insert_return_type<iterator, node_type>{iterator(_Inserted, _Scary), true, _STD move(_Handle)};
			}
		}

		iterator insert(const const_iterator _Hint, node_type&& _Handle) {
			const auto _Scary = _Get_scary();
		#if _ITERATOR_DEBUG_LEVEL == 2
			_STL_VERIFY(_Hint._Getcont() == _Scary, "map/set insert iterator outside range");
		#endif // _ITERATOR_DEBUG_LEVEL == 2
			if (_Handle.empty()) {
				return end();
			}

			_Check_node_allocator(_Handle);
			const auto _Attempt_node = _Handle._Getptr();
			const auto& _Keyval = _Traits::_Kfn(_Attempt_node->_Myval);
			const auto _Loc = _Find_hint(_Hint._Ptr, _Keyval);
			if (_Loc._Duplicate) {
				return iterator(_Loc._Location._Parent, _Scary);
			}

			_Check_grow_by_1();

			_Attempt_node->_Left = _Scary->_Myhead;
			// _Attempt_node->_Parent handled in _Insert_node
			_Attempt_node->_Right = _Scary->_Myhead;
			_Attempt_node->_Color = _Red;

			return iterator(_Scary->_Insert_node(_Loc._Location, _Handle._Release()), _Scary);
		}

		template <class>
		friend class _Tree;

		template <class _Other_traits>
		void merge(_Tree<_Other_traits>& _That) {
			static_assert(is_same_v<_Nodeptr, typename _Tree<_Other_traits>::_Nodeptr>,
				"merge() requires an argument with a compatible node type.");

			static_assert(is_same_v<allocator_type, typename _Tree<_Other_traits>::allocator_type>,
				"merge() requires an argument with the same allocator type.");

			if constexpr (is_same_v<_Tree, _Tree<_Other_traits>>) {
				if (this == _STD addressof(_That)) {
					return;
				}
			}

		#if _ITERATOR_DEBUG_LEVEL == 2
			if constexpr (!_Alnode_traits::is_always_equal::value) {
				_STL_VERIFY(_Getal() == _That._Getal(), "allocator incompatible for merge");
			}
		#endif // _ITERATOR_DEBUG_LEVEL == 2

			const auto _Scary = _Get_scary();
			const auto _Head = _Scary->_Myhead;
			const auto _That_scary = _That._Get_scary();
			auto _First = _That._Unchecked_begin();
			while (!_First._Ptr->_Isnil) {
				const auto _Attempt_node = _First._Ptr;
				++_First;
				_Tree_find_result<_Nodeptr> _Loc;
				const auto& _Keyval = _Traits::_Kfn(_Attempt_node->_Myval);
				if constexpr (_Multi) {
					_Loc = _Find_upper_bound(_Keyval);
				}
				else {
					_Loc = _Find_lower_bound(_Keyval);
					if (_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
						continue;
					}
				}

				_Check_grow_by_1();

				// nothrow hereafter for this iteration
				const auto _Extracted = _That_scary->_Extract(_Unchecked_const_iterator(_Attempt_node, nullptr));
				_Extracted->_Left = _Head;
				// _Extracted->_Parent handled in _Insert_node
				_Extracted->_Right = _Head;
				_Extracted->_Color = _Red;

				const auto _Inserted = _Scary->_Insert_node(_Loc._Location, _Extracted);
				_Reparent_ptr(_Inserted, _That);
			}
		}

		template <class _Other_traits>
		void merge(_Tree<_Other_traits>&& _That) {
			static_assert(is_same_v<_Nodeptr, typename _Tree<_Other_traits>::_Nodeptr>,
				"merge() requires an argument with a compatible node type.");

			static_assert(is_same_v<allocator_type, typename _Tree<_Other_traits>::allocator_type>,
				"merge() requires an argument with the same allocator type.");

			merge(_That);
		}

	protected:
		template <class _Other_traits>
		void _Reparent_ptr(const _Nodeptr _Ptr, _Tree<_Other_traits>& _Old_parent) {
			(void)_Ptr;
			(void)_Old_parent;
		#if _ITERATOR_DEBUG_LEVEL == 2
			_Lockit _Lock(_LOCK_DEBUG);
			const auto _Old_parent_scary = _Old_parent._Get_scary();
			_Iterator_base12** _Pnext = &_Old_parent_scary->_Myproxy->_Myfirstiter;
			_STL_VERIFY(_Pnext, "source container corrupted");
			if (_Ptr == nullptr || _Ptr == _Old_parent_scary->_Myhead) {
				return;
			}

			const auto _My_saved_proxy = _Get_scary()->_Myproxy;
			_Iterator_base12** const _My_saved_first = &_My_saved_proxy->_Myfirstiter;

			while (*_Pnext) {
				_Iterator_base12** const _Next = &(*_Pnext)->_Mynextiter;
				const auto _Iter = static_cast<const_iterator*>(*_Pnext);
				if (_Iter->_Ptr == _Ptr) { // reparent the iterator
					*_Pnext = *_Next;
					_Iter->_Myproxy = _My_saved_proxy;
					_Iter->_Mynextiter = *_My_saved_first;
					*_My_saved_first = _Iter;
				}
				else { // skip the iterator
					_Pnext = _Next;
				}
			}
		#endif // _ITERATOR_DEBUG_LEVEL == 2
		}

		void _Check_node_allocator(node_type& _Handle) const {
			(void)_Handle;
		#if _ITERATOR_DEBUG_LEVEL == 2
			_STL_VERIFY(get_allocator() == _Handle._Getal(), "node handle allocator incompatible for insert");
		#endif // _ITERATOR_DEBUG_LEVEL == 2
		}
	#endif // _HAS_CXX17

		void _Alloc_sentinel_and_proxy() {
			const auto _Scary = _Get_scary();
			auto&& _Alproxy = _GET_PROXY_ALLOCATOR(_Alnode, _Getal());
			_Container_proxy_ptr<_Alnode> _Proxy(_Alproxy, *_Scary);
			_Scary->_Myhead = _Node::_Buyheadnode(_Getal());
			_Proxy._Release();
		}

		template <class... _Valty>
		_Nodeptr _Buynode(_Valty&&... _Val) {
			return _Node::_Buynode(_Getal(), _Get_scary()->_Myhead, _STD forward<_Valty>(_Val)...);
		}

		key_compare& _Getcomp() noexcept {
			static key_compare comp;
			return comp; // _Mypair._Get_first();
		}

		const key_compare& _Getcomp() const noexcept {
			static key_compare comp;
			return comp; //  _Mypair._Get_first();
		}

		_Alnode& _Getal() noexcept {
			static _Alnode alnode;
			return alnode; // _Mypair._Myval2._Get_first();
		}

		const _Alnode& _Getal() const noexcept {
			static _Alnode alnode;
			return alnode; // _Mypair._Myval2._Get_first();
		}

		_Scary_val* _Get_scary() noexcept {
			return _STD addressof(scary);
		}

		const _Scary_val* _Get_scary() const noexcept {
			return _STD addressof(scary);
		}

	private:
		_Scary_val scary;
		//_Compressed_pair<key_compare, _Compressed_pair<_Alnode, _Scary_val>> _Mypair;
	};
}
