#include "Palette.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

struct Palette16
{
    int numColours;
    // these are stored in numerical order to speed up comparisons
    uint16_t colours[PALETTE16_NUM_COLOURS];
};

struct Palette16 *Palette16_New(void)
{
    struct Palette16 *p = malloc(sizeof(struct Palette16));
    p->numColours = 0;

    return p;
}

void Palette16_Free(struct Palette16 *palette)
{
    free(palette);
}

int Palette16_AddColour(struct Palette16 *palette, uint16_t colour)
{
    for (int i = 0; i < palette->numColours; i++)
    {
        // already contains this colour
        if (palette->colours[i] == colour)
        {
            return palette->numColours;
        }

        if (palette->colours[i] > colour)
        {
            // need to insert colour here
            if (palette->numColours == PALETTE16_NUM_COLOURS)
            {
                return -1;
            }

            for (int j = palette->numColours; j > i; j--)
            {
                palette->colours[j] = palette->colours[j - 1];
            }

            palette->colours[i] = colour;
            palette->numColours++;
            return palette->numColours;
        }
    }

    // need to add this colour to the end
    if (palette->numColours == PALETTE16_NUM_COLOURS)
    {
        // no space
        return -1;
    }

    palette->colours[palette->numColours] = colour;
    palette->numColours++;

    return palette->numColours;
}

uint16_t Palette16_GetColour(struct Palette16 *palette, int i)
{
    assert(0 <= i && i < palette->numColours);
    return palette->colours[i];
}

int Palette16_GetNumColours(struct Palette16 *palette)
{
    return palette->numColours;
}

int Palette16_Contains(struct Palette16 *first, struct Palette16 *second)
{
    int retIfContains = Palette16_GetNumColours(first) < Palette16_GetNumColours(second) ? 1 : -1;
    struct Palette16 *longer = Palette16_GetNumColours(first) < Palette16_GetNumColours(second) ? second : first;
    struct Palette16 *shorter = Palette16_GetNumColours(first) < Palette16_GetNumColours(second) ? first : second;

    int i = 0, j = 0;
    while (1)
    {
        if (Palette16_GetNumColours(shorter) == i)
        {
            return retIfContains;
        }

        if (Palette16_GetNumColours(longer) == j)
        {
            return 0;
        }

        if (Palette16_GetColour(shorter, i) == Palette16_GetColour(longer, j))
        {
            i++;
            j++;
            continue;
        }

        j++;
    }
}

bool Palette16_HasColour(struct Palette16 *palette, uint16_t colour)
{
    for (int i = 0; i < Palette16_GetNumColours(palette); i++)
    {
        if (Palette16_GetColour(palette, i) == colour)
        {
            return true;
        }
    }

    return false;
}

#ifdef TEST

#define DOTEST                                  \
    {                                           \
        struct Palette16 *p1 = Palette16_New(); \
        struct Palette16 *p2 = Palette16_New();
#define ENDTEST         \
    Palette16_Free(p1); \
    Palette16_Free(p2); \
    }

int main()
{
    DOTEST
    assert(Palette16_AddColour(p1, 1) == 1);
    assert(Palette16_AddColour(p1, 2) == 2);
    assert(Palette16_AddColour(p1, 2) == 2);

    assert(Palette16_GetColour(p1, 0) == 1);
    assert(Palette16_GetColour(p1, 1) == 2);
    ENDTEST

    DOTEST
    assert(Palette16_AddColour(p1, 2) == 1);
    assert(Palette16_AddColour(p1, 1) == 2);
    assert(Palette16_AddColour(p1, 2) == 2);

    assert(Palette16_GetColour(p1, 0) == 1);
    assert(Palette16_GetColour(p1, 1) == 2);
    ENDTEST

    DOTEST
    Palette16_AddColour(p1, 1);
    Palette16_AddColour(p2, 1);

    assert(Palette16_Contains(p1, p2) == -1);
    ENDTEST

    DOTEST
    Palette16_AddColour(p1, 1);
    Palette16_AddColour(p2, 1);
    Palette16_AddColour(p2, 2);

    assert(Palette16_Contains(p1, p2) == 1);
    ENDTEST

    DOTEST
    Palette16_AddColour(p1, 1);
    Palette16_AddColour(p1, 2);
    Palette16_AddColour(p2, 1);
    Palette16_AddColour(p2, 2);
    Palette16_AddColour(p2, 3);

    assert(Palette16_Contains(p1, p2) == 1);
    ENDTEST

    DOTEST
    Palette16_AddColour(p1, 1);
    Palette16_AddColour(p1, 3);
    Palette16_AddColour(p2, 1);
    Palette16_AddColour(p2, 2);
    Palette16_AddColour(p2, 3);

    assert(Palette16_Contains(p1, p2) == 1);
    ENDTEST

    DOTEST
    Palette16_AddColour(p1, 1);
    Palette16_AddColour(p1, 4);
    Palette16_AddColour(p2, 1);
    Palette16_AddColour(p2, 2);
    Palette16_AddColour(p2, 3);

    assert(Palette16_Contains(p1, p2) == 0);
    ENDTEST

    DOTEST
    Palette16_AddColour(p1, 1);
    Palette16_AddColour(p1, 4);
    Palette16_AddColour(p2, 1);
    Palette16_AddColour(p2, 2);
    Palette16_AddColour(p2, 3);
    Palette16_AddColour(p2, 4);

    assert(Palette16_Contains(p1, p2) == 1);
    ENDTEST
}

#endif