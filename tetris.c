#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define HEIGHT 19
#define WIDTH 18

char map[HEIGHT][WIDTH];

void *input();
void *tatris_print();

void invisible();
void display_init();

void new_block(int f_rand, int d_rand);

void move_right();
void move_left();
void move_down();
void move_turn();

void block_clear();
int block_chek(int x);
void block_erase();
void block_wirte();

typedef struct {
    int x, y, f, d;
} Block;

Block BB;

int block[7][4][2][4] = {{{{-1, 0, 1, 2}, // 상 // I
                           {0, 0, 0, 0}}, 
                          {{0, 0, 0, 0},  // 우
                           {-1, 0, 1, 2}},
                          {{-2, -1, 0, 1}, // 하
                           {0, 0, 0, 0}},
                          {{0, 0, 0, 0},   // 좌
                           {-2, -1, 0, 1}}},
 
                         {{{0, 1, 0, 1}, // 상 // namo
                           {0, 0, 1, 1}}, 
                          {{0, 1, 0, 1},  // 우
                           {0, 0, 1, 1}},
                          {{0, 1, 0, 1}, // 하
                           {0, 0, 1, 1}},
                          {{0, 1, 0, 1},   // 좌
                           {0, 0, 1, 1}}},

                         {{{0, 1, 0, 0}, // 상 // L
                           {0, 0, -1, -2}}, 
                          {{0, 1, 2, 0},  // 우
                           {0, 0, 0, 1}},
                          {{0, -1, 0, 0}, // 하
                           {0, 0, 1, 2}},
                          {{0, -1, -2, 0},   // 좌
                           {0, 0, 0, -1}}},
                           
                         {{{0, -1, 0, 0}, // 상 // RL
                           {0, 0, -1, -2}}, 
                          {{0, 0, 1, 2},  // 우
                           {0, -1, 0, 0}},
                          {{0, 1, 0, 0}, // 하
                           {0, 0, 1, 2}},
                          {{0, 0, -1, -2},   // 좌
                           {0, 1, 0, 0}}},

                         {{{0, 1, 0, -1}, // 상 // N
                           {0, 0, -1, -1}}, 
                          {{0, 0, 1, 1},  // 우
                           {0, 1, 0, -1}},
                          {{0, 1, 0, -1}, // 하
                           {0, 0, -1, -1}},
                          {{0, 0, 1, 1},   // 좌
                           {0, 1, 0, -1}}},

                         {{{0, -1, 0, 1}, // 상 // RN
                           {0, 0, -1, -1}}, 
                          {{0, 0, 1, 1},  // 우
                           {0, -1, 0, 1}},
                          {{0, -1, 0, 1}, // 하
                           {0, 0, -1, -1}},
                          {{0, 0, 1, 1},   // 좌
                           {0, -1, 0, 1}}},

                         {{{0, -1, 1, 0}, // 상 // t
                           {0, 0, 0, -1}}, 
                          {{0, 1, 0, 0},  // 우
                           {0, 0, -1, 1}},
                          {{0, 0, -1, 1}, // 하
                           {0, 1, 0, 0}},
                          {{0, -1, 0, 0},   // 좌
                           {0, 0, -1, 1}}}
                           };


int main(void)
{
    system("clear");
    invisible();
    display_init();

    srand(time(NULL));
	
    new_block(rand()%7, rand()%4);
    
    pthread_t pt1, pt2;

    pthread_create(&pt1, NULL, input, NULL);
    pthread_create(&pt2, NULL, tatris_print, NULL);

    pthread_join(pt1, NULL);
    pthread_join(pt2, NULL);

    return 0;
}

void display_init(){
    for (int i = 0; i < HEIGHT-1; i++)
        memcpy(map[i],"I               I\0", WIDTH);
    memcpy(map[HEIGHT-1],"IIIIIIIIIIIIIIIII\0", WIDTH);
}


void *tatris_print(){
    while(1){
        printf(u8"\033[1;1H");
        for (int i = 0; i < HEIGHT; i++){
            printf("%s\n", map[i]);
        }
        usleep(1000);
    }
}


