
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#include "font.h"

#define rgb565_BLACK       0x0000      /*   0,   0,   0 */
#define rgb565_NAVY        0x000F      /*   0,   0, 128 */
#define rgb565_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define rgb565_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define rgb565_MAROON      0x7800      /* 128,   0,   0 */
#define rgb565_PURPLE      0x780F      /* 128,   0, 128 */
#define rgb565_OLIVE       0x7BE0      /* 128, 128,   0 */
#define rgb565_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define rgb565_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define rgb565_BLUE        0x001F      /*   0,   0, 255 */
#define rgb565_GREEN       0x07E0      /*   0, 255,   0 */
#define rgb565_CYAN        0x07FF      /*   0, 255, 255 */
#define rgb565_RED         0xF800      /* 255,   0,   0 */
#define rgb565_MAGENTA     0xF81F      /* 255,   0, 255 */
#define rgb565_YELLOW      0xFFE0      /* 255, 255,   0 */
#define rgb565_WHITE       0xFFFF      /* 255, 255, 255 */
#define rgb565_ORANGE      0xFD20      /* 255, 165,   0 */
#define rgb565_GREENYELLOW 0xAFE5      /* 173, 255,  47 */

#define rgb565_GREY       0x37E7      /*   0,   0,   0 */

#define rgb565_GREY_33     0x52AA 
#define rgb565_GREY_66     0xAD55 

#define rgb565_PINK       0xFB56      /*   0,   0,   0 */
#define rgb565_BROWN       0x8A22      /* 139, 69, 19 */
#define rgb565_SKYBLUE     0x2F4F      /* 90, 210, 255 */

static unsigned int color_pallet[] =        // 모드 내 PRESET COLOR 12가지 색상 설정
{
    rgb565_RED,
    rgb565_GREEN,
    rgb565_BLUE,
    rgb565_YELLOW,
    rgb565_CYAN,
    rgb565_MAGENTA,
    rgb565_PURPLE, //rgb565_BLACK,
    rgb565_GREY_33,
    rgb565_BROWN, //rgb565_GREY_66,
    rgb565_SKYBLUE, //rgb565_WHITE,
    rgb565_PINK,
    rgb565_ORANGE, //rgb565_DARKCYAN,
};


void put_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) ;
void put_pixel(uint16_t x, uint16_t y, uint16_t color) ;


uint16_t mk_16bit_color( uint8_t R, uint8_t G, uint8_t B) ;

void draw_fill_box_4p(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) ;
void draw_fill_box_size(uint16_t x1, uint16_t y1, uint16_t xsize, uint16_t ysize, uint16_t color) ;
void draw_rectangle_size(uint16_t x1, uint16_t y1, uint16_t xsize, uint16_t ysize, uint16_t color) ; 

uint8_t put_char_lcd(uint16_t sx, uint16_t sy, char ch, uint16_t color, FONT_INFO font) ;
uint8_t put_char_lcd_bgc(uint16_t sx, uint16_t sy, char ch, uint16_t color, uint16_t BG_color, FONT_INFO font) ;
uint16_t put_str_lcd(uint16_t sx, uint16_t sy, char tstr[], uint16_t color, FONT_INFO font) ;
uint16_t put_str_lcd_bgc(uint16_t sx, uint16_t sy, char tstr[], uint16_t color, uint16_t BG_color, FONT_INFO font) ;

static void draw_color_pallet(unsigned int sx,unsigned int sy, uint16_t sel) ;      //Draw Color Pallet
static void R_colorbar(void) ;    // R 색상바 LCD 그리기
static void B_colorbar(void) ;    // B 색상바 LCD 그리기
static void G_colorbar(void) ;    // G 색상바 LCD 그리기

void RGB_COLOR_OUTLINE(void) ;    // RGB 모드 고정 항목 , LCD 출력

void led_dac(uint16_t v3K, uint16_t v5K, uint16_t vR, uint16_t vG, uint16_t vB) ;
void put_fill_box(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) ;

void sel_triangle(unsigned int x_pos,unsigned int y_pos) ;   //선택 삼각형 그리기
void rem_triangle(unsigned int x_pos,unsigned int y_pos) ;   //선택 삼각형 그리기
void bar_pointer(unsigned char rgb, unsigned int _x, unsigned int _y, uint16_t sel) ;  
void ratio_bar(uint8_t sx,uint16_t sy,uint8_t x_size,uint8_t y_size,unsigned char per) ; 
void bright_disp(uint8_t bri_val, uint16_t x_pos, uint16_t y_pos) ;
uint8_t key_in_PC() ;
void draw_circle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius, uint16_t color) ;      
void CCT_MODE_OUTLINE(void) ;     

uint8_t dac_R=255,dac_G=255,dac_B=0,dac_3K=0,dac_5K=0 ;


// g_cxxx : 컬러 관련 글로벌 변수들
uint8_t g_cRGB_R = 128 ; 
uint8_t g_cRGB_G = 128 ; 
uint8_t g_cRGB_B = 0 ; 
uint8_t g_cRGB_ratio = 50 ; 
//---------------------------------------------------------------------------
// 메뉴 관련 글로벌 변수
//---------------------------------------------------------------------------

uint16_t g_scr_pos ; 		// 모드에서 현재 위치
uint16_t old_g_scr_pos ;

uint8_t g_mode_list_sel ; 

// g_mxxxxx : g_m 프리픽스가 붙으면 모드 관련 변수들이다.
uint16_t g_mode_bright ;

uint16_t g_mode_pos ; 		// 모드에서 현재 위치

uint16_t g_mCCT_para_bright ;
uint16_t g_mCCT_cct = 27 ;

uint16_t g_mRGB_para_bright ;
uint16_t g_mRGB_para_R ;
uint16_t g_mRGB_para_G ;
uint16_t g_mRGB_para_B ;
uint16_t g_mRGB_para_cwratio ;
uint16_t g_mRGB_para_cct ;

uint16_t g_mHSI_para_bright ;
uint16_t g_mHSI_para_hue ;
uint16_t g_mHSI_para_sat ;

uint16_t g_mEFF_para_effpos ;
uint16_t g_mEFF_para_speed ;
uint16_t g_mEFF_para_bright ;

uint16_t g_mUEF_para_effpos ;
uint16_t g_mUEF_para_speed ;
uint16_t g_mUEF_para_bright ;

// g_kxxxxx : g_k 프리픽스가 붙으면 키 입력 관련 변수들이다
//            눌리면 1이 된다. 인지해서 사용후 0으로 만들어 주어야 한다.

