/*******************************************************************************
  * File Name          : LPH8731-3C.c
  * Description        : LCD driver (Siemens M60)
  *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "LPH8731-3C.h"
#include "Ascii_tab.h"

/* Private variables ---------------------------------------------------------*/
unsigned int rot=0; // угол поворота экрана (0, 90, 180, 270)

/* Private function prototypes -----------------------------------------------*/
static void LCD_port_init(void);

//*************************************************************
//Команда/Данные
#define CMD 0
#define DAT 1
char RS_old;
//*************************************************************

//===============================================================
//			Задержка времени в мкс
//===============================================================
void lcd_delay_us(unsigned int t)
{
	unsigned long i;
	i = t*SYSCLK;
	while(i--);
}

//===============================================================
//			Задержка времени в мс
//===============================================================
void lcd_delay_ms(unsigned int t)
{
	unsigned long i;
	i = t*SYSCLK*100;
	while(i--);
}

//===============================================================
//			Инициализация портов
//===============================================================
static void LCD_port_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStruct.GPIO_Pin = LCD_CS | LCD_RESET | LCD_RS | LCD_CLK | LCD_DATA;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD_PORT, &GPIO_InitStruct);
	
	
	
		
}

//===============================================================
//                          ИНИЦИАЛИЗАЦИЯ
//===============================================================
void LCD_init ()
{
 LCD_port_init();
 //Сброс
	LCD_CS_LO();
	LCD_DATA_LO();
	LCD_RESET_LO();
 lcd_delay_ms(100);
	LCD_RESET_HI();
 lcd_delay_ms(500);
	LCD_CS_HI();
 lcd_delay_us(5);
	LCD_CS_LO();
 //Инициализация
 Send_to_lcd(CMD,0x01); //reset sw
 lcd_delay_ms(50);
 Send_to_lcd(CMD,0xc6); //initial escape
 lcd_delay_ms(40);
 Send_to_lcd(CMD,0xb9); //Refresh set
 Send_to_lcd(DAT,0x00);
 Send_to_lcd(CMD,0xb6); //Display control
 Send_to_lcd(DAT,0x80); //
 Send_to_lcd(DAT,0x04); //
 Send_to_lcd(DAT,0x8a); //
 Send_to_lcd(DAT,0x54); //
 Send_to_lcd(DAT,0x45); //
 Send_to_lcd(DAT,0x52); //
 Send_to_lcd(DAT,0x43); //
 Send_to_lcd(CMD,0xb3); //Gray scale position set 0
 Send_to_lcd(DAT,0x02); //
 Send_to_lcd(DAT,0x0a); //
 Send_to_lcd(DAT,0x15); //
 Send_to_lcd(DAT,0x1f); //
 Send_to_lcd(DAT,0x28); //
 Send_to_lcd(DAT,0x30); //
 Send_to_lcd(DAT,0x37); //
 Send_to_lcd(DAT,0x3f); //
 Send_to_lcd(DAT,0x47); //
 Send_to_lcd(DAT,0x4c); //
 Send_to_lcd(DAT,0x54); //
 Send_to_lcd(DAT,0x65); //
 Send_to_lcd(DAT,0x75); //
 Send_to_lcd(DAT,0x80); //
 Send_to_lcd(DAT,0x85); //
 Send_to_lcd(CMD,0xb5); //Gamma curve
 Send_to_lcd(DAT,0x01); //
 Send_to_lcd(CMD,0xb7); //Temperature gradient
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(CMD,0xbd); //Common driver output select
 Send_to_lcd(DAT,0x00); //
 /*
 Send_to_lcd(CMD,0x33); //Vertical scrolling definition - Границы вертикальной прокрутки
 Send_to_lcd(DAT,0x00); //Число линий, используемых в качестве верхней фиксированной области дисплея (max - 0x53)
 Send_to_lcd(DAT,0x53); //Определяем область прокрутки
 Send_to_lcd(DAT,0x00); //Число линий, используемых в качестве нижней фиксированной области дисплея
 Send_to_lcd(CMD,0x37); //Vertical scrolling start adress
 Send_to_lcd(DAT,0x00);
*/
 Send_to_lcd(CMD,0x36); //Memory access control - Метод доступа к дисплейной памяти (установка нулевого адреса страницы и столбца, направление записи в память дисплея)
 Send_to_lcd(DAT,0x48); //6-й бит, установленный в "1" делает адресацию столбца - слева-направо, а 3-й бит в "1" - порядок RGB
 Send_to_lcd(CMD,0x2d); //Colour set
 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x03); //
 Send_to_lcd(DAT,0x05); //
 Send_to_lcd(DAT,0x07); //
 Send_to_lcd(DAT,0x09); //
 Send_to_lcd(DAT,0x0b); //
 Send_to_lcd(DAT,0x0d); //
 Send_to_lcd(DAT,0x0f); //

 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x03); //
 Send_to_lcd(DAT,0x05); //
 Send_to_lcd(DAT,0x07); //
 Send_to_lcd(DAT,0x09); //
 Send_to_lcd(DAT,0x0b); //
 Send_to_lcd(DAT,0x0d); //
 Send_to_lcd(DAT,0x0f); //

 Send_to_lcd(DAT,0x00); //
 Send_to_lcd(DAT,0x05); //
 Send_to_lcd(DAT,0x0b); //
 Send_to_lcd(DAT,0x0f); //
 Send_to_lcd(CMD,0xba); //Voltage control
 Send_to_lcd(DAT,0x2f); //
 Send_to_lcd(DAT,0x03); //
 Send_to_lcd(CMD,0x25); //Установка контраста
 Send_to_lcd(DAT,0x61); //
 Send_to_lcd(CMD,0xbe); //Power control
 Send_to_lcd(DAT,0x58); //

 Send_to_lcd(CMD,0x3a); //Информация о выводе пикселя
 #ifdef _8_BIT_COLOR
 Send_to_lcd(DAT,0x02); //8-ми битный цвет
 #else
 Send_to_lcd(DAT,0x03); //12-ти битный цвет
 #endif
 Send_to_lcd(CMD,0x03); //Booster voltage ON
 lcd_delay_ms(40);
 Send_to_lcd(CMD,0x11); //Выход из спящего режима

 lcd_delay_ms(20); 		//Перед включением ждем 20 миллисекунд
 Send_to_lcd(CMD,0x29); //Включение дисплея
}


