#include <memory>

/// Circular Random Access Memory: contains an array of T objects
template<typename T>
struct C_RAM
{
 using RAM_PTR = std::unique_ptr<T[]>; // Unique Ptr to the systems RAM
 private:
    int m_size;     // number of memory addresses
    RAM_PTR m_ram;  // array of memory addresses

 public:
    /// Creates an array of memory addresses of type <T>
    C_RAM(int const _size) 
    {
        m_size = _size;
        m_ram = RAM_PTR(new T[m_size]);
    }
    C_RAM() = default;

    /// Loops the address when outside the bounds of RAM
    /// @param address RAM address
    inline int loop_index(int address) const
    {
        // loop address until it is within the 
        while (address < 0 || address >= m_size)
        {
            
            if (address < 0)        // address -> upper bounds (m_size)
            {
                address += m_size;
            }
            else address -= m_size; // address -> lower bounds (0)
        }
        return address;
    }
    
    /// Returns the number of memory addresses
    inline int size() const { return m_size; };

    /// Access address of RAM's memory array (filters address)
    T *const operator[](int address) const { return &m_ram[loop_index(address)]; }
    /// Modify address of RAM's memory array (filters address)
    T *operator[](int address)             { return &m_ram[loop_index(address)]; }
};
