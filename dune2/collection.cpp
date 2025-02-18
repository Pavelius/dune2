#include <algorithm>
#include <cassert>
#include <cstring>
#include <functional>
#include <random>
#include <string>
#include <vector>

#include "array.h"  // Assuming this exists for source array type
#include "stringbuilder.h"  // Assuming this exists for szcmp

// Function pointer types for better type safety
using GetNameFn = const char*(*)(const void*);
using VisibleFn = bool(*)(const void*);
using AllowFn = bool(*)(const void*, int);
using CompareFn = int(*)(const void*, const void*);
using GroupFn = void*(*)(void*);

// Modernized collection class
class Collection {
public:
    // Constructors
    explicit Collection(size_t initial_capacity = 0) {
        if (initial_capacity > 0) {
            data_.reserve(initial_capacity);
        }
    }

    // Rule of Five
    Collection(const Collection& other) = default;
    Collection(Collection&& other) noexcept = default;
    Collection& operator=(const Collection& other) = default;
    Collection& operator=(Collection&& other) noexcept = default;
    ~Collection() = default;

    // Capacity management
    [[nodiscard]] size_t size() const noexcept { return data_.size(); }
    [[nodiscard]] size_t capacity() const noexcept { return data_.capacity(); }
    [[nodiscard]] bool empty() const noexcept { return data_.empty(); }
    void reserve(size_t new_capacity) { data_.reserve(new_capacity); }

    // Element access
    void* operator[](size_t index) const {
        assert(index < size());
        return data_[index];
    }
    void* at(size_t index) const {
        if (index >= size()) throw std::out_of_range("Collection index out of range");
        return data_[index];
    }
    void* first() const noexcept { return empty() ? nullptr : data_[0]; }

    // Modifiers
    void add(void* object) { 
        if (object) data_.push_back(object); 
    }
    void add(const Collection& source) {
        data_.insert(data_.end(), source.data_.begin(), source.data_.end());
    }
    
    void insert(size_t index, void* object) {
        if (index > size() || !object) return;
        data_.insert(data_.begin() + index, object);
    }

    void remove(size_t index, size_t count = 1) {
        if (index >= size()) return;
        count = std::min(count, size() - index);
        data_.erase(data_.begin() + index, data_.begin() + index + count);
    }

    void* pick() {
        if (empty()) return nullptr;
        void* result = data_[0];
        remove(0, 1);
        return result;
    }

    void clear() noexcept { data_.clear(); }
    void top(size_t number) { 
        if (size() > number) data_.resize(number); 
    }

    // Search and filtering
    [[nodiscard]] int find(void* object) const {
        auto it = std::find(data_.begin(), data_.end(), object);
        return (it == data_.end()) ? -1 : static_cast<int>(it - data_.begin());
    }

    void distinct() {
        auto last = std::unique(data_.begin(), data_.end());
        data_.erase(last, data_.end());
    }

    void group(GroupFn proc) {
        if (!proc) return;
        std::transform(data_.begin(), data_.end(), data_.begin(), proc);
        distinct();
    }

    // Selection methods
    void select(const Array& source) {
        data_.clear();
        data_.reserve(source.size() / source.element_size());
        for (auto* p = source.begin(); p < source.end(); p += source.element_size()) {
            data_.push_back(p);
        }
    }

    void select(const Array& source, VisibleFn proc, bool keep = true) {
        data_.clear();
        data_.reserve(source.size() / source.element_size());
        for (auto* p = source.begin(); p < source.end(); p += source.element_size()) {
            if (proc && proc(p) == keep) {
                data_.push_back(p);
            }
        }
    }

    void select(const Array& source, AllowFn proc, int param, bool keep = true) {
        data_.clear();
        data_.reserve(source.size() / source.element_size());
        for (auto* p = source.begin(); p < source.end(); p += source.element_size()) {
            if (proc && proc(p, param) == keep) {
                data_.push_back(p);
            }
        }
    }

    // Matching methods
    void match(VisibleFn proc, bool keep = true) {
        auto new_end = std::partition(data_.begin(), data_.end(),
            [proc, keep](void* p) { return proc && proc(p) == keep; });
        data_.erase(new_end, data_.end());
    }

    void match(const Collection& source, bool keep = true) {
        auto new_end = std::partition(data_.begin(), data_.end(),
            [&source, keep](void* p) { return (source.find(p) != -1) == keep; });
        data_.erase(new_end, data_.end());
    }

    void match(AllowFn proc, int param, bool keep = true) {
        auto new_end = std::partition(data_.begin(), data_.end(),
            [proc, param, keep](void* p) { return proc && proc(p, param) == keep; });
        data_.erase(new_end, data_.end());
    }

    // Sorting and randomization
    void sort(GetNameFn proc) {
        if (!proc) return;
        std::sort(data_.begin(), data_.end(), 
            [proc](void* a, void* b) { return szcmp(proc(a), proc(b)) < 0; });
    }

    void sort(CompareFn proc) {
        if (!proc) return;
        std::sort(data_.begin(), data_.end(), 
            [proc](void* a, void* b) { return proc(a, b) < 0; });
    }

    void shuffle() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::shuffle(data_.begin(), data_.end(), gen);
    }

    void* random() const {
        if (empty()) return nullptr;
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, size() - 1);
        return data_[dis(gen)];
    }

    // Iterators
    auto begin() noexcept { return data_.begin(); }
    auto end() noexcept { return data_.end(); }
    auto begin() const noexcept { return data_.begin(); }
    auto end() const noexcept { return data_.end(); }

private:
    std::vector<void*> data_;
};

// Legacy compatibility (if needed)
using collectiona = Collection;
