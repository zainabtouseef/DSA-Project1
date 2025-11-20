#include "../include/free_block_manager.hpp"
#include <algorithm>

void FreeBlockManager::init(uint64_t total_blocks, uint64_t block_size) {
    blocks.assign(static_cast<size_t>(total_blocks), true);
    block_size_bytes = block_size;
}

int FreeBlockManager::allocate_block() {
    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i]) {
            blocks[i] = false;
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool FreeBlockManager::free_block(uint64_t index) {
    if (index >= blocks.size()) return false;
    blocks[index] = true;
    return true;
}

bool FreeBlockManager::is_free(uint64_t index) const {
    if (index >= blocks.size()) return false;
    return blocks[index];
}

uint64_t FreeBlockManager::total_blocks() const {
    return blocks.size();
}

uint64_t FreeBlockManager::used_blocks() const {
    uint64_t used = 0;
    for (bool b : blocks) if (!b) ++used;
    return used;
}

uint64_t FreeBlockManager::free_blocks() const {
    return total_blocks() - used_blocks();
}

uint64_t FreeBlockManager::block_size() const {
    return block_size_bytes;
}

std::vector<bool> FreeBlockManager::to_vector_bool() const {
    return blocks;
}

void FreeBlockManager::load_from_vector_bool(const std::vector<bool>& bits, uint64_t block_size) {
    blocks = bits;             // assign bits to the actual member
    block_size_bytes = block_size; // assign block_size to correct member
}
