#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <fstream>
using namespace std;

class Node
{
public:
    int a;
    char c;
    Node *left, *right;

    Node()
    {
        left = right = nullptr;
    }

    Node(Node *L, Node *R)
    {
        left =  L;
        right = R;
        a = L->a + R->a;
    }
};

struct Compare
{
    bool operator()(const Node* l, const Node* r) const
    {
        return l->a < r->a;
    }
};


vector<bool> code;
map<char,vector<bool> > table;

void CreateTable(Node *root)
{
    if (root->left != nullptr)
    {
        code.push_back(false);
        CreateTable(root->left);
    }

    if (root->right != nullptr)
    {
        code.push_back(true);
        CreateTable(root->right);
    }

    if (root->left == nullptr && root->right == nullptr) table[root->c] = code;

    code.pop_back();
}


int main (int argc, char *argv[])
{
    ifstream in("C:/Users/marsi/CLionProjects/Huffman/text.txt", ios::out | ios::binary);

    map<char, int> freq;

    while (!in.eof())
    {
        char c = in.get();
        freq[c]++;
    }

    list<Node*> symbols;
    for(map<char,int>::iterator itr = freq.begin(); itr != freq.end(); ++itr)
    {
        Node *p = new Node;
        p->c = itr->first;
        p->a = itr->second;
        symbols.push_back(p);
    }



    while (symbols.size() != 1)
    {
        symbols.sort(Compare());

        Node *SonL = symbols.front();
        symbols.pop_front();
        Node *SonR = symbols.front();
        symbols.pop_front();

        Node *parent = new Node(SonL,SonR);
        symbols.push_back(parent);

    }

    Node *root = symbols.front();


    CreateTable(root);

//encode
    in.clear();
    in.seekg(0);

    ofstream out("C:/Users/marsi/CLionProjects/Huffman/output.txt", ios::out | ios::binary);

    int count = 0;
    char buf = 0;
    while (!in.eof())
    {
        char c = in.get();
        vector<bool> x = table[c];
        for(int n = 0; n < x.size(); n++)
        {
            buf |= x[n] << (7-count);
            count++;
            if (count == 8)
            {
                count = 0;
                out << buf;
                buf = 0;
            }
        }
    }

    in.close();
    out.close();

//decode
    ifstream IN("C:/Users/marsi/CLionProjects/Huffman/output.txt", ios::in | ios::binary);
    ofstream OUT("C:/Users/marsi/CLionProjects/Huffman/decoded.txt", ios::in | ios::binary);

    setlocale(LC_ALL,"Russian");

    Node *p = root;
    count = 0;
    char byte;
    byte = IN.get();
    while(!IN.eof())
    {
        bool b = byte & (1 << (7-count));
        if (b) p = p->right;
            else p = p->left;
        if (p->left == nullptr && p->right == nullptr)
        {
            cout << p->c;
            OUT << p->c;
            p = root;
        }
        count++;
        if (count == 8)
        {
            count = 0;
            byte = IN.get();
        }
    }

    IN.close();
    OUT.close();

    return 0;
}