/*
 * ILI9341.c
 *
 *  Created on: Jun 25, 2025
 *      Author: Semi Eren Gökyıldız
 */
//-------------------------------------------------------------------
#include <ILI9341.h>
//-------------------------------------------------------------------
#define swap(a,b) {int16_t t=a;a=b;b=t;}
//-------------------------------------------------------------------
void Reset_Active(ILI9341_HandleTypeDef* hili9341)
{
	HAL_GPIO_WritePin(hili9341->RST.port, hili9341->RST.pin, GPIO_PIN_RESET);
}

void Reset_Idle(ILI9341_HandleTypeDef* hili9341)
{
	HAL_GPIO_WritePin(hili9341->RST.port, hili9341->RST.pin, GPIO_PIN_SET);
}

void CS_Active(ILI9341_HandleTypeDef* hili9341)
{
	HAL_GPIO_WritePin(hili9341->CS.port, hili9341->CS.pin, GPIO_PIN_RESET);
}

void CS_Idle(ILI9341_HandleTypeDef* hili9341)
{
	HAL_GPIO_WritePin(hili9341->CS.port, hili9341->CS.pin, GPIO_PIN_SET);
}

void DC_Command_Mode(ILI9341_HandleTypeDef* hili9341)
{
	HAL_GPIO_WritePin(hili9341->DC.port, hili9341->DC.pin, GPIO_PIN_RESET);
}

void DC_Data_Mode(ILI9341_HandleTypeDef* hili9341)
{
	HAL_GPIO_WritePin(hili9341->DC.port, hili9341->DC.pin, GPIO_PIN_SET);
}

void ILI9341_SendCommand(ILI9341_HandleTypeDef* hili9341, uint8_t cmd)
{
	DC_Command_Mode(hili9341);
	HAL_SPI_Transmit (hili9341->hspi, &cmd, 1, 5000);
}

void ILI9341_SendData(ILI9341_HandleTypeDef* hili9341, uint8_t dt)
{
	DC_Data_Mode(hili9341);
	HAL_SPI_Transmit (hili9341->hspi, &dt, 1, 5000);
}

void ILI9341_Pins_Init(ILI9341_HandleTypeDef* hili9341)
{
	GPIO_InitTypeDef gpio_Initdef;
	gpio_Initdef.Mode=GPIO_MODE_OUTPUT_PP;
	gpio_Initdef.Pull=GPIO_NOPULL;
	gpio_Initdef.Speed=GPIO_SPEED_FREQ_HIGH;

	gpio_Initdef.Pin = hili9341->CS.pin;
	HAL_GPIO_Init(hili9341->CS.port, &gpio_Initdef);

	gpio_Initdef.Pin = hili9341->RST.pin;
	HAL_GPIO_Init(hili9341->RST.port, &gpio_Initdef);

	gpio_Initdef.Pin = hili9341->DC.pin;
	HAL_GPIO_Init(hili9341->DC.port, &gpio_Initdef);
}

void ILI9341_WriteData(ILI9341_HandleTypeDef* hili9341, uint8_t* buff, size_t buff_size) {
	DC_Data_Mode(hili9341);
	while(buff_size > 0) {
		uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
		HAL_SPI_Transmit(hili9341->hspi, buff, chunk_size, HAL_MAX_DELAY);
		buff += chunk_size;
		buff_size -= chunk_size;
	}
}

void ILI9341_Reset(ILI9341_HandleTypeDef* hili9341)
{
	Reset_Active(hili9341);
	HAL_Delay(5);
	Reset_Idle(hili9341);
}

