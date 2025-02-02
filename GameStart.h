#include<stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAP_WIDTH 71
#define MAP_HEIGHT 36


typedef struct {
    char name[100];
    int score;
}winner;


typedef struct {
    int x;
    int y;
} point;

typedef struct{
    point point;
    int health;
    int attack;
    int followtype; //1=doesnt move //2=moves for 5 TIME //3=snake
    int exist;
    int doifollow;
    char type;
}enemy;

typedef struct {
    point point;
    int health;
    int color;
    int key;
    int broken_key;
    int food;
    int food_status;
    int gold;
    int difficulty;
    int weapon[5];  //0==mace /1=dagger /2=Magic wand /3=normal arrow /4=sword
    int spell[3];   //0=HP //1=Speed //2=Damage
    int floor;
    char equipped_weapon[15];
} player;

typedef struct {
    int startx;
    int starty;
    int length;
    int width;
    int type1;
}room;

void play_game(room rooms[8], player character, char board[36][71], int visible[36][71], int room_count, int floor, char name[], enemy enemy[]);

void nextfloor(int room_count, int floor, room rooms[8], player character,char name[]);


int randit(int a,int b){
    int ans=(rand() % (b-a+1))+a;
    return ans;
}




void generatefoodgoldblackgold(char board[36][71],room rooms[],int room_count,int difficulty){
    int counter=4-difficulty;
    while(counter--){
    int chancefood,chancegold,chanceblackgold;
    for(int i=0;i<room_count;i++){
        chancefood=randit(1,5);
        if(chancefood!=1)
            continue;
        else{
            int lengran=randit(2,rooms[i].length-4),widran=randit(2,rooms[i].width-4);
            board[rooms[i].starty+widran][rooms[i].startx+lengran]='@';
        }
    }
    for(int i=0;i<room_count;i++){
        chancegold=randit(1,15);
        if(chancegold!=7)
            continue;
        else{
            int lengran=randit(2,rooms[i].length-4),widran=randit(2,rooms[i].width-4);
            board[rooms[i].starty+widran][rooms[i].startx+lengran]='g';
        }
    }
    for(int i=0;i<room_count;i++){
        chanceblackgold=randit(1,30);
        if(chanceblackgold!=7)
            continue;
        else{
            int lengran=randit(2,rooms[i].length-4),widran=randit(2,rooms[i].width-4);
            board[rooms[i].starty+widran][rooms[i].startx+lengran]='*';
        }
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

//void debuglengwid(room rooms[],int room_count){
//    for(int i=0;i<room_count;i++){
//        mvprintw(rooms[i].startx+2,rooms[i].starty+2,"%d || %d",rooms[i].length,rooms[i].width);
//        refresh();
//    }
//}

void generateweaponandspell(char board[36][71],room rooms[],int room_count){
    char spell[4]="hsd"; 
    char weapon[5]="MAWNR"; //A=dagger R=sword
    for(int i=0;i<room_count;i++){
        int ran1=randit(1,5),ran2=randit(1,4);
        if(ran1!=3)
            continue;
        else{
            int lengran=randit(2,rooms[i].length-4),widran=randit(2,rooms[i].width-2);
            board[rooms[i].starty+widran][rooms[i].startx+lengran]=weapon[ran2];
        }
    }
    for(int i=0;i<room_count;i++){
        int ran3=randit(1,10),ran4=randit(0,2);
        if(ran3!=7)
            continue;
        else{
            int lengran=randit(2,rooms[i].length-4),widran=randit(2,rooms[i].width-2);
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



void setupenemy(enemy enemy[],char board[36][71],room rooms[],int room_count){
    for(int i=0;i<10;i++){
        if(randit(1,3)==1)
            enemy[i].exist=1;
    }
    for(int i=0;i<10;i++)
        if(enemy[i].exist){
            while(true){
            int roomnumber=randit(0,room_count-1);
            int widran=randit(2,rooms[roomnumber].width-2),lengran=randit(2,rooms[roomnumber].length-2);
            if(board[rooms[roomnumber].starty + widran][rooms[roomnumber].startx + lengran]=='.'){
                enemy[i].point.x=rooms[roomnumber].startx + lengran;
                enemy[i].point.y=rooms[roomnumber].starty + widran;
                board[enemy[i].point.y][enemy[i].point.x]=enemy[i].type;
                break;
            }
        }
        } 
}

int distance(int x1, int y1, int x2, int y2) {
    int ans = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    return ans;
}



void enemyaction(char board[36][71],enemy enemy[],player *character){
    for(int i=0;i<10;i++){
        if(enemy[i].exist && enemy[i].doifollow==0){
            if( abs(enemy[i].point.x-character->point.x) < 5 && abs(enemy[i].point.y - character->point.y) < 5 ){
                if(enemy[i].type=='S')
                    enemy[i].doifollow=10000;
                else
                    enemy[i].doifollow=5;
            }
        }
    }
    for(int i=0;i<10;i++){
        if(enemy[i].exist && enemy[i].doifollow){
            if(distance(enemy[i].point.x,enemy[i].point.y,character->point.x,character->point.y)<3){
                enemy[i].doifollow--;
                //enemy attack
                character->health-=enemy[i].attack;
                move(0,0);
                clrtoeol;
                attron(COLOR_PAIR(2) | A_BOLD );
                mvprintw(0,0,"The Monster hit you for %d damage!",enemy[i].attack);
                attron(COLOR_PAIR(2) | A_BOLD );
                refresh();
                //enemy attack
                continue;
            }
            if((character->point.x > enemy[i].point.x)  &&  board[enemy[i].point.y][enemy[i].point.x+1]=='.'){
                board[enemy[i].point.y][enemy[i].point.x]='.';
                enemy[i].point.x+=1;
                board[enemy[i].point.y][enemy[i].point.x]=enemy[i].type;
                enemy[i].doifollow--;
                continue;
            }
            if((character->point.x < enemy[i].point.x)  &&  board[enemy[i].point.y][enemy[i].point.x-1]=='.'){
                board[enemy[i].point.y][enemy[i].point.x]='.';
                enemy[i].point.x-=1;
                board[enemy[i].point.y][enemy[i].point.x]=enemy[i].type;
                enemy[i].doifollow--;
                continue;
            }
            if((character->point.y > enemy[i].point.y)  &&  board[enemy[i].point.y+1][enemy[i].point.x]=='.'){
                board[enemy[i].point.y][enemy[i].point.x]='.';
                enemy[i].point.y+=1;
                board[enemy[i].point.y][enemy[i].point.x]=enemy[i].type;
                enemy[i].doifollow--;
                continue;
            }
            if((character->point.y < enemy[i].point.y)  &&  board[enemy[i].point.y-1][enemy[i].point.x]=='.'){
                board[enemy[i].point.y][enemy[i].point.x]='.';
                enemy[i].point.y-=1;
                board[enemy[i].point.y][enemy[i].point.x]=enemy[i].type;
                enemy[i].doifollow--;
                continue;
            }

        }
    }


}




int closestroom(room rooms[], int room_count, int thisone, int notthis1, int notthis2) {
    int current_min = 1000000;
    int ans = 20;
    
    for (int i = 0; i < room_count; i++) {
        if (i == thisone || i == notthis1 || i == notthis2) continue;
        
        int tempdis = distance(rooms[thisone].startx, rooms[thisone].starty, rooms[i].startx, rooms[i].starty);
        
        if (tempdis < current_min) {
            current_min = tempdis;
            ans = i;
        }
    }
    
    return ans;
}

void buildpath(char board[36][71], room room1, room room2) {
    if (room2.startx > room1.startx && room2.starty > room1.starty) {
        int valid=1;
        point door1;
        door1.x = room1.startx + room1.length;
        door1.y = room1.starty + randit(2, room1.width-1);
        
        point door2;
        door2.x = room2.startx + randit(2, room2.length-1);
        door2.y = room2.starty;
        if(door2.x > door1.x  && door2.y > door1.y){
        for (int i = door1.x + 1; i <= door2.x; i++) {
            if(board[door1.y][i] !=' ')
                valid=0;
        }
        
        for (int j = door1.y; j < door2.y; j++) {
            if(board[j][door2.x] != ' ')
                valid=0;
        }
        if(valid){
        board[door1.y][door1.x] = '+';
        board[door2.y][door2.x] = '+';
        
        for (int i = door1.x + 1; i <= door2.x; i++) {
            board[door1.y][i] = '#';
        }
        
        for (int j = door1.y; j < door2.y; j++) {
            board[j][door2.x] = '#';
        }
        }
        }
        if(door2.x < room1.startx + room1.length && door2.x > room1.startx){
            int val=1;
            for(int j=door2.y-1;j>room1.starty+room1.width;j--)
                if(board[j][door2.x]!=' ')
                    val=0;
            if(val){
            int i=door2.y-1;
            board[door2.y][door2.x] = '+';
            while(board[i][door2.x]!='_'){
                board[i][door2.x]='#';
                i--;
            }
            board[i][door2.x]='+';
            }
        }
        if(door1.y < room2.starty + room2.width && door1.y > room2.starty){
            int val=1;
            for(int j=door1.x+1;j<room2.startx;j++)
                if(board[door1.y][j]!=' ')
                    val=0;
            if(val){
            int i=door1.x+1;
            board[door1.y][door1.x] = '+';
            while(board[door1.y][i]!='|'){
                board[door1.y][i]='#';
                i++;
            }
            board[door1.y][i]='+';
        }
        }
    }
    if(room2.startx < room1.startx  &&  room2.starty > room1.starty){
        int valid=1;
        point door1;
        door1.x = room1.startx;
        door1.y = room1.starty + randit(2, room1.width-1);
        
        point door2;
        door2.x = room2.startx + randit(2, room2.length-1);
        door2.y = room2.starty;
        if(door2.x < door1.x && door2.y > door1.y){
            for(int i=door1.x-1;i>=door2.x;i--){
                if(board[door1.y][i]!=' ')
                    valid=0;
            }
            for(int i=door1.y;i<door2.y;i++){
                if(board[i][door2.x]!=' ')
                    valid=0;
            }
            if(valid){
            board[door1.y][door1.x] = '+';
            board[door2.y][door2.x] = '+';
            for(int i=door1.x-1;i>=door2.x;i--)
                board[door1.y][i]='#';
            for(int i=door1.y;i<door2.y;i++)
                board[i][door2.x]='#';        
        }
        }
        if(door2.y < room1.starty + room1.width && door2.y > room1.starty){
                int val=1;
                if(board[door2.y][door2.x+1]=='_'  || board[door2.y][door2.x+1]=='|')
                    val=0;
                for(int i=door2.x+1;i<room1.startx;i++)
                    if(board[door2.y][i]!=' ')
                        val=0;
                if(val){
                for(int i=door2.x+1;i<room1.startx;i++)
                    board[door2.y][i]='#';
                board[door2.y][door2.x]='+';
                board[door2.y][room1.startx]='+';
        }
        }
        if(door1.x > room2.startx  && door1.x < room2.startx+room2.length){
            int val=1;
            if(board[door1.y+1][door1.x]=='|' || board[door1.y+1][door1.x]=='_')
                val=0;
            if(val){
            int i=door1.y+1;
            board[door1.y][door1.x]='+';
            while(board[i][door1.x]!='_'){
                board[i][door1.x]='#';
                i++;
            }
            board[i][door1.x]='+';
            }
        }
    }
    if (room1.startx > room2.startx && room1.starty > room2.starty) {
        int valid=1;
        point door1,door2;
        door1.x=room1.startx;
        door1.y=room1.starty+randit(2,room1.width-1);
        door2.x=room2.startx+randit(2,room2.length);
        door2.y=room2.starty+room2.width;
        if(door1.x > door2.x && door1.y > door2.y){
            for(int i=door2.y+1;i<=door1.y;i++){
                if(board[i][door2.x]!=' ')
                    valid=0;
            }
            for(int i=door2.x;i<door1.x;i++){
                if(board[door1.y][i])
                    valid=0;
            }
            if(valid){
            board[door1.y][door1.x]='+';
            board[door2.y][door2.x]='+';
            for(int i=door2.y+1;i<=door1.y;i++){
                board[i][door2.x]='#';
            }
            for(int i=door2.x;i<door1.x;i++){
                board[door1.y][i]='#';
            }
            }
        }
        if(door2.x < room1.startx + room1.length && door2.x > room1.startx){
            int val=1;
            for(int j=door2.y+1;j<room1.starty;j++)
                if(board[j][door2.x]!=' ')
                    val=0;
            if(val){
            int i=door2.y+1;
            board[door2.y][door2.x] = '+';
            while(board[i][door2.x]!='_'){
                board[i][door2.x]='#';
                i++;
            }
            board[i][door2.x]='+';
        }
        }
        if(door1.y < room2.starty + room2.width && door1.y > room2.starty){
            int val=1;
            for(int j=door1.x-1;j>room2.startx+room2.length;j--)
                if(board[door1.y][j]!=' ')
                    val=0;
            if(val){
            int i=door1.x-1;
            board[door1.y][door1.x] = '+';
            while(board[door2.y][i]!='|'){
                board[door1.y][i]='#';
                i--;
            }
            board[door1.y][i]='+';
            }
        }
    }
    if(room1.starty==room2.starty  && room1.startx < room2.startx){
        int valid=1;
        point door1,door2;
        door1.x=room1.startx+room1.length;
        door1.y=room1.starty+2;
        for(int i=door1.x+1;i<room2.startx;i++)
            if(board[door1.y][i]!=' ')
                valid=0;
        if(valid){
            board[door1.y][door1.x]='+';
            for(int i=door1.x+1;i<room2.startx;i++)
                board[door1.y][i]='#';
            board[door1.y][room2.startx]='+';
        }

    }

}

void spawncorridors(int room_count, room rooms[], char board[36][71]) {
//     for (int i = 0; i < room_count; i++) {
//         int close = closestroom(rooms, room_count, i, i, i);
//         buildpath(board, rooms[i], rooms[close]);
//         buildpath(board, rooms[i], rooms[close]);
//         buildpath(board, rooms[i], rooms[close]);
//         int block=close;
//         close = closestroom(rooms,room_count,i,block,i);
//         buildpath(board, rooms[i], rooms[close]);
//         buildpath(board, rooms[i], rooms[close]);
//         close = closestroom(rooms,room_count,i,block,close);
//         buildpath(board,rooms[i],rooms[close]);
        
//     }
        for(int i=0;i<room_count;i++)
            for(int j=0;j<room_count;j++){
                if(i==j)
                    continue;
                buildpath(board,rooms[i],rooms[j]);
            }
        for(int i=0;i<room_count;i++)
            for(int j=0;j<room_count;j++){
                if(i==j)
                    continue;
                buildpath(board,rooms[j],rooms[i]);
            }
 }











void visiblesetup(int visible[36][71]){
    for (int i = 0; i<36; i++) {
        for (int j = 0; j<71; j++) {
            visible[i][j]=0;
        }
    }
}

void savegame(room rooms[],player character,char board[36][71],int visible[36][71],int room_count,char name[],enemy enemy[]){
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
    FILE *enemyfile = fopen("enemy.txt", "r");
    if(enemyfile == NULL)
        enemyfile = fopen("enemy.txt", "w");
    fclose(enemyfile);

    roomfile=fopen("rooms.txt","w");
    fprintf(roomfile,"%d//",room_count);
    for(int i=0;i<room_count;i++){
        fprintf(roomfile,"%d,%d,%d,%d,%d||",rooms[i].startx,rooms[i].starty,rooms[i].length,rooms[i].width,rooms[i].type1);
    }
    fprintf(roomfile,"\n");
    fclose(roomfile);

    enemyfile=fopen("enemy.txt","w");
    for(int i=0;i<10;i++){
        fprintf(enemyfile,"%d %d %d %d %d %d %c ",enemy[i].point.x,enemy[i].point.y,enemy[i].health,enemy[i].attack,enemy[i].exist,enemy[i].doifollow,enemy[i].type);
    }
    fclose(enemyfile);

    charfile=fopen("character.txt","w");
    fprintf(charfile,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\n",character.point.x,character.point.y,character.health,character.color,character.key,character.broken_key,
    character.food,character.gold,character.difficulty,character.weapon[0],character.weapon[1],character.weapon[2],character.weapon[3],character.weapon[4],
    character.spell[0],character.spell[1],character.spell[2],character.floor);
    fclose(charfile);

    boardfile=fopen("board.txt","w");
    for(int i=0;i<36;i++)
        for(int j=0;j<71;j++)
            fputc(board[i][j],boardfile);
    fprintf(boardfile,"\n");
    fclose(boardfile);

    visiblefile=fopen("visible.txt","w");
    for(int i=0;i<36;i++)
        for(int j=0;j<71;j++)
            fprintf(visiblefile,"%d",visible[i][j]);
    fprintf(visiblefile,"\n");
    fclose(visiblefile);
    FILE *namefile = fopen("name.txt", "r");
    if(namefile == NULL)
        namefile = fopen("name.txt", "w");
    fclose(namefile);
    namefile=fopen("name.txt","w");
    fprintf(namefile,"%s\n",name);
    fclose(namefile);
}



void drawcharacter(char board[36][71],int color,int x , int y){
    attron(A_BOLD);
    board[y][x]='P';
    const char *utfplayer="⚚";
    if(color==2)
        attron(COLOR_PAIR(2));
    if(color==3)
        attron(COLOR_PAIR(3));
    mvprintw(y,x,"%s",utfplayer);
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


//void showmap(char board[36][71],int visible[36][71]) {
//    clear();
//        for (int j = 0; j < 70; j++) {
//    for (int i=0;i<35;i++) {
//            mvprintw(i,j,"%c",board[i][j]);
//        }
//    }
//    refresh();
//    getch();
//}

void drawmap(char board[36][71],int visible[36][71]) {
    const char *Mace="⚒️";
    const char *Dagger="†";
    const char *MagicWand="✪";
    const char *NormalArrow="➻";
    const char *Sword="‡";
    clear();
        for (int j=0; j<70; j++) {
    for (int i=0;i<35;i++) {
            if(visible[i][j]){
            
            if(board[i][j]=='M'){
                mvprintw(i,j,"%s",Mace);
                continue;
            }
            if(board[i][j]=='A'){
                mvprintw(i,j,"%s",Dagger);
                continue;
            }
            if(board[i][j]=='W'){
                attron(COLOR_PAIR(10));
                mvprintw(i,j,"%s",MagicWand);
                attroff(COLOR_PAIR(10));
                continue;
            }
            if(board[i][j]=='N'){
                mvprintw(i,j,"%s",NormalArrow);
                continue;
            }
            if(board[i][j]=='R'){
                mvprintw(i,j,"%s",Sword);
                continue;
            }
            if(board[i][j]=='@')
                attron(COLOR_PAIR(3));
            if(board[i][j]== 'g'  || board[i][j]=='*')
                attron(COLOR_PAIR(5));
            if(board[i][j]=='D' || board[i][j]=='F' || board[i][j]=='G'  || board[i][j]=='S'  || board[i][j]=='U' )
                attron(COLOR_PAIR(2));
           if(board[i][j]=='d' || board[i][j]=='s' || board[i][j]=='h')
                attron(COLOR_PAIR(8));
            mvprintw(i,j,"%c",board[i][j]);
            if(board[i][j]=='D' || board[i][j]=='F' || board[i][j]=='G'  || board[i][j]=='S'  || board[i][j]=='U' )
                attroff(COLOR_PAIR(2));
            if(board[i][j]=='d' || board[i][j]=='s' || board[i][j]=='h')
                attroff(COLOR_PAIR(8));
            if(board[i][j]== 'g'  || board[i][j]=='*')
                attroff(COLOR_PAIR(5));
            if(board[i][j]=='@')
                attroff(COLOR_PAIR(3));
            }
        }
    }
    refresh();
//    getch();
}
void drawmapfalse(char board[36][71]) {
    const char *Mace="⚒️";
    const char *Dagger="†";
    const char *MagicWand="✪";
    const char *NormalArrow="➻";
    const char *Sword="‡";
    clear();
        for (int j=0; j<70; j++) {
    for (int i=0;i<35;i++) {
            
            if(board[i][j]=='M'){
                mvprintw(i,j,"%s",Mace);
                continue;
            }
            if(board[i][j]=='A'){
                mvprintw(i,j,"%s",Dagger);
                continue;
            }
            if(board[i][j]=='W'){
                attron(COLOR_PAIR(10));
                mvprintw(i,j,"%s",MagicWand);
                attroff(COLOR_PAIR(10));
                continue;
            }
            if(board[i][j]=='N'){
                mvprintw(i,j,"%s",NormalArrow);
                continue;
            }
            if(board[i][j]=='R'){
                mvprintw(i,j,"%s",Sword);
                continue;
            }
            if(board[i][j]=='@')
                attron(COLOR_PAIR(3));
            if(board[i][j]== 'g'  || board[i][j]=='*')
                attron(COLOR_PAIR(5));
            if(board[i][j]=='D' || board[i][j]=='F' || board[i][j]=='G'  || board[i][j]=='S'  || board[i][j]=='U' )
                attron(COLOR_PAIR(2));
            if(board[i][j]=='d' || board[i][j]=='s' || board[i][j]=='h')
                attron(COLOR_PAIR(8));
            mvprintw(i,j,"%c",board[i][j]);
            if(board[i][j]=='D' || board[i][j]=='F' || board[i][j]=='G'  || board[i][j]=='S'  || board[i][j]=='U' )
                attroff(COLOR_PAIR(2));
            if(board[i][j]=='d' || board[i][j]=='s' || board[i][j]=='h')
                attroff(COLOR_PAIR(8));
            if(board[i][j]== 'g'  || board[i][j]=='*')
                attroff(COLOR_PAIR(5));
            if(board[i][j]=='@')
                attroff(COLOR_PAIR(3));
        }
    }
    refresh();
//    getch();
}



void lightupplayer(int visible[36][71],int x,int y){
    for(int i=-3;i<4;i++)
        for(int j=-3;j<4;j++)
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
    if(character.difficulty==1)
        mvprintw(1,80,"Game Difficulty: Easy");
    if(character.difficulty==2)
        mvprintw(1,80,"Game Difficulty: Normal");
    if(character.difficulty==3)
        mvprintw(1,80,"Game Difficulty: Hard");
    mvprintw(2,80,"FOOD:%d",character.food);
    mvprintw(3,80,"HEALTH:%d",character.health);
    mvprintw(4,80,"KEYS:%d",character.key);
    mvprintw(5,80,"BROKEN KEYS:%d",character.broken_key);
    mvprintw(6,80,"GOLD:%d",character.gold);
    mvprintw(7,80,"Equipped Weapon:%s",character.equipped_weapon);
    if(character.food_status==4){
        mvprintw(8,80,"Hunger Status:Fully satiated");
        attron(COLOR_PAIR(3));
        mvprintw(9,94,"###");
        attroff(COLOR_PAIR(3));
    }
    if(character.food_status==3){
        mvprintw(8,80,"Hunger Status:moderately Fed");
        attron(COLOR_PAIR(3));
        mvprintw(9,94,"##");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(2));
        mvprintw(9,96,",");
        attroff(COLOR_PAIR(2));
    }
    if(character.food_status==2){
        mvprintw(8,80,"Hunger Status:Hungry");
        attron(COLOR_PAIR(3));
        mvprintw(9,94,"#");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(2));
        mvprintw(9,95,",,");
        attroff(COLOR_PAIR(2));
    }
    if(character.food_status==1){
        mvprintw(8,80,"Hunger Status:Starving");
        attron(COLOR_PAIR(2));
        mvprintw(9,94,",,,");
        attroff(COLOR_PAIR(2));
    }
    attroff(A_BOLD);
}



void treasure(room rooms[],char board[36][71],int room_count){
    int E=randit(1,room_count-1);
    for(int i=rooms[E].startx;i<=rooms[E].startx+rooms[E].length;i++)
        for(int j=rooms[E].starty;j<=rooms[E].starty+rooms[E].width;j++){
            if(board[j][i]=='.'){
                if(randit(1,15)==1){
                    board[j][i]='g';
                }
                if(randit(1,20)==1)
                    board[j][i]='*';
            }
            if(board[j][i]=='O')
                board[j][i]='g';
        }
    int randomx=randit(rooms[E].startx+2,rooms[E].startx+rooms[E].length-2);
    int randomy=randit(rooms[E].starty+2,rooms[E].starty+rooms[E].width-2);
    board[randomy][randomx]='$';
}





void nextfloor(int room_count,int floor,room rooms[8],player character,char name[]){
    char board[36][71];
    int visible[36][71];
    room rooms2[8];
    setupboard(board,visible);
    int x=character.point.x;
    int y=character.point.y;
    for(int i=0;i<room_count;i++){
        if(rooms[i].startx <=x && x<=rooms[i].startx+rooms[i].length  && rooms[i].starty<=y  && y<=rooms[i].starty+rooms[i].width)
            rooms2[0]=rooms[i];

    }
    clear();
    placeroom(board,rooms2[0].startx,rooms2[0].starty,rooms2[0].length,rooms2[0].width);
    int q =6+(rand() % 3);
    room_count=q;
    int count=1;
    int max_attempts = 10000;
    int attempts = 0;
    while (q>0) {
            if (attempts++ > max_attempts) {
            setupboard(board, visible);
            placeroom(board,rooms2[0].startx,rooms2[0].starty,rooms2[0].length,rooms2[0].width);
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
            rooms2[count].startx=startx;
            rooms2[count].starty=starty;
            rooms2[count].length=length;
            rooms2[count].width=width;
            rooms2[count].type1=1;
            count++;
            q--;
        }
    }
    visiblesetup(visible);
    lightuproom(rooms2[0],visible);
    spawncorridors(room_count,rooms2,board);
    generateweaponandspell(board,rooms2,room_count);
    generatefoodgoldblackgold(board,rooms2,room_count,character.difficulty);
    if(floor!=3)
        spawnstair(board,rooms2,room_count);
    if(floor==3)
        treasure(rooms2,board,room_count);
    drawmap(board,visible);
    drawcharacter(board,character.color,character.point.x,character.point.y);
    refresh();
    enemy enemy[10];
    //Demon
    enemy[0].attack=3*character.difficulty;
    enemy[0].health= 5;
    enemy[0].followtype=1;
    enemy[0].point.x=0;
    enemy[0].point.y=0;
    enemy[0].exist=0;
    enemy[0].doifollow=0;
    enemy[0].type='D';
    enemy[1]=enemy[0];
    //FIIIRE
    enemy[2].attack=5*character.difficulty;
    enemy[2].health=10;
    enemy[2].followtype=1;
    enemy[2].point.x=0;
    enemy[2].point.y=0;
    enemy[2].exist=0;
    enemy[2].doifollow=0;
    enemy[2].type='F';
    enemy[3]=enemy[2];
    //GIANT
    enemy[4].attack=7*character.difficulty;
    enemy[4].health=15;
    enemy[4].followtype=2;
    enemy[4].point.x=0;
    enemy[4].point.y=0;
    enemy[4].exist=0;
    enemy[4].doifollow=0;
    enemy[4].type='G';
    enemy[5]=enemy[4];
    //Snake
    enemy[6].attack=8*character.difficulty;
    enemy[6].health=20;
    enemy[6].followtype=3;
    enemy[6].point.x=0;
    enemy[6].point.y=0;
    enemy[6].exist=0;
    enemy[6].doifollow=0;
    enemy[6].type='S';
    enemy[7]=enemy[6];
    //The Undead
    enemy[8].attack=12*character.difficulty;
    enemy[8].health=30;
    enemy[8].followtype=2;
    enemy[8].point.x=0;
    enemy[8].point.y=0;
    enemy[8].exist=0;
    enemy[8].doifollow=0;
    enemy[8].type='U';
    enemy[9]=enemy[8];
    setupenemy(enemy,board,rooms2,room_count);
    play_game(rooms2,character,board,visible,room_count,floor+1,name,enemy);
}

void loadgame(int savenum){
    player character;
    room rooms[8];
    int room_count;
    char board[36][71];
    int visible[36][71];
    enemy enemy[10];
    char name[100];
    FILE *roomfile = fopen("rooms.txt", "r");
    FILE *charfile = fopen("character.txt", "r");
    FILE *boardfile = fopen("board.txt", "r");
    FILE *visiblefile = fopen("visible.txt", "r");
    FILE *namefile = fopen("name.txt", "r");
    FILE *enemyfile = fopen("enemy.txt","r");
    if(savenum==1){
    fscanf(roomfile, "%d//", &room_count);
    for (int i = 0; i < room_count; i++) {
        fscanf(roomfile, "%d,%d,%d,%d,%d||", 
            &rooms[i].startx, &rooms[i].starty, 
            &rooms[i].length, &rooms[i].width, 
            &rooms[i].type1);
    }
    fclose(roomfile);

    for(int i=0;i<10;i++){
        fscanf(enemyfile,"%d %d %d %d %d %d %c",&enemy[i].point.x,&enemy[i].point.y,&enemy[i].health,&enemy[i].attack,&enemy[i].exist,&enemy[i].doifollow,&enemy[i].type);
    }
    fclose(enemyfile);


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
    strcpy(character.equipped_weapon,"Mace");
    character.food_status=2;
    for (int i = 0; i < 36; i++) {
        for (int j = 0; j < 71; j++) {
            board[i][j] = fgetc(boardfile);
        }
    }
    fclose(boardfile);
    for (int i = 0; i < 36; i++) {
        for (int j = 0; j < 71; j++) {
            fscanf(visiblefile, "%1d", &visible[i][j]);
        }
    }
    fclose(visiblefile);
    fscanf(namefile, "%s", name);
    fclose(namefile);
    clear;
//    visiblesetup(visible);
//    lightuproom(rooms[0],visible);
    drawmap(board,visible);
    drawcharacter(board,character.color,character.point.x,character.point.y);
    play_game(rooms,character,board,visible,room_count,character.floor,name,enemy);
    }
}



int spellmenu(player *character){
    nodelay(stdscr,FALSE);
    int order;
    int result=000;
    int use1=1,use2=1,use3=1;
    while(true){
    clear();
    refresh();
    attron(A_BOLD | COLOR_PAIR(5));
    mvprintw(0,0,"Click 1,2,3 To Use the Coresponding Spells");
    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(10));
    mvprintw(1,1,"1)Health     %d pots",character->spell[0]);
    mvprintw(2,1,"2)Speed      %d pots",character->spell[1]);
    mvprintw(3,1,"3)Damage     %d pots",character->spell[2]);
    refresh();
    order=getch();
    if(order=='1'  && character->spell[0]!=0 && use1 ){
        character->spell[0]-=1;
        use1=0;
        result+=100;
    }
    if(order=='2' && character->spell[1]!=0  && use2){
        character->spell[1]-=1;
        use2=0;
        result+=10;
    }
    if(order=='3' && character->spell[2]!=0  && use3){
        character->spell[2]-=1;
        use3=0;
        result+=1;
    }
    if(order=='q' || order=='Q')
        break;
    }
    nodelay(stdscr,TRUE);
    return result;
}


void weaponsmenu(player *character){
    nodelay(stdscr,FALSE);
    int order='M';
    while(true){
    if(strcmp(character->equipped_weapon,"Mace")==0)
        order='M';
    if(strcmp(character->equipped_weapon,"Dagger")==0)
        order='D';
    if(strcmp(character->equipped_weapon,"Magic Wand")==0)
        order='W';
    if(strcmp(character->equipped_weapon,"Normal Arrow")==0)
        order='N';
    if(strcmp(character->equipped_weapon,"Sword")==0)
        order='S';
    clear();
    refresh();
    attron(A_BOLD | COLOR_PAIR(5));
    mvprintw(0,0,"Press ENTER on any weapon to select it to Leave press Q");
    mvprintw(1,0,"press D to equip dagger | M to equip Mace | W to equip magic Wand ");
    mvprintw(2,0,"N to equip Normal Arrow | S to equip Sword ");
    attroff(COLOR_PAIR(5));
    if( (order=='M' || order=='m') && character->weapon[0]!=0)
        attron(COLOR_PAIR(1));
    mvprintw(3,0," ⚒️ Mace|| Amount: %d || Melee || 5 DMG",character->weapon[0]);
    if( (order=='M' || order=='m') && character->weapon[0]!=0)
        attroff(COLOR_PAIR(1));
    if( (order=='D' || order=='d') && character->weapon[1]!=0)
        attron(COLOR_PAIR(1));
    mvprintw(4,0," † Dagger || Amount: %d || Ranged with the Range of 5 || 12 DMG",character->weapon[1]);
    if( (order=='D' || order=='d') && character->weapon[1]!=0)
        attroff(COLOR_PAIR(1));
    if( (order=='w' || order=='W') && character->weapon[2]!=0)
        attron(COLOR_PAIR(1));
    mvprintw(5,0," ✪ Magic Wand || Amount:%d || Ranged with the range of 10 || 15 DMG ",character->weapon[2]);
    if( (order=='w' || order=='W') && character->weapon[2]!=0)
        attroff(COLOR_PAIR(1));
    if( (order=='n' || order=='N') && character->weapon[3]!=0)
        attron(COLOR_PAIR(1));
    mvprintw(6,0," ➻ Normal Arrow || Amount: %d || Ranged with the range of 5 || 5 DMG",character->weapon[3]);
    if( (order=='n' || order=='N') && character->weapon[3]!=0)
        attroff(COLOR_PAIR(1));
    if( (order=='S' || order=='s') && character->weapon[4]!=0)
        attron(COLOR_PAIR(1));
    mvprintw(7,0," ‡ Sword || Amount: %d || Melee || 10 DMG",character->weapon[4]);
    if( (order=='S' || order=='s') && character->weapon[4]!=0)
        attroff(COLOR_PAIR(1));
    mvprintw(8,0,"Equipped Weapon= %s",character->equipped_weapon);
    attroff(A_BOLD);
    refresh();
    refresh();
    order=getch();
    if(order=='q' || order=='Q')
        break;
    if( (order=='m' || order=='M') && character->weapon[0]!=0)
        strcpy(character->equipped_weapon,"Mace");
    if( (order=='d' || order=='D') && character->weapon[1]!=0)
        strcpy(character->equipped_weapon,"Dagger");
    if( (order=='w' || order=='W') && character->weapon[2]!=0)
        strcpy(character->equipped_weapon,"Magic Wand");
    if( (order=='n' || order=='N') && character->weapon[3]!=0)
        strcpy(character->equipped_weapon,"Normal Arrow");
    if( (order=='s' || order=='S') && character->weapon[4]!=0)
        strcpy(character->equipped_weapon,"Sword");
    }
    nodelay(stdscr,TRUE);
    clear();
}

void gameoverscreen(player character,char name[]){
    clear();
    int score=character.gold * 100 + character.floor*1000 + character.food * 10+ character.key * 50 + character.broken_key * 20;
    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(20,20,"Y O U D I E D %s your score was %d",name,score);
    attroff(A_BOLD| COLOR_PAIR(2));
    mvprintw(21,20,"press any key to close the program");
    refresh();
    sleep(2);
    nodelay(stdscr,FALSE);
    getch();
    clear();
    endwin();
    exit(0);
}


void attacksquares(point squares[],int square_count,enemy enemy[],int damage,char board[36][71]){
    for(int i=0;i<10;i++){
        for(int j=0;j<square_count;j++){
            if(enemy[i].point.x==squares[j].x  && enemy[i].point.y==squares[j].y ){
                enemy[i].health-=damage;
                if(enemy[i].health <= 0 ){
                    enemy[i].exist=0;
                    board[enemy[i].point.y][enemy[i].point.x]='.';
                    showmsg("You Killed the MONSTER!",3);
                }
                else{
                move(37,0);
                clrtoeol;
                attron(COLOR_PAIR(2) | A_BOLD );
                mvprintw(37,0,"You Hit the Monster for %d Damage, It has %d HP remaining",damage,enemy[i].health);
                attroff(COLOR_PAIR(2) | A_BOLD );
                refresh();
                }
            }
        }
    }
    
}




void SHOOT(char board[36][71],int direction,int weapontype,enemy enemy[],int x,int y,int weapon[]){ //down left right up
    int distance=5;
    int damage=10;
    if(weapontype==1){
        distance=5;
        damage=12;
    }
    if(weapontype==2){
        distance=10;
        damage=15;
    }
    if(weapontype==3){
        distance=5;
        damage=5;
    }
    if(direction==1){
        for(int i=y;i<y+distance+1;i++){
            if(board[i][x]=='D' || board[i][x]=='F' || board[i][x]=='S' || board[i][x]=='U' || board[i][x]=='G' ){
            point squares[2];
            squares[0].x=x;
            squares[0].y=i;
            attacksquares(squares,1,enemy,damage,board);
            break;
            }
            if(i==y+distance)
                showmsg("You did Not hit an enemy",2);
        }
    }
    if(direction==2){
        for(int i=x;i>x-distance-1;i--){
            if(board[y][i]=='D' || board[y][i]=='F' || board[y][i]=='S' || board[y][i]=='U' || board[y][i]=='G' ){
            point squares[2];
            squares[0].x=i;
            squares[0].y=y;
            attacksquares(squares,1,enemy,damage,board);
            break;
            }
            if(i==x-distance)
                showmsg("You did Not hit an enemy",2);
        }
    }
    if(direction==3){
        for(int i=x;i<x+distance+1;i++){
            if(board[y][i]=='D' || board[y][i]=='F' || board[y][i]=='S' || board[y][i]=='U' || board[y][i]=='G' ){
            point squares[2];
            squares[0].x=i;
            squares[0].y=y;
            attacksquares(squares,1,enemy,damage,board);
            break;
            }
            if(i==x+distance)
                showmsg("You did Not hit an enemy",2);
        }
    }
    if(direction==4){
        for(int i=y;i<y-distance-1;i--){
            if(board[i][x]=='D' || board[i][x]=='F' || board[i][x]=='S' || board[i][x]=='U' || board[i][x]=='G' ){
            point squares[2];
            squares[0].x=x;
            squares[0].y=i;
            attacksquares(squares,1,enemy,damage,board);
            break;
            }
            if(i==y-distance)
                showmsg("You did Not hit an enemy",2);
        }
    }




}


void treasureroom(player character,char name[]){
    room rooms[8];
    rooms[0].startx=randit(2,10);
    rooms[0].starty=randit(2,10);
    rooms[0].width=randit(6,10);
    rooms[0].length=randit(10,12);
    int difficulty=character.difficulty;
    int room_count=1;
    clear();
    refresh();
    char board[36][71];
    int visible[36][71];
    setupboard(board,visible);
    placeroom(board,rooms[0].startx,rooms[0].starty,rooms[0].length,rooms[0].width);
    int count=0;
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
    drawcharacter(board,character.color,x,y);
    refresh();
    character.floor=4;
    enemy enemy[10];
    //Demon
    enemy[0].attack=3*difficulty;
    enemy[0].health= 5;
    enemy[0].followtype=1;
    enemy[0].point.x=0;
    enemy[0].point.y=0;
    enemy[0].exist=0;
    enemy[0].doifollow=0;
    enemy[0].type='D';
    enemy[1]=enemy[0];
    //FIIIRE
    enemy[2].attack=5*difficulty;
    enemy[2].health=10;
    enemy[2].followtype=1;
    enemy[2].point.x=0;
    enemy[2].point.y=0;
    enemy[2].exist=0;
    enemy[2].doifollow=0;
    enemy[2].type='F';
    enemy[3]=enemy[2];
    //GIANT
    enemy[4].attack=7*difficulty;
    enemy[4].health=15;
    enemy[4].followtype=2;
    enemy[4].point.x=0;
    enemy[4].point.y=0;
    enemy[4].exist=0;
    enemy[4].doifollow=0;
    enemy[4].type='G';
    enemy[5]=enemy[4];
    //Snake
    enemy[6].attack=8*difficulty;
    enemy[6].health=20;
    enemy[6].followtype=3;
    enemy[6].point.x=0;
    enemy[6].point.y=0;
    enemy[6].exist=0;
    enemy[6].doifollow=0;
    enemy[6].type='S';
    enemy[7]=enemy[6];
    //The Undead
    enemy[8].attack=12*difficulty;
    enemy[8].health=30;
    enemy[8].followtype=2;
    enemy[8].point.x=0;
    enemy[8].point.y=0;
    enemy[8].exist=0;
    enemy[8].doifollow=0;
    enemy[8].type='U';
    enemy[9]=enemy[8];
    setupenemy(enemy,board,rooms,room_count);
    for(int i=rooms[0].startx;i<=rooms[0].startx+rooms[0].length;i++)
        for(int j=rooms[0].starty;j<=rooms[0].starty+rooms[0].width;j++){
            if(board[j][i]=='.'){
                if(randit(1,10)==1){
                    board[j][i]='g';
                }
                if(randit(1,20)==1)
                    board[j][i]='*';
            }
            if(board[j][i]=='O')
                board[j][i]='*';
        }
    play_game(rooms,character,board,visible,room_count,4,name,enemy);

}

void VICTORY(char name[], player character) {
    clear();
    int score = character.gold * 100 + character.floor * 1000 + character.food * 10 + 
                character.key * 50 + character.broken_key * 20 + 10000;
    
    attron(A_BOLD | COLOR_PAIR(5));
    mvprintw(20, 20, "You Escaped from the Dungeon %s!!! Your Score Was %d", name, score);
    attroff(A_BOLD | COLOR_PAIR(5));
    mvprintw(21, 20, "Press any key to close the program");
    refresh();
    sleep(2);

    FILE *scoreboard = fopen("scoreboard.txt", "r");
    char player_names[10][50];
    int scores[10];
    int count = 0;
    int found = 0;

    while (count<10 && fscanf(scoreboard, "%s %d", player_names[count], &scores[count]) == 2) {
        if (strcmp(player_names[count], name) == 0) {
            scores[count] += score;
            found = 1;
        }
        count++;
    }
    fclose(scoreboard);

    if (!found && count<10) {
        strcpy(player_names[count], name);
        scores[count] = score;
        count++;
    }

    scoreboard = fopen("scoreboard.txt", "w");

    for (int i=0; i<count;i++) {
        fprintf(scoreboard, "%s %d\n", player_names[i], scores[i]);
    }
    fclose(scoreboard);
    nodelay(stdscr, FALSE);
    getch();
    clear();
    endwin();
    exit(0);
}


void play_game(room rooms[8],player character,char board[36][71],int visible[36][71],int room_count,int floor,char name[],enemy enemy[]){
    nodelay(stdscr, TRUE);
    int WHENEATFOOD=0;
    int time=0;
    int wasitastair=0;
    int ismaptrue=1;
    int wasitadoor=0;
    int wasitacor=0;
    int IMSTARVING=0;
    int IREALLYAMSTARVING=0;
    int spellD=1,spellH=1,spellS=1;
    int spells=000;
    int spelltimer=0;
    int direction=-1;
    while(true){
        int didyoumove=0;
        int attacked=0;
        int isita=0;
        refresh();
        int order='#';
        order=getch();
        refresh();
        if(order==KEY_RIGHT && wasitastair==1)
            nextfloor(room_count,floor,rooms,character,name);
        if(order=='E' || order=='e' && character.food>0){
            character.food--;
            character.food_status++;
            character.health+=5;
            if(character.health>200)
                character.health=200;
            showmsg("You Ate some Food And recovered some HP!",3);
        }
        if( (order=='a' || order=='A') && direction!=-1){
            order=' ';
            isita=1;
        }
        if(order==' '  && (strcmp(character.equipped_weapon,"Mace")==0  || strcmp(character.equipped_weapon,"Sword")==0 )){ //0 1 2 \n 3 
            point damage[7];
            damage[0].x=character.point.x-1;
            damage[0].y=character.point.y-1;
            damage[1].x=character.point.x;
            damage[1].y=character.point.y-1;
            damage[2].x=character.point.x+1;
            damage[2].y=character.point.y-1;
            damage[3].x=character.point.x-1;
            damage[3].y=character.point.y;
            damage[4].x=character.point.x+1;
            damage[4].y=character.point.y;
            damage[5].x=character.point.x-1;
            damage[5].y=character.point.y+1;
            damage[6].x=character.point.x;
            damage[6].y=character.point.y+1;
            damage[7].x=character.point.x+1;
            damage[7].y=character.point.y+1;
            int damageint=0;
            if(strcmp(character.equipped_weapon,"Mace")==0)
                damageint=5;
            if(strcmp(character.equipped_weapon,"Sword")==0)
                damageint=10;
            damageint*=spellD;
            attacksquares(damage,7,enemy,damageint,board);
            attacked=1;
        }
        if(order==' ' && (strcmp(character.equipped_weapon,"Magic Wand")==0  || strcmp(character.equipped_weapon,"Dagger")==0 ||
              strcmp(character.equipped_weapon,"Normal Arrow")==0)  ){
            int whichweapon=0;
            if(strcmp(character.equipped_weapon,"Dagger")==0)
                whichweapon=1;
            if(strcmp(character.equipped_weapon,"Magic Wand")==0)
                whichweapon=2;
            if(strcmp(character.equipped_weapon,"Normal Arrow")==0)
                whichweapon=3;
            
            showmsg("choose the direction",2);
            nodelay(stdscr,FALSE);
            while(true){
            if(!isita)
                direction=getch();
            if(direction==KEY_DOWN){
                SHOOT(board,1,whichweapon,enemy,character.point.x,character.point.y,character.weapon);
                break;
            }
            if(direction==KEY_LEFT){
                SHOOT(board,2,whichweapon,enemy,character.point.x,character.point.y,character.weapon);
                break;
            }
            if(direction==KEY_RIGHT){
                SHOOT(board,3,whichweapon,enemy,character.point.x,character.point.y,character.weapon);
                break;

            }
            if(direction==KEY_UP){
                SHOOT(board,4,whichweapon,enemy,character.point.x,character.point.y,character.weapon);
                break;

            }
            else{
                move(0,0);
                clrtoeol;
                showmsg("CHOOSE A VALID DIRECTION",2);
            }
            }
            nodelay(stdscr,TRUE);
            attacked=1;
            character.weapon[whichweapon]--;
            if(character.weapon[whichweapon]==0)
                strcpy(character.equipped_weapon,"Nothing");
        }
        if(order=='i' || order=='I'){
            clear();
            refresh();
            weaponsmenu(&character);
            if(ismaptrue)
                drawmap(board,visible);
            else
                drawmapfalse(board);
            drawcharacter(board,character.color,character.point.x,character.point.y);
        }
        if(order=='o' || order=='O'){
            clear();
            refresh();
            spells=spellmenu(&character);
            spelltimer=10;
            if(ismaptrue)
                drawmap(board,visible);
            else
                drawmapfalse(board);
            drawcharacter(board,character.color,character.point.x,character.point.y);
        }
        if(spells%10==1)
            spellD=2;
        if(spells>=100)
            spellH=2;

        if(order=='s' || order=='S'){
            savegame(rooms,character,board,visible,room_count,name,enemy);
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
        if (order=='W' || order=='w'){
            showmsg("You put your weapon in your backpack",1);
            strcpy(character.equipped_weapon,"Nothing");
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=1;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
            else if(board[character.point.y -1][character.point.x -1]=='#'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                wasitacor=1;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
                else if(board[character.point.y-1][character.point.x]=='#'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                wasitacor=1;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=1;
                 wasitastair=0;
                 wasitacor=0;
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
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
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
            else if(board[character.point.y -1][character.point.x +1]=='#'){
                didyoumove=1;
               if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                wasitacor=1;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=1;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
            else if(board[character.point.y][character.point.x -1]=='#'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                wasitacor=1;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=1;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
        else if(board[character.point.y][character.point.x +1]=='#'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                wasitacor=1;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=1;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
            else if(board[character.point.y +1][character.point.x -1]=='#'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                wasitacor=1;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=1;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
            else if(board[character.point.y +1][character.point.x]=='#'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                wasitacor=1;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=1;
                 wasitastair=0;
                 wasitacor=0;
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
            else if(board[character.point.y +1][character.point.x +1]=='#'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                wasitacor=1;
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
            if(board[character.point.y -1][character.point.x -1]=='@'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                showmsg("You Got a Piece of Food!",3);
                refresh();
                refresh();

            }
            else if(board[character.point.y -1][character.point.x -1]=='g'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                character.point.y-=1;
                character.point.x-=1;
                character.gold+=randit(1,10);
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                showmsg("You Found Some Gold!",5);
                refresh();
                refresh();

            }
            else if(board[character.point.y -1][character.point.x -1]=='*'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                wasitadoor=0;
                wasitastair=0;
                character.point.y-=1;
                character.point.x-=1;
                character.gold+=randit(15,25);
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                showmsg("You Found Some Black Gold!",5);
                refresh();
                refresh();

            }
        }
       if(order== '8' && !didyoumove){
                if(board[character.point.y-1][character.point.x]=='@'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
                showmsg("You Got a Piece of Food!",3);
                refresh();
                refresh();
                }
                else if(board[character.point.y-1][character.point.x]=='g'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                character.gold+=randit(1,10);
                character.point.y-=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                showmsg("You Found Some Gold!",5);
                refresh();
                refresh();
                }
                else if(board[character.point.y-1][character.point.x]=='*'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
                showmsg("You Found Some Black Gold!",5);
                refresh();
                refresh();
                }

            
        }
     if(order== '9' && !didyoumove){
            if(board[character.point.y -1][character.point.x +1]=='@'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                showmsg("You Got a Piece of Food!",3);
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                showmsg("You Found Some Gold!",5);
                refresh();
                refresh();
            }
            else if(board[character.point.y -1][character.point.x +1]=='*'){
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
                showmsg("You Found Some Black Gold!",5);
                refresh();
                refresh();
            }
        }
       if(order== '4' && !didyoumove){
            if(board[character.point.y][character.point.x -1]=='@'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                character.food+=1;
                character.point.x-=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                showmsg("You Got a Piece of Food!",3);
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                character.gold+=randit(1,10);
                character.point.x-=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                showmsg("You Found Some Gold!",5);
                refresh();
                refresh();

            }
            else if(board[character.point.y][character.point.x -1]=='*'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
                showmsg("You Found Some Black Gold!",5);
                refresh();
                refresh();

            }
            
        }
       if(order== '6' && !didyoumove){
            if(board[character.point.y][character.point.x +1]=='@'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                character.food+=1;
                character.point.x+=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                showmsg("You Got a Piece of Food!",3);
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                character.gold+=randit(1,10);
                character.point.x+=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                showmsg("You Found Some Gold!",5);
                refresh();
                refresh();

            }
        else if(board[character.point.y][character.point.x +1]=='*'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
                showmsg("You Found Some Black Gold!",5);
                refresh();
                refresh();

            }
            
        }
        if(order== '1' && !didyoumove){
            if(board[character.point.y +1][character.point.x -1]=='@'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                showmsg("You Got a Piece of Food!",3);
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                showmsg("You Found Some Gold!",5);
                refresh();
                refresh();

            }
            else if(board[character.point.y +1][character.point.x -1]=='*'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
                showmsg("You Found Some Black Gold!",5);
                refresh();
                refresh();

            }
            
        }
      if(order== '2' && !didyoumove){
            if(board[character.point.y +1][character.point.x]=='@'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                character.food+=1;
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                character.point.y+=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                showmsg("You Got a Piece of Food!",3);
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                character.gold+=randit(1,10);
                character.point.y+=1;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                drawcharacter(board,character.color,character.point.x,character.point.y);
                showmsg("You Found Some Gold!",5);
                refresh();
                refresh();
                refresh();

            }
            else if(board[character.point.y +1][character.point.x]=='*'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
                showmsg("You Found Some Black Gold!",5);
                refresh();
                refresh();

            }      
        }
       if(order== '3' && !didyoumove){
            if(board[character.point.y +1][character.point.x +1]=='@'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                showmsg("You Got a Piece of Food!",3);
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
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
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
                showmsg("You Found Some Gold!",5);
                refresh();

            }
            else if(board[character.point.y +1][character.point.x +1]=='*'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
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
                showmsg("You Found Some Black Gold!",5);
                refresh();
                refresh();

            }
            
            
        }
        if(order== '7' && !didyoumove){
            if(board[character.point.y -1][character.point.x -1]=='A' || board[character.point.y -1][character.point.x -1]=='W' || board[character.point.y -1][character.point.x -1]=='N' 
               || board[character.point.y -1][character.point.x -1]=='R' || board[character.point.y -1][character.point.x -1]=='d'|| board[character.point.y -1][character.point.x -1]=='s'
               ||  board[character.point.y -1][character.point.x -1]=='h'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                if(board[character.point.y -1][character.point.x -1]=='A' ){
                    showmsg("You Picked up 10 Daggers!",2);
                    character.weapon[1]+=10;
                }
                if(board[character.point.y -1][character.point.x -1]=='W' ){
                    showmsg("You Picked up 8 Magic Wands!",2);
                    character.weapon[2]+=8;
                }
                if(board[character.point.y -1][character.point.x -1]=='N' ){
                    showmsg("You Picked up 20 Arrows!",2);
                    character.weapon[3]+=20;
                }
                if(board[character.point.y -1][character.point.x -1]=='R' ){
                    showmsg("You Picked up a Sword!",2);
                    character.weapon[4]+=1;
                }
                if(board[character.point.y -1][character.point.x -1]=='d' ){
                    showmsg("You found a Damage Potion!",10);
                    character.spell[2]+=1;
                }
                if(board[character.point.y -1][character.point.x -1]=='s' ){
                    showmsg("You found a Speed Potion!",10);
                    character.spell[1]+=1;
                }
                if(board[character.point.y -1][character.point.x -1]=='h' ){
                    showmsg("You found a Health Potion!",10);
                    character.spell[0]+=1;
                }
                refresh();
                character.point.y-=1;
                character.point.x-=1;
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
            }
        }
        if(order== '8' && !didyoumove){
            if(board[character.point.y -1][character.point.x]=='A' || board[character.point.y -1][character.point.x]=='W' || board[character.point.y -1][character.point.x]=='N' 
               || board[character.point.y -1][character.point.x]=='R' || board[character.point.y -1][character.point.x]=='d'|| board[character.point.y -1][character.point.x]=='s'
               ||  board[character.point.y -1][character.point.x]=='h'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                if(board[character.point.y -1][character.point.x]=='A' ){
                    showmsg("You Picked up 10 Daggers!",2);
                    character.weapon[1]+=10;
                }
                if(board[character.point.y -1][character.point.x]=='W' ){
                    showmsg("You Picked up 8 Magic Wands!",2);
                    character.weapon[2]+=8;
                }
                if(board[character.point.y -1][character.point.x]=='N' ){
                    showmsg("You Picked up 20 Arrows!",2);
                    character.weapon[3]+=20;
                }
                if(board[character.point.y -1][character.point.x]=='R' ){
                    showmsg("You Picked up a Sword!",2);
                    character.weapon[4]+=1;
                }
                if(board[character.point.y -1][character.point.x]=='d' ){
                    showmsg("You found a Damage Potion!",10);
                    character.spell[2]+=1;
                }
                if(board[character.point.y -1][character.point.x]=='s' ){
                    showmsg("You found a Speed Potion!",10);
                    character.spell[1]+=1;
                }
                if(board[character.point.y -1][character.point.x]=='h' ){
                    showmsg("You found a Health Potion!",10);
                    character.spell[0]+=1;
                }
                refresh();
                character.point.y-=1;
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
            }
        }
        if(order== '9' && !didyoumove){
            if(board[character.point.y -1][character.point.x+1]=='A' || board[character.point.y -1][character.point.x+1]=='W' || board[character.point.y -1][character.point.x+1]=='N' 
               || board[character.point.y -1][character.point.x+1]=='R' || board[character.point.y -1][character.point.x+1]=='d'|| board[character.point.y -1][character.point.x+1]=='s'
               ||  board[character.point.y -1][character.point.x+1]=='h'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                if(board[character.point.y -1][character.point.x+1]=='A' ){
                    showmsg("You Picked up 10 Daggers!",2);
                    character.weapon[1]+=10;
                }
                if(board[character.point.y -1][character.point.x+1]=='W' ){
                    showmsg("You Picked up 8 Magic Wands!",2);
                    character.weapon[2]+=8;
                }
                if(board[character.point.y -1][character.point.x+1]=='N' ){
                    showmsg("You Picked up 20 Arrows!",2);
                    character.weapon[3]+=20;
                }
                if(board[character.point.y -1][character.point.x+1]=='R' ){
                    showmsg("You Picked up a Sword!",2);
                    character.weapon[4]+=1;
                }
                if(board[character.point.y -1][character.point.x+1]=='d' ){
                    showmsg("You found a Damage Potion!",10);
                    character.spell[2]+=1;
                }
                if(board[character.point.y -1][character.point.x+1]=='s' ){
                    showmsg("You found a Speed Potion!",10);
                    character.spell[1]+=1;
                }
                if(board[character.point.y -1][character.point.x+1]=='h' ){
                    showmsg("You found a Health Potion!",10);
                    character.spell[0]+=1;
                }
                refresh();
                character.point.y-=1;
                character.point.x+=1;
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
            }
        }
        if(order== '4' && !didyoumove){
            if(board[character.point.y][character.point.x-1]=='A' || board[character.point.y][character.point.x-1]=='W' || board[character.point.y][character.point.x-1]=='N' 
               || board[character.point.y][character.point.x-1]=='R' || board[character.point.y][character.point.x-1]=='d'|| board[character.point.y][character.point.x-1]=='s'
               ||  board[character.point.y][character.point.x-1]=='h'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                if(board[character.point.y][character.point.x-1]=='A' ){
                    showmsg("You Picked up 10 Daggers!",2);
                    character.weapon[1]+=10;
                }
                if(board[character.point.y][character.point.x-1]=='W' ){
                    showmsg("You Picked up 8 Magic Wands!",2);
                    character.weapon[2]+=8;
                }
                if(board[character.point.y][character.point.x-1]=='N' ){
                    showmsg("You Picked up 20 Arrows!",2);
                    character.weapon[3]+=20;
                }
                if(board[character.point.y][character.point.x-1]=='R' ){
                    showmsg("You Picked up a Sword!",2);
                    character.weapon[4]+=1;
                }
                if(board[character.point.y][character.point.x-1]=='d' ){
                    showmsg("You found a Damage Potion!",10);
                    character.spell[2]+=1;
                }
                if(board[character.point.y][character.point.x-1]=='s' ){
                    showmsg("You found a Speed Potion!",10);
                    character.spell[1]+=1;
                }
                if(board[character.point.y][character.point.x-1]=='h' ){
                    showmsg("You found a Health Potion!",10);
                    character.spell[0]+=1;
                }
                refresh();
                character.point.x-=1;
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
            }
        }
        if(order== '6' && !didyoumove){
            if(board[character.point.y][character.point.x+1]=='A' || board[character.point.y][character.point.x+1]=='W' || board[character.point.y][character.point.x+1]=='N' 
               || board[character.point.y][character.point.x+1]=='R' || board[character.point.y][character.point.x+1]=='d'|| board[character.point.y][character.point.x+1]=='s'
               ||  board[character.point.y][character.point.x+1]=='h'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                if(board[character.point.y][character.point.x+1]=='A' ){
                    showmsg("You Picked up 10 Daggers!",2);
                    character.weapon[1]+=10;
                }
                if(board[character.point.y][character.point.x+1]=='W' ){
                    showmsg("You Picked up 8 Magic Wands!",2);
                    character.weapon[2]+=8;
                }
                if(board[character.point.y][character.point.x+1]=='N' ){
                    showmsg("You Picked up 20 Arrows!",2);
                    character.weapon[3]+=20;
                }
                if(board[character.point.y][character.point.x+1]=='R' ){
                    showmsg("You Picked up a Sword!",2);
                    character.weapon[4]+=1;
                }
                if(board[character.point.y][character.point.x+1]=='d' ){
                    showmsg("You found a Damage Potion!",10);
                    character.spell[2]+=1;
                }
                if(board[character.point.y][character.point.x+1]=='s' ){
                    showmsg("You found a Speed Potion!",10);
                    character.spell[1]+=1;
                }
                if(board[character.point.y][character.point.x+1]=='h' ){
                    showmsg("You found a Health Potion!",10);
                    character.spell[0]+=1;
                }
                refresh();
                character.point.x+=1;
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
            }
        }
        if(order== '1' && !didyoumove){
            if(board[character.point.y+1][character.point.x-1]=='A' || board[character.point.y+1][character.point.x-1]=='W' || board[character.point.y+1][character.point.x-1]=='N' 
               || board[character.point.y+1][character.point.x-1]=='R' || board[character.point.y+1][character.point.x-1]=='d'|| board[character.point.y+1][character.point.x-1]=='s'
               ||  board[character.point.y+1][character.point.x-1]=='h'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                if(board[character.point.y+1][character.point.x-1]=='A' ){
                    showmsg("You Picked up 10 Daggers!",2);
                    character.weapon[1]+=10;
                }
                if(board[character.point.y+1][character.point.x-1]=='W' ){
                    showmsg("You Picked up 8 Magic Wands!",2);
                    character.weapon[2]+=8;
                }
                if(board[character.point.y+1][character.point.x-1]=='N' ){
                    showmsg("You Picked up 20 Arrows!",2);
                    character.weapon[3]+=20;
                }
                if(board[character.point.y+1][character.point.x-1]=='R' ){
                    showmsg("You Picked up a Sword!",2);
                    character.weapon[4]+=1;
                }
                if(board[character.point.y+1][character.point.x-1]=='d' ){
                    showmsg("You found a Damage Potion!",10);
                    character.spell[2]+=1;
                }
                if(board[character.point.y+1][character.point.x-1]=='s' ){
                    showmsg("You found a Speed Potion!",10);
                    character.spell[1]+=1;
                }
                if(board[character.point.y+1][character.point.x-1]=='h' ){
                    showmsg("You found a Health Potion!",10);
                    character.spell[0]+=1;
                }
                refresh();
                character.point.x-=1;
                character.point.y+=1;
                refresh();
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
            }
        }
        if(order== '2' && !didyoumove){
            if(board[character.point.y+1][character.point.x]=='A' || board[character.point.y+1][character.point.x]=='W' || board[character.point.y+1][character.point.x]=='N' 
               || board[character.point.y+1][character.point.x]=='R' || board[character.point.y+1][character.point.x]=='d'|| board[character.point.y+1][character.point.x]=='s'
               ||  board[character.point.y+1][character.point.x]=='h'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                if(board[character.point.y+1][character.point.x]=='A' ){
                    showmsg("You Picked up 10 Daggers!",2);
                    character.weapon[1]+=10;
                }
                if(board[character.point.y+1][character.point.x]=='W' ){
                    showmsg("You Picked up 8 Magic Wands!",2);
                    character.weapon[2]+=8;
                }
                if(board[character.point.y+1][character.point.x]=='N' ){
                    showmsg("You Picked up 20 Arrows!",2);
                    character.weapon[3]+=20;
                }
                if(board[character.point.y+1][character.point.x]=='R' ){
                    showmsg("You Picked up a Sword!",2);
                    character.weapon[4]+=1;
                }
                if(board[character.point.y+1][character.point.x]=='d' ){
                    showmsg("You found a Damage Potion!",10);
                    character.spell[2]+=1;
                }
                if(board[character.point.y+1][character.point.x]=='s' ){
                    showmsg("You found a Speed Potion!",10);
                    character.spell[1]+=1;
                }
                if(board[character.point.y+1][character.point.x]=='h' ){
                    showmsg("You found a Health Potion!",10);
                    character.spell[0]+=1;
                }
                refresh();
                character.point.y+=1;
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
            }
        }
        if(order== '3' && !didyoumove){
            if(board[character.point.y+1][character.point.x+1]=='A' || board[character.point.y+1][character.point.x+1]=='W' || board[character.point.y+1][character.point.x+1]=='N' 
               || board[character.point.y+1][character.point.x+1]=='R' || board[character.point.y+1][character.point.x+1]=='d'|| board[character.point.y+1][character.point.x+1]=='s'
               ||  board[character.point.y+1][character.point.x+1]=='h'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                clear();
                if(ismaptrue)
                    drawmap(board,visible);
                else
                    drawmapfalse(board);
                refresh();
                if(board[character.point.y+1][character.point.x+1]=='A' ){
                    showmsg("You Picked up 10 Daggers!",2);
                    character.weapon[1]+=10;
                }
                if(board[character.point.y+1][character.point.x+1]=='W' ){
                    showmsg("You Picked up 8 Magic Wands!",2);
                    character.weapon[2]+=8;
                }
                if(board[character.point.y+1][character.point.x+1]=='N' ){
                    showmsg("You Picked up 20 Arrows!",2);
                    character.weapon[3]+=20;
                }
                if(board[character.point.y+1][character.point.x+1]=='R' ){
                    showmsg("You Picked up a Sword!",2);
                    character.weapon[4]+=1;
                }
                if(board[character.point.y+1][character.point.x+1]=='d' ){
                    showmsg("You found a Damage Potion!",10);
                    character.spell[2]+=1;
                }
                if(board[character.point.y+1][character.point.x+1]=='s' ){
                    showmsg("You found a Speed Potion!",10);
                    character.spell[1]+=1;
                }
                if(board[character.point.y+1][character.point.x+1]=='h' ){
                    showmsg("You found a Health Potion!",10);
                    character.spell[0]+=1;
                }
                refresh();
                character.point.y+=1;
                character.point.x+=1;
                drawcharacter(board,character.color,character.point.x,character.point.y);
                refresh();
            }
        }
        if(order== '7' && !didyoumove){
            if(board[character.point.y -1][character.point.x -1]=='$'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                 treasureroom(character,name);
            }
        }
       if(order== '8' && !didyoumove){
                if(board[character.point.y-1][character.point.x]=='$'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                wasitadoor=0;
                wasitastair=0;
                treasureroom(character,name);
            }
       }
        if(order== '9' && !didyoumove){
            if(board[character.point.y -1][character.point.x +1]=='$'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                 treasureroom(character,name);
            }
        }
        if(order== '4' && !didyoumove){
            if(board[character.point.y][character.point.x -1]=='$'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                 treasureroom(character,name);
            }
        }
        if(order== '6' && !didyoumove){
            if(board[character.point.y][character.point.x +1]=='$'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                 treasureroom(character,name);
            }
        }
        if(order== '1' && !didyoumove){
            if(board[character.point.y +1][character.point.x -1]=='$'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                 treasureroom(character,name);
            }
        }
        if(order== '2' && !didyoumove){
            if(board[character.point.y +1][character.point.x]=='$'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                 treasureroom(character,name);
            }
        }
        if(order== '3' && !didyoumove){
            if(board[character.point.y +1][character.point.x +1]=='$'){
                didyoumove=1;
                if(!wasitadoor)
                    board[character.point.y][character.point.x]='.';
                if(wasitadoor)
                    board[character.point.y][character.point.x]='+';
                if(wasitastair)
                    board[character.point.y][character.point.x]='<';
                 if(wasitacor)
                    board[character.point.y][character.point.x]='#';
                 wasitadoor=0;
                 wasitastair=0;
                 wasitacor=0;
                 treasureroom(character,name);
            }
        }
        


    if(didyoumove || attacked){
        time++;
        if(!spelltimer)
            spelltimer--;
        enemyaction(board,enemy,&character);
    }
    if(!spelltimer){
        spells=000;
        spellD=1;
        spellH=1;
        spellS=1;
    }
    if(time%4==2 && character.food_status==4 && character.health!=200  && WHENEATFOOD!=time){
        character.health+=4 * spellH;
        if(character.health!=204)
            showmsg("You recovered some Health!",3);
        if(character.health>200)
            character.health=200;
        WHENEATFOOD=time;
    }
    if(time%16==15  && IMSTARVING!=time && character.food_status!=1){
        character.food_status--;
        IMSTARVING=time;
    }
    if(time%10==4 && IREALLYAMSTARVING!=time && character.food_status==1){
        character.health-=character.difficulty*2;
        IREALLYAMSTARVING=time;
        showmsg("YOU'RE STARVING!!!",2);
    }
    if(floor==4)
        for(int i=0;i<10;i++){
            if(enemy[i].exist)
                break;
            if(i==9)
                VICTORY(name,character);
        }




    if(character.health<=0)
        gameoverscreen(character,name);
    showstats(character);
    lightupplayer(visible,character.point.x,character.point.y);
    if(wasitadoor){
        lightupplayersroom(visible,character.point.x,character.point.y,rooms,room_count);
    }
    usleep(100);
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
    generatefoodgoldblackgold(board,rooms,room_count,character.difficulty);
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
    character.food_status=4;
    character.gold=0;
    character.food=2;
    character.key=0;
    character.health=200;
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
    strcpy(character.equipped_weapon,"Mace");
    character.floor=1;
    enemy enemy[10];
    //Demon
    enemy[0].attack=3*difficulty;
    enemy[0].health= 5;
    enemy[0].followtype=1;
    enemy[0].point.x=0;
    enemy[0].point.y=0;
    enemy[0].exist=0;
    enemy[0].doifollow=0;
    enemy[0].type='D';
    enemy[1]=enemy[0];
    //FIIIRE
    enemy[2].attack=5*difficulty;
    enemy[2].health=10;
    enemy[2].followtype=1;
    enemy[2].point.x=0;
    enemy[2].point.y=0;
    enemy[2].exist=0;
    enemy[2].doifollow=0;
    enemy[2].type='F';
    enemy[3]=enemy[2];
    //GIANT
    enemy[4].attack=7*difficulty;
    enemy[4].health=15;
    enemy[4].followtype=2;
    enemy[4].point.x=0;
    enemy[4].point.y=0;
    enemy[4].exist=0;
    enemy[4].doifollow=0;
    enemy[4].type='G';
    enemy[5]=enemy[4];
    //Snake
    enemy[6].attack=8*difficulty;
    enemy[6].health=20;
    enemy[6].followtype=3;
    enemy[6].point.x=0;
    enemy[6].point.y=0;
    enemy[6].exist=0;
    enemy[6].doifollow=0;
    enemy[6].type='S';
    enemy[7]=enemy[6];
    //The Undead
    enemy[8].attack=12*difficulty;
    enemy[8].health=30;
    enemy[8].followtype=2;
    enemy[8].point.x=0;
    enemy[8].point.y=0;
    enemy[8].exist=0;
    enemy[8].doifollow=0;
    enemy[8].type='U';
    enemy[9]=enemy[8];
    setupenemy(enemy,board,rooms,room_count);
    play_game(rooms,character,board,visible,room_count,floor,name,enemy);
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