uint16_t g_key_rotpush ;
uint16_t g_key_rotleft ;
uint16_t g_key_rotright ;
uint16_t g_key_enter ;
uint16_t g_key_mode ;
uint16_t g_key_lockleft ;
uint16_t g_key_lockright ;
uint16_t g_key_m1short ;
uint16_t g_key_m2short ;
uint16_t g_key_m3short ;
uint16_t g_key_m4short ;
uint16_t g_key_m5short ;
uint16_t g_key_m6short ;
uint16_t g_key_m1long ;
uint16_t g_key_m2long ;
uint16_t g_key_m3long ;
uint16_t g_key_m4long ;
uint16_t g_key_m5long ;
uint16_t g_key_m6long ;

uint16_t g_key_volup ;
uint16_t g_key_voldown ;


// mp_xxxx : mode 위치에 대한 상수 정의값들

#define mp_CCT 				101

#define mp_RGBsel_plt 		201
#define mp_RGBsel_R			202
#define mp_RGBsel_G			203
#define mp_RGBsel_B			204
#define mp_RGBsel_CWratio	205
#define mp_RGBsel_CCT		206
#define mp_RGBsel_Chistory	207


void MODE_LIST(unsigned char idx) ;       //46

char *strModeList[] = {
	"CCT MODE",
	"RGB MODE",
	"HUE/SAT MODE",
	"EFFECT MODE",
    "USER EFFECT",
	"SYSTEM SET",
};

uint8_t g_key_in = 0 ;

uint8_t key_in_PC() ;

void MODE_LIST_proc(void) ; 
void CCT_proc() ;    
void RGB_proc() ;    

#define scr_MODE_LIST 1 
#define scr_CCT 2
#define scr_RGB 3

#define key_VOLUP	 	1  
#define key_VOLDOWN  	2
#define key_ROTLEFT  	3
#define key_ROTRIGHT  	4
#define key_ENTER  	 	5
#define key_MODE  	 	6
#define key_ROTPUSH  	7
#define key_M1SHORT 	8
#define key_M2SHORT 	9
#define key_M3SHORT		10
#define key_M4SHORT 	11
#define key_M5SHORT 	12
#define key_M6SHORT 	13
#define key_M1LONG  	14
#define key_M2LONG  	15
#define key_M3LONG  	16
#define key_M4LONG  	17
#define key_M5LONG  	18
#define key_M6LONG  	19

uint16_t g_mRGB_p0sel = 0 ;
uint16_t g_mRGB_pos = 0 ;
void draw_RGB_p0sel(void) ;
void draw_RGB_pos(void) ; 

uint16_t g_mRGB_pltnum = 0 ;

int main( void)
{
	FILE *fp, *fa ;
	int i,j, proc_num, R,G,B ;
	uint16_t t_color ;
	uint16_t b_color ;
	uint16_t color, bri_val ;
	int c, c1, c2 ;

	// fifo.txt는 파이썬과의 버퍼 역할을 하는 파일이다.
	// 현 폴더에 있으면 지우고 처음부터 시작한다.
	if( (fp=fopen("fifo.txt","r")) != NULL )
	{
		fclose(fp) ;
		system("rm fifo.txt") ;
	}

	// 랜덤 넘버로 태스크 번호를 쓰고 시작한다.
	if( (fa = fopen("fifo.txt","w")) != NULL ) 
	{
   		srand((unsigned int)time(NULL));
		fprintf(fa,"%d\n",rand()) ;
		fclose(fa) ;
	}

	// 3K, 5K, R, G, B. 16bit 0 ~ 65535 
	led_dac(255*256 , 255*256 ,  255*256,  255*256,  255*256) ;

	draw_fill_box_4p(0,0,240,320,0x0000) ;

	//MODE_LIST(0) ;       //46
	CCT_MODE_OUTLINE() ;    // RGB 모드 고정 항목 , LCD 출력
	//RGB_COLOR_OUTLINE() ;    // RGB 모드 고정 항목 , LCD 출력


	// 화살표 왼쪽,오른쪽 : 로터리 인코더 좌우
	// 화살표 위,아래 : 밝기 볼륨
	// end : Enter/Exit, home : MODE
	// 숫자키 1,2,3,4,5,6 LCD 옆의 키 6개
	// F1 ~ F6 : LCD 옆의 키 6개 길게 누름 
	// delete : 로터리 lock, page down : 밝기 lock
	// enter : 로터리 Push

	old_g_scr_pos = 0xffff ;
	g_scr_pos = 0 ;

	while(1)
	{
		g_key_in = key_in_PC() ;

		if( g_key_in == key_MODE ) {
			g_key_in = 0 ; // 처리하면 반드시 0으로 해 줄것.
			g_scr_pos = scr_MODE_LIST ; 
		}

		if( g_scr_pos == scr_MODE_LIST ) 
		{
			MODE_LIST_proc() ;

			if(g_scr_pos != old_g_scr_pos) 
			{
				if( g_scr_pos == scr_MODE_LIST )
				{
					//g_mode_level = 0 ;
					g_mode_list_sel = 0 ;
					MODE_LIST(g_mode_list_sel) ;       //46
				}
				else if( g_scr_pos == scr_CCT )
				{
					CCT_MODE_OUTLINE() ;    
				}
				else if( g_scr_pos == scr_RGB )
				{
					RGB_COLOR_OUTLINE() ;   
				}
				old_g_scr_pos = g_scr_pos ;
			}
		}
		else if( g_scr_pos == scr_CCT ) {
			CCT_proc() ;    
		}
		else if( g_scr_pos == scr_RGB ) {
			RGB_proc() ;    
		}
	}

	return 0 ;
} 

void MODE_LIST_proc(void ) 
{
	if( g_key_in == 0 ) return ;

	if( g_key_in == key_ROTLEFT ) 
	{
		if( g_mode_list_sel <= 0 ) g_mode_list_sel = 5 ;
		else g_mode_list_sel -= 1 ;
		MODE_LIST(g_mode_list_sel) ;       //46
	}
	else if( g_key_in == key_ROTRIGHT ) 
	{
		if( g_mode_list_sel >= 5 ) g_mode_list_sel = 0 ;
		else g_mode_list_sel += 1 ;
		MODE_LIST(g_mode_list_sel) ;       //46
	}
	else if( g_key_in == key_ROTPUSH ) 
	{
		if( g_mode_list_sel == 0 ) g_scr_pos = scr_CCT ;
		else if( g_mode_list_sel == 1 ) g_scr_pos = scr_RGB ;
		//else if( g_mode_list_sel == 2 ) g_scr_pos = scr_HSI ;
	}
	g_key_in = 0 ;

	printf("g_scr_pos, old_g_scr_pos : %d,%d\n", g_scr_pos , old_g_scr_pos) ;
}

