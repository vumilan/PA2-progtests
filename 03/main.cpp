#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>

using namespace std;

class InvalidRangeException {

};

#endif /* __PROGTEST__ */

// uncomment if your code implements initializer lists
#define EXTENDED_SYNTAX

long long Min(const long long &a, const long long &b) {
    if (a < b)
        return a;
    return b;
}

long long Max(const long long &a, const long long &b) {
    if (a > b)
        return a;
    return b;
}

class CRangeList;

class CRange {
public:
    CRange(long long lo, long long hi) {
        if (lo > hi)
            throw InvalidRangeException();
        this->lo = lo;
        this->hi = hi;
    }

    bool operator==(const CRange &range) const;
    bool operator!=(const CRange &range) const;

    CRange plusminus(const CRange &range1, bool plus) const;
    CRange operator+(const CRange &range) const;
    CRange operator-(const CRange &range) const;

    long long int getLo() const;

    long long int getHi() const;

    string print() const;

private:
    long long lo;
    long long hi;
};

long long int CRange::getLo() const {
    return lo;
}

long long int CRange::getHi() const {
    return hi;
}

bool CRange::operator==(const CRange &range) const {
    return (lo == range.getLo() && hi == range.getHi());
}

bool CRange::operator!=(const CRange &range) const {
    return !(lo == range.getLo() && hi == range.getHi());
}

static vector<pair<CRange, bool>> notEditedRanges;

CRange CRange::plusminus(const CRange &range, bool plus) const {
    if (notEditedRanges.empty()) {
        notEditedRanges.emplace_back(make_pair(*this, true));
    }
    notEditedRanges.emplace_back(make_pair(range, plus));
    return CRange{0, 0};
}

CRange CRange::operator+(const CRange &range) const {
    return plusminus(range, true);
}

CRange CRange::operator-(const CRange &range) const {
    return plusminus(range, false);
}

class CRangeList {
public:
    CRangeList() {}
    CRangeList(initializer_list<CRange> init);

    // copy constructors
    CRangeList &operator=(const CRange &range);

    CRangeList &operator=(const CRangeList &rangeList);

    // += range / range list
    CRangeList &correctAppend(const CRange &range);
    CRangeList &operator+=(const CRange &range);
    CRangeList operator+(const CRange &range) const;
    CRangeList &operator+=(const CRangeList &rangeList);

    // -= range / range list
    CRangeList &correctDelete(const CRange &range);
    CRangeList &operator-=(const CRange &range);
    CRangeList &operator-=(const CRangeList &rangeList);

    vector<CRange>::iterator begin() {
        return correctRanges.begin();
    }

    vector<CRange>::iterator end() {
        return correctRanges.end();
    }

    const vector<CRange> &getRanges() const;

    // Includes long long / range
    bool Includes(long long number) const;
    bool Includes(const CRange &range) const;

    // operator ==
    bool operator==(const CRangeList &rangeList) const;
    // operator !=
    bool operator!=(const CRangeList &rangeList) const;

    // operator <<
    string print() const;
private:
    // vector of ranges where intersecting ranges merged into one
    vector<CRange> correctRanges;
    static bool compareLo(const CRange &lhs,const CRange &rhs) {
        return lhs.getLo() < rhs.getLo();
    }
    static bool compareHi(const CRange &lhs,const CRange &rhs) {
        return lhs.getHi() < rhs.getHi();
    }
};

CRangeList::CRangeList(initializer_list<CRange> init) {
    correctRanges.clear();
    for (CRange range : init) {
        correctAppend(range);
    }
}

// operator ==, !=
bool CRangeList::operator==(const CRangeList &rangeList) const {
    const vector<CRange> &rangeListToCompare = rangeList.getRanges();
    for (unsigned i = 0; i < correctRanges.size(); i++) {
        if (correctRanges[i] != rangeListToCompare[i])
            return false;
    }
    return correctRanges.size() == rangeListToCompare.size();
}

bool CRangeList::operator!=(const CRangeList &rangeList) const {
    return !(*this == rangeList);
}

// Includes long long / range
bool CRangeList::Includes(long long number) const {
    if (correctRanges.empty())
        return false;
    CRange range(number, number);
    auto lbHi = lower_bound(correctRanges.begin(), correctRanges.end(), range, compareHi);
    if (lbHi == correctRanges.end())
        return false;
    return lbHi->getLo() <= number;
}

bool CRangeList::Includes(const CRange &range) const {
    if (correctRanges.empty())
        return false;
    auto lbHi = lower_bound(correctRanges.begin(), correctRanges.end(), range, compareHi);
    if (lbHi == correctRanges.end())
        return false;
    return lbHi->getLo() <= range.getLo();
}


