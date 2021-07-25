# sysy-compiler-skeleton

Skeleton of a compiler from SysY to Eeyore or Tigger that contains some essential files.

# Contents

+ example.l & example.y
  
  An example of Flex and Bison files that handles file dependencies, line number counting, e.t.c.
  
+ lambda_visitor.h & variant_printer.h

  The utility files for std::variant.

+ sysy_type.h & sysy_type.cc

  The definition of all the types in SysY, including void, int, array, pointer and function types. Useful in ASTs and symbol tables.
  
+ eeyore.h & eeyore.cc

  The Eeyore statement definitions. Also provides printing methods of these statements through std::ostream.
  
+ tigger.h & tigger.cc

  The Tigger statement definitions and printing methods.

+ bitmap.h & bitmap.cc

  A bitmap implementation that can be used as a util for dataflow analysis.

 \*Note that Eeyore and Tigger (and even SysY) are subject to change. You may have to modify the files as necessary before using them.
