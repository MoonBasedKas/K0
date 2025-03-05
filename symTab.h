

struct symTab{
    char *s; // uhhh nice?
    struct symTab *parent;
    struct SymEntry **buckets;
};

struct symEntry{
    char *name;
    void *type;
    struct symTab *parent;  
};


struct symTab *root;

int add(char *elem, void *type);
struct symEntry *contains(char *elem);

struct symTab *createTable(); // call calloc 
int hash(char *elem);
int check(char *elem);