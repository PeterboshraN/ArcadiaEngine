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

// --- 1. PlayerTable (Double Hashing) ---
class ConcretePlayerTable : public PlayerTable
{
private:
    static const int TABLE_SIZE = 101;
    static const int PRIME = 97;

    struct Entry
    {
        int playerID;
        string name;
        bool occupied;

        Entry() : playerID(-1), name(""), occupied(false) {}
    };

    Entry table[TABLE_SIZE];

    // Division method
    int h1(int key) const
    {
        return key % TABLE_SIZE;
    }

    // Second hash
    int h2(int key) const
    {
        return PRIME - (key % PRIME);
    }

public:
    ConcretePlayerTable() {}

    void insert(int playerID, string name) override
    {
        int index1 = h1(playerID);
        int index2 = h2(playerID);

        for (int i = 0; i < TABLE_SIZE; i++)
        {
            int index = (index1 + i * index2) % TABLE_SIZE;

            // Update existing
            if (table[index].occupied && table[index].playerID == playerID)
            {
                table[index].name = name;
                return;
            }

            // Insert new
            if (!table[index].occupied)
            {
                table[index].playerID = playerID;
                table[index].name = name;
                table[index].occupied = true;
                return;
            }
        }

        cout << "Table is Full" << endl;
    }

    string search(int playerID) override
    {
        int index1 = h1(playerID);
        int index2 = h2(playerID);

        for (int i = 0; i < TABLE_SIZE; i++)
        {
            int index = (index1 + i * index2) % TABLE_SIZE;

            if (!table[index].occupied)
                return "";

            if (table[index].playerID == playerID)
                return table[index].name;
        }

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
    enum Color
    {
        RED,
        BLACK
    };

    struct RBNode
    {
        int id;
        int price;
        Color color;
        RBNode *left;
        RBNode *right;
        RBNode *parent;

        RBNode(int itemID, int itemPrice)
            : id(itemID), price(itemPrice), color(RED),
              left(nullptr), right(nullptr), parent(nullptr) {}
    };

    RBNode *root;

    // ==================== COLOR HELPERS ====================
    Color getColor(RBNode *node)
    {
        return (node == nullptr) ? BLACK : node->color;
    }

    void setColor(RBNode *node, Color c)
    {
        if (node != nullptr)
            node->color = c;
    }

    // ==================== ROTATIONS ====================
    void rotateLeft(RBNode *&root, RBNode *x)
    {
        RBNode *y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;

        y->parent = x->parent;
        if (x->parent == nullptr)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        y->left = x;
        x->parent = y;
    }

    void rotateRight(RBNode *&root, RBNode *x)
    {
        RBNode *y = x->left;
        x->left = y->right;
        if (y->right != nullptr)
            y->right->parent = x;

        y->parent = x->parent;
        if (x->parent == nullptr)
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;

        y->right = x;
        x->parent = y;
    }

    // ==================== COMPOSITE KEY ====================
    bool less(RBNode *a, RBNode *b)
    {
        if (a->price != b->price)
            return a->price < b->price;
        return a->id < b->id;
    }

    // ==================== BST INSERT ====================
    RBNode *insertNode(RBNode *&root, RBNode *newNode)
    {
        if (root == nullptr)
            return newNode;

        if (less(newNode, root))
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

    // ==================== FIX INSERT ====================
    void fixInsert(RBNode *&root, RBNode *node)
    {
        while (node != root && getColor(node->parent) == RED)
        {
            RBNode *parent = node->parent;
            RBNode *grand = parent->parent;

            if (parent == grand->left)
            {
                RBNode *uncle = grand->right;

                if (getColor(uncle) == RED)
                {
                    setColor(parent, BLACK);
                    setColor(uncle, BLACK);
                    setColor(grand, RED);
                    node = grand;
                }
                else
                {
                    if (node == parent->right)
                    {
                        node = parent;
                        rotateLeft(root, node);
                    }
                    setColor(parent, BLACK);
                    setColor(grand, RED);
                    rotateRight(root, grand);
                }
            }
            else
            {
                RBNode *uncle = grand->left;

                if (getColor(uncle) == RED)
                {
                    setColor(parent, BLACK);
                    setColor(uncle, BLACK);
                    setColor(grand, RED);
                    node = grand;
                }
                else
                {
                    if (node == parent->left)
                    {
                        node = parent;
                        rotateRight(root, node);
                    }
                    setColor(parent, BLACK);
                    setColor(grand, RED);
                    rotateLeft(root, grand);
                }
            }
        }
        setColor(root, BLACK);
    }

    // ==================== SEARCH ====================
    RBNode *search(RBNode *node, int itemID)
    {
        if (!node)
            return nullptr;
        if (node->id == itemID)
            return node;
        RBNode *leftRes = search(node->left, itemID);
        if (leftRes)
            return leftRes;
        return search(node->right, itemID);
    }

    // ==================== MIN ====================
    RBNode *min(RBNode *node)
    {
        if (!node)
            return nullptr;
        while (node->left)
            node = node->left;
        return node;
    }

    // ==================== TRANSPLANT ====================
    void transplant(RBNode *&root, RBNode *u, RBNode *v)
    {
        if (!u->parent)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;

        if (v)
            v->parent = u->parent;
    }

    // ==================== FIX DELETE ====================
    void fixDelete(RBNode *&root, RBNode *x, RBNode *xParent)
    {
        while (x != root && getColor(x) == BLACK)
        {
            if (!xParent)
                break;

            if (x == xParent->left)
            {
                RBNode *w = xParent->right;

                if (getColor(w) == RED)
                {
                    setColor(w, BLACK);
                    setColor(xParent, RED);
                    rotateLeft(root, xParent);
                    w = xParent->right;
                }

                if (getColor(w->left) == BLACK && getColor(w->right) == BLACK)
                {
                    setColor(w, RED);
                    x = xParent;
                    xParent = x->parent;
                }
                else
                {
                    if (getColor(w->right) == BLACK)
                    {
                        setColor(w->left, BLACK);
                        setColor(w, RED);
                        rotateRight(root, w);
                        w = xParent->right;
                    }
                    setColor(w, getColor(xParent));
                    setColor(xParent, BLACK);
                    setColor(w->right, BLACK);
                    rotateLeft(root, xParent);
                    x = root;
                }
            }
            else
            {
                RBNode *w = xParent->left;

                if (getColor(w) == RED)
                {
                    setColor(w, BLACK);
                    setColor(xParent, RED);
                    rotateRight(root, xParent);
                    w = xParent->left;
                }

                if (getColor(w->left) == BLACK && getColor(w->right) == BLACK)
                {
                    setColor(w, RED);
                    x = xParent;
                    xParent = x->parent;
                }
                else
                {
                    if (getColor(w->left) == BLACK)
                    {
                        setColor(w->right, BLACK);
                        setColor(w, RED);
                        rotateLeft(root, w);
                        w = xParent->left;
                    }
                    setColor(w, getColor(xParent));
                    setColor(xParent, BLACK);
                    setColor(w->left, BLACK);
                    rotateRight(root, xParent);
                    x = root;
                }
            }
        }
        setColor(x, BLACK);
    }

public:
    ConcreteAuctionTree() : root(nullptr) {}

    // ==================== INSERT ====================
    void insertItem(int itemID, int price) override
    {
        RBNode *node = new RBNode(itemID, price);
        root = insertNode(root, node);
        fixInsert(root, node);
    }

    // ==================== DELETE ====================
    void deleteItem(int itemID) override
    {
        RBNode *z = search(root, itemID);
        if (!z)
            return;

        RBNode *y = z;
        RBNode *x;
        Color yOriginalColor = y->color;

        if (!z->left)
        {
            x = z->right;
            transplant(root, z, z->right);
        }
        else if (!z->right)
        {
            x = z->left;
            transplant(root, z, z->left);
        }
        else
        {
            y = min(z->right);
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == z)
            {
                if (x)
                    x->parent = y;
            }
            else
            {
                transplant(root, y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(root, z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        RBNode *xParent = (x != nullptr) ? x->parent : y->parent;
        delete z;
        if (yOriginalColor == BLACK)
            fixDelete(root, x, xParent);

        if (root)
            root->color = BLACK;
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
    long long len = word.size();
    
    // If the string is empty, return 1
    if (len == 0) {
        return 1;
    }
    
    // Check for invalid characters 'm' or 'w'
    for (int i = 0; i < len; i++) {
        if (word[i] == 'm' || word[i] == 'w') {
            return 0;
        }
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
