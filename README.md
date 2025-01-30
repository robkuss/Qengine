# Qengine
I hope this will be worth the struggle

### File header order (please stick to this for consistency):
1. 
   + for *.h files: #pragma once
   + for *.cpp files: *.h file for this file
2. "using namespace" definitions
3. standard library includes with <...>
4. user-defined header includes with "..."
5. forward decl. of classes
6. forward decl. of structs
7. defines
8. macros
9. constant decl.
10. global variable decl.
11. static variable inits
12. 2 newlines before the first function, class, struct, enum etc.

As an example, refer to [UIElement.h](src/graphics/ui/UIElement.h)