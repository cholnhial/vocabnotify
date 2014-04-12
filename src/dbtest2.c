#include "database.h"
#include <stdlib.h>


int main(int argc, char** argv)
{

sqlite3* db = database_open(argv[1], FALSE);

char* query = "CREATE TABLE PEOPLE(id INT PRIMARY KEY, name TEXT, age INT)";

/*int res = database_execute(db, query);

if(res == -1) {
	 printf("Error: %s\n", database_get_last_error(db));
     exit(1);
}

query = "INSERT INTO PEOPLE(id, name, age) VALUES(1, 'Chol Nhial Chol', 19)";

res = database_execute(db, query);
if(res == -1) {
	 printf("Error: %s\n", database_get_last_error(db));
     exit(1);
}
*/
char* name;

query = "SELECT count(*) FROM PEOPLE";

int table_size;
int res;
res = database_generic_get(db, query, NULL, 1, INT, &table_size, 0);

if(res == -1) {
	 printf("Error1: %s\n", database_get_last_error(db));
     exit(1);
}

    char* equery = malloc(250);
    for(int i = 1; i <= table_size; i++) {
        int age;
        char* name;
        int id;
    
     sprintf(equery, "SELECT * FROM PEOPLE WHERE oid=%d", i);
     res = database_generic_get(db, equery, NULL, 3, INT, &id, 0, TEXT, &name, 1, INT, &age, 2);

    if(res == -1) {
	 printf("Error1: %s\n", database_get_last_error(db));
     exit(1);
    }
     
    printf("ID: %d\n", id);
    printf("NAME: %s\n", name);
    printf("AGE: %d\n", age);

    }
}
