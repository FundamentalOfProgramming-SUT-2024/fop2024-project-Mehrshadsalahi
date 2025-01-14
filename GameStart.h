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
    int health;
    int color;
    int key;
    int broken_key;
    int food;
    int gold;
    int difficulty;
    int weapon[5];  //0==mace /1=dagger /2=Magic wand /3=normal arrow /4=sword
    int spell[3];   //0=HP //1=Speed //2=Damage
} player;

typedef struct {
    int startx;
    int starty;
    int length;
    int width;
    int type1;
}room;

int randit(int a,int b){
    int ans=(rand() % (b-a+1))+a;
    return ans;
}


void spawncorridors(int room_count,room rooms,char board[36][71]){
    for(int i=0;i<=room_count;i++){
        int side=randit(1,4);
        int widran=randit(1,rooms[i].width);
        int lengran=randit(1,rooms[i].length);
    }


    

}



void savegame(room rooms[8],player character,char board[36][71],int visible[35][70]){
    FILE *roomfile = fopen("rooms.txt", "r");
    if(roomfile == NULL)
        roomfile = fopen("rooms.txt", "w");
    fclose(roomfile);
    FILE *charfile = fopen("character.txt", "r");
    if(charfile == NULL)
        charfile = fopen("character.txt", "w");
    fclose(charfile);
    FILE *boardfile = fopen("board.txt", "r");
    if(boardfile == NULL)
        boardfile = fopen("board.txt", "w");
    fclose(boardfile);
    FILE *visiblefile = fopen("visible.txt", "r");
    if(visiblefile == NULL)
        visiblefile = fopen("visible.txt", "w");
    fclose(visiblefile);
    //save room
    roomfile=fopen("rooms.txt","a");
    //save room

    charfile=fopen("character.txt","a");
    fprintf(charfile,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\n",character.point.x,character.point.y,character.health,character.color,character.key,character.broken_key,
    character.food,character.gold,character.difficulty,character.weapon[0],character.weapon[1],character.weapon[2],character.weapon[3],character.weapon[4],
    character.spell[0],character.spell[1],character.spell[2]);
    fclose(charfile);

    boardfile=fopen("board.txt","a");
    for(int i=0;i<36;i++)
        for(int j=0;j<71;j++)
            fputc(board[i][j],boardfile);
    fprintf(boardfile,"\n");
    fclose(boardfile);

    visiblefile=fopen("visible.txt","a");
    for(int i=0;i<36;i++)
        for(int j=0;j<71;j++)
            fprintf(visiblefile,"%d",visible[i][j]);
    fprintf(visiblefile,"\n");
    fclose(visiblefile);









}



void drawcharacter(char board[36][71],int color,int x , int y){
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
    refresh();
}

