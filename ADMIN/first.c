#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>   // for open()
#include <unistd.h>
#include <string.h>
#include "./admin_detail.h"
int main(){
    
    struct adminDetails admin={"admin-1","sarZWRLdBTKEc"};
    // strcpy(admin.loginId, "admin-1");        // Copy "admin-1" into loginId
    // strcpy(admin.password, "sarZWRLdBTKEc");
    int fd=open("ADMIN/admin.txt",O_RDWR |O_CREAT,0777);
    write(fd,&admin,sizeof(admin));
    printf("%s",admin.password);
    return 0;
}
sprintf(writeBuffer,"Employee whose details are to be modified\nId-> %d\nUserId-> %s\nName-> %s\nAge-> %d\n Gender-> %s\nRole-> %s",emp.id,emp.userId,emp.name,emp.age,emp.role);