// copy constructors
CRangeList &CRangeList::operator=(const CRange &range) {
    if (notEditedRanges.empty()) {
        correctRanges.clear();
        correctRanges.push_back(range);
    }
    else {
        correctRanges.clear();
        for (auto &pairInVec : notEditedRanges) {
            if (pairInVec.second)
                correctAppend(pairInVec.first);
            else
                correctDelete(pairInVec.first);
        }
        notEditedRanges.clear();
    }
    return *this;
}

CRangeList &CRangeList::operator=(const CRangeList &rangeList) {
    correctRanges.clear();
    for (auto &range : rangeList.getRanges()) {
        correctRanges.push_back(range);
    }
    return *this;
}

// += operator, +operator
CRangeList &CRangeList::correctAppend(const CRange &range) {
    if (correctRanges.empty()) {
        correctRanges.emplace_back(range);
        return *this;
    }
    long long newLo = range.getLo();
    long long newHi = range.getHi();
    int leftOffset = 0, rightOffset = 0;
    auto lbLo = lower_bound(correctRanges.begin(), correctRanges.end(), range, compareLo);
    auto lbHi = lower_bound(correctRanges.begin(), correctRanges.end(), range, compareHi);
    if (lbHi != correctRanges.end() && lbHi->getLo() <= range.getLo() && lbHi->getHi() >= range.getHi())
        return *this;
    // test mild intersection with range to the left
    if (lbLo != correctRanges.begin()) {
        if ((lbLo-1)->getHi() >= range.getLo() || (lbLo-1)->getHi() >= range.getLo()-1) {
            newLo = Min((lbLo-1)->getLo(), range.getLo());
            leftOffset = -1;
        }
    }
    // test mild intersection with range to the right
    if (lbHi != correctRanges.end()) {
        if (lbHi->getLo() <= range.getHi() || lbHi->getLo() <= range.getHi()+1) {
            newHi = Max(lbHi->getHi(), range.getHi());
            rightOffset = 1;
        }
    }
    for (int i = 0; i < (int)distance(lbLo, lbHi) + rightOffset - leftOffset; i++) {
        correctRanges.erase(lbLo+leftOffset);
    }
    correctRanges.insert(lbLo+leftOffset, CRange(newLo, newHi));
    return *this;
}

CRangeList& CRangeList::correctDelete(const CRange &range) {
    if (correctRanges.empty()) {
        return *this;
    }
    correctAppend(range);
    auto lbHi = lower_bound(correctRanges.begin(), correctRanges.end(), range, compareHi);
    long long newLo1 = Min(lbHi->getLo(), range.getLo());
    long long newHi1 = range.getLo();
    long long newLo2 = range.getHi();
    long long newHi2 = Max(lbHi->getHi(), range.getHi());
    correctRanges.erase(lbHi);
    if (newLo2 != newHi2)
        correctRanges.insert(lbHi, CRange(newLo2 + 1, newHi2));
    if (newLo1 != newHi1)
        correctRanges.insert(lbHi, CRange(newLo1, newHi1 - 1));
    return *this;
}

CRangeList& CRangeList::operator-=(const CRange &range) {
    if (notEditedRanges.empty()) {
        correctDelete(range);
    }
    else {
        for (auto &pairInVec : notEditedRanges) {
            if (pairInVec.second)
                correctDelete(pairInVec.first);
            else
                correctAppend(pairInVec.first);
        }
        notEditedRanges.clear();
    }
    return *this;
}

CRangeList& CRangeList::operator-=(const CRangeList &rangeList) {
    for (auto &range : rangeList.getRanges()) {
        this->correctDelete(range);
    }
    return *this;
}

CRangeList &CRangeList::operator+=(const CRange &range) {
    if (notEditedRanges.empty()) {
        correctAppend(range);
    }
    else {
        for (auto &pairInVec : notEditedRanges) {
            if (pairInVec.second)
                correctAppend(pairInVec.first);
            else
                correctDelete(pairInVec.first);
        }
        notEditedRanges.clear();
    }
    return *this;
}

CRangeList CRangeList::operator+(const CRange &range) const {
    CRangeList a = *this;
    a.correctAppend(range);
    return a;
}

CRangeList &CRangeList::operator+=(const CRangeList &rangeList) {
    for (auto &range : rangeList.getRanges()) {
        this->correctAppend(range);
    }
    return *this;
}

// print functions

string CRange::print() const {
    if (lo == hi)
        return to_string(lo);
    return '<' + to_string(lo) + ".." + to_string(hi) + '>';
}

string CRangeList::print() const {
    string output;
    output += '{';
    for (unsigned i = 0; i < correctRanges.size(); i++) {
        output += correctRanges[i].print();
        (i < correctRanges.size() - 1) ? output += "," : output += "";
    }
    output += '}';
    return output;
}

const vector<CRange> &CRangeList::getRanges() const {
    return correctRanges;
}

ostream &operator<<(ostream &out, const CRangeList &rangeList) { return out << rangeList.print(); }
ostream &operator<<(ostream &out, const CRange &range) { return out << range.print(); }

#ifndef __PROGTEST__

