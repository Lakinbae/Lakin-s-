#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <map>
using namespace std;

// ============= USER DEFINED DATATYPES =============

enum class Role { ADMIN, STUDENT, FACULTY };
enum class EventType { ACADEMIC, SPORTS, CULTURAL, SEMINAR, WORKSHOP, OTHER };
enum class EventStatus { UPCOMING, ONGOING, COMPLETED, CANCELLED };

struct User {
    string username, password, name, id;
    Role role;
};

struct Venue {
    int id, capacity;
    string name, building;
    bool available = true;
};

struct Attendee {
    string userId, userName;
    bool verified = false;
    time_t registeredAt;
};

struct Event {
    int id;
    string name, description;
    EventType type;
    time_t dateTime;
    int venueId;
    string organizerId;
    int maxCapacity;
    vector<Attendee> attendees;
    double registrationFee;
    double budget;
    double expenses;
    EventStatus status;
    
    Event() {
        expenses = 0;
        id = 0;
        maxCapacity = 0;
        registrationFee = 0;
        budget = 0;
    }
};

// ============= GLOBAL HELPER FUNCTIONS =============

time_t stringToTime(const string& dateStr) {
    tm tm = {};
    stringstream ss(dateStr);
    ss >> tm.tm_mon >> tm.tm_mday >> tm.tm_year;
    tm.tm_mon -= 1;
    tm.tm_year -= 1900;
    return mktime(&tm);
}

string timeToString(time_t t) {
    tm* tm_ptr = localtime(&t);
    stringstream ss;
    ss << put_time(tm_ptr, "%Y-%m-%d %H:%M");
    return ss.str();
}

string getCurrentDate() {
    time_t now = time(0);
    return timeToString(now);
}

bool isPastEvent(time_t eventDate) {
    return eventDate < time(0);
}

EventType stringToEventType(const string& type) {
    if (type == "ACADEMIC") return EventType::ACADEMIC;
    if (type == "SPORTS") return EventType::SPORTS;
    if (type == "CULTURAL") return EventType::CULTURAL;
    if (type == "SEMINAR") return EventType::SEMINAR;
    if (type == "WORKSHOP") return EventType::WORKSHOP;
    return EventType::OTHER;
}

string eventTypeToString(EventType type) {
    switch(type) {
        case EventType::ACADEMIC: return "Academic";
        case EventType::SPORTS: return "Sports";
        case EventType::CULTURAL: return "Cultural";
        case EventType::SEMINAR: return "Seminar";
        case EventType::WORKSHOP: return "Workshop";
        default: return "Other";
    }
}

// ============= MAIN SYSTEM CLASS =============

class CampusEventSystem {
private:
    vector<User> users;
    vector<Event> events;
    vector<Venue> venues;
    User* currentUser = nullptr;
    int nextEventId = 1;
    int nextVenueId = 1;

    // Clear input buffer
    void clearInput() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // File Operations
    void saveToFile() {
        ofstream out("campus_events.txt");
        if (!out.is_open()) return;

        out << "=== USERS ===\n";
        for (const auto& u : users) {
            out << u.id << "|" << u.username << "|" << u.password << "|" 
                << u.name << "|" << static_cast<int>(u.role) << "\n";
        }

        out << "=== VENUES ===\n";
        for (const auto& v : venues) {
            out << v.id << "|" << v.name << "|" << v.building << "|" 
                << v.capacity << "|" << v.available << "\n";
        }

        out << "=== EVENTS ===\n";
        for (const auto& e : events) {
            out << e.id << "|" << e.name << "|" << e.description << "|"
                << static_cast<int>(e.type) << "|" << e.dateTime << "|"
                << e.venueId << "|" << e.organizerId << "|" << e.maxCapacity << "|"
                << e.registrationFee << "|" << e.budget << "|" << e.expenses << "|"
                << static_cast<int>(e.status) << "|";
            for (const auto& a : e.attendees) {
                out << a.userId << "," << a.userName << "," << a.verified << ";";
            }
            out << "\n";
        }
        out.close();
    }

