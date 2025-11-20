#ifndef FREE_BLOCK_MANAGER_HPP
#define FREE_BLOCK_MANAGER_HPP

#include <vector>
#include <cstdint>
#include <cstddef>

class FreeBlockManager {
private:
    std::vector<bool> blocks; // true = free, false = used
    uint64_t block_size_bytes = 0;

public:
    FreeBlockManager() = default;

    // initialize blocks
    void init(uint64_t total_blocks, uint64_t block_size);

    // allocation / free
    int allocate_block(); // returns block index or -1
    bool free_block(uint64_t index);
    bool is_free(uint64_t index) const;

    // Helpers used by FS stats & persistence
    uint64_t total_blocks() const;
    uint64_t used_blocks() const;
    uint64_t free_blocks() const;
    uint64_t block_size() const;

    // Serialize helpers
    std::vector<bool> to_vector_bool() const;
  void load_from_vector_bool(const std::vector<bool>& bits, uint64_t block_size);

};
#endif
