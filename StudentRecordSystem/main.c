/* StudentRecordSystem - Full version with extra features
   Compile: gcc main.c -o StudentRecordSystem
   (Works in Code::Blocks on Windows with MinGW)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 100
#define MAX_STUDENTS 2000
#define ENC_KEY 0xAA   // XOR key for simple encryption

typedef struct {
    long long id;
    char name[MAX_NAME];
    int age;
    float marks;
} Student;

/* ---------- Helper: pause / clear input ---------- */
void flush_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* ---------- Login ---------- */
int login() {
    char user[50], pass[50];
    const char *ADMIN_USER = "admin";
    const char *ADMIN_PASS = "admin123";

    printf("===== LOGIN =====\n");
    printf("Username: ");
    if (scanf("%49s", user) != 1) return 0;
    printf("Password: ");
    if (scanf("%49s", pass) != 1) return 0;
    flush_stdin();

    if (strcmp(user, ADMIN_USER) == 0 && strcmp(pass, ADMIN_PASS) == 0) {
        printf("Login successful!\n\n");
        return 1;
    } else {
        printf("Login failed! Username or password wrong.\n\n");
        return 0;
    }
}

/* ---------- Basic file utilities ---------- */
int file_exists(const char *fname) {
    FILE *f = fopen(fname, "rb");
    if (!f) return 0;
    fclose(f);
    return 1;
}

int load_all(Student arr[], int max_count) {
    FILE *fp = fopen("students.dat", "rb");
    if (!fp) return 0;
    int count = 0;
    while (count < max_count && fread(&arr[count], sizeof(Student), 1, fp) == 1) {
        count++;
    }
    fclose(fp);
    return count;
}

int save_all(Student arr[], int count) {
    FILE *fp = fopen("students.dat", "wb");
    if (!fp) return 0;
    for (int i = 0; i < count; ++i) {
        fwrite(&arr[i], sizeof(Student), 1, fp);
    }
    fclose(fp);
    return 1;
}

/* ---------- Add / Display ---------- */
void addStudent() {
    Student s;
    FILE *fp = fopen("students.dat", "ab");
    if (!fp) {
        printf("Error opening file for append!\n");
        return;
    }

    printf("Enter student id: ");
    if (scanf("%lld", &s.id) != 1) { printf("Invalid id.\n"); flush_stdin(); fclose(fp); return; }
    flush_stdin();

    printf("Enter student full name: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = '\0';

    printf("Enter student age: ");
    if (scanf("%d", &s.age) != 1) { printf("Invalid age.\n"); flush_stdin(); fclose(fp); return; }

    printf("Enter student marks: ");
    if (scanf("%f", &s.marks) != 1) { printf("Invalid marks.\n"); flush_stdin(); fclose(fp); return; }
    flush_stdin();

    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);
    printf("\nStudent added successfully!\n");
}

void displayStudents() {
    Student s;
    FILE *fp = fopen("students.dat", "rb");
    if (!fp) {
        printf("No records found.\n");
        return;
    }

    printf("\n------ Student Records ------\n\n");
    int any = 0;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        printf("ID: %lld\n", s.id);
        printf("Name: %s\n", s.name);
        printf("Age: %d\n", s.age);
        printf("Marks: %.2f\n\n", s.marks);
        any = 1;
    }
    if (!any) printf("No records found.\n");
    fclose(fp);
}

/* ---------- Search ---------- */
void searchStudent() {
    Student s;
    FILE *fp = fopen("students.dat", "rb");
    if (!fp) { printf("No record found!\n"); return; }

    long long id;
    int found = 0;
    printf("Enter the student id: ");
    if (scanf("%lld", &id) != 1) { printf("Invalid id.\n"); flush_stdin(); fclose(fp); return; }

    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.id == id) {
            printf("\n--- Student Found ---\n");
            printf("ID: %lld\n", s.id);
            printf("Name: %s\n", s.name);
            printf("Age: %d\n", s.age);
            printf("Marks: %.2f\n", s.marks);
            found = 1;
            break;
        }
    }

    if (!found) printf("\nStudent with ID %lld not found!\n", id);
    fclose(fp);
}

