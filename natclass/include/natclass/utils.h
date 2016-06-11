#include <map>
#include <iterator>

template <typename Iter>
class MapValueIterator : public std::iterator<std::bidirectional_iterator_tag,
        typename Iter::value_type::second_type> {

public:
    MapValueIterator() {}
    MapValueIterator(Iter j) : i(j) {}

    MapValueIterator& operator++() {
        ++i;
        return *this;
    }

    MapValueIterator operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    MapValueIterator& operator--() {
        --i;
        return *this;
    }

    MapValueIterator operator--(int) {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    bool operator==(MapValueIterator j) const {
        return i == j.i;
    }

    bool operator!=(MapValueIterator j) const {
        return !(*this == j);
    }

    typename std::iterator<std::bidirectional_iterator_tag, typename Iter::value_type::second_type>::reference operator*() {
        return i->second;
    }

    typename std::iterator<std::bidirectional_iterator_tag, typename Iter::value_type::second_type>::pointer operator->() {
        return &i->second;
    }

protected:
    Iter i;
};

template <typename Iter>
inline MapValueIterator<Iter> make_map_iterator(Iter j) {
    return MapValueIterator<Iter>(j);
}
