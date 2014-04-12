#include "database.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
	char name[256];
	int age;
} person_t;

int main(int argc, char** argv) 
{
sqlite3* db;

db = database_open(argv[1], 0);

if(db == NULL) {
    printf("Error: %s\n", database_get_last_error(db));
    exit(1);
}


char* query = "CREATE TABLE PEOPLE(id INT, name TEXT PRIMARY KEY, data BLOB, size INT)";
int res = database_execute(db, query);

if(res == -1) {
	 printf("Error: %s\n", database_get_last_error(db));
     exit(1);
}

//query = "INSERT INTO PEOPLE(name, data) VALUES(?, ?)";

/*char* name = "Chol Nhial";
int age = 18;
database_generic_insert(db, query, 2, TEXT, name, 1, strlen(name), INT, age, 2, 0);
*/
/*char* name;
int age;
char* query = "SELECT * FROM PEOPLE WHERE name = ?";
database_generic_get(db, query, "Chol Nhial", 2, TEXT, &name, 0, INT, &age, 1);
printf("Name: %s\n", name);
printf("Age: %d\n", age);
*/
person_t person[2];
strcpy(person[0].name, "Chol Nhial");
strcpy(person[1].name, "Lual Nhial");

person[0].id = 1;
person[0].age = 18;
person[1].age = 13;
person[1].id = 2;

printf("Writing to database...\n");
query = "INSERT INTO PEOPLE(name, data) VALUES(?, ?)";
database_generic_insert(db, query, 2, BLOB, &person, 2, sizeof(person_t)*2, TEXT, person[0].name, 1, strlen(person[0].name) + 1);
printf("Reading from database...\n");
person_t* person2;
query = "SELECT * FROM PEOPLE WHERE id = ";
int size;
database_generic_get(db, query, "1", 1, BLOB, &person2, &size, 1);


for(int i = 0; i < size/sizeof(person_t); i++) {
	printf("Name: %s\n", person2->name);
	printf("Age: %d\n", person2->age);
	person2++;
}

database_close(db);
}
