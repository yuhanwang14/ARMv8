typedef struct {
    char *key; // label
    int value; // corresponding offset
} Pair;

typedef struct {
    Pair **pairs;
    int used;
    int size;
} Map;

void set_input(char *path);

unsigned first_pass();

void read_into(char **buf);

void substitute_labels(char **lines);
