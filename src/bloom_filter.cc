#include <array>
#include <common.hpp>
#include <datatypes.hpp>

namespace dt {
    template<>
    struct BloomHash<std::string, 3>{
        std::array<std::size_t, 3> operator()(const std::string& s) const noexcept {
            std::array<std::size_t, 3> ret;
            ret[0] = djb2(s);
            ret[1] = sdbm(s);
            ret[2] = loselose(s);

            return ret;
        }
    private:
        std::size_t djb2(const std::string& s) const noexcept {
            const char* str = s.c_str();
            unsigned long hash = 5381;
            int c;

            while ((c = *str++))
                hash = ((hash << 5) + hash) + c;

            return hash;
        }
        std::size_t sdbm(const std::string& s) const noexcept {
            const char* str = s.c_str();
            unsigned long hash = 0;
            int c;

            while ((c = *str++))
                hash = c + (hash << 6) + (hash << 16) - hash;

            return hash;
        }
        std::size_t loselose(const std::string& s) const noexcept {
            const char* str = s.c_str();
            unsigned int hash = 0;
            int c;

            while ((c = *str++))
                hash += c;

            return hash;
        }
    };
}

void test_bloom_filter(){
    dt::HashMap<std::size_t, bool> map;
    dt::BloomHash<std::string, 3> bloom_hasher;
    dt::BloomFilter<std::string, 3> bloom_filter(1024);

    srand(time(NULL));
    const auto size = common::get_random_in_range(5, 50);
    std::vector<std::pair<std::string, bool>> arr(size);
    auto i = common::get_random_in_range(-1000, 1000);
    std::ranges::for_each(arr, [&](auto& v) {
            v.first = std::format("{:x}", rand());
            //this signifies if there is a false positive
            v.second = false;
            i += common::get_random_in_range(1, 50);
        });
    std::ranges::shuffle(arr, std::mt19937 { std::random_device {}() });

    for(auto& [k, v] : arr){
        auto hashes = bloom_hasher(k);
        v |= map.insert({hashes[0], true});
        v |= map.insert({hashes[1], true});
        v |= map.insert({hashes[2], true});
        bloom_filter.insert(k);
    }

    for(auto& [k, v] : arr){
        assert(bloom_filter.contains(k));
    }
}

int main(void){
    for(auto i = 0; i < 100; i++){

    }
}
