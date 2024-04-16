#include <not_implemented.h>

#include "../include/allocator_boundary_tags.h"

using byte = unsigned char;

allocator_boundary_tags::~allocator_boundary_tags()
{
    debug_with_guard("Called allocator destructor");
    (*get_mutex()).~mutex();
    deallocate_with_guard(_trusted_memory);
}

allocator_boundary_tags::allocator_boundary_tags(
    allocator_boundary_tags const &other)
{
    throw not_implemented("allocator_boundary_tags::allocator_boundary_tags(allocator_boundary_tags const &)", "your code should be here...");
}

allocator_boundary_tags &allocator_boundary_tags::operator=(
    allocator_boundary_tags const &other)
{
    throw not_implemented("allocator_boundary_tags &allocator_boundary_tags::operator=(allocator_boundary_tags const &)", "your code should be here...");
}

allocator_boundary_tags::allocator_boundary_tags(
    allocator_boundary_tags &&other) noexcept
{
    throw not_implemented("allocator_boundary_tags::allocator_boundary_tags(allocator_boundary_tags &&) noexcept", "your code should be here...");
}

allocator_boundary_tags &allocator_boundary_tags::operator=(
    allocator_boundary_tags &&other) noexcept
{
    throw not_implemented("allocator_boundary_tags &allocator_boundary_tags::operator=(allocator_boundary_tags &&) noexcept", "your code should be here...");
}

allocator_boundary_tags::allocator_boundary_tags(
    size_t space_size,
    allocator *parent_allocator,
    logger *logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    size_t memory_size = space_size + _meta_size;
    void* memory;

    if (parent_allocator == nullptr)
    {
        try
        {
            memory = ::operator new(memory_size);
        } catch (const std::bad_alloc& ex)
        {
            error_with_guard("Bad memory allocation");
            throw;
        }
    }
    else
    {
        memory = parent_allocator->allocate(memory_size, 1);
    }
    _trusted_memory = memory;
    unsigned char* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);

    *reinterpret_cast<class logger**> (memory) = logger;
    ptr += sizeof(class logger*);

    *reinterpret_cast<allocator**>(ptr) = parent_allocator;
    ptr += sizeof(allocator*);

    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr) = allocate_fit_mode;
    ptr += sizeof(allocator_with_fit_mode::fit_mode);

    std::mutex* mut = reinterpret_cast<std::mutex*>(ptr);
    construct(mut);
    ptr += sizeof(std::mutex);

    *reinterpret_cast<size_t*>(ptr) = space_size;
    ptr += sizeof(size_t);

    *reinterpret_cast<void**>(ptr) = nullptr;
    ptr += sizeof(void*);

    debug_with_guard("Created " + get_typename());
    // meta  :: logger -> allocator -> fit_mode -> mutes -> size -> first_block
    // block :: alloc, size, left_ptr, right_ptr
}
inline void **allocator_boundary_tags::get_first_block_ptr() const noexcept
{
    byte *ptr = (reinterpret_cast<byte*>(_trusted_memory)) + _meta_size - sizeof(void *);
    return reinterpret_cast<void**>(ptr);
}
inline void* allocator_boundary_tags::get_first_block() const noexcept
{
    byte* ptr = (reinterpret_cast<byte*>(_trusted_memory)) + _meta_size;
    return reinterpret_cast<void*>(ptr);
}
[[nodiscard]] void *allocator_boundary_tags::allocate(
    size_t value_size,
    size_t values_count)
{
    size_t total_memory_size = value_size * values_count + _block_meta_size;
    trace_with_guard("Started allocate process of " + std::to_string(total_memory_size) + " bytes");

    std::lock_guard lock(*get_mutex());

    void** first_block = get_first_block_ptr();
    byte* ptr;

    if (*first_block == nullptr)
    {
        ptr = (reinterpret_cast<byte*>(_trusted_memory)) + _meta_size;
        size_t size = get_size();
        if (size < total_memory_size)
        {
            error_with_guard("Try to allocate " + 
                std::to_string(total_memory_size) + "from" + std::to_string(size));
            throw std::bad_alloc();
        }
     
        *first_block = ptr;
        return create_block_meta(reinterpret_cast<void*>(ptr), total_memory_size - _block_meta_size, nullptr, nullptr);
    }

    allocator_with_fit_mode::fit_mode mode = get_fit_mode();

    void* memory = nullptr;

    if (mode == allocator_with_fit_mode::fit_mode::first_fit)
    {
        memory = allocate_first_fit(*first_block, value_size * values_count);
    }
    else if (mode == allocator_with_fit_mode::fit_mode::the_best_fit)
    {
        memory = allocate_best_fit(*first_block, value_size * values_count);
    }
    else if (mode == allocator_with_fit_mode::fit_mode::the_worst_fit)
    {
        memory = allocate_worst_fit(*first_block, value_size * values_count);
    }
    if (memory != nullptr)
    {
        trace_with_guard("Succesfully allocated " + std::to_string(value_size * values_count) + " bytes with meta"+
        " with size " + std::to_string(_block_meta_size));
        return memory;
    }
    error_with_guard("Error with allocating " + std::to_string(value_size * values_count) + " bytes with meta" +
        " with size " + std::to_string(_block_meta_size));
    throw std::bad_alloc();
    //throw std::logic_error("i dont made this");
}
void* allocator_boundary_tags::allocate_first_fit(void* left_elem, size_t size)
{
    void* place_to_place = nullptr;
    void* that_block = nullptr;

    size_t actual_block_size = 0;
    bool at_start = false;

    void* next_block = get_next_existing_block(left_elem);
    void* block = left_elem;
    if (slide_block_for(_trusted_memory, _meta_size) != left_elem) // block at start
    {
        void* st_ptr = slide_block_for(_trusted_memory, _meta_size);
        if (reinterpret_cast<byte*> (st_ptr) - reinterpret_cast<byte*>(left_elem) >= _block_meta_size + size &&
            place_to_place == nullptr)
        {
            actual_block_size = get_block_distance(st_ptr, left_elem);
            that_block = left_elem;
            place_to_place = st_ptr;
            at_start = true;
        }
    }

    while (next_block != nullptr && !at_start) // search block in midle
    {
        size_t dt = get_block_distance(block, next_block);
        if (dt >= size + _block_meta_size)
        {
            actual_block_size = dt;
            that_block = block;
            place_to_place = slide_block_for(block, _block_meta_size + get_block_data_size(block));
            break;
        }
        block = next_block;
        next_block = get_next_existing_block(block);
    }
    size_t dt = (reinterpret_cast<byte*>(block) + get_block_data_size(block) + _block_meta_size) - (reinterpret_cast<byte*>(_trusted_memory) + _meta_size);

    if (!at_start && place_to_place == nullptr && get_size() - dt >= size + _block_meta_size) // block at end
    {
        actual_block_size = get_size() - dt;
        that_block = block;
        place_to_place = slide_block_for(block, _block_meta_size + get_block_data_size(block));
    }
    if (at_start)
    {
        *get_first_block_ptr() = place_to_place;
    }
    if (place_to_place == nullptr)
    {
        return nullptr;
    }
    
    byte* next_block_ = reinterpret_cast<byte*>(get_next_existing_block(that_block));
    if (next_block_ != nullptr)
    {
        *reinterpret_cast<void**>(next_block_ + sizeof(void*) + sizeof(size_t)) = place_to_place;
    }

    byte* prev_block = reinterpret_cast<byte*>(that_block);
    if (prev_block != nullptr)
    {
        *reinterpret_cast<void**>(prev_block + 2 * sizeof(void*) + sizeof(size_t)) = place_to_place;
    }
   
    return create_block_meta(place_to_place, size, that_block, reinterpret_cast<void*>(next_block));

}

