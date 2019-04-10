#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <memory>
#include <algorithm>

using namespace std;

class InvalidIndexException {
};

#endif /* __PROGTEST__ */

class String {
    size_t size;
    char *buffer;
public:
    String();

    String(const String &);

    String(const char *);

    ~String();

    size_t length() const;

    void resize(unsigned int, char);

    void insert(unsigned int, String &);

    char &operator[](unsigned int);

    void operator=(const String &);

    void operator+=(const String &);

    // other methods
    friend bool operator==(const String &, const String &);

    friend bool operator<=(const String &, const String &);

    friend bool operator<(const String &, const String &);

    friend ostream &operator<<(ostream &, const String &);

    char *toChar() {
        return buffer;
    }
};

String::String() {
    buffer = nullptr;
    size = 0;
}

String::String(const String &s) {
    size = s.size;
    buffer = new char[size];
    for (size_t i = 0; i < size; i++) {
        buffer[i] = s.buffer[i];
    }
}

String::String(const char *p) {
    size_t i = 0;
    const char *t = p;

    while (*p++) {
        i++;
    }

    buffer = new char[i];
    size_t j = 0;

    for (j = 0; *t; t++, j++) {
        buffer[j] = *t;
    }
    size = j;
}

String::~String() {
    delete[] buffer;
}

size_t String::length() const {
    if (buffer == nullptr) {
        return 0;
    } else {
        return size;
    }
}

char &String::operator[](unsigned int x) {
    return buffer[x];
}

void String::operator=(const String &s) {
    buffer = s.buffer;
}

ostream &operator<<(ostream &os, const String &s) {
    for (size_t i = 0; i < s.size; i++) {
        os << s.buffer[i];
    }
    return os;
}

bool operator==(const String &s, const String &t) {
    if (s.length() != t.length()) {
        return false;
    } else {
        for (size_t i = 0; i < s.length(); i++) {
            if (s.buffer[i] != t.buffer[i]) {
                return false;
            }
        }
    }
    return true;
}

template <class T>
class  Vector
{
public:

    typedef T * iterator;
    typedef const T * const_iterator;
    Vector();
    Vector(unsigned int size);
    Vector(unsigned int size, const T & initial);
    Vector(const Vector<T> & v);
    ~Vector();

    unsigned int capacity() const;
    unsigned int size() const;
    bool empty() const;
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    iterator lower_bound(size_t from) const;
    T & front();
    T & back();
    void push_back(const T & value);
    void insert(iterator it, const T & value);
    void insert2(iterator it, const T & value);
    void erase(iterator it);
    void pop_back();

    void reserve(unsigned int capacity);
    void resize(unsigned int size);

    T & operator[](unsigned int index);
    T & operator[](unsigned int index) const;
    Vector<T> & swap(const Vector<T> &);
    void clear();
private:
    unsigned int my_size;
    unsigned int my_capacity;
    T * buffer;
};

template<class T>
Vector<T>::Vector() {
    my_capacity = 0;
    my_size = 0;
    buffer = 0;
}

template<class T>
Vector<T>::Vector(const Vector<T> & v) {
    my_size = v.my_size;
    my_capacity = v.my_capacity;
    buffer = new T[my_size];
    for (unsigned int i = 0; i < my_size; i++)
        buffer[i] = v.buffer[i];
}

template<class T>
Vector<T>::Vector(unsigned int size) {
    my_capacity = size;
    my_size = size;
    buffer = new T[size];
}

template<class T>
Vector<T>::Vector(unsigned int size, const T & initial) {
    my_size = size;
    my_capacity = size;
    buffer = new T [size];
    for (unsigned int i = 0; i < size; i++)
        buffer[i] = initial;
}