//===============================================================
//         Задание угла поворота экрана
//===============================================================
void SetRotation (unsigned int angle)
{
    rot=angle;
    Send_to_lcd(CMD, 0x36);
    switch (rot)
    {
        case 0:
        Send_to_lcd(DAT, 0x48); //Начальный адрес осей Х и У - левый верхний угол дисплея
        break;
        //================================
        case 90:
        Send_to_lcd(DAT, 0x08); //Начальный адрес осей Х и У - правый верхний угол дисплея
        break;
        //================================
        case 180:
        Send_to_lcd(DAT, 0x88); //Начальный адрес осей Х и У - правый нижний угол дисплея
        break;
        //================================
        case 270:
        Send_to_lcd(DAT, 0xC8); //Начальный адрес осей Х и У - левый нижний угол дисплея
        break;
        //================================
    };
}

//===============================================================
//              Задание прямоугольной области экрана
//===============================================================
void SetArea(char x1, char x2, char y1, char y2)
{
 Send_to_lcd( CMD, 0x2A );  //задаем область по X
 Send_to_lcd( DAT, x1 );    //начальная
 Send_to_lcd( DAT, x2 );    //конечная

 Send_to_lcd( CMD, 0x2B );  //задаем область по Y
 Send_to_lcd( DAT, y1+1 );  //начальная (у этого контроллера Y отсчитывается от 1, а не от 0)
 Send_to_lcd( DAT, y2+1 );  //конечная

 Send_to_lcd( CMD, 0x2C );  //отправляем команду на начало записи в память и начинаем посылать данные
}

//===============================================================
//                          Рисуем точку
//===============================================================
void Put_Pixel (char x, char y, unsigned int color)
{
 SetArea( x, x, y, y );
 #ifdef _8_BIT_COLOR	//(8-ми битовая цветовая палитра (256 цветов))
 Send_to_lcd( DAT, color ); //Данные - задаём цвет пикселя
 #else			//(12-ми битовая цветовая палитра (4096 цветов))
 Send_to_lcd( DAT, color>>4 );  Send_to_lcd( DAT, color<<4 );
 #endif
}

