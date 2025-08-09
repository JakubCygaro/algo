#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstring>
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
    template <typename T, bool(*F)(const T&, const T&)>
    class Heap {
    private:
        T* m_data;
        std::size_t m_cap;
        std::size_t m_size;

        inline static constexpr unsigned int GROW_FACTOR = 2;
        inline static constexpr unsigned int SHRINK_FACTOR = 2;
    public:
    Heap() : m_cap(1), m_size(0) {
        m_data = new T[m_cap];
    }
    explicit Heap(std::size_t capacity) : m_cap(capacity), m_size(0) {
        m_data = new T[m_cap];
    }
    Heap(const Heap<T, F>& other) : m_cap(other.m_cap), m_size(other.m_size) {
        m_data = new T[m_cap];
        std::memcpy(m_data, other.m_data, m_cap);
    }
    Heap& operator=(const Heap<T, F>& other) {
        m_cap = other.m_cap;
        m_size = other.m_size;
        m_data = new T[m_cap];
        std::memcpy(m_data, other.m_data, m_cap);
        return *this;
    }
    Heap(Heap<T, F>&& other) : m_cap(other.m_cap), m_size(other.m_size) {
        m_data = other.m_data;
        other.m_size = 0;
        other.m_cap = 0;
        other.m_data = nullptr;
    }
    Heap& operator=(Heap<T, F>&& other) {
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
            auto tmp = new T[m_cap];
            std::memcpy(tmp, m_data, m_size * sizeof(T));
            delete[] m_data;
            m_data = tmp;
        }
        inline void shrink() {
            m_cap = std::clamp(m_cap / SHRINK_FACTOR, m_size, m_cap);
            auto tmp = new T[m_cap];
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
        inline void insert(T elem) {
            if(++m_size > m_cap)
                grow();
            auto i = m_size - 1;
            m_data[i] = elem;
            while(auto p = parent(i)) {
                if (F(m_data[i], m_data[p.value()])){
                    std::swap(m_data[p.value()], m_data[i]);
                    i = p.value();
                } else {
                    break;
                }
            }
        }
        inline T extract_min() {
            auto ret = m_data[0];
            std::swap(m_data[0], m_data[--m_size]);
            std::size_t i = 0;
            while(true ) {
                auto lc = left_child(i);
                auto rc = right_child(i);
                if (lc && rc) {
                    auto min_idx = F(m_data[lc.value()], m_data[rc.value()]) ? lc.value() : rc.value();
                    if (F(m_data[min_idx], m_data[i])) {
                        std::swap(m_data[min_idx], m_data[i]);
                        i = min_idx;
                    } else {
                        break;
                    }
                } else if (lc) {
                    if (F(m_data[lc.value()], m_data[i])) {
                        std::swap(m_data[lc.value()], m_data[i]);
                        i = lc.value();
                    } else {
                        break;
                    }
                } else if (rc) {
                    if (F(m_data[rc.value()], m_data[i])) {
                        std::swap(m_data[rc.value()], m_data[i]);
                        i = rc.value();
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
            return ret;
        }
    };
    template <typename T>
    using MinHeap = Heap<T, a_less_b_fn<T>>;
    template <typename T>
    using MaxHeap = Heap<T, a_more_b_fn<T>>;

}
