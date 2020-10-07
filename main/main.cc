#include <iostream>
#include <string>
#include <vector>

template <class T>
class Vector {
public:
    Vector() = default;
    ~Vector();
    void push(T const&);
    void pop();
    std::vector<T>* get_ptr();
private:
    std::vector<T> vector_;
};

template <class T>
Vector<T>::~Vector() = default;


template <class T>
void Vector<T>::push(T const& element) {
    vector_.push_back(element);
}

template <class T>
void Vector<T>::pop(){
    vector_.pop_back();
}

template<class T>
std::vector<T>* Vector<T>::get_ptr(){
    return &vector_;
}

int main() {
    Vector<int> vector_my;
    vector_my.push(1);
    vector_my.push(2);
    vector_my.push(3);
    vector_my.push(4);
    vector_my.pop();
    auto* vec = vector_my.get_ptr();
    std::cout << "vec.size: " << vec->size() << " vec.top: " << vec->back();
    

}




