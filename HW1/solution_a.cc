#include <iostream>

using namespace std;

class TreeNode
{
  public:

    TreeNode
    (
      char value, // to make template
      int  depth
    );

    // return TreeNodes or pointers or reference ?
    std::vector< TreeNode > Expand // to make template
    (
      void
    );
 
  public:

    char value; // to make template
    int  depth;
};

void GeneratePasswords
(
  int length
)
{
  // init LIFO with an empty node

  TreeNode * currNode = new TreeNode( "", 0 );

  /*
   * While some_condition do
   *  curr_node
   * FinWhile
   */
}

int main
(
  int     argc,
  char ** argv
)
{
  cout << "Hello world" << endl;

  return 0;
}