void CCT_proc()     
{
    char tstr[30] ;

	if( g_key_in == 0 ) return ;

	if( g_key_in == key_ROTLEFT ) 
	{
		if( g_mCCT_cct > 27 ) g_mCCT_cct-- ;
	}
	else if( g_key_in == key_ROTRIGHT ) 
	{
		if( g_mCCT_cct < 65 ) g_mCCT_cct++ ;
	}

    sprintf(tstr,"%4d",g_mCCT_cct*100);
	put_str_lcd_bgc(65, 160, tstr, rgb565_BLACK, rgb565_GREEN, nngt_38_Info) ;

	g_key_in = 0 ;
}

void RGB_proc()     
{
    char tstr[30] ;

printf("key : %d, pos : %d\n",g_key_in, g_mRGB_pos) ;

	if( g_key_in == 0 ) return ;

	// RGB 모드 톱레벨
	if( g_mRGB_pos == 0 )	
	{
		if( g_key_in == key_ROTLEFT ) 
		{
			if( g_mRGB_p0sel <= 0 ) g_mRGB_p0sel = 6 ;
			else g_mRGB_p0sel-- ;

			draw_RGB_p0sel() ;
		}
		else if( g_key_in == key_ROTRIGHT ) 
		{
			if( g_mRGB_p0sel >= 6 ) g_mRGB_p0sel = 0 ;
			else g_mRGB_p0sel++ ;

			draw_RGB_p0sel() ;
		}
		else if( g_key_in == key_ROTPUSH ) 
		{
			g_mRGB_pos = g_mRGB_p0sel + 1 ;
			draw_RGB_pos() ;
		}
	}
	// RGB mode 컬러 팔레트 키 처리
	else if( g_mRGB_pos == 1 )	
	{
		if( g_key_in == key_ROTLEFT ) 
		{
			if( g_mRGB_pltnum <= 0 ) g_mRGB_pltnum = 11 ;
			else g_mRGB_pltnum-- ;

    		draw_color_pallet(20, 100 , 0xFF) ;
    		draw_color_pallet(20, 100 , g_mRGB_pltnum) ;
		}
		else if( g_key_in == key_ROTRIGHT ) 
		{
			if( g_mRGB_pltnum >= 11 ) g_mRGB_pltnum = 0 ;
			else g_mRGB_pltnum++ ;

    		draw_color_pallet(20, 100 , 0xFF) ;
    		draw_color_pallet(20, 100 , g_mRGB_pltnum) ;
		}
		else if( g_key_in == key_ROTPUSH ) 
		{
			g_cRGB_R = ((color_pallet[ g_mRGB_pltnum ]>>11)<<3) + 7 ; 
			g_cRGB_G = (((color_pallet[ g_mRGB_pltnum ]>>5)&0x3f)<<2) + 3 ; 
			g_cRGB_B = ((color_pallet[ g_mRGB_pltnum ]&0x1f)<<3) + 7 ; 
			draw_fill_box_size(15, 45, 70, 40, color_pallet[ g_mRGB_pltnum ]) ;

			g_mRGB_pos = 0 ;
    		draw_color_pallet(20, 100 , 0xFF) ;
			g_mRGB_p0sel = 0 ;
			draw_RGB_p0sel() ;
		}
	}
	// RGB mode R 값 키 처리
	else if( g_mRGB_pos == 2 )	
	{
		if( g_key_in == key_ROTLEFT ) 
		{
			if( g_cRGB_R > 0 ) g_cRGB_R-- ;
			bar_pointer(0, 40 + g_cRGB_R/2, 170, 0) ;  // R 바포인터
		}
		else if( g_key_in == key_ROTRIGHT ) 
		{
			if( g_cRGB_R < 255 ) g_cRGB_R++ ;
			bar_pointer(0, 40 + g_cRGB_R/2, 170, 0) ;  // R 바포인터
		}
		else if( g_key_in == key_ROTPUSH ) 
		{
			g_mRGB_pos = 0 ;
			bar_pointer(0, 40 + g_cRGB_R/2, 170, 0xff) ;  // R 바포인터
			g_mRGB_p0sel = 1 ;
			draw_RGB_p0sel() ;
		}
	}
	// RGB mode G 값 키 처리
	else if( g_mRGB_pos == 3 )	
	{
		if( g_key_in == key_ROTLEFT ) 
		{
			if( g_cRGB_G > 0 ) g_cRGB_G-- ;
			bar_pointer(1, 40 + g_cRGB_G/2, 192, 0) ;  // R 바포인터
		}
		else if( g_key_in == key_ROTRIGHT ) 
		{
			if( g_cRGB_G < 255 ) g_cRGB_G++ ;
			bar_pointer(1, 40 + g_cRGB_G/2, 192, 0) ;  // R 바포인터
		}
		else if( g_key_in == key_ROTPUSH ) 
		{
			g_mRGB_pos = 0 ;
			bar_pointer(1, 40 + g_cRGB_G/2, 192, 0xff) ;  // R 바포인터
			g_mRGB_p0sel = 2 ;
			draw_RGB_p0sel() ;
		}
	}
	// RGB mode B 값 키 처리
	else if( g_mRGB_pos == 4 )	
	{
		if( g_key_in == key_ROTLEFT ) 
		{
			if( g_cRGB_B > 0 ) g_cRGB_B-- ;
			bar_pointer(2, 40 + g_cRGB_B/2, 214, 0) ;  // R 바포인터
		}
		else if( g_key_in == key_ROTRIGHT ) 
		{
			if( g_cRGB_B < 255 ) g_cRGB_B++ ;
			bar_pointer(2, 40 + g_cRGB_B/2, 214, 0) ;  // R 바포인터
		}
		else if( g_key_in == key_ROTPUSH ) 
		{
			g_mRGB_pos = 0 ;
			bar_pointer(2, 40 + g_cRGB_B/2, 214, 0xff) ;  // R 바포인터
			g_mRGB_p0sel = 3 ;
			draw_RGB_p0sel() ;
		}
	}
	// RGB mode Ratio 키 처리
	else if( g_mRGB_pos == 5 )	
	{
		if( g_key_in == key_ROTLEFT ) 
		{
			if( g_cRGB_ratio > 0 ) g_cRGB_ratio-- ;
			ratio_bar(55,240,138,20,g_cRGB_ratio) ;     
		}
		else if( g_key_in == key_ROTRIGHT ) 
		{
			if( g_cRGB_ratio < 100 ) g_cRGB_ratio++ ;
			ratio_bar(55,240,138,20,g_cRGB_ratio) ;     
		}
		else if( g_key_in == key_ROTPUSH ) 
		{
			g_mRGB_pos = 0 ;
			g_mRGB_p0sel = 4 ;
			draw_RGB_p0sel() ;
		}
	}
	// RGB mode CCT 키 처리
	else if( g_mRGB_pos == 6 )	
	{
	}
	// RGB mode Color History 키 처리
	else if( g_mRGB_pos == 7 )	
	{
	}
}

