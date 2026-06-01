template< class T, class Compare >
PriorityQueue< T, Compare >::PriorityQueue( std::initializer_list<T> init_list, const Compare& compare )
    : heap(compare)
{
    for (const T& item : init_list) {
        this->Push(item);
    }
}

template<class T, class Compare>
PriorityQueue<T, Compare>& PriorityQueue<T, Compare>::operator=(const PriorityQueue<T, Compare>& other) {
    if (this != &other) {
        this->heap = other.heap;
    }
    return *this;
}

template<class T, class Compare>
PriorityQueue<T, Compare>& PriorityQueue<T, Compare>::operator=(PriorityQueue<T, Compare>&& other) {
    if (this != &other) {
        this->heap = std::move(other.heap);
    }
    return *this;
}

template<class T, class Compare>
template<class... Args>
void PriorityQueue<T, Compare>::Emplace(Args&&... args) {
    heap.Insert(T(std::forward<Args>(args)...));
}

template<class T, class Compare>
template<class R>
void PriorityQueue<T, Compare>::PushRange(R&& rg) {
    for (auto& el : rg) {
        heap.Insert(el); 
    }
}

template <typename T, typename Compare>
PriorityQueue<int, std::less<int>> PriorityQueue<T, Compare>::Map(std::function<int(const T&)> func) const {
    PriorityQueue<int, std::less<int>> result;
    PriorityQueue<T, Compare> temp = *this;
    
    while (!temp.Empty()) {
        result.Push(func(temp.Top()));
        temp.pop();
    }
    return result;
}

template<class T, class Compare>
PriorityQueue<T, Compare> PriorityQueue<T, Compare>::Where(std::function<bool(const T&)> predicate) const {
    PriorityQueue<T, Compare> result;
    PriorityQueue<T, Compare> temp(*this);
    while (!temp.Empty()) {
        if (predicate(temp.Top())) {
            result.Push(temp.Top());
        }
        temp.pop();
    }
    return result;
}

template<class T, class Compare>
template<class T2>
T2 PriorityQueue<T, Compare>::Reduce(std::function<T2(const T2&, const T&)> func) const {
    T2 acc{};
    PriorityQueue<T, Compare> temp(*this);
    while (!temp.Empty()) {
        acc = func(acc, temp.Top());
        temp.pop();
    }
    return acc;
}

template<class T, class Compare>
PriorityQueue<T, Compare> PriorityQueue<T, Compare>::Concat(const PriorityQueue& other) const {
    PriorityQueue<T, Compare> result(*this);
    PriorityQueue<T, Compare> temp(other);
    while (!temp.Empty()) {
        result.Push(temp.Top());
        temp.pop();
    }
    return result;
}

template<class T, class Compare>
PriorityQueue<T, Compare> PriorityQueue<T, Compare>::GetSubsequence(size_t startIndex, size_t endIndex) const {
    PriorityQueue<T, Compare> result;
    
    if (startIndex > endIndex || startIndex >= heap.GetSize()) {
        return result;
    }

    PriorityQueue<T, Compare> temp(*this);
    size_t currentIdx = 0;

    while (!temp.Empty() && currentIdx <= endIndex) {
        if (currentIdx >= startIndex) {
            result.Push(temp.Top());
        }
        temp.pop();
        currentIdx++;
    }

    return result;
}

template<class T, class Compare>
bool PriorityQueue<T, Compare>::IsSubsequence(const PriorityQueue& subQueue) const {
    PriorityQueue<T, Compare> subTemp(subQueue);
    while (!subTemp.Empty()) {
        const T& target = subTemp.Top();
        PriorityQueue<T, Compare> mainTemp(*this);
        bool found = false;
        while (!mainTemp.Empty()) {
            if (mainTemp.Top() == target) {
                found = true;
                break;
            }
            mainTemp.pop();
        }
        if (!found) return false;
        subTemp.pop();
    }
    return true;
}

template<class T, class Compare>
void PriorityQueue<T, Compare>::Append(const PriorityQueue& other) {
    PriorityQueue<T, Compare> temp(other);
    while (!temp.Empty()) {
        Push(temp.Top());
        temp.pop();
    }
}

template<class T, class Compare>
std::pair<PriorityQueue<T, Compare>, PriorityQueue<T, Compare>> 
PriorityQueue<T, Compare>::Split(std::function<bool(const T&)> predicate) const {
    PriorityQueue<T, Compare> trueQueue;
    PriorityQueue<T, Compare> falseQueue;
    PriorityQueue<T, Compare> temp(*this);
    while (!temp.Empty()) {
        if (predicate(temp.Top())) {
            trueQueue.Push(temp.Top());
        } else {
            falseQueue.Push(temp.Top());
        }
        temp.pop();
    }
    return std::make_pair(trueQueue, falseQueue);
}