//===============================================================
//           Функция прорисовки символа на дисплее
//===============================================================
void Send_Symbol (unsigned char symbol, char x, char y, int t_color, int b_color, char zoom_width, char zoom_height)
{
 unsigned char temp_symbol, a, b, zw, zh, mask;
 char m, n;
 m=x;
 n=y;
 if (symbol>127) symbol-=64;    //Убираем отсутствующую часть таблицы ASCII
 for ( a = 0; a < 5; a++) //Перебираю 5 байт, составляющих символ
 {
  temp_symbol = font_5x8[symbol-32][a];
  zw = 0;
  while(zw != zoom_width) //Вывод байта выполняется zw раз
  {
   switch(rot)
   {
    case 0: case 180: n=y; break;
    case 90: case 270: m=x; break;
   }
   mask=0x01;
   for ( b = 0; b < 8; b++ ) //Цикл перебирания 8 бит байта
   {
    zh = 0; //в zoom_height раз увеличится высота символа
    while(zh != zoom_height) //Вывод пикселя выполняется z раз
    {
     switch(rot)
     {
      case 0: case 180:
      if (temp_symbol&mask)
      {
       Put_Pixel (m+zw, n+zh, t_color);
      }
      else
      {
       Put_Pixel (m+zw, n+zh, b_color);
      }
      break;
      case 90: case 270:
      if (temp_symbol&mask)
      {
       Put_Pixel (m+zh, n+zw, t_color);
      }
      else
      {
       Put_Pixel (m+zh, n+zw, b_color);
      }
      break; //Получить адрес начального пикселя по оси y для вывода очередного байта
     }
     zh++;
    }
    mask<<=1; //Смещаю содержимое mask на 1 бит влево;
    switch(rot)
    {
     case 0: case 180: n=n+zoom_height; break;
     case 90: case 270: m=m+zoom_height; break;
    }
   }
   zw++;
  }
  switch(rot)
  {
   case 0: case 180: m=m+zoom_width; break;
   case 90: case 270: n=n+zoom_width; break;
  }
 }
}


//===============================================================
// Функция вывода одного символа ASCII-кода (из файла Symbols.h)
//===============================================================
void LCD_Putchar(char symbol, char x, char y, int t_color, int b_color, char zoom_width, char zoom_height)
{
    unsigned char m;
    if(zoom_width == 0)   zoom_width = 1;
    if(zoom_height == 0)  zoom_height = 1;
    switch (rot)
    {
        case 90:	m=y+3; y=x; x=m;
               		break;
        case 180:       x+=3; y+=2;
      			break;
        case 270:       m=y; y=x+2; x=m;
    			break;
    };
    Send_Symbol( symbol, x, y, t_color, b_color, zoom_width, zoom_height);
}


//===============================================================
//          Функция вывода строки
//===============================================================
void LCD_Puts(const char *str, int x, int y,  int t_color, int b_color, char zoom_width, char zoom_height)
{
    unsigned char i=0;

    if(zoom_width == 0)   zoom_width = 1;
    if(zoom_height == 0)  zoom_height = 1;

    while (str[i]) //x и y - адрес пикселя начальной позиции; с увеличением переменной i адрес вывода очередного символа смещается на i*6 (чем организуются столбцы дисплея)
    {
        LCD_Putchar(str[i], x+(i*6*zoom_width), y, t_color, b_color, zoom_width, zoom_height);
        i++;
    }
}

