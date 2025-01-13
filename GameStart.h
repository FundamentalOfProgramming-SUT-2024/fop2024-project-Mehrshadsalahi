#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>



typedef struct {
    int x;
    int y;
} point;

typedef struct {
    point point;
    int color;
    int key;
    int food;
    int gold;
    int difficulty;
    char weapon[20];
    char spell[20];
} player;

typedef struct {
    int startx;
    int starty;
    int length;
    int width;
    int type1;
    int type2;
}room;



void drawcharacter(char board[35][70],int color,int x , int y){
    board[y][x]='P';
    if(color==2)
        attron(COLOR_PAIR(2));
    if(color==3)
        attron(COLOR_PAIR(3));
    mvprintw(y,x,"%c",board[y][x]);
    if(color==2)
        attroff(COLOR_PAIR(2));
    if(color==3)
        attroff(COLOR_PAIR(3));
}




int randit(int a,int b){
    int ans=(rand() % (b-a+1))+a; //random number between a,b
    return ans;
}

void placeroom(char board[35][70],int startx,int starty,int length, int width){
    int maxx=startx+length+1;
    int maxy=starty+width+1;
    int i,j;
    for(i=startx;i<maxx;i++){
        j=starty;
        board[j][i]='_';
        j=maxy-1;
        board[j][i]='_';
    }
    for(j=starty;j<maxy;j++){
        i=startx;
        board[j][i]='|';
        i=maxx-1;
        board[j][i]='|';
    }
    int ran=0,maxran=20;
    for(i=startx+1;i<maxx-1;i++)
        for(j=starty+1;j<maxy-1;j++){
            ran=randit(1,maxran);
            if(ran==7){
                board[j][i]='O';
                maxran*=4;
            }
            else{
                board[j][i]='.';
            }
        }
            
}


int isroomvalid(char board[35][70],int startx,int starty,int length,int width){
    int valid=1;
    int maxx=startx+length+1;
    int maxy=starty+width+1;
    for(int i=startx;i<maxx;i++)
        for(int j=starty;j<maxy;j++)
            for(int k=-3;k<=4;k++)
                for(int p=-3;p<=4;p++){
                if(board[j+k][i+p]== '|'  || board[j+k][i+p]== '_')
                    valid=0;
            }
    return valid;
    }

void setupboard(char board[35][70],int visible[35][70]) {
    for (int i = 0; i < 35; i++) {
        for (int j = 0; j < 70; j++) {
            board[i][j] = ' ';
            visible[i][j]=0;
        }
    }

    for (int i=0; i<70;i++) {
        board[0][i] = '_';
        board[34][i] = '_';
    }

    for (int i=0;i<35;i++) {
        board[i][0]='|';
        board[i][69]='|';
    }
    board[0][0]='/';
    board[0][69]='\\';
}


void showmap(char board[35][70],int visible[35][70]) {
    clear();
        for (int j = 0; j < 70; j++) {
    for (int i=0;i<35;i++) {
            mvprintw(i,j,"%c",board[i][j]);
        }
    }
    refresh();
    getch();
}

void drawmap(char board[35][70],int visible[35][70]) {
    clear();
        for (int j = 0; j < 70; j++) {
    for (int i=0;i<35;i++) {
            if(visible[i][j]){
            mvprintw(i,j,"%c",board[i][j]);
            }
        }
    }
    refresh();
    getch();
}



void lightupplayer(int visible[35][70],int x,int y){
    for(int i=-2;i<3;i++)
        for(int j=-2;j<3;j++)
            visible[j][i]=1;



}

void lightuproom(room room,int visible[35][70]){
    int maxx=room.startx+room.length+1;
    int maxy=room.starty+room.width+1;
    for(int i=room.startx;i<maxx;i++)
        for(int j=room.starty;j<maxy;j++){
            visible[j][i]=1;
    }
}



void play_game(room rooms[8],player character,char board[35][70],int visible[35][70]){
    while(true){
        int order=getch();









    }
    





}





void BEGIN(player character) {
    room rooms[8];
    int q = 6 + (rand() % 3);
    clear();
    refresh();
    char board[35][70];
    int visible[35][70];
    setupboard(board,visible);
    int count=0;
    int max_attempts = 10000;
    int attempts = 0;
    while (q) {
            if (attempts++ > max_attempts) {
            setupboard(board, visible);
            count = 0;
            q = 6 + (rand() % 3);
            attempts = 0;
            continue;
        }
        int startx,starty,length,width;
        startx=randit(3,66);
        starty=randit(3,30);
        length=randit(6,10);
        width=randit(6,10);
        if(isroomvalid(board,startx,starty,length,width)){
            placeroom(board,startx,starty,length,width);
            rooms[count].startx=startx;
            rooms[count].starty=starty;
            rooms[count].length=length;
            rooms[count].width=width;
            if(!count){
                rooms[count].type1=1;
                rooms[count].type2=1;
            }
            count++;
            q--;
        }
    }
    int x,y;
    x=randit(rooms[0].startx+1,rooms[0].startx+rooms[0].length - 3);
    y=randit(rooms[0].starty+1,rooms[0].starty+rooms[0].width - 3);
    character.point.x=x;
    character.point.y=y;
    refresh();
    lightuproom(rooms[0],visible);
    drawmap(board,visible);
    refresh();
    drawcharacter(board,character.color,x,y);
    refresh();
    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(0,0,"YOU HAVE STARTED A GAME!");
    attroff(A_BOLD | COLOR_PAIR(2));
    getch();
    
}
//int main() {
 //   initscr();
 //   noecho();
 //   cbreak();
 //   curs_set(0);
 //   srand(time(NULL));

 //   player character = { .C = {0, 0}, .color = 2, .key = 0, .food = 0, .gold = 0, .difficulty = 0 };

 //   if (has_colors()) {
 //       start_color();
 //       init_pair(1, COLOR_WHITE, COLOR_GREEN);
  //      init_pair(2, COLOR_RED, COLOR_BLACK);
  //      init_pair(3, COLOR_GREEN, COLOR_BLACK);
  //  }
  //  BEGIN(character);
//
 //   endwin();
  //  return 0;
//}
