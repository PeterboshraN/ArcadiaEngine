#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;


int lootSplitting(int coins[], int n){ //O(n * sum)
    int totalSum = 0;
    for (int i = 0; i < n; i++) { //calc the total sum
        totalSum += coins[i];
    }
    int halfsum = totalSum/2;
    bool table[n+1][halfsum+1]; // + 1 as we add 0 in both row and column
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

int inventoryPacker(int capacity ,vector<pair<int, int>> items ){   // O(n * w)
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
int ChatAutocorrect(string word){
    const int MOD = 1e9 + 7;
    long long len =word.size();
    int i = 0;
    while (i < len) {
        if (word[i] == 'm' || word[i] == 'w') {
            return 0;
        }
    i++;
}
    long long table[len+1]; 
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
