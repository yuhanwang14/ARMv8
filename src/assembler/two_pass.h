typedef struct {
    char *key; // label
    int value; // corresponding offset
} Pair;

// an array based map
typedef struct {
    Pair **pairs;
    int used;
    int size;
} Map;

// sets a local variable so we don't have to pass it every time
void set_input(char *path);

// counts the lines and collects the labels
unsigned first_pass();

// split lines and read into the given buffer
void read_into(char **buf);

// substitute all the labels into literals
void substitute_labels(char **lines);
