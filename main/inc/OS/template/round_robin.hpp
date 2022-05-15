#include <vector>

/// A Round Robin System which manages its position rotation 
template<typename T>
class RoundRobin
{
 private:
    int m_pos;                  // position of current round robin sequance
    int m_length;               // length of iterations before rotating to 0
    std::vector<T> m_sequance;  // collection containing of elements rotating in a sequance

 public:
    /// Create a round robin system with a position & length of 0
    RoundRobin()
    {
        m_pos    = -1;
        m_length =  0;
    }

    /// Returns current position address
    inline int const pos() 
    { 
        if (m_pos < 0 || m_pos >= m_length)
        {
            m_pos = 0;
        }
        return m_pos;
    } 

    /// Returns length of the round robin sequance
    inline int const len() const { return m_length; }

    /// Return the iteration element in the sequance WITHOUT moving to the next
    inline T i() { return m_sequance[pos()]; }

    /// Move to next position in the sequance, and return the data
    inline T next()
    {
        if (is_running())
        {
            m_pos++;
        }
        return m_sequance[pos()];
    }

    /// Returns true if the sequance has been depleted to one or zero
    inline bool const is_running() const { return m_length > 1; }

    /// Adds a new element to the end of the sequance
    /// @param data element to add
    inline void push_back(T _data)
    {
        m_sequance.push_back(_data);
        m_length = m_sequance.size();
    }

    /// Removes element given from the sequance
    /// @param _element element to be removed
    inline void remove(T _element)
    {
        if (is_running())
        {
            // find and remove
            for (int i = 0; i < m_length; i++)
            {
                if (_element == m_sequance[i])
                {
                    m_sequance.erase(m_sequance.begin() + i);
                    m_length = m_sequance.size();
                }
            }
        }
    }
};