/* ---------- Update ---------- */
void updateStudent() {
    Student s;
    FILE *fp = fopen("students.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    if (!fp || !temp) { printf("Error opening file!\n"); if (fp) fclose(fp); if (temp) fclose(temp); return; }

    long long id;
    int found = 0;
    printf("Enter the student ID to update: ");
    if (scanf("%lld", &id) != 1) { printf("Invalid id.\n"); flush_stdin(); fclose(fp); fclose(temp); return; }
    flush_stdin();

    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.id == id) {
            found = 1;
            printf("\n--- Current Record ---\n");
            printf("ID: %lld\n", s.id);
            printf("Name: %s\n", s.name);
            printf("Age: %d\n", s.age);
            printf("Marks: %.2f\n", s.marks);

            printf("\nEnter NEW name: ");
            fgets(s.name, sizeof(s.name), stdin);
            s.name[strcspn(s.name, "\n")] = '\0';

            printf("Enter NEW age: ");
            if (scanf("%d", &s.age) != 1) { printf("Invalid age.\n"); flush_stdin(); }
            printf("Enter NEW marks: ");
            if (scanf("%f", &s.marks) != 1) { printf("Invalid marks.\n"); flush_stdin(); }
            flush_stdin();

            printf("\nRecord updated successfully!\n");
        }
        fwrite(&s, sizeof(Student), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove("students.dat");
    rename("temp.dat", "students.dat");

    if (!found) printf("\nStudent with ID %lld not found!\n", id);
}

/* ---------- Delete ---------- */
void deleteStudent() {
    Student s;
    FILE *fp = fopen("students.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    if (!fp || !temp) { printf("Error opening file!\n"); if (fp) fclose(fp); if (temp) fclose(temp); return; }

    long long id;
    int found = 0;
    printf("Enter Student ID to delete: ");
    if (scanf("%lld", &id) != 1) { printf("Invalid id.\n"); flush_stdin(); fclose(fp); fclose(temp); return; }
    flush_stdin();

    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.id == id) {
            found = 1;
            printf("\nRecord with ID %lld deleted successfully!\n", id);
            continue;
        }
        fwrite(&s, sizeof(Student), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove("students.dat");
    rename("temp.dat", "students.dat");

    if (!found) printf("\nStudent with ID %lld not found!\n", id);
}

/* ---------- Count ---------- */
void countStudents() {
    Student s;
    FILE *fp = fopen("students.dat", "rb");
    if (!fp) { printf("No records found!\n"); return; }

    int count = 0;
    while (fread(&s, sizeof(Student), 1, fp) == 1) count++;
    fclose(fp);
    printf("\nTotal Students: %d\n", count);
}

/* ---------- Topper ---------- */
void showTopper() {
    Student s, topper;
    FILE *fp = fopen("students.dat", "rb");
    if (!fp) { printf("No records found!\n"); return; }

    int first = 1;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (first) { topper = s; first = 0; }
        else if (s.marks > topper.marks) topper = s;
    }
    fclose(fp);

    if (first) { printf("No records available!\n"); return; }
    printf("\n----- TOPPER -----\n");
    printf("ID: %lld\n", topper.id);
    printf("Name: %s\n", topper.name);
    printf("Age: %d\n", topper.age);
    printf("Marks: %.2f\n", topper.marks);
}

/* ---------- Sort helpers ---------- */
int cmp_id_asc(const void *a, const void *b) {
    const Student *x = a, *y = b;
    if (x->id < y->id) return -1;
    if (x->id > y->id) return 1;
    return 0;
}
int cmp_marks_desc(const void *a, const void *b) {
    const Student *x = a, *y = b;
    if (x->marks < y->marks) return 1;
    if (x->marks > y->marks) return -1;
    return 0;
}
int cmp_name_asc(const void *a, const void *b) {
    const Student *x = a, *y = b;
    return strcasecmp(x->name, y->name);
}
int cmp_age_asc(const void *a, const void *b) {
    const Student *x = a, *y = b;
    return x->age - y->age;
}

/* ---------- Sort and Display functions ---------- */
void sortByID() {
    Student arr[MAX_STUDENTS];
    int n = load_all(arr, MAX_STUDENTS);
    if (n == 0) { printf("No records found!\n"); return; }

    qsort(arr, n, sizeof(Student), cmp_id_asc);
    printf("\n------ Students Sorted by ID ------\n\n");
    for (int i = 0; i < n; ++i) {
        printf("ID: %lld\nName: %s\nAge: %d\nMarks: %.2f\n\n", arr[i].id, arr[i].name, arr[i].age, arr[i].marks);
    }
}

void sortByMarks() {
    Student arr[MAX_STUDENTS];
    int n = load_all(arr, MAX_STUDENTS);
    if (n == 0) { printf("No records found!\n"); return; }

    qsort(arr, n, sizeof(Student), cmp_marks_desc);
    printf("\n------ Students Sorted by Marks (High -> Low) ------\n\n");
    for (int i = 0; i < n; ++i) {
        printf("ID: %lld\nName: %s\nAge: %d\nMarks: %.2f\n\n", arr[i].id, arr[i].name, arr[i].age, arr[i].marks);
    }
}

void sortByName() {
    Student arr[MAX_STUDENTS];
    int n = load_all(arr, MAX_STUDENTS);
    if (n == 0) { printf("No records found!\n"); return; }

    qsort(arr, n, sizeof(Student), cmp_name_asc);
    printf("\n------ Students Sorted by Name (A -> Z) ------\n\n");
    for (int i = 0; i < n; ++i) {
        printf("ID: %lld\nName: %s\nAge: %d\nMarks: %.2f\n\n", arr[i].id, arr[i].name, arr[i].age, arr[i].marks);
    }
}

void sortByAge() {
    Student arr[MAX_STUDENTS];
    int n = load_all(arr, MAX_STUDENTS);
    if (n == 0) { printf("No records found!\n"); return; }

    qsort(arr, n, sizeof(Student), cmp_age_asc);
    printf("\n------ Students Sorted by Age (Low -> High) ------\n\n");
    for (int i = 0; i < n; ++i) {
        printf("ID: %lld\nName: %s\nAge: %d\nMarks: %.2f\n\n", arr[i].id, arr[i].name, arr[i].age, arr[i].marks);
    }
}

/* ---------- Export to text ---------- */
void exportToText() {
    Student s;
    FILE *fp = fopen("students.dat", "rb");
    FILE *txt = fopen("report.txt", "w");
    if (!fp) { printf("No records found!\n"); if (txt) fclose(txt); return; }
    if (!txt) { printf("Cannot create report.txt\n"); fclose(fp); return; }

    fprintf(txt, "------ Student Records Report ------\n\n");
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        fprintf(txt, "ID: %lld\n", s.id);
        fprintf(txt, "Name: %s\n", s.name);
        fprintf(txt, "Age: %d\n", s.age);
        fprintf(txt, "Marks: %.2f\n\n", s.marks);
    }
    fclose(fp);
    fclose(txt);
    printf("Exported records to report.txt successfully.\n");
}