template<class T>
typename Vector<T>::iterator Vector<T>::lower_bound(size_t from) const {
//    size_t count = my_size;
//    size_t step = 0;
//    iterator it;
//    iterator out = buffer + size();
//    while (count>0)
//    {
//        step=count/2;
//        it = buffer + step;
//        if (it->second + it->first.getLength() < from + 1) {
//            count-=step+1;
//        }
//        else {
//            count=step;
//            out = it;
//        }
//    }
//    return out;
    iterator it = buffer;
    while (it->second + it->first.getLength() < from + 1 && it != buffer + size()) {
        it++;
    }
    return it;
}

template<class T>
Vector<T> & Vector<T>::swap (const Vector<T> & v)
{
    delete[] buffer;
    my_size = v.my_size;
    my_capacity = v.my_capacity;
    buffer = new T [my_size];
    for (unsigned int i = 0; i < my_size; i++)
        buffer[i] = v.buffer[i];
    return *this;
}

template<class T>
typename Vector<T>::iterator Vector<T>::begin()
{
    return buffer;
}

template<class T>
typename Vector<T>::const_iterator Vector<T>::begin() const
{
    return buffer;
}

template<class T>
typename Vector<T>::iterator Vector<T>::end()
{
    return buffer + size();
}

template<class T>
typename Vector<T>::const_iterator Vector<T>::end() const
{
    return buffer + size();
}

template<class T>
T& Vector<T>::front()
{
    return buffer[0];
}

template<class T>
T& Vector<T>::back()
{
    return buffer[my_size - 1];
}

template<class T>
void Vector<T>::push_back(const T & v)
{
    if (my_size >= my_capacity)
        reserve(my_capacity+1000);
    buffer [my_size++] = v;
}

template<class T>
void Vector<T>::insert(iterator it, const T & v) {
    if (my_size >= my_capacity)
        reserve(my_capacity+1000);
    T *newBuffer = new T[my_size];
    for (unsigned i = 0; i < my_size; i++) {
        newBuffer[i] = buffer[i];
    }
    unsigned itPos = it - this->begin();
    buffer[itPos] = v;
    my_size++;
    for (unsigned i = itPos; i < my_size-1; i++) {
        buffer[i+1] = newBuffer[i];
        buffer[i+1].second += v.first.getLength();
//        cout << newBuffer[i].second << endl;
//        cout << *newBuffer[i].first.getStr() << endl;
//        cout << newBuffer[i].first.getLength() << endl;
    }
    delete[] newBuffer;
}

template<class T>
void Vector<T>::insert2(iterator it, const T & v) {
    if (my_size >= my_capacity)
        reserve(my_capacity+1000);
    T *newBuffer = new T[my_size];
    for (unsigned i = 0; i < my_size; i++) {
        newBuffer[i] = buffer[i];
    }
    unsigned itPos = it - this->begin();
    buffer[itPos] = v;
    my_size++;
    for (unsigned i = itPos; i < my_size-1; i++) {
        buffer[i+1] = newBuffer[i];
    }
    delete[] newBuffer;
}

template<class T>
void Vector<T>::erase(iterator it) {
    unsigned itPos = it - this->begin();
    for (unsigned i = itPos; i < my_size-1; i++) {
        buffer[i] = buffer[i+1];
    }
    my_size--;
}

template<class T>
void Vector<T>::pop_back()
{
    my_size--;
}

template<class T>
void Vector<T>::reserve(unsigned int capacity)
{
    if(buffer == 0)
    {
        my_size = 0;
        my_capacity = 0;
    }
    T * Newbuffer = new T [capacity];

    unsigned int l_Size = capacity < my_size ? capacity : my_size;

    for (unsigned int i = 0; i < l_Size; i++)
        Newbuffer[i] = buffer[i];
    my_capacity = capacity;
    delete[] buffer;
    buffer = Newbuffer;
}

template<class T>
unsigned int Vector<T>::size()const//
{
    return my_size;
}

template<class T>
void Vector<T>::resize(unsigned int size)
{
    reserve(size);
    my_size = size;
}

