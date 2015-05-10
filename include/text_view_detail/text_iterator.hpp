#ifndef TEXT_VIEW_TEXT_ITERATOR_HPP // {
#define TEXT_VIEW_TEXT_ITERATOR_HPP


#include <text_view_detail/adl_customization.hpp>
#include <text_view_detail/concepts.hpp>
#include <iterator>
#include <origin/core/traits.hpp>


namespace std {
namespace experimental {
namespace text_view {


namespace detail {
/*
 * Decoder concep        Code unit iterator concept Text iterator category
 * -------------------   -------------------------- ---------------------------
 * Decoder               Input_iterator             input_iterator_tag
 * Decoder               Forward_iterator           forward_iterator_tag
 * Decoder               Bidirectional_iterator     forward_iterator_tag
 * Decoder               Random_access_iterator     forward_iterator_tag
 * Bidirectional_decoder Input_iterator             input_iterator_tag
 * Bidirectional_decoder Forward_iterator           forward_iterator_tag
 * Bidirectional_decoder Bidirectional_iterator     bidirectional_iterator_tag
 * Bidirectional_decoder Random_access_iterator     bidirectional_iterator_tag
 * Random_access_decoder Input_iterator             input_iterator_tag
 * Random_access_decoder Forward_iterator           forward_iterator_tag
 * Random_access_decoder Bidirectional_iterator     bidirectional_iterator_tag
 * Random_access_decoder Random_access_iterator     random_access_iterator_tag
 */
template<typename Codec, typename Iterator>
struct itext_iterator_category_selector;

template<Codec C, Code_unit_iterator CUIT>
struct itext_iterator_category_selector<C, CUIT> {
    using type = origin::Iterator_category<CUIT>;
};
template<Codec C, Code_unit_iterator CUIT>
requires origin::Bidirectional_iterator<CUIT>() // or Random_access_iterator
      && ! Bidirectional_decoder<C, CUIT>()     // and ! Random_access_decoder
struct itext_iterator_category_selector<C, CUIT> {
    using type = std::forward_iterator_tag;
};
template<Codec C, Code_unit_iterator CUIT>
requires origin::Random_access_iterator<CUIT>()
      && Bidirectional_decoder<C, CUIT>()
      && ! Random_access_decoder<C, CUIT>()
struct itext_iterator_category_selector<C, CUIT> {
    using type = std::bidirectional_iterator_tag;
};


template<Encoding ET, origin::Input_range RT>
struct itext_iterator_base
    : protected ET::codec_type::state_type
{
    using encoding_type = ET;
    using range_type = origin::Remove_reference<RT>;
    using state_type = typename encoding_type::codec_type::state_type;
    using iterator = origin::Iterator_type<const range_type>;
    using iterator_category =
              typename detail::itext_iterator_category_selector<
                  typename encoding_type::codec_type,
                  iterator>::type;
    using value_type = typename encoding_type::codec_type::character_type;
    using reference = const value_type&;
    using pointer = const value_type*;
    using difference_type = origin::Difference_type<iterator>;

protected:
    itext_iterator_base()
        requires origin::Default_constructible<state_type>()
        = default;

    itext_iterator_base(const state_type &state)
        // CWG DR1467.  List-initialization doesn't consider copy constructors
        // for aggregates.  The state_type base class must be initialized with
        // an expression-list.
        : state_type(state) {}

public:
    const state_type& state() const {
        return *this;
    }
    state_type& state() {
        return *this;
    }
};

template<Encoding ET, origin::Input_range RT>
requires Decoder<typename ET::codec_type, origin::Iterator_type<const RT>>()
      && origin::Input_iterator<origin::Iterator_type<const RT>>()
struct itext_iterator_data
    : public itext_iterator_base<ET, RT>
{
    using state_type = typename itext_iterator_base<ET, RT>::state_type;
    using range_type = typename itext_iterator_base<ET, RT>::range_type;
    using iterator = typename itext_iterator_base<ET, RT>::iterator;
    using value_type = typename itext_iterator_base<ET, RT>::value_type;

protected:
    itext_iterator_data()
        requires origin::Default_constructible<state_type>()
              && origin::Default_constructible<iterator>()
        : range{}, current{}, value{} {}

    itext_iterator_data(
        const state_type &state,
        const range_type *range,
        iterator current)
    :
        itext_iterator_base<ET, RT>{state},
        range{range},
        current{current}
    {}

    const range_type *range;
    iterator current;
    value_type value;

public:
    iterator base() const {
        return current;
    }
};

template<Encoding ET, origin::Input_range RT>
requires Decoder<typename ET::codec_type, origin::Iterator_type<const RT>>()
      && origin::Forward_iterator<origin::Iterator_type<const RT>>()
struct itext_iterator_data<ET, RT>
    : public itext_iterator_base<ET, RT>
{
    using state_type = typename itext_iterator_base<ET, RT>::state_type;
    using range_type = typename itext_iterator_base<ET, RT>::range_type;
    using iterator = typename itext_iterator_base<ET, RT>::iterator;
    using value_type = typename itext_iterator_base<ET, RT>::value_type;

protected:
    itext_iterator_data()
        requires origin::Default_constructible<state_type>()
              && origin::Default_constructible<iterator>()
        = default;

    itext_iterator_data(
        const state_type &state,
        const range_type *range,
        iterator first)
    :
        itext_iterator_base<ET, RT>{state},
        range{range},
        current_range{first, first}
    {
    }

    struct current_range_type {
        current_range_type()
            : first{}, last{} {}
        current_range_type(iterator first, iterator last)
            : first{first}, last{last} {}

        iterator begin() const { return first; }
        iterator end() const { return last; }

        iterator first;
        iterator last;
    };

    const range_type *range;
    current_range_type current_range;
    value_type value;

public:
    iterator base() const {
        return current_range.first;
    }

    const current_range_type& base_range() const {
        return current_range;
    }
};

} // namespace detail


template<Encoding ET, origin::Input_range RT>
requires Decoder<typename ET::codec_type, origin::Iterator_type<const RT>>()
struct itext_iterator
    : public detail::itext_iterator_data<ET, RT>
{
    using encoding_type = typename detail::itext_iterator_data<ET, RT>::encoding_type;
    using range_type = typename detail::itext_iterator_data<ET, RT>::range_type;
    using state_type = typename detail::itext_iterator_data<ET, RT>::state_type;
    using value_type = typename detail::itext_iterator_data<ET, RT>::value_type;
    using iterator_category = typename detail::itext_iterator_data<ET, RT>::iterator_category;
    using iterator = typename detail::itext_iterator_data<ET, RT>::iterator;
    using pointer = typename detail::itext_iterator_data<ET, RT>::pointer;
    using reference = typename detail::itext_iterator_data<ET, RT>::reference;
    using difference_type = typename detail::itext_iterator_data<ET, RT>::difference_type;

    itext_iterator()
        requires origin::Default_constructible<state_type>()
              && origin::Default_constructible<iterator>()
        = default;

    itext_iterator(
        const state_type &state,
        const range_type *range,
        iterator first)
    :
        detail::itext_iterator_data<ET, RT>{state, range, first}
    {
        ++*this;
    }

    reference operator*() const {
        return this->value;
    }
    pointer operator->() const {
        return &this->value;
    }

    bool operator==(const itext_iterator& other) const {
        return this->base() == other.base();
    }
    bool operator!=(const itext_iterator& other) const {
        return !(*this == other);
    }

    bool operator<(const itext_iterator& other) const
        requires Random_access_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        return (other - *this) > 0;
    }
    bool operator>(const itext_iterator& other) const
        requires Random_access_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        return other < *this;
    }
    bool operator<=(const itext_iterator& other) const
        requires Random_access_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        return !(*this > other);
    }
    bool operator>=(const itext_iterator& other) const
        requires Random_access_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        return !(*this < other);
    }

    // FIXME: Overloading of member operators based on constraints is currently
    // FIXME: rejected by r222769 of the gcc c++-concepts branch, so the
    // FIXME: operator++() definition below dispatches to next().
    // FIXME:   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66091
    itext_iterator& next(std::input_iterator_tag) {
        using codec_type = typename encoding_type::codec_type;

        iterator tmp_iterator{this->current};
        auto end(detail::adl_end(*this->range));
        while (tmp_iterator != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = codec_type::decode(
                this->state(),
                tmp_iterator,
                end,
                tmp_value,
                decoded_code_units);
            this->current = tmp_iterator;
            if (decoded_code_point) {
                this->value = tmp_value;
                break;
            }
        }
        return *this;
    }

    itext_iterator& next(std::forward_iterator_tag)
        requires Forward_decoder<typename encoding_type::codec_type,
                                 iterator>()
    {
        using codec_type = typename encoding_type::codec_type;

        this->current_range.first = this->current_range.last;
        iterator tmp_iterator{this->current_range.first};
        auto end(detail::adl_end(*this->range));
        while (tmp_iterator != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = codec_type::decode(
                this->state(),
                tmp_iterator,
                end,
                tmp_value,
                decoded_code_units);
            this->current_range.last = tmp_iterator;
            if (decoded_code_point) {
                this->value = tmp_value;
                break;
            }
            this->current_range.first = this->current_range.last;
        }
        return *this;
    }

    itext_iterator& operator++() {
        return next(iterator_category());
    }
    itext_iterator operator++(int) {
        itext_iterator it{*this};
        ++*this;
        return it;
    }

    itext_iterator& operator--()
        requires Bidirectional_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        using codec_type = typename encoding_type::codec_type;

        this->current_range.last = this->current_range.first;
        std::reverse_iterator<iterator> rcurrent{this->current_range.last};
        std::reverse_iterator<iterator> rend{detail::adl_begin(*this->range)};
        while (rcurrent != rend) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = codec_type::rdecode(
                this->state(),
                rcurrent,
                rend,
                tmp_value,
                decoded_code_units);
            this->current_range.first = rcurrent.base();
            if (decoded_code_point) {
                this->value = tmp_value;
                break;
            }
            this->current_range.last = this->current_range.first;
        }
        return *this;
    }
    itext_iterator operator--(int)
        requires Bidirectional_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        itext_iterator it{*this};
        --*this;
        return it;
    }

    itext_iterator& operator+=(difference_type n)
        requires Random_access_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        if (n < 0) {
            this->current_range.first +=
                ((n+1) * encoding_type::codec_type::max_code_units);
            --*this;
        } else if (n > 0) {
            this->current_range.last +=
                ((n-1) * encoding_type::codec_type::max_code_units);
            ++*this;
        }
        return *this;
    }

    itext_iterator operator+(difference_type n) const
        requires Random_access_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        itext_iterator it{*this};
        return it += n;
    }

    itext_iterator& operator-=(difference_type n)
        requires Random_access_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        return *this += -n;
    }

    itext_iterator operator-(difference_type n) const
        requires Random_access_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        itext_iterator it{*this};
        return it -= n;
    }

    difference_type operator-(itext_iterator it) const
        requires Random_access_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        return (this->current_range.first - it.current_range.first) /
               encoding_type::codec_type::max_code_units;
    }

    // Random access iterator requirements state that operator[] must return
    // a reference.  That isn't possible here since the reference would be to
    // a value stored in an object (The itext_iterator instance produced for
    // '*this + n') that is destroyed before the function returns.
    value_type operator[](difference_type n) const
        requires Random_access_decoder<typename encoding_type::codec_type,
                                       iterator>()
    {
        return *(*this + n);
    }
};

