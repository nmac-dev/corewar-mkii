#include <vector>

/// A Round Robin System which manages its position rotation 
template<typename T>
class RR_T
{
 private:
    int m_pos;                  // position of current round robin sequance
    int m_length;               // length of iterations before rotating to 0
    std::vector<T> m_sequance;  // collection containing of elements rotating in a sequance
 
 public:
    /// Create a round robin system with a sequance of elements
    /// @param _sequance a collection containing elements to rotate in the round robin sequance
    RR_T(std::vector<T> _sequance) 
    {
        m_pos       = 0;
        m_sequance  = _sequance;
        m_length    = m_sequance.size();
    }
    RR_T() = default;

    /// Return the current position index
    inline int pos() const { return m_pos;    } 
    /// Return the length of the round robin sequance
    inline int len() const { return m_length; }

    /// Return the current element in the sequance without moving to the next
    inline T i() { return m_sequance[m_pos]; }

    /// Move to next position in the sequance, and return the data
    inline T next()
    { 
        m_pos = (m_pos < m_length -1) ? m_pos +1 : 0; // rotate on complete loop
        return m_sequance[m_pos];
    }
    /// Returns the index of the previous round robin sequance
    inline int prev() { return (m_pos != 0) ? m_pos -1 : m_length -1; }

    /// Remove an element in the sequance at the position given and adjust the length
    /// @param _pos position of the element to be removed
    inline void remove(int _pos) 
    {
        if (_pos >= 0 && _pos < m_length)
        {
            m_sequance.erase(m_sequance.begin() + _pos);
            m_length = m_sequance.size();

        }
        if (m_pos >= m_length) m_pos--;
    }
};
