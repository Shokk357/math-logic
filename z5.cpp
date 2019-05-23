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

void buildworld(string s, tree *node) {
    string tmp = s.substr(1, s.length() - 2);
    if (tmp[0] == 'x' && tmp[tmp.length() - 1] == 'x') {
        node->left = new tree();
        node->right = new tree();
        node->left->parent = node;
        node->right->parent = node;
        node->boolvariables = variables;
        node->left->boolvariables = variables;
        node->right->boolvariables = variables;
        return;
    } else if (tmp[0] == 'x') {
        node->boolvariables = variables;
        node->left = new tree();
        node->right = new tree();
        node->left->parent = node;
        node->right->parent = node;
        node->left->boolvariables = variables;
        tmp = tmp.substr(1, tmp.length() - 1);
        buildworld(tmp, node->right);
    } else if (tmp[tmp.length() - 1] == 'x') {
        node->boolvariables = variables;
        node->left = new tree();
        node->right = new tree();
        node->left->parent = node;
        node->right->parent = node;
        node->right->boolvariables = variables;
        tmp = tmp.substr(0, tmp.length() - 1);
        buildworld(tmp, node->left);
    } else {
        int pos = 0;
        for (int i = 0; i < tmp.length() - 1; i++) {
            if (tmp[i] == ')' && tmp[i + 1] == '(') {
                pos = i;
                break;
            }
        }
        node->boolvariables = variables;
        node->right = new tree();
        node->left = new tree();
        node->left->parent = node;
        node->right->parent = node;
        buildworld(tmp.substr(0, pos + 1), node->left);
        buildworld(tmp.substr(pos + 1, tmp.length() - pos + 1), node->right);
    }
}

int number = 0;

void numerical(tree *node) {
    node->num = number;
    number++;
    if (node->left != nullptr) {
        numerical(node->left);
    } else {
        return;
    }
    if (node->right != nullptr) {
        numerical(node->right);
    } else {
        return;
    }
}

vector<int> perebor(9, 0);
bool flag = false;

void longroad(tree *node) {
    if (node->num == 0) {
        node->boolvariables = variables;
    } else {
        node->boolvariables = node->parent->boolvariables;
    }
    int ch = 1;
    for (auto j = node->boolvariables.begin(); j != node->boolvariables.end(); j++) {
        if (perebor[node->num] == ch) {
            j->second = true;
        }
        ch++;
    }
    if (node->left != nullptr) {
        longroad(node->left);
    } else {
        return;
    }
    if (node->right != nullptr) {
        longroad(node->right);
    } else {
        return;
    }
}

void add1(int pos) {
    if (flag) return;
    perebor[pos]++;
    if (perebor[pos] == variables.size() + 1) {
        if (pos == 0) {
            perebor[pos] = 0;
            flag = true;
            return;
        }
        perebor[pos] = 0;
        add1(pos - 1);
    } else {
        return;
    }
}

void improve_tree_by_or(string left, string right, tree *node) {
    node->boolvariables["(|," + left + "," + right + ")"] = node->boolvariables[left] || node->boolvariables[right];
    if (node->left != nullptr) {
        improve_tree_by_or(left, right, node->left);
    }
    if (node->right != nullptr) {
        improve_tree_by_or(left, right, node->right);
    }
}

void improve_tree_by_and(string left, string right, tree *node) {
    node->boolvariables["(&," + left + "," + right + ")"] = node->boolvariables[left] && node->boolvariables[right];
    if (node->left != nullptr) {
        improve_tree_by_and(left, right, node->left);
    }
    if (node->right != nullptr) {
        improve_tree_by_and(left, right, node->right);
    }
}

bool improve_tree_by_impl(string left, string right, tree *node) {
    bool value = true;
    if (node->left != nullptr) {
        value &= improve_tree_by_impl(left, right, node->left);
    }
    if (node->right != nullptr) {
        value &= improve_tree_by_impl(left, right, node->right);
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
    if (node->left != nullptr) {
        value &= improve_tree_by_not(left, node->left);
    }
    if (node->right != nullptr) {
        value &= improve_tree_by_not(left, node->right);
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
    if (node->left != nullptr) {
        value &= check_full_expression(node->left, full_expresion);
    }
    if (node->right != nullptr) {
        value &= check_full_expression(node->right, full_expresion);
    }
    return value && node->boolvariables[full_expresion];
}

bool check_expression_tree(tree *node) {
    string expression = improve_tree(expression_tree, node);
    return check_full_expression(node, expression);
}

bool setboolvariables(tree *node) {
    for (int i = 0; i < perebor.size(); i++) {
        perebor[i] = 0;
    }
    while (!flag) {
        longroad(node);
        if (!check_expression_tree(node)) {
            return false;
        }
        add1(8);
    }
    return true;
}

string models[14] = {
        "((((xx)x)x)x)",
        "(((xx)(xx))x)",
        "((x((xx)x))x)",
        "((x(x(xx)))x)",
        "(((xx)x)(xx))",
        "((x(xx))(xx))",
        "((xx)((xx)x))",
        "((xx)(x(xx)))",
        "(x(((xx)x)x))",
        "(x((x(xx))x))",
        "(x((xx)(xx)))",
        "(x(x((xx)x)))",
        "(x(x(x(xx))))",
        "(((x(xx))x)x)"
};

tree *treemodels[14];

set<tree *> base;
set<int> tmp;
vector<set<int> > final_base;

void create_base(tree *node) {
    base.insert(node);
    if (node->left != nullptr) {
        create_base(node->left);
    }
    if (node->right != nullptr) {
        create_base(node->right);
    }
}

set<int> push_num(tree *node) {
    tmp.insert(node->num);
    if (node->left != nullptr) {
        push_num(node->left);
    }
    if (node->right != nullptr) {
        push_num(node->right);
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
set<set<int> > need_node;

void find_node(string var, tree *node) {
    if (node->boolvariables[var]) {
        useless_node.insert(node->num);
    }
    if (node->left != nullptr) {
        find_node(var, node->left);
    }
    if (node->right != nullptr) {
        find_node(var, node->right);
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

int main() {
    string s;
    getline(cin, s);
    string ss = expression(s);
    expression_tree = new tree();
    buildtree(ss, expression_tree);
    int tree_number = -1;
    for (int i = 0; i < 14; i++) {
        treemodels[i] = new tree();
        buildworld(models[i], treemodels[i]);
        number = 0;
        numerical(treemodels[i]);
        if (!setboolvariables(treemodels[i])) {
            tree_number = i;
            break;
        }
    }
    if (tree_number == -1) {
        cout << "Формула общезначима";
        return 0;
    }
    create_base(treemodels[tree_number]);
    mini_tree(base);
    create_top(final_base);
    create_alg_Gey(topology);
    cout << topology.size() << endl;
    for (int i = 0; i < alg_Gey.size(); i++) {
        for (auto j = alg_Gey[i].begin(); j != alg_Gey[i].end(); j++) {
                cout << *j + 1 << " ";
        }
        cout << endl;
    }
    for (auto i = variables.begin(); i != variables.end(); i++) {
        useless_node.clear();
        pair<string, bool> tmp = *i;
        find_node(tmp.first, treemodels[tree_number]);
        if (i == variables.begin()) {
            cout << tmp.first << "=" << create_good_node(topology);
        } else {
            cout << "," << tmp.first << "=" << create_good_node(topology);
        }

    }
    return 0;
}