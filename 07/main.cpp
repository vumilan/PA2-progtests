#ifndef __PROGTEST__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <list>
#include <algorithm>

#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

#endif /* C++ 11 */

using namespace std;
#endif /* __PROGTEST__ */

class NoRouteException {
};
template<typename _T>
struct keyCompare {
    bool operator() (const _T & lhs, const _T & rhs) const {
        return (*lhs).m_Name < (*rhs).m_Name;
    }
};

template<typename _T, typename _E>
class CCity {
public:
    CCity() = default;
    CCity(const _T & stopName) : m_Name(stopName) {
    }
    _T m_Name;
    shared_ptr<CCity> parent = nullptr;
    map<shared_ptr<CCity<_T, _E>>,shared_ptr<vector<_E>>,keyCompare<shared_ptr<CCity<_T,_E>>>> m_Stops;
};

template<typename _T, typename _E>
class CRoute {
public:
    // default constructor
    // destructor
    // Add
    CRoute<_T, _E> &Add(const _T &from, const _T &to, const _E &train) {
        shared_ptr<CCity<_T,_E>> stopFrom = make_shared<CCity<_T,_E>>(from);
        shared_ptr<CCity<_T,_E>> stopTo = make_shared<CCity<_T,_E>>(to);
        auto itFrom = m_Nodes.insert(stopFrom);
        auto itTo = m_Nodes.insert(stopTo);
        vector<_E> trains = {train};
        shared_ptr<vector<_E>> trainsPtr = make_shared<vector<_E>>(trains);
        auto itChild = (*(*itFrom.first)).m_Stops.insert(make_pair(*itTo.first,trainsPtr));
        if (!itChild.second) {
            (*(*itChild.first).second).push_back(train);
        }
        itChild = (*(*itTo.first)).m_Stops.insert(make_pair(*itFrom.first,trainsPtr));
        if (!itChild.second) {
            (*(*itChild.first).second).push_back(train);
        }
        return *this;
    }

    list<_T> Find(const _T &from, const _T &to) const {
        if (from == to)
            return list<_T>{from};
        list<_T> findResult;
        queue<shared_ptr<CCity<_T,_E>>> queue;
        set<shared_ptr<CCity<_T,_E>>> visited;
        int searchDepth = 0;
        unsigned long timeToDepthIncrease;
        bool pendingDepthIncrease = false;
        auto it = m_Nodes.find(make_shared<CCity<_T,_E>>(from));
        if (it == m_Nodes.end()) {
            throw NoRouteException();
        }
        auto itTo = m_Nodes.find(make_shared<CCity<_T,_E>>(to));
        if (itTo == m_Nodes.end()) {
            throw NoRouteException();
        }
        findResult.push_back((*itTo)->m_Name);
        (*it)->parent = nullptr;
        queue.push(*it);
        visited.insert(*it);
        timeToDepthIncrease = queue.size();
        while (!queue.empty()) {
            auto itQ = queue.front();
            queue.pop();
            --timeToDepthIncrease;
            if (timeToDepthIncrease == 0) {
                ++searchDepth;
                pendingDepthIncrease = true;
            }
            for (auto && itC : itQ->m_Stops) {
                if (visited.find(itC.first) == visited.end()) {
                    queue.push(itC.first);
                    visited.insert(itC.first);
                    itC.first->parent = itQ;
                    if (itC.first == *itTo) {
                        auto itP = itC.first->parent;
                        while(itP != nullptr) {
                            findResult.push_back(itP->m_Name);
                            itP = itP->parent;
                        }
                        reverse(findResult.begin(), findResult.end());
                        return findResult;
                    }
                }
            }
            if (pendingDepthIncrease) {
                timeToDepthIncrease = queue.size();
                pendingDepthIncrease = false;
            }
        }
        throw NoRouteException();
    }

    list<_T> Find(const _T &from, const _T &to, function<bool(const _E & train)> func) const {
        if (from == to)
            return list<_T>{from};
        list<_T> findResult;
        queue<shared_ptr<CCity<_T,_E>>> queue;
        set<shared_ptr<CCity<_T,_E>>> visited;
        int searchDepth = 0;
        unsigned long timeToDepthIncrease;
        bool pendingDepthIncrease = false;
        auto it = m_Nodes.find(make_shared<CCity<_T,_E>>(from));
        if (it == m_Nodes.end()) {
            throw NoRouteException();
        }
        auto itTo = m_Nodes.find(make_shared<CCity<_T,_E>>(to));
        if (itTo == m_Nodes.end()) {
            throw NoRouteException();
        }
        findResult.push_back((*itTo)->m_Name);
        (*it)->parent = nullptr;
        queue.push(*it);
        visited.insert(*it);
        timeToDepthIncrease = queue.size();
        while (!queue.empty()) {
            auto itQ = queue.front();
            queue.pop();
            --timeToDepthIncrease;
            if (timeToDepthIncrease == 0) {
                ++searchDepth;
                pendingDepthIncrease = true;
            }
            for (auto && itC : itQ->m_Stops) {
                if (visited.find(itC.first) == visited.end()) {
                    for (auto && itT : *itC.second) {
                        if (func(itT)) {
                            queue.push(itC.first);
                            visited.insert(itC.first);
                            itC.first->parent = itQ;
                            if (itC.first == *itTo) {
                                auto itP = itC.first->parent;
                                while(itP != nullptr) {
                                    findResult.push_back(itP->m_Name);
                                    itP = itP->parent;
                                }
                                reverse(findResult.begin(), findResult.end());
                                return findResult;
                            }
                            break;
                        }
                    }
                }
            }
            if (pendingDepthIncrease) {
                timeToDepthIncrease = queue.size();
                pendingDepthIncrease = false;
            }
        }
        throw NoRouteException();
    }
private:
    set<shared_ptr<CCity<_T,_E>>,keyCompare<shared_ptr<CCity<_T,_E>>>> m_Nodes;
};

