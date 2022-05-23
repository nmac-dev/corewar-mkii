/// Queue of nodes containing data <T>
template <typename T> 
class Queue
{
 private:
    /// Containts data and pointer to the next|previous node
    struct Node
    {
        T data;     // data of type <T>
        Node *prev, // previous node in queue
             *next; // next     node in queue
        
        /// Creates a new node containing data <T>
        /// @param _data element to store in the node
        Node(T _data)
        {
            data = _data;
            prev = nullptr;
            next = nullptr;
        }
    };
    
    bool m_empty;   // empty queue flag
    int   m_size;   // current size of the queue
    Node *m_front,  // node at the front of the queue
         *m_back;   // node at the back  of the queue

 public:
    /// Creates a queue to contain elements
    Queue()
    {
        m_empty  = true;
        m_size   = 0;
        m_front  = nullptr;
        m_back   = nullptr;
    }
    /// Deletes the queue & all nodes 
    ~Queue() { while (this->dequeue()); }
    
    /// Returns the size of the queue
    inline int size()       const { return m_size; }
    /// Returns true if the queue is empty
    inline bool is_empty()   const { return m_empty; }

    /// Returns the element at the front of the queue (copy)
    inline T const front() { return !is_empty() ? m_front->data : T(); }
    /// Returns the element at the front of the queue (copy)
    inline T const back()  { return !is_empty() ? m_back->data  : T(); }
    
    /// Enqueue a new node with the stored data to the back of the queue
    /// @param _data to store inside the node
    inline void enqueue(T _data)
    {
        Node *node = new Node(_data);
        
        // queue is empty
        if (is_empty())
        {
            m_front = node;
            m_back  = m_front; // new node is only element in queue

            m_size++;
            m_empty = false;
            return;
        }

        // contains multiple elements
        if (m_back != m_front)
        {
            node->next = m_back; // link new node to list
            m_back->prev = node; // set previous node
            m_back       = node; // update back node
        }
        else // one element
        {
            node->next    = m_front;
            m_front->prev = node;
            m_back = node;
        }
        m_size++;
    }

    /// Copies the node data at the front of the queue into the data buffer, then deletes the node
    /// @return true:  queue copied element to data buffer and deleted node
    /// @return false: no node was deleted (empty queue) and buffer was left unchanged
    inline bool dequeue(T *data_bf = nullptr)
    {   
        if (is_empty()) return false;    // empty queue
        
        Node *ptr = m_front;            // point to front node
        if (data_bf != nullptr)
        {
            *data_bf = ptr->data;       // copy node data
        }

        if (m_front != m_back)          // queue contains multiple elements
        {
            m_front       = m_front->prev;
            m_front->next = nullptr;
        }
        else                            // only front node in queue
        {
            m_front = nullptr;
            m_back  = nullptr;

            m_empty = true;
        }
        delete ptr;
        m_size--;

        return true;
    }
};
