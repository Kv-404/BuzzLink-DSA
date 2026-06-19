/*
 * ============================================================================
 *  BuzzLink — Micro-Blogging Social Media Backend (C++17)
 * ============================================================================
 *  A complete OOP-based system managing a scalable social networking platform.
 *  Demonstrates Trie, Stack, Queue, Hash Map, Graph (BFS/DFS), Sorting
 *  Algorithms (Quick/Merge/STL), and Greedy Optimization.
 *
 *  Compile:  g++ -std=c++17 -O2 -o buzzlink buzzlink.cpp
 *  Run:      ./buzzlink
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
// ANSI colour helpers (for terminal UI)
// ─────────────────────────────────────────────────────────────────────────────
namespace clr {
    const string RESET  = "\033[0m";
    const string BOLD   = "\033[1m";
    const string DIM    = "\033[2m";
    const string CYAN   = "\033[36m";
    const string GREEN  = "\033[32m";
    const string YELLOW = "\033[33m";
    const string RED    = "\033[31m";
    const string MAGENTA= "\033[35m";
    const string BLUE   = "\033[34m";
    const string WHITE  = "\033[97m";
}

void printSeparator() {
    cout << clr::DIM << string(64, '-') << clr::RESET << "\n";
}

void printHeader(const string& title) {
    cout << "\n";
    printSeparator();
    cout << clr::BOLD << clr::CYAN << "  " << title << clr::RESET << "\n";
    printSeparator();
}

// ─────────────────────────────────────────────────────────────────────────────
// BASE ENTITIES
// ─────────────────────────────────────────────────────────────────────────────

class User {
public:
    int userID;
    string username;
    int followers;
    int engagementScore;
    string privacyStatus; // "PUBLIC" or "PRIVATE"

    User() : userID(-1), followers(0), engagementScore(0), privacyStatus("PUBLIC") {}

    User(int id, string uname, int f, int es, string priv = "PUBLIC")
        : userID(id), username(uname), followers(f), engagementScore(es), privacyStatus(priv) {}

    void print() const {
        cout << clr::WHITE << "  ID: " << clr::YELLOW << userID << clr::RESET
             << "  |  @" << clr::GREEN << username << clr::RESET
             << "  |  " << clr::MAGENTA << followers << " followers" << clr::RESET
             << "  |  engagement " << clr::BLUE << engagementScore << clr::RESET
             << "  |  " << privacyStatus << "\n";
    }
};

struct FriendRequest {
    int fromID;
    int toID;
    string message;
};

// Forward declaration for lookup dependency
class UserManager;

// ─────────────────────────────────────────────────────────────────────────────
// 1. TRIE MANAGER (User Directory)
// ─────────────────────────────────────────────────────────────────────────────
// Solves: O(L) fast prefix search for autocompleting handles.

struct TrieNode {
    unordered_map<char, TrieNode*> children;
    bool isEnd = false;
    int userID = -1;
};

class TrieManager {
private:
    TrieNode* root;

    void collect(TrieNode* node, string& current, vector<pair<string,int>>& results) const {
        if (node->isEnd) results.emplace_back(current, node->userID);
        for (auto& pair : node->children) {
            current.push_back(pair.first);
            collect(pair.second, current, results);
            current.pop_back();
        }
    }

    void destroy(TrieNode* n) {
        for (auto& pair : n->children) destroy(pair.second);
        delete n;
    }

public:
    TrieManager() : root(new TrieNode()) {}
    ~TrieManager() { destroy(root); }

    void insert(const string& username, int userID) {
        TrieNode* cur = root;
        for (char c : username) {
            if (!cur->children.count(c)) cur->children[c] = new TrieNode();
            cur = cur->children[c];
        }
        cur->isEnd = true;
        cur->userID = userID;
    }

    vector<pair<string,int>> searchPrefix(const string& prefix) const {
        TrieNode* cur = root;
        for (char c : prefix) {
            if (!cur->children.count(c)) return {};
            cur = cur->children[c];
        }
        vector<pair<string,int>> results;
        string buf = prefix;
        collect(cur, buf, results);
        return results;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// 2. PRIVACY MANAGER (Stack)
// ─────────────────────────────────────────────────────────────────────────────
// Solves: O(1) undo history for privacy settings.

class PrivacyManager {
private:
    unordered_map<int, stack<string>> history;

public:
    void changePrivacy(int userID, const string& setting) {
        history[userID].push(setting);
    }

    bool undoChange(int userID, string& revertedTo) {
        auto it = history.find(userID);
        if (it == history.end() || it->second.size() <= 1) return false;
        it->second.pop(); // remove current
        revertedTo = it->second.top(); // reveal previous
        return true;
    }

    string getCurrent(int userID) const {
        auto it = history.find(userID);
        if (it == history.end() || it->second.empty()) return "PUBLIC";
        return it->second.top();
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// 3. REQUEST MANAGER (Queue)
// ─────────────────────────────────────────────────────────────────────────────
// Solves: O(1) fairness in processing friend requests via FIFO Queue.

class RequestManager {
private:
    queue<FriendRequest> q;

public:
    void sendRequest(const FriendRequest& req) {
        q.push(req);
    }

    bool processNext(FriendRequest& out) {
        if (q.empty()) return false;
        out = q.front();
        q.pop();
        return true;
    }

    void viewPending() const {
        cout << "  Pending requests: " << q.size() << "\n";
        if (!q.empty()) {
            cout << "  Next up: User " << q.front().fromID << " -> User " << q.front().toID << "\n";
        }
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// 4. USER MANAGER (Hash Map)
// ─────────────────────────────────────────────────────────────────────────────
// Solves: O(1) average lookup/update/delete by userID.

class UserManager {
private:
    unordered_map<int, User> users;

public:
    void addUser(const User& u) {
        users[u.userID] = u;
    }

    bool findUser(int userID, User& out) const {
        auto it = users.find(userID);
        if (it == users.end()) return false;
        out = it->second;
        return true;
    }

    void updateUser(int userID, const User& u) {
        if (users.count(userID)) users[userID] = u;
    }

    const unordered_map<int, User>& getAllUsers() const {
        return users;
    }

    bool exists(int userID) const { return users.count(userID); }
};

// ─────────────────────────────────────────────────────────────────────────────
// 5. GRAPH MANAGER (Friend Network)
// ─────────────────────────────────────────────────────────────────────────────
// Solves: Adjacency list representation O(1) insertions, BFS/DFS O(V+E).

class GraphManager {
private:
    unordered_map<int, unordered_set<int>> adj;

public:
    void registerUser(int userID) { adj[userID]; }

    void addFriendship(int u, int v) {
        adj[u].insert(v);
        adj[v].insert(u);
    }

    void removeFriendship(int u, int v) {
        if(adj.count(u)) adj[u].erase(v);
        if(adj.count(v)) adj[v].erase(u);
    }

    const unordered_set<int>& getFriends(int u) const {
        static const unordered_set<int> empty;
        auto it = adj.find(u);
        return it != adj.end() ? it->second : empty;
    }

    bool areFriends(int u, int v) const {
        auto it = adj.find(u);
        return it != adj.end() && it->second.count(v);
    }

    vector<int> getMutualFriends(int u, int v) const {
        vector<int> mutual;
        const auto& fu = getFriends(u);
        const auto& fv = getFriends(v);
        const auto& smaller = fu.size() < fv.size() ? fu : fv;
        const auto& larger  = fu.size() < fv.size() ? fv : fu;
        for (int id : smaller) if (larger.count(id)) mutual.push_back(id);
        return mutual;
    }


    // BFS: Fastest Viral Spread Path
    void findViralSpreadPath(int src, int dst, const UserManager& uMgr) const {
        if (src == dst) { cout << "  Source and destination are the same.\n"; return; }
        if (!adj.count(src) || !adj.count(dst)) { cout << "  Invalid users.\n"; return; }

        unordered_map<int,int> parent;
        queue<int> q;
        q.push(src);
        parent[src] = -1;

        while (!q.empty()) {
            int cur = q.front(); q.pop();
            if (cur == dst) {
                vector<int> path;
                for (int n = dst; n != -1; n = parent[n]) path.push_back(n);
                reverse(path.begin(), path.end());
                cout << clr::GREEN << "  Viral path found in " << (path.size() - 1) << " hops!\n" << clr::RESET << "  ";
                for (size_t i=0; i<path.size(); ++i) {
                    User u; uMgr.findUser(path[i], u);
                    cout << "@" << u.username << (i == path.size()-1 ? "" : " -> ");
                }
                cout << "\n";
                return;
            }
            for (int nb : getFriends(cur)) {
                if (!parent.count(nb)) {
                    parent[nb] = cur;
                    q.push(nb);
                }
            }
        }
        cout << clr::RED << "  No path exists (disconnected).\n" << clr::RESET;
    }

    // DFS Helper
    void dfsUtil(int v, unordered_set<int>& visited, vector<int>& cluster) const {
        visited.insert(v);
        cluster.push_back(v);
        for (int nb : getFriends(v)) {
            if (!visited.count(nb)) {
                dfsUtil(nb, visited, cluster);
            }
        }
    }

    // DFS: Community Detection
    void detectCommunities(const UserManager& uMgr) const {
        unordered_set<int> visited;
        vector<vector<int>> allCommunities;
        
        for (const auto& pair : adj) {
            if (!visited.count(pair.first)) {
                vector<int> cluster;
                dfsUtil(pair.first, visited, cluster);
                allCommunities.push_back(cluster);
            }
        }
        
        cout << clr::GREEN << "\n  Total Communities Detected: " << allCommunities.size() << "\n" << clr::RESET;
        
        sort(allCommunities.begin(), allCommunities.end(), [](auto& a, auto& b){ return a.size() > b.size(); });
        
        cout << "  Displaying Top 3 Largest Communities:\n";
        for (size_t i = 0; i < min((size_t)3, allCommunities.size()); i++) {
            cout << clr::YELLOW << "  Community " << (i+1) << " (" << allCommunities[i].size() << " users): " << clr::RESET;
            for (size_t j = 0; j < min((size_t)10, allCommunities[i].size()); j++) {
                User u; uMgr.findUser(allCommunities[i][j], u);
                cout << "@" << u.username << " ";
            }
            if (allCommunities[i].size() > 10) cout << "...and " << (allCommunities[i].size() - 10) << " more";
            cout << "\n";
        }
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// 6. RANKING SYSTEM (Sorting Algorithms)
// ─────────────────────────────────────────────────────────────────────────────
// Solves: O(N log N) influence ranking using multiple sorting algorithms.

class RankingSystem {
private:
    double computeScore(const User& p) const {
        return p.followers * 0.7 + p.engagementScore * 0.3;
    }

    size_t partition(vector<User>& arr, size_t low, size_t high) {
        // Use middle element as pivot to avoid O(N^2) on sorted arrays
        size_t mid = low + (high - low) / 2;
        swap(arr[mid], arr[high]);
        
        double pivot = computeScore(arr[high]);
        size_t i = low;
        for (size_t j = low; j < high; j++) {
            if (computeScore(arr[j]) > pivot) { // Sort descending
                swap(arr[i], arr[j]);
                i++;
            }
        }
        swap(arr[i], arr[high]);
        return i;
    }

    void quickSortRecursive(vector<User>& arr, size_t low, size_t high) {
        if (low < high) {
            size_t pi = partition(arr, low, high);
            if (pi > 0) quickSortRecursive(arr, low, pi - 1);
            quickSortRecursive(arr, pi + 1, high);
        }
    }

    void merge(vector<User>& arr, vector<User>& temp, size_t l, size_t m, size_t r) {
        size_t i = l, j = m + 1, k = l;
        
        while (i <= m && j <= r) {
            if (computeScore(arr[i]) >= computeScore(arr[j])) {
                temp[k++] = std::move(arr[i++]);
            } else {
                temp[k++] = std::move(arr[j++]);
            }
        }
        
        while (i <= m) temp[k++] = std::move(arr[i++]);
        while (j <= r) temp[k++] = std::move(arr[j++]);
        
        for (size_t p = l; p <= r; p++) arr[p] = std::move(temp[p]);
    }

    void mergeSortRecursive(vector<User>& arr, vector<User>& temp, size_t l, size_t r) {
        if (l < r) {
            size_t m = l + (r - l) / 2;
            mergeSortRecursive(arr, temp, l, m);
            mergeSortRecursive(arr, temp, m + 1, r);
            merge(arr, temp, l, m, r);
        }
    }

public:
    void displayRankings(const UserManager& uMgr) {
        vector<User> usersList;
        for (auto& pair : uMgr.getAllUsers()) usersList.push_back(pair.second);
        if (usersList.empty()) return;

        // Perform Comparisons
        vector<User> qsList = usersList;
        vector<User> msList = usersList;
        vector<User> stlList = usersList;
        
        // Pre-allocate temp array for Merge Sort to prevent constant reallocation
        vector<User> msTemp(msList.size());

        auto start = chrono::high_resolution_clock::now();
        quickSortRecursive(qsList, 0, qsList.size() - 1);
        auto end = chrono::high_resolution_clock::now();
        auto qsTime = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        start = chrono::high_resolution_clock::now();
        mergeSortRecursive(msList, msTemp, 0, msList.size() - 1);
        end = chrono::high_resolution_clock::now();
        auto msTime = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        start = chrono::high_resolution_clock::now();
        sort(stlList.begin(), stlList.end(), [this](const User& a, const User& b) {
            return computeScore(a) > computeScore(b);
        });
        end = chrono::high_resolution_clock::now();
        auto stlTime = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        cout << clr::CYAN << "  Sorting Performance (Nanoseconds): " << clr::RESET << "\n"
             << "  Quick Sort: " << qsTime << " ns  |  Merge Sort: " << msTime << " ns  |  STL Sort: " << stlTime << " ns\n\n";

        cout << clr::GREEN << "  Top Influencers Leaderboard:\n" << clr::RESET;
        for(size_t i=0; i<min((size_t)10, stlList.size()); i++) {
            cout << "  " << clr::YELLOW << i+1 << ". " << clr::RESET 
                 << "@" << stlList[i].username << " (Score: " << computeScore(stlList[i]) << ")\n";
        }
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// 7. RECOMMENDATION ENGINE (Greedy Optimization)
// ─────────────────────────────────────────────────────────────────────────────
// Solves: O(V_friends * V_fof) greedy search for candidates maximizing mutual connections.

class RecommendationEngine {
public:
    void suggestFriends(int userID, const GraphManager& gMgr, const UserManager& uMgr) {
        if(!uMgr.exists(userID)) { cout << "  User not found.\n"; return; }

        const auto& myFriends = gMgr.getFriends(userID);
        unordered_map<int,int> mutualCount;

        for (int f : myFriends) {
            for (int fof : gMgr.getFriends(f)) {
                if (fof == userID || myFriends.count(fof)) continue;
                mutualCount[fof]++;
            }
        }

        vector<pair<int,int>> candidates(mutualCount.begin(), mutualCount.end());
        // Greedy approach: Pick candidates with highest mutual count
        sort(candidates.begin(), candidates.end(), [](auto& a, auto& b){ return a.second > b.second; });

        if (candidates.empty()) {
            cout << clr::YELLOW << "  No suggestions available.\n" << clr::RESET;
            return;
        }

        cout << clr::GREEN << "  Top Recommendations:\n" << clr::RESET;
        for (size_t i=0; i<min((size_t)5, candidates.size()); ++i) {
            User cand; uMgr.findUser(candidates[i].first, cand);
            cout << "  @" << cand.username << " (" << candidates[i].first << ") - Mutual Friends: " << candidates[i].second << "\n";
        }
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// APPLICATION CONTEXT
// ─────────────────────────────────────────────────────────────────────────────

class BuzzLinkApp {
private:
    TrieManager          trie;
    PrivacyManager       privacy;
    RequestManager       requests;
    UserManager          users;
    GraphManager         graph;
    RankingSystem        ranking;
    RecommendationEngine recommendations;

    void preloadData() {
        ifstream file("users.csv");
        if (!file.is_open()) {
            cout << clr::RED << "  Warning: Could not open users.csv. Loading fallback data.\n" << clr::RESET;
            User fallback(101, "fallback_user", 0, 0, "PUBLIC");
            users.addUser(fallback);
            trie.insert("fallback_user", 101);
            privacy.changePrivacy(101, "PUBLIC");
            graph.registerUser(101);
        } else {
            string line;
            while (getline(file, line)) {
                if (line.empty() || line[0] == '\r') continue;
                stringstream ss(line);
                string idStr, username, followersStr, engagementStr, privacyStatus;
                
                getline(ss, idStr, ',');
                getline(ss, username, ',');
                getline(ss, followersStr, ',');
                getline(ss, engagementStr, ',');
                getline(ss, privacyStatus, ',');

                if (!privacyStatus.empty() && privacyStatus.back() == '\r') {
                    privacyStatus.pop_back();
                }

                try {
                    int id = stoi(idStr);
                    int followers = stoi(followersStr);
                    int engagement = stoi(engagementStr);

                    User u(id, username, followers, engagement, privacyStatus);
                    users.addUser(u);
                    trie.insert(u.username, u.userID);
                    privacy.changePrivacy(u.userID, u.privacyStatus);
                    graph.registerUser(u.userID);
                } catch (...) {
                    // Ignore malformed lines silently
                }
            }
            file.close();
        }

        // Generate random realistic graph interconnections
        vector<int> userIDs;
        for (auto& pair : users.getAllUsers()) userIDs.push_back(pair.first);

        if (userIDs.size() > 1) {
            mt19937 rng(42); // fixed seed for reproducible graph
            uniform_int_distribution<size_t> dist(0, userIDs.size() - 1);
            
            // Aim for an average degree of ~6 (3000 edges for 1000 users)
            size_t numEdges = userIDs.size() * 3;
            for(size_t i = 0; i < numEdges; i++) {
                int u = userIDs[dist(rng)];
                int v = userIDs[dist(rng)];
                if (u != v) graph.addFriendship(u, v);
            }
        }

        // Add a few explicit requests for demonstration
        if (userIDs.size() >= 2) {
            requests.sendRequest({userIDs[0], userIDs[1], "Hey, let's connect!"});
            requests.sendRequest({userIDs[1], userIDs[0], "Hi!"});
        }
    }

public:
    BuzzLinkApp() { preloadData(); }

    void run() {
        bool running = true;
        while(running) {
            cout << "\n" << clr::BOLD << clr::CYAN << "  BUZZLINK SOCIAL PLATFORM DASHBOARD\n" << clr::RESET;
            printSeparator();
            cout << "  1. Add User                      9. Add Friendship\n"
                 << "  2. Search User by Prefix        10. Find Mutual Friends\n"
                 << "  3. Change Privacy Setting       11. Detect Communities\n"
                 << "  4. Undo Privacy Change          12. Find Shortest Path Between Users\n"
                 << "  5. Send Friend Request          13. Friend Recommendations\n"
                 << "  6. Process Friend Request       14. Exit\n"
                 << "  7. Find User by ID\n"
                 << "  8. Display Influencer Rankings\n";
            printSeparator();
            cout << "  Select option > ";
            
            int ch;
            if(!(cin >> ch)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }

            switch(ch) {
                case 1: {
                    printHeader("1. Add User");
                    int id, f, e; string name;
                    cout << "  UserID: "; cin >> id;
                    cout << "  Username: "; cin >> name;
                    cout << "  Followers: "; cin >> f;
                    cout << "  Engagement Score: "; cin >> e;
                    User u(id, name, f, e);
                    users.addUser(u);
                    trie.insert(name, id);
                    privacy.changePrivacy(id, "PUBLIC");
                    graph.registerUser(id);
                    cout << clr::GREEN << "  User added successfully.\n" << clr::RESET;
                    break;
                }
                case 2: {
                    printHeader("2. Search User by Prefix (Trie)");
                    cout << "  Prefix: "; string prefix; cin >> prefix;
                    auto res = trie.searchPrefix(prefix);
                    if (res.empty()) cout << clr::YELLOW << "  No matches found.\n" << clr::RESET;
                    else {
                        cout << clr::GREEN << "  Found " << res.size() << " match(es):\n" << clr::RESET;
                        for(size_t i = 0; i < min((size_t)15, res.size()); i++) {
                            cout << "  @" << res[i].first << " (ID: " << res[i].second << ")\n";
                        }
                        if (res.size() > 15) {
                            cout << clr::DIM << "  ...and " << (res.size() - 15) << " more. Please refine your search.\n" << clr::RESET;
                        }
                    }
                    break;
                }
                case 3: {
                    printHeader("3. Change Privacy Setting");
                    cout << "  UserID: "; int id; cin >> id;
                    cout << "  New Setting (PUBLIC/PRIVATE): "; string p; cin >> p;
                    if(users.exists(id)) {
                        privacy.changePrivacy(id, p);
                        User u; users.findUser(id, u);
                        u.privacyStatus = p;
                        users.updateUser(id, u);
                        cout << clr::GREEN << "  Privacy updated.\n" << clr::RESET;
                    } else cout << clr::RED << "  User not found.\n" << clr::RESET;
                    break;
                }
                case 4: {
                    printHeader("4. Undo Privacy Change (Stack)");
                    cout << "  UserID: "; int id; cin >> id;
                    string prev;
                    if(privacy.undoChange(id, prev)) {
                        User u; users.findUser(id, u);
                        u.privacyStatus = prev;
                        users.updateUser(id, u);
                        cout << clr::GREEN << "  Reverted back to: " << prev << "\n" << clr::RESET;
                    } else cout << clr::YELLOW << "  No history to undo.\n" << clr::RESET;
                    break;
                }
                case 5: {
                    printHeader("5. Send Friend Request");
                    int from, to; string msg;
                    cout << "  From ID: "; cin >> from;
                    cout << "  To ID: "; cin >> to;
                    cout << "  Message: "; cin >> ws; getline(cin, msg);
                    requests.sendRequest({from, to, msg});
                    cout << clr::GREEN << "  Request enqueued.\n" << clr::RESET;
                    break;
                }
                case 6: {
                    printHeader("6. Process Friend Request (Queue)");
                    FriendRequest r;
                    if(requests.processNext(r)) {
                        cout << clr::GREEN << "  Processed: User " << r.fromID << " wants to follow User " << r.toID << "\n" << clr::RESET;
                        cout << "  Accept? (y/n): "; char y; cin >> y;
                        if(y == 'y' || y == 'Y') {
                            graph.addFriendship(r.fromID, r.toID);
                            cout << clr::GREEN << "  Friendship created!\n" << clr::RESET;
                        }
                    } else cout << clr::YELLOW << "  No pending requests.\n" << clr::RESET;
                    break;
                }
                case 7: {
                    printHeader("7. Find User by ID");
                    cout << "  UserID: "; int id; cin >> id;
                    User u;
                    if(users.findUser(id, u)) u.print();
                    else cout << clr::RED << "  User not found.\n" << clr::RESET;
                    break;
                }
                case 8: {
                    printHeader("8. Display Influencer Rankings");
                    ranking.displayRankings(users);
                    break;
                }
                case 9: {
                    printHeader("9. Add Friendship");
                    cout << "  User A: "; int a; cin >> a;
                    cout << "  User B: "; int b; cin >> b;
                    if(users.exists(a) && users.exists(b)) {
                        graph.addFriendship(a, b);
                        cout << clr::GREEN << "  Friendship established.\n" << clr::RESET;
                    } else cout << clr::RED << "  Invalid users.\n" << clr::RESET;
                    break;
                }
                case 10: {
                    printHeader("10. Find Mutual Friends");
                    cout << "  User A: "; int a; cin >> a;
                    cout << "  User B: "; int b; cin >> b;
                    auto mut = graph.getMutualFriends(a, b);
                    cout << "  " << mut.size() << " Mutual Friends: ";
                    for(size_t i = 0; i < min((size_t)20, mut.size()); i++) {
                        User u; users.findUser(mut[i], u); 
                        cout << "@" << u.username << " "; 
                    }
                    if (mut.size() > 20) cout << "...and " << (mut.size() - 20) << " more";
                    cout << "\n";
                    break;
                }
                case 11: {
                    printHeader("11. Detect Communities");
                    graph.detectCommunities(users);
                    break;
                }
                case 12: {
                    printHeader("12. Find Shortest Path Between Users");
                    cout << "  Source ID: "; int s; cin >> s;
                    cout << "  Dest ID: "; int d; cin >> d;
                    graph.findViralSpreadPath(s, d, users);
                    break;
                }
                case 13: {
                    printHeader("13. Friend Recommendations");
                    cout << "  UserID: "; int id; cin >> id;
                    recommendations.suggestFriends(id, graph, users);
                    break;
                }
                case 14: {
                    running = false; cout << clr::CYAN << "  Exiting BuzzLink...\n" << clr::RESET;
                    break;
                }
                default: cout << clr::RED << "  Invalid choice.\n" << clr::RESET;
            }
        }
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// MAIN
// ─────────────────────────────────────────────────────────────────────────────
int main() {
    BuzzLinkApp app;
    app.run();
    return 0;
}