void draw_RGB_pos(void) 
{
	// RGB mode 컬러 팔레트 키 처리
	if( g_mRGB_pos == 1 )	
	{
    	draw_color_pallet(20, 100 , g_mRGB_pltnum) ;
	}
	// RGB mode R 값 키 처리
	else if( g_mRGB_pos == 2 )	
	{
		bar_pointer(0, 40 + g_cRGB_R/2, 170, 0) ;  // R 바포인터
	}
	// RGB mode G 값 키 처리
	else if( g_mRGB_pos == 3 )	
	{
		bar_pointer(1, 40 + g_cRGB_G/2, 192, 0) ;  // R 바포인터
	}
	// RGB mode B 값 키 처리
	else if( g_mRGB_pos == 4 )	
	{
		bar_pointer(2, 40 + g_cRGB_B/2, 214, 0) ;  // R 바포인터
	}
	// RGB mode Ratio 키 처리
	else if( g_mRGB_pos == 5 )	
	{
		ratio_bar(55,240,138,20,g_cRGB_ratio) ;     
	}
	// RGB mode CCT 키 처리
	else if( g_mRGB_pos == 6 )	
	{
	}
	// RGB mode Color History 키 처리
	else if( g_mRGB_pos == 7 )	
	{
	}
}

// RGB mode 톱레벨의 선택 화면을 그린다 
void draw_RGB_p0sel(void)
{
	rem_triangle(5,120) ; // RGB모드 팔레트 위치
	rem_triangle(5,171) ; // RGB모드 R 위치
	rem_triangle(5,193) ; // RGB모드 G 위치
	rem_triangle(5,215) ; // RGB모드 B 위치
	rem_triangle(5,239) ; // RGB모드 RGB 위치
	rem_triangle(5,260) ; // RGB모드 CCT 위치
	rem_triangle(5,277) ; // RGB모드 Color History 위치

	if( g_mRGB_p0sel == 0 ) sel_triangle(5,120) ; // RGB모드 팔레트 위치
	else if( g_mRGB_p0sel == 1 ) sel_triangle(5,171) ; // RGB모드 R 위치
	else if( g_mRGB_p0sel == 2 ) sel_triangle(5,193) ; // RGB모드 G 위치
	else if( g_mRGB_p0sel == 3 ) sel_triangle(5,215) ; // RGB모드 B 위치
	else if( g_mRGB_p0sel == 4 ) sel_triangle(5,239) ; // RGB모드 RGB 위치
	else if( g_mRGB_p0sel == 5 ) sel_triangle(5,260) ; // RGB모드 CCT 위치
	else // if( g_mRGB_p0sel == 6 ) 
		sel_triangle(5,277) ; // RGB모드 Color History 위치
}


// 볼륨 포함 전면의 키값을 읽어들인다.
uint8_t key_in_PC()
{
	uint8_t c , c2 ;
	uint8_t key_press ;

	c2 = 0 ;

	c = getch() ;

	if((c == 224 ) || (c == 0)) c2 = getch() ;

	printf("%d ",c) ;
	if( c2 != 0 )  printf(", %d\n",c2) ;
	else printf("\n") ;

	if( c == 92 ) exit(0) ; // 백슬래시면 프로그램 끝낸다. PC only

	key_press = 0 ;

	if( (c == 224) && (c2 == 72) ) 		key_press = key_VOLUP ; // 위 화살표. 밝기 up
	else if( (c == 224) && (c2 == 80) ) key_press = key_VOLDOWN ; // 아래 화살표. 밝기 down
	else if( (c == 224) && (c2 == 75) ) key_press = key_ROTLEFT ; // 왼쪽 옆 화살표. 메뉴 위로
	else if( (c == 224) && (c2 == 77) ) key_press = key_ROTRIGHT ; // 오른쪽 옆 화살표. 메뉴 아래로
	else if( (c == 224) && (c2 == 79) ) key_press = key_ENTER ; // end . Enter/Exit 키
	else if( (c == 224) && (c2 == 71) ) key_press = key_MODE ; // home. MODE 키
	else if( c == 13) 					key_press = key_ROTPUSH ; // Enter. 로터리 스위치 누름 키
	else if( c == 49) 					key_press = key_M1SHORT ;	// 숫자 1.  1번키 짧게
	else if( c == 50) 					key_press = key_M2SHORT ;	// 숫자 2.  2번키 짧게
	else if( c == 51) 					key_press = key_M3SHORT ;	// 숫자 3.  3번키 짧게
	else if( c == 52) 					key_press = key_M4SHORT ;	// 숫자 4.  4번키 짧게
	else if( c == 53) 					key_press = key_M5SHORT ;	// 숫자 5.  5번키 짧게
	else if( c == 54) 					key_press = key_M6SHORT ;	// 숫자 6.  6번키 짧게
	else if( (c == 0) && (c2 == 59) ) 	key_press = key_M1LONG ; // F1 1번키 길게 
	else if( (c == 0) && (c2 == 60) ) 	key_press = key_M2LONG ; // F2 2번키 길게 
	else if( (c == 0) && (c2 == 61) ) 	key_press = key_M3LONG ; // F3 3번키 길게 
	else if( (c == 0) && (c2 == 62) ) 	key_press = key_M4LONG ; // F4 4번키 길게 
	else if( (c == 0) && (c2 == 63) ) 	key_press = key_M5LONG ; // F5 5번키 길게 
	else if( (c == 0) && (c2 == 64) ) 	key_press = key_M6LONG ; // F6 6번키 길게 

	return( key_press ) ;
}

#define rgb565_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define DATA_SET_Line    20      

