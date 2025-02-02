#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> //c library for system call file routines
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

// database include files
#include "db.h"
#include "sdbsc.h"

/*
 *  open_db
 *      dbFile:  name of the database file
 *      should_truncate:  indicates if opening the file also empties it
 *
 *  returns:  File descriptor on success, or ERR_DB_FILE on failure
 *
 *  console:  Does not produce any console I/O on success
 *            M_ERR_DB_OPEN on error
 *
 */
int open_db(char *dbFile, bool should_truncate)
{
    // Set permissions: rw-rw----
    // see sys/stat.h for constants
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

    // open the file if it exists for Read and Write,
    // create it if it does not exist
    int flags = O_RDWR | O_CREAT;

    if (should_truncate)
        flags += O_TRUNC;

    // Now open file
    int fd = open(dbFile, flags, mode);

    if (fd == -1)
    {
        // Handle the error
        printf(M_ERR_DB_OPEN);
        return ERR_DB_FILE;
    }

    return fd;
}

int get_student(int fd, int id, student_t *s) {
    int offset = id * STUDENT_RECORD_SIZE;
    if (lseek(fd, offset, SEEK_SET) == -1) {
        printf(M_ERR_DB_READ);
        return ERR_DB_FILE;
    }
    if (read(fd, s, STUDENT_RECORD_SIZE) != STUDENT_RECORD_SIZE) {
        printf(M_ERR_DB_READ);
        return ERR_DB_FILE;
    }
    if (s->id == 0) {
        return SRCH_NOT_FOUND;
    }
    return NO_ERROR;
}


int add_student(int fd, int id, char *fname, char *lname, int gpa) {
    student_t student = {id, "", "", gpa};
    strncpy(student.fname, fname, sizeof(student.fname) - 1);
    strncpy(student.lname, lname, sizeof(student.lname) - 1);

    int offset = id * STUDENT_RECORD_SIZE;
    if (lseek(fd, offset, SEEK_SET) == -1) {
        printf(M_ERR_DB_WRITE);
        return ERR_DB_FILE;
    }

    student_t existing;
    if (read(fd, &existing, STUDENT_RECORD_SIZE) == STUDENT_RECORD_SIZE && existing.id != 0) {
        printf(M_ERR_DB_ADD_DUP, id);
        return ERR_DB_OP;
    }

    if (lseek(fd, offset, SEEK_SET) == -1) {
        printf(M_ERR_DB_WRITE);
        return ERR_DB_FILE;
    }

    if (write(fd, &student, STUDENT_RECORD_SIZE) != STUDENT_RECORD_SIZE) {
        printf(M_ERR_DB_WRITE);
        return ERR_DB_FILE;
    }

    printf(M_STD_ADDED, id);
    return NO_ERROR;
}


int del_student(int fd, int id) {
    student_t student;
    int rc = get_student(fd, id, &student);
    if (rc == SRCH_NOT_FOUND) {
        printf(M_STD_NOT_FND_MSG, id);
        return ERR_DB_OP;
    } else if (rc != NO_ERROR) {
        return rc;
    }

    int offset = id * STUDENT_RECORD_SIZE;
    if (lseek(fd, offset, SEEK_SET) == -1) {
        printf(M_ERR_DB_WRITE);
        return ERR_DB_FILE;
    }

    if (write(fd, &EMPTY_STUDENT_RECORD, STUDENT_RECORD_SIZE) != STUDENT_RECORD_SIZE) {
        printf(M_ERR_DB_WRITE);
        return ERR_DB_FILE;
    }

    printf(M_STD_DEL_MSG, id);
    return NO_ERROR;
}

/*
 *  count_db_records
 *      fd:     linux file descriptor
 *
 *  Counts the number of records in the database.  Start by reading the
 *  database at the beginning, and continue reading individual records
 *  until you it EOF.  EOF is when the read() syscall returns 0. Check
 *  if a slot is empty or previously deleted by investigating if all of
 *  the bytes in the record read are zeros - I would suggest using memory
 *  compare memcmp() for this. Create a counter variable and initialize it
 *  to zero, every time a non-zero record is read increment the counter.
 *
 *  returns:  <number>       returns the number of records in db on success
 *            ERR_DB_FILE    database file I/O issue
 *            ERR_DB_OP      database operation logically failed (aka student
 *                           not in database)
 *
 *
 *  console:  M_DB_RECORD_CNT  on success, to report the number of students in db
 *            M_DB_EMPTY       on success if the record count in db is zero
 *            M_ERR_DB_READ    error reading or seeking the database file
 *            M_ERR_DB_WRITE   error writing to db file (adding student)
 *
 */
