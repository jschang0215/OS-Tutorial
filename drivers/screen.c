#include "screen.h"
#include "../cpu/ports.h"
#include "../libc/mem.h"

// Private Function 선언
int get_cursor_offset();
void set_cursor_offset(int offset);
int print_char(char c, int col, int row, char attr);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

// Public Kernel API Functions

// 특정 위치에 문자열 출력
// col, row 음수이면 현재 offset에 출력
void kprint_at(char *message, int col, int row) {
    int offset;
    if(col>=0 && row>=0) {
        offset = get_offset(col, row);
    } else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    int i=0;
    while(message[i]!=0) {
        offset = print_char(message[i++], col, row, WHITE_ON_BLACK);
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

// kprint_at의 기본 Wrapper
void kprint(char *message) {
    kprint_at(message, -1, -1);
}

// Backspace 입력된 경우
void kprint_backspace() {
    int offset = get_cursor_offset()-2;
    int row = get_offset_row(offset);
    int col = get_offset_col(offset);
    // 0x08: Backspace
    print_char(0x08, col, row, WHITE_ON_BLACK);
}

// Private Kernel Functions

// VGA 직접 접근해 문자 출력
// col, row 음수이면 현재 커서 위치에 출력
// attr 0이면 white on black
// 다음 문자의 offset 리턴
// 커서를 리턴된 offset위치로 이동
int print_char(char c, int col, int row, char attr) {
    unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
    if(!attr) {
        attr = WHITE_ON_BLACK;
    }

    // 좌표가 범위 밖일 때 경고 문자 출력
    if(col>=MAX_COLS || row>=MAX_ROWS) {
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-2] = 'E';
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-1] = RED_ON_WHITE;
        return get_offset(col, row);
    }

    int offset;
    if(col>=0 && row>=0) {
        offset = get_offset(col, row);
    } else {
        offset = get_cursor_offset();
    }

    // 줄바꿈일때는 현재 row에 +1
    if(c=='\n') {
        row = get_offset_row(offset);
        offset = get_offset(0, row+1);
    } else if(c==0x08) {
        vidmem[offset] = ' ';
        vidmem[offset+1] = attr;
    } else {
        vidmem[offset] = c;
        vidmem[offset+1] = attr;
        offset += 2;
    }

    // offset이 화면 크기보다 크면 스크롤함
    if(offset>=MAX_ROWS*MAX_COLS*2) {
        int i;
        // 현재 라인을 이전 라인으로 하나씩 밀음 
        for(i=1; i<MAX_ROWS; i++) {
            memory_copy((uint8_t *)(get_offset(0, i)+VIDEO_ADDRESS), (uint8_t *)(get_offset(0, i-1)+VIDEO_ADDRESS), MAX_COLS*2);
        }
        // 마지막 줄을 빈칸으로
        char *last_line = (char *)(get_offset(0, MAX_ROWS-1) + VIDEO_ADDRESS);
        for(i=0; i<MAX_COLS*2; i++) {
            last_line[i] = 0;
        }
        // 범위 넘은 offset을 스크롤 다음 줄로 밀음
        offset -= 2*MAX_COLS;
    }

    set_cursor_offset(offset);
    return offset;
}

// VGA port이용해 현재 커서 위치 반환
int get_cursor_offset() {
    // (data 14)로 상위 바이트로 커서의 offset 알아냄
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    // offset은 커서위치의 2배 (문자+설정)
    return offset*2;
}

// 커서 위치 설정
void set_cursor_offset(int offset) {
    // 하나의 셀은 2개의 데이터로 이루어짐
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    // 레지스터에 입력
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

// 빈 화면으로
void clear_screen() {
    int screen_size = MAX_ROWS*MAX_COLS;
    int i;
    char *screen = (char *)VIDEO_ADDRESS;

    for(i=0; i<screen_size; i++) {
        screen[i*2] = ' ';
        screen[i*2+1] = WHITE_ON_BLACK;
    }
    set_cursor_offset(get_offset(0, 0));
}

int get_offset(int col, int row) {
    return 2*(row*MAX_COLS + col);
}

int get_offset_row(int offset) {
    return offset/(2*MAX_COLS);
}

int get_offset_col(int offset) {
    return (offset - (get_offset_row(offset)*2*MAX_COLS))/2;
}
