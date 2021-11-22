#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <cstdlib>

using namespace std;

// Строка для анализа
string str;

// Сопоставляет встречающиеся символы с их относительными частотами
map<char, double> P;

// Дерево Хаффмана
struct HuffTreeNode
{
    double p;
    bool leaf;
    char letter;

    HuffTreeNode *parent;
    HuffTreeNode *l;
    HuffTreeNode *r;

// нелистовой узел
    HuffTreeNode(vector<HuffTreeNode*> &vec)
    {
        if(vec.size() > 1){
            vector<vector<HuffTreeNode*>> two = twoVector(vec);
            this->l = new HuffTreeNode(two[0]);
            this->r = new HuffTreeNode(two[1]);
        }
    }

    vector<vector<HuffTreeNode *>> twoVector(vector<HuffTreeNode *> &vec)
    {
        double count = 1;
        double c = 0;
        double before;
        vector<vector<HuffTreeNode *>> two;
        vector<HuffTreeNode *> first;
        vector<HuffTreeNode *> second;
        for (size_t i = 0; i < vec.size(); i++)
        {
            before = c;
            c += vec[i]->p;
            if (abs(int(count - 2 * before)) > abs(int(count - 2 * c)))//Проверяем, что если добавить новый символ в первый вектор, то не будет большо
                first.push_back(vec[i]);
            else
                second.push_back(vec[i]);
        }
        two.push_back(first);
        two.push_back(second);
        return two;
    }

    // листовой узел
    HuffTreeNode(double p, char c)
    {
        this -> p = p;
        this -> leaf = false;
        this -> letter = c;
        this -> parent = this -> l = this -> r = NULL;
    }

    HuffTreeNode(char c)
    {
        this -> leaf = true;
        this -> letter = c;
        l -> parent = this;
        r -> parent = this;
    }
};

// Компаратор двух указателей узлов
struct CmpNodePtrs
{
    // Поскольку priority_queue является макс кучей,
    // а не мин кучей, инвертируйте значение оператора <,
    // чтобы получить более низкие вероятности сверху
    bool operator()(const HuffTreeNode* lhs, const HuffTreeNode* rhs) const
    {
        return (lhs -> p) > (rhs -> p);
    }
};

// корень дерева
HuffTreeNode *root;

// сопоставление каждого символа с его листовым узлом (для быстрого кодирования)
map<char, HuffTreeNode*> leaf;

// Производит распределение вероятности (может быть опущено, если заранее известно)
inline void analyse()
{
    for (int i = 0; i < str.length(); i++)
    {
        P[str[i]]++;
    }
    for (auto it = P.begin(); it != P.end(); it++)
    {
        P[it -> first] = it -> second / str.length();
    }
}

// Построение дерева Хаффмана, используя распределение вероятности
inline void build_tree()
{
    priority_queue<HuffTreeNode*, vector<HuffTreeNode*>, CmpNodePtrs> pq;

    // Сначала нужно построить листья и заполните очередь приоритетов
    for (auto it = P.begin(); it != P.end(); it++)
    {
        leaf[it -> first] = new HuffTreeNode(it -> second, it -> first);
        pq.push(leaf[it -> first]);
    }

    vector<HuffTreeNode*> vec;
    while (!pq.empty())
    {
        vec.push_back(pq.top());
        pq.pop();
    }

    HuffTreeNode* par = new HuffTreeNode(vec);
}

// Хаффман-кодирование заданного символа
inline string encode(char c)
{
    string ret = "";

    HuffTreeNode* curr = leaf[c];
    while (curr -> parent != NULL)
    {
        if (curr == curr -> parent -> l)
            ret += "0";
        else if (curr == curr -> parent -> r)
            ret += "1";

        curr = curr -> parent;
    }

    reverse(ret.begin(), ret.end());
    return ret;
}

// Хаффман-кодирование заданной строки
inline string encode(string s)
{
    string ret = "";

    for (int i = 0; i < s.length(); i++)
    {
        ret += encode(s[i]);
    }

    return ret;
}

// Хаффман-декодирование заданной строки
inline string decode(string s)
{
    string ret = "";

    int i = 0;
    HuffTreeNode* curr;

    while (i < s.length())
    {
        curr = root;
        while (!(curr -> leaf))
        {
            if (s[i++] == '0')
                curr = curr -> l;
            else
                curr = curr -> r;
        }
        ret += curr -> letter;
    }
    return ret;
}

int main()
{
    cin >> str;

    analyse();
    build_tree();

    map<char, int> word;

    for (int i = 0; i < str.length(); i++)
    {
        if(word[str[i]] == 0){
            cout << "Encode(" << str[i] << ") = " << encode(str[i]) << endl;
            word[str[i]]++;
        }
    }

    cout << endl << "after encode: " << encode(str) << endl;
    cout << endl << "after decode: " << decode(encode(str)) << endl;

    cout << endl << "compression ratio: " << int((double(str.size()) / double(encode(str).size())) * 100) << "%";

    return 0;
}