//===============================================================
//     Функция прорисовки символа на дисплее без цвета фона
//===============================================================
void Send_Symbol_Shadow (unsigned char symbol, char x, char y, int t_color, char zoom_width, char zoom_height)
{
 unsigned char temp_symbol, a, b, zw, zh, mask;
 char m, n;
 m=x;
 n=y;
 if (symbol>127) symbol-=64;    //Убираем отсутствующую часть таблицы ASCII
 for ( a = 0; a < 5; a++) //Перебираю 5 байт, составляющих символ
 {
  temp_symbol = font_5x8[symbol-32][a];
  zw = 0;
  while(zw != zoom_width) //Вывод байта выполняется zw раз
  {
   switch(rot)
   {
    case 0: case 180: n=y; break;
    case 90: case 270: m=x; break;
   }
   mask=0x01;
   for ( b = 0; b < 8; b++ ) //Цикл перебирания 8 бит байта
   {
    zh = 0; //в zoom_height раз увеличится высота символа
    while(zh != zoom_height) //Вывод пикселя выполняется z раз
    {
     switch(rot)
     {
      case 0: case 180:
      if (temp_symbol&mask)
      {
       Put_Pixel (m+zw, n+zh, t_color);
      }
      break;
      case 90: case 270:
      if (temp_symbol&mask)
      {
       Put_Pixel (m+zh, n+zw, t_color);
      }
      break; //Получить адрес начального пикселя по оси y для вывода очередного байта
     }
     zh++;
    }
    mask<<=1; //Смещаю содержимое mask на 1 бит влево;
    switch(rot)
    {
     case 0: case 180: n=n+zoom_height; break;
     case 90: case 270: m=m+zoom_height; break;
    }
   }
   zw++;
  }
  switch(rot)
  {
   case 0: case 180: m=m+zoom_width; break;
   case 90: case 270: n=n+zoom_width; break;
  }
 }
}

//===============================================================
// Функция вывода одного символа ASCII-кода без цвета фона
//===============================================================
void LCD_Putchar_Shadow (char symbol, char x, char y, int t_color, char zoom_width, char zoom_height)
{
 unsigned char m;
 if(zoom_width == 0)   zoom_width = 1;
 if(zoom_height == 0)  zoom_height = 1;
 switch (rot)
 {
  case 90:	m=y+3; y=x; x=m;
  		break;
  case 180:	x+=3; y+=2;
  		break;
  case 270:	m=y; y=x+2; x=m;
  		break;
 };
 Send_Symbol_Shadow( symbol, x, y, t_color, zoom_width, zoom_height);
}

//===============================================================
//   Функция вывода строки без цвета фона
//===============================================================
void LCD_Puts_Shadow(const char *str, int x, int y,  int t_color, char zoom_width, char zoom_height)
{
 unsigned char i=0;

 if(zoom_width == 0)   zoom_width = 1;
 if(zoom_height == 0)  zoom_height = 1;

 while (str[i]) //x и y - адрес пикселя начальной позиции; с увеличением переменной i адрес вывода очередного символа смещается на i*6 (чем организуются столбцы дисплея)
 {
  LCD_Putchar_Shadow(str[i], x+(i*6*zoom_width), y, t_color, zoom_width, zoom_height);
  i++;
 }
}

//===============================================================
//                  ЗАЛИВКА ЭКРАНА ЦВЕТОМ
//===============================================================
void LCD_FillScreen (unsigned int color)
{
 unsigned int x;
 SetArea( 0, 101, 0, 80 );   //Область всего экрана
 for (x = 0; x < 101*81; x++)
 {
  #ifdef _8_BIT_COLOR        //(8-ми битовая цветовая палитра (256 цветов))
  Send_to_lcd( DAT, color ); //Данные - задаём цвет пикселя
  #else                      //(12-ми битовая цветовая палитра (4096 цветов))
  Send_to_lcd( DAT, color>>4 );  Send_to_lcd( DAT, (color<<4)|(color>>8) ); Send_to_lcd( DAT, color ); x++; //Задаём цвет двух пикселей
  #endif
 }
}

//===============================================================
//                 ФУНКЦИЯ ВЫВОДА ИЗОБРАЖЕНИЯ
//===============================================================
void LCD_Output_image (char x, char y, char width, char height, const char *img)
{
 unsigned char m;
 switch (rot)
 {
  case 90:	m=y+3; y=x; x=m;
  		break;
  case 180:	x+=3; y+=2;
  		break;
  case 270:	m=y; y=x+2; x=m;
  		break;
 };
 Send_Image (x, y, width, height, img);
}

