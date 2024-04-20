#include "definitions.h"
#include "PORT.h"

char Table[ROWS][COLS] = {0};
int score = 0;
char GameOn = TRUE;
suseconds_t timer = 400000; // decrease this to make it faster
int decrease = 1000;

typedef struct {
    char **array;
    int width, row, col;
} Shape;
Shape current;

const Shape ShapesArray[7] = {
    {(char *[]){(char []){0,1,1},(char []){1,1,0}, (char []){0,0,0}}, 3, 0, 0}, // S shape
    {(char *[]){(char []){1,1,0},(char []){0,1,1}, (char []){0,0,0}}, 3, 0, 0}, // Z shape
    {(char *[]){(char []){0,1,0},(char []){1,1,1}, (char []){0,0,0}}, 3, 0, 0}, // T shape
    {(char *[]){(char []){0,0,1},(char []){1,1,1}, (char []){0,0,0}}, 3, 0, 0}, // L shape
    {(char *[]){(char []){1,0,0},(char []){1,1,1}, (char []){0,0,0}}, 3, 0, 0}, // flipped L shape
    {(char *[]){(char []){1,1},(char []){1,1}}, 2, 0, 0},                         // square shape
    {(char *[]){(char []){0,0,0,0}, (char []){1,1,1,1}, (char []){0,0,0,0}, (char []){0,0,0,0}}, 4, 0, 0} // long bar shape
};


Shape CopyShape(Shape shape){
    Shape new_shape = shape;
    char **copyshape = shape.array;
    new_shape.array = (char**)malloc(new_shape.width*sizeof(char*));
    int i, j;
    for(i = 0; i < new_shape.width; i++){
        new_shape.array[i] = (char*)malloc(new_shape.width*sizeof(char));
        for(j=0; j < new_shape.width; j++) {
            new_shape.array[i][j] = copyshape[i][j];
        }
    }
    return new_shape;
}

void DeleteShape(Shape shape){
    int i;
    for(i = 0; i < shape.width; i++){
        free(shape.array[i]);
    }
    free(shape.array);
}

int CheckPosition(Shape shape){ //Check the position of the copied shape
    char **array = shape.array;
    int i, j;
    for(i = 0; i < shape.width;i++) {
        for(j = 0; j < shape.width ;j++){
            if((shape.col+j < 0 || shape.col+j >= COLS || shape.row+i >= ROWS)){ //Out of borders
                if(array[i][j]) //but is it just a phantom?
                    return FALSE;
                
            }
            else if(Table[shape.row+i][shape.col+j] && array[i][j])
                return FALSE;
        }
    }
    return TRUE;
}

void SetNewRandomShape(){ //updates [current] with new shape
    Shape new_shape = CopyShape(ShapesArray[rand()%7]);

    new_shape.col = rand()%(COLS-new_shape.width+1);
    new_shape.row = 0;
    DeleteShape(current);
    current = new_shape;
    if(!CheckPosition(current)){
        GameOn = FALSE;
    }
}

void RotateShape(Shape shape){ //rotates clockwise
    Shape temp = CopyShape(shape);
    int i, j, k, width;
    width = shape.width;
    for(i = 0; i < width ; i++){
        for(j = 0, k = width-1; j < width ; j++, k--){
                shape.array[i][j] = temp.array[k][i];
        }
    }
    DeleteShape(temp);
}

void WriteToTable(){
    int i, j;
    for(i = 0; i < current.width ;i++){
        for(j = 0; j < current.width ; j++){
            if(current.array[i][j])
                Table[current.row+i][current.col+j] = current.array[i][j];
        }
    }
}

void RemoveFullRowsAndUpdateScore(){
    int i, j, sum, count=0;
    for(i=0;i<ROWS;i++){
        sum = 0;
        for(j=0;j< COLS;j++)
{
            sum+=Table[i][j];
        }
        if(sum==COLS){
            count++;
            int l, k;
            for(k = i;k >=1;k--)
                for(l=0;l<COLS;l++)
                    Table[k][l]=Table[k-1][l];
            for(l=0;l<COLS;l++)
                Table[k][l]=0;
            timer-=decrease--;
        }
    }
    score += 100*count;
}

struct timeval before_now, now;
int hasToUpdate(){
	return ((suseconds_t)(now.tv_sec*1000000 + now.tv_usec) -((suseconds_t)before_now.tv_sec*1000000 + before_now.tv_usec)) > timer;
}

