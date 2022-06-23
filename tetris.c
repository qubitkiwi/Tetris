#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define HEIGHT 19
#define WIDTH 18

char map[HEIGHT][WIDTH];

void *input();
void *tatris_print();

void invisible();
void display_init();
void block_move(int x);
int block_chek(int x);
void block_erase();
void block_wirte();

typedef struct {
    int x, y, f, d;
} Block;

Block BB;

  // 위치, xy, 0~3
// int I[4][2][4] = {{{-1, 0, 1, 2}, // 상
//                    {0, 0, 0, 0}}, 
//                   {{0, 0, 0, 0},  // 우
//                    {-1, 0, 1, 2}},
//                   {{-2, -1, 0, 1}, // 하
//                    {0, 0, 0, 0}},
//                   {{0, 0, 0, 0},   // 좌
//                    {-2, -1, 0, 1}}};

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

                         {{{0, 1, 0, 2}, // 상 // L
                           {0, 0, -1, 0}}, 
                          {{0, -1, 0, 0},  // 우
                           {0, 0, 1, 2}},
                          {{0, -1, 0, 0}, // 하
                           {0, 0, 1, 1}},
                          {{0, 1, 0, 0},   // 좌
                           {0, 0, -1, -2}}},
                           
                          {{{0, 1, 0, 2}, // 상 // RL
                           {0, 0, -1, 0}}, 
                          {{0, -1, 0, 0},  // 우
                           {0, 0, 1, 2}},
                          {{0, -1, 0, 0}, // 하
                           {0, 0, 1, 1}},
                          {{0, 1, 0, 0},   // 좌
                           {0, 0, -1, -2}}},

                          {{{0, 1, 0, 2}, // 상 // N
                           {0, 0, -1, 0}}, 
                          {{0, -1, 0, 0},  // 우
                           {0, 0, 1, 2}},
                          {{0, -1, 0, 0}, // 하
                           {0, 0, 1, 1}},
                          {{0, 1, 0, 0},   // 좌
                           {0, 0, -1, -2}}},

                          {{{0, 1, 0, 2}, // 상 // RN
                           {0, 0, -1, 0}}, 
                          {{0, -1, 0, 0},  // 우
                           {0, 0, 1, 2}},
                          {{0, -1, 0, 0}, // 하
                           {0, 0, 1, 1}},
                          {{0, 1, 0, 0},   // 좌
                           {0, 0, -1, -2}}},

                          {{{0, 1, 0, 2}, // 상 // t
                           {0, 0, -1, 0}}, 
                          {{0, -1, 0, 0},  // 우
                           {0, 0, 1, 2}},
                          {{0, -1, 0, 0}, // 하
                           {0, 0, 1, 1}},
                          {{0, 1, 0, 0},   // 좌
                           {0, 0, -1, -2}}},



                           };


int main(void)
{
    system("clear");
    invisible();
    display_init();
	
    BB.y = 2;
    BB.x = WIDTH/2;
    BB.f = 0;
    BB.d = 0;

    block_wirte();
    
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
                block_move(3);
                break;
            case 'w':
                block_erase();
                --BB.y;
                block_wirte();
                break;
            case 's':
                block_move(2);
                break;
            case 'a':
                block_move(1);
                break;
            case 'd':
                block_move(0);
                break;
        }
        
        usleep(1000);
    }
}

void block_move(int x){
    
    if(block_chek(x) == 0)
        return ;
        
    block_erase();
    switch (x)
    {
    case 0: // 오른쪽
        ++BB.x;
        break;
    case 1: // 왼쪽    
        --BB.x;
        break;
    case 2: // 아래
        ++BB.y;
        break;
    case 3: // 회전
        BB.d = (BB.d+1)%4;
        break;
    
    }
    block_wirte();
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