template<class T>
T& Vector<T>::operator[](unsigned int index)
{
    return buffer[index];
}

template<class T>
T& Vector<T>::operator[](unsigned int index) const
{
    return buffer[index];
}

template<class T>
unsigned int Vector<T>::capacity()const
{
    return my_capacity;
}

template<class T>
Vector<T>::~Vector()
{
    delete[] buffer;
}
template <class T>
void Vector<T>::clear()
{
    my_capacity = 0;
    my_size = 0;
    buffer = 0;
}

class CSubPatchStr {
public:
    CSubPatchStr() {
        str = nullptr;
        length = 0;
        offset = 0;
    };
    CSubPatchStr(size_t offset, size_t length, const shared_ptr<String> &str) {
        this->str = str;
        this->length = length;
        this->offset = offset;
    };

    const shared_ptr<String> &getStr() const {
        return str;
    }

    size_t getOffset() const {
        return offset;
    }

    size_t getLength() const {
        return length;
    }

    void setStr(const shared_ptr<String> &str) {
        CSubPatchStr::str = str;
    }

    void setOffset(size_t offset) {
        CSubPatchStr::offset = offset;
    }

    void setLength(size_t length) {
        CSubPatchStr::length = length;
    }

private:
    shared_ptr<String> str;
    size_t offset;
    size_t length;
};



class CPatchStr {
    Vector<pair<CSubPatchStr, size_t>> vec;
    size_t length = 0;
public:

    CPatchStr(void) = default;

    CPatchStr(const char *str) {
        String string1 = str;
        auto ptr = make_shared<String>(string1);
        vec.push_back(make_pair(CSubPatchStr(0, (*ptr).length(), ptr), length));
        length += (*ptr).length();
    };
    // copy constructor
    CPatchStr(const CPatchStr &src) {
        vec.clear();
        length = 0;
        for (auto const &str : src.vec) {
            vec.push_back(str);
        }
        length = src.length;
    }

    // destructor
    // operator =
    CPatchStr &operator=(const char *str) {
        vec.clear();
        length = 0;
        String string1 = str;
        auto ptr = make_shared<String>(string1);
        vec.push_back(make_pair(CSubPatchStr(0, (*ptr).length(), ptr), length));
        length += (*ptr).length();
        return *this;
    }
    CPatchStr &operator=(const CPatchStr &src) {
        vec.clear();
        length = src.length;
        for (auto const &str : src.vec) {
            vec.push_back(str);
        }
        return *this;
    }
    CPatchStr SubStr(size_t from, size_t len) const {
        if (from + len > length)
            throw InvalidIndexException();
        CPatchStr out;
        if (len == 0) {
            out.vec.push_back(make_pair(CSubPatchStr(0,0,make_shared<String>("")),0));
            return out;
        }
        auto lb = vec.lower_bound(from);
//        cout << *lb->first.getStr() << endl;
        size_t newOffset = from - lb->first.getOffset() - lb->second;
        size_t newLength = lb->first.getLength() - newOffset;
        size_t newGlobOffset = 0;
        if (newLength > len) {
            CSubPatchStr first = CSubPatchStr(newOffset, len, lb->first.getStr());
            out.vec.push_back(make_pair(first, newGlobOffset));
            out.length = len;
            return out;
        }
        CSubPatchStr first = CSubPatchStr(newOffset, newLength, lb->first.getStr());
        out.vec.push_back(make_pair(first, newGlobOffset));
        newGlobOffset += newLength;
        if (lb->first.getLength() >= len) {
            out.length = len;
            return out;
        }
        lb++;
        while (newGlobOffset + lb->first.getLength() < len) {
            if (lb->first.getLength() + lb->second < from + len) {
                out.vec.push_back(make_pair(lb->first, newGlobOffset));
                newGlobOffset += lb->first.getLength();
                lb++;
            }
            else break;
        }
        newLength = from + len - lb->second;
        CSubPatchStr last = CSubPatchStr(lb->first.getOffset(), newLength, lb->first.getStr());
        out.vec.push_back(make_pair(last, newGlobOffset));
        out.length = len;
        return out;
    }

