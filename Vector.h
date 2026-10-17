#ifndef VECTOR_H
#define VECTOR_H
template <typename T>
class Vector {
private:
    T* data;       
    int size;      
    int capacity;  
    void grow() {
        capacity = capacity * 2;
        T* newData = new T[capacity];
        for (int i = 0; i < size; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
    }
public:
    Vector() : data(new T[2]), size(0), capacity(2) {}
    Vector(const Vector& other) : data(new T[other.capacity]), size(other.size), capacity(other.capacity) 
    {
        for (int i = 0; i < size; i++) 
        {
            data[i] = other.data[i];
        }
    }
    Vector& operator=(const Vector& other) 
    {
        if (this == &other) 
        {
            return *this;
        }
        delete[] data;
        capacity = other.capacity;
        size = other.size;
        data = new T[capacity];
        for (int i = 0; i < size; i++) 
        {
            data[i] = other.data[i];
        }
        return *this;
    }
    ~Vector() 
    { 
        delete[] data; 
    }
    void push_back(const T& value) 
    {
        if (size == capacity) grow();
        data[size++] = value;
    }
    T& operator[](int index) 
    { return data[index]; 
    }
    const T& operator[](int index) const 
    { 
        return data[index]; 
    }
    int getSize() const 
    { 
        return size; 
    }
};
#endif
