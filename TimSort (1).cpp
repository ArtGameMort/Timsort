#include <iostream>
#include <cmath>
#include <initializer_list>

template<typename T>
class my_vector {
public:
    my_vector();  // Конструктор по умолчанию

    my_vector(const my_vector &, size_t left, size_t right);

    my_vector(const my_vector &);   // Конструктор копирования

    my_vector(const std::initializer_list<T> &list);

    ~my_vector();   // Деструктор

    my_vector &operator=(const my_vector &);  // Оператор присваивания

    void push_back(const T &value);   // Метод добавления элемента в конец

    [[nodiscard]] size_t size() const;   // Метод получения размера

    [[nodiscard]] size_t capacity() const;   // Метод получения вместимости

    T &operator[](size_t i); // Перегрузка оператора [] для не константных объектов

    const T &operator[](size_t i) const; // Перегрузка оператора [] для константных объектов

    template<typename C>
    friend std::istream &operator>>(std::istream &in, my_vector<C> &rhs);  // Перегрузка оператора ввода

    template<typename C>
    friend std::ostream &operator<<(std::ostream &out, const my_vector<C> &rhs); // Перегрузка оператора вывода

private:
    T *data;       // Динамический массив с данными
    size_t _size;  // Размер
    size_t cap;    // Вместимость
};

template<typename T>
my_vector<T>::my_vector() : data(nullptr), _size(0), cap(0) {}

template<typename T>
my_vector<T>::~my_vector() { delete[] data; }

template<typename T>
size_t my_vector<T>::size() const {
    return _size;
}

template<typename T>
size_t my_vector<T>::capacity() const {
    return cap;
}

template<typename T>
T &my_vector<T>::operator[](size_t i) {
    if (i >= _size) {
        throw std::out_of_range("Index out of range.");
    }
    return data[i];
}

template<typename T>
const T &my_vector<T>::operator[](size_t i) const {
    if (i >= _size) {
        throw std::out_of_range("Index out of range.");
    }
    return data[i];
}

template<typename T>
std::istream &operator>>(std::istream &in, my_vector<T> &rhs) {
    size_t _size;
    in >> _size;
    for (size_t i = 0; i < _size; ++i) {
        int num;
        in >> num;
        rhs.push_back(num);
    }
    return in;
}

template<typename T>
std::ostream &operator<<(std::ostream &out, const my_vector<T> &rhs) {
    for (size_t i = 0; i < rhs._size; ++i) {
        out << rhs.data[i] << ' ';
    }
    return out;
}

template<typename T>
my_vector<T> &my_vector<T>::operator=(const my_vector &rhs) {
    if (&rhs == this) {
        return *this;
    }
    delete[] data;
    data = new T[rhs.cap];
    cap = rhs.cap;
    _size = rhs._size;
    for (size_t i = 0; i < _size; ++i) {
        data[i] = rhs.data[i];
    }
    return *this;
}

template<typename T>
my_vector<T>::my_vector(const my_vector &rhs) : data(new T[rhs.cap]), _size(rhs._size), cap(rhs.cap) {
    for (size_t i = 0; i < _size; ++i) {
        data[i] = rhs.data[i];
    }
}

template<typename T>
my_vector<T>::my_vector(const my_vector &arr, size_t left, size_t right) :cap(ceil(log2(right - left))),
                                                                          _size(right - left), data(new T[cap]) {
    for (size_t i = left, j = 0; i < right; ++i, ++j) {
        data[j] = arr.data[i];
    }
}

template<typename T>
my_vector<T>::my_vector(const std::initializer_list<T> &list) : cap(ceil(log2(list.size()))), _size(list.size()),
                                                                data(new T[cap]) {
    size_t i = 0;
    for (const auto &x: list) {
        data[i++] = x;
    }
}

template<typename T>
void my_vector<T>::push_back(const T &value) {
    if (_size + 1 > cap) {
        T *tmp = new T[_size];
        for (size_t i = 0; i < _size; ++i) {
            tmp[i] = data[i];
        }
        delete[] data;

        if (cap == 0) cap = 1;
        else cap *= 2;
        data = new T[cap];

        for (size_t i = 0; i < _size; ++i) {
            data[i] = tmp[i];
        }
        delete[] tmp;
    }
    data[_size] = value;
    ++_size;
}


const int RUN = 32;

void insertionSort(my_vector<int> &arr, int left, int right) {
    for (size_t i = left + 1; i <= right; ++i) {
        int tmp = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > tmp) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = tmp;
    }
}

void merge(my_vector<int> &arr, int l, int m, int r) {
    int len1 = m - l + 1, len2 = r - m;
    int left[len1], right[len2];
    for (int i = 0; i < len1; i++)
        left[i] = arr[l + i];
    for (int i = 0; i < len2; i++)
        right[i] = arr[m + 1 + i];

    int i = 0;
    int j = 0;
    int k = l;

    while (i < len1 && j < len2) {
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        } else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    // Copy remaining elements of
    // left, if any
    while (i < len1) {
        arr[k] = left[i];
        k++;
        i++;
    }

    // Copy remaining element of
    // right, if any
    while (j < len2) {
        arr[k] = right[j];
        k++;
        j++;
    }
}

void timSort(my_vector<int> &arr) {
    for (size_t i = 0; i < arr.size(); i += RUN) {
        insertionSort(arr, i, std::min((i + RUN - 1), (arr.size() - 1)));
    }

    for (size_t size = RUN; size < arr.size(); size = 2 * size) {
        for (size_t left = 0; left < arr.size(); left += 2 * size) {
            size_t mid = left + size - 1;
            size_t right = std::min((left + 2 * size - 1), (arr.size() - 1));
            if (mid < right)
                merge(arr, left, mid, right);
        }
    }
}

int main() {
    my_vector<int> arr = {9, 5, 2, 7, 1, 3, 8, 6, 4};
    timSort(arr);
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i] << " ";
    }
    return 0;
}