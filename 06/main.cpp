#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <list>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
using namespace std;
#endif /* __PROGTEST__ */

class CNetwork;
class CComputer;

// virtual parent class to wrap components
class CComponent {
public:
    // call in child class to get a pointer to such class, polymorphic
    virtual CComponent *clone() const = 0;
    virtual ~CComponent() = default;
    // virtual definition of FindNetwork since CNetwork can now be a component
    virtual CComputer *FindComputer(const string &name) {
        return nullptr;
    }
    // virtual definition of FindNetwork since CNetwork can now be a component
    virtual CNetwork *FindNetwork(const string &name) {
        return nullptr;
    }
    // polymorphic print
    virtual ostream &print(ostream & os, const string &indent) const = 0;
};

// function to split indent information for header and children, returns a pair of such information
pair<string, string> getIndents(const string &indent) {
    string headerIndent = indent;
    string childIndent;
    if (indent.length() > 1) {
        headerIndent.erase(indent.length() - 4, 2);
        childIndent = indent.substr(0, indent.length() - 2);
    }
    return make_pair(headerIndent, childIndent);
}

// component CPU
class CCPU : public CComponent {
public:
    CCPU(size_t cores, size_t frequency) : cores(cores), frequency(frequency) {}
    CComponent *clone() const override {
        return new CCPU(*this);
    }
    ostream &print(ostream & os, const string &indent) const override {
        pair<string, string> indents = getIndents(indent);
        return os << indents.first << "CPU, " << cores << " cores @ " << frequency << "MHz" << endl;
    }
private:
    size_t cores;
    size_t frequency;
};

// component RAM
class CMemory : public CComponent {
public:
    explicit CMemory(size_t capacity) : capacity(capacity) {}
    CComponent *clone() const override {
        return new CMemory(*this);
    }
    ostream &print(ostream & os, const string &indent) const override {
        pair<string, string> indents = getIndents(indent);
        return os << indents.first << "Memory, " << capacity << " MiB" << endl;
    }
private:
    size_t capacity;
};

// component Disk
class CDisk : public CComponent {
public:
    enum type {
        SSD, MAGNETIC
    };
    CDisk(type type, size_t capacity) : type(typeStrings.find(type)->second), capacity(capacity) {}
    CComponent *clone() const override {
        return new CDisk(*this);
    }
    ostream &print(ostream & os, const string &indent) const override {
        pair<string, string> indents = getIndents(indent);
        os << indents.first << type << ", " << capacity << " GiB" << endl;
        string indent2 = indents.second + "+-";
        size_t pos = 0;
        // print children
        for (auto &partition : partitions) {
            // different indent for last element
            if (partition == partitions.back())
                indent2 = indents.second + "\\-";
            os << indent2 << "[" << pos << "]: " << partition.first << " GiB, " << partition.second << endl;
            pos++;
        }
        return os;
    }
    CDisk &AddPartition(size_t size, const string &name) {
        partitions.emplace_back(size, name);
        return *this;
    }
private:
    map<size_t, string> typeStrings = { {0, "SSD"}, {1, "HDD"} };
    string type;
    size_t capacity;
    list<pair<size_t, string>> partitions;
};

// for copy constructor, functor to clone element
class cloneFunctor {
public:
    CComponent * operator() (CComponent * a) {
        return a->clone();
    }
};