void ILI9341_SetAddrWindow(ILI9341_HandleTypeDef* hili9341, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  // column address set
	ILI9341_SendCommand(hili9341, 0x2A); // CASET
  {
    uint8_t data[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
    ILI9341_WriteData(hili9341, data, sizeof(data));
  }

  // row address set
  ILI9341_SendCommand(hili9341, 0x2B); // RASET
  {
    uint8_t data[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };
    ILI9341_WriteData(hili9341, data, sizeof(data));
  }

  // write to RAM
  ILI9341_SendCommand(hili9341, 0x2C); // RAMWR
}

void ILI9341_FillRect(ILI9341_HandleTypeDef* hili9341, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  if((x1 >= hili9341->w_size) || (y1 >= hili9341->h_size) || (x2 >= hili9341->w_size) || (y2 >= hili9341->h_size)) return;
	if(x1>x2) swap(x1,x2);
	if(y1>y2) swap(y1,y2);
  ILI9341_SetAddrWindow(hili9341, x1, y1, x2, y2);
  uint8_t data[] = { color >> 8, color & 0xFF };
  DC_Data_Mode(hili9341);
  for(uint32_t i = 0; i < (x2-x1+1)*(y2-y1+1); i++)
  {
      HAL_SPI_Transmit(hili9341->hspi, data, 2, HAL_MAX_DELAY);
  }
}

void ILI9341_FillScreen(ILI9341_HandleTypeDef* hili9341, uint16_t color)
{
	ILI9341_FillRect(hili9341, 0, 0, hili9341->w_size-1, hili9341->h_size-1, color);
}

void ILI9341_DrawPixel(ILI9341_HandleTypeDef* hili9341, int x, int y, uint16_t color)
{
	if((x<0)||(y<0)||(x>=hili9341->w_size)||(y>=hili9341->h_size)) return;
	ILI9341_SetAddrWindow(hili9341, x,y,x,y);
	ILI9341_SendCommand(hili9341, 0x2C);
	ILI9341_SendData(hili9341, color>>8);
	ILI9341_SendData(hili9341, color & 0xFF);
}

void ILI9341_DrawLine(ILI9341_HandleTypeDef* hili9341,
		uint16_t color,
		uint16_t x1, uint16_t y1,
		uint16_t x2, uint16_t y2)
{
  int steep = abs(y2-y1)>abs(x2-x1);
  if(steep)
  {
    swap(x1,y1);
    swap(x2,y2);
  }
  if(x1>x2)
  {
    swap(x1,x2);
    swap(y1,y2);
  }
  int dx,dy;
  dx=x2-x1;
  dy=abs(y2-y1);
  int err=dx/2;
  int ystep;
  if(y1<y2) ystep=1;
  else ystep=-1;
  for(;x1<=x2;x1++)
  {
    if(steep) ILI9341_DrawPixel(hili9341, y1,x1,color);
    else ILI9341_DrawPixel(hili9341, x1,y1,color);
    err-=dy;
    if(err<0)
    {
      y1 += ystep;
      err=dx;
    }
  }
}

void ILI9341_DrawRect(ILI9341_HandleTypeDef* hili9341,
		uint16_t color,
		uint16_t x1, uint16_t y1,
		uint16_t x2, uint16_t y2)
{
	ILI9341_DrawLine(hili9341, color,x1,y1,x2,y1);
	ILI9341_DrawLine(hili9341, color,x2,y1,x2,y2);
	ILI9341_DrawLine(hili9341, color,x1,y1,x1,y2);
	ILI9341_DrawLine(hili9341, color,x1,y2,x2,y2);
}

void ILI9341_DrawCircle(ILI9341_HandleTypeDef* hili9341, uint16_t x0, uint16_t y0, int r, uint16_t color)
{
	int f = 1-r;
	int ddF_x=1;
	int ddF_y=-2*r;
	int x = 0;
	int y = r;
	ILI9341_DrawPixel(hili9341, x0,y0+r,color);
	ILI9341_DrawPixel(hili9341, x0,y0-r,color);
	ILI9341_DrawPixel(hili9341, x0+r,y0,color);
	ILI9341_DrawPixel(hili9341, x0-r,y0,color);
	while (x<y)
	{
		if (f>=0)
		{
			y--;
			ddF_y+=2;
			f+=ddF_y;
		}
		x++;
		ddF_x+=2;
		f+=ddF_x;
		ILI9341_DrawPixel(hili9341, x0+x,y0+y,color);
		ILI9341_DrawPixel(hili9341, x0-x,y0+y,color);
		ILI9341_DrawPixel(hili9341, x0+x,y0-y,color);
		ILI9341_DrawPixel(hili9341, x0-x,y0-y,color);
		ILI9341_DrawPixel(hili9341, x0+y,y0+x,color);
		ILI9341_DrawPixel(hili9341, x0-y,y0+x,color);
		ILI9341_DrawPixel(hili9341, x0+y,y0-x,color);
		ILI9341_DrawPixel(hili9341, x0-y,y0-x,color);
	}
}



uint16_t ILI9341_RandColor(ILI9341_HandleTypeDef* hili9341)
{
	return 565&0x0000FFFF;
}

void ILI9341_SetTextColor(ILI9341_HandleTypeDef* hili9341, uint16_t color)
{
	hili9341->lcdprop.TextColor=color;
}

void ILI9341_SetBackColor(ILI9341_HandleTypeDef* hili9341,uint16_t color)
{
	hili9341->lcdprop.BackColor=color;
}

void ILI9341_SetFont(ILI9341_HandleTypeDef* hili9341, sFONT *pFonts)
{
	hili9341->lcdprop.pFont=pFonts;
}

void ILI9341_DrawChar(ILI9341_HandleTypeDef* hili9341, uint16_t x, uint16_t y, uint8_t c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t offset;
  uint8_t *c_t;
  uint8_t *pchar;
  uint32_t line=0;
  height = hili9341->lcdprop.pFont->Height;
  width  = hili9341->lcdprop.pFont->Width;
  offset = 8 *((width + 7)/8) -  width ;
  c_t = (uint8_t*) &(hili9341->lcdprop.pFont->table[(c-' ') * hili9341->lcdprop.pFont->Height * ((hili9341->lcdprop.pFont->Width + 7) / 8)]);
  for(i = 0; i < height; i++)
  {
    pchar = ((uint8_t *)c_t + (width + 7)/8 * i);
    switch(((width + 7)/8))
    {
      case 1:
          line =  pchar[0];
          break;
      case 2:
          line =  (pchar[0]<< 8) | pchar[1];
          break;
      case 3:
      default:
        line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];
        break;
    }
    for (j = 0; j < width; j++)
    {
      if(line & (1 << (width- j + offset- 1)))
      {
        ILI9341_DrawPixel(hili9341, (x + j), y, hili9341->lcdprop.TextColor);
      }
      else
      {
        ILI9341_DrawPixel(hili9341, (x + j), y, hili9341->lcdprop.BackColor);
      }
    }
    y++;
  }
}