template<Encoding ET, origin::Input_range RT>
requires Random_access_decoder<typename ET::codec_type,
                               origin::Iterator_type<const RT>>()
itext_iterator<ET, RT> operator+(
    origin::Difference_type<origin::Iterator_type<const RT>> n,
    itext_iterator<ET, RT> it)
{
    return it += n;
}


template<Encoding ET, origin::Input_range RT>
struct itext_sentinel {
    using range_type = origin::Remove_reference<RT>;
    using sentinel = origin::Sentinel_type<RT>;

    itext_sentinel(sentinel s)
        : s{s} {}

    // This constructor is provided in lieu of a specialization of
    // std::common_type in order to satisfy the common type requirement for
    // cross-type equality comparison.  See N4382 19.2.5 "Concept Common",
    // N4382 19.3.2 "Concept EqualityComparable", and N3351 3.3
    // "Foundational Concepts".
    itext_sentinel(const itext_iterator<ET, RT> &ti)
        : s{ti.base()} {}

    bool operator==(const itext_sentinel& other) const {
        // Sentinels always compare equal regardless of any internal state.
        // See N4128, 10.1 "Sentinel Equality".
        return true;
    }
    bool operator!=(const itext_sentinel& other) const {
        return !(*this == other);
    }

    friend bool operator==(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    {
        return ti.base() == ts.base();
    }
    friend bool operator!=(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    {
        return !(ti == ts);
    }
    friend bool operator==(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    {
        return ti == ts;
    }
    friend bool operator!=(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    {
        return !(ts == ti);
    }

    bool operator<(const itext_sentinel& other) const {
        // Sentinels always compare equal regardless of any internal state.
        // See N4128, 10.1 "Sentinel Equality".
        return false;
    }
    bool operator>(const itext_sentinel& other) const {
        return other < *this;
    }
    bool operator<=(const itext_sentinel& other) const {
        return !(*this > other);
    }
    bool operator>=(const itext_sentinel& other) const {
        return !(*this < other);
    }

    friend bool operator<(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ti.base() < ts.base();
    }
    friend bool operator>(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ti.base() > ts.base();
    }
    friend bool operator<=(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ti.base() <= ts.base();
    }
    friend bool operator>=(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ti.base() >= ts.base();
    }

    friend bool operator<(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ts.base() < ti.base();
    }
    friend bool operator>(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ts.base() > ti.base();
    }
    friend bool operator<=(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ts.base() <= ti.base();
    }
    friend bool operator>=(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ts.base() >= ti.base();
    }

    sentinel base() const {
        return s;
    }

private:
    sentinel s;
};


template<Encoding E, Code_unit_iterator CUIT>
requires origin::Output_iterator<CUIT, typename E::codec_type::code_unit_type>()
struct otext_iterator
    : private E::codec_type::state_type
{
    using encoding_type = E;
    using state_type = typename E::codec_type::state_type;
    using state_transition_type = typename E::codec_type::state_transition_type;
    using iterator = CUIT;
    using iterator_category = std::output_iterator_tag;
    using value_type = typename encoding_type::codec_type::character_type;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = origin::Difference_type<iterator>;

    otext_iterator()
        requires origin::Default_constructible<state_type>()
        = default;

    otext_iterator(
        state_type state,
        iterator current)
    :
        // CWG DR1467.  List-initialization doesn't consider copy constructors
        // for aggregates.  The state_type base class must be initialized with
        // an expression-list.
        state_type(state),
        current{current}
    {}

    const state_type& state() const {
        return *this;
    }
    state_type& state() {
        return *this;
    }

    iterator base() const {
        return current;
    }

    otext_iterator& operator*() {
        return *this;
    }

    bool operator==(const otext_iterator& other) const {
        return current == other.current;
    }
    bool operator!=(const otext_iterator& other) const {
        return current != other.current;
    }

    otext_iterator& operator++() {
        return *this;
    }
    otext_iterator& operator++(int) {
        return *this;
    }

    otext_iterator& operator=(
        const state_transition_type &stt)
    {
        using codec_type = typename encoding_type::codec_type;
        iterator tmp{current};
        int encoded_code_units = 0;
        codec_type::encode_state_transition(state(), tmp, stt,
                                            encoded_code_units);
        current = tmp;
        return *this;
    }

    otext_iterator& operator=(
        const typename encoding_type::codec_type::character_type &value)
    {
        using codec_type = typename encoding_type::codec_type;
        iterator tmp{current};
        int encoded_code_units = 0;
        codec_type::encode(state(), tmp, value, encoded_code_units);
        current = tmp;
        return *this;
    }

protected:
    iterator current;
};


} // namespace text_view
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_TEXT_ITERATOR_HPP
