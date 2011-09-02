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
  vector< string > passwords;

  // we use the depth-limited DFS implemented with a LIFO 
  // stack< TreeNode > tree;
  // TreeNode currNode
  // vector< TreeNode > children

  /* Algo
   *
   * init tree with empty node
   *
   * While not tree.empty() do
   *  curr_node = tree.top
   *  
   *  tree.pop
   *
   *  if curr_node.depth < lenght
   *   children = curr_node.Expand()
   *   tree += children // pseudo-code :)
   *  else
   *   password += currNode.value
   *  fi
   *
   * EndWhile
   */

   // print all the passwords
   vector< string >::const_iterator pw_it;

   for( pw_it = passwords.begin() ; pw_it != passwords.end() ; pw_it++ )
   {
     cout << *pw_it << endl;
   }
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
