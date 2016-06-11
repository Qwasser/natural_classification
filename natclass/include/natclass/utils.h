#include <map>
#include <iterator>

template <typename Iter>
class MapValueIterator : public std::iterator<std::bidirectional_iterator_tag,
        typename Iter::value_type::second_type> {

public:
    map_iterator() {}
    map_iterator(Iter j) : i(j) {}

    map_iterator& operator++() {
        ++i;
        return *this;
    }

    map_iterator operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    map_iterator& operator--() {
        --i;
        return *this;
    }

    map_iterator operator--(int) {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    bool operator==(map_iterator j) const {
        return i == j.i;
    }

    bool operator!=(map_iterator j) const {
        return !(*this == j);
    }

    reference operator*() {
        return i->second;
    }

    pointer operator->() {
        return &i->second;
    }

protected:
    Iter i;
};

template <typename Iter>
inline MapValueIterator<Iter> make_map_iterator(Iter j) {
    return map_iterator<Iter>(j);
}
