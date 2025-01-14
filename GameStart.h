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
    int floor;
} player;

typedef struct {
    int startx;
    int starty;
    int length;
    int width;
    int type1;
}room;

void play_game(room rooms[8], player character, char board[36][71], int visible[36][71], int room_count, int floor, char name[]);

void nextfloor(int room_count, int floor, room rooms[8], player character,char name[]);


int randit(int a,int b){
    int ans=(rand() % (b-a+1))+a;
    return ans;
}



void generatefoodgoldblackgold(char board[36][71],room rooms[],int room_count){
    int chancefood,chancegold,chanceblackgold;
    for(int i=0;i<room_count;i++){
        chancefood=randit(1,10);
        if(chancefood!=7)
            continue;
        else{
            int lengran=randit(2,rooms[i].length-4),widran=randit(2,rooms[i].length-4);
            board[rooms[i].starty+widran][rooms[i].startx+lengran]='F';
        }
    }
    for(int i=0;i<room_count;i++){
        chancegold=randit(1,20);
        if(chancegold!=7)
            continue;
        else{
            int lengran=randit(2,rooms[i].length-4),widran=randit(2,rooms[i].length-4);
            board[rooms[i].starty+widran][rooms[i].startx+lengran]='g';
        }
    }
    for(int i=0;i<room_count;i++){
        chanceblackgold=randit(1,40);
        if(chanceblackgold!=7)
            continue;
        else{
            int lengran=randit(2,rooms[i].length-4),widran=randit(2,rooms[i].length-4);
            board[rooms[i].starty+widran][rooms[i].startx+lengran]='G';
        }
    }
}

void spawnstair(char board[36][71],room rooms[8],int room_count){       // 1   2
    int ran=randit(1,room_count-1),rancorner=randit(1,4);               // 3   4 
    room ranroom=rooms[ran];
    if(rancorner==1){
        board[ranroom.starty+1][ranroom.startx+1]='<';
    }
    if(rancorner==2){
        board[ranroom.starty+1][ranroom.startx+ranroom.length-1]='<';
    }
    if(rancorner==3){
        board[ranroom.starty+ranroom.width-1][ranroom.startx+1]='<';
    }
    if(rancorner==4){
        board[ranroom.starty+ranroom.width-1][ranroom.startx+ranroom.length-1]='<';
    }

}

void debuglengwid(room rooms[],int room_count){
    for(int i=0;i<room_count;i++){
        mvprintw(rooms[i].startx+2,rooms[i].starty+2,"%d || %d",rooms[i].length,rooms[i].width);
        refresh();
    }
}

void generateweaponandspell(char board[36][71],room rooms[],int room_count){
    char spell[4]="hsd";  //speed=E //damage=G
    char weapon[5]="MDWNS";
    for(int i=0;i<room_count;i++){
        int ran1=randit(1,5),ran2=randit(0,4);
        if(ran1!=3)
            continue;
        else{
            int lengran=randit(2,rooms[i].length-4),widran=randit(2,rooms[i].length-4);
            board[rooms[i].starty+widran][rooms[i].startx+lengran]=weapon[ran2];
        }
    }
    for(int i=0;i<room_count;i++){
        int ran3=randit(1,10),ran4=(0,2);
        if(ran3!=7)
            continue;
        else{
            int lengran=randit(2,rooms[i].length-4),widran=randit(2,rooms[i].length-4);
            board[rooms[i].starty+widran][rooms[i].startx+lengran]=spell[ran4];
        }
    }
}

void showmsg(char msg[], int color){ //green=3 //red=2 //normal=1
    move(0,0);
    attron(A_BOLD);
    if(color!=1)
        attron(COLOR_PAIR(color));
    printw("%s",msg);
    if(color!=1)
        attroff(COLOR_PAIR(color));
    attroff(A_BOLD);

} 



