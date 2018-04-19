#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <algorithm>
using namespace std;

struct node
{
  string text;
  node* parent;
  vector<node*> children;

  /*node(string val, node* par)
  {
    this->text = val;
    this->parent = par;
  }*/
};

struct tree
{
    int nodeCount;
    int height;
    //int* nodeCountPerRow;

    vector<string> nodeString;
    //int nodeLevel[];
};

bool stringMatchesString(string a, string b, int position)
{
  for (int i = 0; i < b.length(); i++)
  {
    if (position + i >= a.length() || a.at(position + i) != b.at(i))
    {
      return false;
    }
  }
  return true;
}

bool stringContainsSubstr(string str, string substr)
{
  for (int i = 0; i < str.length() - substr.length() + 1; i++)
  {
    if (stringMatchesString(str, substr, i))
    {
      return true;
    }
  }
  return false;
}

string getNodeTextFromFile(string fileContents, int start)
{
  int end = start;
  while (fileContents.at(end) != '\n' && fileContents.at(end) != ']' && fileContents.at(end) != '[')
  {
    end++;
  }
  return fileContents.substr(start, end - start);
}
/*
vector<node> buildTree(string filePath)
{
  ifstream ifs (filePath, ifstream::in);
  string fileContents((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

  vector<node> roots;
  vector<node*> parentStack;

  int currentLevel = 0;
  for (int i = 0; i < fileContents.length(); i++)
  {
    if (stringMatchesString(fileContents, "[", i))
    {
      node newNode;
      newNode.text = getNodeTextFromFile(fileContents, i + 1);
      if (currentLevel > 0) // is not root
      {
        newNode.parent = parentStack.back();
        parentStack.back()->children.push_back(&newNode);
      }
      else
      {
        roots.push_back(newNode);
      }
      parentStack.push_back(&newNode);
      currentLevel++;
    }
    else if (stringMatchesString(fileContents, "]", i))
    {
      currentLevel--;
      parentStack.pop_back();
    }
  }

  return roots;
}
*/
int main (int argc, char** argv)
{
  //vector<node> tree = buildTree("sample.tree");;
  //node child;
  //child.text = "child";
  //node root;
  //root.text = "root";
  //root.children.push_back(&child);
  //tree.push_back(root);
  //cout << tree[0].text << endl;
  //cout << tree[0].children[0]->text << endl;

  ifstream ifs ("sample.tree", ifstream::in);
  string fileContents((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

  vector<node*> roots;
  vector<node*> parentStack;

  int currentLevel = 0;
  for (int i = 0; i < fileContents.length(); i++)
  {
    if (stringMatchesString(fileContents, "[", i))
    {
      node* newNode = new node;
      //node newNode;
      newNode->text = getNodeTextFromFile(fileContents, i + 1);
      if (currentLevel > 0) // is not root
      {
        newNode->parent = parentStack.back(); // get last parent
        parentStack.back()->children.push_back(newNode);
      }
      else
      {
        roots.push_back(newNode);
      }
      parentStack.push_back(newNode);
      currentLevel++;
    }
    else if (stringMatchesString(fileContents, "]", i))
    {
      currentLevel--;
      parentStack.pop_back();
    }
  }

  cout << roots[0]->text << endl;
  cout << roots[0]->children[2]->children[0]->text << endl;

  return 0;
}
