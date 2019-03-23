#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>

using namespace std;
#endif /* __PROGTEST__ */

string toUpper(string str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

class CCity;

class COwner {
public:
    COwner(const string &name) : name(name) {};

    string Name(void) const {
        return name;
    }

    void AddCity(CCity *city);

    vector<CCity *> &getOwnedCitiesVec() {
        return ownedCities;
    }

    void unOwnCity(CCity *city);

private:
    static bool compareCityAddIndex(const CCity *cityA, const CCity *cityB);

    string name;
    unsigned int lastCityIndex = 0;
    vector<CCity *> ownedCities;
};

class CCity {
public:
    CCity(const string &name, const string &addr, const string &region, unsigned int id)
            : name(name), addr(addr), region(region), id(id) {};

    void setOwner(COwner *owner) {
        this->owner = owner;
    }

    COwner *getOwner() {
        return owner;
    }

    void setAddIndex(unsigned int index) {
        addIndex = index;
    }

    unsigned int getAddIndex() const {
        return addIndex;
    }

    string Name() const {
        return name;
    }

    string Addr() const {
        return addr;
    }

    string Region() const {
        return region;
    }

    unsigned int Id() const {
        return id;
    }

    string Owner() const {
        return ownerCaseSensitiveName;
    }

    bool SelTo(COwner *newOwner, const string &newOwnerCaseSensitiveName) {
        if (toUpper(this->owner->Name()) == toUpper(newOwner->Name()))
            return false;
        this->owner->unOwnCity(this);
        newOwner->AddCity(this);
        this->owner = newOwner;
        ownerCaseSensitiveName = newOwnerCaseSensitiveName;
        return true;
    }

private:
    string name;
    string addr;
    string region;
    unsigned int id;
    unsigned int addIndex = 0;
    string ownerCaseSensitiveName = "";
    COwner *owner = nullptr;
};

void COwner::AddCity(CCity *city) {
    city->setAddIndex(lastCityIndex);
    lastCityIndex++;
    ownedCities.push_back(city);
}

bool COwner::compareCityAddIndex(const CCity *cityA, const CCity *cityB) {
    return cityA->getAddIndex() < cityB->getAddIndex();
}

void COwner::unOwnCity(CCity *city) {
    auto lower = lower_bound(ownedCities.begin(), ownedCities.end(), city, compareCityAddIndex);
    ownedCities.erase(lower);
}

class CIterator {
public:
    CIterator(const vector<CCity *> &vecCities) : vecCities(vecCities) {}

    bool AtEnd(void) const {
        return index >= vecCities.size();
    }

    void Next(void) {
        index++;
    }

    string City(void) const {
        return vecCities[index]->Name();
    }

    string Addr(void) const {
        return vecCities[index]->Addr();
    }

    string Region(void) const {
        return vecCities[index]->Region();
    }

    unsigned ID(void) const {
        return vecCities[index]->Id();
    }

    string Owner(void) const {
        return vecCities[index]->Owner();
    }

    unsigned int index = 0;
    const vector<CCity *> vecCities;
};

class CLandRegister {
public:
    CLandRegister();

    ~CLandRegister();

    bool Add(const string &city,
             const string &addr,
             const string &region,
             unsigned int id);

    bool Del(const string &city,
             const string &addr);

    bool Del(const string &region,
             unsigned int id);

    bool GetOwner(const string &city,
                  const string &addr,
                  string &owner) const;

    bool GetOwner(const string &region,
                  unsigned int id,
                  string &owner) const;

    bool NewOwner(const string &city,
                  const string &addr,
                  const string &owner);

    bool NewOwner(const string &region,
                  unsigned int id,
                  const string &owner);

    unsigned Count(const string &owner) const;

    CIterator ListByAddr(void) const;

    CIterator ListByOwner(const string &owner) const;

private:
    vector<CCity *> VecCityAddr;
    vector<CCity *> VecRegionId;
    vector<COwner *> Owners;
    unsigned int numberOfCities;
    unsigned int numberOfOwners;

    static bool ownersNameCompare(const COwner *ownerA, const COwner *ownerB);

    static bool cityCityAddrCompare(const CCity *cityA, const CCity *cityB);

    static bool cityCityRegionIdCompare(const CCity *cityA, const CCity *cityB);
};

CLandRegister::CLandRegister(void) {
    VecCityAddr.reserve(1000);
    VecRegionId.reserve(1000);
    Owners.reserve(1000);
    numberOfCities = 1000;
    numberOfOwners = 1000;
}

CLandRegister::~CLandRegister() {
    for (auto &it : VecCityAddr) {
        delete it;
    }
    for (auto &it : Owners) {
        delete it;
    }
}

// function to compare names between two owners, case insensitive
bool CLandRegister::ownersNameCompare(const COwner *ownerA, const COwner *ownerB) {
    return toUpper(ownerA->Name()) < toUpper(ownerB->Name());
}

// compare cities by name and addr
bool CLandRegister::cityCityAddrCompare(const CCity *cityA, const CCity *cityB) {
    if (cityA->Name() == cityB->Name()) {
        return cityA->Addr() < cityB->Addr();
    }
    return cityA->Name() < cityB->Name();
}

// compare cities by region and id
bool CLandRegister::cityCityRegionIdCompare(const CCity *cityA, const CCity *cityB) {
    if (cityA->Region() == cityB->Region()) {
        return cityA->Id() < cityB->Id();
    }
    return cityA->Region() < cityB->Region();
}

bool CLandRegister::Add(const string &city, const string &addr, const string &region, unsigned int id) {
    // first check whether city is already in register
    auto *ptrCity = new CCity(city, addr, region, id);
    auto lower = lower_bound(VecCityAddr.begin(), VecCityAddr.end(), ptrCity, cityCityAddrCompare);
    if (!VecCityAddr.empty()) {
        auto upper = upper_bound(VecCityAddr.begin(), VecCityAddr.end(), ptrCity, cityCityAddrCompare);
        if (lower != upper) {
            delete ptrCity;
            return false;
        }
    }
    auto it = lower;
    lower = lower_bound(VecRegionId.begin(), VecRegionId.end(), ptrCity, cityCityRegionIdCompare);
    if (!VecRegionId.empty()) {
        auto upper = upper_bound(VecRegionId.begin(), VecRegionId.end(), ptrCity, cityCityRegionIdCompare);
        if (lower != upper) {
            delete ptrCity;
            return false;
        }
    }
    // resize vectors if its full
    numberOfCities++;
    if (numberOfCities <= VecCityAddr.size()) {
        VecCityAddr.resize(2 * numberOfCities);
        VecRegionId.resize(2 * numberOfCities);
    }
    // insert new city into vectors
    VecCityAddr.insert(it, ptrCity);
    VecRegionId.insert(lower, ptrCity);
    // check if owner is in vector
    auto *ptrOwner = new COwner("");
    auto lower_owner = lower_bound(Owners.begin(), Owners.end(), ptrOwner, ownersNameCompare);
    if (!Owners.empty()) {
        auto upper_owner = upper_bound(Owners.begin(), Owners.end(), ptrOwner, ownersNameCompare);
        // found owner in vector
        if (lower_owner != upper_owner) {
            ptrCity->setOwner(*lower_owner);
            (*lower_owner)->AddCity(ptrCity);
            delete ptrOwner;
            return true;
        }
    }
    // didnt find owner in vector
    ptrCity->setOwner(ptrOwner);
    ptrOwner->AddCity(ptrCity);
    Owners.insert(lower_owner, ptrOwner);
    numberOfOwners++;
    if (numberOfOwners <= Owners.size()) {
        Owners.resize(2 * numberOfOwners);
    }
    return true;
}

CIterator CLandRegister::ListByAddr() const {
    return CIterator(VecCityAddr);
}

CIterator CLandRegister::ListByOwner(const string &owner) const {
    auto *ptrOwner = new COwner(owner);
    auto lower_owner = lower_bound(Owners.begin(), Owners.end(), ptrOwner, ownersNameCompare);
    if (!Owners.empty()) {
        auto upper_owner = upper_bound(Owners.begin(), Owners.end(), ptrOwner, ownersNameCompare);
        // found owner in vector
        if (lower_owner != upper_owner) {
            delete ptrOwner;
            return CIterator(((*lower_owner)->getOwnedCitiesVec()));
        }
    }
    delete ptrOwner;
    return CIterator(vector<CCity *>());
}

unsigned CLandRegister::Count(const string &owner) const {
    auto *ptrOwner = new COwner(owner);
    auto lower = lower_bound(Owners.begin(), Owners.end(), ptrOwner, ownersNameCompare);
    if (!Owners.empty()) {
        auto upper = upper_bound(Owners.begin(), Owners.end(), ptrOwner, ownersNameCompare);
        // found owner in vector
        if (lower != upper) {
            delete ptrOwner;
            return (unsigned int)(*lower)->getOwnedCitiesVec().size();
        }
    }
    delete ptrOwner;
    return 0;
}

bool CLandRegister::NewOwner(const string &city, const string &addr, const string &owner) {
    if (VecCityAddr.empty())
        return false;
    // check if city is in vector
    auto *ptrCity = new CCity(city, addr, "", 0);
    auto lower = lower_bound(VecCityAddr.begin(), VecCityAddr.end(), ptrCity, cityCityAddrCompare);
    if (lower == VecCityAddr.end() || (*lower)->Name() != city || (*lower)->Addr() != addr) {
        delete ptrCity;
        return false;
    }
    delete ptrCity;
    // check if owner is in vector
    auto *ptrOwner = new COwner(owner);
    auto lower_owner = lower_bound(Owners.begin(), Owners.end(), ptrOwner, ownersNameCompare);
    // owner not found in vector of owners
    if (lower_owner == Owners.end() || toUpper((*lower_owner)->Name()) != toUpper(owner)) {
        Owners.insert(lower_owner, ptrOwner);
        numberOfOwners++;
        if (numberOfOwners <= Owners.size()) {
            Owners.resize(2 * numberOfOwners);
        }
        return (*lower)->SelTo(ptrOwner, owner);
    }
    delete ptrOwner;
    return (*lower)->SelTo(*lower_owner, owner);
}

bool CLandRegister::NewOwner(const string &region, unsigned int id, const string &owner) {
    if (VecRegionId.empty())
        return false;
    // check if city is in vector
    auto *ptrCity = new CCity("", "", region, id);
    auto lower = lower_bound(VecRegionId.begin(), VecRegionId.end(), ptrCity, cityCityRegionIdCompare);
    if (lower == VecRegionId.end() || (*lower)->Region() != region || (*lower)->Id() != id) {
        delete ptrCity;
        return false;
    }
    delete ptrCity;
    // check if owner is in vector
    auto *ptrOwner = new COwner(owner);
    auto lower_owner = lower_bound(Owners.begin(), Owners.end(), ptrOwner, ownersNameCompare);
    // owner not found in vector of owners
    if (lower_owner == Owners.end() || toUpper((*lower_owner)->Name()) != toUpper(owner)) {
        Owners.insert(lower_owner, ptrOwner);
        numberOfOwners++;
        if (numberOfOwners <= Owners.size()) {
            Owners.resize(2 * numberOfOwners);
        }
        return (*lower)->SelTo(ptrOwner, owner);
    }
    delete ptrOwner;
    return (*lower)->SelTo(*lower_owner, owner);
}

bool CLandRegister::GetOwner(const string &city, const string &addr, string &owner) const {
    // check if city is in vector
    auto *ptrCity = new CCity(city, addr, "", 0);
    auto lower = lower_bound(VecCityAddr.begin(), VecCityAddr.end(), ptrCity, cityCityAddrCompare);
    if (!VecCityAddr.empty()) {
        auto upper = upper_bound(VecCityAddr.begin(), VecCityAddr.end(), ptrCity, cityCityAddrCompare);
        // found city
        if (lower != upper) {
            owner = (*lower)->Owner();
            delete ptrCity;
            return true;
        }
    }
    delete ptrCity;
    return false;
}

bool CLandRegister::GetOwner(const string &region, unsigned int id, string &owner) const {
    // check if city is in vector
    auto *ptrCity = new CCity("", "", region, id);
    auto lower = lower_bound(VecRegionId.begin(), VecRegionId.end(), ptrCity, cityCityRegionIdCompare);
    if (!VecRegionId.empty()) {
        auto upper = upper_bound(VecRegionId.begin(), VecRegionId.end(), ptrCity, cityCityRegionIdCompare);
        // found city
        if (lower != upper) {
            owner = (*lower)->Owner();
            delete ptrCity;
            return true;
        }
    }
    delete ptrCity;
    return false;
}

bool CLandRegister::Del(const string &city, const string &addr) {
    if (VecCityAddr.empty())
        return false;
    // check if city is in vector
    auto *ptrCity = new CCity(city, addr, "", 0);
    auto lower = lower_bound(VecCityAddr.begin(), VecCityAddr.end(), ptrCity, cityCityAddrCompare);
    if (lower == VecCityAddr.end() || (*lower)->Name() != city || (*lower)->Addr() != addr) {
        delete ptrCity;
        return false;
    }
    delete ptrCity;
    (*lower)->getOwner()->unOwnCity(*lower);
    auto *ptrCity2 = new CCity("", "", (*lower)->Region(), (*lower)->Id());
    auto lower2 = lower_bound(VecRegionId.begin(), VecRegionId.end(), ptrCity2, cityCityRegionIdCompare);
    if (lower2 == VecRegionId.end() || (*lower2)->Region() != ptrCity2->Region() || (*lower2)->Id() != ptrCity2->Id()) {
        delete ptrCity2;
        return false;
    }
    delete *lower2;
    delete ptrCity2;
    VecCityAddr.erase(lower);
    VecRegionId.erase(lower2);
    numberOfCities--;
    return true;
}

bool CLandRegister::Del(const string &region, unsigned int id) {
    if (VecRegionId.empty())
        return false;
    // check if city is in vector
    auto *ptrCity = new CCity("", "", region, id);
    auto lower = lower_bound(VecRegionId.begin(), VecRegionId.end(), ptrCity, cityCityRegionIdCompare);
    if (lower == VecRegionId.end() || (*lower)->Region() != region || (*lower)->Id() != id) {
        delete ptrCity;
        return false;
    }
    delete ptrCity;
    (*lower)->getOwner()->unOwnCity(*lower);
    auto *ptrCity2 = new CCity((*lower)->Name(), (*lower)->Addr(), "", 0);
    auto lower2 = lower_bound(VecCityAddr.begin(), VecCityAddr.end(), ptrCity2, cityCityAddrCompare);
    if (lower2 == VecCityAddr.end() || (*lower2)->Name() != ptrCity2->Name() || (*lower2)->Addr() != ptrCity2->Addr()) {
        delete ptrCity2;
        return false;
    }
    delete *lower2;
    delete ptrCity2;
    VecRegionId.erase(lower);
    VecCityAddr.erase(lower2);
    numberOfCities--;
    return true;
}

#ifndef __PROGTEST__

static void test0(void) {
    CLandRegister x;
    string owner;

    assert (x.Add("Prague", "Thakurova", "Dejvice", 12345));
    assert (x.Add("Prague", "Evropska", "Vokovice", 12345));
    assert (x.Add("Prague", "Technicka", "Dejvice", 9873));
    assert (x.Add("Plzen", "Evropska", "Plzen mesto", 78901));
    assert (x.Add("Liberec", "Evropska", "Librec", 4552));
    CIterator i0 = x.ListByAddr();
    assert (!i0.AtEnd()
            && i0.City() == "Liberec"
            && i0.Addr() == "Evropska"
            && i0.Region() == "Librec"
            && i0.ID() == 4552
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Plzen"
            && i0.Addr() == "Evropska"
            && i0.Region() == "Plzen mesto"
            && i0.ID() == 78901
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Evropska"
            && i0.Region() == "Vokovice"
            && i0.ID() == 12345
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Technicka"
            && i0.Region() == "Dejvice"
            && i0.ID() == 9873
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Thakurova"
            && i0.Region() == "Dejvice"
            && i0.ID() == 12345
            && i0.Owner() == "");
    i0.Next();
    assert (i0.AtEnd());

    assert (x.Count("") == 5);
    CIterator i1 = x.ListByOwner("");
    assert (!i1.AtEnd()
            && i1.City() == "Prague"
            && i1.Addr() == "Thakurova"
            && i1.Region() == "Dejvice"
            && i1.ID() == 12345
            && i1.Owner() == "");
    i1.Next();
    assert (!i1.AtEnd()
            && i1.City() == "Prague"
            && i1.Addr() == "Evropska"
            && i1.Region() == "Vokovice"
            && i1.ID() == 12345
            && i1.Owner() == "");
    i1.Next();
    assert (!i1.AtEnd()
            && i1.City() == "Prague"
            && i1.Addr() == "Technicka"
            && i1.Region() == "Dejvice"
            && i1.ID() == 9873
            && i1.Owner() == "");
    i1.Next();
    assert (!i1.AtEnd()
            && i1.City() == "Plzen"
            && i1.Addr() == "Evropska"
            && i1.Region() == "Plzen mesto"
            && i1.ID() == 78901
            && i1.Owner() == "");
    i1.Next();
    assert (!i1.AtEnd()
            && i1.City() == "Liberec"
            && i1.Addr() == "Evropska"
            && i1.Region() == "Librec"
            && i1.ID() == 4552
            && i1.Owner() == "");
    i1.Next();
    assert (i1.AtEnd());

    assert (x.Count("CVUT") == 0);
    CIterator i2 = x.ListByOwner("CVUT");
    assert (i2.AtEnd());

    assert (x.NewOwner("Prague", "Thakurova", "CVUT"));
    assert (x.NewOwner("Dejvice", 9873, "CVUT"));
    assert (!x.NewOwner("Dejvice", 9873, "cVUT"));
    assert (x.NewOwner("Plzen", "Evropska", "Anton Hrabis"));
    assert (x.NewOwner("Librec", 4552, "Cvut"));
    assert (x.GetOwner("Prague", "Thakurova", owner) && owner == "CVUT");
    assert (x.GetOwner("Dejvice", 12345, owner) && owner == "CVUT");
    assert (x.GetOwner("Prague", "Evropska", owner) && owner == "");
    assert (x.GetOwner("Vokovice", 12345, owner) && owner == "");
    assert (x.GetOwner("Prague", "Technicka", owner) && owner == "CVUT");
    assert (x.GetOwner("Dejvice", 9873, owner) && owner == "CVUT");
    assert (x.GetOwner("Plzen", "Evropska", owner) && owner == "Anton Hrabis");
    assert (x.GetOwner("Plzen mesto", 78901, owner) && owner == "Anton Hrabis");
    assert (x.GetOwner("Liberec", "Evropska", owner) && owner == "Cvut");
    assert (x.GetOwner("Librec", 4552, owner) && owner == "Cvut");
    CIterator i3 = x.ListByAddr();
    assert (!i3.AtEnd()
            && i3.City() == "Liberec"
            && i3.Addr() == "Evropska"
            && i3.Region() == "Librec"
            && i3.ID() == 4552
            && i3.Owner() == "Cvut");
    i3.Next();
    assert (!i3.AtEnd()
            && i3.City() == "Plzen"
            && i3.Addr() == "Evropska"
            && i3.Region() == "Plzen mesto"
            && i3.ID() == 78901
            && i3.Owner() == "Anton Hrabis");
    i3.Next();
    assert (!i3.AtEnd()
            && i3.City() == "Prague"
            && i3.Addr() == "Evropska"
            && i3.Region() == "Vokovice"
            && i3.ID() == 12345
            && i3.Owner() == "");
    i3.Next();
    assert (!i3.AtEnd()
            && i3.City() == "Prague"
            && i3.Addr() == "Technicka"
            && i3.Region() == "Dejvice"
            && i3.ID() == 9873
            && i3.Owner() == "CVUT");
    i3.Next();
    assert (!i3.AtEnd()
            && i3.City() == "Prague"
            && i3.Addr() == "Thakurova"
            && i3.Region() == "Dejvice"
            && i3.ID() == 12345
            && i3.Owner() == "CVUT");
    i3.Next();
    assert (i3.AtEnd());

    assert (x.Count("cvut") == 3);
    CIterator i4 = x.ListByOwner("cVuT");
    assert (!i4.AtEnd()
            && i4.City() == "Prague"
            && i4.Addr() == "Thakurova"
            && i4.Region() == "Dejvice"
            && i4.ID() == 12345
            && i4.Owner() == "CVUT");
    i4.Next();
    assert (!i4.AtEnd()
            && i4.City() == "Prague"
            && i4.Addr() == "Technicka"
            && i4.Region() == "Dejvice"
            && i4.ID() == 9873
            && i4.Owner() == "CVUT");
    i4.Next();
    assert (!i4.AtEnd()
            && i4.City() == "Liberec"
            && i4.Addr() == "Evropska"
            && i4.Region() == "Librec"
            && i4.ID() == 4552
            && i4.Owner() == "Cvut");
    i4.Next();
    assert (i4.AtEnd());

    assert (x.NewOwner("Plzen mesto", 78901, "CVut"));
    assert (x.Count("CVUT") == 4);
    CIterator i5 = x.ListByOwner("CVUT");
    assert (!i5.AtEnd()
            && i5.City() == "Prague"
            && i5.Addr() == "Thakurova"
            && i5.Region() == "Dejvice"
            && i5.ID() == 12345
            && i5.Owner() == "CVUT");
    i5.Next();
    assert (!i5.AtEnd()
            && i5.City() == "Prague"
            && i5.Addr() == "Technicka"
            && i5.Region() == "Dejvice"
            && i5.ID() == 9873
            && i5.Owner() == "CVUT");
    i5.Next();
    assert (!i5.AtEnd()
            && i5.City() == "Liberec"
            && i5.Addr() == "Evropska"
            && i5.Region() == "Librec"
            && i5.ID() == 4552
            && i5.Owner() == "Cvut");
    i5.Next();
    assert (!i5.AtEnd()
            && i5.City() == "Plzen"
            && i5.Addr() == "Evropska"
            && i5.Region() == "Plzen mesto"
            && i5.ID() == 78901
            && i5.Owner() == "CVut");
    i5.Next();
    assert (i5.AtEnd());

    assert (x.Del("Liberec", "Evropska"));
    assert (x.Del("Plzen mesto", 78901));
    assert (x.Count("cvut") == 2);
    CIterator i6 = x.ListByOwner("cVuT");
    assert (!i6.AtEnd()
            && i6.City() == "Prague"
            && i6.Addr() == "Thakurova"
            && i6.Region() == "Dejvice"
            && i6.ID() == 12345
            && i6.Owner() == "CVUT");
    i6.Next();
    assert (!i6.AtEnd()
            && i6.City() == "Prague"
            && i6.Addr() == "Technicka"
            && i6.Region() == "Dejvice"
            && i6.ID() == 9873
            && i6.Owner() == "CVUT");
    i6.Next();
    assert (i6.AtEnd());

    assert (x.Add("Liberec", "Evropska", "Librec", 4552));
}

static void test1(void) {
    CLandRegister x;
    string owner;

    assert (x.Add("Prague", "Thakurova", "Dejvice", 12345));
    assert (x.Add("Prague", "Evropska", "Vokovice", 12345));
    assert (x.Add("Prague", "Technicka", "Dejvice", 9873));
    assert (!x.Add("Prague", "Technicka", "Hradcany", 7344));
    assert (!x.Add("Brno", "Bozetechova", "Dejvice", 9873));
    assert (!x.GetOwner("Prague", "THAKUROVA", owner));
    assert (!x.GetOwner("Hradcany", 7343, owner));
    CIterator i0 = x.ListByAddr();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Evropska"
            && i0.Region() == "Vokovice"
            && i0.ID() == 12345
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Technicka"
            && i0.Region() == "Dejvice"
            && i0.ID() == 9873
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Thakurova"
            && i0.Region() == "Dejvice"
            && i0.ID() == 12345
            && i0.Owner() == "");
    i0.Next();
    assert (i0.AtEnd());

    assert (x.NewOwner("Prague", "Thakurova", "CVUT"));
    assert (!x.NewOwner("Prague", "technicka", "CVUT"));
    assert (!x.NewOwner("prague", "Technicka", "CVUT"));
    assert (!x.NewOwner("dejvice", 9873, "CVUT"));
    assert (!x.NewOwner("Dejvice", 9973, "CVUT"));
    assert (!x.NewOwner("Dejvice", 12345, "CVUT"));
    assert (x.Count("CVUT") == 1);
    CIterator i1 = x.ListByOwner("CVUT");
    assert (!i1.AtEnd()
            && i1.City() == "Prague"
            && i1.Addr() == "Thakurova"
            && i1.Region() == "Dejvice"
            && i1.ID() == 12345
            && i1.Owner() == "CVUT");
    i1.Next();
    assert (i1.AtEnd());

    assert (!x.Del("Brno", "Technicka"));
    assert (!x.Del("Karlin", 9873));
    assert (x.Del("Prague", "Technicka"));
    assert (!x.Del("Prague", "Technicka"));
    assert (!x.Del("Dejvice", 9873));
}