    void loadFromFile() {
        ifstream in("campus_events.txt");
        if (!in.is_open()) return;

        string line;
        string section;

        while (getline(in, line)) {
            if (line == "=== USERS ===") { section = "USERS"; continue; }
            if (line == "=== VENUES ===") { section = "VENUES"; continue; }
            if (line == "=== EVENTS ===") { section = "EVENTS"; continue; }

            if (section == "USERS" && !line.empty()) {
                stringstream ss(line);
                User u;
                string roleStr;
                getline(ss, u.id, '|');
                getline(ss, u.username, '|');
                getline(ss, u.password, '|');
                getline(ss, u.name, '|');
                getline(ss, roleStr, '|');
                u.role = static_cast<Role>(stoi(roleStr));
                users.push_back(u);
            }

            if (section == "VENUES" && !line.empty()) {
                stringstream ss(line);
                Venue v;
                string availStr;
                getline(ss, v.name, '|');
                getline(ss, v.building, '|');
                getline(ss, v.name, '|');
                ss >> v.capacity;
                nextVenueId = max(nextVenueId, v.id + 1);
            }

            if (section == "EVENTS" && !line.empty()) {
                stringstream ss(line);
                Event e;
                string attendeesStr, typeStr, statusStr;
                string tempId;

                getline(ss, tempId, '|');
                e.id = stoi(tempId);
                getline(ss, e.name, '|');
                getline(ss, e.description, '|');
                getline(ss, typeStr, '|');
                e.type = static_cast<EventType>(stoi(typeStr));

                string dateStr;
                getline(ss, dateStr, '|');
                e.dateTime = stoll(dateStr);

                getline(ss, tempId, '|');
                e.venueId = stoi(tempId);
                getline(ss, e.organizerId, '|');
                getline(ss, tempId, '|');
                e.maxCapacity = stoi(tempId);
                getline(ss, tempId, '|');
                e.registrationFee = stod(tempId);
                getline(ss, tempId, '|');
                e.budget = stod(tempId);
                getline(ss, tempId, '|');
                e.expenses = stod(tempId);
                getline(ss, statusStr, '|');
                e.status = static_cast<EventStatus>(stoi(statusStr));
                getline(ss, attendeesStr, '|');

                if (!attendeesStr.empty()) {
                    stringstream attStream(attendeesStr);
                    string att;
                    while (getline(attStream, att, ';')) {
                        if (att.empty()) continue;
                        stringstream aStream(att);
                        Attendee a;
                        string verifiedStr;
                        getline(aStream, a.userId, ',');
                        getline(aStream, a.userName, ',');
                        getline(aStream, verifiedStr, ',');
                        a.verified = (verifiedStr == "1");
                        e.attendees.push_back(a);
                    }
                }
                events.push_back(e);
                nextEventId = max(nextEventId, e.id + 1);
            }
        }
        in.close();
    }

    // Utility Methods
    Venue* findVenue(int id) {
        for (auto& v : venues) {
            if (v.id == id) return &v;
        }
        return nullptr;
    }

    Event* findEvent(int id) {
        for (auto& e : events) {
            if (e.id == id) return &e;
        }
        return nullptr;
    }

    bool isUserRegistered(int eventId, const string& userId) {
        Event* e = findEvent(eventId);
        if (!e) return false;
        for (const auto& a : e->attendees) {
            if (a.userId == userId) return true;
        }
        return false;
    }

    void displayDivider() {
        cout << string(50, '=') << "\n";
    }

    // Search and Filter Methods
    vector<Event*> searchByDate(const string& dateStr) {
        vector<Event*> results;
        time_t searchDate = stringToTime(dateStr);
        tm* searchTm = localtime(&searchDate);

        for (auto& e : events) {
            tm* eventTm = localtime(&e.dateTime);
            if (eventTm->tm_year == searchTm->tm_year &&
                eventTm->tm_mon == searchTm->tm_mon &&
                eventTm->tm_mday == searchTm->tm_mday) {
                results.push_back(&e);
            }
        }
        return results;
    }

    vector<Event*> searchByType(EventType type) {
        vector<Event*> results;
        for (auto& e : events) {
            if (e.type == type) results.push_back(&e);
        }
        return results;
    }

    vector<Event*> searchByVenue(int venueId) {
        vector<Event*> results;
        for (auto& e : events) {
            if (e.venueId == venueId) results.push_back(&e);
        }
        return results;
    }

    vector<Event*> getUpcomingEvents() {
        vector<Event*> results;
        for (auto& e : events) {
            if (!isPastEvent(e.dateTime) && e.status != EventStatus::CANCELLED) {
                results.push_back(&e);
            }
        }
        sort(results.begin(), results.end(), 
            [](Event* a, Event* b) { return a->dateTime < b->dateTime; });
        return results;
    }

