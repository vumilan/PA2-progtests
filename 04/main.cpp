#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <memory>
using namespace std;

class InvalidIndexException
{
};

#endif /* __PROGTEST__ */

class String {
    size_t size;
    char *buffer;
public:
    String();

    String(const String &);

    String(const char *);

    void del() {
        delete[] buffer;
    }

    ~String();

    size_t length() const;

    void resize(unsigned int, char);

    void insert(unsigned int, String &);

    char &operator[](unsigned int) const;

    void operator=(const String &);

    void operator+=(const String &str) {
        char *newBuffer = new char[size + str.size + 1];
        newBuffer[size + str.size] = '\0';
        for (size_t i = 0; i < size; i++) {
            newBuffer[i] = buffer[i];
        }
        for (size_t i = size; i < size + str.size; i++) {
            newBuffer[i] = str.buffer[i-size];
        }
        size += str.size;
        delete[] buffer;
        buffer = newBuffer;
    }

    void Insert ( size_t pos, const String & src ) {
        char *newBuffer = new char[size + src.size + 1];
        newBuffer[size + src.size] = '\0';
        size_t i = 0;
        for (i = 0; i < pos; i++) {
            newBuffer[i] = buffer[i];
        }
        for (i = pos; i < pos + src.size; i++) {
            newBuffer[i] = src.buffer[i-pos];
        }
        for (i = pos + src.size; i < size + src.size; i++) {
            newBuffer[i] = buffer[i - src.size];
        }
        size += src.size;
        delete[] buffer;
        buffer = newBuffer;
    }
    void Delete    ( size_t            from,
                            size_t            len ) {
        char *newBuffer = new char[size - len + 1];
        newBuffer[size - len] = '\0';
        for (size_t i = 0; i < from; i++) {
            newBuffer[i] = buffer[i];
        }
        for (size_t i = from + len; i < size; i++) {
            newBuffer[i - len] = buffer[i];
        }
        size -= len;
        delete[] buffer;
        buffer = newBuffer;
    }

    // other methods
    friend bool operator==(const String &, const String &);

    friend bool operator<=(const String &, const String &);

    friend bool operator<(const String &, const String &);

    friend ostream &operator<<(ostream &, const String &);

    char *toChar() const {
        char *out = new char[size + 1];
        memcpy(out, buffer, size + 1);
        out[size] = '\0';
        return out;
    }
};

String::String() {
    buffer = nullptr;
    size = 0;
}

String::String(const String &s) {
    size = s.size;
    buffer = new char[size + 1];
    for (size_t i = 0; i < size; i++) {
        buffer[i] = s.buffer[i];
    }
    buffer[size] = '\0';
}

String::String(const char *p) {
    size_t i = 0;
    const char *t = p;

    while (*p++) {
        i++;
    }

    buffer = new char[i + 1];
    size_t j = 0;

    for (j = 0; *t; t++, j++) {
        buffer[j] = *t;
    }
    size = j;
    buffer[size] = '\0';
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

char &String::operator[](unsigned int x) const {
    return buffer[x];
}

void String::operator=(const String &s) {
    if (buffer)
        delete[] buffer;
    buffer = new char[s.size + 1];
    strcpy(buffer, s.buffer);
    buffer[s.size] = '\0';
    size = s.size;
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

class CPatchStr
{
public:
    CPatchStr ( void ) {
        buffer = "";
    }
    CPatchStr ( const char * src ) {
        buffer = String(src);
    }
    // copy constructor
    CPatchStr ( const CPatchStr &src ) {
        buffer = src.buffer;
    }
    // destructor
    // operator =
    void operator= ( const char * src ) {
        buffer = String(src);
    }
    ~CPatchStr() {
//        buffer.del();
    }
    size_t Length() {
        return buffer.length();
    }
    CPatchStr   SubStr    ( size_t            from,
    size_t            len ) const {
        if (from + len > buffer.length())
            throw InvalidIndexException();
        if (len == 0)
            return CPatchStr("");
        char *out = new char[len + 1];
        for (size_t i = from; i < from + len; i++) {
            out[i - from] = buffer.toChar()[i];
        }
        out[len] = '\0';
        return CPatchStr(out);
    }
    CPatchStr & Append    ( const CPatchStr & src ) {
        buffer += src.buffer;
        return *this;
    }

    CPatchStr & Insert    ( size_t            pos,
                            const CPatchStr & src ) {
        if (pos > buffer.length() || pos < 0)
            throw InvalidIndexException();
        buffer.Insert(pos, src.buffer);
        return *this;
    }
    CPatchStr & Delete    ( size_t            from,
                            size_t            len ) {
        if (from + len > buffer.length())
            throw InvalidIndexException();
        if (len == 0)
            return *this;
        buffer.Delete(from, len);
        return *this;
    }
    char      * ToStr     ( void ) const {
        return buffer.toChar();
    }
private:
    String buffer;
};

#ifndef __PROGTEST__
bool stringMatch ( char       * str,
                   const char * expected )
{
    bool res = strcmp ( str, expected ) == 0;
    delete [] str;
    return res;
}

int main ( void )
{
    char tmpStr[100];
    return 0;
}
#endif /* __PROGTEST__ */