void MODE_LIST(unsigned char idx)       //46
{
    unsigned int y_pos = DATA_SET_Line,_max_ = 6,offset = 47; //, num = 0;
	int i ;

    y_pos = 28;
    //printf(" Mode List num = %d \r\n", idx );

    switch(idx)
    {
        case 0xFF:            

    		draw_fill_box_4p(0,17,240,300,rgb565_LIGHTGREY);
            for(i = 0;i<_max_;i++)
            {
    			put_str_lcd(20, y_pos, strModeList[i], rgb565_WHITE, nngt_28_Info);
                y_pos += offset;
            }
            break;

        default:

    		draw_fill_box_4p(0,17,240,300,rgb565_LIGHTGREY);
            for(i = 0;i < _max_;i++) 
            {
    			put_str_lcd(20, y_pos, strModeList[i], rgb565_WHITE, nngt_28_Info);
                y_pos += offset;
            }
            draw_fill_box_4p(0,17+(offset*idx),240,17+(offset*idx)+offset,rgb565_DARKGREY);
            put_str_lcd(20,28+(offset*idx),strModeList[idx],rgb565_BLACK, nngt_28_Info);  
        
        break;
    }  
    y_pos = 18;
    for(int i = 0;i < (_max_);i++) 
    {
        y_pos += offset;
        draw_fill_box_4p(0, y_pos,240,y_pos,rgb565_BLACK);
    }

}


void CCT_MODE_OUTLINE(void)     // CCT 모드 고정 항목,  LCD 출력
{
    char tstr[30] ;

    draw_fill_box_4p(0,17,240,300,rgb565_DARKGREY);
    put_str_lcd(0, DATA_SET_Line, "[CCT MODE]", rgb565_WHITE, nngt_21_Info);
    draw_circle(205,160,4,rgb565_BLACK);
    put_str_lcd(210, 160, "K", rgb565_WHITE, nngt_38_Info);

    draw_fill_box_4p(65,160,200,160+48,rgb565_DARKGREY);
    sprintf(tstr,"%4d",29*100);
	put_str_lcd_bgc(65, 160, tstr, rgb565_BLACK, rgb565_GREEN, nngt_38_Info) ;
    
	bright_disp(100, 80, 80) ;
}


void RGB_COLOR_OUTLINE(void)    // RGB 모드 고정 항목 , LCD 출력
{        
    char tstr[30] ;
	uint16_t rgbmode_R ;
	uint16_t rgbmode_G ;
	uint16_t rgbmode_B ;
	uint16_t rgbmode_CCT ;
	uint16_t rgbmode_Ratio ;
	uint16_t tcolor, bri_val ;
    
	//effect_mode = 0;
    //user_effect_mode = 0;
    //sys_led.mode = RGB_COLOR;
    //memcpy(&old_system_set ,&system_set, sizeof(SYS_SET) );
    
    // RGB 모드에 필요한 화면 전체를 지운다.
    draw_fill_box_4p(0,17,240,300,rgb565_DARKGREY);

    put_str_lcd(0, 19, "[RGB MODE]", rgb565_WHITE, nngt_16_Info);
    draw_rectangle_size(10,40,220,50,rgb565_BLACK);
    
	//  Color Pallet         
    //draw_fill_box_4p(16,95,230,165,rgb565_LIGHTGREY);
    draw_color_pallet(20, 100 ,0xff);   

	// color bar
    //draw_rectangle_size(39,174,129,15,rgb565_DARKGREY);
    R_colorbar();
    
    //draw_rectangle_size(39,196,129,15,rgb565_DARKGREY);
    G_colorbar();
    
    //draw_rectangle_size(39,218,129,15,rgb565_DARKGREY);
    B_colorbar();

	bar_pointer(0, 40 + g_cRGB_R/2, 170, 0xff) ;  // R 바포인터
	bar_pointer(1, 40 + g_cRGB_G/2, 192, 0xff) ;  // G 바 포인터
	bar_pointer(2, 40 + g_cRGB_B/2, 214, 0xff) ;  // B 바 포인터
    
	// RGB vs White Ratio Bar
	ratio_bar(55,240,138,20,50) ;     

    put_str_lcd(15,172," R ",rgb565_RED, nngt_16_Info);
    put_str_lcd(15,194," G ",rgb565_GREEN, nngt_16_Info);
    put_str_lcd(15,216," B ",rgb565_BLUE, nngt_16_Info);
    
    put_str_lcd(220, 172,"%",rgb565_WHITE, nngt_16_Info);
    put_str_lcd(220, 194,"%",rgb565_WHITE, nngt_16_Info);
    put_str_lcd(220, 216,"%",rgb565_WHITE, nngt_16_Info);
    
    put_str_lcd(120, 263,"CCT",rgb565_WHITE, nngt_16_Info);
    put_str_lcd(220, 263,"K",rgb565_WHITE, nngt_16_Info);
    
    put_str_lcd(15, 241,"RGB",rgb565_WHITE, nngt_16_Info);
    put_str_lcd(198, 241,"W",rgb565_WHITE, nngt_16_Info);
    
    put_str_lcd(15, 280,">>Color History",rgb565_WHITE, nngt_16_Info);

	sel_triangle(5,120) ; // RGB모드 팔레트 위치


	rgbmode_R = 100 ;
	rgbmode_G = 50 ;
	rgbmode_B = 0 ;
	rgbmode_CCT = 3500 ;
	rgbmode_Ratio = 77 ;
	sprintf(tstr,"%d",rgbmode_R) ;
    put_str_lcd(174, 172,tstr ,rgb565_WHITE, nngt_16_Info);
	sprintf(tstr,"%d",rgbmode_G) ;
    put_str_lcd(174, 194,tstr ,rgb565_WHITE, nngt_16_Info);
	sprintf(tstr,"%d",rgbmode_B) ;
    put_str_lcd(174, 216,tstr ,rgb565_WHITE, nngt_16_Info);

	sprintf(tstr,"%d",rgbmode_CCT) ;
    put_str_lcd(170, 263,tstr ,rgb565_WHITE, nngt_16_Info);

	sprintf(tstr,"%d",rgbmode_Ratio) ;
    put_str_lcd(57, 241,tstr ,rgb565_WHITE, nngt_16_Info);
	tstr[0] = '%' ; tstr[1] = 0 ;
    put_str_lcd(90, 241,tstr ,rgb565_WHITE, nngt_16_Info);

	sprintf(tstr,"%d",100-rgbmode_Ratio) ;
    put_str_lcd(150, 241,tstr ,rgb565_WHITE, nngt_16_Info);
	tstr[0] = '%' ; tstr[1] = 0 ;
    put_str_lcd(182, 241,tstr ,rgb565_WHITE, nngt_16_Info);

	// 현재 RGB 모드에서 선택된 컬러를 좌상단에 표시한다. 
	tcolor = mk_16bit_color( rgbmode_R, rgbmode_G, rgbmode_B) ;
	draw_fill_box_size(15, 45, 70, 40, tcolor) ;

	bri_val = 100 ;
	bright_disp(bri_val, 120, 42) ;
}