    vector<Event*> getPastEvents() {
        vector<Event*> results;
        for (auto& e : events) {
            if (isPastEvent(e.dateTime)) {
                results.push_back(&e);
            }
        }
        sort(results.begin(), results.end(), 
            [](Event* a, Event* b) { return a->dateTime > b->dateTime; });
        return results;
    }

    // ============= MENU FUNCTIONS =============

    void adminMenu() {
        int choice;
        do {
            displayDivider();
            cout << "       ADMIN MENU\n";
            displayDivider();
            cout << "1. Create Event\n2. Manage Events\n3. Manage Venues\n";
            cout << "4. View All Users\n5. Budget Overview\n";
            cout << "6. Back to Main\n";
            cout << "Enter choice: ";
            cin >> choice;

            switch(choice) {
                case 1: createEvent(); break;
                case 2: manageEvents(); break;
                case 3: manageVenues(); break;
                case 4: viewAllUsers(); break;
                case 5: budgetOverview(); break;
            }
        } while (choice != 6);
    }

    void userMenu() {
        int choice;
        do {
            displayDivider();
            cout << "       EVENT MENU\n";
            displayDivider();
            cout << "1. Browse Events\n2. RSVP for Event\n3. My Registrations\n";
            cout << "4. Search Events\n5. View History\n";
            cout << "6. Back to Main\n";
            cout << "Enter choice: ";
            cin >> choice;

            switch(choice) {
                case 1: browseEvents(); break;
                case 2: rsvpEvent(); break;
                case 3: myRegistrations(); break;
                case 4: searchEvents(); break;
                case 5: viewEventHistory(); break;
            }
        } while (choice != 6);
    }

    // ============= CORE FUNCTIONS =============

    void createEvent() {
        Event e;
        e.id = nextEventId++;
        clearInput();

        cout << "\n--- Create New Event ---\n";
        cout << "Event Name: ";
        getline(cin, e.name);
        cout << "Description: ";
        getline(cin, e.description);

        cout << "\nEvent Types:\n";
        cout << "0-ACADEMIC 1-SPORTS 2-CULTURAL 3-SEMINAR 4-WORKSHOP 5-OTHER\n";
        cout << "Select type: ";
        int typeChoice;
        cin >> typeChoice;
        e.type = static_cast<EventType>(typeChoice);

        string dateStr;
        clearInput();
        cout << "Date (MM DD YYYY): ";
        getline(cin, dateStr);
        e.dateTime = stringToTime(dateStr);

        cout << "\nAvailable Venues:\n";
        for (const auto& v : venues) {
            cout << v.id << ". " << v.name << " (" << v.building 
                 << ") - Capacity: " << v.capacity << "\n";
        }
        cout << "Select Venue ID: ";
        cin >> e.venueId;

        cout << "Max Capacity: ";
        cin >> e.maxCapacity;
        cout << "Registration Fee: $";
        cin >> e.registrationFee;
        cout << "Budget: $";
        cin >> e.budget;

        e.organizerId = currentUser->id;
        e.status = EventStatus::UPCOMING;
        e.expenses = 0;

        events.push_back(e);
        saveToFile();
        cout << "\n✓ Event created successfully! ID: " << e.id << "\n";
    }