void ILI9341_String(ILI9341_HandleTypeDef* hili9341, uint16_t x,uint16_t y, char *str)
{
  while(*str)
  {
    ILI9341_DrawChar(hili9341, x,y,str[0]);
    x+=hili9341->lcdprop.pFont->Width;
    (void)*str++;
  }
}

void ILI9341_SetRotation(ILI9341_HandleTypeDef* hili9341, uint8_t r)
{
  ILI9341_SendCommand(hili9341, 0x36);
  switch(r)
  {
    case 0:
    	ILI9341_SendData(hili9341, 0x48);
    	hili9341->w_size = 240;
    	hili9341->h_size = 320;
      break;
    case 1:
    	ILI9341_SendData(hili9341, 0x28);
    	hili9341->w_size = 320;
    	hili9341->h_size = 240;
      break;
    case 2:
    	ILI9341_SendData(hili9341, 0x88);
    	hili9341->w_size = 240;
    	hili9341->h_size = 320;
      break;
    case 3:
    	ILI9341_SendData(hili9341, 0xE8);
    	hili9341->w_size = 320;
    	hili9341->h_size = 240;
      break;
  }
}

void ILI9341_FontsInit(ILI9341_HandleTypeDef* hili9341)
{
  Font8.Height = 8;
  Font8.Width = 5;
  Font12.Height = 12;
  Font12.Width = 7;
  Font16.Height = 16;
  Font16.Width = 11;
  Font20.Height = 20;
  Font20.Width = 14;
  Font24.Height = 24;
  Font24.Width = 17;
  hili9341->lcdprop.BackColor=ILI9341_BLACK;
  hili9341->lcdprop.TextColor=ILI9341_GREEN;
  hili9341->lcdprop.pFont=&Font16;
}

void ILI9341_Printf(ILI9341_HandleTypeDef* hili9341, uint16_t x, uint16_t y, const char *fmt, ...)
{
    char buffer[100];  // Yazı için geçici buffer
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);  // Formatlı string oluştur
    va_end(args);


    // Formatlanmış yazıyı LCD'ye yaz
    ILI9341_String(hili9341, x, y, buffer);
}


