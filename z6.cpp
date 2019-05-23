#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
#include <map>
#include <set>

using namespace std;

struct tree {
    int num = 0;
    bool visited = false;
    string value = "";
    tree *parent = nullptr;
    tree *left = nullptr;
    tree *right = nullptr;
    map<string, bool> boolvariables;
    vector<tree *> children;

};

map<string, bool> variables;

tree *expression_tree;

string expression(string s);

string negativ(string s) {
    int balance = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '(')
            balance++;
        if (s[i] == ')')
            balance--;
        if (balance == 0 && s[i] == '!') {
            return "(!" + negativ(s.substr(i + 1, s.length() - i - 1)) + ")";
        }
    }
    if (s.length() > 0 && s[0] == '(') {
        return expression(s.substr(1, s.length() - 2));
    } else
        return s;
}

string kon(string s) {
    int balance = 0;
    for (int i = s.length() - 1; i >= 0; i--) {
        if (s[i] == '(')
            balance++;
        if (s[i] == ')')
            balance--;
        if (balance == 0 && s[i] == '&') {
            return "(&," + kon(s.substr(0, i)) + "," + negativ(s.substr(i + 1, s.length() - i)) + ")";
        }
    }
    return negativ(s);
}

string diz(string s) {
    int balance = 0;
    for (int i = s.length() - 1; i >= 0; i--) {
        if (s[i] == '(')
            balance++;
        if (s[i] == ')')
            balance--;
        if (balance == 0 && s[i] == '|') {
            return "(|," + diz(s.substr(0, i)) + "," + kon(s.substr(i + 1, s.length() - i - 1)) + ")";
        }
    }
    return kon(s);
}

string expression(string s) {
    int balance = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '(')
            balance++;
        if (s[i] == ')')
            balance--;
        if (balance == 0 && s[i] == '-') {
            return "(->," + diz(s.substr(0, i)) + "," + expression(s.substr(i + 2, s.length() - i - 2)) + ")";
        }
    }
    return diz(s);
}

void buildtree(string s, tree *node) {
    bool flag = false;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '&' or s[i] == '|' or s[i] == '-') {
            node->value = s[i];
            int j = 0;
            if (s[i + 1] == '>') j = i + 2;
            else
                j = i + 1;
            int balance = 0;
            for (int k = j + 1; k < s.length(); k++) {
                if (s[k] == '(') balance++;
                if (s[k] == ')') balance--;
                if (s[k] == ',' && balance == 0) {
                    node->left = new tree();
                    node->right = new tree();
                    buildtree(s.substr(j + 1, k - j - 1), node->left);
                    buildtree(s.substr(k + 1, s.length() - k - 2), node->right);
                    flag = true;
                    break;
                }
            }
            if (flag) break;
        } else {
            if (s[i] == '!') {
                node->value = s[i];
                node->left = new tree();
                buildtree(s.substr(i + 1, s.length() - 2 - i), node->left);
                break;
            }
            int j = i;
            if (node->value == "") {
                while (j < s.length() && s[j] != '(' && s[j] != ')') {
                    node->value += s[j];
                    j++;
                }
                if (j != i) {
                    variables.insert(make_pair(node->value, false));
                }
            }
        }
    }
}

int number;

void numerical(tree *node) {
    node->num = number;
    number++;
    for (int i = 0; i < node->children.size(); i++) {
        numerical(node->children[i]);
    }
}

bool flag = false;

void improve_tree_by_or(string left, string right, tree *node) {
    node->boolvariables["(|," + left + "," + right + ")"] = node->boolvariables[left] || node->boolvariables[right];
    for (int i = 0; i < node->children.size(); i++) {
        improve_tree_by_or(left, right, node->children[i]);
    }
}

void improve_tree_by_and(string left, string right, tree *node) {
    node->boolvariables["(&," + left + "," + right + ")"] = node->boolvariables[left] && node->boolvariables[right];
    for (int i = 0; i < node->children.size(); i++) {
        improve_tree_by_and(left, right, node->children[i]);
    }
}

bool improve_tree_by_impl(string left, string right, tree *node) {
    bool value = true;
    for (int i = 0; i < node->children.size(); i++) {
        value &= improve_tree_by_impl(left, right, node->children[i]);
    }
    if (value && (!node->boolvariables[left] || node->boolvariables[right])) {
        node->boolvariables["(->," + left + "," + right + ")"] = true;
        return true;
    } else {
        node->boolvariables["(->," + left + "," + right + ")"] = false;
        return false;
    }
}

