#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <tuple>
#include <vector>
namespace dt {
    namespace {
        template <typename T>
        inline bool a_less_b_fn(const T& a, const T& b){
            return a < b;
        }
        template <typename T>
        inline bool a_more_b_fn(const T& a, const T& b){
            return !a_less_b_fn<T>(a, b);
        }
    }
    template <typename Key, typename T, bool(*CompareFunc)(const Key&, const Key&)>
    class Heap {
    private:
        struct Cell {
            Key key;
            T value;
            Cell(){};
            Cell(Key key, T value) : key(key), value(value){};
            Cell(std::tuple<Key, T> tup) : key(std::get<0>(tup)), value(std::get<1>(tup)){}
        };
        Cell* m_data;
        std::size_t m_cap;
        std::size_t m_size;

        inline static constexpr unsigned int GROW_FACTOR = 2;
        inline static constexpr unsigned int SHRINK_FACTOR = 2;

    public:
    Heap() : m_cap(1), m_size(0) {
        m_data = new Cell[m_cap];
    }
    explicit Heap(std::size_t capacity) : m_cap(capacity), m_size(0) {
        m_data = new Cell[m_cap];
    }
    Heap(const Heap<Key, T, CompareFunc>& other) : m_cap(other.m_cap), m_size(other.m_size) {
        m_data = new Cell[m_cap];
        std::memcpy(m_data, other.m_data, m_cap);
    }
    Heap& operator=(const Heap<Key, T, CompareFunc>& other) {
        m_cap = other.m_cap;
        m_size = other.m_size;
        m_data = new Cell[m_cap];
        std::memcpy(m_data, other.m_data, m_cap);
        return *this;
    }
    Heap(Heap<Key, T, CompareFunc>&& other) : m_cap(other.m_cap), m_size(other.m_size) {
        m_data = other.m_data;
        other.m_size = 0;
        other.m_cap = 0;
        other.m_data = nullptr;
    }
    Heap& operator=(Heap<Key, T, CompareFunc>&& other) {
        m_cap = other.m_cap;
        m_size = other.m_size;
        m_data = other.m_data;
        other.m_size = 0;
        other.m_cap = 0;
        other.m_data = nullptr;
        return *this;
    }
    ~Heap() {
        if(m_cap != 0){
            delete[] m_data;
        }
    }
    private:
        inline void grow() {
            m_cap = m_cap * GROW_FACTOR;
            auto tmp = new Cell[m_cap];
            std::memcpy(tmp, m_data, m_size * sizeof(Cell));
            delete[] m_data;
            m_data = tmp;
        }
        inline void shrink() {
            m_cap = std::clamp(m_cap / SHRINK_FACTOR, m_size, m_cap);
            auto tmp = new Cell[m_cap];
            std::memcpy(tmp, m_data, m_size);
            delete[] m_data;
            m_data = tmp;
        }
        inline std::optional<std::size_t> parent(std::size_t index) const {
            if (index == 0)
                return std::nullopt;
            if(index % 2 == 0) {
                index--;
            }
            return index / 2;
        }
        inline std::optional<std::size_t> left_child(std::size_t index) const{
            auto ret = index * 2 + 1;
            return ret < m_size ? std::optional<std::size_t>{ret} : std::nullopt;
        }
        inline std::optional<std::size_t>  right_child(std::size_t index) const{
            auto ret = index * 2 + 2;
            return ret < m_size ? std::optional<std::size_t>{ret} : std::nullopt;
        }
    public:
        inline bool empty() const {
            return m_size == 0;
        }
        inline std::size_t size() const {
            return m_size;
        }
        inline void insert(Key k, T elem) {
            if(++m_size > m_cap)
                grow();
            auto i = m_size - 1;
            m_data[i] = Cell{k, elem};
            // while(auto p = parent(i)) {
            //     if (CompareFunc(m_data[i].key, m_data[p.value()].key)){
            //         std::swap(m_data[p.value()], m_data[i]);
            //         i = p.value();
            //     } else {
            //         break;
            //     }
            // }
            while(auto j = shift_up(i)) {
                i = j.value();
            }
        }
        inline std::tuple<Key, T> extract() {
            auto ret = m_data[0];
            std::swap(m_data[0], m_data[--m_size]);
            std::size_t i = 0;
            while(auto j = shift_down(i)) {
                i = j.value();
            }
            // while(true ) {
            //     auto lc = left_child(i);
            //     auto rc = right_child(i);
            //     if (lc && rc) {
            //         auto min_idx = CompareFunc(m_data[lc.value()].key, m_data[rc.value()].key) ? lc.value() : rc.value();
            //         if (CompareFunc(m_data[min_idx].key, m_data[i].key)) {
            //             std::swap(m_data[min_idx], m_data[i]);
            //             i = min_idx;
            //         } else {
            //             break;
            //         }
            //     } else if (lc) {
            //         if (CompareFunc(m_data[lc.value()].key, m_data[i].key)) {
            //             std::swap(m_data[lc.value()], m_data[i]);
            //             i = lc.value();
            //         } else {
            //             break;
            //         }
            //     } else if (rc) {
            //         if (CompareFunc(m_data[rc.value()].key, m_data[i].key)) {
            //             std::swap(m_data[rc.value()], m_data[i]);
            //             i = rc.value();
            //         } else {
            //             break;
            //         }
            //     } else {
            //         break;
            //     }
            // }
            return std::make_tuple(ret.key, ret.value);
        }
        inline T& top() const {
            return m_data[0];
        }
        inline bool delete_item(T* elem) {

        }
        /// if a shift up has occured, return the new position of i
        inline std::optional<std::size_t> shift_up(std::size_t i) {
            if(auto p = parent(i); p.has_value() && CompareFunc(m_data[i].key, m_data[p.value()].key)) {
                std::swap(m_data[i], m_data[p.value()]);
                return p.value();
            }
            return std::nullopt;
        }
        inline std::optional<std::size_t> shift_down(std::size_t i) {
            auto lc = left_child(i);
            auto rc = right_child(i);
            if (lc && rc) {
                auto min_idx = CompareFunc(m_data[lc.value()].key, m_data[rc.value()].key) ? lc.value() : rc.value();
                if (CompareFunc(m_data[min_idx].key, m_data[i].key)) {
                    std::swap(m_data[min_idx], m_data[i]);
                    return min_idx;
                }
            } else if (lc.has_value() && CompareFunc(m_data[lc.value()].key, m_data[i].key)) {
                std::swap(m_data[lc.value()], m_data[i]);
                return lc.value();
            } else if (rc.has_value() && CompareFunc(m_data[rc.value()].key, m_data[i].key)) {
                std::swap(m_data[rc.value()], m_data[i]);
                return rc.value();
            }
            return std::nullopt;
        }
        inline static Heap<Key, T, CompareFunc> heapify(std::vector<std::tuple<Key, T>>&& arr){
            Heap<Key, T, CompareFunc> heap(arr.size());
            std::size_t i = 0;
            std::for_each(arr.begin(), arr.end(), [&](auto v){
                heap.m_data[i++] = v;
            });
            for(auto i = heap.m_size - 1; i >= 0; i--){
                heap.shift_up(i);
            }
            return heap;
        }
        inline static Heap<Key, T, CompareFunc> heapify(std::vector<Key>&& arr){
            Heap<Key, T, CompareFunc> heap(arr.size());
            heap.m_size = arr.size();
            std::size_t i = 0;
            std::for_each(arr.begin(), arr.end(), [&](auto v){
                heap.m_data[i++] = {v, T{}};
            });
            for(auto i = heap.m_size - 1; i != 0; i--){
                heap.shift_up(i);
            }
            return heap;
        }
    };
    template <typename T>
    using MinHeap = Heap<int, T, a_less_b_fn<T>>;
    template <typename T>
    using MaxHeap = Heap<int, T, a_more_b_fn<T>>;

}