    CPatchStr &Append(const CPatchStr &src) {
        for (auto const &str : src.vec) {
            vec.push_back(make_pair(str.first, length));
            length += str.first.getLength();
        }
        return *this;
    }

    CPatchStr &Insert(size_t pos, const CPatchStr &ssrc) {
        CPatchStr src = ssrc;
        if (pos > length || pos < 0)
            throw InvalidIndexException();
        auto lb = vec.lower_bound(pos);
        CSubPatchStr last;
        size_t nextOffset;
        if (lb == vec.end())
            nextOffset = length;
        else
            nextOffset = lb->second;
        if (lb->second != pos && lb->first.getStr() != nullptr) {
            size_t lastLength = lb->first.getLength();
            lb->first.setLength(pos - lb->second);
            last.setStr(lb->first.getStr());
            last.setOffset(lb->first.getLength() + lb->first.getOffset());
            last.setLength(lb->second + lastLength - pos);
            nextOffset = lb->second + lb->first.getLength();
            lb++;
        }
        for (auto const &str : src.vec) {
            vec.insert(lb, make_pair(str.first, nextOffset));
            nextOffset += str.first.getLength();
            length += str.first.getLength();
            lb++;
        }
        if (last.getStr() != nullptr) {
            vec.insert2(lb, make_pair(last, nextOffset));
//            length += last.getLength();
        }
        return *this;
    }

    CPatchStr &Delete(size_t from, size_t len) {
        if (from + len > length)
            throw InvalidIndexException();
        if (len == 0)
            return *this;
        auto lb = vec.lower_bound(from);
        size_t oldLen = lb->first.getLength();
        lb->first.setLength(from - lb->second);
        size_t decreaseOffset = oldLen - lb->first.getLength();
        if (from + len == lb->second + oldLen) {
            for (auto it = lb + 1; it != vec.end(); it++) {
                it->second -= oldLen - lb->first.getLength();
            }
            length -= len;
            return *this;
        }
        if (from + len <= lb->second + oldLen) {
            CSubPatchStr next = CSubPatchStr(lb->first.getOffset() + len + lb->first.getLength(), oldLen - lb->first.getLength() - len, lb->first.getStr());
            vec.insert(lb+1, make_pair(next, lb->second + lb->first.getLength()));
            // decrease offset of all continuous substrings
            for (auto it = lb + 2; it != vec.end(); it++) {
                it->second -= len + (lb+1)->first.getLength();
            }
            length -= len;
            return *this;
        }
        lb++;
        if (lb == vec.end()) {
            length -= len;
            return *this;
        }
        while (from + len > lb->second + lb->first.getLength()) {
            decreaseOffset += lb->first.getLength();
            vec.erase(lb);
        }
        size_t oldOffset = lb->first.getOffset();
        lb->first.setOffset(oldOffset + from + len - lb->second);
        oldLen = lb->first.getLength();
        lb->first.setLength(lb->second + lb->first.getLength() - from - len);
        lb->second -= decreaseOffset;
        decreaseOffset += oldLen - lb->first.getLength();
        // decrease offset of all continuous substrings
        for (auto it = lb + 1; it != vec.end(); it++) {
            it->second -= decreaseOffset;
        }
        length -= len;
        return *this;
    }

    void Print() const {
        for (auto &str : vec) {
            char *out = new char[str.first.getLength()];
            for (size_t j = str.first.getOffset(); j < str.first.getLength() + str.first.getOffset(); j++) {
                out[j - str.first.getOffset()] = (*(str.first.getStr()))[j];
            }
            cout << out << ", O:" << str.first.getOffset() << ", L:" << str.first.getLength() << ", OS:" << str.second << endl;
        }
    }