#ifndef __PROGTEST__

//=================================================================================================
class CTrain {
public:
    CTrain(const string &company,
           int speed)
            : m_Company(company),
              m_Speed(speed) {
    }
    //---------------------------------------------------------------------------------------------
    string m_Company;
    int m_Speed;
};

//=================================================================================================
class TrainFilterCompany {
public:
    TrainFilterCompany(const set<string> &companies)
            : m_Companies(companies) {
    }

    //---------------------------------------------------------------------------------------------
    bool operator()(const CTrain &train) const {
        return m_Companies.find(train.m_Company) != m_Companies.end();
    }
    //---------------------------------------------------------------------------------------------
private:
    set<string> m_Companies;
};

//=================================================================================================
class TrainFilterSpeed {
public:
    TrainFilterSpeed(int min,
                     int max)
            : m_Min(min),
              m_Max(max) {
    }

    //---------------------------------------------------------------------------------------------
    bool operator()(const CTrain &train) const {
        return train.m_Speed >= m_Min && train.m_Speed <= m_Max;
    }
    //---------------------------------------------------------------------------------------------
private:
    int m_Min;
    int m_Max;
};

//=================================================================================================
bool NurSchnellzug(const CTrain &zug) {
    return (zug.m_Company == "OBB" || zug.m_Company == "DB") && zug.m_Speed > 100;
}

//=================================================================================================
static string toText(const list<string> &l) {
    ostringstream oss;

    auto it = l.cbegin();
    oss << *it;
    for (++it; it != l.cend(); ++it)
        oss << " > " << *it;
    return oss.str();
}

//=================================================================================================
int main(void) {
    CRoute<string, CTrain> lines;

    lines.Add("Berlin", "Prague", CTrain("DB", 120))
            .Add("Berlin", "Prague", CTrain("CD", 80))
            .Add("Berlin", "Dresden", CTrain("DB", 160))
            .Add("Dresden", "Munchen", CTrain("DB", 160))
            .Add("Munchen", "Prague", CTrain("CD", 90))
            .Add("Munchen", "Linz", CTrain("DB", 200))
            .Add("Munchen", "Linz", CTrain("OBB", 90))
            .Add("Linz", "Prague", CTrain("CD", 50))
            .Add("Prague", "Wien", CTrain("CD", 100))
            .Add("Linz", "Wien", CTrain("OBB", 160))
            .Add("Paris", "Marseille", CTrain("SNCF", 300))
            .Add("Paris", "Dresden", CTrain("SNCF", 250));
    list<string> r1 = lines.Find("Berlin", "Linz");
    cout << toText(r1) << endl;
    assert (toText(r1) == "Berlin > Prague > Linz");

    list<string> r2 = lines.Find("Linz", "Berlin");
    assert (toText(r2) == "Linz > Prague > Berlin");

    list<string> r3 = lines.Find("Wien", "Berlin");
    assert (toText(r3) == "Wien > Prague > Berlin");

    list<string> r4 = lines.Find("Wien", "Berlin", NurSchnellzug);
    assert (toText(r4) == "Wien > Linz > Munchen > Dresden > Berlin");

    list<string> r5 = lines.Find("Wien", "Munchen", TrainFilterCompany(set<string>{"CD", "DB"}));
    assert (toText(r5) == "Wien > Prague > Munchen");

    list<string> r6 = lines.Find("Wien", "Munchen", TrainFilterSpeed(120, 200));
    assert (toText(r6) == "Wien > Linz > Munchen");

    list<string> r7 = lines.Find("Wien", "Munchen", [](const CTrain &x) { return x.m_Company == "CD"; });
    assert (toText(r7) == "Wien > Prague > Munchen");

    list<string> r8 = lines.Find("Munchen", "Munchen");
    assert (toText(r8) == "Munchen");

    list<string> r9 = lines.Find("Marseille", "Prague");
    assert (toText(r9) == "Marseille > Paris > Dresden > Berlin > Prague"
            || toText(r9) == "Marseille > Paris > Dresden > Munchen > Prague");

    try {
        list<string> r10 = lines.Find("Marseille", "Prague", NurSchnellzug);
        assert ("Marseille > Prague connection does not exist!!" == NULL);
    }
    catch (const NoRouteException &e) {
    }

    list<string> r11 = lines.Find("Salzburg", "Salzburg");
    assert (toText(r11) == "Salzburg");

    list<string> r12 = lines.Find("Salzburg", "Salzburg", [](const CTrain &x) { return x.m_Company == "SNCF"; });
    assert (toText(r12) == "Salzburg");

    try {
        list<string> r13 = lines.Find("London", "Oxford");
        assert ("London > Oxford connection does not exist!!" == NULL);
    }
    catch (const NoRouteException &e) {
    }
    return 0;
}

#endif  /* __PROGTEST__ */