// computer class
class CComputer {
public:
    explicit CComputer(const string &name) : name(name) {
        MAC = "";
    }
    // copy function used for copy constructor and operator =
    CComputer &copy(const CComputer &computer) {
        name = computer.name;
        MAC = computer.MAC;
        IPs.clear();
        for(auto &ip : computer.IPs)
            IPs.push_back(ip);
        // copy each element being pointed to by pointer in list
        transform(computer.components.begin(), computer.components.end(), back_inserter(components), cloneFunctor());
        return *this;
    }
    // needed deletion with for loop since pointers are stored
    void clearComponents() {
        for (auto & it : components)
            delete it;
        components.clear();
    }
    // copy constructor
    CComputer(const CComputer &computer) {
        copy(computer);
    }
    // copy operator
    CComputer &operator=(const CComputer &computer){
        if (this == &computer)
            return *this;
        clearComponents();
        copy(computer);
        return *this;
    }
    ~CComputer() {
        clearComponents();
    }
    const string &getName() const {
        return name;
    }
    CComputer &AddComponent(const CComponent &component) {
        components.push_back(component.clone());
        return *this;
    }
    CComputer &AddAddress(const string &address) {
        // add IP or MAC address depending on what types of commas it contains
        if (address.find('.') != string::npos)
            IPs.push_back(address);
        else if (address.find(':') != string::npos)
            MAC = address;
        return *this;
    }
    CComputer *FindComputer(const string &name) {
        if (this->name == name)
            return this;
        for (auto &component : components) {
            auto ptr = component->FindComputer(name);
            if (ptr != nullptr)
                return ptr;
        }
        return nullptr;
    }
    CNetwork *FindNetwork(const string &name) {
        for (auto &component : components) {
            auto ptr = component->FindNetwork(name);
            if (ptr != nullptr)
                return ptr;
        }
        return nullptr;
    }
    void setName(const string &name) {
        this->name = name;
    }
    // function inline since it needs a setter from CNetwork (which is bellow)
    CComputer Duplicate(const list<pair<string, string>> &vecNames) const;
    ostream &print(ostream &os, const string &indent) const {
        // indent from parent class stores information
        // about element position (different indents for last element, etc.)
        // so splitting information for header and child elements is needed
        pair<string, string> indents = getIndents(indent);
        os << indents.first << "Host: " << name << endl;
        string endIndent = "+-";
        string indent2 = indents.second + "| +-";
        // if IP is the last child of computer, use different indent
        if (MAC.empty() && components.empty()) {
            endIndent = "\\-";
        }
        for (auto &ip : IPs) {
            if (ip == IPs.back())
                os << indents.second << endIndent << ip << endl;
            else
                os << indents.second << "+-" << ip << endl;
        }
        // if MAC is the last child of computer, use different indent
        if (components.empty()) {
            endIndent = "\\-";
        }
        if (!MAC.empty())
            os << indents.second + endIndent << MAC << endl;
        // print children
        for (auto &component : components) {
            // different indent for last element
            if (component == components.back())
                indent2 = indents.second + "  \\-";
            component->print(os, indent2);
        }
        return os;
    }
    friend ostream & operator<<(ostream &os, const CComputer &computer) {
        return computer.print(os, "");
    }
private:
    string name;
    list<string> IPs;
    string MAC;
    // storing pointers of components so we can access them
    list<CComponent*> components;
};

// network class
class CNetwork : public CComponent {
public:
    explicit CNetwork(const string &name) : name(name) {}
    // copy function used for copy constructor and operator =
    CNetwork &copy(const CNetwork &network) {
        name = network.name;
        computers.clear();
        for (auto &computer : network.computers)
            computers.push_back(computer);
        return *this;
    }
    // copy constructor
    CNetwork(const CNetwork &network) {
        copy(network);
    }
    // copy operator
    CNetwork &operator=(const CNetwork &network){
        if (this == &network)
            return *this;
        copy(network);
        return *this;
    }
    CComponent *clone() const override {
        return new CNetwork(*this);
    }
    // return back to this for continuous calling
    CNetwork &AddComputer(const CComputer &computer) {
        computers.push_back(computer);
        return *this;
    }
    // find computer by name, searching linearly, return pointer to computer if found, return nullptr otherwise
    CComputer *FindComputer(const string &name) override {
        for (auto &computer : computers) {
            if (name == computer.getName())
                return &computer;
            auto ptr = computer.FindComputer(name);
            if (ptr != nullptr)
                return ptr;
        }
        return nullptr;
    }
    // find network function recursive calls between both CNetwork and CComputer
    CNetwork *FindNetwork(const string &name) override {
        if (this->name == name)
            return this;
        for (auto &computer : computers) {
            auto ptr = computer.FindNetwork(name);
            if (ptr != nullptr)
                return ptr;
        }
        return nullptr;
    }

    void setName(const string &name) {
        this->name = name;
    }
    ostream &print(ostream &os, const string &indent) const override {
        pair<string, string> indents = getIndents(indent);
        os << indents.first << "Network: " << name << endl;
        string indent2 = indents.second + "| +-";
        // print children
        for (auto &computer : computers ) {
            // different indent for last element
            if (&computer == &computers.back())
                indent2 = indents.second + "  \\-";
            computer.print(os, indent2);
        }
        return os;
    }
    friend ostream & operator<<(ostream &os, const CNetwork &network) {
        return network.print(os, "");
    }
private:
    string name;
    // using list as vector changes the addresses of objects when automatically reallocating
    list<CComputer> computers;
};

