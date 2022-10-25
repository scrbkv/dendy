#include "lcd.h"
#include "LCD_font.h"

volatile uint8_t* FMC_DATA = (uint8_t*)0xC0080000;
volatile uint8_t* FMC_REG = (uint8_t*)0xC0000000;

void Init() {
	*FMC_REG = 0xE0;//MDDI Control 1 (E0h)
		*FMC_DATA = 0x01;//VWAKE_EN=1, When VWAKE_EN is 1, client initiated wake-up is enabled

		*FMC_REG = 0x11;//Sleep Out (11h)
		HAL_Delay(150);

		*FMC_REG = 0xF3;//Power Control Register (F3h)
		*FMC_DATA = 0x01;//VCI1_EN
		*FMC_DATA = 0x00;//
		*FMC_DATA = 0x00;//
		*FMC_DATA = 0x0C;//Do not set any higher VCI1 level than VCI -0.15V. 0C 0A
		*FMC_DATA = 0x03;//VGH和VGL 01 02VGH=6VCI1,VGL=-4VCI1.
		*FMC_DATA = 0x75;//
		*FMC_DATA = 0x75;//
		*FMC_DATA = 0x30;//

		*FMC_REG = 0xF4;//VCOM Control Register (F4h)
		*FMC_DATA = 0x4C;//
		*FMC_DATA = 0x4C;//
		*FMC_DATA = 0x44;//
		*FMC_DATA = 0x44;//
		*FMC_DATA = 0x22;//

		*FMC_REG = 0xF5;//Source Output Control Register (F5h)
		*FMC_DATA = 0x10;//
		*FMC_DATA = 0x22;//
		*FMC_DATA = 0x05;//
		*FMC_DATA = 0xF0;//
		*FMC_DATA = 0x70;//
		*FMC_DATA = 0x1F;//
		HAL_Delay(30);//

		*FMC_REG = 0xF3;//Power Control Register (F3h)
		*FMC_DATA = 0x03;//

		HAL_Delay(30);

		*FMC_REG = 0xF3;//Power Control Register (F3h)
		*FMC_DATA = 0x07;//

		HAL_Delay(30);

		*FMC_REG = 0xF3;//Power Control Register (F3h)
		*FMC_DATA = 0x0F;//

		HAL_Delay(30);

		*FMC_REG = 0xF3;//Power Control Register (F3h)
		*FMC_DATA = 0x1F;//

		HAL_Delay(30);

		*FMC_REG = 0xF3;//Power Control Register (F3h)
		*FMC_DATA = 0x7F;//

		HAL_Delay(30);

		*FMC_REG = 0xF7;//Positive Gamma Control Register for Red (F7h)
		*FMC_DATA = 0x80;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x05;
		*FMC_DATA = 0x0D;
		*FMC_DATA = 0x1F;
		*FMC_DATA = 0x26;
		*FMC_DATA = 0x2D;
		*FMC_DATA = 0x14;
		*FMC_DATA = 0x15;
		*FMC_DATA = 0x26;
		*FMC_DATA = 0x20;
		*FMC_DATA = 0x01;
		*FMC_DATA = 0x22;
		*FMC_DATA = 0x22;

		*FMC_REG = 0xF8;//Negative Gamma Control Register for Red
		*FMC_DATA = 0x80;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x07;
		*FMC_DATA = 0x1E;
		*FMC_DATA = 0x2A;
		*FMC_DATA = 0x32;
		*FMC_DATA = 0x10;
		*FMC_DATA = 0x16;
		*FMC_DATA = 0x36;
		*FMC_DATA = 0x3C;
		*FMC_DATA = 0x3B;
		*FMC_DATA = 0x22;
		*FMC_DATA = 0x22;

		*FMC_REG = 0xF9;//Positive Gamma Control Register for Green
		*FMC_DATA = 0x80;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x05;
		*FMC_DATA = 0x0D;
		*FMC_DATA = 0x1F;
		*FMC_DATA = 0x26;
		*FMC_DATA = 0x2D;
		*FMC_DATA = 0x14;
		*FMC_DATA = 0x15;
		*FMC_DATA = 0x26;
		*FMC_DATA = 0x20;
		*FMC_DATA = 0x01;
		*FMC_DATA = 0x22;
		*FMC_DATA = 0x22;

		*FMC_REG = 0xFA;//Negative Gamma Control Register for Green
		*FMC_DATA = 0x80;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x07;
		*FMC_DATA = 0x1E;
		*FMC_DATA = 0x2A;
		*FMC_DATA = 0x32;
		*FMC_DATA = 0x10;
		*FMC_DATA = 0x16;
		*FMC_DATA = 0x36;
		*FMC_DATA = 0x3C;
		*FMC_DATA = 0x3B;
		*FMC_DATA = 0x22;
		*FMC_DATA = 0x22;

		*FMC_REG = 0xFB;//Positive Gamma Control Register for Blue
		*FMC_DATA = 0x80;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x05;
		*FMC_DATA = 0x0D;
		*FMC_DATA = 0x1F;
		*FMC_DATA = 0x26;
		*FMC_DATA = 0x2D;
		*FMC_DATA = 0x14;
		*FMC_DATA = 0x15;
		*FMC_DATA = 0x26;
		*FMC_DATA = 0x20;
		*FMC_DATA = 0x01;
		*FMC_DATA = 0x22;
		*FMC_DATA = 0x22;

		*FMC_REG = 0xFC;//Negative Gamma Control Register for Blue
		*FMC_DATA = 0x80;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x07;
		*FMC_DATA = 0x1E;
		*FMC_DATA = 0x2A;
		*FMC_DATA = 0x32;
		*FMC_DATA = 0x10;
		*FMC_DATA = 0x16;
		*FMC_DATA = 0x36;
		*FMC_DATA = 0x3C;
		*FMC_DATA = 0x3B;
		*FMC_DATA = 0x22;
		*FMC_DATA = 0x22;

		//*FMC_REG = 0x35);
		*FMC_REG = 0x34; //Tearing Effect Line OFF (34h)

		*FMC_REG = 0x36;//Memory Data Access Control (36h)
		*FMC_DATA = 0x68;//48//08 //A8

		*FMC_REG = 0x3A;//Interface Pixel Format (3Ah)
		*FMC_DATA = 0x55;//101-16bit/pixel
		//ReadReg(0x0C, array, 1);//

		*FMC_REG = 0xF2;//Display Control Register (F2h)
		*FMC_DATA = 0x17;
		*FMC_DATA = 0x17;
		*FMC_DATA = 0x0F;
		*FMC_DATA = 0x08;
		*FMC_DATA = 0x08;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x06;
		*FMC_DATA = 0x13;
		*FMC_DATA = 0x00;

		*FMC_REG = 0xF6;//Interface Control Register (F6h)
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x80;
		*FMC_DATA = 0x00;
		*FMC_DATA = 0x00;
		//ReadReg(0xF6, array, 4);//

		*FMC_REG = 0xFD;//Gate Control Register (FDh)
		*FMC_DATA = 0x02;
		*FMC_DATA = 0x01; //240*400

		HAL_Delay(20);

		*FMC_REG = 0x29;//Display On (29h)
		//ReadReg(0x0B, array, 1);//read MADCTL
		HAL_Delay(20);
}

