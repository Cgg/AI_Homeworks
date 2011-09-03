#include <iostream>
#include <vector>
#include <stack>

using namespace std;


template< class T, class Expander > class TreeNode
{
  public:

    TreeNode
    (
      T   value,
      int depth
    );

    int Depth
    (
      void
    );

    T Value
    (
      void
    );

    std::vector< TreeNode< T, Expander > > Expand
    (
      void
    );
 
    friend std::vector< TreeNode< T, Expander > > 
           Expander::Expand( TreeNode< T, Expander > * node );

  public:

    T   value;
    int depth;
};


class StringExpanderHinter
{
  public:
  
    static std::vector< TreeNode< string, StringExpanderHinter > > Expand
    (
      TreeNode< string, StringExpanderHinter > * node
    );
};


/******************************************************************************
 * Class TreeNode implementation
 ******************************************************************************/
template< class T, class Expander >
TreeNode< T, Expander >::TreeNode
(
  T   value,
  int depth
):
  value( value ), depth( depth )
{
}


template< class T, class Expander >
int TreeNode< T, Expander >::Depth
(
  void
)
{
  return depth;
}


template< class T, class Expander >
T TreeNode< T, Expander >::Value
(
  void
)
{
  return value;
}


template< class T, class Expander >
vector< TreeNode< T, Expander > > TreeNode< T, Expander >::Expand
(
  void
)
{
  return Expander::Expand( this );
}


/******************************************************************************
 * Class StringExpanderHinter implementation
 ******************************************************************************/
vector< TreeNode< string, StringExpanderHinter > > StringExpanderHinter::Expand
(
  TreeNode< string, StringExpanderHinter > * node
)
{
  typedef TreeNode< string, StringExpanderHinter > TN;
  
  vector< TN > result;

  int nextDepth = node->depth++;

  if( nextDepth == 1 ) // first letter of passw is I or E
  {
    result.push_back( TN( string( 1, 'I' ), nextDepth ) );
    result.push_back( TN( string( 1, 'E' ), nextDepth ) );
  }
  else    
  {
    string currPass = node->value;

    switch( currPass[ currPass.length() - 1 ] )
    {
      case 'A':
        result.push_back( TN( currPass + 'D', nextDepth ) );
        result.push_back( TN( currPass + 'E', nextDepth ) );
        break;

      case 'G': // everything but A
        for( int i = 'B'; i <= 'I' ; i++ ) // ARGH this is ugly xD
          result.push_back( TN( currPass + char(i), nextDepth ) );
        break;

      default: // everything
        for( int i = 'A'; i <= 'I' ; i++ ) // ARGH this is ugly xD
          result.push_back( TN( currPass + char(i), nextDepth ) );
        break;
    }
  }

  return result;
}


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