    void manageEvents() {
        if (events.empty()) {
            cout << "\nNo events found.\n";
            return;
        }

        cout << "\n--- Manage Events ---\n";
        for (const auto& e : events) {
            cout << "\n[" << e.id << "] " << e.name << "\n";
            cout << "    Type: " << eventTypeToString(e.type)
                 << " | Date: " << timeToString(e.dateTime) << "\n";
            cout << "    Venue ID: " << e.venueId
                 << " | Capacity: " << e.maxCapacity << "/" 
                 << e.attendees.size() << " registered\n";
        }

        cout << "\nEnter Event ID to manage (0 to exit): ";
        int eventId;
        cin >> eventId;
        if (eventId == 0) return;

        Event* e = findEvent(eventId);
        if (!e) {
            cout << "Event not found.\n";
            return;
        }

        int choice;
        cout << "\n1. Edit Event\n2. Swap Venue\n3. Add Expense\n";
        cout << "4. Cancel Event\n5. View Attendees\n";
        cout << "Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: editEvent(e); break;
            case 2: swapVenue(e); break;
            case 3: addExpense(e); break;
            case 4: e->status = EventStatus::CANCELLED; saveToFile(); 
                    cout << "Event cancelled.\n"; break;
            case 5: viewAttendees(e); break;
        }
    }

    void editEvent(Event* e) {
        clearInput();
        cout << "\n--- Edit Event ---\n";
        cout << "Current: " << e->name << "\nNew Name (Enter to skip): ";
        string input;
        getline(cin, input);
        if (!input.empty()) e->name = input;

        cout << "Current: " << e->description << "\nNew Description: ";
        getline(cin, input);
        if (!input.empty()) e->description = input;

        saveToFile();
        cout << "\n✓ Event updated.\n";
    }

    void swapVenue(Event* e) {
        cout << "\n--- Swap Venue ---\n";
        cout << "Current Venue ID: " << e->venueId << "\n";
        cout << "Available Venues:\n";

        for (const auto& v : venues) {
            if (v.capacity >= e->maxCapacity) {
                cout << v.id << ". " << v.name << " (Capacity: " 
                     << v.capacity << ")\n";
            }
        }

        cout << "New Venue ID: ";
        int newVenueId;
        cin >> newVenueId;

        Venue* newVenue = findVenue(newVenueId);
        if (newVenue && newVenue->capacity >= e->maxCapacity) {
            e->venueId = newVenueId;
            saveToFile();
            cout << "\n✓ Venue swapped successfully.\n";
        } else {
            cout << "Invalid venue or insufficient capacity.\n";
        }
    }

    void addExpense(Event* e) {
        cout << "\nCurrent Budget: $" << e->budget 
             << " | Spent: $" << e->expenses << "\n";
        cout << "Add Expense: $";
        double expense;
        cin >> expense;
        
        e->expenses += expense;
        if (e->expenses > e->budget) {
            cout << "⚠ Warning: Expenses exceed budget!\n";
        }
        saveToFile();
        cout << "\n✓ Expense recorded.\n";
    }

    void viewAttendees(Event* e) {
        cout << "\n--- Attendees for " << e->name << " ---\n";
        if (e->attendees.empty()) {
            cout << "No attendees yet.\n";
            return;
        }

        for (size_t i = 0; i < e->attendees.size(); i++) {
            cout << (i + 1) << ". " << e->attendees[i].userName 
                 << " (ID: " << e->attendees[i].userId << ")";
            if (e->attendees[i].verified) {
                cout << " [✓ Verified]";
            } else {
                cout << " [Not Verified]";
            }
            cout << "\n";
        }
    }

    void manageVenues() {
        int choice;
        do {
            cout << "\n--- Venue Management ---\n";
            cout << "1. Add Venue\n2. List Venues\n3. Remove Venue\n";
            cout << "4. Back\nChoice: ";
            cin >> choice;

            if (choice == 1) addVenue();
            else if (choice == 2) listVenues();
            else if (choice == 3) removeVenue();
        } while (choice != 4);
    }

    void addVenue() {
        Venue v;
        v.id = nextVenueId++;
        clearInput();

        cout << "Venue Name: ";
        getline(cin, v.name);
        cout << "Building: ";
        getline(cin, v.building);
        cout << "Capacity: ";
        cin >> v.capacity;
        v.available = true;

        venues.push_back(v);
        saveToFile();
        cout << "\n✓ Venue added.\n";
    }

    void listVenues() {
        cout << "\n--- All Venues ---\n";
        for (const auto& v : venues) {
            cout << v.id << ". " << v.name << " - " << v.building
                 << " (Cap: " << v.capacity << ") "
                 << (v.available ? "[Available]" : "[Booked]") << "\n";
        }
    }

    void removeVenue() {
        listVenues();
        cout << "Enter Venue ID to remove: ";
        int id;
        cin >> id;

        auto it = remove_if(venues.begin(), venues.end(), 
            [id](Venue& v) { return v.id == id; });
        if (it != venues.end()) {
            venues.erase(it);
            saveToFile();
            cout << "✓ Venue removed.\n";
        } else {
            cout << "Venue not found.\n";
        }
    }

    void viewAllUsers() {
        cout << "\n--- All Users ---\n";
        for (const auto& u : users) {
            cout << "ID: " << u.id << " | " << u.name 
                 << " | " << (u.role == Role::ADMIN ? "Admin" : 
                            u.role == Role::FACULTY ? "Faculty" : "Student") << "\n";
        }
    }

    void budgetOverview() {
        cout << "\n--- Budget Overview ---\n";
        double totalBudget = 0, totalExpenses = 0, totalRevenue = 0;

        for (const auto& e : events) {
            totalBudget += e.budget;
            totalExpenses += e.expenses;
            totalRevenue += e.registrationFee * e.attendees.size();
        }

        cout << fixed << setprecision(2);
        cout << "Total Budget Allocated: $" << totalBudget << "\n";
        cout << "Total Expenses: $" << totalExpenses << "\n";
        cout << "Total Revenue: $" << totalRevenue << "\n";
        cout << "Net Balance: $" << (totalRevenue - totalExpenses) << "\n";
        cout << "Remaining Budget: $" << (totalBudget - totalExpenses) << "\n";
    }

    // ============= USER FUNCTIONS =============

    void browseEvents() {
        cout << "\n--- Upcoming Events ---\n";
        vector<Event*> upcoming = getUpcomingEvents();

        if (upcoming.empty()) {
            cout << "No upcoming events.\n";
            return;
        }

        for (Event* e : upcoming) {
            Venue* v = findVenue(e->venueId);
            cout << "\n[" << e->id << "] " << e->name << "\n";
            cout << "    Type: " << eventTypeToString(e->type) << "\n";
            cout << "    Date: " << timeToString(e->dateTime) << "\n";
            cout << "    Venue: " << (v ? v->name : "Unknown") << "\n";
            cout << "    Capacity: " << e->attendees.size() << "/" << e->maxCapacity << "\n";
            cout << "    Fee: $" << e->registrationFee << "\n";
        }
    }

    void rsvpEvent() {
        browseEvents();
        cout << "\nEnter Event ID to RSVP (0 to cancel): ";
        int eventId;
        cin >> eventId;
        if (eventId == 0) return;

        Event* e = findEvent(eventId);
        if (!e) {
            cout << "Event not found.\n";
            return;
        }

        if (isPastEvent(e->dateTime)) {
            cout << "Cannot RSVP to past events.\n";
            return;
        }

        if (e->status == EventStatus::CANCELLED) {
            cout << "This event has been cancelled.\n";
            return;
        }

        if (isUserRegistered(eventId, currentUser->id)) {
            cout << "You are already registered.\n";
            return;
        }

        if ((int)e->attendees.size() >= e->maxCapacity) {
            cout << "Event is full.\n";
            return;
        }

        Attendee a;
        a.userId = currentUser->id;
        a.userName = currentUser->name;
        a.verified = false;
        a.registeredAt = time(0);

        e->attendees.push_back(a);
        saveToFile();
        cout << "\n✓ RSVP successful! You are registered.\n";
    }

    void myRegistrations() {
        cout << "\n--- My Registrations ---\n";
        bool found = false;

        for (const auto& e : events) {
            for (const auto& a : e.attendees) {
                if (a.userId == currentUser->id) {
                    found = true;
                    cout << "\n[" << e.id << "] " << e.name << "\n";
                    cout << "    Date: " << timeToString(e.dateTime) << "\n";
                    cout << "    Status: " << (a.verified ? "✓ Verified" : "Pending") << "\n";
                    cout << "    Venue: " << (findVenue(e.venueId) ? 
                                   findVenue(e.venueId)->name : "Unknown") << "\n";
                }
            }
        }

        if (!found) {
            cout << "No registrations found.\n";
        }
    }

    void searchEvents() {
        cout << "\n--- Search Events ---\n";
        cout << "1. By Date\n2. By Type\n3. By Venue\n";
        cout << "Choice: ";
        int choice;
        cin >> choice;

        vector<Event*> results;

        switch(choice) {
            case 1: {
                clearInput();
                cout << "Enter date (MM DD YYYY): ";
                string dateStr;
                getline(cin, dateStr);
                results = searchByDate(dateStr);
                break;
            }
            case 2: {
                cout << "0-ACADEMIC 1-SPORTS 2-CULTURAL 3-SEMINAR 4-WORKSHOP\n";
                int typeChoice;
                cin >> typeChoice;
                results = searchByType(static_cast<EventType>(typeChoice));
                break;
            }
            case 3: {
                listVenues();
                cout << "Enter Venue ID: ";
                int venueId;
                cin >> venueId;
                results = searchByVenue(venueId);
                break;
            }
        }

        if (results.empty()) {
            cout << "No events found.\n";
            return;
        }

        cout << "\n--- Search Results ---\n";
        for (Event* e : results) {
            cout << "[" << e->id << "] " << e->name << " | "
                 << timeToString(e->dateTime) << " | "
                 << eventTypeToString(e->type) << "\n";
        }
    }

    void viewEventHistory() {
        int choice;
        cout << "\n--- Event History ---\n";
        cout << "1. Upcoming Events\n2. Past Events\nChoice: ";
        cin >> choice;

        vector<Event*> history;
        if (choice == 1) {
            history = getUpcomingEvents();
            cout << "\n--- Upcoming Events ---\n";
        } else {
            history = getPastEvents();
            cout << "\n--- Past Events ---\n";
        }

        if (history.empty()) {
            cout << "No events.\n";
            return;
        }

        for (Event* e : history) {
            cout << "\n[" << e->id << "] " << e->name << "\n";
            cout << "    Date: " << timeToString(e->dateTime) << "\n";
            cout << "    Attendees: " << e->attendees.size() << "/" << e->maxCapacity << "\n";
            cout << "    Revenue: $" << fixed << setprecision(2) 
                 << (e->registrationFee * e->attendees.size()) << "\n";
        }
    }

    // Attendance Verification (Admin)
    void verifyAttendance() {
        cout << "\n--- Attendance Verification ---\n";
        cout << "Enter Event ID: ";
        int eventId;
        cin >> eventId;

        Event* e = findEvent(eventId);
        if (!e) {
            cout << "Event not found.\n";
            return;
        }

        cout << "Attendees:\n";
        for (size_t i = 0; i < e->attendees.size(); i++) {
            cout << (i + 1) << ". " << e->attendees[i].userName 
                 << " - " << (e->attendees[i].verified ? "✓" : "[ ]") << "\n";
        }

        cout << "\nEnter attendee number to verify (0 to finish): ";
        int num;
        while (cin >> num && num > 0) {
            if (num <= (int)e->attendees.size()) {
                e->attendees[num - 1].verified = true;
                cout << "✓ Verified: " << e->attendees[num - 1].userName << "\n";
            }
            cout << "Enter number (0 to finish): ";
        }
        saveToFile();
    }

