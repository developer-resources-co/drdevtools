/*============================================================================*/
/* random.c: cheap pseudo-random # generator from Craig						  */
/*============================================================================*/

static unsigned long int next;

int Random(int limit)
{
    next = next * 1103515245 + 12345;
    return((unsigned int) (next/262144) % limit);
}

int rand(void)
{
    next = next * 1103515245 + 12345;
    return((unsigned int) (next/262144) % 32768);
}

void srand(unsigned int seed)
{
    next = seed;
}

/*============================================================================*/
