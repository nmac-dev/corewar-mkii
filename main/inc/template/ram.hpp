#include <memory>

/// Random Access Memory (circular): Contains an array of T objects
template<typename T>
struct RAM_T
{
 using RAM_PTR = std::unique_ptr<T[]>; // Unique Ptr to the systems RAM
 private:
    int m_size;     // number of memory addresses
    RAM_PTR m_ram;  // array of memory addresses

 public:
    /// Creates an array of memory addresses of type <T>
    RAM_T(int _size) 
    {
        m_size = _size;
        m_ram = RAM_PTR(new T[m_size]);
    }
    RAM_T() = default;

    /// Loops the index when outside the bounds of RAM
    /// @param index RAM index
    inline int loopIndex(int index) const
    {
        // loop index until it is within the 
        while (index < 0 || index > m_size)
        {
            // index to upper core section
            if (index < 0)  index += m_size;
            // index to lower core (> core_size)
            else index -= m_size;
        }
        return index;
    }
    
    /// Returns the number of memory addresses
    inline int size() const { return m_size; };

    /// Access index of RAM's memory array (filters index)
    T *const operator[](int index) const { return &m_ram[loopIndex(index)]; }
    /// Modify index of RAM's memory array (filters index)
    T *operator[](int index)             { return &m_ram[loopIndex(index)]; }
};
