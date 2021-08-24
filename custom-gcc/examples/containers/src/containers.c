#include "win32.h"
#include "list.h"
#include "map.h"

void _start(void) {
    // List example
    List *names = List_New(8);

    List_Add(names, "Bastiaan");
    List_Add(names, "Sander");
    List_Add(names, "Leonard");
    List_Add(names, "Jiska");

    List_Insert(names, 0, "Willem");
    List_Insert(names, 1, "Wietske");

    List_Set(names, 10, "Jan");

    printf("# Names:\n");
    List_Foreach(names, char *name, {
        if (name != NULL) {
            printf("Hello %s!\n", name);
        } else {
            printf("null\n");
        }
    })

    List_Free(names, NULL);

    // Map example
    Map *person = Map_New(8);
    Map_Set(person, "name", "Jan de Man");
    Map_Set(person, "age", "25");
    Map_Set(person, "address", "Steenstraat 27");
    Map_Set(person, "city", "Gouda");
    Map_Set(person, "country", "Netherlands");

    Map_Set(person, "city", "Rotterdam");
    Map_Remove(person, "age");

    printf("\n# Person map:\n");
    Map_Foreach(person, char *key, char *value, {
        printf("%s = %s\n", key, value);
    })

    Map_Free(person, NULL);

    ExitProcess(0);
}
