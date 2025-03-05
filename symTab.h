

struct symTab{

};

struct symEntry{

};


struct symTab *root;

int add(char *elem, void *type);
struct symEntry *contains(char *elem);


int hash(char *elem);
int check(char *elem);