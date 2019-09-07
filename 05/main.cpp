#ifndef __PROGTEST__

#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>

using namespace std;

class CTimeStamp {
public:
    CTimeStamp(int year, int month, int day, int hour, int minute, double sec) : year(year), month(month), day(day),
                                                                                 hour(hour), minute(minute), sec(sec) {}

    int Compare(const CTimeStamp &x) const {
        if (this->year == x.year) {
            if (this->month == x.month) {
                if (this->day == x.day) {
                    if (this->hour == x.hour) {
                        if (this->minute == x.minute) {
                            if (this->sec == x.sec) {
                                return 0;
                            }
                            return this->sec < x.sec ? -1 : 1;
                        }
                        return this->minute < x.minute ? -1 : 1;
                    }
                    return this->hour < x.hour ? -1 : 1;
                }
                return this->day < x.day ? -1 : 1;
            }
            return this->month < x.month ? -1 : 1;
        }
        return this->year < x.year ? -1 : 1;
    }

    friend ostream &operator<<(ostream &os, const CTimeStamp &x) {
        return os << x.year << "-" << setfill('0') << setw(2) << x.month << "-" << setfill('0') << setw(2) << x.day
        << " " << setfill('0') << setw(2) << x.hour << ":" << setfill('0') << setw(2) << x.minute << ":"
        << fixed << setprecision(3) << x.sec;
    }

private:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    double sec;
};

class CMail {
public:
    CMail(const CTimeStamp &timeStamp, const string &from, const string &to, const string &subject) : timeStamp(
            timeStamp), from(from), to(to), subject(subject) {}

    int CompareByTime(const CTimeStamp &x) const {
        return this->timeStamp.Compare(x);
    }

    int CompareByTime(const CMail &x) const {
        return this->timeStamp.Compare(x.timeStamp);
    }

    const string &From(void) const {
        return from;
    }

    const string &To(void) const {
        return to;
    }

    const string &Subject(void) const {
        return subject;
    }

    const CTimeStamp &TimeStamp(void) const {
        return timeStamp;
    }

    friend ostream &operator<<(ostream &os, const CMail &x) {
        return os << x.timeStamp << " " << x.from << " -> " << x.to << ", subject: " << x.subject;
    }

private:
    CTimeStamp timeStamp;
    string from;
    string to;
    string subject;
};
// your code will be compiled in a separate namespace
namespace MysteriousNamespace {
#endif /* __PROGTEST__ */

//----------------------------------------------------------------------------------------
    class CMailLog {
    public:
        // split line into string tokens function
        vector<string> getStringTokens(const string &line) const {
            vector<string> tokens;
            unsigned long j = 0;
            int count = 0;
            for (unsigned i = 0; i < line.length(); i++) {
                if (line[i] == ' ') {
                    tokens.push_back(line.substr(j, i - j));
                    j = i + 1;
                    count++;
                    if (count == 6) {
                        tokens.push_back(line.substr(j));
                        break;
                    }
                }
            }
            return tokens;
        }