/* ---------- Simple file XOR encryption/decryption ---------- */
int xor_file_bytes(const char *src, const char *dst) {
    FILE *in = fopen(src, "rb");
    if (!in) return 0;
    FILE *out = fopen(dst, "wb");
    if (!out) { fclose(in); return 0; }

    unsigned char buf[4096];
    size_t r;
    while ((r = fread(buf, 1, sizeof(buf), in)) > 0) {
        for (size_t i = 0; i < r; ++i) buf[i] ^= ENC_KEY;
        fwrite(buf, 1, r, out);
    }

    fclose(in);
    fclose(out);
    return 1;
}

void encryptFile() {
    if (!file_exists("students.dat")) { printf("students.dat not found!\n"); return; }
    if (!xor_file_bytes("students.dat", "students.dat.enc")) { printf("Encryption failed!\n"); return; }
    remove("students.dat");
    rename("students.dat.enc", "students.dat");
    printf("File encrypted successfully (simple XOR).\n");
}

void decryptFile() {
    if (!file_exists("students.dat")) { printf("students.dat not found!\n"); return; }
    if (!xor_file_bytes("students.dat", "students.dat.dec")) { printf("Decryption failed!\n"); return; }
    remove("students.dat");
    rename("students.dat.dec", "students.dat");
    printf("File decrypted successfully (simple XOR).\n");
}

/* ---------- Menu ---------- */
void printMenu() {
    printf("\n========== MENU ==========\n");
    printf("1. Add Student Record\n");
    printf("2. Display Student Records\n");
    printf("3. Search Student by ID\n");
    printf("4. Update Student Record\n");
    printf("5. Delete Student Record\n");
    printf("6. Count Students\n");
    printf("7. Show Topper\n");
    printf("8. Sort by ID\n");
    printf("9. Sort by Marks\n");
    printf("10. Sort by Name\n");
    printf("11. Sort by Age\n");
    printf("12. Export to report.txt (text)\n");
    printf("13. Encrypt data file (XOR)\n");
    printf("14. Decrypt data file (XOR)\n");
    printf("15. Exit\n");
    printf("==========================\n");
    printf("Enter your choice: ");
}

int main() {
    if (!login()) return 0;

    int choice;
    while (1) {
        printMenu();
        if (scanf("%d", &choice) != 1) { printf("Invalid input.\n"); flush_stdin(); continue; }
        flush_stdin();

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
            case 6: countStudents(); break;
            case 7: showTopper(); break;
            case 8: sortByID(); break;
            case 9: sortByMarks(); break;
            case 10: sortByName(); break;
            case 11: sortByAge(); break;
            case 12: exportToText(); break;
            case 13: encryptFile(); break;
            case 14: decryptFile(); break;
            case 15: printf("Exiting...\n"); exit(0);
            default: printf("Invalid choice! Try again.\n"); break;
        }
    }

    return 0;
}
