#ifndef EMPLOYEE_DETAILS
#define EMPLOYEE_DETAILS
struct employeeDetails{
    int id;
    char name[30];
    int age;
    char role[15];
    bool isManager;
    char gender[10];
    char userId[29];
    char password[20];
    int count;
    int assignedLoans[100];
};
#endif 