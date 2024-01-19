#include <iostream>
#include <vector>
#include <map>
#include <cstring>

#define CHUNK_SIZE 10
//#define DEBUG

template <class T, class Allocator = std::allocator<T> >
class my_vector {

private:
    uint32_t size = 0;
    T *data;
    Allocator alloc;

public:
    my_vector(uint32_t size, Allocator alloc) 
    {
        this->size = size;
        this->alloc = alloc;

        this->data = alloc.allocate(size);
    }
    my_vector(Allocator alloc) {this->alloc = alloc;}

    my_vector();
    ~my_vector() {}

    void push_back(const T& x)
    {
        T *new_data = alloc.allocate((size + 1));
        std::memcpy(new_data, data, size * sizeof(T));
        std::swap(new_data, data);
        alloc.deallocate(new_data, size);
        ++size;
        *(data + size - 1) = x;
    }

    T& operator[](int position)
    {
        return *(data + position);
    }
};

template <class T>
struct allocator_header
{
    typedef T value_type;
    T *mem;
    uint32_t size;
    bool used_memory;
};

template <class T>
struct simple_allocator {
    typedef T value_type;
    T start_mem_ptr[CHUNK_SIZE], *end_mem_ptr, *used_mem_ptr;
    int pos = 0;
 
    simple_allocator()
    {
        end_mem_ptr = &start_mem_ptr[CHUNK_SIZE - 1];
        used_mem_ptr = start_mem_ptr;
    }
    ~simple_allocator() {}

    T* allocate(std::size_t n)
    {
        if (pos + n > CHUNK_SIZE)
            throw std::bad_alloc();

#ifdef DEBUG
        std::cout << "allocated: " << n << std::endl;
#endif
        T *mem = used_mem_ptr;
        pos += n;
        used_mem_ptr += n;

        return mem;
    }

    void deallocate(T* ptr, std::size_t n) {}
};

template <class T, class U>
bool operator==(const simple_allocator<T>&, const simple_allocator<U>&)
{
    return true;
}

template <class T, class U>
bool operator!=(const simple_allocator<T>&, const simple_allocator<U>&)
{
    return false;
}

unsigned long long int fact(int n)
{
   return (n > 1) ? n * fact(n - 1) : 1;
}

int main()
{
    simple_allocator<int> my_alloc1;
    simple_allocator<int> my_alloc2;
    std::allocator<int> std_alloc;

    auto m_my_alloc = std::map<int, int, std::less<int>, simple_allocator<std::pair<const int, int>>>{};

    my_vector<int, std::allocator<int>> v_std_alloc(9, std_alloc);
    my_vector<int, simple_allocator<int>> v_my_alloc(9, my_alloc2);

    for(size_t i = 0; i < 10; ++i) {
        if (i == 0)
            m_my_alloc[i] = 1;
        else
            m_my_alloc[i] = m_my_alloc[i - 1] * i;
    }

    std::cout << "map my_allocator: ";
    for (const auto& [key, value] : m_my_alloc)
        std::cout << '[' << key << "] = " << value << "; ";
    std::cout << std::endl;

    for (int i = 0; i < 9; ++i)
        v_std_alloc[i] = i;
    v_std_alloc.push_back(9);

    for (int i = 0; i < 9; ++i)
        v_my_alloc[i] = i;
    v_my_alloc.push_back(9);

    std::cout << "my_vector std::allocator : ";
    for (int i = 0; i < 10; ++i)
        std::cout << v_std_alloc[i] << " ";
    std::cout << std::endl;

    std::cout << "my_vector my_allocator : ";
    for (int i = 0; i < 10; ++i)
        std::cout << v_my_alloc[i] << " ";
    std::cout << std::endl;

	return 0;
}