void *input(){
    char c;
    while (read(0, &c, sizeof(c)) > 0){
        switch(c){   
            case 'q':
                block_erase();
                BB.f = (BB.f+1)%7;
                block_wirte();                
                break;
            case 'e':
                move_turn();
                break;
            case 'w':
                block_erase();
                --BB.y;
                block_wirte();
                break;
            case 's':
                move_down();
                break;
            case 'a':
                move_left();
                break;
            case 'd':
                move_right();
                break;
        }
        
        usleep(1000);
    }
}



int block_chek(int x){
    int i;
    
    switch (x)
    {
    case 0: // 오른쪽 
        for (i = 0; i < 4; i++)
            if(map[BB.y+block[BB.f][BB.d][1][i]][BB.x+block[BB.f][BB.d][0][i] + 1] == 'I')
                return 0;
        break;
    case 1: // 왼쪽
        for (i = 0; i < 4; i++)
            if(map[BB.y+block[BB.f][BB.d][1][i]][BB.x+block[BB.f][BB.d][0][i] - 1] == 'I')
                return 0;
        break;
    case 2: // 아래
        for (i = 0; i < 4; i++)
            if(map[BB.y+block[BB.f][BB.d][1][i]+1][BB.x+block[BB.f][BB.d][0][i]] == 'I')
                return 0;
        break;
    case 3: // 회전
        return 1;
        break;
    }
    
    return 1;
}

void move_right(){
    
    if(!block_chek(0))
        return ;

    block_erase();
    ++BB.x;
    block_wirte();
}

void move_left(){
    if(!block_chek(1))
        return ;

    block_erase();
    --BB.x;
    block_wirte();
}

void move_down(){
    if(!block_chek(2)){
        for (int i = 0; i < 4; i++)
            map[BB.y+block[BB.f][BB.d][1][i]][BB.x+block[BB.f][BB.d][0][i]] = 'I';

        block_clear();
        new_block(rand()%7, rand()%4);
        return ;
    }

    block_erase();
    ++BB.y;
    block_wirte();        
}

void move_turn(){

    

    block_erase();
    BB.d = (BB.d +1)%4;
    block_wirte();
}

void new_block(int f_rand, int d_rand){
    BB.y = 2;
    BB.x = WIDTH/2;
    BB.f = f_rand; // 랜덤
    BB.d = d_rand; // 랜덤

    block_wirte();
}

int I_chek(int idx){
    for (int i = 1; i < WIDTH-2; i++){
        if (map[idx][i] != 'I'){
            return 0;
        }
    }   
    return 1;
}

int empty_chek(int idx){
    for (int i = 1; i < WIDTH-2; i++){
        if (map[idx][i] != ' '){
            return 0;
        }
    }   
    return 1;
}

void block_clear(){
    for (int i = HEIGHT-2; i >= 0; i--){ // 꽉찬 줄 지우기
        if(I_chek(i)){
            for (int j = 1; j < WIDTH-2; j++){
                map[i][j] = ' ';
            }
        }
    }


    int empty = HEIGHT-2, full;

    while (full > 0){
        while(empty>0 && !empty_chek(empty)) empty--;
        full = empty-1;
        while (full>0 && empty_chek(full)) full--;

        for (int i = 1; i < WIDTH-2; i++){
            map[empty][i] = map[full][i];
            map[full][i] = ' ';
        }
    }    
}

void block_erase(){
    for (int i = 0; i < 4; i++)
            map[BB.y+block[BB.f][BB.d][1][i]][BB.x+block[BB.f][BB.d][0][i]] = ' ';
}
void block_wirte(){
    for (int i = 0; i < 4; i++)
        map[BB.y+block[BB.f][BB.d][1][i]][BB.x+block[BB.f][BB.d][0][i]] = 'O';
}

void invisible(){
    struct termios term;
    
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~ICANON;    // non-canonical input 설정
	term.c_lflag &= ~ECHO;      // 입력 시 터미널에 보이지 않게
	term.c_cc[VMIN] = 1;        // 최소 입력 버퍼 크기
	term.c_cc[VTIME] = 0;       //버퍼 비우는 시간 (timeout)
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}