static void test2(void) {
    CLandRegister x;
    string owner;

    assert (x.Add("CITYA", "ADDRA", "REGIONA", 12345));
    assert (x.Add("CITYA", "ADDRB", "REGIONA", 12344));
    assert (x.Add("CITYA", "ADDRC", "REGIONA", 12343));
    assert (x.Add("CITYB", "ADDRA", "REGIONA", 1));
    assert (x.Add("CITYB", "ADDRB", "REGIONA", 12));
    assert (x.Add("CITYB", "ADDRC", "REGIONA", 123));
    cout << x.Count("") << endl;
    assert (x.NewOwner("CITYA", "ADDRA", "GOOG"));
    assert (x.NewOwner("CITYA", "ADDRB", "GOOG"));
    assert (x.NewOwner("CITYA", "ADDRC", "GOOG"));
    assert (x.NewOwner("CITYB", "ADDRA", "GOOG"));
    assert (x.NewOwner("CITYB", "ADDRB", "GOOG"));
    assert (x.NewOwner("CITYB", "ADDRC", "GOOG"));
    cout << x.Count("GOOG") << endl;
    assert (x.NewOwner("CITYA", "ADDRA", "DADA"));
    assert (x.NewOwner("CITYA", "ADDRB", "DADA"));
    cout << x.Count("DADA") << endl;
    assert (x.NewOwner("CITYA", "ADDRA", ""));
    assert (x.NewOwner("CITYA", "ADDRB", ""));
    cout << x.Count("") << endl;
    assert (x.NewOwner("CITYA", "ADDRA", "SS"));
    assert (x.NewOwner("CITYA", "ADDRB", "SS"));
    cout << x.Count("GOOG") << endl;
    assert (x.Del("CITYA", "ADDRC"));
    assert (x.Del("CITYB", "ADDRA"));
    assert (x.Del("CITYB", "ADDRB"));
    assert (x.Del("CITYB", "ADDRC"));
    CIterator i1 = x.ListByOwner("GOOD");
    assert (i1.AtEnd());
    cout << x.Count("GOOG") << endl;
}

int main(void) {
    test0();
    test1();
    test2();
    return 0;
}

#endif /* __PROGTEST__ */