void bright_disp(uint8_t bri_val, uint16_t x_pos, uint16_t y_pos)
{
	char tstr[10] ;

	sprintf(tstr,"%3d",bri_val) ;
	put_str_lcd(x_pos, y_pos, tstr, rgb565_WHITE,nngt_38_Info); 
	tstr[0] = '%' ; tstr[1] = 0 ;
	put_str_lcd(x_pos + 70, y_pos, tstr, rgb565_WHITE,nngt_38_Info); 
}

//percentage input
void ratio_bar(uint8_t sx,uint16_t sy,uint8_t x_size,uint8_t y_size,unsigned char per)     
{
    unsigned int pos = 0;
    unsigned int color = rgb565_BLACK; //Get_Background_color()

    pos = (x_size-6)*per/100;

    //erase bar
    draw_fill_box_4p(sx+1,sy+1,sx+x_size-1,sy+y_size-1, rgb565_DARKGREY); 

    // draw outline
    put_line(sx,sy,sx+x_size,sy,rgb565_BLACK); 
    put_line(sx,sy+y_size,sx+x_size,sy+y_size, rgb565_BLACK); 
    put_line(sx,sy,sx,sy+y_size, rgb565_BLACK); 
    put_line(sx+x_size,sy,sx+x_size,sy+y_size, rgb565_BLACK); 
    
    //draw bar
    for(int j = (sy+1);j<(sy+y_size-1);j++)    
    {
        put_line(sx+1+pos,j,sx+6+pos,j, rgb565_ORANGE); 
    }
}


//선택 삼각형 그리기
void sel_triangle(unsigned int x_pos,unsigned int y_pos)   
{   
    unsigned char offset ;
	int k ;

    for(int k=0;k<5 ;k++)
    {
        offset = 20-(4*k);
        put_line(x_pos+k,y_pos,x_pos+k,y_pos+offset, rgb565_WHITE); 
        y_pos += 2;
    }
}

//선택 삼각형 지우기
void rem_triangle(unsigned int x_pos,unsigned int y_pos)   
{   
    unsigned char offset = 0;
    unsigned char max = 5;
	int k ;

    for(int k=0;k<max;k++)
    {
        offset = 20-(4*k);
        put_line(x_pos+k,y_pos,x_pos+k,y_pos+offset, rgb565_DARKGREY); 
        y_pos += 2;
    }
}


void bar_pointer(unsigned char rgb, unsigned int _x, unsigned int _y, uint16_t sel)  // 컬러 위치 표시 
{
    switch(rgb)
    {
        case 0: R_colorbar();   break ;
        case 1: G_colorbar();   break ;
        case 2: B_colorbar();   break ;
    }
	draw_fill_box_4p(40, _y, 167, _y+4, rgb565_DARKGREY) ; 
	draw_fill_box_4p(40, _y+19, 167, _y+23, rgb565_DARKGREY) ; 
    
    if(_x < 41)     _x = 41;
    if(_x > 166)    _x = 166;
    
	draw_fill_box_4p(_x-1, _y+5, _x+1, _y+18, rgb565_WHITE) ; 

	if( sel == 0xff ) return ;

	draw_rectangle_size(_x-4, _y+2, 8, 19, rgb565_BLACK) ; 
}

static void R_colorbar(void)    // R 색상바 LCD 그리기
{
	uint16_t y_pos, x_pos, j,k, color ;

    y_pos = 175;
    
    for(int j = 0 ; j < 14;j++)
    {
        color = rgb565_RED;
        x_pos = 164; //36;
        for(int k = 0;k<32 ; k++)
        {
			draw_fill_box_4p(x_pos,y_pos,x_pos+4,y_pos,color); 
            color -= 0x0800;
            x_pos -= 4;
        }
        y_pos++;
    }
}

static void G_colorbar(void)    // G 색상바 LCD 그리기
{
	uint16_t y_pos, x_pos, j,k, color ;

    y_pos = 197; //192;    
    for(int j = 0 ; j < 14;j++)
    {
        color = rgb565_GREEN;
        x_pos = 164; //54;
        for(int k = 0;k<32 ; k++)
        {
			draw_fill_box_4p(x_pos,y_pos,x_pos+4,y_pos,color); 
            color -= 0x20;
            x_pos -= 4;          
        }
        y_pos++;
    }   
}

static void B_colorbar(void)    // B 색상바 LCD 그리기
{
	uint16_t y_pos, x_pos, j,k, color ;

    y_pos = 219; //212;
    for(int j = 0 ; j < 14;j++)
    {
        color = rgb565_BLUE;
        x_pos = 164; //54;
        for(int k = 0;k<32 ; k++)
        {
			draw_fill_box_4p(x_pos,y_pos,x_pos+4,y_pos,color); 
            color -= 0x0001;
            x_pos -= 4;
        }
        y_pos++;
    }
}


static void draw_color_pallet(unsigned int sx,unsigned int sy, uint16_t sel)       
{
    unsigned int x0, y0 ;
    x0 = sx;
	y0 = sy ;

    draw_fill_box_4p(16,95,230,165,rgb565_LIGHTGREY);
    
    for(int i = 0; i< 6; i++)   
    {
        draw_fill_box_size(x0, y0, 26, 26, color_pallet[i]);   
        x0 += 35;
    }

    x0 = sx;
    y0 += 34;

    for(int i = 6; i< 12; i++)   
    {
        draw_fill_box_size(x0, y0, 26, 26, color_pallet[i]);
        x0 += 35;
    }

	if( sel == 0xff ) return ; // sel이 0xff 이면 선택 박스를 그리지 않는다.

	// 선택된 컬러 주변에 검은 박스를 그린다.
	if( sel < 6 ) 
	{
		x0 = (sx - 2) + sel*35 ;
		y0 = sy - 2 ;
	}
	else 
	{
		x0 = (sx - 2) + (sel-6)*35 ;
		y0 = sy + 32 ;
	}

    draw_rectangle_size(x0, y0, 26+4, 26+4, rgb565_BLACK );
}

