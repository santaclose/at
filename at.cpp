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

vector<node*> buildTree(string filePath, bool& fileNotFound)
{
  vector<node*> roots;

  ifstream ifs (filePath, ifstream::in);
  if (ifs.fail())
  {
    fileNotFound = true;
    return roots;
  }

  string fileContents((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

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

int getMaxDepth(vector<node*>& roots)
{
  int maxDepth = 0;
  for (auto a : roots)
  {
    int cur = getMaxDepth(a);
    if (cur > maxDepth)
    {
      maxDepth = cur;
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
  }
  for (auto a : someNode->children)
  {
    calculatePositionsForLevel(a, level);
  }
}

void fixPositions(node*& someNode, float xSep, float ySep, float xMargin, float yMargin)
{
  someNode->position = someNode->position * xSep + xMargin;
  someNode->depth = someNode->depth * ySep + yMargin;

  for (auto a : someNode->children)
  {
    fixPositions(a, xSep, ySep, xMargin, yMargin);
  }
}

float getMaxPosition(node*& root)
{
  if (root->children.size() > 0)
  {
    return getMaxPosition(root->children[root->children.size() - 1]);
  }
  else
  {
    return root->position;
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

void calculatePositions(vector<node*>& roots)
{
  float curOffset = 0;
  for (auto a : roots)
  {
    calculatePositions(a);
    fixPositions(a, 1, 1, curOffset, 0);
    curOffset = getMaxPosition(a) + 1;
  }
}

void printTree(node* root, int level)
{
  for (int i = 0; i < level; i++)
  {
    cout << "  ";
  }
  cout << root->text << ": " << root->position << ", " << root->depth << endl;

  for (auto a : root->children)
  {
    printTree(a, level + 1);
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

void writeSVG(vector<node*>& roots, string output, float width, float height, float nodeRadius, float topHandlerOffset, float bottomHandlerOffset)
{
  // <svg width="400" height="400">
  // <ellipse cx="20" cy="20" rx=".5" ry=".5" stroke="black" fill="none" stroke-width="5" />
  // </svg>

  ofstream outputFile;
  outputFile.open ("output.svg");
  outputFile << "<svg width=\"" << width << "\" height=\"" << height << "\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink= \"http://www.w3.org/1999/xlink\">\n";

  for (auto a : roots)
  {
    writeSVGhelper(a, outputFile, nodeRadius, topHandlerOffset, bottomHandlerOffset);
  }

  outputFile << "</svg>";
  outputFile.close();
}

int main (int argc, char** argv)
{
  if (argc == 1)
  {
    cout << "usage:\n    at path" << endl;
    cout << "    at path [options]" << endl;
    cout << "options:\n[m] margin\n[x] xSep\n[y] ySep\n[n] nodeRadius\n[b] bottomHandlerOffset\n[t] topHandlerOffset\n";
    return 0;
  }
  else
  {
    float margin = 50;
    float xSep = 40;
    float ySep = 60;
    float nodeRadius = 5;
    float bottomHandlerOffset = 15;
    float topHandlerOffset = 30;

    if (argc % 2 == 1)
    {
      cout << "invalid arguments\n";
    }
    int i = 2;
    while (i < argc)
    {
      if (argv[i][0] == 'm')
      {
        margin = atof(argv[i + 1]);
      }
      else if (argv[i][0] == 'x')
      {
        xSep = atof(argv[i + 1]);
      }
      else if (argv[i][0] == 'y')
      {
        ySep = atof(argv[i + 1]);
      }
      else if (argv[i][0] == 'n')
      {
        nodeRadius = atof(argv[i + 1]);
      }
      else if (argv[i][0] == 'b')
      {
        bottomHandlerOffset = atof(argv[i + 1]);
      }
      else if (argv[i][0] == 't')
      {
        topHandlerOffset = atof(argv[i + 1]);
      }
      i += 2;
    }

    bool fileNotFound = false;
    vector<node*> roots = buildTree(argv[1], fileNotFound);
    if (fileNotFound)
    {
      cout << "file not found\n";
      return -1;
    }

    calculatePositions(roots);
    for (auto a : roots)
    {
      fixPositions(a, xSep, ySep, margin, margin);
    }

    float canvas[2];
    canvas[0] = getMaxPosition(roots[roots.size()-1]) + margin;
    canvas[1] = getMaxDepth(roots) + margin;

    writeSVG(roots, "output.svg", canvas[0], canvas[1], nodeRadius, topHandlerOffset, bottomHandlerOffset);

    return 0;
  }
}