void buildcorridorRIGHTTOLEFT(char board[36][71],point door1,point door2){
    board[door1.y][door1.x]='+';
    board[door2.y][door2.x]='+';
    for(int i=door2.x+1;i<door1.x;i++){
        board[door1.y][i]='.';
        board[door1.y-1][i]='#';
        board[door1.y+1][i]='#';
    }
}
void buildcorridorBOTTOTOP(char board[36][71],point door1,point door2){
    board[door1.y][door1.x]='+';
    board[door2.y][door2.x]='+';
    for(int i=door1.y-1;i>door2.y;i--){
        board[i][door1.x]='.';
        board[i][door1.x+1]='#';
        board[i][door1.x-1]='#';
    }

}
void buildcorridorTOPTOBOT(char board[36][71],point door1,point door2){
    board[door1.y][door1.x]='+';
    board[door2.y][door2.x]='+';
    for(int i=door1.y+1;i<door2.y;i++){
        board[i][door1.x]='.';
        board[i][door1.x+1]='#';
        board[i][door1.x-1]='#';
    }
}
void buildcorridorLEFTTORIGHT(char board[36][71],point door1,point door2){
    board[door1.y][door1.x]='+';
    board[door2.y][door2.x]='+';
    for(int i=door1.x+1;i<door2.x;i++){
        board[door1.y][i]='.';
        board[door1.y-1][i]='#';
        board[door1.y+1][i]='#';
    }


}


void spawncorridors(int room_count,room rooms[8],char board[36][71]){
    int runtime=3;
    for(int Q=0;Q<runtime;Q++){
    int i=0;
    int attempts=10000;
    while(i!=room_count){
        if(attempts==0){
            attempts=10000;
            i++;
            continue;
        }
        int side=randit(1,4);    // 1 is left | 2 is up | 3 is down | 4 is right
        int widran=randit(1,rooms[i].width-1);
        int lengran=randit(1,rooms[i].length-1);
        int didithappen=0;
        int hasithappend=0;
        point door1,door2;
        if(side==1){
            for(int k=rooms[i].starty;k<rooms[i].starty+rooms[i].width;k++){
                if(board[k][rooms[i].startx]=='+'){
                    hasithappend=1;
                }
            }
        }
        if(side==2){
            for(int k=rooms[i].startx;k<rooms[i].startx+rooms[i].length;k++){
                if(board[rooms[i].starty][k]=='+'){
                    hasithappend=1;
                }
            }
        }
        if(side==3){
            for(int k=rooms[i].startx;k<rooms[i].startx+rooms[i].length;k++){
                if(board[rooms[i].starty+rooms[i].width][k]=='+'){
                    hasithappend=1;
                }
            }
       }
        if(side==4){
            for(int k=rooms[i].starty;k<rooms[i].starty+rooms[i].width;k++){
                if(board[k][rooms[i].startx+rooms[i].length]=='+'){
                    hasithappend=1;
                }
            }
        }
        if(hasithappend){
            continue;
        }


        if(side==1){
            door1.x=rooms[i].startx;
            door1.y=rooms[i].starty+widran;
            for(int j=1;j<13;j++){
                if(board[door1.y][door1.x-j]=='|')
                    break;
                if((board[door1.y][door1.x-j]=='|') && (door1.x-j>4)  && (board[door1.y][door1.x-j-1]!='_') ){
                    didithappen=1;
                    door2.x=door1.x-j;
                    door2.y=door1.y;
                    buildcorridorRIGHTTOLEFT(board,door1,door2);
                    break;
                }
            }
        }
        if(side==2){
            door1.x=rooms[i].startx+lengran;
            door1.y=rooms[i].starty;
            for(int j=1;j<13;j++){
                if((board[door1.y-j][door1.x]=='_') && door1.y-j!=0){
                    didithappen=1;
                    door2.x=door1.x;
                    door2.y=door1.y-j;
                    buildcorridorBOTTOTOP(board,door1,door2);
                    break;
                }
            }
        }
        if(side==3){
            door1.x=rooms[i].startx+lengran;
            door1.y=rooms[i].starty+rooms[i].width;
            for(int j=1;j<13;j++){
                if((board[door1.y+j][door1.x]=='_') && door1.y+j!=34){
                    didithappen=1;
                    door2.x=door1.x;
                    door2.y=door1.y+j;
                    buildcorridorTOPTOBOT(board,door1,door2);
                    break;
                }
            }
        }
        if(side==4){
            door1.x=rooms[i].startx+rooms[i].length;
            door1.y=rooms[i].starty+widran;
            for(int j=1;j<13;j++){
                if(board[door1.y][door1.x+j]=='_')
                    break;
                if((board[door1.y][door1.x+j]=='|') && (door1.x+j<66) && (board[door1.y][door1.x+j+1]!='_') ){
                    didithappen=1;
                    door2.x=door1.x+j;
                    door2.y=door1.y;
                    buildcorridorLEFTTORIGHT(board,door1,door2);
                    break;
                }
            }
        }
        

        if(didithappen){
            i++;
            attempts=10000;
        }
        attempts--;
        }
    }
    }