void placeroom(char board[36][71],int startx,int starty,int length, int width){
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


int isroomvalid(char board[36][71],int startx,int starty,int length,int width){
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

void setupboard(char board[36][71],int visible[35][70]) {
    for (int i = 0; i<36; i++) {
        for (int j = 0; j<71; j++) {
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


void showmap(char board[36][71],int visible[35][70]) {
    clear();
        for (int j = 0; j < 70; j++) {
    for (int i=0;i<35;i++) {
            mvprintw(i,j,"%c",board[i][j]);
        }
    }
    refresh();
    getch();
}

void drawmap(char board[36][71],int visible[35][70]) {
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
void drawmapfalse(char board[36][71]) {
    clear();
        for (int j = 0; j < 70; j++) {
    for (int i=0;i<35;i++) {
            mvprintw(i,j,"%c",board[i][j]);
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


void play_game(room rooms[8],player character,char board[36][71],int visible[35][70]){
    nodelay(stdscr, TRUE);
    int ismaptrue=1;
    while(true){
        refresh();
        refresh();
        int order='#';
        order=getch();
        if(order=='s' || order=='S'){
            savegame(rooms,character,board,visible);
            return;
        }
        if( (order=='m' || order=='M')  && ismaptrue ){
            drawmapfalse(board);
            refresh();
            drawcharacter(board,character.color,character.point.x,character.point.y);
            ismaptrue=0;
            refresh();
            continue;
        }
        if( (order=='m' || order=='M')  && (!ismaptrue) ){
            drawmap(board,visible);
            refresh();
            drawcharacter(board,character.color,character.point.x,character.point.y);
            refresh();
            ismaptrue=1;
        }
        if(order== '7'){
            if(board[character.point.y -1][character.point.x -1]=='.'){
                board[character.point.y][character.point.x]='.';
                character.point.y-=1;
                character.point.x-=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
                refresh();

            }
        }
        if(order== '8'){
                board[character.point.y][character.point.x]='.';
                character.point.y-=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
            if(board[character.point.y -1][character.point.x]=='.'){
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
                refresh();

            }
            
        }
        if(order== '9'){
            if(board[character.point.y -1][character.point.x +1]=='.'){
                board[character.point.y][character.point.x]='.';
                character.point.y-=1;
                character.point.x+=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
                refresh();

            }
            
        }
        if(order== '4'){
            if(board[character.point.y][character.point.x -1]=='.'){
                board[character.point.y][character.point.x]='.';
                character.point.x-=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
                refresh();

            }
            
        }
        if(order== '6'){
            if(board[character.point.y][character.point.x +1]=='.'){
                board[character.point.y][character.point.x]='.';
                character.point.x+=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
                refresh();

            }
            
        }
        if(order== '1'){
            if(board[character.point.y +1][character.point.x -1]=='.'){
                board[character.point.y][character.point.x]='.';
                character.point.y+=1;
                character.point.x-=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
                refresh();

            }
            
        }
        if(order== '2'){
            if(board[character.point.y +1][character.point.x]=='.'){
                board[character.point.y][character.point.x]='.';
                character.point.y+=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
                refresh();

            }
            
        }
        if(order== '3'){
            if(board[character.point.y +1][character.point.x +1]=='.'){
                board[character.point.y][character.point.x]='.';
                character.point.y+=1;
                character.point.x+=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
                refresh();

            }
            
        }







    usleep(50);
    }
    
}


void visiblesetup(int visible[35][70]){
    for (int i = 0; i<36; i++) {
        for (int j = 0; j<71; j++) {
            visible[i][j]=0;
        }
    }
}




void BEGIN(int color,int difficulty) {
    player character;
    room rooms[8];
    int q = 6+(rand() % 3);
    int roomcount=q;
    clear();
    refresh();
    char board[36][71];
    int visible[35][70];
    setupboard(board,visible);
    int count=0;
    int max_attempts = 10000;
    int attempts = 0;
    while (q) {
            if (attempts++ > max_attempts) {
            setupboard(board, visible);
            count=0;
            q =6+(rand() % 3);
            roomcount=q;
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
            }
            count++;
            q--;
        }
    }
    spawncorridors(room_count,rooms,board);
    int x,y;
    x=randit(rooms[0].startx+1,rooms[0].startx+rooms[0].length - 3);
    y=randit(rooms[0].starty+1,rooms[0].starty+rooms[0].width - 3);
    character.point.x=x;
    character.point.y=y;
    refresh();
    visiblesetup(visible);
    lightuproom(rooms[0],visible);
    drawmap(board,visible);
    refresh();
    character.color=color;
    character.difficulty=difficulty;
    drawcharacter(board,character.color,x,y);
    refresh();
    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(0,0,"YOU HAVE STARTED A GAME!");
    attroff(A_BOLD | COLOR_PAIR(2));
    character.gold=0;
    character.food=0;
    character.key=0;
    character.health=20;
    character.broken_key=0;
    character.spell[0]=0;
    character.spell[1]=0;
    character.spell[2]=0;
    character.weapon[0]=1;
    character.weapon[1]=0;
    character.weapon[2]=0;
    character.weapon[3]=0;
    character.weapon[4]=0;
    character.weapon[5]=0;
    character.spell[3]=0;
    play_game(rooms,character,board,visible);
    
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
