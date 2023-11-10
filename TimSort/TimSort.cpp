#include <iostream>
#include <cmath>
#include <initializer_list>

template<typename T>
class my_vector {
public:
    my_vector();  // Конструктор по умолчанию

    my_vector(const my_vector&, size_t left, size_t right);

    my_vector(const my_vector&);   // Конструктор копирования

    my_vector(const std::initializer_list<T>& list);

    ~my_vector();   // Деструктор

    my_vector& operator=(const my_vector&);  // Оператор присваивания

    void push_back(const T& value);   // Метод добавления элемента в конец

    size_t size() const;   // Метод получения размера

    size_t capacity() const;   // Метод получения вместимости

    T& operator[](size_t i); // Перегрузка оператора [] для не константных объектов

    const T& operator[](size_t i) const; // Перегрузка оператора [] для константных объектов

    template<typename C>
    friend std::istream& operator>>(std::istream& in, my_vector<C>& rhs);  // Перегрузка оператора ввода

    template<typename C>
    friend std::ostream& operator<<(std::ostream& out, const my_vector<C>& rhs); // Перегрузка оператора вывода

private:
    T* data;       // Динамический массив с данными
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
T& my_vector<T>::operator[](size_t i) {
    if (i >= _size) {
        throw std::out_of_range("Index out of range.");
    }
    return data[i];
}

template<typename T>
const T& my_vector<T>::operator[](size_t i) const {
    if (i >= _size) {
        throw std::out_of_range("Index out of range.");
    }
    return data[i];
}

template<typename T>
std::istream& operator>>(std::istream& in, my_vector<T>& rhs) {
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
std::ostream& operator<<(std::ostream& out, const my_vector<T>& rhs) {
    for (size_t i = 0; i < rhs._size; ++i) {
        out << rhs.data[i] << ' ';
    }
    return out;
}

template<typename T>
my_vector<T>& my_vector<T>::operator=(const my_vector& rhs) {
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
my_vector<T>::my_vector(const my_vector& rhs) : data(new T[rhs.cap]), _size(rhs._size), cap(rhs.cap) {
    for (size_t i = 0; i < _size; ++i) {
        data[i] = rhs.data[i];
    }
}

template<typename T>
my_vector<T>::my_vector(const my_vector& arr, size_t left, size_t right) :cap(ceil(log2(right - left))), _size(right - left), data(new T[cap]) {
    for (size_t i = left, j = 0; i < right; ++i, ++j) {
        data[j] = arr.data[i];
    }
}

template<typename T>
my_vector<T>::my_vector(const std::initializer_list<T>& list) : cap(ceil(log2(list.size()))), _size(list.size()), data(new T[cap]) {
    size_t i = 0;
    for (const auto& x : list) {
        data[i++] = x;
    }
}

template<typename T>
void my_vector<T>::push_back(const T& value) {
    if (_size + 1 > cap) {
        T* tmp = new T[_size];
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



// Сортировка вставками
template<typename T>
void insertionSort(my_vector<T>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        T key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Поиск последовательностей run
template<typename T>
int findRun(my_vector<T>& arr) {
    int n = arr.size();
    int run = 0;
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i - 1])
            return run;
        run++;
    }
    return run;
}

// Подсчет minrun
template<typename T>
int calcMinrun(int n) {
    int r = 0;
    while (n >= 64) {
        r |= n & 1;
        n >>= 1;
    }
    return n + r;
}

// Слияние последовательностей run
template<typename T>
void mergeRuns(my_vector<T>& arr, int minrun) {
    int n = arr.size();
    my_vector<T> temp(arr);
    for (int i = 0; i < n; i += minrun) {
        insertionSort(temp, i, std::min(i + minrun - 1, n - 1));
    }
    for (int size = minrun; size < n; size *= 2) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = std::min(left + 2 * size - 1, n - 1);
            my_vector<T> temp2(arr.begin() + left, arr.begin() + mid + 1);
            my_vector<T> temp3(arr.begin() + mid + 1, arr.begin() + right + 1);
            int i = 0, j = 0, k = left;
            while (i < temp2.size() && j < temp3.size()) {
                if (temp2[i] <= temp3[j])
                    arr[k++] = temp2[i++];
                else
                    arr[k++] = temp3[j++];
            }
            while (i < temp2.size()) {
                arr[k++] = temp2[i++];
            }
            while (j < temp3.size()) {
                arr[k++] = temp3[j++];
            }
        }
    }
}

// Режим галопа при слиянии
template<typename T>
void gallopMerge(my_vector<T>& arr, int left, int mid, int right) {
    my_vector<T> temp(arr, left, right + 1);
    //    my_vector<T> temp(arr.begin() + left, arr.begin() + right + 1);
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        if (temp[i - left] <= temp[j - left])
            arr[k++] = temp[i++ - left];
        else
            arr[k++] = temp[j++ - left];
    }
    while (i <= mid) {
        arr[k++] = temp[i++ - left];
    }
}

// Timsort
template<typename T>
void timsort(my_vector<T>& arr) {
    int n = arr.size();
    int minrun = calcMinrun<T>(n);
    for (int i = 0; i < n; i += minrun) {
        insertionSort(arr, i, std::min(i + minrun - 1, n - 1));
    }
    for (int size = minrun; size < n; size *= 2) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = std::min(left + 2 * size - 1, n - 1);
            gallopMerge(arr, left, mid, right);
        }
    }
}

int main() {
    my_vector<int> arr = { 9, 5, 2, 7, 1, 3, 8, 6, 4 };
    timsort(arr);
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i] << " ";
    }
    return 0;
}