// function is inline since it needed a declared setter from CNetwork
CComputer CComputer::Duplicate(const list<pair<string, string>> &vecNames) const {
    CComputer duplicateComputer = *this;
    // remap names by argument
    for (auto &namePair : vecNames) {
        auto foundNetwork = duplicateComputer.FindNetwork(namePair.first);
        auto foundComputer = duplicateComputer.FindComputer(namePair.first);
        if (foundNetwork != nullptr)
            foundNetwork->setName(namePair.second);
        if (foundComputer != nullptr)
            foundComputer->setName(namePair.second);
    }
    return duplicateComputer;
}

#ifndef __PROGTEST__
template<typename _T>
string toString(const _T & x) {
    ostringstream oss;
    oss << x;
    return oss . str ();
}

int main() {
    CNetwork n ( "FIT network" );
    n . AddComputer (
            CComputer ( "progtest.fit.cvut.cz" ) .
                    AddAddress ( "147.32.232.142" ) .
                    AddComponent ( CCPU ( 8, 2400 ) ) .
                    AddComponent ( CCPU ( 8, 1200 ) ) .
                    AddComponent ( CDisk ( CDisk::MAGNETIC, 1500 ) .
                    AddPartition ( 50, "/" ) .
                    AddPartition ( 5, "/boot" ).
                    AddPartition ( 1000, "/var" ) ) .
                    AddComponent ( CDisk ( CDisk::SSD, 60 ) .
                    AddPartition ( 60, "/data" )  ) .
                    AddComponent ( CMemory ( 2000 ) ).
                    AddComponent ( CMemory ( 2000 ) ) ) .
            AddComputer (
            CComputer ( "edux.fit.cvut.cz" ) .
                    AddAddress ( "147.32.232.158" ) .
                    AddComponent ( CCPU ( 4, 1600 ) ) .
                    AddComponent ( CMemory ( 4000 ) ).
                    AddComponent ( CDisk ( CDisk::MAGNETIC, 2000 ) .
                    AddPartition ( 100, "/" )   .
                    AddPartition ( 1900, "/data" ) ) ) .
            AddComputer (
            CComputer ( "imap.fit.cvut.cz" ) .
                    AddAddress ( "147.32.232.238" ) .
                    AddComponent ( CCPU ( 4, 2500 ) ) .
                    AddAddress ( "2001:718:2:2901::238" ) .
                    AddComponent ( CMemory ( 8000 ) ) );
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: edux.fit.cvut.cz\n"
             "| +-147.32.232.158\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "\\-Host: imap.fit.cvut.cz\n"
             "  +-147.32.232.238\n"
             "  +-2001:718:2:2901::238\n"
             "  +-CPU, 4 cores @ 2500MHz\n"
             "  \\-Memory, 8000 MiB\n" );
    CNetwork x = n;
    auto c = x . FindComputer ( "imap.fit.cvut.cz" );
    assert ( toString ( *c ) ==
             "Host: imap.fit.cvut.cz\n"
             "+-147.32.232.238\n"
             "+-2001:718:2:2901::238\n"
             "+-CPU, 4 cores @ 2500MHz\n"
             "\\-Memory, 8000 MiB\n" );
    c -> AddComponent ( CDisk ( CDisk::MAGNETIC, 1000 ) .
            AddPartition ( 100, "system" ) .
            AddPartition ( 200, "WWW" ) .
            AddPartition ( 700, "mail" ) );
    assert ( toString ( x ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: edux.fit.cvut.cz\n"
             "| +-147.32.232.158\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "\\-Host: imap.fit.cvut.cz\n"
             "  +-147.32.232.238\n"
             "  +-2001:718:2:2901::238\n"
             "  +-CPU, 4 cores @ 2500MHz\n"
             "  +-Memory, 8000 MiB\n"
             "  \\-HDD, 1000 GiB\n"
             "    +-[0]: 100 GiB, system\n"
             "    +-[1]: 200 GiB, WWW\n"
             "    \\-[2]: 700 GiB, mail\n" );
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: edux.fit.cvut.cz\n"
             "| +-147.32.232.158\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "\\-Host: imap.fit.cvut.cz\n"
             "  +-147.32.232.238\n"
             "  +-2001:718:2:2901::238\n"
             "  +-CPU, 4 cores @ 2500MHz\n"
             "  \\-Memory, 8000 MiB\n" );
    n . AddComputer ( CComputer ( "vm01.fit.cvut.cz" ) .
            AddAddress ( "147.32.232.232" ) .
            AddComponent ( CCPU ( 32, 4000 ) ) .
            AddComponent ( CMemory ( 32768 ) ) .
            AddComponent ( CDisk ( CDisk::MAGNETIC, 3000 ) .
            AddPartition ( 500, "SYSTEM" ) .
            AddPartition ( 2000, "DATA" ) .
            AddPartition ( 500, "BACKUP" ) ) .
            AddComponent ( CNetwork ( "dummy00" ) .
            AddComputer ( CComputer ( "testing.fit.cvut.cz" ) .
            AddAddress ( "192.168.1.1" ) .
            AddComponent ( CCPU ( 1, 300 ) ) .
            AddComponent ( CMemory ( 256 ) ) ) ) .
            AddComponent ( CMemory ( 16384 ) ) .
            AddComponent ( CNetwork ( "vnet00" ) ) );
    cout << toString(n) << endl;
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: edux.fit.cvut.cz\n"
             "| +-147.32.232.158\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "+-Host: imap.fit.cvut.cz\n"
             "| +-147.32.232.238\n"
             "| +-2001:718:2:2901::238\n"
             "| +-CPU, 4 cores @ 2500MHz\n"
             "| \\-Memory, 8000 MiB\n"
             "\\-Host: vm01.fit.cvut.cz\n"
             "  +-147.32.232.232\n"
             "  +-CPU, 32 cores @ 4000MHz\n"
             "  +-Memory, 32768 MiB\n"
             "  +-HDD, 3000 GiB\n"
             "  | +-[0]: 500 GiB, SYSTEM\n"
             "  | +-[1]: 2000 GiB, DATA\n"
             "  | \\-[2]: 500 GiB, BACKUP\n"
             "  +-Network: dummy00\n"
             "  | \\-Host: testing.fit.cvut.cz\n"
             "  |   +-192.168.1.1\n"
             "  |   +-CPU, 1 cores @ 300MHz\n"
             "  |   \\-Memory, 256 MiB\n"
             "  +-Memory, 16384 MiB\n"
             "  \\-Network: vnet00\n" );
    auto vmnet = n . FindNetwork ( "vnet00" );
    vmnet -> AddComputer ( CComputer ( "monitor1.fit.cvut.cz" ) .
            AddAddress ( "147.32.232.254" ) .
            AddComponent ( CCPU ( 2, 1500 ) ) .
            AddComponent ( CMemory ( 4096 ) ) .
            AddComponent ( CDisk ( CDisk::MAGNETIC, 750 ) .
            AddPartition ( 100, "root" ) .
            AddPartition ( 600, "log" ) ) );
    assert ( toString ( *vmnet ) ==
             "Network: vnet00\n"
             "\\-Host: monitor1.fit.cvut.cz\n"
             "  +-147.32.232.254\n"
             "  +-CPU, 2 cores @ 1500MHz\n"
             "  +-Memory, 4096 MiB\n"
             "  \\-HDD, 750 GiB\n"
             "    +-[0]: 100 GiB, root\n"
             "    \\-[1]: 600 GiB, log\n" );
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: edux.fit.cvut.cz\n"
             "| +-147.32.232.158\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "+-Host: imap.fit.cvut.cz\n"
             "| +-147.32.232.238\n"
             "| +-2001:718:2:2901::238\n"
             "| +-CPU, 4 cores @ 2500MHz\n"
             "| \\-Memory, 8000 MiB\n"
             "\\-Host: vm01.fit.cvut.cz\n"
             "  +-147.32.232.232\n"
             "  +-CPU, 32 cores @ 4000MHz\n"
             "  +-Memory, 32768 MiB\n"
             "  +-HDD, 3000 GiB\n"
             "  | +-[0]: 500 GiB, SYSTEM\n"
             "  | +-[1]: 2000 GiB, DATA\n"
             "  | \\-[2]: 500 GiB, BACKUP\n"
             "  +-Network: dummy00\n"
             "  | \\-Host: testing.fit.cvut.cz\n"
             "  |   +-192.168.1.1\n"
             "  |   +-CPU, 1 cores @ 300MHz\n"
             "  |   \\-Memory, 256 MiB\n"
             "  +-Memory, 16384 MiB\n"
             "  \\-Network: vnet00\n"
             "    \\-Host: monitor1.fit.cvut.cz\n"
             "      +-147.32.232.254\n"
             "      +-CPU, 2 cores @ 1500MHz\n"
             "      +-Memory, 4096 MiB\n"
             "      \\-HDD, 750 GiB\n"
             "        +-[0]: 100 GiB, root\n"
             "        \\-[1]: 600 GiB, log\n" );
    vmnet -> AddComputer ( n . FindComputer ( "monitor1.fit.cvut.cz" )
                                   -> Duplicate ( { pair<string,string>("monitor1.fit.cvut.cz","monitor2.fit.cvut.cz") } ) );
    cout << toString( *vmnet ) << endl;
    assert ( toString ( *vmnet ) ==
             "Network: vnet00\n"
             "+-Host: monitor1.fit.cvut.cz\n"
             "| +-147.32.232.254\n"
             "| +-CPU, 2 cores @ 1500MHz\n"
             "| +-Memory, 4096 MiB\n"
             "| \\-HDD, 750 GiB\n"
             "|   +-[0]: 100 GiB, root\n"
             "|   \\-[1]: 600 GiB, log\n"
             "\\-Host: monitor2.fit.cvut.cz\n"
             "  +-147.32.232.254\n"
             "  +-CPU, 2 cores @ 1500MHz\n"
             "  +-Memory, 4096 MiB\n"
             "  \\-HDD, 750 GiB\n"
             "    +-[0]: 100 GiB, root\n"
             "    \\-[1]: 600 GiB, log\n" );
    n . AddComputer ( n . FindComputer ( "vm01.fit.cvut.cz" )
                              -> Duplicate (
                                      {
                                              pair<string,string>("monitor1.fit.cvut.cz","monitor3.fit.cvut.cz"),
                                              pair<string,string>("monitor2.fit.cvut.cz","monitor4.fit.cvut.cz"),
                                              pair<string,string>("vm01.fit.cvut.cz","vm02.fit.cvut.cz")
                                      } ) );
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: edux.fit.cvut.cz\n"
             "| +-147.32.232.158\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "+-Host: imap.fit.cvut.cz\n"
             "| +-147.32.232.238\n"
             "| +-2001:718:2:2901::238\n"
             "| +-CPU, 4 cores @ 2500MHz\n"
             "| \\-Memory, 8000 MiB\n"
             "+-Host: vm01.fit.cvut.cz\n"
             "| +-147.32.232.232\n"
             "| +-CPU, 32 cores @ 4000MHz\n"
             "| +-Memory, 32768 MiB\n"
             "| +-HDD, 3000 GiB\n"
             "| | +-[0]: 500 GiB, SYSTEM\n"
             "| | +-[1]: 2000 GiB, DATA\n"
             "| | \\-[2]: 500 GiB, BACKUP\n"
             "| +-Network: dummy00\n"
             "| | \\-Host: testing.fit.cvut.cz\n"
             "| |   +-192.168.1.1\n"
             "| |   +-CPU, 1 cores @ 300MHz\n"
             "| |   \\-Memory, 256 MiB\n"
             "| +-Memory, 16384 MiB\n"
             "| \\-Network: vnet00\n"
             "|   +-Host: monitor1.fit.cvut.cz\n"
             "|   | +-147.32.232.254\n"
             "|   | +-CPU, 2 cores @ 1500MHz\n"
             "|   | +-Memory, 4096 MiB\n"
             "|   | \\-HDD, 750 GiB\n"
             "|   |   +-[0]: 100 GiB, root\n"
             "|   |   \\-[1]: 600 GiB, log\n"
             "|   \\-Host: monitor2.fit.cvut.cz\n"
             "|     +-147.32.232.254\n"
             "|     +-CPU, 2 cores @ 1500MHz\n"
             "|     +-Memory, 4096 MiB\n"
             "|     \\-HDD, 750 GiB\n"
             "|       +-[0]: 100 GiB, root\n"
             "|       \\-[1]: 600 GiB, log\n"
             "\\-Host: vm02.fit.cvut.cz\n"
             "  +-147.32.232.232\n"
             "  +-CPU, 32 cores @ 4000MHz\n"
             "  +-Memory, 32768 MiB\n"
             "  +-HDD, 3000 GiB\n"
             "  | +-[0]: 500 GiB, SYSTEM\n"
             "  | +-[1]: 2000 GiB, DATA\n"
             "  | \\-[2]: 500 GiB, BACKUP\n"
             "  +-Network: dummy00\n"
             "  | \\-Host: testing.fit.cvut.cz\n"
             "  |   +-192.168.1.1\n"
             "  |   +-CPU, 1 cores @ 300MHz\n"
             "  |   \\-Memory, 256 MiB\n"
             "  +-Memory, 16384 MiB\n"
             "  \\-Network: vnet00\n"
             "    +-Host: monitor3.fit.cvut.cz\n"
             "    | +-147.32.232.254\n"
             "    | +-CPU, 2 cores @ 1500MHz\n"
             "    | +-Memory, 4096 MiB\n"
             "    | \\-HDD, 750 GiB\n"
             "    |   +-[0]: 100 GiB, root\n"
             "    |   \\-[1]: 600 GiB, log\n"
             "    \\-Host: monitor4.fit.cvut.cz\n"
             "      +-147.32.232.254\n"
             "      +-CPU, 2 cores @ 1500MHz\n"
             "      +-Memory, 4096 MiB\n"
             "      \\-HDD, 750 GiB\n"
             "        +-[0]: 100 GiB, root\n"
             "        \\-[1]: 600 GiB, log\n" );
    vmnet -> AddComputer ( n . FindComputer ( "vm02.fit.cvut.cz" )
                                   -> Duplicate (
                                           {
                                                   pair<string,string>("monitor3.fit.cvut.cz","monitor5.fit.cvut.cz"),
                                                   pair<string,string>("monitor4.fit.cvut.cz","monitor6.fit.cvut.cz"),
                                                   pair<string,string>("vm02.fit.cvut.cz","vm03.fit.cvut.cz"),
                                                   pair<string,string>("vnet00","vnet01")
                                           } ) );

    assert ( toString ( *vmnet ) ==
             "Network: vnet00\n"
             "+-Host: monitor1.fit.cvut.cz\n"
             "| +-147.32.232.254\n"
             "| +-CPU, 2 cores @ 1500MHz\n"
             "| +-Memory, 4096 MiB\n"
             "| \\-HDD, 750 GiB\n"
             "|   +-[0]: 100 GiB, root\n"
             "|   \\-[1]: 600 GiB, log\n"
             "+-Host: monitor2.fit.cvut.cz\n"
             "| +-147.32.232.254\n"
             "| +-CPU, 2 cores @ 1500MHz\n"
             "| +-Memory, 4096 MiB\n"
             "| \\-HDD, 750 GiB\n"
             "|   +-[0]: 100 GiB, root\n"
             "|   \\-[1]: 600 GiB, log\n"
             "\\-Host: vm03.fit.cvut.cz\n"
             "  +-147.32.232.232\n"
             "  +-CPU, 32 cores @ 4000MHz\n"
             "  +-Memory, 32768 MiB\n"
             "  +-HDD, 3000 GiB\n"
             "  | +-[0]: 500 GiB, SYSTEM\n"
             "  | +-[1]: 2000 GiB, DATA\n"
             "  | \\-[2]: 500 GiB, BACKUP\n"
             "  +-Network: dummy00\n"
             "  | \\-Host: testing.fit.cvut.cz\n"
             "  |   +-192.168.1.1\n"
             "  |   +-CPU, 1 cores @ 300MHz\n"
             "  |   \\-Memory, 256 MiB\n"
             "  +-Memory, 16384 MiB\n"
             "  \\-Network: vnet01\n"
             "    +-Host: monitor5.fit.cvut.cz\n"
             "    | +-147.32.232.254\n"
             "    | +-CPU, 2 cores @ 1500MHz\n"
             "    | +-Memory, 4096 MiB\n"
             "    | \\-HDD, 750 GiB\n"
             "    |   +-[0]: 100 GiB, root\n"
             "    |   \\-[1]: 600 GiB, log\n"
             "    \\-Host: monitor6.fit.cvut.cz\n"
             "      +-147.32.232.254\n"
             "      +-CPU, 2 cores @ 1500MHz\n"
             "      +-Memory, 4096 MiB\n"
             "      \\-HDD, 750 GiB\n"
             "        +-[0]: 100 GiB, root\n"
             "        \\-[1]: 600 GiB, log\n" );
    n . FindComputer ( "vm02.fit.cvut.cz" )
            -> FindNetwork ( "vnet00" )
            -> AddComputer ( n . FindComputer ( "vm01.fit.cvut.cz" )
                                     -> Duplicate (
                                             {
                                                     pair<string,string>("monitor1.fit.cvut.cz","monitor11.fit.cvut.cz"),
                                                     pair<string,string>("monitor2.fit.cvut.cz","monitor12.fit.cvut.cz"),
                                                     pair<string,string>("vm01.fit.cvut.cz","vm11.fit.cvut.cz"),
                                                     pair<string,string>("vm03.fit.cvut.cz","vm13.fit.cvut.cz"),
                                                     pair<string,string>("vnet00","vnet10"),
                                                     pair<string,string>("vnet01","vnet11")
                                             } ) );
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: edux.fit.cvut.cz\n"
             "| +-147.32.232.158\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "+-Host: imap.fit.cvut.cz\n"
             "| +-147.32.232.238\n"
             "| +-2001:718:2:2901::238\n"
             "| +-CPU, 4 cores @ 2500MHz\n"
             "| \\-Memory, 8000 MiB\n"
             "+-Host: vm01.fit.cvut.cz\n"
             "| +-147.32.232.232\n"
             "| +-CPU, 32 cores @ 4000MHz\n"
             "| +-Memory, 32768 MiB\n"
             "| +-HDD, 3000 GiB\n"
             "| | +-[0]: 500 GiB, SYSTEM\n"
             "| | +-[1]: 2000 GiB, DATA\n"
             "| | \\-[2]: 500 GiB, BACKUP\n"
             "| +-Network: dummy00\n"
             "| | \\-Host: testing.fit.cvut.cz\n"
             "| |   +-192.168.1.1\n"
             "| |   +-CPU, 1 cores @ 300MHz\n"
             "| |   \\-Memory, 256 MiB\n"
             "| +-Memory, 16384 MiB\n"
             "| \\-Network: vnet00\n"
             "|   +-Host: monitor1.fit.cvut.cz\n"
             "|   | +-147.32.232.254\n"
             "|   | +-CPU, 2 cores @ 1500MHz\n"
             "|   | +-Memory, 4096 MiB\n"
             "|   | \\-HDD, 750 GiB\n"
             "|   |   +-[0]: 100 GiB, root\n"
             "|   |   \\-[1]: 600 GiB, log\n"
             "|   +-Host: monitor2.fit.cvut.cz\n"
             "|   | +-147.32.232.254\n"
             "|   | +-CPU, 2 cores @ 1500MHz\n"
             "|   | +-Memory, 4096 MiB\n"
             "|   | \\-HDD, 750 GiB\n"
             "|   |   +-[0]: 100 GiB, root\n"
             "|   |   \\-[1]: 600 GiB, log\n"
             "|   \\-Host: vm03.fit.cvut.cz\n"
             "|     +-147.32.232.232\n"
             "|     +-CPU, 32 cores @ 4000MHz\n"
             "|     +-Memory, 32768 MiB\n"
             "|     +-HDD, 3000 GiB\n"
             "|     | +-[0]: 500 GiB, SYSTEM\n"
             "|     | +-[1]: 2000 GiB, DATA\n"
             "|     | \\-[2]: 500 GiB, BACKUP\n"
             "|     +-Network: dummy00\n"
             "|     | \\-Host: testing.fit.cvut.cz\n"
             "|     |   +-192.168.1.1\n"
             "|     |   +-CPU, 1 cores @ 300MHz\n"
             "|     |   \\-Memory, 256 MiB\n"
             "|     +-Memory, 16384 MiB\n"
             "|     \\-Network: vnet01\n"
             "|       +-Host: monitor5.fit.cvut.cz\n"
             "|       | +-147.32.232.254\n"
             "|       | +-CPU, 2 cores @ 1500MHz\n"
             "|       | +-Memory, 4096 MiB\n"
             "|       | \\-HDD, 750 GiB\n"
             "|       |   +-[0]: 100 GiB, root\n"
             "|       |   \\-[1]: 600 GiB, log\n"
             "|       \\-Host: monitor6.fit.cvut.cz\n"
             "|         +-147.32.232.254\n"
             "|         +-CPU, 2 cores @ 1500MHz\n"
             "|         +-Memory, 4096 MiB\n"
             "|         \\-HDD, 750 GiB\n"
             "|           +-[0]: 100 GiB, root\n"
             "|           \\-[1]: 600 GiB, log\n"
             "\\-Host: vm02.fit.cvut.cz\n"
             "  +-147.32.232.232\n"
             "  +-CPU, 32 cores @ 4000MHz\n"
             "  +-Memory, 32768 MiB\n"
             "  +-HDD, 3000 GiB\n"
             "  | +-[0]: 500 GiB, SYSTEM\n"
             "  | +-[1]: 2000 GiB, DATA\n"
             "  | \\-[2]: 500 GiB, BACKUP\n"
             "  +-Network: dummy00\n"
             "  | \\-Host: testing.fit.cvut.cz\n"
             "  |   +-192.168.1.1\n"
             "  |   +-CPU, 1 cores @ 300MHz\n"
             "  |   \\-Memory, 256 MiB\n"
             "  +-Memory, 16384 MiB\n"
             "  \\-Network: vnet00\n"
             "    +-Host: monitor3.fit.cvut.cz\n"
             "    | +-147.32.232.254\n"
             "    | +-CPU, 2 cores @ 1500MHz\n"
             "    | +-Memory, 4096 MiB\n"
             "    | \\-HDD, 750 GiB\n"
             "    |   +-[0]: 100 GiB, root\n"
             "    |   \\-[1]: 600 GiB, log\n"
             "    +-Host: monitor4.fit.cvut.cz\n"
             "    | +-147.32.232.254\n"
             "    | +-CPU, 2 cores @ 1500MHz\n"
             "    | +-Memory, 4096 MiB\n"
             "    | \\-HDD, 750 GiB\n"
             "    |   +-[0]: 100 GiB, root\n"
             "    |   \\-[1]: 600 GiB, log\n"
             "    \\-Host: vm11.fit.cvut.cz\n"
             "      +-147.32.232.232\n"
             "      +-CPU, 32 cores @ 4000MHz\n"
             "      +-Memory, 32768 MiB\n"
             "      +-HDD, 3000 GiB\n"
             "      | +-[0]: 500 GiB, SYSTEM\n"
             "      | +-[1]: 2000 GiB, DATA\n"
             "      | \\-[2]: 500 GiB, BACKUP\n"
             "      +-Network: dummy00\n"
             "      | \\-Host: testing.fit.cvut.cz\n"
             "      |   +-192.168.1.1\n"
             "      |   +-CPU, 1 cores @ 300MHz\n"
             "      |   \\-Memory, 256 MiB\n"
             "      +-Memory, 16384 MiB\n"
             "      \\-Network: vnet10\n"
             "        +-Host: monitor11.fit.cvut.cz\n"
             "        | +-147.32.232.254\n"
             "        | +-CPU, 2 cores @ 1500MHz\n"
             "        | +-Memory, 4096 MiB\n"
             "        | \\-HDD, 750 GiB\n"
             "        |   +-[0]: 100 GiB, root\n"
             "        |   \\-[1]: 600 GiB, log\n"
             "        +-Host: monitor12.fit.cvut.cz\n"
             "        | +-147.32.232.254\n"
             "        | +-CPU, 2 cores @ 1500MHz\n"
             "        | +-Memory, 4096 MiB\n"
             "        | \\-HDD, 750 GiB\n"
             "        |   +-[0]: 100 GiB, root\n"
             "        |   \\-[1]: 600 GiB, log\n"
             "        \\-Host: vm13.fit.cvut.cz\n"
             "          +-147.32.232.232\n"
             "          +-CPU, 32 cores @ 4000MHz\n"
             "          +-Memory, 32768 MiB\n"
             "          +-HDD, 3000 GiB\n"
             "          | +-[0]: 500 GiB, SYSTEM\n"
             "          | +-[1]: 2000 GiB, DATA\n"
             "          | \\-[2]: 500 GiB, BACKUP\n"
             "          +-Network: dummy00\n"
             "          | \\-Host: testing.fit.cvut.cz\n"
             "          |   +-192.168.1.1\n"
             "          |   +-CPU, 1 cores @ 300MHz\n"
             "          |   \\-Memory, 256 MiB\n"
             "          +-Memory, 16384 MiB\n"
             "          \\-Network: vnet11\n"
             "            +-Host: monitor5.fit.cvut.cz\n"
             "            | +-147.32.232.254\n"
             "            | +-CPU, 2 cores @ 1500MHz\n"
             "            | +-Memory, 4096 MiB\n"
             "            | \\-HDD, 750 GiB\n"
             "            |   +-[0]: 100 GiB, root\n"
             "            |   \\-[1]: 600 GiB, log\n"
             "            \\-Host: monitor6.fit.cvut.cz\n"
             "              +-147.32.232.254\n"
             "              +-CPU, 2 cores @ 1500MHz\n"
             "              +-Memory, 4096 MiB\n"
             "              \\-HDD, 750 GiB\n"
             "                +-[0]: 100 GiB, root\n"
             "                \\-[1]: 600 GiB, log\n" );
    return 0;
}
#endif /* __PROGTEST__ */