//===============================================================
//          Функция для обеспечения вывода изображения
//===============================================================
//Вывод картинки с Image2Lcd и NokiaImageCreator должен выполняться слева-направо сверху-вниз.
//x, y - начало области вывода изображения; width и height - ширина и высота изображения
// Примечание! Для 12-ти битного режима ширина изображения (width) должна быть кратна 2!
void Send_Image (char x, char y, char width, char height, const char *img)
{
 char x1, y1;
 y+=1;
 switch (rot)
 {
  case 0: case 180:
  for(y1=y; y1<(y+height); y1++)
  {
   SetArea( x, x+(width-1), y1, y1 );
   for(x1=x; x1<x+width; x1++)
   {
    #ifdef _8_BIT_COLOR	//(8-ми битовая цветовая палитра (256 цветов))
    Send_to_lcd( DAT, *img++ ); //Данные - задаём цвет пикселя
    #else			//(12-ти битовая цветовая палитра (4096 цветов))
    Send_to_lcd( DAT, (*img<<4)|(*(img+1)>>4) ); img++; Send_to_lcd( DAT, (*img<<4)|(*(img+1)) ); img+=2;  Send_to_lcd( DAT, *img++ ); x1++;  //Задаём цвет двух пикселей
    #endif
   }
  }
  break;

  case 90: case 270:
  for(x1=x; x1<x+height; x1++)
  {
   SetArea( x1, x1, y, y+(width-1) );
   for(y1=y; y1<y+width; y1++)
   {
    #ifdef _8_BIT_COLOR	//(8-ми битовая цветовая палитра (256 цветов))
    Send_to_lcd( DAT, *img++ ); //Данные - задаём цвет пикселя
    #else			//(12-ти битовая цветовая палитра (4096 цветов))
   Send_to_lcd( DAT, (*img<<4)|(*(img+1)>>4) ); img++; Send_to_lcd( DAT, (*img<<4)|(*(img+1)) ); img+=2;  Send_to_lcd( DAT, *img++ ); y1++;  //Задаём цвет двух пикселей
    #endif
   }
  }
  break;
 };
}

#ifdef _GEOMETRICAL
//===============================================================
//                      НАРИСОВАТЬ ЛИНИЮ
//===============================================================
void LCD_DrawLine (char x1, char y1, char x2, char y2, int color)
{
 short  x, y, d, dx, dy, i, i1, i2, kx, ky;
 signed char flag;
 unsigned char m;

 switch (rot)
 {
    case 90: case 270:  m=y1; y1=x1; x1=m; m=y2; y2=x2; x2=m;
                        break;
 };

 dx = x2 - x1;
 dy = y2 - y1;
 if (dx == 0 && dy == 0) Put_Pixel(x1, y1, color);  //Точка
 else      //Линия
 {
  kx = 1;
  ky = 1;
  if( dx < 0 )
  {
   dx = -dx;
   kx = -1;
  }
  else
  if(dx == 0) kx = 0;
  if(dy < 0)
  {
   dy = -dy;
   ky = -1;
  }
  if(dx < dy)
  {
   flag = 0;
   d = dx;
   dx = dy;
   dy = d;
  }
  else flag = 1;
  i1 = dy + dy;
  d = i1 - dx;
  i2 = d - dx;
  x = x1;
  y = y1;

  for(i=0; i < dx; i++)
  {
   Put_Pixel(x, y, color);
   if(flag) x += kx;
   else y += ky;
   if( d < 0 ) d += i1;
   else
   {
    d += i2;
    if(flag) y += ky;
    else x += kx;
   }
  }
  Put_Pixel(x, y, color);
 }
}

//===============================================================
//             НАРИСОВАТЬ ЛИНИЮ С ЗАДАВАЕМОЙ ШИРИНОЙ
//===============================================================
void LCD_FillLine (char tx1, char ty1, char tx2, char ty2, char width, int color)
{
    signed long x, y, addx, dx, dy;
    signed long P;
    int i;
    long x1,x2,y1,y2;

   if(ty1>ty2)  {y1=ty2; y2=ty1; x1=tx2; x2=tx1;}
   else         {y1=ty1; y2=ty2; x1=tx1; x2=tx2;}
    dx = x2 - x1;
    dy = y2 - y1;
   if(dx<0)     dx=-dx;
   if(dy<0)     dy=-dy;
    x = x1;
    y = y1;

   if(x1 > x2)  addx = -1;
   else         addx = 1;

   if(dx >= dy)
   {
    P = 2*dy - dx;

      for(i=0; i<=dx; ++i)
      {
        LCD_FillRect (x, y, width, width, color);

         if(P < 0)
         {
            P += 2*dy;
            x += addx;
         }
         else
         {
            P += 2*dy - 2*dx;
            x += addx;
            y ++;
         }
      }
   }
   else
   {
    P = 2*dx - dy;

      for(i=0; i<=dy; ++i)
      {
        LCD_FillRect (x, y, width, width, color);

         if(P < 0)
         {
            P += 2*dx;
            y ++;
         }
         else
         {
            P += 2*dx - 2*dy;
            x += addx;
            y ++;
         }
      }
   }
}