        int ParseLog(istream &in) {
            string line;
            int year, day, month, hour, minute, loggedMails = 0;
            double sec;
            while (getline(in, line)) {
                // split line into string tokens
                vector<string> tokens = getStringTokens(line);
                // check log line validity
                if (tokens.size() < 7 || tokens[0].size() != 3 || tokens[1].size() > 2
                    || tokens[3].size() < 5 || tokens[6].size() < 3)
                    continue;
                if (count(tokens[3].begin(), tokens[3].end(), ':') != 2)
                    continue;
                // check message validity
                if (tokens[6].find('=') == string::npos)
                    continue;
                string tag = tokens[6].substr(0, tokens[6].find('='));
                if (tag != "from" && tag != "to" && tag != "subject")
                    continue;
                // assign values if valid
                month = getMonthIndex(tokens[0]);
                day = stoi(tokens[1]);
                year = stoi(tokens[2]);
                unsigned long hourEndPos = tokens[3].find(':');
                hour = stoi(tokens[3].substr(0, hourEndPos));
                unsigned long minuteEndPos = tokens[3].find(':', tokens[3].find(':') + 1);
                minute = stoi(tokens[3].substr(hourEndPos + 1, minuteEndPos - hourEndPos - 1));
                sec = stod(tokens[3].substr(minuteEndPos + 1));
                // check values validity
                if (month == -1 || day < 1 || day > 31 || year < 0 || hour < 0
                    || hour > 24 || minute < 0 || minute > 60 || sec < 0 || sec > 60)
                    continue;
                string id = tokens[5];
                // process tags
                if (tag == "from") {
                    auto it = froms.emplace(id, make_pair(tokens[6].substr(5), ""));
                    if (!it.second) {
                        it.first->second.second = "";
                        it.first->second.first = tokens[6].substr(5);
                    }
                }
                else if (tag == "subject") {
                    auto it = froms.find(id);
                    if (it != froms.end()) {
                        it->second.second = tokens[6].substr(8);
                    }
                }
                else if (tag == "to") {
                    CTimeStamp timeStamp = CTimeStamp(year, month, day, hour, minute, sec);
                    auto it = froms.find(id);
                    loggedMails++;
                    if (it != froms.end()) {
                        mails.emplace(timeStamp, CMail(
                                timeStamp, it->second.first, tokens[6].substr(3), it->second.second
                        ));
                    }
                }
            }
            return loggedMails;
        }

        list<CMail> ListMail(const CTimeStamp &from, const CTimeStamp &to) const {
            list<CMail> output;
            auto upper = mails.upper_bound(to);
            for (auto it = mails.lower_bound(from); it != upper; it++) {
                output.push_back(it->second);
            }
            return output;
        }

        set<string> ActiveUsers(const CTimeStamp &from, const CTimeStamp &to) const {
            set<string> output;
            auto upper = mails.upper_bound(to);
            for (auto it = mails.lower_bound(from); it != upper; it++) {
                output.insert(it->second.From());
                output.insert(it->second.To());
            }
            return output;
        }

    private:
        struct timeStampCompare {
            bool operator()(const CTimeStamp &lhs, const CTimeStamp &rhs) const {
                return lhs.Compare(rhs) == -1;
            }
        };
        int getMonthIndex(const string &name) const {
            map<string, int> months {
                    {"Jan", 1},
                    {"Feb", 2},
                    {"Mar", 3},
                    {"Apr", 4},
                    {"May", 5},
                    {"Jun", 6},
                    {"Jul", 7},
                    {"Aug", 8},
                    {"Sep", 9},
                    {"Oct", 10},
                    {"Nov", 11},
                    {"Dec", 12}
            };
            const auto iter = months.find(name);
            if (iter != months.cend())
                return iter->second;
            return -1;
        }
        // map of sent mails
        map<string, pair<string, string>> froms;
        // multimap of sent and received mails
        multimap<CTimeStamp, CMail, timeStampCompare> mails;
    };
//----------------------------------------------------------------------------------------
#ifndef __PROGTEST__
} // namespace
string printMail(const list<CMail> &all) {
    ostringstream oss;
    for (const auto &mail : all)
        oss << mail << endl;
    return oss.str();
}

string printUsers(const set<string> &all) {
    ostringstream oss;
    bool first = true;
    for (const auto &name : all) {
        if (!first)
            oss << ", ";
        else
            first = false;
        oss << name;
    }
    return oss.str();
}