void visiblesetup(int visible[36][71]){
    for (int i = 0; i<36; i++) {
        for (int j = 0; j<71; j++) {
            visible[i][j]=0;
        }
    }
}

void savegame(room rooms[],player character,char board[36][71],int visible[36][71],int room_count,char name[]){
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

    roomfile=fopen("rooms.txt","a");
    fprintf(roomfile,"%d//",room_count);
    for(int i=0;i<room_count;i++){
        fprintf(roomfile,"%d,%d,%d,%d,%d||",rooms[i].startx,rooms[i].starty,rooms[i].length,rooms[i].width,rooms[i].type1);
    }
    fprintf(roomfile,"\n");
    fclose(roomfile);

    charfile=fopen("character.txt","a");
    fprintf(charfile,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\n",character.point.x,character.point.y,character.health,character.color,character.key,character.broken_key,
    character.food,character.gold,character.difficulty,character.weapon[0],character.weapon[1],character.weapon[2],character.weapon[3],character.weapon[4],
    character.spell[0],character.spell[1],character.spell[2],character.floor);
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
    FILE *namefile = fopen("name.txt", "r");
    if(namefile == NULL)
        namefile = fopen("name.txt", "w");
    fclose(namefile);
    namefile=fopen("name.txt","a");
    fprintf(namefile,"%s\n",name);
    fclose(namefile);
}