bool improve_tree_by_not(string left, tree *node) {
    bool value = true;
    for (int i = 0; i < node->children.size(); i++) {
        value &= improve_tree_by_not(left, node->children[i]);
    }
    if (value && !node->boolvariables[left]) {
        node->boolvariables["(!" + left + ")"] = true;
        return true;
    } else {
        node->boolvariables["(!" + left + ")"] = false;
        return false;
    }
}

string improve_tree(tree *node, tree *model_tree) {
    if (node == nullptr) {
        return "";
    }
    if (node->value == "|") {
        string left = improve_tree(node->left, model_tree);
        string right = improve_tree(node->right, model_tree);
        improve_tree_by_or(left, right, model_tree);
        return "(|," + left + "," + right + ")";
    } else if (node->value == "&") {
        string left = improve_tree(node->left, model_tree);
        string right = improve_tree(node->right, model_tree);
        improve_tree_by_and(left, right, model_tree);
        return "(&," + left + "," + right + ")";
    } else if (node->value == "-") {
        string left = improve_tree(node->left, model_tree);
        string right = improve_tree(node->right, model_tree);
        improve_tree_by_impl(left, right, model_tree);
        return "(->," + left + "," + right + ")";
    } else if (node->value == "!") {
        string left = improve_tree(node->left, model_tree);
        improve_tree_by_not(left, model_tree);
        return "(!" + left + ")";
    } else {
        return node->value;
    }
}

bool check_full_expression(tree *node, string full_expresion) {
    bool value = true;
    for (int i = 0; i < node->children.size(); i++) {
        value &= check_full_expression(node->children[i], full_expresion);
    }
    return value && node->boolvariables[full_expresion];
}

bool check_expression_tree(tree *node) {
    string expression = improve_tree(expression_tree, node);
    return check_full_expression(node, expression);
}

vector<tree *> treemodels;

set<tree *> base;
set<int> tmp;
vector<set<int> > final_base;

void create_base(tree *node) {
    base.insert(node);
    for (int i = 0; i < node->children.size(); i++) {
        create_base(node->children[i]);
    }
}

set<int> push_num(tree *node) {
    tmp.insert(node->num);
    for (int i = 0; i < node->children.size(); i++) {
        push_num(node->children[i]);
    }
    return tmp;
}

void mini_tree(set<tree *> base) {
    for (auto i = base.begin(); i != base.end(); i++) {
        tmp.clear();
        final_base.push_back(push_num(*i));
    }
}

set<set<int> > topology;

void create_top(vector<set<int> > top) {
    set<int> tmp;
    for (int i = 0; i < (1 << top.size()); i++) {
        int help = i;
        for (int j = top.size() - 1; j >= 0; j--) {
            if (help % 2 == 1) {
                for (auto k = top[j].begin(); k != top[j].end(); k++) {
                    tmp.insert(*k);
                }
            }
            help /= 2;
        }
        topology.insert(tmp);
        tmp.clear();
    }
}

vector<set<int> > alg_Gey(47);

bool compare(set<int> set1, set<int> set2) {
    for (auto i = set1.begin(); i != set1.end(); i++) {
        if (set2.count(*i) == 0) {
            return false;
        }
    }
    return true;
}

void create_alg_Gey(const set<set<int> > &mySet) {
    int ch = 0;
    for (auto i = mySet.begin(); i != mySet.end(); i++) {
        int chh = 0;
        for (auto j = mySet.begin(); j != mySet.end(); j++) {
            if (compare(*i, *j)) {
                alg_Gey[ch].insert(chh);
            }
            chh++;
        }
        ch++;
    }
}

set<int> useless_node;

void find_node(string var, tree *node) {
    if (node->boolvariables[var]) {
        useless_node.insert(node->num);
    }
    for (int i = 0; i < node->children.size(); i++) {
        find_node(var, node->children[i]);
    }
}

int create_good_node(set<set<int> > topol) {
    int ch = 0;
    for (auto j = topol.begin(); j != topol.end(); j++) {
        set<int> tmp = *j;
        if (tmp == useless_node) {
            return ch + 1;
        }
        ch++;
    }
}