int count_db_records(int fd)
{
    // TODO
    printf(M_NOT_IMPL);
    return NOT_IMPLEMENTED_YET;
}

/*
 *  print_db
 *      fd:     linux file descriptor
 *
 *  Prints all records in the database.  Start by reading the
 *  database at the beginning, and continue reading individual records
 *  until you it EOF.  EOF is when the read() syscall returns 0. Check
 *  if a slot is empty or previously deleted by investigating if all of
 *  the bytes in the record read are zeros - I would suggest using memory
 *  compare memcmp() for this. Be careful as the database might be empty.
 *  on the first real row encountered print the header for the required output:
 *
 *     printf(STUDENT_PRINT_HDR_STRING, "ID",
 *                  "FIRST NAME", "LAST_NAME", "GPA");
 *
 *  then for each valid record encountered print the required output:
 *
 *     printf(STUDENT_PRINT_FMT_STRING, student.id, student.fname,
 *                    student.lname, calculated_gpa_from_student);
 *
 *  The code above assumes you are reading student records into a local
 *  variable named student that is of type student_t. Also dont forget that
 *  the GPA in the student structure is an int, to convert it into a real
 *  gpa divide by 100.0 and store in a float variable.
 *
 *  returns:  NO_ERROR       on success
 *            ERR_DB_FILE    database file I/O issue
 *
 *
 *  console:  <see above>      on success, print table or database empty
 *            M_ERR_DB_READ    error reading or seeking the database file
 *
 */
int print_db(int fd)
{
    // TODO
    printf(M_NOT_IMPL);
    return NOT_IMPLEMENTED_YET;
}


void print_student(student_t *s) {
    if (s == NULL || s->id == 0) {
        printf(M_ERR_STD_PRINT);
        return;
    }
    printf(STUDENT_PRINT_HDR_STRING, "ID", "FIRST NAME", "LAST NAME", "GPA");
    float gpa = s->gpa / 100.0;
    printf(STUDENT_PRINT_FMT_STRING, s->id, s->fname, s->lname, gpa);
}