int main(void) {
    MysteriousNamespace::CMailLog m;
    list<CMail> mailList;
    set<string> users;
    istringstream iss;

    iss.clear();
    iss.str(
            "Mar 29 2019 12:35:32.233 relay.fit.cvut.cz ADFger72343D: from=user1@fit.cvut.cz\n"
            "Mar 29 2019 12:37:16.234 relay.fit.cvut.cz JlMSRW4232Df: from=person3@fit.cvut.cz\n"
            "Mar 29 2019 12:55:13.23 relay.fit.cvut.cz JlMSRW4232Df: subject=New progtest homework!\n"
            "Mar 29 2019 13:38:45.043 relay.fit.cvut.cz Kbced342sdgA: from=office13@fit.cvut.cz\n"
            "Mar 29 2019 13:36:13.023 relay.fit.cvut.cz JlMSRW4232Df: to=user76@fit.cvut.cz\n"
            "Mar 29 2019 13:55:31.456 relay.fit.cvut.cz KhdfEjkl247D: from=PR-department@fit.cvut.cz\n"
            "Mar 29 2019 14:18:12.654 relay.fit.cvut.cz Kbced342sdgA: to=boss13@fit.cvut.cz\n"
            "Mar 29 2019 14:48:32.563 relay.fit.cvut.cz KhdfEjkl247D: subject=Business partner\n"
            "Mar 29 2019 14:58:32 relay.fit.cvut.cz KhdfEjkl247D: to=HR-department@fit.cvut.cz\n"
            "Mar 29 2019 14:25:23.233 relay.fit.cvut.cz ADFger72343D: mail undeliverable\n"
            "Mar 29 2019 15:2:34.231 relay.fit.cvut.cz KhdfEjkl247D: to=CIO@fit.cvut.cz\n"
            "Mar 29 2019 15:2:34.23 relay.fit.cvut.cz KhdfEjkl247D: to=CEO@fit.cvut.cz\n"
            "Mar 29 2019 15:2:34.23 relay.fit.cvut.cz KhdfEjkl247D: to=dean@fit.cvut.cz\n"
            "Mar 29 2019 15:2:34.23 relay.fit.cvut.cz KhdfEjkl247D: to=vice-dean@fit.cvut.cz\n"
            "Mar 29 2019 15:2:34.23 relay.fit.cvut.cz KhdfEjkl247D: to=archive@fit.cvut.cz\n");
    assert (m.ParseLog(iss) == 8);
    mailList = m.ListMail(CTimeStamp(2019, 3, 28, 0, 0, 0),
                          CTimeStamp(2019, 3, 29, 23, 59, 59));
    cout << printMail(mailList) << endl;
    assert (printMail(mailList) ==
            "2019-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!\n"
            "2019-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz, subject: \n"
            "2019-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner\n"
            "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> CEO@fit.cvut.cz, subject: Business partner\n"
            "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> dean@fit.cvut.cz, subject: Business partner\n"
            "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> vice-dean@fit.cvut.cz, subject: Business partner\n"
            "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> archive@fit.cvut.cz, subject: Business partner\n"
            "2019-03-29 15:02:34.231 PR-department@fit.cvut.cz -> CIO@fit.cvut.cz, subject: Business partner\n");
    mailList = m.ListMail(CTimeStamp(2019, 3, 28, 0, 0, 0),
                          CTimeStamp(2019, 3, 29, 14, 58, 32));
    assert (printMail(mailList) ==
            "2019-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!\n"
            "2019-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz, subject: \n"
            "2019-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner\n");
    mailList = m.ListMail(CTimeStamp(2019, 3, 30, 0, 0, 0),
                          CTimeStamp(2019, 3, 30, 23, 59, 59));
    assert (printMail(mailList) == "");
    users = m.ActiveUsers(CTimeStamp(2019, 3, 28, 0, 0, 0),
                          CTimeStamp(2019, 3, 29, 23, 59, 59));
    assert (printUsers(users) ==
            "CEO@fit.cvut.cz, CIO@fit.cvut.cz, HR-department@fit.cvut.cz, PR-department@fit.cvut.cz, archive@fit.cvut.cz, boss13@fit.cvut.cz, dean@fit.cvut.cz, office13@fit.cvut.cz, person3@fit.cvut.cz, user76@fit.cvut.cz, vice-dean@fit.cvut.cz");
    users = m.ActiveUsers(CTimeStamp(2019, 3, 28, 0, 0, 0),
                          CTimeStamp(2019, 3, 29, 13, 59, 59));
    assert (printUsers(users) == "person3@fit.cvut.cz, user76@fit.cvut.cz");
    return 0;
}

#endif /* __PROGTEST__ */
