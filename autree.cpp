#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <algorithm>
using namespace std;

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

static tree getTree(string filePath)
{
    ifstream ifs (filePath, ifstream::in);
    string fileContents((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

    tree theTree;

    // get height and get node count
    theTree.height = 0;
    theTree.nodeCount = 0;
    int curHeight = 0;
    for (int i = 0; i < fileContents.length(); i++)
    {
      if (stringMatchesString(fileContents, "[", i))
      {
        theTree.nodeCount++;
        curHeight++;
        if (curHeight > theTree.height)
        {
          theTree.height = curHeight;
        }
      }
      else if (stringMatchesString(fileContents, "]", i))
      {
        curHeight--;
      }
    }

    /*theTree.nodeCountPerRow = new int[theTree.height];
    for (int i = 0; i < theTree.height; i++)
    {
      theTree.nodeCountPerRow[i] = 0;
    }
    int currentLevel = -1;
    for (int i = 0; i < fileContents.length(); i++)
    {
      if (stringMatchesString(fileContents, "[", i))
      {
        currentLevel++;
        theTree.nodeCountPerRow[currentLevel]++;
      }
      else if (stringMatchesString(fileContents, "]", i))
      {
        currentLevel--;
      }
    }

    cout << "nodeCountPerRow:" << endl;
    for (int i = 0; i < theTree.height; i++)
    {
      cout << theTree.nodeCountPerRow[i] << endl;
    }*/

    // fill string verctor
    int aux = 0;
    bool readingString = false;
    for (int i = 0; i < fileContents.length(); i++)
    {
      if (!readingString && stringMatchesString(fileContents, "[", i))
      {
        aux = i + 1;
        readingString = true;
      }
      else if (readingString && (stringMatchesString(fileContents, "]", i) || stringMatchesString(fileContents, "\n", i) || stringMatchesString(fileContents, "[", i)))
      {
        readingString = false;
        theTree.nodeString.push_back(fileContents.substr(aux, i - aux));
      }
    }

    curHeight = 0;
    int childCountPerNode[theTree.nodeCount];
    for (int i = 0; i < fileContents.length(); i++)
    {
      if (stringMatchesString(fileContents, "[", i))
      {
        if (curHeight > 0)
        {
          
        }
        curHeight++;
      }
    }


    cout << theTree.height;
    return theTree;
}

int main (int argc, char** argv)
{
  tree fileTree = getTree("sample.tree");
  return 0;
}
