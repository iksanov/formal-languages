//Tests:
//ab+c.aba.*.bac.+.+* 3 1
//acb..bab.c.*.ab.ba.+.+*a. 3 0

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <stack>
#include <limits>
#include <set>
#include <cstdio>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::stack;
using std::pair;
using std::make_pair;
using std::set;

//consists of vector of pairs:
// - bool: is this length (= index of vector) possible to reach, with the help of this stack element
// - int: full length ('n' that will be answer), because index - is reminder
struct StackElement {
    StackElement() {}
    explicit StackElement(int size) : possible_len(size, make_pair(false, std::numeric_limits<int>::max())) {}
    vector<pair<bool, int>> possible_len; //possible lengths
    pair<bool, int> operator[](int i) const { return possible_len[i]; }
    pair<bool, int> &operator[](int i) { return possible_len[i]; }
};

string str; //regular expression from input
int k = -1; //k from input
int l = -1; //l from input
stack <StackElement> ans; //"stack" where we get answer in the end

void print_err_message() {
    cout << "Incorrect input." << endl <<
         "(Remind that regular expression shouldn't include spaces.)" << endl;
    exit(1);
}

void ans_letter() { //if letter
    StackElement elem(k);
    elem[1].first = true;
    elem[1].second = 1;
    ans.push(elem);
}

void ans_epsilon() { //if epsilon
    StackElement elem(k);
    elem[0].first = true;
    elem[0].second = 0;
    ans.push(elem);
}

void ans_concat() { //concatenation of two last expressions in stack
    StackElement elem2 = ans.top();
    ans.pop();
    StackElement elem1 = ans.top();
    ans.pop();
    StackElement new_elem(k);
    for (int i = 0; i < k; ++i) {
        if (elem2[i].first) {
            for (int j = 0; j < k; ++j) {
                if (elem1[j].first) {
                    new_elem[(i + j) % k].first = true;
                    new_elem[(i + j) % k].second = std::min(elem1[j].second + elem2[i].second, new_elem[(i + j) % k].second);
                }
            }
        }
    }
    ans.push(new_elem);
}

void ans_add() { //adding two last expressions in stack
    StackElement elem2 = ans.top();
    ans.pop();
    StackElement elem1 = ans.top();
    ans.pop();
    StackElement new_elem(k);
    for (int i = 0; i < k; ++i) {
        new_elem[i] = make_pair(elem1[i].first || elem2[i].first, std::min(elem1[i].second, elem2[i].second));
    }
    ans.push(new_elem);
}

bool cmp(const std::pair<int, int>& pair1, const std::pair<int, int>& pair2) {
    return pair1.second < pair2.second;
}

void ans_star() {
    StackElement elem = ans.top();
    ans.pop();
    StackElement new_elem(k);
    new_elem[0].first = 1; //for an empty word
    new_elem[0].second = 0;
    //vector consists of possible minimum length(n) and corresponding reminder (index in original vector)
    std::vector<pair<int, int>> sorted_rems(k -1); //do not include possible length in zero index (len[0]) because it can be "relaxed"
    for (int i = 1; i < k; ++i) {
        if (elem[i].first) {
            sorted_rems[i - 1] = std::make_pair(i, elem[i].second);
        }
        else { //put INF if rem does not exist
            sorted_rems[i - 1] = std::make_pair(i, std::numeric_limits<int>::max());
        }
    }
    std::sort(sorted_rems.begin(), sorted_rems.end(), cmp); //sort by min possible length
    vector<int> indexes(k); //map old indexes and their new positions in new vector
    for (int i = 0; i < k - 1; ++i) {
        indexes[sorted_rems[i].first] = i;
    }
    for (int i = 0; i < k - 1; ++i) {
        if (sorted_rems[i].second == std::numeric_limits<int>::max()) {
            break;
        }
        for (int j = i; j < k - 1; ++j) {
            if (sorted_rems[j].second == std::numeric_limits<int>::max()) {
                break;
            }
            int ind_rem_sum = (sorted_rems[i].first + sorted_rems[j].first) % k;
            int len_sum = (sorted_rems[i].second + sorted_rems[j].second);
            if (ind_rem_sum == 0) {
                continue;
            }
            int index = indexes[ind_rem_sum];
            if(sorted_rems[index].second <= len_sum) { //if relaxation is not possible
                continue;
            }
            sorted_rems[index].second = len_sum; //relax
            //move "relaxed" elements (and also their corresponding indexes in "indexes" vector) to the left like in bubble sort
            while (sorted_rems[index - 1].second > sorted_rems[index].second) {
                std::swap(sorted_rems[index - 1], sorted_rems[index]);
                std::swap(indexes[sorted_rems[index - 1].first], indexes[sorted_rems[index].first]);
                index -= 1;
            }
        }
    }
    for (int i = 0; i < k - 1; ++i) {
        if (sorted_rems[i].second != std::numeric_limits<int>::max()) {
            new_elem.possible_len[sorted_rems[i].first] = std::make_pair(true, sorted_rems[i].second);
        } else {
            new_elem.possible_len[sorted_rems[i].first] = std::make_pair(false, 0);
        }
    }
    ans.push(new_elem);
}

int main() {
    freopen("input", "r", stdin);
    freopen("output", "w", stdout);
    cin >> str;
    cin >> k;
    cin >> l;
    if (l >= k || k == -1 || l == -1) { //check for incorrect input
        print_err_message();
    }

    for (int i = 0; i < str.length(); ++i) {
        if (str[i] == 'a' || str[i] == 'b' || str[i] == 'c') { //if symbol is a letter
            ans_letter();
        }
        else if (str[i] == '1') { //if epsilon
            ans_epsilon();
        }
        else if (str[i] == '.') { //if concatenation
            if (ans.size() < 2) {
                print_err_message();
            }
            ans_concat();
        }
        else if (str[i] == '+') { //if additing
            if (ans.size() < 2) {
                print_err_message();
            }
            ans_add();
        }
        else if (str[i] == '*') { //if star
            if (ans.size() < 1) {
                print_err_message();
            }
            ans_star();
        }
        else { //other variants aren't possible
            print_err_message();
        }
    }

    bool is_inf = true;
    if (ans.size() != 1) { //check for incorrect input
        print_err_message();
    }
    else {
        if (ans.top()[l].first == true) {
            cout << ans.top()[l].second;
            is_inf = false;
        }
        else {
            is_inf = true;
        }
    }
    if (is_inf == true) {
        cout << "INF";
    }
    return 0;
}