//===============================================================
//			НАРИСОВАТЬ РАМКУ
//===============================================================
void LCD_DrawRect (char x1, char y1, char width, char height, char size, int color)
{
 unsigned int i;
 char x2=x1+(width-1), y2=y1+(height-1); //Конечные размеры рамки по осям х и у

  for( i=1; i<=size; i++)   // size - толщина рамки
 {
  LCD_DrawLine(x1, y1, x1, y2, color);
  LCD_DrawLine(x2, y1, x2, y2, color);
  LCD_DrawLine(x1, y1, x2, y1, color);
  LCD_DrawLine(x1, y2, x2, y2, color);
  x1++; // Увеличиваю толщину рамки, если это задано
  y1++;
  x2--;
  y2--;
 }
}

//===============================================================
//              ЗАПОЛНИТЬ ПРЯМОУГОЛЬНИК ЦВЕТОМ COLOR
//===============================================================
void LCD_FillRect (char x1, char y1, char width, char height, int color)
{
 unsigned int x;
 unsigned char m;

 switch (rot)
 {
    case 90: case 270:  m=y1; y1=x1; x1=m; m=width; width=height; height=m; break;
 };

 SetArea( x1, x1+(width-1), y1, y1+(height-1) );

 for (x = 0; x < width * height; x++)
 {
  #ifdef _8_BIT_COLOR        //(8-ми битовая цветовая палитра (256 цветов))
  Send_to_lcd( DAT, color ); Send_to_lcd( DAT, color ); //Данные - задаём цвет пикселя
  #else
  Send_to_lcd( DAT, color>>4 ); Send_to_lcd( DAT, (color<<4)|(color>>8) ); Send_to_lcd( DAT, color ); x++; //Задаём цвет двух пикселей
  #endif
 }
}

//===============================================================
//                  НАРИСОВАТЬ ОКРУЖНОСТЬ
//===============================================================
void LCD_DrawCircle (char xcenter, char ycenter, char rad, int color)
{
    signed int k, b, P;
    unsigned char m;

    switch (rot)
    {
        case 90: case 270:  m=ycenter; ycenter=xcenter; xcenter=m; break;
    };

    k = 0;
    b = rad;
    P = 1 - rad;
    do
    {
        Put_Pixel(k+xcenter, b+ycenter, color);
        Put_Pixel(b+xcenter, k+ycenter, color);
        Put_Pixel(xcenter-k, b+ycenter, color);
        Put_Pixel(xcenter-b, k+ycenter, color);
        Put_Pixel(b+xcenter, ycenter-k, color);
        Put_Pixel(k+xcenter, ycenter-b, color);
        Put_Pixel(xcenter-k, ycenter-b, color);
        Put_Pixel(xcenter-b, ycenter-k, color);

        if(P < 0)   P+= 3 + 2*k++;
        else        P+= 5 + 2*(k++ - b--);
    } while(k <= b);
}


//===============================================================
//                 ЗАПОЛНИТЬ КРУГ ЦВЕТОМ COLOR
//===============================================================
void LCD_FillCircle (char xcenter, char ycenter, char rad, int color)
{
    signed int x1=0, y1, tswitch;
    y1 = rad;
    tswitch = 1 - rad;

    do
    {
        LCD_DrawLine(xcenter-x1, ycenter+y1, xcenter+x1, ycenter+y1, color);
        LCD_DrawLine(xcenter-x1, ycenter-y1, xcenter+x1, ycenter-y1, color);
        LCD_DrawLine(xcenter-y1, ycenter+x1, xcenter+y1, ycenter+x1, color);
        LCD_DrawLine(xcenter-y1, ycenter-x1, xcenter+y1, ycenter-x1, color);

    if(tswitch < 0)
    tswitch+= 3 + 2*x1++;
    else
    tswitch+= 5 + 2*(x1++ - y1--);
    } while(x1 <= y1);
}