void* allocator_boundary_tags::allocate_best_fit(void* left_elem, size_t size)
{
    void* place_to_place = nullptr;
    void* that_block = nullptr;

    size_t actual_block_size = get_size() + 1;
    bool at_start = false;

    void* next_block = get_next_existing_block(left_elem);
    void* block = left_elem;

    while (next_block != nullptr) // search block in midle
    {
        size_t dt = get_block_distance(block, next_block);
        if (dt < actual_block_size && dt >= size + _block_meta_size)
        {
            actual_block_size = dt;
            that_block = block;
            place_to_place = slide_block_for(block, _block_meta_size + get_block_data_size(block));
        }
        block = next_block;
        next_block = get_next_existing_block(block);
    }
    size_t dt = (reinterpret_cast<byte*>(block) + get_block_data_size(block) + _block_meta_size) - (reinterpret_cast<byte*>(_trusted_memory) + _meta_size);

    if (get_size() - dt >= size + _block_meta_size && get_size() - dt < actual_block_size + _block_meta_size) // block at end
    {
        actual_block_size = get_size() - dt;
        that_block = block;
        place_to_place = slide_block_for(block, _block_meta_size + get_block_data_size(block));
    }

    if (slide_block_for(_trusted_memory, _meta_size) != left_elem) // block at start
    {
        void* st_ptr = slide_block_for(_trusted_memory, _meta_size);
        if (get_block_distance(st_ptr, left_elem) >= _block_meta_size + size && 
            get_block_distance(st_ptr, left_elem) < actual_block_size &&
            place_to_place == nullptr)
        {
            actual_block_size = get_block_distance(st_ptr, left_elem);
            that_block = left_elem;
            place_to_place = st_ptr;
            at_start = true;
        }
    }
    
    if (at_start)
    {
        *get_first_block_ptr() = place_to_place;
    }
    if (place_to_place == nullptr)
    {
        return nullptr;
    }
    byte* next_block_ = reinterpret_cast<byte*>(get_next_existing_block(that_block));
    if (next_block_ != nullptr)
    {
        *reinterpret_cast<void**>(next_block_ + sizeof(void*) + sizeof(size_t)) = place_to_place;
    }

    byte* prev_block = reinterpret_cast<byte*>(that_block);
    if (prev_block != nullptr)
    {
        *reinterpret_cast<void**>(prev_block + 2 * sizeof(void*) + sizeof(size_t)) = place_to_place;
    }

    return create_block_meta(place_to_place, size, that_block, reinterpret_cast<void*>(next_block));
}

