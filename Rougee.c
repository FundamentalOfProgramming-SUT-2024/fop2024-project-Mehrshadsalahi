#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int first_menu(){
    int return_value=1;
    attron(A_BOLD | A_UNDERLINE);
    mvprintw(1,1,"Select An Option\n Press ENTER to confirm");
    attroff(A_BOLD | A_UNDERLINE);
    refresh();
    attron(COLOR_PAIR(1));
    mvprintw(2,1,"Create a New Character");
    attroff(COLOR_PAIR(1));
    refresh();
    mvprintw(3,1,"Login With an Existing Character");
    refresh();
    while(1){
        refresh();
        int input = getch();
        refresh();
        if(input == KEY_DOWN && return_value == 1){
            clear();
            attron(A_BOLD | A_UNDERLINE);
            mvprintw(1,1,"Select An Option\n Press ENTER to confirm");
            attroff(A_BOLD | A_UNDERLINE);
            refresh();
            attron(COLOR_PAIR(1));
            mvprintw(3,1,"Login With an Existing Character");
            attroff(COLOR_PAIR(1));
            mvprintw(2,1,"Create a New Character");
            return_value = 2;
            refresh();
        }
        else if (input == KEY_UP && return_value == 2){
            clear();
            attron(A_BOLD | A_UNDERLINE);
            mvprintw(1,1,"Select An Option\n Press ENTER to confirm");
            attroff(A_BOLD | A_UNDERLINE);
            refresh();
            attron(COLOR_PAIR(1));
            mvprintw(2,1,"Create a New Character");
            attroff(COLOR_PAIR(1));
            mvprintw(3,1,"Login With an Existing Character");
            return_value = 1;
            refresh();
        }
        else if (input == '\n'){
            clear();
            refresh();
            break;
        }
        refresh();
    }
    return return_value;
}

void create_account_menu(){
    FILE *file1 = fopen("accounts.txt", "r");
    if(file1 == NULL)
        file1 = fopen("accounts.txt", "w");
    fclose(file1);
    attron(A_BOLD | A_UNDERLINE);
    mvprintw(1,1,"Enter a Valid Email,Name and Password:");
    attroff(A_BOLD | A_UNDERLINE);
    refresh();
    char name[100], password[100], email[100];
    mvprintw(2,1,"Name:");
    mvprintw(3,1,"Email:");
    mvprintw(4,1,"PassWord:");
    refresh();
    while(true){
        move(2,6);
        echo();
        getstr(name);
        refresh();
        int nameval = 1;
        file1 = fopen("accounts.txt", "r");
        while(true){
            char stringcheck[300], namecheck[100];
            if(fgets(stringcheck, sizeof(stringcheck), file1) == NULL)
                break;
            strncpy(namecheck, stringcheck, strlen(name));
            if(strcmp(namecheck, name) == 0){
                nameval = 0;
                break;
            }
        }
        fclose(file1);
        if(nameval == 0){
            move(2,6);
            clrtoeol();
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(2,30,"This Name Already Exists. Try Again!");
            attroff(COLOR_PAIR(2) | A_BOLD);
            refresh();
            continue;
        }
        else if(nameval == 1)
            break;
    }
    while(true){
        move(3,7);
        getstr(email);
        refresh();
        int valid = 0, sign = 0, dot = 0, signval = 1, dotval = 1, lenE = strlen(email);
        for(int i = 0; i < lenE; i++){
            if(email[i] == '@'){
                if(!signval)
                    valid = -1;
                sign = i;
                signval = 0;
            }
            if(email[i] == '.'){
                if(!dotval)
                    valid = -1;
                dot = i;
                dotval = 0;
            }
        }
        if(valid != -1 && sign != 0 && dot > sign && (lenE - dot) > 2)
            valid = 1;
        if(valid == 1)
            break;
        else{
            move(3,7);
            clrtoeol();
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(3,70,"PLEASE WRITE A VALID EMAIL");
            attroff(COLOR_PAIR(2) | A_BOLD);
            refresh();
        }
    }
    while(true){
        move(4,10);
        getstr(password);
        refresh();
        int lenP = strlen(password), numcheck = 0, capitalcheck = 0, noncapitalcheck = 0;
        for(int i = 0; i < lenP; i++){
            if(password[i] > 96 && password[i] < 123)
                noncapitalcheck = 1;
            if(password[i] > 64 && password[i] < 91)
                capitalcheck = 1;
            if(password[i] > 47 && password[i] < 58)
                numcheck = 1;
        }
        if(noncapitalcheck && capitalcheck && numcheck)
            break;
        else{
            move(4,10);
            clrtoeol();
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(4,70,"PLEASE WRITE A VALID PASSWORD");
            attroff(COLOR_PAIR(2) | A_BOLD);
            refresh();
        }
    }
    file1 = fopen("accounts.txt", "a");
    char account[300];
    strcpy(account, name);
    strcat(account, " ");
    strcat(account, password);
    strcat(account, " ");
    strcat(account, email);
    fputs(account, file1);
    fputs("\n", file1);
    fclose(file1);
    clear();
    refresh();
    noecho();
    curs_set(FALSE);
    attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(20,20,"ACCOUNT SUCCESSFULLY CREATED");
    attroff(COLOR_PAIR(3) | A_BOLD);
    mvprintw(22,32,"Press any key to continue");
    refresh();
    getch();
    clear();
    refresh();
}

void login_menu(){
    char name[100];
    char password[100];
    char check[201];
    echo();
    while(true){
    refresh();
    attron(A_BOLD);
    mvprintw(1,1,"Login with an existing Account");
    attroff(A_BOLD);
    mvprintw(2,1,"Name:");
    mvprintw(3,1,"Password:");
    move(2,6);
    getstr(name);
    move(3,10);
    getstr(password);
    strcpy(check,name);
    strcat(check," ");
    strcat(check,password);
    FILE *file1 = fopen("accounts.txt","r");
    int YN=0;
    while(true){
        char stringcheck[201];
        if(fgets(stringcheck, sizeof(stringcheck), file1)==NULL)
            break;
        if(strstr(stringcheck,check)!=NULL)
            YN=1;
    }
    if(YN)
        break;
    else{
        clear();
        attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(3,50,"Password or Username incorrect Try again!");
        attroff(COLOR_PAIR(2) | A_BOLD);

    }
 //   strcat(check,"\0");
    }
}
int main() {
    initscr();

    keypad(stdscr, TRUE);
    if (has_colors()){
        start_color();
    init_pair(1, COLOR_WHITE, COLOR_GREEN);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
    }
    while(true){
        refresh();
        curs_set(FALSE);
        noecho();
        int a = first_menu();
        if(a == 1){
            create_account_menu();
            continue;
        }
        if(a == 2){
            login_menu();
        }
    }
    endwin();
    return 0;
}
