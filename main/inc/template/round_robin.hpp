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
        m_pos = m_length = 0;
    }

    /// Returns current position address
    inline int pos() const { return m_pos;    } 

    /// Returns length of the round robin sequance
    inline int len() const { return m_length; }

    /// Return the current element in the sequance without moving to the next
    inline T i() { return m_sequance[m_pos]; }

    /// Move to next position in the sequance, and return the data
    inline T next()
    {
        m_pos = (m_pos < m_length -1) ? m_pos +1  // rotate forward on complete loop
                                      : 0;
        return m_sequance[m_pos];
    }
    /// Returns the address of the previous round robin sequance
    inline int prev() 
    { 
        return (m_pos != 0) ? m_pos -1      // rotate back after complete loop
                            : m_length -1;
    }

    /// Adds a new element to the end of the sequance
    /// @param data element to add
    inline void push_back(T _data)
    {
        m_sequance.push_back(_data);
        m_length = m_sequance.size();
    }

    /// Remove an element in the sequance at the position given and adjust the length
    /// @param _pos position of the element to be removed
    inline void remove(int _pos)
    {
        if (m_length > 1)
        {
            if (_pos >= 0 && _pos < m_length)
            {
                m_sequance.erase(m_sequance.begin() + _pos);
                m_length = m_sequance.size();
            }
            if (m_pos >= m_length) 
                m_pos--;
        }
    }
};