void drawcharacter(char board[36][71],int color,int x , int y){
    attron(A_BOLD);
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
    attroff(A_BOLD);
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

void setupboard(char board[36][71],int visible[36][71]) {
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


void showmap(char board[36][71],int visible[36][71]) {
    clear();
        for (int j = 0; j < 70; j++) {
    for (int i=0;i<35;i++) {
            mvprintw(i,j,"%c",board[i][j]);
        }
    }
    refresh();
    getch();
}

void drawmap(char board[36][71],int visible[36][71]) {
    clear();
        for (int j=0; j<70; j++) {
    for (int i=0;i<35;i++) {
            if(visible[i][j]){
            if(board[i][j]=='F')
                attron(COLOR_PAIR(3));
            if(board[i][j]== 'g'  || board[i][j]=='G')
                attron(COLOR_PAIR(5));
            if(board[i][j]=='M' || board[i][j]=='D' || board[i][j]=='W'  || board[i][j]=='N'  || board[i][j]=='S' )
                attron(COLOR_PAIR(2));
            if(board[i][j]=='d' || board[i][j]=='s' || board[i][j]=='h')
                attron(COLOR_PAIR(2));
            mvprintw(i,j,"%c",board[i][j]);
            if(board[i][j]=='M' || board[i][j]=='D' || board[i][j]=='W'  || board[i][j]=='N'  || board[i][j]=='S' )
                attroff(COLOR_PAIR(2));
            if(board[i][j]=='d' || board[i][j]=='s' || board[i][j]=='h')
                attroff(COLOR_PAIR(2));
            if(board[i][j]== 'g'  || board[i][j]=='G')
                attroff(COLOR_PAIR(5));
            if(board[i][j]=='F')
                attroff(COLOR_PAIR(3));
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
            if(board[i][j]=='F')
                attron(COLOR_PAIR(3));
            if(board[i][j]== 'g'  || board[i][j]=='G')
                attron(COLOR_PAIR(5));
            if(board[i][j]=='M' || board[i][j]=='D' || board[i][j]=='W'  || board[i][j]=='N'  || board[i][j]=='S' )
                attron(COLOR_PAIR(2));
            if(board[i][j]=='d' || board[i][j]=='s' || board[i][j]=='h')
                attron(COLOR_PAIR(2));
            mvprintw(i,j,"%c",board[i][j]);
            if(board[i][j]=='M' || board[i][j]=='D' || board[i][j]=='W'  || board[i][j]=='N'  || board[i][j]=='S' )
                attroff(COLOR_PAIR(2));
            if(board[i][j]=='d' || board[i][j]=='s' || board[i][j]=='h')
                attroff(COLOR_PAIR(2));
            if(board[i][j]== 'g'  || board[i][j]=='G')
                attroff(COLOR_PAIR(5));
            if(board[i][j]=='F')
                attroff(COLOR_PAIR(3));
        }
    }
    refresh();
    getch();
}



void lightupplayer(int visible[36][71],int x,int y){
    for(int i=-2;i<3;i++)
        for(int j=-2;j<3;j++)
            visible[y+j][x+i]=1;



}

void lightuproom(room room,int visible[36][71]){
    int maxx=room.startx+room.length+1;
    int maxy=room.starty+room.width+1;
    for(int i=room.startx;i<maxx;i++)
        for(int j=room.starty;j<maxy;j++){
            visible[j][i]=1;
    }
}

void lightupplayersroom(int visible[36][71],int x,int y,room rooms[8],int room_count){
    room shine=rooms[0];
    for(int i=0;i<room_count;i++){
        if(rooms[i].startx <=x && x<=rooms[i].startx+rooms[i].length  && rooms[i].starty<=y  && y<=rooms[i].starty+rooms[i].width)
            shine=rooms[i];
    }
    lightuproom(shine,visible);
}



void showstats(player character){
    attron(A_BOLD);
    mvprintw(1,80,"Game Difficulty:%d |1=easy|2=normal|3=hard|",character.difficulty);
    mvprintw(2,80,"FOOD:%d",character.food);
    mvprintw(3,80,"HEALTH:%d",character.health);
    mvprintw(4,80,"KEYS:%d",character.key);
    mvprintw(5,80,"BROKEN KEYS:%d",character.broken_key);
    mvprintw(6,80,"GOLD:%d",character.gold);
    attroff(A_BOLD);
 //   mvprintw(7,37,"Equipped Weapon:%s",character.eqippedW);
}







void nextfloor(int room_count,int floor,room rooms[8],player character,char name[]){
    char board[36][71];
    int visible[36][71];
    setupboard(board,visible);
    int x=character.point.x;
    int y=character.point.y;
    room lastroom;
    for(int i=0;i<room_count;i++){
        if(rooms[i].startx <=x && x<=rooms[i].startx+rooms[i].length  && rooms[i].starty<=y  && y<=rooms[i].starty+rooms[i].width)
            lastroom=rooms[i];

    }
    clear();
    placeroom(board,lastroom.startx,lastroom.starty,lastroom.length,lastroom.width);
    int q =6+(rand() % 3);
    int count=1;
    int max_attempts = 10000;
    int attempts = 0;
    while (q!=1) {
            if (attempts++ > max_attempts) {
            setupboard(board, visible);
            int x=character.point.x;
            int y=character.point.y;
            room lastroom;
            for(int i=0;i<room_count;i++){
                if(rooms[i].startx <=x && x<=rooms[i].startx+rooms[i].length  && rooms[i].starty<=y  && y<=rooms[i].starty+rooms[i].width)
                    lastroom=rooms[i];
    }
            count=1;
            q =6+(rand() % 3);
            room_count=q;
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
    placeroom(board,lastroom.startx,lastroom.starty,lastroom.length,lastroom.width);
    rooms[0]=lastroom;
    visiblesetup(visible);
    lightuproom(lastroom,visible);
    spawncorridors(room_count,rooms,board);
    generateweaponandspell(board,rooms,room_count);
    spawnstair(board,rooms,room_count);
    drawmap(board,visible);
    drawcharacter(board,character.color,character.point.x,character.point.y);
    refresh();
    play_game(rooms,character,board,visible,room_count,floor+1,name);
}








void play_game(room rooms[8],player character,char board[36][71],int visible[36][71],int room_count,int floor,char name[]){
    nodelay(stdscr, TRUE);
    int wasitastair=0;
    int ismaptrue=1;
    int wasitadoor=0;
    while(true){
        int didyoumove=0;
        refresh();
        refresh();
        int order='#';
        order=getch();
        if(order==KEY_RIGHT && wasitastair==1)
            nextfloor(room_count,floor,rooms,character,name);

        if(order=='s' || order=='S'){
            savegame(rooms,character,board,visible,room_count,name);
            showmsg("Game has been saved",3);
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
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
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
            else if(board[character.point.y -1][character.point.x -1]=='+'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=1;
                 wasitastair=0;
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
            else if(board[character.point.y -1][character.point.x -1]=='<'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=1;
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
                if(board[character.point.y-1][character.point.x]=='.'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitadoor=0;
                wasitastair=0;
                character.point.y-=1;
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
                else if(board[character.point.y-1][character.point.x]=='+'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                 wasitadoor=1;
                 wasitastair=0;
                character.point.y-=1;
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
                else if(board[character.point.y-1][character.point.x]=='<'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=1;
                character.point.y-=1;
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
           
        if(order== '9'){
            if(board[character.point.y -1][character.point.x +1]=='.'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
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
            else if(board[character.point.y -1][character.point.x +1]=='+'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=1;
                 wasitastair=0;
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
            else if(board[character.point.y -1][character.point.x +1]=='<'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                    board[character.point.y][character.point.x]='+';
                wasitastair=1;
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
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
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
            else if(board[character.point.y][character.point.x -1]=='+'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=1;
                 wasitastair=0;
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
            else if(board[character.point.y][character.point.x -1]=='<'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=1;
                character.point.x-=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                refresh();

            }
            
        }
        if(order== '6'){
            if(board[character.point.y][character.point.x +1]=='.'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
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
        else if(board[character.point.y][character.point.x +1]=='+'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=1;
                 wasitastair=0;
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
        else if(board[character.point.y][character.point.x +1]=='<'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=1;
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
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
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
            else if(board[character.point.y +1][character.point.x -1]=='+'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=1;
                 wasitastair=0;
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
            else if(board[character.point.y +1][character.point.x -1]=='<'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=1;
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
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
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
            else if(board[character.point.y +1][character.point.x]=='+'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=1;
                 wasitastair=0;
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
            else if(board[character.point.y +1][character.point.x]=='<'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=1;
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
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
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
            else if(board[character.point.y +1][character.point.x +1]=='+'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=1;
                 wasitastair=0;
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
            else if(board[character.point.y +1][character.point.x +1]=='<'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=1;
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

        if(order== '7' && !didyoumove){
            if(board[character.point.y -1][character.point.x -1]=='F'){
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.point.y-=1;
                character.point.x-=1;
                character.food++;
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
            else if(board[character.point.y -1][character.point.x -1]=='g'){
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.point.y-=1;
                character.point.x-=1;
                character.gold+= randit(1,10);
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
            else if(board[character.point.y -1][character.point.x -1]=='G'){
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=0;
                character.point.y-=1;
                character.point.x-=1;
                character.gold=randit(15,25);
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
       if(order== '8' && !didyoumove){
                if(board[character.point.y-1][character.point.x]=='F'){
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitadoor=0;
                wasitastair=0;
                character.point.y-=1;
           
                clear();
                character.food++;
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
                refresh();
                }
                else if(board[character.point.y-1][character.point.x]=='g'){
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                 wasitadoor=0;
                 wasitastair=0;
                character.gold+=randit(1,10);
                character.point.y-=1;
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
                else if(board[character.point.y-1][character.point.x]=='G'){
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=0;
                character.gold+=randit(15,25);
                character.point.y-=1;
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
     if(order== '9' && !didyoumove){
            if(board[character.point.y -1][character.point.x +1]=='F'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.food+=1;
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
            else if(board[character.point.y -1][character.point.x +1]=='g'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.gold+=randit(1,10);
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
            else if(board[character.point.y -1][character.point.x +1]=='G'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                    board[character.point.y][character.point.x]='+';
                wasitastair=0;
                character.gold+=randit(15,25);
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
       if(order== '4' && !didyoumove){
            if(board[character.point.y][character.point.x -1]=='F'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.food+=1;
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
            else if(board[character.point.y][character.point.x -1]=='g'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.gold+=randit(1,10);
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
            else if(board[character.point.y][character.point.x -1]=='G'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=0;
                character.gold+=randit(1,25);
                character.point.x-=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                refresh();

            }
            
        }
       if(order== '6' && !didyoumove){
            if(board[character.point.y][character.point.x +1]=='F'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.food+=1;
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
        else if(board[character.point.y][character.point.x +1]=='g'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.gold+=randit(1,10);
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
        else if(board[character.point.y][character.point.x +1]=='G'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=0;
                character.gold+=randit(15,25);
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
        if(order== '1' && !didyoumove){
            if(board[character.point.y +1][character.point.x -1]=='F'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.food+=1;
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
            else if(board[character.point.y +1][character.point.x -1]=='g'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.gold+=randit(1,10);
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
            else if(board[character.point.y +1][character.point.x -1]=='G'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=0;
                character.gold+=randit(15,25);
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
      if(order== '2' && !didyoumove){
            if(board[character.point.y +1][character.point.x]=='F'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                character.food+=1;
                 wasitadoor=0;
                 wasitastair=0;
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
            else if(board[character.point.y +1][character.point.x]=='g'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.gold+=randit(1,10);
                character.point.y+=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
                refresh();
                refresh();

            }
            else if(board[character.point.y +1][character.point.x]=='G'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=0;
                character.gold+=randit(15,25);
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
       if(order== '3' && !didyoumove){
            if(board[character.point.y +1][character.point.x +1]=='F'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.food+=1;
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
            else if(board[character.point.y +1][character.point.x +1]=='g'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 wasitadoor=0;
                 wasitastair=0;
                character.gold+=randit(1,10);
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

            }
            else if(board[character.point.y +1][character.point.x +1]=='G'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitastair=0;
                character.gold+=randit(15,25);
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







    showstats(character);
    lightupplayer(visible,character.point.x,character.point.y);
    if(wasitadoor){
        lightupplayersroom(visible,character.point.x,character.point.y,rooms,room_count);
    }
    usleep(50);
    }
    
}


void loadgame(int savenum){
    player character;
    room rooms[8];
    int room_count;
    char board[36][71];
    int visible[36][71];
    char name[100];
    FILE *roomfile = fopen("rooms.txt", "r");
    FILE *charfile = fopen("character.txt", "r");
    FILE *boardfile = fopen("board.txt", "r");
    FILE *visiblefile = fopen("visible.txt", "r");
    FILE *namefile = fopen("name.txt", "r");
    if(savenum==1){
    fscanf(roomfile, "%d//", &room_count);
    for (int i = 0; i < room_count; i++) {
        fscanf(roomfile, "%d,%d,%d,%d,%d||", 
            &rooms[i].startx, &rooms[i].starty, 
            &rooms[i].length, &rooms[i].width, 
            &rooms[i].type1);
    }
    fclose(roomfile);

    // Load character data
    fscanf(charfile, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,",
        &character.point.x, &character.point.y,
        &character.health, &character.color,
        &character.key, &character.broken_key,
        &character.food, &character.gold,
        &character.difficulty,
        &character.weapon[0], &character.weapon[1], &character.weapon[2],
        &character.weapon[3], &character.weapon[4],
        &character.spell[0], &character.spell[1], &character.spell[2],
        &character.floor);
    fclose(charfile);

    // Load board data
    for (int i = 0; i < 36; i++) {
        for (int j = 0; j < 71; j++) {
            board[i][j] = fgetc(boardfile);
        }
    }
    fclose(boardfile);

    // Load visibility data
    for (int i = 0; i < 36; i++) {
        for (int j = 0; j < 71; j++) {
            fscanf(visiblefile, "%1d", &visible[i][j]);
        }
    }
    fclose(visiblefile);

    // Load player name
    fscanf(namefile, "%s", name);
    fclose(namefile);
    clear;
    visiblesetup(visible);
    lightuproom(rooms[0],visible);
    drawmap(board,visible);
    play_game(rooms,character,board,visible,room_count,character.floor,name);
    }
}



void BEGIN(int color,int difficulty,int floor,char name[]) {
    player character;
    room rooms[8];
    int q = 6+(rand() % 3);
    int room_count=q;
    clear();
    refresh();
    char board[36][71];
    int visible[36][71];
    setupboard(board,visible);
    int count=0;
    int max_attempts = 10000;
    int attempts = 0;
    while (q) {
            if (attempts++ > max_attempts) {
            setupboard(board, visible);
            count=0;
            q =6+(rand() % 3);
            room_count=q;
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
    generateweaponandspell(board,rooms,room_count);
    generatefoodgoldblackgold(board,rooms,room_count);
    int x,y;
    x=randit(rooms[0].startx+1,rooms[0].startx+rooms[0].length - 3);
    y=randit(rooms[0].starty+1,rooms[0].starty+rooms[0].width - 3);
    character.point.x=x;
    character.point.y=y;
    refresh();
    visiblesetup(visible);
    lightuproom(rooms[0],visible);
    spawnstair(board,rooms,room_count);
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
    if(floor==1)
        character.floor=1;
    play_game(rooms,character,board,visible,room_count,floor,name);
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