void LcdInit() {
	HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_SET);

	Init();
	ClearScreen(0x0F0F);

	SetWindow(0,0,239,255);
}

void DrawPixel(uint16_t x,uint16_t y,uint16_t color) {
	  *FMC_REG = 0x2B;//
	  *FMC_DATA = x>>8;//start upper byte
	  *FMC_DATA = (uint8_t)x;//start lower byte

	  *FMC_DATA = x>>8;//end upper byte
	  *FMC_DATA = (uint8_t)x;//end lower byte

	  *FMC_REG = 0x2A;//
	  *FMC_DATA = y>>8;//start upper byte
	  *FMC_DATA = (uint8_t)y;//start lower byte

	  *FMC_DATA = y>>8;//end upper byte
	  *FMC_DATA = (uint8_t)y;//end lower byte

	  *FMC_REG = 0x2C;//Memory Write (2Ch)
	  *FMC_DATA = (uint8_t)(color>>8);
	  *FMC_DATA = (uint8_t)color;
}

void FillScreen(uint16_t xStart,uint16_t yStart,uint16_t xEnd, uint16_t yEnd,uint16_t* data) {
	  *FMC_REG = 0x2A;//
	  *FMC_DATA = yStart>>8;//start upper byte
	  *FMC_DATA = (uint8_t)yStart;//start lower byte
	  *FMC_DATA = yEnd>>8;//end upper byte
	  *FMC_DATA = (uint8_t)yEnd;//end lower byte

	  *FMC_REG = 0x2B;//
	  *FMC_DATA = xStart>>8;//start upper byte
	  *FMC_DATA = (uint8_t)xStart;//start lower byte
 	  *FMC_DATA = xEnd>>8;//end upper byte
	  *FMC_DATA = (uint8_t)xEnd;//end lower byte

	  uint32_t count = (xEnd - xStart + 1)*(yEnd - yStart + 1);
	  *FMC_REG = 0x2C;//Memory Write (2Ch)
	  for (uint32_t i = 0; i < count; ++i) {
		  *FMC_DATA = (uint8_t)(data[i]>>8);
		  *FMC_DATA = (uint8_t)data[i];
	  }
}