public:
    // Constructor
    CampusEventSystem() {
        loadFromFile();
        // Add default admin if none exists
        if (users.empty()) {
            users.push_back({"admin", "admin123", "Administrator", "A001", Role::ADMIN});
            users.push_back({"faculty1", "pass123", "Dr. Smith", "F001", Role::FACULTY});
            users.push_back({"student1", "pass123", "John Doe", "S001", Role::STUDENT});
            saveToFile();
        }
        if (venues.empty()) {
            venues.push_back({nextVenueId++, "Main Auditorium", "Building A", 500, true});
            venues.push_back({nextVenueId++, "Seminar Hall", "Building B", 100, true});
            venues.push_back({nextVenueId++, "Sports Complex", "Building C", 200, true});
            saveToFile();
        }
    }

    // Authentication
    bool login() {
        clearInput();
        string username, password;
        cout << "\n=== Campus Event Management System ===\n";
        cout << "Username: ";
        getline(cin, username);
        cout << "Password: ";
        getline(cin, password);

        for (auto& u : users) {
            if (u.username == username && u.password == password) {
                currentUser = &u;
                cout << "\n✓ Welcome, " << u.name << "!\n";
                return true;
            }
        }
        cout << "\n✗ Invalid credentials.\n";
        return false;
    }

    void logout() {
        currentUser = nullptr;
        saveToFile();
        cout << "Logged out successfully.\n";
    }

    // Main Run
    void run() {
        if (!login()) return;

        if (currentUser->role == Role::ADMIN) {
            int choice;
            do {
                displayDivider();
                cout << "       MAIN MENU\n";
                displayDivider();
                cout << "1. Event Management\n2. Venue Management\n";
                cout << "3. Attendance Verification\n4. Reports\n";
                cout << "5. Logout\n";
                cout << "Choice: ";
                cin >> choice;

                switch(choice) {
                    case 1: adminMenu(); break;
                    case 2: manageVenues(); break;
                    case 3: verifyAttendance(); break;
                    case 4: budgetOverview(); break;
                    case 5: logout(); choice = 0; break;
                }
            } while (choice != 0);
        } else {
            userMenu();
            logout();
        }
    }
};

// ============= MAIN =============

int main() {
    CampusEventSystem system;
    system.run();
    return 0;
}