    size_t Length() const {
        return length;
    }

    char *ToStr(void) const {
        char *out = new char[length+1];
        out[length] = '\0';
        if (length == 0)
            return out;
        size_t l = 0;
        for (size_t i = 0; i < vec.size(); i++) {
            for (size_t j = vec[i].first.getOffset(); j < vec[i].first.getLength() + vec[i].first.getOffset(); j++) {
                out[l] = (*(vec[i].first.getStr()))[j];
                l++;
                if (l == length) {
                    out[length] = '\0';
//                    cout << "len: " << length << " ";
                    return out;
                }
            }
        }
//        cout << "len: " << length << " ";
        return out;
    }
};

#ifndef __PROGTEST__

bool stringMatch(char *str,
                 const char *expected) {
    bool res = strcmp(str, expected) == 0;
    delete[] str;
    return res;
}

int main(void) {
    char tmpStr[100];
    
//    CPatchStr a("test");
//    assert (stringMatch(a.ToStr(), "test"));
////    cout << a.ToStr() << endl;
//    strncpy(tmpStr, " da", sizeof(tmpStr));
//    a.Append(tmpStr);
////    cout << a.ToStr() << endl;
//    assert (stringMatch(a.ToStr(), "test da"));
//    strncpy(tmpStr, "ta", sizeof(tmpStr));
//    a.Append(tmpStr);
//    assert (stringMatch(a.ToStr(), "test data"));
//    strncpy(tmpStr, "foo text", sizeof(tmpStr));
//    CPatchStr b(tmpStr);
//    assert (stringMatch(b.ToStr(), "foo text"));
//    CPatchStr c(a);
//    assert (stringMatch(c.ToStr(), "test data"));
//    CPatchStr d(a.SubStr(3, 5));
//    assert (stringMatch(d.ToStr(), "t dat"));
//    d.Append(b);
//    assert (stringMatch(d.ToStr(), "t datfoo text"));
//    d.Append(b.SubStr(3, 4));
////    cout << d.ToStr() << endl;
//    assert (stringMatch(d.ToStr(), "t datfoo text tex"));
//    c.Append(d);
//    assert (stringMatch(c.ToStr(), "test datat datfoo text tex"));
//    c.Append(c);
////    cout << d.ToStr() << endl;
////    cout << c.SubStr(6, 9).ToStr() << endl;
//    assert (stringMatch(c.ToStr(), "test datat datfoo text textest datat datfoo text tex"));
//    d.Insert(2, c.SubStr(6, 9));
////    cout << d.ToStr() << endl;
//    assert (stringMatch(d.ToStr(), "t atat datfdatfoo text tex"));
//    b = "abcdefgh";
//    assert (stringMatch(b.ToStr(), "abcdefgh"));
//    assert (stringMatch(b.ToStr(), "abcdefgh"));
//    assert (stringMatch(d.ToStr(), "t atat datfdatfoo text tex"));
////    cout << d.ToStr() << endl;
////    d.Print();
////    cout << d.SubStr(4, 8).ToStr() << endl;
//    assert (stringMatch(d.SubStr(4, 8).ToStr(), "at datfd"));
////    cout << b.SubStr(2, 6).ToStr() << endl;
//
//    assert (stringMatch(b.SubStr(2, 6).ToStr(), "cdefgh"));
//    try {
//        b.SubStr(2, 7).ToStr();
//        assert ("Exception not thrown" == NULL);
//    }
//    catch (InvalidIndexException &e) {
//    }
//    catch (...) {
//        assert ("Invalid exception thrown" == NULL);
//    }
////    cout << a.ToStr() << endl;
////    a.Print();
//    a.Delete(3, 5);
////    cout << a.ToStr() << endl;
////    a.Print();
//    assert (stringMatch(a.ToStr(), "tesa"));

    return 0;
}

#endif /* __PROGTEST__ */