string toString(const CRangeList &x) {
    ostringstream oss;
    oss << x;
    return oss.str();
}

int main(void) {
    CRangeList a, b;

    assert ( sizeof ( CRange ) <= 2 * sizeof ( long long ) );
    a = CRange ( 5, 10 );
    a += CRange ( 25, 100 );
    assert ( toString ( a ) == "{<5..10>,<25..100>}" );
    a += CRange ( -5, 0 );
    a += CRange ( 8, 50 );
    assert ( toString ( a ) == "{<-5..0>,<5..100>}" );
    a += CRange ( 101, 105 ) + CRange ( 120, 150 ) + CRange ( 160, 180 ) + CRange ( 190, 210 );
    cout << toString(a) << endl;
    assert ( toString ( a ) == "{<-5..0>,<5..105>,<120..150>,<160..180>,<190..210>}" );
    a += CRange ( 106, 119 ) + CRange ( 152, 158 );
    assert ( toString ( a ) == "{<-5..0>,<5..150>,<152..158>,<160..180>,<190..210>}" );
    a += CRange ( -3, 170 );
    a += CRange ( -30, 1000 );
    assert ( toString ( a ) == "{<-30..1000>}" );
    b = CRange ( -500, -300 ) + CRange ( 2000, 3000 ) + CRange ( 700, 1001 );
    a += b;
    assert ( toString ( a ) == "{<-500..-300>,<-30..1001>,<2000..3000>}" );
    a -= CRange ( -400, -400 );
    assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..1001>,<2000..3000>}" );
    a -= CRange ( 10, 20 ) + CRange ( 900, 2500 ) + CRange ( 30, 40 ) + CRange ( 10000, 20000 );
    assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
    try
    {
        a += CRange ( 15, 18 ) + CRange ( 10, 0 ) + CRange ( 35, 38 );
        assert ( "Exception not thrown" == NULL );
    }
    catch ( const InvalidRangeException & e )
    {
    }
    catch ( ... )
    {
        assert ( "Invalid exception thrown" == NULL );
    }
    assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
    b = a;
    assert ( a == b );
    assert ( !( a != b ) );
    b += CRange ( 2600, 2700 );
    assert ( toString ( b ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
    assert ( a == b );
    assert ( !( a != b ) );
    b += CRange ( 15, 15 );
    assert ( toString ( b ) == "{<-500..-401>,<-399..-300>,<-30..9>,15,<21..29>,<41..899>,<2501..3000>}" );
    assert ( !( a == b ) );
    assert ( a != b );
    assert ( b . Includes ( 15 ) );
    assert ( b . Includes ( 2900 ) );
    assert ( b . Includes ( CRange ( 15, 15 ) ) );
    assert ( b . Includes ( CRange ( -350, -350 ) ) );
    assert ( b . Includes ( CRange ( 100, 200 ) ) );
    assert ( !b . Includes ( CRange ( 800, 900 ) ) );
    assert ( !b . Includes ( CRange ( -1000, -450 ) ) );
    assert ( !b . Includes ( CRange ( 0, 500 ) ) );
    a += CRange ( -10000, 10000 ) + CRange ( 10000000, 1000000000 );
    assert ( toString ( a ) == "{<-10000..10000>,<10000000..1000000000>}" );
    b += a;
    assert ( toString ( b ) == "{<-10000..10000>,<10000000..1000000000>}" );
    b -= a;
    assert ( toString ( b ) == "{}" );
    b += CRange ( 0, 100 ) + CRange ( 200, 300 ) - CRange ( 150, 250 ) + CRange ( 160, 180 ) - CRange ( 170, 170 );
    assert ( toString ( b ) == "{<0..100>,<160..169>,<171..180>,<251..300>}" );
    b -= CRange ( 10, 90 ) - CRange ( 20, 30 ) - CRange ( 40, 50 ) - CRange ( 60, 90 ) + CRange ( 70, 80 );
    cout << toString(b) << endl;
    assert ( toString ( b ) == "{<0..9>,<20..30>,<40..50>,<60..69>,<81..100>,<160..169>,<171..180>,<251..300>}" );
    b += CRange(302, 302);
    cout << toString(b) << endl;
#ifdef EXTENDED_SYNTAX
    CRangeList x { { 5, 20 }, { 150, 200 }, { -9, 12 }, { 48, 93 } };
      assert ( toString ( x ) == "{<-9..20>,<48..93>,<150..200>}" );
      ostringstream oss;
      oss << setfill ( '=' ) << hex << left;
      for ( const auto & v : x + CRange ( -100, -100 ) )
        oss << v << endl;
      oss << setw ( 10 ) << 1024;
      cout << oss . str () << endl;
      assert ( oss . str () == "-100\n<-9..20>\n<48..93>\n<150..200>\n400=======" );
    return 0;
#endif /* EXTENDED_SYNTAX */
}

#endif /* __PROGTEST__ */