void SetWindow(uint16_t xStart,uint16_t yStart,uint16_t xEnd, uint16_t yEnd) {
	  *FMC_REG = 0x2A;//
	  *FMC_DATA = yStart>>8;//start upper byte
	  *FMC_DATA = (uint8_t)yStart;//start lower byte
	  *FMC_DATA = yEnd>>8;//end upper byte
	  *FMC_DATA = (uint8_t)yEnd;//end lower byte

	  *FMC_REG = 0x2B;//
	  *FMC_DATA = xStart>>8;//start upper byte
	  *FMC_DATA = (uint8_t)xStart;//start lower byte
 	  *FMC_DATA = xEnd>>8;//end upper byte
	  *FMC_DATA = (uint8_t)xEnd;//end lower byte

	  *FMC_REG = 0x2C;//Memory Write (2Ch)
}

void ClearScreen(uint16_t color) {
	*FMC_REG = 0x2A;//
		  *FMC_DATA = 0;//start upper byte
		  *FMC_DATA = 0;//start lower byte
		  *FMC_DATA = 399>>8;//end upper byte
		  *FMC_DATA = (uint8_t)399;//end lower byte

		  *FMC_REG = 0x2B;//
		  *FMC_DATA = 0;//start upper byte
		  *FMC_DATA = 0;//start lower byte
	 	  *FMC_DATA = 239>>8;//end upper byte
		  *FMC_DATA = (uint8_t)239;//end lower byte

		  uint32_t count = 240*400;
		  *FMC_REG = 0x2C;//Memory Write (2Ch)
		  for (uint32_t i = 0; i < count; ++i) {
			  *FMC_DATA = (uint8_t)(color>>8);
			  *FMC_DATA = (uint8_t)color;
		  }
}



#define LCD_WIDTH 400
#define LCD_HEIGHT 240
#define WINDOW_SHIFT 256


uint16_t POINT_COLOR = 0xFFFF;
uint16_t BACK_COLOR = 0x0000;

void Putchar8x16(uint16_t x,uint16_t y,uint8_t num,uint8_t mode)
{
  uint8_t temp;
  uint8_t pos,t;
  uint16_t colortemp = POINT_COLOR;

  if(x > (LCD_WIDTH - 8) || y > LCD_HEIGHT - 16) return;
  num=num-' ';

  if(!mode)
  {
    for(pos = 0; pos < 16; pos++)
    {
      temp = asc2_1608[(uint16_t)num*16 + pos];
      for(t = 0; t < 8; t++)
      {
        if(temp&0x01)DrawPixel(y+pos,x+t,POINT_COLOR);
        else DrawPixel(y+pos,x+t,BACK_COLOR);
        temp>>=1;
      }
    }
  }
  else
  {
    for(pos=0;pos<16;pos++)
    {
      temp=asc2_1608[(uint16_t)num*16+pos];
      for(t=0;t<8;t++)
      {
        if(temp&0x01)DrawPixel(y+pos,x+t,POINT_COLOR);
        temp>>=1;
      }
    }
  }

  POINT_COLOR=colortemp;
}

void Putchar14x24(uint16_t x,uint16_t y,uint8_t data,uint8_t mode)
{
  if((x>LCD_WIDTH-14)||(y>LCD_HEIGHT-24)) return;
  uint8_t i,j,k,temp;

  for(i=0;i<24/8;i++)
  {
    for(j=0;j<8;j++)
    {
      for(k=0;k<14;k++)
      {
        temp=Consolas14x24[(data-' ')*(24/8)*14+k*(24/8)+i];
        if(mode==TFT_STRING_MODE_BACKGROUND)
        {
          if(temp&(0x01<<j))
          {
          	DrawPixel(y+(8*i+j),x+k,POINT_COLOR);
          }
          else
          {
          	DrawPixel(y+(8*i+j),x+k,BACK_COLOR);
          }
        }
        else
        {
          if(temp&(0x01<<j))
          {
          	DrawPixel(y+(8*i+j),x+k,POINT_COLOR);
          }
        }
      }
    }
  }
}