int compress_db(int fd) {
    close(fd);
    int temp_fd = open(TMP_DB_FILE, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (temp_fd == -1) {
        printf(M_ERR_DB_OPEN);
        return ERR_DB_FILE;
    }

    fd = open(DB_FILE, O_RDONLY);
    if (fd == -1) {
        printf(M_ERR_DB_OPEN);
        return ERR_DB_FILE;
    }

    student_t student;
    while (read(fd, &student, STUDENT_RECORD_SIZE) == STUDENT_RECORD_SIZE) {
        if (student.id != 0) {
            if (write(temp_fd, &student, STUDENT_RECORD_SIZE) != STUDENT_RECORD_SIZE) {
                printf(M_ERR_DB_WRITE);
                return ERR_DB_FILE;
            }
        }
    }

    close(fd);
    close(temp_fd);

    if (rename(TMP_DB_FILE, DB_FILE) == -1) {
        printf(M_ERR_DB_CREATE);
        return ERR_DB_FILE;
    }

    fd = open_db(DB_FILE, false);
    if (fd == -1) {
        printf(M_ERR_DB_OPEN);
        return ERR_DB_FILE;
    }

    printf(M_DB_COMPRESSED_OK);
    return fd;
}


int validate_range(int id, int gpa)
{

    if ((id < MIN_STD_ID) || (id > MAX_STD_ID))
        return EXIT_FAIL_ARGS;

    if ((gpa < MIN_STD_GPA) || (gpa > MAX_STD_GPA))
        return EXIT_FAIL_ARGS;

    return NO_ERROR;
}


void usage(char *exename)
{
    printf("usage: %s -[h|a|c|d|f|p|z] options.  Where:\n", exename);
    printf("\t-h:  prints help\n");
    printf("\t-a id first_name last_name gpa(as 3 digit int):  adds a student\n");
    printf("\t-c:  counts the records in the database\n");
    printf("\t-d id:  deletes a student\n");
    printf("\t-f id:  finds and prints a student in the database\n");
    printf("\t-p:  prints all records in the student database\n");
    printf("\t-x:  compress the database file [EXTRA CREDIT]\n");
    printf("\t-z:  zero db file (remove all records)\n");
}

// Welcome to main()
int main(int argc, char *argv[])
{
    char opt;      // user selected option
    int fd;        // file descriptor of database files
    int rc;        // return code from various operations
    int exit_code; // exit code to shell
    int id;        // userid from argv[2]
    int gpa;       // gpa from argv[5]

    // space for a student structure which we will get back from
    // some of the functions we will be writing such as get_student(),
    // and print_student().
    student_t student = {0};

    // This function must have at least one arg, and the arg must start
    // with a dash
    if ((argc < 2) || (*argv[1] != '-'))
    {
        usage(argv[0]);
        exit(1);
    }

    // The option is the first character after the dash for example
    //-h -a -c -d -f -p -x -z
    opt = (char)*(argv[1] + 1); // get the option flag

    // handle the help flag and then exit normally
    if (opt == 'h')
    {
        usage(argv[0]);
        exit(EXIT_OK);
    }

    // now lets open the file and continue if there is no error
    // note we are not truncating the file using the second
    // parameter
    fd = open_db(DB_FILE, false);
    if (fd < 0)
    {
        exit(EXIT_FAIL_DB);
    }

    // set rc to the return code of the operation to ensure the program
    // use that to determine the proper exit_code.  Look at the header
    // sdbsc.h for expected values.

    exit_code = EXIT_OK;
    switch (opt)
    {
    case 'a':
        //   arv[0] arv[1]  arv[2]      arv[3]    arv[4]  arv[5]
        // prog_name     -a      id  first_name last_name     gpa
        //-------------------------------------------------------
        // example:  prog_name -a 1 John Doe 341
        if (argc != 6)
        {
            usage(argv[0]);
            exit_code = EXIT_FAIL_ARGS;
            break;
        }

        // convert id and gpa to ints from argv.  For this assignment assume
        // they are valid numbers
        id = atoi(argv[2]);
        gpa = atoi(argv[5]);

        exit_code = validate_range(id, gpa);
        if (exit_code == EXIT_FAIL_ARGS)
        {
            printf(M_ERR_STD_RNG);
            break;
        }

        rc = add_student(fd, id, argv[3], argv[4], gpa);
        if (rc < 0)
            exit_code = EXIT_FAIL_DB;

        break;

    case 'c':
        //    arv[0] arv[1]
        // prog_name     -c
        //-----------------
        // example:  prog_name -c
        rc = count_db_records(fd);
        if (rc < 0)
            exit_code = EXIT_FAIL_DB;
        break;

    case 'd':
        //   arv[0]  arv[1]  arv[2]
        // prog_name     -d      id
        //-------------------------
        // example:  prog_name -d 100
        if (argc != 3)
        {
            usage(argv[0]);
            exit_code = EXIT_FAIL_ARGS;
            break;
        }
        id = atoi(argv[2]);
        rc = del_student(fd, id);
        if (rc < 0)
            exit_code = EXIT_FAIL_DB;

        break;

    case 'f':
        //    arv[0] arv[1]  arv[2]
        // prog_name     -f      id
        //-------------------------
        // example:  prog_name -f 100
        if (argc != 3)
        {
            usage(argv[0]);
            exit_code = EXIT_FAIL_ARGS;
            break;
        }
        id = atoi(argv[2]);
        rc = get_student(fd, id, &student);

        switch (rc)
        {
        case NO_ERROR:
            print_student(&student);
            break;
        case SRCH_NOT_FOUND:
            printf(M_STD_NOT_FND_MSG, id);
            exit_code = EXIT_FAIL_DB;
            break;
        default:
            printf(M_ERR_DB_READ);
            exit_code = EXIT_FAIL_DB;
            break;
        }
        break;

    case 'p':
        //    arv[0] arv[1]
        // prog_name     -p
        //-----------------
        // example:  prog_name -p
        rc = print_db(fd);
        if (rc < 0)
            exit_code = EXIT_FAIL_DB;
        break;

    case 'x':
        //    arv[0] arv[1]
        // prog_name     -x
        //-----------------
        // example:  prog_name -x

        // remember compress_db returns a fd of the compressed database.
        // we close it after this switch statement
        fd = compress_db(fd);
        if (fd < 0)
            exit_code = EXIT_FAIL_DB;
        break;

    case 'z':
        //    arv[0] arv[1]
        // prog_name     -x
        //-----------------
        // example:  prog_name -x
        // HINT:  close the db file, we already have fd
        //       and reopen db indicating truncate=true
        close(fd);
        fd = open_db(DB_FILE, true);
        if (fd < 0)
        {
            exit_code = EXIT_FAIL_DB;
            break;
        }
        printf(M_DB_ZERO_OK);
        exit_code = EXIT_OK;
        break;
    default:
        usage(argv[0]);
        exit_code = EXIT_FAIL_ARGS;
    }

    // dont forget to close the file before exiting, and setting the
    // proper exit code - see the header file for expected values
    close(fd);
    exit(exit_code);
}