void* allocator_boundary_tags::allocate_worst_fit(void* left_elem, size_t size)
{
    void* place_to_place = nullptr;
    void* that_block = nullptr;

    size_t actual_block_size = 0;
    bool at_start = false;

    void* next_block = get_next_existing_block(left_elem);
    void* block = left_elem;

    while (next_block != nullptr) // search block in midle
    {
        size_t dt = get_block_distance(block, next_block);
        if (dt > actual_block_size && dt >= size + _block_meta_size)
        {
            actual_block_size = dt;
            that_block = block;
            place_to_place = slide_block_for(block, _block_meta_size + get_block_data_size(block));
        }
        block = next_block;
        next_block = get_next_existing_block(block);
    }
    size_t dt = (reinterpret_cast<byte*>(block) + get_block_data_size(block) + _block_meta_size) - (reinterpret_cast<byte*>(_trusted_memory) + _meta_size);

    if (get_size() - dt >= size + _block_meta_size && get_size() - dt > actual_block_size + _block_meta_size) // block at end
    {
        actual_block_size = get_size() - dt;
        that_block = block;
        place_to_place = slide_block_for(block, _block_meta_size + get_block_data_size(block));
    }

    if (slide_block_for(_trusted_memory, _meta_size) != left_elem) // block at start
    {
        void* st_ptr = slide_block_for(_trusted_memory, _meta_size);
        if (get_block_distance(st_ptr, left_elem) >= _block_meta_size + size &&
            get_block_distance(st_ptr, left_elem) > actual_block_size &&
            place_to_place == nullptr)
        {
            actual_block_size = get_block_distance(st_ptr, left_elem);
            that_block = left_elem;
            place_to_place = st_ptr;
            at_start = true;
        }
    }

    if (at_start)
    {
        *get_first_block_ptr() = place_to_place;
    }

    if (place_to_place == nullptr)
    {
        return nullptr;
    }
    byte* next_block_ = reinterpret_cast<byte*>(get_next_existing_block(that_block));
    if (next_block_ != nullptr)
    {
        *reinterpret_cast<void**>(next_block_ + sizeof(void*) + sizeof(size_t)) = place_to_place;
    }

    byte* prev_block = reinterpret_cast<byte*>(that_block);
    if (prev_block != nullptr)
    {
        *reinterpret_cast<void**>(prev_block + 2 * sizeof(void*) + sizeof(size_t)) = place_to_place;
    }

    return create_block_meta(place_to_place, size, that_block, reinterpret_cast<void*>(next_block));

}
inline void* allocator_boundary_tags::create_block_meta(void* block, size_t size, void* left = nullptr, void* right = nullptr)
{
    byte* place_ptr = reinterpret_cast<byte*>(block);

    *reinterpret_cast<void**>(place_ptr) = _trusted_memory;
    place_ptr += sizeof(void*);

    *reinterpret_cast<size_t*>(place_ptr) = size;
    place_ptr += sizeof(size_t);

    *reinterpret_cast<void**>(place_ptr) = left;
    place_ptr += sizeof(void*);

    *reinterpret_cast<void**>(place_ptr) = right;
    place_ptr += sizeof(void*);

    return place_ptr;
}

inline void* allocator_boundary_tags::slide_block_for(void* block, size_t bytes)
{
    return reinterpret_cast<void*>(reinterpret_cast<byte*> (block) + bytes);
}
inline void *allocator_boundary_tags::get_next_existing_block(void* left_elem)
{
    byte* ptr = reinterpret_cast<byte*> (left_elem);
    return *reinterpret_cast<void**>(ptr + 2 * sizeof(void*) + sizeof(size_t));
}
inline void* allocator_boundary_tags::get_prev_existing_block(void* right_elem)
{
    byte* ptr = reinterpret_cast<byte*> (right_elem);
    return *reinterpret_cast<void**>(ptr + sizeof(void*) + sizeof(size_t));
}

