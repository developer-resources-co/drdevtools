
#define NSYMS (20)

struct symtab
{
  char* name;
  long (*funcptr)();
  long value;
} symtab[ NSYMS ];

struct symtab* symlook();











