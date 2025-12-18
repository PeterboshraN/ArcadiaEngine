// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable
{
private:
    // TODO: Define your data structures here
    // Hint: You'll need a hash table with double hashing collision resolution

public:
    ConcretePlayerTable()
    {
        // TODO: Initialize your hash table
    }

    void insert(int playerID, string name) override
    {
        // TODO: Implement double hashing insert
        // Remember to handle collisions using h1(key) + i * h2(key)
    }

    string search(int playerID) override
    {
        // TODO: Implement double hashing search
        // Return "" if player not found
        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard
{
private:
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers

    struct Node
    {
        int playerID;
        int score;
        vector<Node *> forward;

        Node(int level, int id, int sc)
            : playerID(id), score(sc), forward(level, nullptr) {}
    };

    static const int MAX_LEVEL = 16;
    float P = 0.5;

    int level;
    Node *header;

    int randomLevel()
    {
        int lvl = 1;
        while (((double)rand() / RAND_MAX) < P && lvl < MAX_LEVEL)
            lvl++;
        return lvl;
    }

    bool comesBefore(int score1, int id1, int score2, int id2)
    {
        if (score1 != score2)
            return score1 > score2;
        return id1 < id2;
    }

public:
    ConcreteLeaderboard()
    {
        // TODO: Initialize your skip list
        level = 1;
        header = new Node(MAX_LEVEL, -1, INT_MAX);
    }

    void addScore(int playerID, int score) override
    {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score

        vector<Node *> update(MAX_LEVEL, nullptr);
        Node *current = header;

        for (int i = level - 1; i >= 0; i--)
        {
            while (current->forward[i] &&
                   comesBefore(current->forward[i]->score,
                               current->forward[i]->playerID,
                               score, playerID))
            {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current && current->playerID == playerID)
        {
            removePlayer(playerID);
        }

        int newLevel = randomLevel();

        if (newLevel > level)
        {
            for (int i = level; i < newLevel; i++)
                update[i] = header;
            level = newLevel;
        }

        Node *newNode = new Node(newLevel, playerID, score);

        for (int i = 0; i < newLevel; i++)
        {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }

    void removePlayer(int playerID) override
    {
        // TODO: Implement skip list deletion

        vector<Node *> update(MAX_LEVEL, nullptr);
        Node *current = header;

        for (int i = level - 1; i >= 0; i--)
        {
            while (current->forward[i] &&
                   current->forward[i]->playerID < playerID)
            {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (!current || current->playerID != playerID)
            return;

        for (int i = 0; i < level; i++)
        {
            if (update[i]->forward[i] != current)
                break;
            update[i]->forward[i] = current->forward[i];
        }

        delete current;

        while (level > 1 && header->forward[level - 1] == nullptr)
        {
            level--;
        }
    }

    vector<int> getTopN(int n) override
    {
        // TODO: Return top N player IDs in descending score order
        vector<int> result;
        Node *current = header->forward[0];

        while (current && n--)
        {
            result.push_back(current->playerID);
            current = current->forward[0];
        }

        return result;
    }
};
// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree
{
private:
    // TODO: Define your Red-Black Tree node structure
    // Hint: Each node needs: id, price, color, left, right, parent pointers
    int id;
    int price;
    string color;
    ConcreteAuctionTree *left;
    ConcreteAuctionTree *right;
    ConcreteAuctionTree *parent;
    ConcreteAuctionTree *root;
    ConcreteAuctionTree *insertNode(ConcreteAuctionTree *&root, ConcreteAuctionTree *newNode);
    void roteateLeft(ConcreteAuctionTree *&root, ConcreteAuctionTree *x);
    void roteateRight(ConcreteAuctionTree *&root, ConcreteAuctionTree *x);
    void fixInsert(ConcreteAuctionTree *&root, ConcreteAuctionTree *x);
    void fixDelete(ConcreteAuctionTree *&root, ConcreteAuctionTree *x);
    void deleteNode(ConcreteAuctionTree *&root, int itemID);
    void setColor(ConcreteAuctionTree *&root, ConcreteAuctionTree *x, string color);
    string getColor(ConcreteAuctionTree *&root, ConcreteAuctionTree *x);

public:
    ConcreteAuctionTree()
    {
        id = 0;
        price = 0;
        color = "";
        left = NULL;
        right = NULL;
        parent = NULL;
        root = NULL;
    }
    ConcreteAuctionTree *insertNode(ConcreteAuctionTree *&root, ConcreteAuctionTree *newNode)
    {
        if (root == NULL)
        {
            root = newNode;
            return newNode;
        }
        if (newNode->price < root->price)
        {
            root->left = insertNode(root->left, newNode);
            root->left->parent = root;
        }
        else
        {
            root->right = insertNode(root->right, newNode);
            root->right->parent = root;
        }
        return root;
    }

    void insertItem(int itemID, int price) override
    {
        // TODO: Implement Red-Black Tree insertion
        // Remember to maintain RB-Tree properties with rotations and recoloring
        ConcreteAuctionTree *newNode = new ConcreteAuctionTree();
        newNode->id = itemID;
        newNode->price = price;
        newNode->color = "RED";
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->parent = NULL;
        root = insertNode(root, newNode);
    }

    void deleteItem(int itemID) override
    {
        // TODO: Implement Red-Black Tree deletion
        // This is complex - handle all cases carefully
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit( int n,vector<int> &coins)
{
    int totalSum = 0;
    for (int i = 0; i < n; i++) { //calc the total sum
        totalSum += coins[i];
    }
    int halfsum = totalSum/2;
    vector<vector<bool>> table(n+1, vector<bool>(halfsum+1)); // + 1 as we add 0 in both row and column
        //     0     1      2   <- s sum of coins I want to reach
        //0    T     F      F
        //1    T     
        //2    T
        //i   
//num of coins i have

    for (int i = 0; i <= n; i++){
        for (int s = 0; s <= halfsum; s++){
            table[i][s] = false;
        }
    }
    for (int i = 0; i <= n; i++){ // i can always make sum of 0
        table[i][0] = true;
    }
    for (int i = 1; i <= n; i++) {
        for (int s = 1; s <= halfsum; s++) {
            if (s >= coins[i-1]){ // if the sum i want to make greater than the coin
                table[i][s] = table[i-1][s] || table[i-1][s - coins[i-1]]; //exclude it and carry previous answer or include it and reduce the sum by the coin you take it
            }else{
                table[i][s] = table[i-1][s]; //exclude it
            }
        }
    }
    int largestSum = 0;
    for (int s = halfsum; s >= 0; s--) {
        if (table[n][s] == true) {
            largestSum = s;
            break;
        }
    }
return totalSum - 2 * largestSum; //return difference
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>> &items)
{
 //{items.first = weight , items.second = value}
    int numItems = items.size();
    vector<vector<int>>table(numItems + 1, vector<int>(capacity+1, 0));//initializing 2D vector and filling it with 0
    //       i| 0    1    2    3  <-- j
    //v   w  0  0    0    0    0
    //10  1  1  0    10   10   10
    //15  2  2  0    10   15   25   
    //40  3  3  0    10   15   40
    for (int i = 1; i <= numItems ; i++){
        for(int j = 1 ; j <= capacity; j++){
            if(j < items[i-1].first){// if capacity < weight of the item just copy the above value from table 
                table[i][j] = table[i-1][j];
            }else{
                int includeValue = items[i-1].second + table[i-1][j-items[i-1].first]; //if i include it take the value of it value of the above row and column of j - weight
                int excludeValue = table[i-1][j]; // if excluded value of the above
                table[i][j] = max(includeValue,excludeValue);
            }
        }
    }
    return table[numItems][capacity];
}

long long InventorySystem::countStringPossibilities(string word)
{
 const int MOD = 1e9 + 7;
    long long len =word.size();
    int i = 0;
    while (i < len) {
        if (word[i] == 'm' || word[i] == 'w') {
            return 0;
        }
    i++;
}
    vector<long long> table(len+1);
    table[0] = 1;
    table[1] = 1;
    for (int i = 2; i <= len; i++){
        if(((word[i-2] == word[i-1]) && (word[i-1] == 'n' )) || ((word[i-2] == word[i-1]) && (word[i-1] == 'u' ))){
            table[i] = (table[i-1] + table[i-2]) % MOD;
        }else{
            table[i] = table[i-1];
        }
    }
    return table[len];
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>> &edges, int source, int dest)
{
    vector<vector<int>> adj(n);
    for (auto &e : edges)
    {
        adj[e[0]].push_back(e[1]);
        adj[e[1]].push_back(e[0]);
    }

    vector<bool> vis(n, false);
    queue<int> q;
    q.push(source);
    vis[source] = true;

    while (!q.empty())
    {
        int u = q.front(); q.pop();
        if (u == dest) return true;

        for (int v : adj[u])
            if (!vis[v])
            {
                vis[v] = true;
                q.push(v);
            }
    }
    return false;
}


long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
struct Edge { int u, v; long long cost; };

struct DSU
{
    vector<int> p, r;
    DSU(int n) : p(n), r(n,0) { iota(p.begin(), p.end(), 0); }
    int find(int x){ return p[x]==x?x:p[x]=find(p[x]); }
    bool unite(int a,int b)
    {
        a=find(a); b=find(b);
        if(a==b) return false;
        if(r[a]<r[b]) swap(a,b);
        p[b]=a;
        if(r[a]==r[b]) r[a]++;
        return true;
    }
};

long long WorldNavigator::minBribeCost(
    int n,int m,long long goldRate,long long silverRate,
    vector<vector<int>> &roadData)
{
    vector<Edge> edges;
    for(auto &r:roadData)
        edges.push_back({r[0], r[1], r[2]*goldRate + r[3]*silverRate});

    sort(edges.begin(), edges.end(),
         [](Edge &a, Edge &b){ return a.cost < b.cost; });

    DSU dsu(n);
    long long ans = 0;
    int cnt = 0;

    for(auto &e:edges)
        if(dsu.unite(e.u,e.v))
            ans += e.cost, cnt++;

    return (cnt == n-1 ? ans : -1);
}


string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>> &roads)
{
    const long long INF = LLONG_MAX / 4;
    vector<vector<long long>> d(n, vector<long long>(n, INF));

    for(int i=0;i<n;i++) d[i][i]=0;

    for(auto &r:roads)
    {
        d[r[0]][r[1]] = min(d[r[0]][r[1]], (long long)r[2]);
        d[r[1]][r[0]] = min(d[r[1]][r[0]], (long long)r[2]);
    }

    for(int k=0;k<n;k++)
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
                if(d[i][k]+d[k][j] < d[i][j])
                    d[i][j] = d[i][k]+d[k][j];

    long long sum = 0;
    for(int i=0;i<n;i++)
        for(int j=i+1;j<n;j++)
            if(d[i][j] < INF) sum += d[i][j];

    if(sum==0) return "0";

    string b="";
    while(sum) b = char('0'+(sum&1)) + b, sum>>=1;
    return b;
}


// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char> &tasks, int n)
{
    vector<int> freq(26, 0);
    for (char c : tasks)
    {
        freq[c - 'A']++;
    }

    int maxFreq = *max_element(freq.begin(), freq.end());

    int countSameMax = 0;
    for (int f : freq)
    {
        if (f == maxFreq)
            countSameMax++;
    }

    int minIntervals = (maxFreq - 1) * (n + 1) + countSameMax;

    return max((int)tasks.size(), minIntervals);
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C"
{
    PlayerTable *createPlayerTable()
    {
        return new ConcretePlayerTable();
    }

    Leaderboard *createLeaderboard()
    {
        return new ConcreteLeaderboard();
    }

    AuctionTree *createAuctionTree()
    {
        return new ConcreteAuctionTree();
    }

}
