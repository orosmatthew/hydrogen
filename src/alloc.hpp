#pragma once

#include <cstddef>
#include <cstdlib>
#include <vector>

class Alloc {
public:
    Alloc()
        : m_current_block(0)
        , m_offset(0)
    {
        m_blocks.push_back((std::byte*)malloc(c_block_size));
    }

    void* alloc(size_t size)
    {
        if (m_offset + size > c_block_size) {
            m_blocks.push_back((std::byte*)malloc(c_block_size));
            m_current_block++;
            m_offset = 0;
        }
        void* ptr = m_blocks[m_current_block] + m_offset;
        m_offset += size;
        return ptr;
    }

    template <typename T>
    T* alloc()
    {
        return (T*)(alloc(sizeof(T)));
    }

    ~Alloc()
    {
        for (void* block : m_blocks) {
            free(block);
        }
    }

private:
    const size_t c_block_size = 1024 * 1024; // 1 mb
    std::vector<std::byte*> m_blocks;
    size_t m_current_block;
    size_t m_offset;
};