void Putchar18x32(uint16_t x,uint16_t y,uint8_t data,uint8_t mode)
{
  if((x>LCD_WIDTH-18)||(y>LCD_HEIGHT-32)) return;
  uint8_t i,j,k,temp;

  for(i=0;i<32/8;i++)
  {
    for(j=0;j<8;j++)
    {
      for(k=0;k<18;k++)
      {
        temp=Consolas18x32[(data-' ')*(32/8)*18+k*(32/8)+i];
        if(mode==TFT_STRING_MODE_BACKGROUND)
        {
          if(temp&(0x01<<j))
          {
          	DrawPixel(y+(8*i+j),x+k,POINT_COLOR);
          }
          else
          {
          	DrawPixel(y+(8*i+j),x+k,BACK_COLOR);
          }
        }
        else
        {
          if(temp&(0x01<<j))
          {
          	DrawPixel(y+(8*i+j),x+k,POINT_COLOR);
          }
        }
      }
    }
  }
}

void Putchar26x48(uint16_t x,uint16_t y,uint8_t data,uint8_t mode)
{
  if((x>LCD_WIDTH-26)||(y>LCD_HEIGHT-48)) return;
  uint8_t i,j,k,temp;

  for(i=0;i<48/8;i++)
  {
    for(j=0;j<8;j++)
    {
      for(k=0;k<26;k++)
      {
        temp=Consolas26x48[(data-' ')*(48/8)*26+k*(48/8)+i];
        if(mode==TFT_STRING_MODE_BACKGROUND)
        {
          if(temp&(0x01<<j))
          {
          	DrawPixel(y+(8*i+j),x+k,POINT_COLOR);
          }
          else
          {
          	DrawPixel(y+(8*i+j),x+k,BACK_COLOR);
          }
        }
        else
        {
          if(temp&(0x01<<j))
          {
          	DrawPixel(y+(8*i+j),x+k,POINT_COLOR);
          }
        }
      }
    }
  }
}


void Puts8x16(uint16_t x, uint16_t y, uint8_t *string, uint8_t TFT_STRING_MODE)
{
	x += WINDOW_SHIFT;
  uint8_t i = 0;
  uint8_t font_w = 8;
  uint8_t font_h = 16;

  while(*(string + i) != '\0')
  {
    if(*(string + i) == '\n')
    {
      y += font_h;
      x = WINDOW_SHIFT;
      string++;
    }

    if(x > LCD_WIDTH - font_w)
    {
      x = WINDOW_SHIFT;
      y += font_h;
    }

    if(y > LCD_HEIGHT - font_h)
    	break;

    Putchar8x16(x, y, *(string+i), TFT_STRING_MODE);
    x += font_w;
    i++;
  }
}

void Puts14x24(uint16_t x, uint16_t y, uint8_t *string,uint8_t TFT_STRING_MODE)
{
	x += WINDOW_SHIFT;
  uint8_t i = 0;
  uint8_t font_w = 14;
  uint8_t font_h = 24;

  while(*(string + i) != '\0')
  {
    if(*(string + i) == '\n')
    {
      y += font_h;
      x = WINDOW_SHIFT;
      string++;
    }

    if(x > LCD_WIDTH - font_w)
    {
      x = WINDOW_SHIFT;
      y += font_h;
    }

    if(y > LCD_HEIGHT - font_h)
    	break;

    Putchar14x24(x, y, *(string+i), TFT_STRING_MODE);
    x += font_w;
    i++;
  }
}

void Puts18x32(uint16_t x, uint16_t y, uint8_t *string, uint8_t TFT_STRING_MODE)
{
	x+= WINDOW_SHIFT;
  uint8_t i=0;
  uint8_t font_w=18;
  uint8_t font_h=32;

  while(*(string+i)!='\0')
  {
    if(*(string+i)=='\n')
    {
      y += font_h;
      x = WINDOW_SHIFT;
      string++;
    }

    if(x > LCD_WIDTH-font_w)
    {
      x = WINDOW_SHIFT;
      y += font_h;
    }

    if(y > LCD_HEIGHT-font_h)
    {
      x = WINDOW_SHIFT;
      y = 0;
    }

    Putchar18x32(x, y, *(string+i),TFT_STRING_MODE);
    x += font_w;
    i++;
  }
}

void Puts26x48(uint16_t x, uint16_t y, uint8_t *string, uint8_t TFT_STRING_MODE)
{
	x += WINDOW_SHIFT;
  uint8_t i = 0;
  uint8_t font_w = 26;
  uint8_t font_h = 48;

  while(*(string + i) != '\0')
  {
    if(*(string + i) == '\n')
    {
      y += font_h;
      x = WINDOW_SHIFT;
      string++;
    }

    if(x > LCD_WIDTH - font_w)
    {
      x = WINDOW_SHIFT;
      y += font_h;
    }

    if(y > LCD_HEIGHT - font_h)
    	break;

    Putchar26x48(x, y, *(string+i), TFT_STRING_MODE);
    x += font_w;
    i++;
  }
}
