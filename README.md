# 🐝 BuzzLink — A Social Media Backend Built with C++

> **What is this?** BuzzLink is a working social media engine (like the backend of Twitter or Instagram) built entirely in C++. It manages 1,000 users, their friendships, privacy settings, friend requests, and even figures out who's the most popular — all using smart computer science techniques called **Data Structures & Algorithms (DSA)**.

---

## 📖 Table of Contents

1. [What Does This Project Do?](#-what-does-this-project-do)
2. [How to Run It](#-how-to-run-it)
3. [Project Files](#-project-files)
4. [The 9 DSA Concepts Explained (In Plain English)](#-the-9-dsa-concepts-explained-in-plain-english)
5. [Menu Options Guide](#-menu-options-guide)
6. [How the Data Flows](#-how-the-data-flows)
7. [Sample Output](#-sample-output)

---

## 🎯 What Does This Project Do?

Imagine you're building Instagram from scratch. You'd need to solve these everyday problems:

| Real-World Problem | What BuzzLink Uses to Solve It |
|---|---|
| "I typed `ali` and want to see all usernames starting with that" | **Trie** (a special tree) |
| "I accidentally set my profile to Private, undo it!" | **Stack** (like an undo button) |
| "Process friend requests in the order they arrived" | **Queue** (like a line at a store) |
| "Find a user instantly by their ID number" | **Hash Map** (like a dictionary) |
| "Who are the top influencers right now?" | **Sorting Algorithms** (Quick Sort, Merge Sort) |
| "Are Alice and Bob connected? Through whom?" | **Graph** (a web of connections) |
| "How many hops for a tweet to go viral from A to B?" | **BFS** (Breadth-First Search) |
| "What friend groups exist in the network?" | **DFS** (Depth-First Search) |
| "Who should I recommend as a new friend?" | **Greedy Algorithm** |

---

## 🏃 How to Run It

### What You Need
- A Mac, Linux, or Windows computer
- A C++ compiler that supports C++17 (like `g++`)

### Step 1: Compile (Turn the code into a program)
Open your terminal, go to the project folder, and type:
```bash
g++ -std=c++17 -O2 -Wall -Wextra -o buzzlink buzzlink.cpp
```

### Step 2: Run
```bash
./buzzlink
```

That's it! The program loads 1,000 users from `users.csv`, creates ~3,000 friendships between them, and shows you a menu with 15 options to play with.

---

## 📁 Project Files

| File | What It Is |
|---|---|
| `buzzlink.cpp` | The entire C++ source code (~786 lines). Everything lives here. |
| `users.csv` | A data file with 1,000 users. Each line has: `ID, username, followers, engagement_score, privacy_status` |
| `README.md` | This file you're reading right now! |

---

## 🧠 The 9 DSA Concepts Explained (In Plain English)

### 1. 🔤 Trie (Prefix Tree) — "The Autocomplete Engine"

**Real-life analogy:** Think of a phone's contact search. You type "al" and it instantly shows "Alice", "Alex", "Alyssa". It doesn't check every single contact one by one — it jumps directly to the "A → L" branch.

**How it works in BuzzLink:**
```
Root
├── a
│   ├── l
│   │   ├── i
│   │   │   ├── c
│   │   │   │   └── e_j ✓ (User ID: 101)
│   │   │   └── ...
│   │   └── e
│   │       └── x_m ✓ (User ID: 205)
├── b
│   └── o
│       └── b_m ✓ (User ID: 102)
```

When you search for prefix `"al"`, the Trie walks down `Root → a → l` and then collects everything below that point. It **never** looks at users starting with `b`, `c`, `d`, etc.

**Why it's fast:** If the prefix is 3 characters long, it takes exactly 3 steps — whether you have 100 users or 10 million users. That's called **O(L)** time, where L = length of what you typed.

**Menu Option:** `2. Search User by Prefix`

---

### 2. 📚 Stack — "The Undo Button"

**Real-life analogy:** A stack of plates. You always add to the top and remove from the top. The last plate you put down is the first one you pick up. This is called **LIFO** (Last In, First Out).

**How it works in BuzzLink:**

Say a user changes their privacy settings over time:
```
Action 1: Set to PUBLIC    → Stack: [PUBLIC]
Action 2: Set to PRIVATE   → Stack: [PUBLIC, PRIVATE]
Action 3: Set to PUBLIC    → Stack: [PUBLIC, PRIVATE, PUBLIC]

Now they hit "Undo":
Pop! → Removes PUBLIC, current setting reverts to PRIVATE
Pop! → Removes PRIVATE, current setting reverts to PUBLIC
```

Each user gets their own stack. You can undo as many times as there are changes in history.

**Why it's fast:** Pushing and popping from the top of a stack takes exactly 1 step — **O(1)** time. No searching needed.

**Menu Options:** `3. Change Privacy Setting` and `4. Undo Privacy Change`

---

### 3. 🧑‍🤝‍🧑 Queue — "The Waiting Line"

**Real-life analogy:** A queue at a coffee shop. The first person who arrives gets served first. This is called **FIFO** (First In, First Out).

**How it works in BuzzLink:**

```
Friend request from Frank → Queue: [Frank]
Friend request from Hank  → Queue: [Frank, Hank]
Friend request from Grace → Queue: [Frank, Hank, Grace]

Process next request:
Dequeue! → Frank gets processed first (he arrived first)
Queue is now: [Hank, Grace]
```

This guarantees fairness — no one can skip the line.

**Why it's fast:** Adding to the back and removing from the front both take **O(1)** time.

**Menu Options:** `5. Send Friend Request` and `6. Process Friend Request`

---

### 4. 🗂️ Hash Map — "The Instant Lookup Dictionary"

**Real-life analogy:** Imagine a library where instead of searching shelf by shelf, every book has a magic number that tells you the exact shelf and position. You walk straight to it.

**How it works in BuzzLink:**

```
Hash Map:
  Key: 101  →  Value: { username: "alice_j", followers: 52400, ... }
  Key: 102  →  Value: { username: "bob_m", followers: 31200, ... }
  Key: 103  →  Value: { username: "charlie_p", followers: 98700, ... }
  ...
```

When you ask "Give me user #547", the hash map doesn't search through 1,000 users. It computes a hash of `547`, jumps to the right bucket, and hands you the data.

**Why it's fast:** Average lookup time is **O(1)** — constant time regardless of how many users exist.

**Menu Option:** `7. Lookup User by ID`

---

### 5. 📊 Sorting Algorithms — "The Influencer Leaderboard"

**The goal:** Rank 1,000 users by a score calculated as:
```
Score = (followers × 0.7) + (engagement × 0.3)
```

BuzzLink runs **three** different sorting algorithms on the same data and times each one in nanoseconds so you can compare them:

#### Quick Sort
- **How:** Pick a middle element as the "pivot". Put everything bigger on the left, everything smaller on the right. Repeat for each half.
- **Speed:** O(N log N) on average
- **Fun fact:** It's called "quick" because it's usually the fastest in practice

#### Merge Sort
- **How:** Split the list in half, sort each half, then merge them back together like a zipper.
- **Speed:** O(N log N) always (guaranteed, never gets slower)
- **Fun fact:** It's "stable" — if two users have the same score, they stay in their original order

#### STL Sort (std::sort)
- **How:** C++'s built-in sort. Uses a hybrid algorithm called IntroSort (Quick Sort + Heap Sort + Insertion Sort).
- **Speed:** O(N log N) — this is the professional benchmark

**Menu Option:** `8. Display Influencer Rankings`

---

### 6. 🕸️ Graph (Adjacency List) — "The Friendship Web"

**Real-life analogy:** Think of users as dots and friendships as lines connecting them. That's a graph!

**How it works in BuzzLink:**

```
Alice (101) ——— Bob (102)
  |               |
  |               |
Charlie (103) ——+
  |
Eve (105) ——— Grace (107)


Stored as an Adjacency List:
  101 → {102, 103}        (Alice knows Bob and Charlie)
  102 → {101, 103}        (Bob knows Alice and Charlie)
  103 → {101, 102, 105}   (Charlie knows Alice, Bob, and Eve)
  105 → {103, 107}        (Eve knows Charlie and Grace)
  107 → {105}             (Grace knows Eve)
```

We use an **adjacency list** (a map of sets) instead of a giant table because most users are NOT friends with most other users. This saves massive amounts of memory.

**Menu Options:** `9. Add Friendship`, `10. Show Mutual Friends`, `14. Display Network`

---

### 7. 🌊 BFS (Breadth-First Search) — "The Viral Spread Tracker"

**Real-life analogy:** Drop a stone in a pond. The ripples spread outward ring by ring. BFS explores a graph the same way — first all neighbors, then all neighbors-of-neighbors, etc.

**How it works in BuzzLink:**

Question: "How does a post travel from Alice (101) to Grace (107)?"

```
Step 1: Start at Alice (101)
        Check Alice's friends: Bob (102), Charlie (103)

Step 2: Check Bob's friends: Alice (already visited), Charlie (already visited)
        Check Charlie's friends: Alice (visited), Bob (visited), Eve (105) ← NEW!

Step 3: Check Eve's friends: Charlie (visited), Grace (107) ← FOUND!

Result: Alice → Charlie → Eve → Grace (3 hops)
```

BFS guarantees this is the **shortest** possible path. It will never find a 5-hop path when a 3-hop path exists.

**Menu Option:** `11. Find Viral Spread Path`

---

### 8. 🔍 DFS (Depth-First Search) — "The Community Finder"

**Real-life analogy:** Imagine exploring a maze. You walk down one path as far as possible, hit a dead end, backtrack, and try another path. That's DFS.

**How it works in BuzzLink:**

A "community" is a group of people who are all connected to each other (directly or through friends), but disconnected from everyone else.

```
Community 1:        Community 2:       Community 3:
Alice — Bob         Diana — Hank       Frank (alone)
  \   /
  Charlie
    |
   Eve — Grace
   
(5 people)          (2 people)         (1 person)
```

DFS starts at an unvisited user, dives deep through all their connections, and marks everyone it reaches as one community. Then it moves on to the next unvisited user and repeats.

**Menu Option:** `12. Detect Communities`

---

### 9. 🎯 Greedy Algorithm — "The Smart Friend Recommender"

**Real-life analogy:** LinkedIn's "People You May Know." If you and a stranger share 5 mutual friends, you're more likely to know each other than someone you share 1 mutual friend with.

**How it works in BuzzLink:**

```
You: Alice (101)
Your friends: Bob (102), Charlie (103)

Bob's friends: Alice (skip), Diana (104), Eve (105)
Charlie's friends: Alice (skip), Eve (105), Frank (106)

Mutual friend counts for people Alice doesn't know:
  Eve:   2 mutual friends (Bob + Charlie both know her)  ← Best pick!
  Diana: 1 mutual friend (Bob knows her)
  Frank: 1 mutual friend (Charlie knows him)

Recommendation: Eve (highest mutual count)
```

It's called **greedy** because it always picks the candidate with the **highest** mutual friend count first, without looking ahead at future consequences.

**Menu Option:** `13. Friend Recommendations`

---

## 🎮 Menu Options Guide

| # | Option | What It Does | DSA Used |
|---|---|---|---|
| 1 | Add User | Create a new user profile | Hash Map |
| 2 | Search User by Prefix | Type a few letters, find all matching usernames | Trie |
| 3 | Change Privacy Setting | Switch between PUBLIC and PRIVATE | Stack |
| 4 | Undo Privacy Change | Revert the last privacy change | Stack |
| 5 | Send Friend Request | Queue a friend request | Queue |
| 6 | Process Friend Request | Handle the oldest pending request | Queue |
| 7 | Lookup User by ID | Find a user by their numeric ID | Hash Map |
| 8 | Display Influencer Rankings | Sort & rank all users by score | Quick/Merge/STL Sort |
| 9 | Add Friendship | Connect two users as friends | Graph |
| 10 | Show Mutual Friends | Find shared connections between two users | Graph |
| 11 | Find Viral Spread Path | Shortest path between two users | BFS |
| 12 | Detect Communities | Find isolated friend groups | DFS |
| 13 | Friend Recommendations | Suggest who to follow next | Greedy Algorithm |
| 14 | Display Network | Show network stats and top connected users | Graph |
| 15 | Exit | Close the program | — |

---

## 🔄 How the Data Flows

```
                    ┌─────────────┐
                    │  users.csv  │  ← 1,000 users stored here
                    │  (external) │
                    └──────┬──────┘
                           │ reads at startup
                           ▼
                   ┌───────────────┐
                   │  preloadData  │  ← Parses CSV, generates ~3,000 friendships
                   └───────┬───────┘
                           │ populates
            ┌──────────────┼──────────────┐
            ▼              ▼              ▼
     ┌────────────┐  ┌───────────┐  ┌───────────┐
     │ UserManager│  │TrieManager│  │GraphManager│
     │ (Hash Map) │  │  (Trie)   │  │(Adj. List) │
     └────────────┘  └───────────┘  └───────────┘
            │              │              │
            └──────────────┼──────────────┘
                           │
                    ┌──────▼──────┐
                    │  Dashboard  │  ← The menu you interact with
                    │  (15 opts)  │
                    └─────────────┘
```

---

## 📸 Sample Output

### Influencer Rankings (Option 8)
```
  Sorting Performance (Nanoseconds):
  Quick Sort: 163458 ns  |  Merge Sort: 255833 ns  |  STL Sort: 124875 ns

  Top Influencers Leaderboard:
  1. @charlie_s39 (Score: 698910)
  2. @sam_l16 (Score: 698002)
  3. @olivia_q17 (Score: 697968)
  ...
```

### Network Overview (Option 14)
```
  Network Overview:
  Total Users: 1000
  Total Friendships: 2989
  Max Friends (Degree): 15

  Top 10 Nodes by Connections:
  @tina_h40 (325) -> 15 friends
  @mia_j99 (172) -> 14 friends
  ...
```

### Community Detection (Option 12)
```
  Total Communities Detected: 7
  Displaying Top 3 Largest Communities:
  Community 1 (994 users): @sam_i6 @ivy_m41 @sam_d33 ...and 984 more
  Community 2 (1 users): @olivia_f28
  Community 3 (1 users): @charlie_s3
```

### Viral Spread BFS (Option 11)
```
  Viral path found in 4 hops!
  @noah_u83 -> @zack_a65 -> @xander_w53 -> @ivy_m41 -> @sam_i6
```

---

## 🧪 Complexity Summary

| Feature | Data Structure | Time Complexity | Space Complexity |
|---|---|---|---|
| Prefix Search | Trie | O(L) | O(N × L) |
| Privacy Undo | Stack | O(1) | O(H) per user |
| Request Processing | Queue | O(1) | O(R) |
| User Lookup | Hash Map | O(1) avg | O(N) |
| Influencer Ranking | Sorting | O(N log N) | O(N) |
| Add Friendship | Graph (Adj. List) | O(1) | O(V + E) |
| Viral Spread | BFS | O(V + E) | O(V) |
| Community Detection | DFS | O(V + E) | O(V) |
| Friend Suggestions | Greedy | O(D²) | O(D) |

> **Key:** N = total users, L = prefix length, H = history depth, R = pending requests, V = vertices (users), E = edges (friendships), D = average degree (friends per user)

---

*Built with ❤️ by Kv404*