void ILI9341_init(ILI9341_HandleTypeDef* hili9341)
{
	uint8_t data[15];

	if(hili9341->w_size==0)hili9341->w_size=ILI9341_WIDTH;
	if(hili9341->h_size==0)hili9341->h_size=ILI9341_HEIGHT;

	ILI9341_Pins_Init(hili9341);

	CS_Active(hili9341);
	ILI9341_Reset(hili9341);
	//Software Reset
	ILI9341_SendCommand(hili9341, 0x01);
	HAL_Delay(1000);
	//Power Control A
	data[0] = 0x39;
	data[1] = 0x2C;
	data[2] = 0x00;
	data[3] = 0x34;
	data[4] = 0x02;
	ILI9341_SendCommand(hili9341, 0xCB);
	ILI9341_WriteData(hili9341, data, 5);
	//Power Control B
	data[0] = 0x00;
	data[1] = 0xC1;
	data[2] = 0x30;
	ILI9341_SendCommand(hili9341, 0xCF);
	ILI9341_WriteData(hili9341, data, 3);
	//Driver timing control A
	data[0] = 0x85;
	data[1] = 0x00;
	data[2] = 0x78;
	ILI9341_SendCommand(hili9341, 0xE8);
	ILI9341_WriteData(hili9341, data, 3);
	//Driver timing control B
	data[0] = 0x00;
	data[1] = 0x00;
	ILI9341_SendCommand(hili9341, 0xEA);
	ILI9341_WriteData(hili9341, data, 2);
	//Power on Sequence control
	data[0] = 0x64;
	data[1] = 0x03;
	data[2] = 0x12;
	data[3] = 0x81;
	ILI9341_SendCommand(hili9341, 0xED);
	ILI9341_WriteData(hili9341, data, 4);
	//Pump ratio control
	data[0] = 0x20;
	ILI9341_SendCommand(hili9341, 0xF7);
	ILI9341_WriteData(hili9341, data, 1);
	//Power Control,VRH[5:0]
	data[0] = 0x10;
	ILI9341_SendCommand(hili9341, 0xC0);
	ILI9341_WriteData(hili9341, data, 1);
	//Power Control,SAP[2:0];BT[3:0]
	data[0] = 0x10;
	ILI9341_SendCommand(hili9341, 0xC1);
	ILI9341_WriteData(hili9341, data, 1);
	//VCOM Control 1
	data[0] = 0x3E;
	data[1] = 0x28;
	ILI9341_SendCommand(hili9341, 0xC5);
	ILI9341_WriteData(hili9341, data, 2);
	//VCOM Control 2
	data[0] = 0x86;
	ILI9341_SendCommand(hili9341, 0xC7);
	ILI9341_WriteData(hili9341, data, 1);
	//Memory Acsess Control
	data[0] = 0x48;
	ILI9341_SendCommand(hili9341, 0x36);
	ILI9341_WriteData(hili9341, data, 1);
	//Pixel Format Set
	data[0] = 0x55;//16bit
	ILI9341_SendCommand(hili9341, 0x3A);
	ILI9341_WriteData(hili9341, data, 1);
	//Frame Rratio Control, Standard RGB Color
	data[0] = 0x00;
	data[1] = 0x18;
	ILI9341_SendCommand(hili9341, 0xB1);
	ILI9341_WriteData(hili9341, data, 2);
	//Display Function Control
	data[0] = 0x08;
	data[1] = 0x82;
	data[2] = 0x27;//320 строк
	ILI9341_SendCommand(hili9341, 0xB6);
	ILI9341_WriteData(hili9341, data, 3);
	//Enable 3G (пока не знаю что это за режим)
	data[0] = 0x00;//не включаем
	ILI9341_SendCommand(hili9341, 0xF2);
	ILI9341_WriteData(hili9341, data, 1);
	//Gamma set
	data[0] = 0x01;//Gamma Curve (G2.2) (Кривая цветовой гаммы)
	ILI9341_SendCommand(hili9341, 0x26);
	ILI9341_WriteData(hili9341, data, 1);
	//Positive Gamma  Correction
	data[0] = 0x0F;
	data[1] = 0x31;
	data[2] = 0x2B;
	data[3] = 0x0C;
	data[4] = 0x0E;
	data[5] = 0x08;
	data[6] = 0x4E;
	data[7] = 0xF1;
	data[8] = 0x37;
	data[9] = 0x07;
	data[10] = 0x10;
	data[11] = 0x03;
	data[12] = 0x0E;
	data[13] = 0x09;
	data[14] = 0x00;
	ILI9341_SendCommand(hili9341, 0xE0);
	ILI9341_WriteData(hili9341, data, 15);
	//Negative Gamma  Correction
	data[0] = 0x00;
	data[1] = 0x0E;
	data[2] = 0x14;
	data[3] = 0x03;
	data[4] = 0x11;
	data[5] = 0x07;
	data[6] = 0x31;
	data[7] = 0xC1;
	data[8] = 0x48;
	data[9] = 0x08;
	data[10] = 0x0F;
	data[11] = 0x0C;
	data[12] = 0x31;
	data[13] = 0x36;
	data[14] = 0x0F;
	ILI9341_SendCommand(hili9341, 0xE1);
	ILI9341_WriteData(hili9341, data, 15);
	ILI9341_SendCommand(hili9341, 0x11);//Выйдем из спящего режима
	HAL_Delay(120);
	//Display ON
	data[0] = ILI9341_ROTATION;
	ILI9341_SendCommand(hili9341, 0x29);
	ILI9341_WriteData(hili9341, data, 1);

	ILI9341_FontsInit(hili9341);
}
