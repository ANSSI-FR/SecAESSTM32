#include "helpers.h"
#include "string.h"


#if __GNUC__
# pragma GCC push_options
# pragma GCC optimize("O0")
#endif
#if __clang__
# pragma clang optimize off
#endif
void sleep_intern(uint8_t length)
{
  int i = 0, j = 0;
  int time_value = (1 << (length * 2));
  for (i=0; i<time_value; i++) {
    for(j=0; j<time_value; j++);
  }
}
#if __clang__
# pragma clang optimize on
#endif
#if __GNUG__
# pragma GCC pop_options
#endif
