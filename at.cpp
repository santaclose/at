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
  int depth;
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
      newNode->depth = currentLevel;
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

int getMaxDepth(node* someNode)
{
  int maxDepth = someNode->depth;
  for (auto a : someNode->children)
  {
    int childMaxDepth = getMaxDepth(a);
    if (childMaxDepth > maxDepth)
    {
      maxDepth = childMaxDepth;
    }
  }
  return maxDepth;
}

void initializeLeafPositions(node*& someNode, int& counter)
{
  if (someNode->children.size() == 0)//someNode->depth == depth)
  {
    someNode->position = counter;
    counter++;
  }
  for (auto a : someNode->children)
  {
    initializeLeafPositions(a, counter);
  }
}

float getChildPosAverage(node*& someNode)
{
  float result = 0;
  for (auto a : someNode->children)
  {
    result += a->position;
  }
  result /= someNode->children.size();
  return result;
}

void calculatePositionsForLevel(node*& someNode, int level)
{
  if (someNode->depth == level)
  {
    if (someNode->children.size() > 0)
    {
      someNode->position = getChildPosAverage(someNode);
    }
    //else
    //{
    //  someNode->position = counter;
    //  counter++;
    //}
  }
  for (auto a : someNode->children)
  {
    calculatePositionsForLevel(a, level);
  }
}

void calculatePositions(node*& someNode)//, int childIndex)
{
  int maxDepth = getMaxDepth(someNode);
  int counter = 0;
  initializeLeafPositions(someNode, counter);
  int curDepth = maxDepth - 1;
  while (curDepth >= 0)
  {
    calculatePositionsForLevel(someNode, curDepth);
    curDepth--;
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

void getSVGPositions(node*& someNode, float xSep, float ySep, float margin)
{
  someNode->position = someNode->position * xSep + margin;
  someNode->depth = someNode->depth * ySep + margin;

  for (auto a : someNode->children)
  {
    getSVGPositions(a, xSep, ySep, margin);
  }
}

void writeSVGhelper(node* someNode, ofstream& outputFile, float nodeRadius, float topHandlerOffset, float bottomHandlerOffset)
{
  // <path d="M50,300 C50,230 300,270 300,200" stroke="black" fill="none" stroke-width="5" />
  outputFile << "<ellipse cx=\"" << someNode->position << "\" cy=\"" << someNode->depth << "\" rx=\"" << nodeRadius << "\" ry=\"" << nodeRadius << "\" stroke=\"black\" fill=\"none\" stroke-width=\"1\" />\n";
  for (auto a : someNode->children)
  {
    outputFile << "<path d=\"M" << a->position << ',' << a->depth - nodeRadius << " C" << a->position << ',' << a->depth - nodeRadius - bottomHandlerOffset << ' ' << someNode->position << ',' << someNode->depth + nodeRadius + topHandlerOffset << ' ' << someNode->position << ',' << someNode->depth + nodeRadius << "\" stroke=\"black\" fill=\"none\" stroke-width=\"1\" />\n";
    writeSVGhelper(a, outputFile, nodeRadius, topHandlerOffset, bottomHandlerOffset);
  }
}

void writeSVG(node* root, string output, float width, float height, float nodeRadius, float topHandlerOffset, float bottomHandlerOffset)
{
  // <svg width="400" height="400">
  // <ellipse cx="20" cy="20" rx=".5" ry=".5" stroke="black" fill="none" stroke-width="5" />
  // </svg>

  ofstream outputFile;
  outputFile.open ("output.svg");
  outputFile << "<svg width=\"" << width << "\" height=\"" << height << "\">\n";

  writeSVGhelper(root, outputFile, nodeRadius, topHandlerOffset, bottomHandlerOffset);

  outputFile << "</svg>";
  outputFile.close();
}

int main (int argc, char** argv)
{
  vector<node*> roots = buildTree("sample.tree");

  calculatePositions(roots[0]);
  getSVGPositions(roots[0], 35, 50, 70);

  printTree(roots[0], 0);
  //cout << (float)clusterSize(roots[0]->children[1])/2;

  writeSVG(roots[0], "output.svg", 800, 500, 10, 15, 15);

  return 0;
}