//----------------------------------------------------------------------
// 그리기 기본 함수들.
// LCD에 적용에도 바뀌지 않아야 한다. 
//----------------------------------------------------------------------

// 반환 값은 끝난 지점의 수평 위치
uint16_t put_str_lcd(uint16_t sx, uint16_t sy, char tstr[], uint16_t color, FONT_INFO font)
{
	char tmp_str[256] ;
	uint16_t sum_w, i ;

	sum_w = 0 ;
	for( i=0 ; i<strlen(tstr) ; i++ )
	{
		sum_w += put_char_lcd(sx + sum_w, sy, tstr[i], color, font) ;
		//printf("%d , %c\n",tstr[i],tstr[i]) ;
		if( i > 256 ) break ; // 256 글자까지만 쓴다
	}

	return( sx + sum_w ) ;
}

// 반환 값은 끝난 지점의 수평 위치
uint16_t put_str_lcd_bgc(uint16_t sx, uint16_t sy, char tstr[], uint16_t color, uint16_t BG_color, FONT_INFO font)
{
	char tmp_str[256] ;
	uint16_t sum_w, i ;
	char c ;

	sum_w = 0 ;
	for( i=0 ; i<strlen(tstr) ; i++ )
	{
		sum_w += put_char_lcd_bgc(sx + sum_w, sy, tstr[i], color, BG_color, font) ;
		//printf("x,y : %d,%d  , char :  %d , %c\n",sx+sum_w,sy,tstr[i],tstr[i]) ;
		//c = getchar() ;
		if( i > 256 ) break ; // 256 글자까지만 쓴다
	}

	return( sx + sum_w ) ;
}


// 반환 값은 width

uint8_t put_char_lcd(uint16_t sx, uint16_t sy, char ch, uint16_t color, FONT_INFO font)
{
	uint16_t i,j,k ;
	uint16_t id ;
	uint16_t f_id, f_height, f_width, f_offset, byte_id, bit_id, t_id, t_data, t_bit ;
	uint16_t xpos, ypos, n_sx, n_sy ;

	uint16_t xstart, xend , start_bit, start_pos ;


	if( ch < ' ' ) return(0) ;
	if( ch > '~' ) return(0) ;

	// 폰트에 space가 없어, space는 j 간격으로 하고 데이터는 0으로 한다.
	if( ch == ' ' ) 
		f_id = 'j' - font.startChar ; // 보통 '!'
	else
		f_id = ch - font.startChar ; // 보통 '!'

	f_height = font.heightPages ;
	f_width = font.charInfo[f_id].widthBits ;
	f_offset = font.charInfo[f_id].offset ;

	if( (sx + f_width) > 239 ) n_sx = 0 ;
	else n_sx = sx ;

	if( (sy + f_height) >= 319 ) n_sy = 0 ;
	else n_sy = sy ;

//printf("n_sx,n_sy : %d,%d\n",n_sx,n_sy) ;

	for( i=0 ; i<f_height ; i++ )
	{
		byte_id = (f_width - 1)/8 ; // f_width가 8이면 0 

		for( j=0 ; j<byte_id ; j++ )
		{
			t_id = f_offset + i * (byte_id + 1) + j ;

			if( ch == ' ' ) t_data = 0 ;
			else t_data = font.data[ t_id ] ;

			ypos = n_sy + i ;

			for( k=0 ; k<8 ; k++ )
			{
				t_bit = t_data & (0x80>>k) ;

				xpos = n_sx + j*8 + k ;
				ypos = n_sy + i ;

				if( t_bit != 0 ) 
				{
					put_pixel(xpos, ypos, color) ;
				}
			}
		}

		t_id = f_offset + i * (byte_id + 1) + byte_id ;

		if( ch == ' ' ) t_data = 0 ;
		else t_data = font.data[ t_id ] ;

		bit_id = (f_width - 1)%8 ;


		for( k=0 ; k< (bit_id+1) ; k++ )
		{
			t_bit = t_data & (0x80>>k) ;

			xpos = n_sx + byte_id*8 + k ;
			ypos = n_sy + i ;

			if( t_bit != 0 ) 
			{
				put_pixel(xpos, ypos, color) ;
			}
		}
		/*
		*/

		// 우측에 빈 줄 1줄 그림
		xpos = n_sx + byte_id*8 + bit_id + 1 ;
		if( xpos <= 239 ) 
		{
			ypos = n_sy + i ;
		}

		// 우측에 빈 줄 1줄 그림
		xpos = n_sx + byte_id*8 + bit_id + 2 ;
		if( xpos <= 239 ) 
		{
			ypos = n_sy + i ;
		}
	}

	return(f_width + 2) ;
}

// 반환 값은 width
uint8_t put_char_lcd_bgc(uint16_t sx, uint16_t sy, char ch, uint16_t color, uint16_t BG_color, FONT_INFO font)
{
	uint16_t i,j,k ;
	uint16_t id ;
	uint16_t f_id, f_height, f_width, f_offset, byte_id, bit_id, t_id, t_data, t_bit ;
	uint16_t xpos, ypos, n_sx, n_sy ;


	if( ch < ' ' ) return(0) ;
	if( ch > '~' ) return(0) ;

	// 폰트에 space가 없어, space는 j 간격으로 하고 데이터는 0으로 한다.
	if( ch == ' ' ) 
		f_id = 'j' - font.startChar ; // 보통 '!'
	else
		f_id = ch - font.startChar ; // 보통 '!'

	f_height = font.heightPages ;
	f_width = font.charInfo[f_id].widthBits ;
	f_offset = font.charInfo[f_id].offset ;

	if( (sx + f_width) > 239 ) n_sx = 0 ;
	else n_sx = sx ;

	if( (sy + f_height) >= 319 ) n_sy = 0 ;
	else n_sy = sy ;

	for( i=0 ; i<f_height ; i++ )
	{
		byte_id = (f_width - 1)/8 ; // f_width가 8이면 0 

		for( j=0 ; j<byte_id ; j++ )
		{
			t_id = f_offset + i * (byte_id + 1) + j ;

			if( ch == ' ' ) t_data = 0 ;
			else t_data = font.data[ t_id ] ;

			for( k=0 ; k<8 ; k++ )
			{
				t_bit = t_data & (0x80>>k) ;

				xpos = n_sx + j*8 + k ;
				ypos = n_sy + i ;

				if( t_bit != 0 ) 
				{
					put_pixel(xpos, ypos, color) ;
				}
				else
				{
					put_pixel(xpos, ypos, BG_color) ;
				}
			}
		}

		t_id = f_offset + i * (byte_id + 1) + byte_id ;

		if( ch == ' ' ) t_data = 0 ;
		else t_data = font.data[ t_id ] ;

		bit_id = (f_width - 1)%8 ;
		for( k=0 ; k< (bit_id+1) ; k++ )
		{
			t_bit = t_data & (0x80>>k) ;

			xpos = n_sx + byte_id*8 + k ;
			ypos = n_sy + i ;

			if( t_bit != 0 ) 
			{
				put_pixel(xpos, ypos, color) ;
			}
			else
			{
				put_pixel(xpos, ypos, BG_color) ;
			}
		}

		// 우측에 빈 줄 1줄 그림
		xpos = n_sx + byte_id*8 + bit_id + 1 ;
		if( xpos <= 239 ) 
		{
			ypos = n_sy + i ;
			put_pixel(xpos, ypos, BG_color) ;
		}

		// 우측에 빈 줄 1줄 그림
		xpos = n_sx + byte_id*8 + bit_id + 2 ;
		if( xpos <= 239 ) 
		{
			ypos = n_sy + i ;
			put_pixel(xpos, ypos, BG_color) ;
		}
	}

	return(f_width + 2) ;
}

