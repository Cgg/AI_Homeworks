#include <cstdlib>
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

  protected:

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
)
{
  this->value = value;
  this->depth = depth;
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

  int nextDepth = node->depth + 1;

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


/******************************************************************************
 * other functions' implementations
 ******************************************************************************/
int GeneratePasswords
(
  int length
)
{
  int result = 0;

  typedef TreeNode< string, StringExpanderHinter > TN;

  // we use the depth-limited DFS implemented with a LIFO 
  stack< TN > tree;

  TN * currNode;

  vector< TN > children;

  // init tree with empty node
  tree.push( TN( string(), 0 ) );

  while( !tree.empty() )
  {
    currNode = &( tree.top() );

    if( currNode->Depth() < length )
    {
      children = currNode->Expand();

      tree.pop();

      for( int i = 0 ; i < children.size() ; i++ )
        tree.push( children[ i ] );
    }
    else
    {
      result++;

      tree.pop();
    }
  }

   return result;
}


int main
(
  int     argc,
  char ** argv
)
{
  int n_pass =  GeneratePasswords( atoi( argv[ 1 ] ) );

  // print the number of found passwords
  cout << n_pass << endl;

  return 0;
}