inline size_t allocator_boundary_tags::get_block_distance(void* left, void *right)
{
    if (left == right)
    {
        return 0;
    }
    return (reinterpret_cast<byte*> (right) - reinterpret_cast<byte*>(left) - 
           get_block_data_size(left) - _block_meta_size);
}

inline size_t allocator_boundary_tags::get_block_data_size(void* block)
{
    byte* ptr = reinterpret_cast<byte*> (block);
    return *reinterpret_cast<size_t*>(ptr + sizeof(void*));
}


void allocator_boundary_tags::deallocate(void *at)
{
    if (at == nullptr)
    {
        return;
    }
    std::lock_guard lock(*get_mutex());
    void* trusted = *reinterpret_cast<void**>(reinterpret_cast<byte*>(at) - _block_meta_size);
    if (trusted != _trusted_memory || trusted == nullptr)
    {
        error_with_guard("Tryed to deallocate not allocator's property");
        throw std::logic_error("Not allocator's property");
        return;
    }
    void* block = reinterpret_cast<void*>(reinterpret_cast<byte*>(at) - _block_meta_size);

    void* next_block = get_next_existing_block(block);
    void* prev_block = get_prev_existing_block(block);

    if (next_block != nullptr && prev_block != nullptr) // that was midle block
    {
        // setting right -> left
        byte* place_ptr = reinterpret_cast<byte*>(next_block);

        place_ptr += sizeof(void*);
        place_ptr += sizeof(size_t);

        *reinterpret_cast<void**>(place_ptr) = prev_block;

        // setting left -> right
        place_ptr = reinterpret_cast<byte*>(prev_block);

        place_ptr += (2 * sizeof(void*));
        place_ptr += sizeof(size_t);

        *reinterpret_cast<void**>(place_ptr) = next_block;
    }
    else if (next_block != nullptr) // that was the first (max left block)
    {
        byte* place_ptr = reinterpret_cast<byte*>(next_block);

        place_ptr += sizeof(void*);
        place_ptr += sizeof(size_t);

        *reinterpret_cast<void**>(place_ptr) = nullptr;
        *get_first_block_ptr() = next_block;
    }
    else if (prev_block != nullptr) // that was last block
    {
        byte* place_ptr = reinterpret_cast<byte*>(prev_block);

        place_ptr += (2 * sizeof(void*));
        place_ptr += sizeof(size_t);

        *reinterpret_cast<void**>(place_ptr) = nullptr;   
    }
    else if (prev_block == nullptr && next_block == nullptr) // only one block
    {
        *get_first_block_ptr() = nullptr;
    }

    // in theory can clear the meta but dont need

}

inline void allocator_boundary_tags::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    unsigned char* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);

    ptr += sizeof(class logger*);
    ptr += sizeof(allocator*);

    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr) = mode;
}

inline allocator_with_fit_mode::fit_mode allocator_boundary_tags::get_fit_mode() const
{
    unsigned char* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);

    ptr += sizeof(class logger*);
    ptr += sizeof(allocator*);

    return *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr);
}

inline allocator *allocator_boundary_tags::get_allocator() const
{
    unsigned char* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);

    ptr += sizeof(class logger*);
    
    return *reinterpret_cast<allocator**>(ptr);

}

std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const noexcept
{
    throw not_implemented("std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const noexcept", "your code should be here...");
}

inline logger *allocator_boundary_tags::get_logger() const
{
    unsigned char* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);
    return *reinterpret_cast<class logger**> (ptr);
}

inline size_t allocator_boundary_tags::get_size() const
{
    unsigned char* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);

    ptr += sizeof(class logger*);
    ptr += sizeof(allocator*);
    ptr += sizeof(allocator_boundary_tags::fit_mode);
    ptr += sizeof(std::mutex);

    return *reinterpret_cast<size_t*>(ptr);
}
inline std::mutex* allocator_boundary_tags::get_mutex() const
{
    unsigned char* ptr = reinterpret_cast<unsigned char*>(_trusted_memory);

    ptr += sizeof(class logger*);
    ptr += sizeof(allocator*);
    ptr += sizeof(allocator_boundary_tags::fit_mode);

    return reinterpret_cast<std::mutex*>(ptr);
}

inline std::string allocator_boundary_tags::get_typename() const noexcept
{
    return "allocator_boundary_tags";
}