//===============================================================
//                     НАРИСОВАТЬ ТРЕУГОЛЬНИК
//===============================================================
void LCD_DrawTriangle(char x1, char y1, char x2, char y2, char x3, char y3, int color)
{
 LCD_DrawLine(x1, y1, x2, y2, color);
 LCD_DrawLine(x3, y3, x1, y1, color);
 LCD_DrawLine(x3, y3, x2, y2, color);
}

//===============================================================
//              ЗАПОЛНИТЬ ТРЕУГОЛЬНИК ЦВЕТОМ COLOR
//===============================================================
void LCD_FillTriangle(char x1, char y1, char x2, char y2, char x3, char y3, int color)
{
 LCD_FillTriangleA(x1, y1, x2, y2, x3, y3, color);
 LCD_FillTriangleA(x3, y3, x1, y1, x2, y2, color);
 LCD_FillTriangleA(x3, y3, x2, y2, x1, y1, color);
}

//===============================================================
void LCD_FillTriangleA(char x1, char y1, char x2, char y2, char x3, char y3, int color)
{
 signed long x, y, addx, dx, dy;
 signed long P;
 int i;
 long a1,a2,b1,b2;
 if(y1>y2)  {b1=y2; b2=y1; a1=x2; a2=x1;}
 else       {b1=y1; b2=y2; a1=x1; a2=x2;}
 dx = a2 -a1;
 dy = b2 - b1;
 if(dx<0)dx=-dx;
 if(dy<0)dy=-dy;
 x = a1;
 y = b1;

 if(a1 > a2)    addx = -1;
 else           addx = 1;

 if(dx >= dy)
 {
  P = 2*dy - dx;
  for(i=0; i<=dx; ++i)
  {
   LCD_DrawLine((int)x, (int)y, x3, y3, color);
   if(P < 0)
   {
    P += 2*dy;
    x += addx;
   }
   else
   {
    P += 2*dy - 2*dx;
    x += addx;
    y ++;
   }
  }
 }
 else
 {
  P = 2*dx - dy;
  for(i=0; i<=dy; ++i)
  {
   LCD_DrawLine((int)x, (int)y, x3, y3, color);
   if(P < 0)
   {
    P += 2*dx;
    y ++;
   }
   else
   {
    P += 2*dx - 2*dy;
    x += addx;
    y ++;
   }
  }
 }
}

//===============================================================
#endif //_GEOMETRICAL

void ClkPulse (void)
{
	LCD_CLK_HI();
//	lcd_delay_us(1);
	LCD_CLK_LO();
}
//===============================================================
//			            Программный SPI
//===============================================================
void Send_spi(unsigned char data)
{
	LCD_DATA_LO();
	if ((data & 128) == 128) LCD_DATA_HI();
	ClkPulse();
	LCD_DATA_LO();
	if ((data & 64) == 64) LCD_DATA_HI();
  ClkPulse();
	LCD_DATA_LO();
	if ((data & 32) == 32) LCD_DATA_HI();
  ClkPulse();
	LCD_DATA_LO();
	if ((data & 16) ==16) LCD_DATA_HI();
  ClkPulse();
	LCD_DATA_LO();
	if ((data & 8) == 8) LCD_DATA_HI();
  ClkPulse();
	LCD_DATA_LO();
	if ((data & 4) == 4) LCD_DATA_HI();
  ClkPulse();
	LCD_DATA_LO();
	if ((data & 2) == 2) LCD_DATA_HI();
  ClkPulse();
	LCD_DATA_LO();
	if ((data & 1) == 1) LCD_DATA_HI();
  ClkPulse();
}

//===============================================================
//Функция записи команды/данных в LCD (RS==0 - команда, RS==1 - данные)
//===============================================================
void Send_to_lcd (unsigned char RS, unsigned char data)
{
	LCD_CLK_LO();
	LCD_DATA_LO();
	
	if ((RS_old != RS) || (!RS_old && !RS))				 	//Проверяем старое значение RS
	{
		LCD_CS_HI();																	// Установка CS
		if(RS)  LCD_RS_HI();
		else    LCD_RS_LO();
		LCD_CS_LO();																// Сброс CS
	}
	
	Send_spi (data);
	
	RS_old=RS;  //запоминаем значение RS
	
	LCD_DATA_LO();
}


