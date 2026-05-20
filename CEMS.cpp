/*
 * ============================================================================
 *  CAMPUS EVENT MANAGEMENT SYSTEM
 * ============================================================================
 *  Concepts Demonstrated:
 *    - Basics (I/O, variables, constants)
 *    - Flow Control (if-else, switch, loops)
 *    - Pointers & Dynamic Memory
 *    - Modular Programming (functions, separation of concerns)
 *    - User-Defined Datatypes (structs, classes, enums)
 *    - File Handling (read/write with delimiters)
 *    - STL (vector, map, string, algorithm, sort, find_if, etc.)
 *  Author: Campus Project
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <limits>
#include <cmath>

using namespace std;

// ======================== CONSTANTS ========================
const string USERS_FILE    = "data_users.dat";
const string EVENTS_FILE   = "data_events.dat";
const string VENUES_FILE   = "data_venues.dat";
const string DELIMITER     = "|";
const string LINE_SEP      = "========================================================================";

// ======================== ENUMS (User-Defined Datatype) ========================

enum Role { ROLE_ADMIN = 1, ROLE_STUDENT, ROLE_FACULTY };
enum EventType { SEMINAR = 1, WORKSHOP, CONFERENCE, CULTURAL, SPORTS, TECH_TALK, OTHER };
enum EventStatus { UPCOMING = 1, ONGOING, COMPLETED, CANCELLED };
enum RSVPStatus { RSVP_PENDING = 1, RSVP_CONFIRMED, RSVP_CANCELLED };

// ======================== ENUM TO STRING CONVERTERS ========================

string roleToString(Role r) {
    switch (r) {
        case ROLE_ADMIN:   return "Admin";
        case ROLE_STUDENT: return "Student";
        case ROLE_FACULTY: return "Faculty";
        default:           return "Unknown";
    }
}

string eventTypeToString(EventType t) {
    switch (t) {
        case SEMINAR:    return "Seminar";
        case WORKSHOP:   return "Workshop";
        case CONFERENCE: return "Conference";
        case CULTURAL:   return "Cultural";
        case SPORTS:     return "Sports";
        case TECH_TALK:  return "Tech Talk";
        case OTHER:      return "Other";
        default:         return "Unknown";
    }
}

string statusToString(EventStatus s) {
    switch (s) {
        case UPCOMING:  return "Upcoming";
        case ONGOING:   return "Ongoing";
        case COMPLETED: return "Completed";
        case CANCELLED: return "Cancelled";
        default:        return "Unknown";
    }
}

string rsvpStatusToString(RSVPStatus r) {
    switch (r) {
        case RSVP_PENDING:   return "Pending";
        case RSVP_CONFIRMED: return "Confirmed";
        case RSVP_CANCELLED: return "Cancelled";
        default:             return "Unknown";
    }
}

// ======================== DATE STRUCT (User-Defined Datatype) ========================

struct Date {
    int day;
    int month;
    int year;

    Date() : day(1), month(1), year(2024) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    // --- Operator overloading for comparisons (STL sort needs <) ---
    bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }

    bool operator==(const Date& other) const {
        return (day == other.day && month == other.month && year == other.year);
    }

    bool operator!=(const Date& other) const {
        return !(*this == other);
    }

    // --- Validation ---
    bool isValid() const {
        if (year < 2020 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        // Leap year check
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            daysInMonth[2] = 29;
        return (day >= 1 && day <= daysInMonth[month]);
    }

    // --- Get today's date (uses <ctime>) ---
    static Date today() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        return Date(ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);
    }

    // --- Display ---
    string toString() const {
        stringstream ss;
        ss << setfill('0') << setw(2) << day << "/"
           << setfill('0') << setw(2) << month << "/"
           << year;
        return ss.str();
    }

    // --- Serialize for file I/O ---
    string serialize() const {
        return to_string(day) + DELIMITER + to_string(month) + DELIMITER + to_string(year);
    }

    // --- Deserialize from file I/O ---
    static Date deserialize(const string& data) {
        stringstream ss(data);
        string token;
        int d = 1, m = 1, y = 2024;
        if (getline(ss, token, '|')) d = stoi(token);
        if (getline(ss, token, '|')) m = stoi(token);
        if (getline(ss, token, '|')) y = stoi(token);
        return Date(d, m, y);
    }
};

// ======================== VENUE CLASS ========================

class Venue {
private:
    string venueID;
    string name;
    int capacity;
    string location;
    bool available;

public:
    // --- Constructors ---
    Venue() : venueID(""), name(""), capacity(0), location(""), available(true) {}
    Venue(string id, string n, int cap, string loc)
        : venueID(id), name(n), capacity(cap), location(loc), available(true) {}

    // --- Getters ---
    string getVenueID()   const { return venueID; }
    string getName()      const { return name; }
    int    getCapacity()  const { return capacity; }
    string getLocation()  const { return location; }
    bool   isAvailable()  const { return available; }

    // --- Setters ---
    void setAvailable(bool avail) { available = avail; }

    // --- Display ---
    void display() const {
        cout << left
             << setw(12) << venueID
             << setw(25) << name
             << setw(10) << capacity
             << setw(20) << location
             << setw(12) << (available ? "Available" : "Occupied")
             << endl;
    }

    // --- File I/O ---
    string serialize() const {
        return venueID + DELIMITER + name + DELIMITER +
               to_string(capacity) + DELIMITER + location + DELIMITER +
               to_string(available);
    }

    static Venue deserialize(const string& data) {
        stringstream ss(data);
        string token;
        Venue v;
        if (getline(ss, token, '|')) v.venueID = token;
        if (getline(ss, token, '|')) v.name = token;
        if (getline(ss, token, '|')) v.capacity = stoi(token);
        if (getline(ss, token, '|')) v.location = token;
        if (getline(ss, token, '|')) v.available = (token == "1");
        return v;
    }
};

// ======================== RSVP STRUCT ========================

struct RSVP {
    string studentID;
    string eventID;
    RSVPStatus status;
    bool attended;
    Date rsvpDate;

    RSVP() : status(RSVP_PENDING), attended(false) {}

    string serialize() const {
        return studentID + DELIMITER + eventID + DELIMITER +
               to_string(status) + DELIMITER + to_string(attended) + DELIMITER +
               rsvpDate.serialize();
    }

    static RSVP deserialize(const string& data) {
        stringstream ss(data);
        string token;
        RSVP r;
        if (getline(ss, token, '|')) r.studentID = token;
        if (getline(ss, token, '|')) r.eventID = token;
        if (getline(ss, token, '|')) r.status = static_cast<RSVPStatus>(stoi(token));
        if (getline(ss, token, '|')) r.attended = (token == "1");
        if (getline(ss, token, '|')) r.rsvpDate = Date::deserialize(token);
        return r;
    }
};

// ======================== EVENT CLASS ========================

class Event {
private:
    string eventID;
    string title;
    string description;
    EventType type;
    Date startDate;
    Date endDate;
    string venueID;
    int capacity;
    double fee;
    double budget;
    string organizerID;
    EventStatus status;
    vector<RSVP> rsvpList;

    // --- Private helper: generate unique ID ---
    static int eventCounter;

public:
    // --- Constructor ---
    Event()
        : eventID(""), title(""), description(""), type(SEMINAR),
          capacity(0), fee(0.0), budget(0.0), organizerID(""), status(UPCOMING) {}

    Event(string id, string t, string desc, EventType tp, Date sd, Date ed,
          string vid, int cap, double f, double b, string orgID, EventStatus st)
        : eventID(id), title(t), description(desc), type(tp),
          startDate(sd), endDate(ed), venueID(vid), capacity(cap),
          fee(f), budget(b), organizerID(orgID), status(st) {}

    // --- Getters ---
    string      getEventID()    const { return eventID; }
    string      getTitle()      const { return title; }
    string      getDescription()const { return description; }
    EventType   getType()       const { return type; }
    Date        getStartDate()  const { return startDate; }
    Date        getEndDate()    const { return endDate; }
    string      getVenueID()    const { return venueID; }
    int         getCapacity()   const { return capacity; }
    double      getFee()        const { return fee; }
    double      getBudget()     const { return budget; }
    string      getOrganizerID()const { return organizerID; }
    EventStatus getStatus()     const { return status; }
    vector<RSVP>& getRSVPList()       { return rsvpList; }
    const vector<RSVP>& getRSVPList() const { return rsvpList; }

    // --- Setters ---
    void setTitle(const string& t)           { title = t; }
    void setDescription(const string& d)     { description = d; }
    void setType(EventType t)                { type = t; }
    void setStartDate(Date d)                { startDate = d; }
    void setEndDate(Date d)                  { endDate = d; }
    void setVenueID(const string& v)         { venueID = v; }
    void setCapacity(int c)                  { capacity = c; }
    void setFee(double f)                    { fee = f; }
    void setBudget(double b)                 { budget = b; }
    void setStatus(EventStatus s)            { status = s; }

    // --- Get confirmed attendee count ---
    int getConfirmedCount() const {
        return count_if(rsvpList.begin(), rsvpList.end(),
            [](const RSVP& r) { return r.status == RSVP_CONFIRMED; });
    }

    // --- Check if student already RSVP'd ---
    bool hasStudentRSVP(const string& studentID) const {
        return any_of(rsvpList.begin(), rsvpList.end(),
            [&studentID](const RSVP& r) { return r.studentID == studentID; });
    }

    // --- Add RSVP (pointer parameter to demonstrate pointer usage) ---
    bool addRSVP(RSVP* rsvpPtr) {
        if (rsvpPtr == nullptr) return false;
        if (hasStudentRSVP(rsvpPtr->studentID)) return false;
        if (getConfirmedCount() >= capacity) return false;
        rsvpList.push_back(*rsvpPtr);
        return true;
    }

    // --- Cancel RSVP by student ID ---
    bool cancelRSVP(const string& studentID) {
        auto it = find_if(rsvpList.begin(), rsvpList.end(),
            [&studentID](const RSVP& r) { return r.studentID == studentID; });
        if (it != rsvpList.end()) {
            it->status = RSVP_CANCELLED;
            return true;
        }
        return false;
    }

    // --- Mark attendance ---
    bool markAttendance(const string& studentID, bool present) {
        auto it = find_if(rsvpList.begin(), rsvpList.end(),
            [&studentID](const RSVP& r) { return r.studentID == studentID; });
        if (it != rsvpList.end() && it->status == RSVP_CONFIRMED) {
            it->attended = present;
            return true;
        }
        return false;
    }

    // --- Calculate total revenue from fees ---
    double calculateRevenue() const {
        return getConfirmedCount() * fee;
    }

    // --- Calculate profit/loss ---
    double calculateProfitLoss() const {
        return calculateRevenue() - budget;
    }

    // --- Display event details ---
    void display(const map<string, Venue>& venueMap) const {
        cout << LINE_SEP << endl;
        cout << "  EVENT ID       : " << eventID << endl;
        cout << "  TITLE          : " << title << endl;
        cout << "  DESCRIPTION    : " << description << endl;
        cout << "  TYPE           : " << eventTypeToString(type) << endl;
        cout << "  START DATE     : " << startDate.toString() << endl;
        cout << "  END DATE       : " << endDate.toString() << endl;

        // Look up venue name from map (STL map usage)
        auto vit = venueMap.find(venueID);
        if (vit != venueMap.end())
            cout << "  VENUE          : " << vit->second.getName()
                 << " (" << vit->second.getLocation() << ")" << endl;
        else
            cout << "  VENUE          : [Unknown - " << venueID << "]" << endl;

        cout << "  CAPACITY       : " << getConfirmedCount() << " / " << capacity << endl;
        cout << "  FEE            : $" << fixed << setprecision(2) << fee << endl;
        cout << "  BUDGET         : $" << fixed << setprecision(2) << budget << endl;
        cout << "  REVENUE        : $" << fixed << setprecision(2) << calculateRevenue() << endl;
        cout << "  PROFIT/LOSS    : $" << fixed << setprecision(2) << calculateProfitLoss() << endl;
        cout << "  ORGANIZER ID   : " << organizerID << endl;
        cout << "  STATUS         : " << statusToString(status) << endl;
        cout << LINE_SEP << endl;
    }

    // --- Display in table row format ---
    void displayRow(const map<string, Venue>& venueMap) const {
        cout << left
             << setw(12) << eventID
             << setw(25) << title
             << setw(12) << eventTypeToString(type)
             << setw(12) << startDate.toString()
             << setw(15) << (venueMap.count(venueID) ? venueMap.at(venueID).getName() : "N/A")
             << setw(8)  << (to_string(getConfirmedCount()) + "/" + to_string(capacity))
             << setw(12) << statusToString(status)
             << endl;
    }

    // --- File I/O: Serialize ---
    string serialize() const {
        stringstream ss;
        ss << eventID << DELIMITER << title << DELIMITER << description << DELIMITER
           << type << DELIMITER << startDate.serialize() << DELIMITER
           << endDate.serialize() << DELIMITER << venueID << DELIMITER
           << capacity << DELIMITER << fee << DELIMITER << budget << DELIMITER
           << organizerID << DELIMITER << status << DELIMITER
           << rsvpList.size();
        for (const auto& r : rsvpList) {
            ss << DELIMITER << r.serialize();
        }
        return ss.str();
    }

    // --- File I/O: Deserialize ---
    static Event deserialize(const string& data) {
        stringstream ss(data);
        string token;
        Event e;

        if (getline(ss, token, '|')) e.eventID = token;
        if (getline(ss, token, '|')) e.title = token;
        if (getline(ss, token, '|')) e.description = token;
        if (getline(ss, token, '|')) e.type = static_cast<EventType>(stoi(token));
        if (getline(ss, token, '|')) e.startDate = Date::deserialize(token);
        if (getline(ss, token, '|')) e.endDate = Date::deserialize(token);
        if (getline(ss, token, '|')) e.venueID = token;
        if (getline(ss, token, '|')) e.capacity = stoi(token);
        if (getline(ss, token, '|')) e.fee = stod(token);
        if (getline(ss, token, '|')) e.budget = stod(token);
        if (getline(ss, token, '|')) e.organizerID = token;
        if (getline(ss, token, '|')) e.status = static_cast<EventStatus>(stoi(token));

        int rsvpCount = 0;
        if (getline(ss, token, '|')) rsvpCount = stoi(token);

        for (int i = 0; i < rsvpCount; i++) {
            if (getline(ss, token, '|')) {
                // The RSVP data itself contains delimiters, so we need to read the right number of sub-tokens
                // We'll reconstruct the RSVP string by reading 5 sub-tokens
                string rsvpData = token;
                for (int j = 0; j < 4; j++) {
                    string sub;
                    if (getline(ss, sub, '|')) rsvpData += "|" + sub;
                }
                e.rsvpList.push_back(RSVP::deserialize(rsvpData));
            }
        }
        return e;
    }
};

int Event::eventCounter = 1000;

// ======================== USER CLASS ========================

class User {
private:
    string userID;
    string name;
    string email;
    string password;
    Role role;
    string department;

public:
    // --- Constructors ---
    User() : role(ROLE_STUDENT) {}
    User(string id, string n, string em, string pw, Role r, string dept)
        : userID(id), name(n), email(em), password(pw), role(r), department(dept) {}

    // --- Getters ---
    string getUserID()   const { return userID; }
    string getName()     const { return name; }
    string getEmail()    const { return email; }
    string getPassword() const { return password; }
    Role   getRole()     const { return role; }
    string getDepartment()const { return department; }

    // --- Setters ---
    void setName(const string& n)     { name = n; }
    void setEmail(const string& e)    { email = e; }
    void setPassword(const string& p) { password = p; }
    void setDepartment(const string& d){ department = d; }

    // --- Display ---
    void display() const {
        cout << left
             << setw(12) << userID
             << setw(20) << name
             << setw(25) << email
             << setw(15) << roleToString(role)
             << setw(15) << department
             << endl;
    }

    // --- File I/O ---
    string serialize() const {
        return userID + DELIMITER + name + DELIMITER + email + DELIMITER +
               password + DELIMITER + to_string(role) + DELIMITER + department;
    }

    static User deserialize(const string& data) {
        stringstream ss(data);
        string token;
        User u;
        if (getline(ss, token, '|')) u.userID = token;
        if (getline(ss, token, '|')) u.name = token;
        if (getline(ss, token, '|')) u.email = token;
        if (getline(ss, token, '|')) u.password = token;
        if (getline(ss, token, '|')) u.role = static_cast<Role>(stoi(token));
        if (getline(ss, token, '|')) u.department = token;
        return u;
    }
};

// ======================== CAMPUS EVENT MANAGEMENT SYSTEM ========================

class CampusEventManagementSystem {
private:
    vector<User>   users;
    vector<Event>  events;
    vector<Venue>  venues;
    map<string, Venue> venueMap;   // STL map for O(log n) venue lookup

    User* currentUser;             // Pointer to current logged-in user
    bool running;

    // ==================== FILE HANDLING ====================

    // --- Generic file write using templates (advanced STL concept) ---
    template <typename T>
    void saveToFile(const string& filename, const vector<T>& items,
                    string (T::*serializeFn)() const) {
        ofstream outFile(filename);
        if (!outFile) {
            cout << "  [ERROR] Cannot open file for writing: " << filename << endl;
            return;
        }
        for (const auto& item : items) {
            outFile << (item.*serializeFn)() << "\n";
        }
        outFile.close();
    }

    // --- Generic file read using function pointer ---
    template <typename T>
    void loadFromFile(const string& filename, vector<T>& items,
                      T (*deserializeFn)(const string&)) {
        ifstream inFile(filename);
        if (!inFile) {
            cout << "  [INFO] File not found, starting fresh: " << filename << endl;
            return;
        }
        string line;
        while (getline(inFile, line)) {
            if (!line.empty()) {
                items.push_back(deserializeFn(line));
            }
        }
        inFile.close();
    }

    void saveAllData() {
        saveToFile(USERS_FILE,  users,  &User::serialize);
        saveToFile(EVENTS_FILE, events, &Event::serialize);
        saveToFile(VENUES_FILE, venues, &Venue::serialize);
    }

    void loadAllData() {
        loadFromFile(USERS_FILE,  users,  &User::deserialize);
        loadFromFile(EVENTS_FILE, events, &Event::deserialize);
        loadFromFile(VENUES_FILE, venues, &Venue::deserialize);
        rebuildVenueMap();
    }

    // --- Rebuild the STL map from venue vector ---
    void rebuildVenueMap() {
        venueMap.clear();
        for (const auto& v : venues) {
            venueMap[v.getVenueID()] = v;
        }
    }

    // ==================== UTILITY FUNCTIONS ====================

    void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    int getIntInput(const string& prompt, int minVal, int maxVal) {
        int value;
        while (true) {
            cout << prompt;
            cin >> value;
            if (cin.fail() || value < minVal || value > maxVal) {
                cout << "  [ERROR] Invalid input. Please enter a number between "
                     << minVal << " and " << maxVal << "." << endl;
                clearInputBuffer();
            } else {
                clearInputBuffer();
                return value;
            }
        }
    }

    double getDoubleInput(const string& prompt, double minVal = 0.0) {
        double value;
        while (true) {
            cout << prompt;
            cin >> value;
            if (cin.fail() || value < minVal) {
                cout << "  [ERROR] Invalid input. Please enter a valid number." << endl;
                clearInputBuffer();
            } else {
                clearInputBuffer();
                return value;
            }
        }
    }

    string getStringInput(const string& prompt) {
        string value;
        cout << prompt;
        getline(cin, value);
        return value;
    }

    Date getDateInput(const string& prompt) {
        Date d;
        while (true) {
            cout << prompt << " (DD MM YYYY): ";
            cin >> d.day >> d.month >> d.year;
            clearInputBuffer();
            if (d.isValid()) return d;
            cout << "  [ERROR] Invalid date. Please try again." << endl;
        }
    }

    // --- Generate unique IDs ---
    string generateUserID(Role role) {
        int count = users.size() + 1;
        stringstream ss;
        switch (role) {
            case ROLE_ADMIN:   ss << "ADM" << setfill('0') << setw(4) << count; break;
            case ROLE_STUDENT: ss << "STU" << setfill('0') << setw(4) << count; break;
            case ROLE_FACULTY: ss << "FAC" << setfill('0') << setw(4) << count; break;
        }
        return ss.str();
    }

    string generateEventID() {
        Event::eventCounter++;
        stringstream ss;
        ss << "EVT" << setfill('0') << setw(4) << Event::eventCounter;
        return ss.str();
    }

    // --- Find user by ID (returns pointer) ---
    User* findUserByID(const string& id) {
        for (auto& u : users) {
            if (u.getUserID() == id) return &u;  // Pointer to element in vector
        }
        return nullptr;
    }

    // --- Find event by ID (returns pointer) ---
    Event* findEventByID(const string& id) {
        for (auto& e : events) {
            if (e.getEventID() == id) return &e;
        }
        return nullptr;
    }

    // --- Find venue by ID (returns pointer) ---
    Venue* findVenueByID(const string& id) {
        for (auto& v : venues) {
            if (v.getVenueID() == id) return &v;
        }
        return nullptr;
    }

    // --- Print table header for events ---
    void printEventTableHeader() const {
        cout << left
             << setw(12) << "Event ID"
             << setw(25) << "Title"
             << setw(12) << "Type"
             << setw(12) << "Date"
             << setw(15) << "Venue"
             << setw(8)  << "Filled"
             << setw(12) << "Status"
             << endl;
        cout << string(96, '-') << endl;
    }

    // --- Print table header for venues ---
    void printVenueTableHeader() const {
        cout << left
             << setw(12) << "Venue ID"
             << setw(25) << "Name"
             << setw(10) << "Capacity"
             << setw(20) << "Location"
             << setw(12) << "Status"
             << endl;
        cout << string(79, '-') << endl;
    }

    // --- Print table header for users ---
    void printUserTableHeader() const {
        cout << left
             << setw(12) << "User ID"
             << setw(20) << "Name"
             << setw(25) << "Email"
             << setw(15) << "Role"
             << setw(15) << "Department"
             << endl;
        cout << string(87, '-') << endl;
    }

    // --- Pause for user ---
    void pause() {
        cout << "\n  Press Enter to continue...";
        cin.get();
    }

    // ==================== SEED DATA ====================

    void seedDefaultData() {
        // Only seed if no data exists
        if (!users.empty()) return;

        // Default users
        users.push_back(User("ADM0001", "System Admin", "admin@campus.edu", "admin123", ROLE_ADMIN, "Administration"));
        users.push_back(User("FAC0001", "Dr. Sarah Johnson", "sarah.j@campus.edu", "fac123", ROLE_FACULTY, "Computer Science"));
        users.push_back(User("FAC0002", "Prof. Michael Lee", "michael.l@campus.edu", "fac123", ROLE_FACULTY, "Electrical Eng."));
        users.push_back(User("STU0001", "Alice Chen", "alice.c@campus.edu", "stu123", ROLE_STUDENT, "Computer Science"));
        users.push_back(User("STU0002", "Bob Martinez", "bob.m@campus.edu", "stu123", ROLE_STUDENT, "Computer Science"));
        users.push_back(User("STU0003", "Carol Davis", "carol.d@campus.edu", "stu123", ROLE_STUDENT, "Mechanical Eng."));

        // Default venues
        venues.push_back(Venue("VEN001", "Main Auditorium",      500, "Building A, Floor 1"));
        venues.push_back(Venue("VEN002", "Conference Room 101",   50, "Building B, Floor 1"));
        venues.push_back(Venue("VEN003", "Seminar Hall",         150, "Building C, Floor 2"));
        venues.push_back(Venue("VEN004", "Sports Complex",       300, "Outdoor, East Campus"));
        venues.push_back(Venue("VEN005", "Open Air Theater",     200, "Central Lawn"));
        rebuildVenueMap();

        // Default events
        Date today = Date::today();
        Event e1("EVT1001", "AI & Machine Learning Workshop",
                 "Hands-on workshop covering ML fundamentals and practical applications",
                 WORKSHOP, today, today, "VEN003", 100, 25.00, 1500.00,
                 "FAC0001", UPCOMING);

        RSVP r1;
        r1.studentID = "STU0001";
        r1.eventID = "EVT1001";
        r1.status = RSVP_CONFIRMED;
        r1.attended = false;
        r1.rsvpDate = today;
        e1.addRSVP(&r1);

        RSVP r2;
        r2.studentID = "STU0002";
        r2.eventID = "EVT1001";
        r2.status = RSVP_CONFIRMED;
        r2.attended = false;
        r2.rsvpDate = today;
        e1.addRSVP(&r2);

        events.push_back(e1);

        Date pastDate(today.day > 5 ? today.day - 5 : 1, today.month, today.year);
        Event e2("EVT1002", "Annual Cultural Festival",
                 "A grand celebration of art, music, and dance from diverse cultures",
                 CULTURAL, pastDate, pastDate, "VEN001", 400, 10.00, 5000.00,
                 "FAC0002", COMPLETED);

        RSVP r3;
        r3.studentID = "STU0001";
        r3.eventID = "EVT1002";
        r3.status = RSVP_CONFIRMED;
        r3.attended = true;
        r3.rsvpDate = pastDate;
        e2.addRSVP(&r3);

        events.push_back(e2);

        Event::eventCounter = 1002;
        saveAllData();
        cout << "  [INFO] Default data seeded successfully." << endl;
    }

    // ==================== AUTHENTICATION ====================

    bool login() {
        cout << "\n" << LINE_SEP << endl;
        cout << "                    CAMPUS EVENT MANAGEMENT SYSTEM" << endl;
        cout << "                           LOGIN SCREEN" << endl;
        cout << LINE_SEP << endl;

        string id = getStringInput("  User ID     : ");
        string pw = getStringInput("  Password    : ");

        // Using STL algorithm to find user
        auto it = find_if(users.begin(), users.end(),
            [&id, &pw](const User& u) {
                return u.getUserID() == id && u.getPassword() == pw;
            });

        if (it != users.end()) {
            currentUser = &(*it);  // Store pointer to the found user
            cout << "\n  [SUCCESS] Welcome back, " << currentUser->getName()
                 << " (" << roleToString(currentUser->getRole()) << ")!" << endl;
            return true;
        }

        cout << "\n  [ERROR] Invalid User ID or Password." << endl;
        return false;
    }

    void logout() {
        currentUser = nullptr;
        cout << "\n  [INFO] Logged out successfully." << endl;
    }

    // ==================== ADMIN FUNCTIONS ====================

    void adminMenu() {
        int choice;
        do {
            cout << "\n" << LINE_SEP << endl;
            cout << "                    ADMIN DASHBOARD" << endl;
            cout << LINE_SEP << endl;
            cout << "  1.  Manage Users (View / Add / Remove)" << endl;
            cout << "  2.  Manage Venues (View / Add / Remove)" << endl;
            cout << "  3.  View All Events" << endl;
            cout << "  4.  View Budget Summary" << endl;
            cout << "  5.  Change Password" << endl;
            cout << "  0.  Logout" << endl;
            cout << LINE_SEP << endl;
            choice = getIntInput("  Enter choice: ", 0, 5);

            switch (choice) {
                case 1: manageUsers(); break;
                case 2: manageVenues(); break;
                case 3: viewAllEvents(); break;
                case 4: viewBudgetSummary(); break;
                case 5: changePassword(); break;
                case 0: logout(); break;
            }
        } while (choice != 0 && currentUser != nullptr);
    }

    void manageUsers() {
        int choice;
        do {
            cout << "\n--- Manage Users ---" << endl;
            cout << "  1. View All Users" << endl;
            cout << "  2. Add New User" << endl;
            cout << "  3. Remove User" << endl;
            cout << "  0. Back" << endl;
            choice = getIntInput("  Enter choice: ", 0, 3);

            switch (choice) {
                case 1: {
                    printUserTableHeader();
                    for (const auto& u : users) u.display();
                    pause();
                    break;
                }
                case 2: {
                    cout << "\n  --- Add New User ---" << endl;
                    int roleChoice = getIntInput("  Role (1-Admin, 2-Student, 3-Faculty): ", 1, 3);
                    Role newRole = static_cast<Role>(roleChoice);
                    string name = getStringInput("  Full Name    : ");
                    string email = getStringInput("  Email        : ");
                    string pw = getStringInput("  Password     : ");
                    string dept = getStringInput("  Department   : ");
                    string newID = generateUserID(newRole);
                    users.push_back(User(newID, name, email, pw, newRole, dept));
                    saveAllData();
                    cout << "  [SUCCESS] User added with ID: " << newID << endl;
                    pause();
                    break;
                }
                case 3: {
                    string id = getStringInput("  Enter User ID to remove: ");
                    auto it = find_if(users.begin(), users.end(),
                        [&id](const User& u) { return u.getUserID() == id; });
                    if (it != users.end()) {
                        if (it->getUserID() == currentUser->getUserID()) {
                            cout << "  [ERROR] Cannot remove your own account!" << endl;
                        } else {
                            users.erase(it);
                            saveAllData();
                            cout << "  [SUCCESS] User removed." << endl;
                        }
                    } else {
                        cout << "  [ERROR] User not found." << endl;
                    }
                    pause();
                    break;
                }
            }
        } while (choice != 0);
    }

    void manageVenues() {
        int choice;
        do {
            cout << "\n--- Manage Venues ---" << endl;
            cout << "  1. View All Venues" << endl;
            cout << "  2. Add New Venue" << endl;
            cout << "  3. Remove Venue" << endl;
            cout << "  4. Toggle Venue Availability" << endl;
            cout << "  0. Back" << endl;
            choice = getIntInput("  Enter choice: ", 0, 4);

            switch (choice) {
                case 1: {
                    printVenueTableHeader();
                    for (const auto& v : venues) v.display();
                    pause();
                    break;
                }
                case 2: {
                    cout << "\n  --- Add New Venue ---" << endl;
                    string vid = "VEN" + to_string(venues.size() + 1);
                    // Pad with zeros
                    stringstream ss;
                    ss << "VEN" << setfill('0') << setw(3) << (venues.size() + 1);
                    vid = ss.str();
                    string name = getStringInput("  Venue Name   : ");
                    int cap = getIntInput("  Capacity     : ", 1, 10000);
                    string loc = getStringInput("  Location     : ");
                    venues.push_back(Venue(vid, name, cap, loc));
                    rebuildVenueMap();
                    saveAllData();
                    cout << "  [SUCCESS] Venue added with ID: " << vid << endl;
                    pause();
                    break;
                }
                case 3: {
                    string vid = getStringInput("  Enter Venue ID to remove: ");
                    auto it = find_if(venues.begin(), venues.end(),
                        [&vid](const Venue& v) { return v.getVenueID() == vid; });
                    if (it != venues.end()) {
                        // Check if any event uses this venue
                        bool inUse = any_of(events.begin(), events.end(),
                            [&vid](const Event& e) { return e.getVenueID() == vid; });
                        if (inUse) {
                            cout << "  [ERROR] Cannot remove venue: it is assigned to an event." << endl;
                        } else {
                            venues.erase(it);
                            rebuildVenueMap();
                            saveAllData();
                            cout << "  [SUCCESS] Venue removed." << endl;
                        }
                    } else {
                        cout << "  [ERROR] Venue not found." << endl;
                    }
                    pause();
                    break;
                }
                case 4: {
                    string vid = getStringInput("  Enter Venue ID: ");
                    Venue* v = findVenueByID(vid);
                    if (v) {
                        v->setAvailable(!v->isAvailable());
                        rebuildVenueMap();
                        saveAllData();
                        cout << "  [SUCCESS] Venue " << v->getName()
                             << " is now " << (v->isAvailable() ? "Available" : "Occupied") << endl;
                    } else {
                        cout << "  [ERROR] Venue not found." << endl;
                    }
                    pause();
                    break;
                }
            }
        } while (choice != 0);
    }

    void viewAllEvents() {
        cout << "\n--- All Events (" << events.size() << ") ---" << endl;
        printEventTableHeader();
        // Sort events chronologically using STL sort with custom comparator
        vector<Event*> sortedPtrs;
        for (auto& e : events) sortedPtrs.push_back(&e);
        sort(sortedPtrs.begin(), sortedPtrs.end(),
            [](const Event* a, const Event* b) {
                return a->getStartDate() < b->getStartDate();
            });
        for (const auto& ep : sortedPtrs) ep->displayRow(venueMap);
        pause();
    }

    void viewBudgetSummary() {
        cout << "\n" << LINE_SEP << endl;
        cout << "              BUDGET SUMMARY REPORT" << endl;
        cout << LINE_SEP << endl;

        double totalBudget = 0, totalRevenue = 0, totalProfit = 0;
        int eventCount = 0;

        for (const auto& e : events) {
            totalBudget  += e.getBudget();
            totalRevenue += e.calculateRevenue();
            totalProfit  += e.calculateProfitLoss();
            eventCount++;
        }

        cout << fixed << setprecision(2);
        cout << "  Total Events       : " << eventCount << endl;
        cout << "  Total Budget       : $" << totalBudget << endl;
        cout << "  Total Revenue      : $" << totalRevenue << endl;
        cout << "  Total Profit/Loss  : $" << totalProfit << endl;
        cout << LINE_SEP << endl;

        // Per-event breakdown
        cout << "\n  Per-Event Breakdown:" << endl;
        cout << left
             << setw(12) << "Event ID"
             << setw(20) << "Title"
             << setw(12) << "Budget"
             << setw(12) << "Revenue"
             << setw(12) << "P/L"
             << endl;
        cout << string(68, '-') << endl;
        for (const auto& e : events) {
            cout << left
                 << setw(12) << e.getEventID()
                 << setw(20) << e.getTitle().substr(0, 18)
                 << setw(12) << e.getBudget()
                 << setw(12) << e.calculateRevenue()
                 << setw(12) << e.calculateProfitLoss()
                 << endl;
        }
        pause();
    }

    void changePassword() {
        string oldPw = getStringInput("  Enter current password: ");
        if (oldPw != currentUser->getPassword()) {
            cout << "  [ERROR] Incorrect password." << endl;
            return;
        }
        string newPw = getStringInput("  Enter new password    : ");
        string confirmPw = getStringInput("  Confirm new password : ");
        if (newPw != confirmPw) {
            cout << "  [ERROR] Passwords do not match." << endl;
            return;
        }
        currentUser->setPassword(newPw);
        saveAllData();
        cout << "  [SUCCESS] Password changed successfully." << endl;
        pause();
    }

    // ==================== FACULTY FUNCTIONS ====================

    void facultyMenu() {
        int choice;
        do {
            cout << "\n" << LINE_SEP << endl;
            cout << "                   FACULTY DASHBOARD" << endl;
            cout << LINE_SEP << endl;
            cout << "  1.  Create New Event" << endl;
            cout << "  2.  View My Events" << endl;
            cout << "  3.  Edit Event" << endl;
            cout << "  4.  Swap Event Venue" << endl;
            cout << "  5.  Manage Attendees (View / Remove)" << endl;
            cout << "  6.  Verify Attendance" << endl;
            cout << "  7.  View Event Budget Details" << endl;
            cout << "  8.  Cancel Event" << endl;
            cout << "  9.  Change Password" << endl;
            cout << "  0.  Logout" << endl;
            cout << LINE_SEP << endl;
            choice = getIntInput("  Enter choice: ", 0, 9);

            switch (choice) {
                case 1: createEvent(); break;
                case 2: viewMyEvents(); break;
                case 3: editEvent(); break;
                case 4: swapVenue(); break;
                case 5: manageAttendees(); break;
                case 6: verifyAttendance(); break;
                case 7: viewEventBudget(); break;
                case 8: cancelEvent(); break;
                case 9: changePassword(); break;
                case 0: logout(); break;
            }
        } while (choice != 0 && currentUser != nullptr);
    }

    void createEvent() {
        cout << "\n--- Create New Event ---" << endl;

        string eid = generateEventID();
        string title = getStringInput("  Event Title      : ");
        string desc  = getStringInput("  Description      : ");

        cout << "  Event Type (1-Seminar, 2-Workshop, 3-Conference,\n"
             "               4-Cultural, 5-Sports, 6-Tech Talk, 7-Other)" << endl;
        int typeChoice = getIntInput("  Type             : ", 1, 7);
        EventType etype = static_cast<EventType>(typeChoice);

        Date startDate = getDateInput("  Start Date       ");
        Date endDate   = getDateInput("  End Date         ");

        // Show available venues
        cout << "\n  Available Venues:" << endl;
        printVenueTableHeader();
        for (const auto& v : venues) {
            if (v.isAvailable()) v.display();
        }
        string vid = getStringInput("\n  Venue ID         : ");
        Venue* venue = findVenueByID(vid);
        if (!venue) {
            cout << "  [ERROR] Venue not found." << endl;
            pause();
            return;
        }

        int cap = getIntInput("  Capacity (max " + to_string(venue->getCapacity()) + ") : ",
                              1, venue->getCapacity());
        double fee    = getDoubleInput("  Registration Fee  : $");
        double budget = getDoubleInput("  Budget           : $");

        Event newEvent(eid, title, desc, etype, startDate, endDate,
                       vid, cap, fee, budget, currentUser->getUserID(), UPCOMING);
        events.push_back(newEvent);
        saveAllData();

        cout << "\n  [SUCCESS] Event created with ID: " << eid << endl;
        newEvent.display(venueMap);
        pause();
    }

    void viewMyEvents() {
        cout << "\n--- My Events ---" << endl;

        // Filter events by organizer using STL algorithm
        vector<Event*> myEvents;
        for (auto& e : events) {
            if (e.getOrganizerID() == currentUser->getUserID())
                myEvents.push_back(&e);
        }

        if (myEvents.empty()) {
            cout << "  You have not created any events yet." << endl;
            pause();
            return;
        }

        // Sort chronologically
        sort(myEvents.begin(), myEvents.end(),
            [](const Event* a, const Event* b) {
                return a->getStartDate() < b->getStartDate();
            });

        printEventTableHeader();
        for (const auto& ep : myEvents) ep->displayRow(venueMap);
        pause();
    }

    void editEvent() {
        string eid = getStringInput("  Enter Event ID to edit: ");
        Event* event = findEventByID(eid);

        if (!event || event->getOrganizerID() != currentUser->getUserID()) {
            cout << "  [ERROR] Event not found or you don't have permission." << endl;
            pause();
            return;
        }

        if (event->getStatus() == COMPLETED || event->getStatus() == CANCELLED) {
            cout << "  [ERROR] Cannot edit a completed or cancelled event." << endl;
            pause();
            return;
        }

        cout << "\n  Current Event Details:" << endl;
        event->display(venueMap);

        cout << "\n  --- Edit Fields (leave blank to keep current) ---" << endl;

        string title = getStringInput("  New Title [" + event->getTitle() + "]: ");
        if (!title.empty()) event->setTitle(title);

        string desc = getStringInput("  New Description [" + event->getDescription().substr(0, 30) + "...]: ");
        if (!desc.empty()) event->setDescription(desc);

        string capStr = getStringInput("  New Capacity [" + to_string(event->getCapacity()) + "]: ");
        if (!capStr.empty()) {
            int newCap = stoi(capStr);
            if (newCap >= event->getConfirmedCount()) {
                event->setCapacity(newCap);
            } else {
                cout << "  [WARNING] Capacity cannot be less than current attendees. Keeping old value." << endl;
            }
        }

        string feeStr = getStringInput("  New Fee [$" + to_string(event->getFee()) + "]: ");
        if (!feeStr.empty()) event->setFee(stod(feeStr));

        string budgetStr = getStringInput("  New Budget [$" + to_string(event->getBudget()) + "]: ");
        if (!budgetStr.empty()) event->setBudget(stod(budgetStr));

        saveAllData();
        cout << "\n  [SUCCESS] Event updated." << endl;
        event->display(venueMap);
        pause();
    }

    void swapVenue() {
        string eid = getStringInput("  Enter Event ID: ");
        Event* event = findEventByID(eid);

        if (!event || event->getOrganizerID() != currentUser->getUserID()) {
            cout << "  [ERROR] Event not found or no permission." << endl;
            pause();
            return;
        }

        cout << "\n  Current Venue: " << event->getVenueID();
        auto oldVenue = venueMap.find(event->getVenueID());
        if (oldVenue != venueMap.end())
            cout << " (" << oldVenue->second.getName() << ")";
        cout << endl;

        cout << "\n  Available Venues:" << endl;
        printVenueTableHeader();
        for (const auto& v : venues) {
            if (v.isAvailable() && v.getVenueID() != event->getVenueID())
                v.display();
        }

        string newVid = getStringInput("\n  New Venue ID: ");
        Venue* newVenue = findVenueByID(newVid);

        if (!newVenue) {
            cout << "  [ERROR] Venue not found." << endl;
        } else if (!newVenue->isAvailable()) {
            cout << "  [ERROR] Venue is not available." << endl;
        } else if (newVenue->getCapacity() < event->getConfirmedCount()) {
            cout << "  [ERROR] New venue capacity (" << newVenue->getCapacity()
                 << ") is less than current attendees (" << event->getConfirmedCount() << ")." << endl;
        } else {
            // Adjust capacity if new venue is smaller
            if (newVenue->getCapacity() < event->getCapacity()) {
                event->setCapacity(newVenue->getCapacity());
                cout << "  [INFO] Capacity adjusted to " << newVenue->getCapacity() << endl;
            }
            event->setVenueID(newVid);
            saveAllData();
            cout << "  [SUCCESS] Venue swapped to: " << newVenue->getName() << endl;
        }
        pause();
    }

    void manageAttendees() {
        string eid = getStringInput("  Enter Event ID: ");
        Event* event = findEventByID(eid);

        if (!event || event->getOrganizerID() != currentUser->getUserID()) {
            cout << "  [ERROR] Event not found or no permission." << endl;
            pause();
            return;
        }

        cout << "\n--- Attendees for: " << event->getTitle() << " ---" << endl;
        cout << left
             << setw(12) << "Student ID"
             << setw(20) << "Name"
             << setw(12) << "RSVP Status"
             << setw(12) << "Attended"
             << endl;
        cout << string(56, '-') << endl;

        for (const auto& r : event->getRSVPList()) {
            if (r.status == RSVP_CONFIRMED || r.status == RSVP_PENDING) {
                User* stu = findUserByID(r.studentID);
                cout << left
                     << setw(12) << r.studentID
                     << setw(20) << (stu ? stu->getName() : "Unknown")
                     << setw(12) << rsvpStatusToString(r.status)
                     << setw(12) << (r.attended ? "Yes" : "No")
                     << endl;
            }
        }

        cout << "\n  Total Confirmed: " << event->getConfirmedCount()
             << " / " << event->getCapacity() << endl;

        // Option to remove an attendee
        string choice = getStringInput("\n  Remove an attendee? (y/n): ");
        if (choice == "y" || choice == "Y") {
            string sid = getStringInput("  Enter Student ID to remove: ");
            if (event->cancelRSVP(sid)) {
                saveAllData();
                cout << "  [SUCCESS] Attendee removed." << endl;
            } else {
                cout << "  [ERROR] Student not found in attendee list." << endl;
            }
        }
        pause();
    }

    void verifyAttendance() {
        string eid = getStringInput("  Enter Event ID: ");
        Event* event = findEventByID(eid);

        if (!event || event->getOrganizerID() != currentUser->getUserID()) {
            cout << "  [ERROR] Event not found or no permission." << endl;
            pause();
            return;
        }

        cout << "\n--- Attendance Verification: " << event->getTitle() << " ---" << endl;
        cout << left
             << setw(12) << "Student ID"
             << setw(20) << "Name"
             << setw(12) << "Attended"
             << endl;
        cout << string(44, '-') << endl;

        for (const auto& r : event->getRSVPList()) {
            if (r.status == RSVP_CONFIRMED) {
                User* stu = findUserByID(r.studentID);
                cout << left
                     << setw(12) << r.studentID
                     << setw(20) << (stu ? stu->getName() : "Unknown")
                     << setw(12) << (r.attended ? "Yes" : "No")
                     << endl;
            }
        }

        string sid;
        do {
            sid = getStringInput("\n  Enter Student ID to mark attendance (or 'done' to finish): ");
            if (sid == "done" || sid == "DONE") break;

            User* stu = findUserByID(sid);
            if (!stu) {
                cout << "  [ERROR] Student not found." << endl;
                continue;
            }

            string ans = getStringInput("  Mark as present? (y/n): ");
            bool present = (ans == "y" || ans == "Y");
            if (event->markAttendance(sid, present)) {
                cout << "  [SUCCESS] Attendance updated." << endl;
            } else {
                cout << "  [ERROR] Could not update. Student may not have a confirmed RSVP." << endl;
            }
        } while (sid != "done" && sid != "DONE");

        saveAllData();

        // Show attendance summary
        int confirmed = 0, attended = 0;
        for (const auto& r : event->getRSVPList()) {
            if (r.status == RSVP_CONFIRMED) {
                confirmed++;
                if (r.attended) attended++;
            }
        }
        cout << "\n  Attendance Summary: " << attended << " / " << confirmed
             << " (" << (confirmed > 0 ? (attended * 100.0 / confirmed) : 0) << "%)" << endl;
        pause();
    }

    void viewEventBudget() {
        string eid = getStringInput("  Enter Event ID: ");
        Event* event = findEventByID(eid);

        if (!event || event->getOrganizerID() != currentUser->getUserID()) {
            cout << "  [ERROR] Event not found or no permission." << endl;
            pause();
            return;
        }

        cout << "\n" << LINE_SEP << endl;
        cout << "           BUDGET DETAILS: " << event->getTitle() << endl;
        cout << LINE_SEP << endl;
        cout << fixed << setprecision(2);
        cout << "  Allocated Budget      : $" << event->getBudget() << endl;
        cout << "  Registration Fee      : $" << event->getFee() << " per person" << endl;
        cout << "  Confirmed Attendees   : " << event->getConfirmedCount() << endl;
        cout << "  Expected Revenue      : $" << event->calculateRevenue() << endl;
        cout << "  Profit / Loss         : $" << event->calculateProfitLoss() << endl;

        // Breakdown estimation
        cout << "\n  --- Suggested Budget Breakdown ---" << endl;
        double b = event->getBudget();
        cout << "  Venue & Logistics     : $" << b * 0.40 << " (40%)" << endl;
        cout << "  Marketing             : $" << b * 0.15 << " (15%)" << endl;
        cout << "  Materials & Equipment : $" << b * 0.25 << " (25%)" << endl;
        cout << "  Contingency           : $" << b * 0.10 << " (10%)" << endl;
        cout << "  Miscellaneous         : $" << b * 0.10 << " (10%)" << endl;
        cout << LINE_SEP << endl;
        pause();
    }

    void cancelEvent() {
        string eid = getStringInput("  Enter Event ID to cancel: ");
        Event* event = findEventByID(eid);

        if (!event || event->getOrganizerID() != currentUser->getUserID()) {
            cout << "  [ERROR] Event not found or no permission." << endl;
            pause();
            return;
        }

        if (event->getStatus() == CANCELLED || event->getStatus() == COMPLETED) {
            cout << "  [ERROR] Event is already " << statusToString(event->getStatus()) << "." << endl;
            pause();
            return;
        }

        string confirm = getStringInput("  Are you sure you want to cancel '" +
                           event->getTitle() + "'? (yes/no): ");
        if (confirm == "yes") {
            event->setStatus(CANCELLED);
            // Cancel all RSVPs
            for (auto& r : event->getRSVPList()) {
                r.status = RSVP_CANCELLED;
            }
            saveAllData();
            cout << "  [SUCCESS] Event cancelled. All RSVPs have been cancelled." << endl;
        } else {
            cout << "  [INFO] Cancellation aborted." << endl;
        }
        pause();
    }

    // ==================== STUDENT FUNCTIONS ====================

    void studentMenu() {
        int choice;
        do {
            cout << "\n" << LINE_SEP << endl;
            cout << "                   STUDENT DASHBOARD" << endl;
            cout << LINE_SEP << endl;
            cout << "  1.  Browse Upcoming Events" << endl;
            cout << "  2.  Search & Filter Events" << endl;
            cout << "  3.  View Event Details" << endl;
            cout << "  4.  RSVP to an Event" << endl;
            cout << "  5.  Cancel My RSVP" << endl;
            cout << "  6.  View My RSVPs" << endl;
            cout << "  7.  View Event History (Past Events)" << endl;
            cout << "  8.  Change Password" << endl;
            cout << "  0.  Logout" << endl;
            cout << LINE_SEP << endl;
            choice = getIntInput("  Enter choice: ", 0, 8);

            switch (choice) {
                case 1: browseUpcomingEvents(); break;
                case 2: searchAndFilterEvents(); break;
                case 3: viewEventDetails(); break;
                case 4: rsvpToEvent(); break;
                case 5: cancelMyRSVP(); break;
                case 6: viewMyRSVPs(); break;
                case 7: viewEventHistory(); break;
                case 8: changePassword(); break;
                case 0: logout(); break;
            }
        } while (choice != 0 && currentUser != nullptr);
    }

    void browseUpcomingEvents() {
        cout << "\n--- Upcoming Events ---" << endl;

        Date today = Date::today();
        vector<Event*> upcoming;
        for (auto& e : events) {
            if (e.getStatus() == UPCOMING && e.getStartDate() >= today)
                upcoming.push_back(&e);
        }

        if (upcoming.empty()) {
            cout << "  No upcoming events found." << endl;
            pause();
            return;
        }

        // Sort chronologically
        sort(upcoming.begin(), upcoming.end(),
            [](const Event* a, const Event* b) {
                return a->getStartDate() < b->getStartDate();
            });

        printEventTableHeader();
        for (const auto& ep : upcoming) ep->displayRow(venueMap);
        pause();
    }

    void searchAndFilterEvents() {
        int choice;
        do {
            cout << "\n--- Search & Filter Events ---" << endl;
            cout << "  1. Filter by Date" << endl;
            cout << "  2. Filter by Type" << endl;
            cout << "  3. Filter by Venue" << endl;
            cout << "  4. Filter by Status (Past/Upcoming)" << endl;
            cout << "  5. Search by Title (keyword)" << endl;
            cout << "  0. Back" << endl;
            choice = getIntInput("  Enter choice: ", 0, 5);

            vector<Event*> results;

            switch (choice) {
                case 1: {
                    Date d = getDateInput("  Enter date to filter");
                    for (auto& e : events) {
                        if (e.getStartDate() == d) results.push_back(&e);
                    }
                    break;
                }
                case 2: {
                    cout << "  Types: 1-Seminar, 2-Workshop, 3-Conference,\n"
                         << "         4-Cultural, 5-Sports, 6-Tech Talk, 7-Other" << endl;
                    int t = getIntInput("  Enter type: ", 1, 7);
                    EventType et = static_cast<EventType>(t);
                    // Using STL copy_if with back_inserter
                    copy_if(events.begin(), events.end(), back_inserter(results),
                        [et](Event& e) { return e.getType() == et; });
                    // Note: copy_if with back_inserter needs non-const, so we use a workaround
                    results.clear();
                    for (auto& e : events) {
                        if (e.getType() == et) results.push_back(&e);
                    }
                    break;
                }
                case 3: {
                    cout << "\n  Available Venues:" << endl;
                    printVenueTableHeader();
                    for (const auto& v : venues) v.display();
                    string vid = getStringInput("\n  Enter Venue ID: ");
                    for (auto& e : events) {
                        if (e.getVenueID() == vid) results.push_back(&e);
                    }
                    break;
                }
                case 4: {
                    cout << "  1. Past Events (Completed)" << endl;
                    cout << "  2. Upcoming Events" << endl;
                    int s = getIntInput("  Choose: ", 1, 2);
                    Date today = Date::today();
                    if (s == 1) {
                        for (auto& e : events) {
                            if (e.getStatus() == COMPLETED) results.push_back(&e);
                        }
                    } else {
                        for (auto& e : events) {
                            if (e.getStatus() == UPCOMING && e.getStartDate() >= today)
                                results.push_back(&e);
                        }
                    }
                    break;
                }
                case 5: {
                    string keyword = getStringInput("  Enter keyword: ");
                    // Convert keyword to lowercase for case-insensitive search
                    string lowerKeyword = keyword;
                    transform(lowerKeyword.begin(), lowerKeyword.end(),
                              lowerKeyword.begin(), ::tolower);
                    for (auto& e : events) {
                        string lowerTitle = e.getTitle();
                        transform(lowerTitle.begin(), lowerTitle.end(),
                                  lowerTitle.begin(), ::tolower);
                        if (lowerTitle.find(lowerKeyword) != string::npos)
                            results.push_back(&e);
                    }
                    break;
                }
            }

            // Display results
            cout << "\n  Found " << results.size() << " event(s)." << endl;
            if (!results.empty()) {
                printEventTableHeader();
                // Sort results chronologically
                sort(results.begin(), results.end(),
                    [](const Event* a, const Event* b) {
                        return a->getStartDate() < b->getStartDate();
                    });
                for (const auto& ep : results) ep->displayRow(venueMap);
            }
            pause();

        } while (choice != 0);
    }

    void viewEventDetails() {
        string eid = getStringInput("  Enter Event ID: ");
        Event* event = findEventByID(eid);

        if (!event) {
            cout << "  [ERROR] Event not found." << endl;
        } else {
            event->display(venueMap);
        }
        pause();
    }

    void rsvpToEvent() {
        string eid = getStringInput("  Enter Event ID to RSVP: ");
        Event* event = findEventByID(eid);

        if (!event) {
            cout << "  [ERROR] Event not found." << endl;
            pause();
            return;
        }

        if (event->getStatus() != UPCOMING) {
            cout << "  [ERROR] Can only RSVP to upcoming events." << endl;
            pause();
            return;
        }

        if (event->hasStudentRSVP(currentUser->getUserID())) {
            cout << "  [ERROR] You have already RSVP'd to this event." << endl;
            pause();
            return;
        }

        if (event->getConfirmedCount() >= event->getCapacity()) {
            cout << "  [ERROR] Event is at full capacity." << endl;
            pause();
            return;
        }

        // Show event info
        cout << "\n  Event: " << event->getTitle() << endl;
        cout << "  Date: " << event->getStartDate().toString() << endl;
        cout << "  Fee: $" << fixed << setprecision(2) << event->getFee() << endl;
        cout << "  Spots left: " << (event->getCapacity() - event->getConfirmedCount()) << endl;

        string confirm = getStringInput("\n  Confirm RSVP? (y/n): ");
        if (confirm == "y" || confirm == "Y") {
            RSVP newRSVP;
            newRSVP.studentID = currentUser->getUserID();
            newRSVP.eventID = eid;
            newRSVP.status = RSVP_CONFIRMED;
            newRSVP.attended = false;
            newRSVP.rsvpDate = Date::today();

            // Using pointer parameter
            if (event->addRSVP(&newRSVP)) {
                saveAllData();
                cout << "  [SUCCESS] RSVP confirmed for: " << event->getTitle() << endl;
            } else {
                cout << "  [ERROR] Could not complete RSVP." << endl;
            }
        } else {
            cout << "  [INFO] RSVP cancelled." << endl;
        }
        pause();
    }

    void cancelMyRSVP() {
        string eid = getStringInput("  Enter Event ID to cancel RSVP: ");
        Event* event = findEventByID(eid);

        if (!event) {
            cout << "  [ERROR] Event not found." << endl;
            pause();
            return;
        }

        if (event->cancelRSVP(currentUser->getUserID())) {
            saveAllData();
            cout << "  [SUCCESS] Your RSVP has been cancelled." << endl;
        } else {
            cout << "  [ERROR] You don't have an active RSVP for this event." << endl;
        }
        pause();
    }

    void viewMyRSVPs() {
        cout << "\n--- My RSVPs ---" << endl;
        cout << left
             << setw(12) << "Event ID"
             << setw(25) << "Title"
             << setw(12) << "Date"
             << setw(12) << "RSVP Status"
             << setw(10) << "Attended"
             << setw(10) << "Fee"
             << endl;
        cout << string(81, '-') << endl;

        int count = 0;
        for (const auto& e : events) {
            for (const auto& r : e.getRSVPList()) {
                if (r.studentID == currentUser->getUserID()) {
                    cout << left
                         << setw(12) << e.getEventID()
                         << setw(25) << e.getTitle().substr(0, 23)
                         << setw(12) << e.getStartDate().toString()
                         << setw(12) << rsvpStatusToString(r.status)
                         << setw(10) << (r.attended ? "Yes" : "No")
                         << "$" << fixed << setprecision(2) << e.getFee()
                         << endl;
                    count++;
                }
            }
        }

        if (count == 0) cout << "  No RSVPs found." << endl;
        pause();
    }

    void viewEventHistory() {
        cout << "\n--- Past Events (Chronological) ---" << endl;

        Date today = Date::today();
        vector<Event*> pastEvents;

        for (auto& e : events) {
            if (e.getStatus() == COMPLETED || e.getEndDate() < today) {
                pastEvents.push_back(&e);
            }
        }

        if (pastEvents.empty()) {
            cout << "  No past events found." << endl;
            pause();
            return;
        }

        // Sort in reverse chronological order (most recent first)
        sort(pastEvents.begin(), pastEvents.end(),
            [](const Event* a, const Event* b) {
                return b->getStartDate() < a->getStartDate();
            });

        printEventTableHeader();
        for (const auto& ep : pastEvents) ep->displayRow(venueMap);

        // Show my participation in past events
        cout << "\n--- My Participation in Past Events ---" << endl;
        cout << left
             << setw(12) << "Event ID"
             << setw(25) << "Title"
             << setw(12) << "Date"
             << setw(12) << "Attended"
             << endl;
        cout << string(61, '-') << endl;

        int participated = 0;
        for (const auto& ep : pastEvents) {
            for (const auto& r : ep->getRSVPList()) {
                if (r.studentID == currentUser->getUserID()) {
                    cout << left
                         << setw(12) << ep->getEventID()
                         << setw(25) << ep->getTitle().substr(0, 23)
                         << setw(12) << ep->getStartDate().toString()
                         << setw(12) << (r.attended ? "Yes" : "No")
                         << endl;
                    if (r.attended) participated++;
                }
            }
        }

        cout << "\n  Total Events Attended: " << participated << endl;
        pause();
    }

    // ==================== MAIN RUN LOOP ====================

public:
    // --- Constructor ---
    CampusEventManagementSystem() : currentUser(nullptr), running(true) {
        loadAllData();
        seedDefaultData();
    }

    // --- Destructor: ensure data is saved ---
    ~CampusEventManagementSystem() {
        if (currentUser) {
            saveAllData();
        }
    }

    // --- Main entry point ---
    void run() {
        while (running) {
            if (!login()) {
                string retry = getStringInput("  Try again? (y/n): ");
                if (retry != "y" && retry != "Y") {
                    running = false;
                }
                continue;
            }

            // Route to appropriate menu based on role (polymorphism via role enum)
            switch (currentUser->getRole()) {
                case ROLE_ADMIN:
                    adminMenu();
                    break;
                case ROLE_FACULTY:
                    facultyMenu();
                    break;
                case ROLE_STUDENT:
                    studentMenu();
                    break;
            }
        }

        cout << "\n" << LINE_SEP << endl;
        cout << "        Thank you for using the Campus Event Management System!" << endl;
        cout << LINE_SEP << endl;
    }
};

// ======================== MAIN FUNCTION ========================

int main() {
    // Set console output formatting
    cout << fixed << setprecision(2);

    CampusEventManagementSystem system;
    system.run();

    return 0;
}
