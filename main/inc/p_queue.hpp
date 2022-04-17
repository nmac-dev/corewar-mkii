/// Template declarations & definitions go in the header, for compiler to generate type parameters   

/// P_Queue<Parent, Type>: a queue containing elements with an associated parent
template <typename P, typename T> 
class P_Queue
{
 private:
    // Associated parent of the queue elements
    P m_parent;

    // Containts data and pointer to the next node
    struct Node
    {
        T data;     // data of type <T>
        Node *prev, // previous node in queue
             *next; // next     node in queue
        
        /// Creates a new node containing data & sets next node to nullptr
        /// @param _data element to store in the node
        Node(T _data)
        {
            data = _data;
            prev = nullptr;
            next = nullptr;
        }
    };
    
    int   m_size;   // current size of the queue
    Node *m_front,  // node at the front of the queue
         *m_back;   // node at the back  of the queue

 public:
    /// Creates a queue to contain elements with an associated parent
    /// @param parent of the queue elements
    P_Queue(P parent)
    {
        m_parent = parent;
        m_size   = 0;
        m_front  = nullptr;
        m_back   = nullptr;
    }
    P_Queue(){}
    /// Deletes the queue & all nodes 
    ~P_Queue() 
    { 
        while (m_size-- > 0)
        {
            this->pop();
        }
    }

    /// Returns a reference to the parent of the queue elements
    inline P const parent() const { return (m_parent != nullptr) ? m_parent : 0; }
    
    /// Returns the size of the queue
    inline int size()       const { return m_size; }
    /// Returns true if the queue is empty
    inline bool isEmpty()   const { return m_front == nullptr; }
    /// Returns the element at the front of the queue (copy)
    inline T const front()  const { return (m_front != nullptr) ? m_front->data : 0; }
    /// Returns the element at the front of the queue (copy)
    inline T const back()   const { return (m_back != nullptr) ? m_back->data : 0; }
    
    /// Push a new node with the stored data to the back of the queue
    /// @param data to store inside the node
    inline void push(T data) 
    {
        Node *node = new Node(data);
        
        // queue is empty
        if (m_size == 0)
        {
            m_front = node;
            m_back  = m_front; // new node is only element in queue
            m_size++;
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

    /// Returns a copy of the node data at the front of the queue, then deletes the node
    inline T pop() 
    {
        // queue is empty
        if (m_front == nullptr) return 0; // skip
        
        Node *ptr = m_front;    // point to front node
        T data    = ptr->data;  // copy node data

        // queue contains multiple elements
        if (m_front != m_back)
        {
            m_front =  m_front->prev;   // update front node
            m_front->next = nullptr;
            delete ptr;                 // delete pop node
        }
        else // only front node in queue
        {
            m_front = nullptr;
            m_back  = nullptr;
            delete ptr;
        }
        m_size--;
        return data;
    }
};