void PrintTable(){
    
        // printf("Please WOrk 2");
    char Buffer[ROWS][COLS] = {0};
    int i, j;
    for(i = 0; i < current.width ;i++){
        for(j = 0; j < current.width ; j++){
            if(current.array[i][j])
                Buffer[current.row+i][current.col+j] = current.array[i][j];
        }
    }
    clear();
    for(i=0; i<COLS-9; i++)
        printw(" ");
    printw("Not Covid Tetris\n");
    for(i = 0; i < ROWS ;i++){
        for(j = 0; j < COLS ; j++){
            printw("%c ", (Table[i][j] + Buffer[i][j])? '#': '.');
        }
        printw("\n");
    }
    printw("\nScore: %d\n", score);
}

void ManipulateCurrent(int action){
    Shape temp = CopyShape(current);
    switch(action){
        case 's':
            temp.row++;  //move down
            if(CheckPosition(temp))
                current.row++;
            else {
                WriteToTable();
                RemoveFullRowsAndUpdateScore();
                SetNewRandomShape();
            }
            break;
        case 'd':
            temp.col++;  //move right
            if(CheckPosition(temp))
                current.col++;
            break;
        case 'a':
            temp.col--;  //move left
            if(CheckPosition(temp))
                current.col--;
            break;
        case 'w':
            RotateShape(temp); // rotate clockwise
            if(CheckPosition(temp))
                RotateShape(current);
            break;
        case 'q':
            endwin();
            GameOn = FALSE;
            break;
    }
    DeleteShape(temp);
    PrintTable();
}

typedef struct packet{
    char data[1024]; // A packet that will contain a string of data
}Packet;

typedef struct frame{
    int frame_kind; // The kind of frame: ACK:0, SEQ:1, FIN:2
    int sq_no; // Sequence number of the frame
    int ack; // Acknowledgment number of the frame
    Packet packet; // A packet that is transmitted in the frame
}Frame;

// Define packet loss ratio (e.g., 0.1 for 10% packet loss)
double packet_loss_ratio = 0.1;

int simulate_loss(void) {
    return ((double)rand() / (double)RAND_MAX) < packet_loss_ratio;
}

int main() {
    srand(time(0)); // Seed the random number generator

    score = 0;
    int c;

    initscr();
    cbreak();               // Line buffering disabled
    noecho();               // Don't echo input characters
    nodelay(stdscr, TRUE);

    gettimeofday(&before_now, NULL);
    timeout(1);
    SetNewRandomShape();    
    PrintTable();

    // Create a UDP socket
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    char buffer[PACKET_SIZE];
    int n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error on binding");
        exit(EXIT_FAILURE);
    }

    clilen = sizeof(cli_addr);

    while (GameOn) {
        // Check if there is any user input (keyboard)
        if ((c = getch()) != ERR) {
            ManipulateCurrent(c);
        }

        // Simulate packet loss before receiving control command from the client
        if (!simulate_loss()) {
            // Packet loss did not occur, receive control command from the client
            memset(buffer, 0, PACKET_SIZE);
            n = recvfrom(sockfd, buffer, 255, 0, (struct sockaddr *)&cli_addr, &clilen);
            if (n < 0) {
                perror("Error reading from socket");
                exit(EXIT_FAILURE);
            }
            
            // Process the received data (command)
            c = buffer[0]; // Use received control command
            ManipulateCurrent(c);

            memset(buffer, 0, PACKET_SIZE);
            buffer[0] = 'A'; // ACK character
            sendto(sockfd, buffer, 1, 0, (struct sockaddr *)&cli_addr, clilen);
        } else {
            // Packet loss occurred, do not receive control command from the client
            printf("[-]Packet Loss Simulated\n");
        }

        // Check if it's time to update based on some timer
        gettimeofday(&now, NULL);
        if (hasToUpdate()) { // time difference in microsec accuracy
            ManipulateCurrent('s');
            gettimeofday(&before_now, NULL);
        }
    }


    DeleteShape(current);
    endwin();
    int i, j;
    for(i = 0; i < ROWS ;i++){
        for(j = 0; j < COLS ; j++){
            printf("%c ", Table[i][j] ? '#': '.');
        }
        printf("\n");
    }
    printf("\nGame over!\n");
    printf("\nScore: %d\n", score);
    return 0;
}


