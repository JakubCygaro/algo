#include <cstdlib>
#include <datatypes.hpp>
#include <common.hpp>
#include <string>
#include <utility>

namespace dt {
    template<>
    struct Hash<int>{
        std::size_t operator()(const int& t){
            return t;
        }
    };
    template<>
    struct Hash<std::string>{
        std::size_t operator()(const std::string& s){
            const char* raw = s.c_str();

            std::size_t hash = 5381;
            int c;

            while((c = *raw++)){
                hash = ((hash << 5) + hash) + c;
            }
            return hash;
        }
    };
}

void test_remove(){
    srand(time(NULL));
    const auto size = common::get_random_in_range(5, 50);
    std::vector<std::pair<std::string, int>> arr(size);
    auto i = common::get_random_in_range(-1000, 1000);
    std::ranges::for_each(arr, [&](auto& v) {
            v.first = std::to_string(rand());
            v.second = i;
            i += common::get_random_in_range(1, 50);
        });
    std::ranges::shuffle(arr, std::mt19937 { std::random_device {}() });

    dt::HashMap<std::string, int> hm;

    std::ranges::for_each(arr, [&](auto& p) {
            assert(hm.insert({ p.first, std::move(p.second) }));
        });
    for(std::size_t i = 0; i < arr.size(); i++){
        assert(hm.contains(arr[i].first));
        assert(*hm.find(arr[i].first) == arr[i].second && "item with wrong value");
        assert(hm.remove(arr[i].first) && "failed to remove element");
        assert(!hm.remove(arr[i].first) && "object was not removed");
        assert(!hm.contains(arr[i].first));
    }
    assert(hm.empty());
}

void test_insert_and_find(){
    srand(time(NULL));
    const auto size = common::get_random_in_range(5, 50);
    std::vector<std::pair<std::string, int>> arr(size);
    auto i = common::get_random_in_range(-1000, 1000);
    std::ranges::for_each(arr, [&](auto& v) {
            v.first = std::to_string(rand());
            v.second = i;
            i += common::get_random_in_range(1, 50);
        });
    std::ranges::shuffle(arr, std::mt19937 { std::random_device {}() });

    dt::HashMap<std::string, int> hm;

    std::ranges::for_each(arr, [&](auto& p) {
            hm.insert({ p.first, std::move(p.second) });
        });
    for(std::size_t i = 0; i < arr.size(); i++){
        auto f = hm.find(arr[i].first);
        assert(f && "key not found");
        assert(*f == arr[i].second && "value for key incorrect");
        assert(hm.contains(arr[i].first));
    }
}

int main(void){
    for(auto i = 0; i < 100; i++){
        test_insert_and_find();
    }
    for(auto i = 0; i < 100; i++){
        test_remove();
    }
}
