#include <cstdint>
#include <iostream>
#include <memory>


using namespace std;

// custom new unique pointer which will by default be moved and not copied with = operator
template <typename T> class new_unique_ptr {
    public:
    T* ptr; 
    //new_unique_ptr(const new_unique_ptr&) = delete; // remove copy functionality
    new_unique_ptr(new_unique_ptr& other) {
        ptr = other.ptr;
        other.ptr = nullptr;
    }
    new_unique_ptr(){
        ptr = nullptr;
    }
    new_unique_ptr(T* p) : ptr(p) {}

    new_unique_ptr &operator=(const new_unique_ptr&) = delete; // remove copy functionality
    T operator*() {
        return *ptr;
    }
};

template <typename T> new_unique_ptr<T> new_make_unique(int n) {
    return new_unique_ptr<int>(new int(n));
}

template <typename T> new_unique_ptr<T> move(new_unique_ptr<T> other) {
    new_unique_ptr<T> ptr = other;
    other.ptr = nullptr;
    return ptr;
}


// To - do , Custom deleter and reset release functionality

int main(){
    new_unique_ptr<int> ptr = new_make_unique<int>(30);
    new_unique_ptr<int> ptr2 = ptr;
    cout << "ptr2 value :" << *ptr2 << endl; 
    if(!(*ptr)) {
        cout << "ptr is empty" << endl;
    }
    return 0;
}