void set_variables(string s, tree* node) {
    while (s != "") {
        int pos = s.find(',');
        if (pos != -1) {
            string tmp = s.substr(0, pos);
            node->boolvariables[tmp] = true;
            s = s.substr(pos + 1, s.size() - pos - 1);
        } else {
            node->boolvariables[s] = true;
            s = "";
        }


    }
}

void push_node(int pos, string s, tree *node) {
    if (pos != 1) {
        if (node->children.size() != 0){
            push_node(pos - 1, s, node->children[node->children.size() - 1]);
        } else {
            node->children.resize(node->children.size() + 1);
            node->children[node->children.size() - 1] = new tree();
            node->children[node->children.size() - 1]->parent = node;
            node->children[node->children.size() - 1]->boolvariables = variables;
            push_node(pos - 1, s, node->children[node->children.size() - 1]);
        }
    }
    if (flag) {
        return;
    }
    node->children.resize(node->children.size() + 1);
    node->children[node->children.size() - 1] = new tree();
    node->children[node->children.size() - 1]->parent = node;
    node->children[node->children.size() - 1]->boolvariables = variables;
    set_variables(s, node->children[node->children.size() - 1]);
    flag = true;
}

bool comparator(tree* node) {
    if (node->parent == node) {
        return true;
    }
    for (auto i = node->parent->boolvariables.begin(); i != node->parent->boolvariables.end(); i++){
        pair<string, bool> tmp = *i;
        if (tmp.second && !node->boolvariables[tmp.first]){
            return false;
        }
    }
    return true;
}

string correct_model(tree* node) {
    if (node->children.size() != 0) {
        for (int i = 0; i < node->children.size(); i++){
            correct_model(node->children[i]);
        }
    }
    if (flag) {
        return "Не модель Крипке";
    }
    if (!comparator(node)) {
        flag = true;
        return "Не модель Крипке";
    }
    return "";
}

int main() {
    string s;
    getline(cin, s);
    string ss = expression(s);
    expression_tree = new tree();
    buildtree(ss, expression_tree);
    while (getline(cin, s)) {
        if (s == "") break;
        int pos = s.find('*');
        if (pos == s.size() - 1) {
            s = "";
        } else {
            s = s.substr(pos + 2, s.size() - pos - 2);
        }
        string tmp = "";
        for (int i = 0; i < s.size(); i++){
            if (s[i] != ' ') {
                tmp += s[i];
            }
        }
        s = tmp;
        if (pos == 0) {
            treemodels.resize(treemodels.size() + 1);
            treemodels[treemodels.size() - 1] = new tree();
            treemodels[treemodels.size() - 1]->boolvariables = variables;
            treemodels[treemodels.size() - 1]->parent = treemodels[treemodels.size() - 1];
            set_variables(s, treemodels[treemodels.size() - 1]);
        } else {
            flag = false;
            push_node(pos, s, treemodels[treemodels.size() - 1]);
        }
    }
    number = 0;
    for (int i = 0; i < treemodels.size(); i++) {
        numerical(treemodels[i]);
        string s = "";
        flag = false;
        s = correct_model(treemodels[i]);
        if (s != ""){
            cout << s;
            return 0;
        }
        if (check_expression_tree(treemodels[i])){
            cout << "Не опровергает формулу";
            return 0;
        }
    }
    for (int i = 0; i < treemodels.size(); i++){
        create_base(treemodels[i]);
    }
    mini_tree(base);
    create_top(final_base);
    create_alg_Gey(topology);
    cout << topology.size() << endl;
    for (int i = 0; i < alg_Gey.size(); i++) {
        if (alg_Gey[i].size() == 0) continue;
        for (auto j = alg_Gey[i].begin(); j != alg_Gey[i].end(); j++) {
            cout << *j + 1 << " ";
        }
        cout << endl;
    }
    for (auto i = variables.begin(); i != variables.end(); i++) {
        useless_node.clear();
        pair<string, bool> tmp = *i;
        for (int j = 0; j < treemodels.size(); j++) {
            find_node(tmp.first, treemodels[j]);
        }
        if (i == variables.begin()) {
            cout << tmp.first << "=" << create_good_node(topology);
        } else {
            cout << "," << tmp.first << "=" << create_good_node(topology);
        }

    }
    return 0;
}