void draw_rectangle_size(uint16_t x1, uint16_t y1, uint16_t xsize, uint16_t ysize, uint16_t color) 
{
	uint16_t i, ys, ye, x2, y2 ;

	x2 = x1 + xsize ;
	y2 = y1 + ysize ;

	if( y2 > y1 ) {
		ys = y1 ; ye = y2 ;
	}
	else {
		ys = y2 ; ye = y1 ;
	}

	put_line(x1, y1, x2, y1, color ) ;
	put_line(x1, y2, x2, y2, color ) ;
	put_line(x1, y1, x1, y2, color ) ;
	put_line(x2, y1, x2, y2, color ) ;
}

void draw_fill_box_4p(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) 
{
	int i, ys, ye ;

	if( y2 > y1 ) {
		ys = y1 ; ye = y2 ;
	}
	else {
		ys = y2 ; ye = y1 ;
	}

	put_fill_box(x1, y1, x2, y2, color) ;
}

void draw_fill_box_size(uint16_t x1, uint16_t y1, uint16_t xsize, uint16_t ysize, uint16_t color) 
{
	int i, ys, ye, x2, y2 ;

	x2 = x1 + xsize ;
	y2 = y1 + ysize ;

	if( y2 > y1 ) {
		ys = y1 ; ye = y2 ;
	}
	else {
		ys = y2 ; ye = y1 ;
	}

	put_fill_box(x1, ys, x2, ye, color) ;
}


void draw_circle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius,uint16_t color)      //동그라미 선 그리기
{
  int32_t  D;       /* Decision Variable */ 
  uint32_t  CurX;   /* Current X Value */
  uint32_t  CurY;   /* Current Y Value */ 
  
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  
  while (CurX <= CurY)
  {
	//void put_pixel(uint16_t x, uint16_t y, uint16_t color)
    put_pixel((Xpos + CurX), (Ypos - CurY), color);

    put_pixel((Xpos - CurX), (Ypos - CurY), color);

    put_pixel((Xpos + CurY), (Ypos - CurX), color);

    put_pixel((Xpos - CurY), (Ypos - CurX), color);

    put_pixel((Xpos + CurX), (Ypos + CurY), color);

    put_pixel((Xpos - CurX), (Ypos + CurY), color);

    put_pixel((Xpos + CurY), (Ypos + CurX), color);

    put_pixel((Xpos - CurY), (Ypos + CurX), color);   

    /* Initialize the font */
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  } 
}


uint16_t mk_16bit_color( uint8_t R, uint8_t G, uint8_t B)
{
	uint16_t color ;

	color = ((R>>3)<<11) + ((G>>2)<<5) + ((B>>3)) ;
	return(color) ;
}

//----------------------------------------------------------------------
// 다음 세 함수는 Display를 위한 기본 Driving 함수이다.
// LCD에 적용시 바뀔 함수들이다.
//----------------------------------------------------------------------


int proc_num = 0 ;

void put_pixel(uint16_t x, uint16_t y, uint16_t color)
{
	uint8_t R,G,B ;
	FILE *fa ;

	// RGB565로 부터 8비트 컬러를 만든다
	R = (color >> 11) << 3 ;
	G = 0xFF&((color >> 5)<<2) ;
	B = (0x1F&color) << 3 ;

	while( (fa = fopen("fifo.txt","a")) == NULL ) ;
	fprintf(fa,"%9d %3d %3d %3d %3d %3d %3d\n",proc_num,0,x,y, R,G,B) ;
	++proc_num ;
	fclose(fa) ;
}

void put_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	uint8_t R,G,B ;
	FILE *fa ;

	// RGB565로 부터 8비트 컬러를 만든다
	R = (color >> 11) << 3 ;
	G = 0xFF&((color >> 5)<<2) ;
	B = (0x1F&color) << 3 ;

	while( (fa = fopen("fifo.txt","a")) == NULL ) ;
	fprintf(fa,"%9d %3d %3d %3d %3d %3d %3d %3d %3d\n",proc_num,1,x1,y1,x2,y2, R,G,B) ;
	++proc_num ;
	fclose(fa) ;
}

void put_fill_box(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	uint8_t R,G,B ;
	FILE *fa ;

	// RGB565로 부터 8비트 컬러를 만든다
	R = (color >> 11) << 3 ;
	G = 0xFF&((color >> 5)<<2) ;
	B = (0x1F&color) << 3 ;

	while( (fa = fopen("fifo.txt","a")) == NULL ) ;
	fprintf(fa,"%9d %3d %3d %3d %3d %3d %3d %3d %3d\n",proc_num,2,x1,y1,x2,y2, R,G,B) ;
	++proc_num ;
	fclose(fa) ;
}

//----------------------------------------------------------------------
// 컬러 LED 패널을 화면에 켜기 위한 루틴
//----------------------------------------------------------------------

void led_dac(uint16_t v3K, uint16_t v5K, uint16_t vR, uint16_t vG, uint16_t vB)
{
	FILE *fa ;

	while( (fa = fopen("fifo.txt","a")) == NULL ) ;
	fprintf(fa,"%9d %3d %3d %3d %3d %3d %3d\n",proc_num,3,v3K,v5K,vR,vG,vB) ;
	++proc_num ;
	fclose(fa) ;
}
