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
  float position;
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

vector<node*> buildTree(string filePath)
{
  ifstream ifs (filePath, ifstream::in);
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
        newNode->parent = NULL;
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
  return roots;
}

int clusterSize(node* root) // x axis size
{
  if (root->children.size() == 0)
  {
    return 1;
  }
  else
  {
    int res = 0;
    for (auto a : root->children)
    {
      res += clusterSize(a);
    }
    return res;
  }
}


float basicChildOffset(int childrenCount, int childIndex)
{
  return childIndex - ((float) childrenCount / 2 - 0.5);
}

void calculatePositions(node*& someNode)//, int childIndex)
{
  if (someNode->parent == NULL) // is root
  {
    someNode->position = 0;
  }

  if (someNode->children.size() >= 2)
  {
    int cursorA = someNode->children.size()/2;
    int cursorB = cursorA + 1;

    float displacementA = 0;
    float displacementB;
    bool firstIteration = true;

    while (cursorA >= 0)
    {
      float halfClusterSize = (float)clusterSize(someNode->children[cursorA])/2;
      displacementA += halfClusterSize;
      if (firstIteration) // saving offset for the right side in first iteration
      {
        displacementB = halfClusterSize;
        //cout << "middle halfClusterSize: " << halfClusterSize << endl;
        firstIteration = false;
      }
      someNode->children[cursorA]->position = /*basicChildOffset(someNode->children.size(), cursorA) + */someNode->position - displacementA;
      cursorA--;
    }
    while (cursorB < someNode->children.size())
    {
      someNode->children[cursorB]->position = /*basicChildOffset(someNode->children.size(), cursorB) + */someNode->position + displacementB;
      float halfClusterSize = (float)clusterSize(someNode->children[cursorB])/2;
      displacementB += halfClusterSize;
      cursorB++;
    }
  }
  else if (someNode->children.size() == 1) // has one child
  {
    someNode->children[0]->position = someNode->position;
  }
  //else
  //{
    //someNode->position = basicChildOffset(someNode->parent->children.size(), childIndex) * ((float)clusterSize(someNode)/2);
  //}
  //int index = 0;
  for (auto a : someNode->children)
  {
    calculatePositions(a);//, index);
    //index++;
  }
}

void printTree(node* root, int level)
{
  for (int i = 0; i < level; i++)
  {
    cout << "  ";
  }
  cout << root->text << ": " << root->position << endl;

  for (auto a : root->children)
  {
    printTree(a, level + 1);
  }
}

int main (int argc, char** argv)
{
  vector<node*> roots = buildTree("sample.tree");

  calculatePositions(roots[0]);

  printTree(roots[0], 0);
  //cout << (float)clusterSize(roots[0]->children[1])/2;

  return 0;
}
