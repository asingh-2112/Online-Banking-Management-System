#ifndef TRANSACTIONS_DETAILS
#define TRANSACTIONS_DETAILS

#include <time.h>

struct transactionDetails
{
    int transactionID; // 0, 1, 2, 3 ...
    int accountNumber;
    int operation; // 0 -> Withdraw, 1 -> Deposit 2->TransferedTo  3->TransferedFrom
    int transAmount;
    int transferedToFrom;
    char receipentName[30];
    int oldBalance;
    int newBalance;
    time_t transactionTime;
};


#endif