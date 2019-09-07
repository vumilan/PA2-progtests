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
        if ((lbLo-1)->getHi() + 1 >= range.getLo()) {
            newLo = Min((lbLo-1)->getLo(), range.getLo());
            leftOffset = -1;
        }
    }
    // test mild intersection with range to the right
    if (lbHi != correctRanges.end()) {
        if (lbHi->getLo() - 1 <= range.getHi()) {
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
    a += CRange(5, 10);
    a += CRange(1, 30);
    a -= CRange(5, 10);
    a -= CRange(5, 10);
    assert(!a.Includes(5));
    assert(a.Includes(11));
    a += CRange(5, 10);
    a += CRange(-30, 0);
    a += CRange(-60, -45);
    a += CRange(100, 250);
    a += CRange(300, 550);
    a += CRange(-42, -42);
    a += CRange(LLONG_MAX-3, LLONG_MAX);
    b += a;
    cout << b.Includes(LLONG_MAX-4) << endl;
    cout << b.Includes(-42) << endl;
    cout << b.Includes(43) << endl;
    cout << b.Includes(-60) << endl;
    cout << b.Includes(-45) << endl;
    cout << b.Includes(299) << endl;
    cout << b.Includes(-300) << endl;
    cout << b.Includes(350) << endl;


    cout << toString(a) << endl;

    cout << toString(b) << endl;
#ifdef EXTENDED_SYNTAX
    CRangeList x { { 5, 20 }, { 150, 200 }, { -9, 12 }, { 48, 93 } };
      assert ( toString ( x ) == "{<-9..20>,<48..93>,<150..200>}" );
      ostringstream oss;
      oss << setfill ( '=' ) << hex << left;
      for ( const auto & v : x + CRange ( -100, -100 ) )
        oss << v << endl;
      oss << setw ( 10 ) << 1024;
      assert ( oss . str () == "-100\n<-9..20>\n<48..93>\n<150..200>\n400=======" );
    return 0;
#endif /* EXTENDED_SYNTAX */
}

#endif /* __PROGTEST__ */
