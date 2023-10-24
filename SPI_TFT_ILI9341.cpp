/* mbed library for 240*320 pixel display TFT based on ILI9341 LCD Controller
 * Copyright (c) 2013 Peter Drescher - DC2PD
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
// 12.06.13 fork from SPI_TFT code because controller is different ...
// 14.07.13 Test with real display and bugfix 
// 18.10.13 Better Circle function from Michael Ammann
// 22.10.13 Fixes for Kinetis Board - 8 bit spi

#include "SPI_TFT_ILI9341.h"
#include "mbed.h"


//#ifdef PARA_TFT      
    SPI_TFT_ILI9341::SPI_TFT_ILI9341(PinName TFT_D0, PinName TFT_D1, PinName TFT_D2, PinName TFT_D3,
    PinName TFT_D4, PinName TFT_D5, PinName TFT_D6, PinName TFT_D7, PinName rd, PinName wr,
    PinName cs, PinName dc, PinName res, const char *name) : GraphicsDisplay(name), _cs(cs), _dc(dc),
    _TFT_D0(TFT_D0),
    _TFT_D1(TFT_D1),
    _TFT_D2(TFT_D2),
    _TFT_D3(TFT_D3),
    _TFT_D4(TFT_D4),
    _TFT_D5(TFT_D5),
    _TFT_D6(TFT_D6),
    _TFT_D7(TFT_D7),
    _rd(rd),
    _wr(wr),
    _res(res),
    _spi(D11, D12, D13)
{
    iTypeTFT = PARA_TFT_;

    if(strcmp(name, "PARA") == 0)
    {
        iTypeTFT = PARA_TFT_; 
    }

    orientation = 1;
    char_x = 0;
    char_x = 0;

    _TFT_D0 = 1;
    _TFT_D1 = 1;
    _TFT_D2 = 1;
    _TFT_D3 = 1;
    _TFT_D4 = 1;
    _TFT_D5 = 1;
    _TFT_D6 = 1;
    _TFT_D7 = 1;  

    _rd = 1;
    _wr = 1; 
    _cs = 1;
    _dc = 1;      
    _res = 1; 

    tft_reset(0);
}

//#else
SPI_TFT_ILI9341::SPI_TFT_ILI9341(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName reset, PinName dc, int spiSpeed, const char *name)
    : GraphicsDisplay(name), _spi(mosi, miso, sclk), _cs(cs), _dc(dc),
    _TFT_D0(NC),
    _TFT_D1(NC),
    _TFT_D2(NC),
    _TFT_D3(NC),
    _TFT_D4(NC),
    _TFT_D5(NC),
    _TFT_D6(NC),
    _TFT_D7(NC),
    _rd(NC),
    _wr(NC),
    _res(NC)
{
    if(strcmp(name, "SEEED") == 0)
    {
        iTypeTFT = SEEED_TFT_; 
    }

    if(strcmp(name, "ADA") == 0)
    {
        iTypeTFT = ADA_TFT_; 
    }

    orientation = 0;
    char_x = 0;
    char_x = 0;

    _cs = 1;
    _dc = 1;

    _reset = reset;
    tft_reset(spiSpeed);
}
//#endif


int SPI_TFT_ILI9341::width()
{
    if (orientation == 0 || orientation == 2) return 240;
    else return 320;
}


int SPI_TFT_ILI9341::height()
{
    if (orientation == 0 || orientation == 2) return 320;
    else return 240;
}


void SPI_TFT_ILI9341::set_orientation(unsigned int o)
{
    orientation = o;
    wr_cmd(0x36);                               // memory acces control             
    switch (orientation) 
    {
        case 0:
            wr_dat(0x48);
            break;
        case 1:
            wr_dat(0x28);
            break;
        case 2:
            wr_dat(0x88);
            break;
        case 3:
            wr_dat(0xE8);
            break;
    }
    _cs = 1; 
    WindowMax();
} 


void SPI_TFT_ILI9341::wr_cmd(unsigned char cmd)         // write command to tft register
{
    _dc = 0;
    _cs = 0;
    wr_dat(cmd);   
    _dc = 1;
}


void SPI_TFT_ILI9341::wr_dat(unsigned char dat)
{
    if (iTypeTFT == PARA_TFT_) //PARA_TFT
    {
        if (dat & 0x01)
            _TFT_D0 = 1;
        else
            _TFT_D0 = 0;  

        if (dat & 0x02)
            _TFT_D1 = 1;
        else
            _TFT_D1 = 0;

        if (dat & 0x04)
            _TFT_D2 = 1;
        else
            _TFT_D2 = 0;

        if (dat & 0x08)
            _TFT_D3 = 1;
        else
            _TFT_D3 = 0;

        if (dat & 0x10)
            _TFT_D4 = 1;
        else
            _TFT_D4 = 0;

        if (dat & 0x20)
            _TFT_D5 = 1;
        else
            _TFT_D5 = 0; 

        if (dat & 0x40)
            _TFT_D6 = 1;
        else
            _TFT_D6 = 0;

        if (dat & 0x80)
            _TFT_D7 = 1;
        else
            _TFT_D7 = 0;

        _wr = 0;
        _wr = 1;
    }
    else
    {
        _spi.write(dat);                            // mbed lib
    }

}


void SPI_TFT_ILI9341::tft_reset(int spiSpeed)       // Init TFT
{
    if (iTypeTFT == PARA_TFT_) //PARA_TFT
    {  
        _res = 0;                           // display hw reset
        wait_us(50);                        //
        _res = 1;                           //
    }
    else
    {
        _spi.format(8,0);                   // 8 bit spi mode 0
        _spi.frequency(spiSpeed);           // SPI clock    
        if (_reset != NC)
        {
            DigitalOut _res(_reset);
            _res = 0;                       // display hw reset
            wait_us(50);
            _res = 1;                       //
        }
    }

    ThisThread::sleep_for(5ms); 
    wr_cmd(0x01);                           // SW reset 
    ThisThread::sleep_for(5ms);
    wr_cmd(0x28);                           // display off  

    /* Start Initial Sequence ----------------------------------------------------*/
     wr_cmd(0xCF);                     
     wr_dat(0x00);
     wr_dat(0x83);
     wr_dat(0x30);
     _cs = 1;
     
     wr_cmd(0xED);                     
     wr_dat(0x64);
     wr_dat(0x03);
     wr_dat(0x12);
     wr_dat(0x81);
     _cs = 1;
     
     wr_cmd(0xE8);                     
     wr_dat(0x85);
     wr_dat(0x01);
     wr_dat(0x79);
     _cs = 1;
     
     wr_cmd(0xCB);                     
     wr_dat(0x39);
     wr_dat(0x2C);
     wr_dat(0x00);
     wr_dat(0x34);
     wr_dat(0x02);
     _cs = 1;
           
     wr_cmd(0xF7);                     
     wr_dat(0x20);
     _cs = 1;
           
     wr_cmd(0xEA);                     
     wr_dat(0x00);
     wr_dat(0x00);
     _cs = 1;
     
     wr_cmd(0xC0);                     // POWER_CONTROL_1
     wr_dat(0x26);
     _cs = 1;
 
     wr_cmd(0xC1);                     // POWER_CONTROL_2
     wr_dat(0x11);
     _cs = 1;
     
     wr_cmd(0xC5);                     // VCOM_CONTROL_1
     wr_dat(0x35);
     wr_dat(0x3E);
     _cs = 1;
     
     wr_cmd(0xC7);                     // VCOM_CONTROL_2
     wr_dat(0xBE);
     _cs = 1; 
     
     wr_cmd(0x36);                     // MEMORY_ACCESS_CONTROL
     wr_dat(0x48);

     wr_dat(0xC8);

     _cs = 1; 
     
     wr_cmd(0x3A);                     // COLMOD_PIXEL_FORMAT_SET
     wr_dat(0x55);                      // 16 bit pixel 
     _cs = 1;
     
     wr_cmd(0xB1);                     // Frame Rate
     wr_dat(0x00);
     wr_dat(0x1B);               
     _cs = 1;
     
     wr_cmd(0xF2);                     // Gamma Function Disable
     wr_dat(0x08);
     _cs = 1; 
     
     wr_cmd(0x26);                     
     wr_dat(0x01);                 // gamma set for curve 01/2/04/08
     _cs = 1; 
     
     wr_cmd(0xE0);                     // positive gamma correction
     wr_dat(0x1F); 
     wr_dat(0x1A); 
     wr_dat(0x18); 
     wr_dat(0x0A); 
     wr_dat(0x0F); 
     wr_dat(0x06); 
     wr_dat(0x45); 
     wr_dat(0x87); 
     wr_dat(0x32); 
     wr_dat(0x0A); 
     wr_dat(0x07); 
     wr_dat(0x02); 
     wr_dat(0x07);
     wr_dat(0x05); 
     wr_dat(0x00);
     _cs = 1;
     
     wr_cmd(0xE1);                     // negativ gamma correction
     wr_dat(0x00); 
     wr_dat(0x25); 
     wr_dat(0x27); 
     wr_dat(0x05); 
     wr_dat(0x10); 
     wr_dat(0x09); 
     wr_dat(0x3A); 
     wr_dat(0x78); 
     wr_dat(0x4D); 
     wr_dat(0x05); 
     wr_dat(0x18); 
     wr_dat(0x0D); 
     wr_dat(0x38);
     wr_dat(0x3A); 
     wr_dat(0x1F);
     _cs = 1;
     
     WindowMax ();
     
     //wr_cmd(0x34);                     // tearing effect off
     //_cs = 1;
     
     //wr_cmd(0x35);                     // tearing effect on
     //_cs = 1;
      
     wr_cmd(0xB7);                       // entry mode
     wr_dat(0x07);
     _cs = 1;
     
     wr_cmd(0xB6);                       // display function control
     wr_dat(0x0A);
     wr_dat(0x82);
     wr_dat(0x27);
     wr_dat(0x00);
     _cs = 1;
     
     wr_cmd(0x11);                     // sleep out
     _cs = 1;
     
    ThisThread::sleep_for(100ms);
           
    wr_cmd(0x29);                     // display on
    _cs = 1;
     
    ThisThread::sleep_for(100ms);
 }


void SPI_TFT_ILI9341::pixel(int x, int y, int color)
{
    wr_cmd(0x2A);
    wr_dat(x >> 8);
    wr_dat(x);
    _cs = 1;

    wr_cmd(0x2B);
    wr_dat(y >> 8);
    wr_dat(y);
    _cs = 1;

    wr_cmd(0x2C);                                   // send pixel

    if (iTypeTFT == PARA_TFT_) //PARA_TFT
    {  
        wr_dat(color >> 8);
        wr_dat(color & 0xff);
    
    }
    else
    {                                       // 16 bit SPI transfer
        _spi.format(16,0);                              
        _spi.write(color);                              
        _spi.format(8,0);                          
    }

    _cs = 1;
}


void SPI_TFT_ILI9341::window (unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    wr_cmd(0x2A);
    wr_dat(x >> 8);
    wr_dat(x);
    wr_dat((x+w-1) >> 8);
    wr_dat(x+w-1);  
    _cs = 1;

    wr_cmd(0x2B);
    wr_dat(y >> 8);
    wr_dat(y);
    wr_dat((y+h-1) >> 8);
    wr_dat(y+h-1);
    _cs = 1;
}


void SPI_TFT_ILI9341::WindowMax (void)
{
    window (0, 0, width(),  height());
}


void SPI_TFT_ILI9341::cls (void)
{
    int pixel = ( width() * height());
    WindowMax();
    wr_cmd(0x2C);                                   // send pixel

    if (iTypeTFT == PARA_TFT_) //PARA_TFT
    {  
        unsigned int i;
        for (i = 0; i < ( width() * height()); i++)
        {
            wr_dat(_background >> 8);
            wr_dat(_background & 0xff);
        }
    }
    else   
    {                                       // 16 bit SPI transfer
        _spi.format(16,0);                          
        unsigned int i;
        for (i = 0; i < ( width() * height()); i++)
            _spi.write(_background);
        _spi.format(8,0);    
    } 

    _cs = 1; 
}


void SPI_TFT_ILI9341::circle(int x0, int y0, int r, int color)
{

    int x = -r, y = 0, err = 2-2*r, e2;
    do {
        pixel(x0-x, y0+y,color);
        pixel(x0+x, y0+y,color);
        pixel(x0+x, y0-y,color);
        pixel(x0-x, y0-y,color);
        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);
}

void SPI_TFT_ILI9341::fillcircle(int x0, int y0, int r, int color)
{
    int x = -r, y = 0, err = 2-2*r, e2;
    do {
        vline(x0-x, y0-y, y0+y, color);
        vline(x0+x, y0-y, y0+y, color);
        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);
}


void SPI_TFT_ILI9341::hline(int x0, int x1, int y, int color)
{
    int w;
    w = x1 - x0 + 1;
    window(x0,y,w,1);
    wr_cmd(0x2C);  // send pixel

    if (iTypeTFT == PARA_TFT_) //PARA_TFT
    { 
        int j;
        for (j=0; j<w; j++) 
        {
            wr_dat(color >> 8);
            wr_dat(color & 0xff);
        } 

    }
    else
    {                                          // 16 bit SPI transfer
        _spi.format(16,0);                            
        int j;
        for (j=0; j<w; j++) 
        {
            _spi.write(color);
        }
        _spi.format(8,0);
    } 
    _cs = 1;
    
    WindowMax();
    return;
}


void SPI_TFT_ILI9341::vline(int x, int y0, int y1, int color)
{
    int h;
    h = y1 - y0 + 1;
    window(x,y0,1,h);
    wr_cmd(0x2C);  // send pixel
    
    if (iTypeTFT == PARA_TFT_) //PARA_TFT
    { 
        for (int y=0; y<h; y++) 
        {
            wr_dat(color >> 8);
            wr_dat(color & 0xff);
        } 
    }
    else
    {                                          // 16 bit SPI transfer
        _spi.format(16,0);                        
        for (int y=0; y<h; y++) 
        {
            _spi.write(color);
        }
        _spi.format(8,0);
    }

    _cs = 1;
    
    WindowMax();
    return;
}


void SPI_TFT_ILI9341::line(int x0, int y0, int x1, int y1, int color)
{
    int   dx = 0, dy = 0;
    int   dx_sym = 0, dy_sym = 0;
    int   dx_x2 = 0, dy_x2 = 0;
    int   di = 0;

    dx = x1-x0;
    dy = y1-y0;

    if (dx == 0)                                    // vertical line 
     {                                 
        if (y1 > y0) vline(x0,y0,y1,color);
        else vline(x0,y1,y0,color);
        return;
    }

    if (dx > 0) 
    {
        dx_sym = 1;
    } else {
        dx_sym = -1;
    }
    if (dy == 0)                                    // horizontal line 
    {       
        if (x1 > x0) hline(x0,x1,y0,color);
        else  hline(x1,x0,y0,color);
        return;
    }

    if (dy > 0) 
    {
        dy_sym = 1;
    } else 
    {
        dy_sym = -1;
    }

    dx = dx_sym*dx;
    dy = dy_sym*dy;

    dx_x2 = dx*2;
    dy_x2 = dy*2;

    if (dx >= dy) 
    {
        di = dy_x2 - dx;
        while (x0 != x1) 
        {

            pixel(x0, y0, color);
            x0 += dx_sym;
            if (di<0) 
            {
                di += dy_x2;
            } else 
            {
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }
        pixel(x0, y0, color);
    } 
    else 
    {
        di = dx_x2 - dy;
        while (y0 != y1) 
        {
            pixel(x0, y0, color);
            y0 += dy_sym;
            if (di < 0) 
            {
                di += dx_x2;
            } else 
            {
                di += dx_x2 - dy_x2;
                x0 += dx_sym;
            }
        }
        pixel(x0, y0, color);
    }
    return;
}


void SPI_TFT_ILI9341::rect(int x0, int y0, int x1, int y1, int color)
{

    if (x1 > x0) hline(x0,x1,y0,color);
    else  hline(x1,x0,y0,color);

    if (y1 > y0) vline(x0,y0,y1,color);
    else vline(x0,y1,y0,color);

    if (x1 > x0) hline(x0,x1,y1,color);
    else  hline(x1,x0,y1,color);

    if (y1 > y0) vline(x1,y0,y1,color);
    else vline(x1,y1,y0,color);

    return;
}


void SPI_TFT_ILI9341::fillrect(int x0, int y0, int x1, int y1, int color)
{

    int h = y1 - y0 + 1;
    int w = x1 - x0 + 1;
    int pixel = h * w;
    window(x0,y0,w,h);
    wr_cmd(0x2C);  // send pixel 
    if (iTypeTFT == PARA_TFT_) //PARA_TFT
    { 
        for (int p=0; p<pixel; p++) 
        {
            wr_dat(color >> 8);
            wr_dat(color & 0xff);
        }
   }
   else
   {                                           // 16 bit SPI transfer
        _spi.format(16,0);                      
        for (int p=0; p<pixel; p++)
        {
            _spi.write(color);
        }
        _spi.format(8,0);
    } 

    _cs = 1;
    WindowMax();
    return;
}


void SPI_TFT_ILI9341::locate(int x, int y)
{
    char_x = x;
    char_y = y;
}


int SPI_TFT_ILI9341::columns()
{
    return width() / font[1];
}


int SPI_TFT_ILI9341::rows()
{
    return height() / font[2];
}


int SPI_TFT_ILI9341::_putc(int value)
{
    if (value == '\n') {                                // new line
        char_x = 0;
        char_y = char_y + font[2];
        if (char_y >= height() - font[2]) 
        {
            char_y = 0;
        }
    }
    else
    {
        character(char_x, char_y, value);
    }
    return value;
}


void SPI_TFT_ILI9341::character(int x, int y, int c)
{
    unsigned int hor,vert,offset,bpl,j,i,b;
    unsigned char* zeichen;
    unsigned char z,w;

    if ((c < 31) || (c > 127)) return;   // test char range

    // read font parameter from start of array
    offset = font[0];                    // bytes / char
    hor = font[1];                       // get hor size of font
    vert = font[2];                      // get vert size of font
    bpl = font[3];                       // bytes per line

    if (char_x + hor > width())
    {
        char_x = 0;
        char_y = char_y + vert;
        if (char_y >= height() - font[2]) 
        {
            char_y = 0;
        }
    }
    window(char_x, char_y,hor,vert);                // char box
    wr_cmd(0x2C);                                   // send pixel

    if (iTypeTFT != PARA_TFT_) //#ifndef PARA_TFT
    {                                     // 16 Bit SPI transfer
        _spi.format(16,0);   
    }

    zeichen = &font[((c -32) * offset) + 4];        // start of char bitmap
    w = zeichen[0];                                 // width of actual char
     for (j=0; j<vert; j++)                         //  vert line
     {                       
        for (i=0; i<hor; i++)                       //  horz line
        {                     
            z =  zeichen[bpl * i + ((j & 0xF8) >> 3)+1];
            b = 1 << (j & 0x07);
            if (( z & b ) == 0x00) 
            {
                if (iTypeTFT != PARA_TFT_) //#ifndef PARA_TFT
                {                                  // 16 bit SPI transfer 
                    _spi.write(_background);
                }
                else
                {                               // 8 bit parallel transfer                
                    wr_dat(_background >> 8);
                    wr_dat(_background & 0xff);
                }
            } 
            else 
            {
                if (iTypeTFT != PARA_TFT_) //#ifndef PARA_TFT
                {
                    _spi.write(_foreground);
                }
                else
                {                               // 8 bit parallel transfer
                    wr_dat(_foreground >> 8);
                    wr_dat(_foreground & 0xff);
                }
            }
        }
    }
    _cs = 1;

    if (iTypeTFT != PARA_TFT_) //#ifndef PARA_TFT
    {         
        _spi.format(8,0);
    }

    WindowMax();
    if ((w + 2) < hor)                              // x offset to next char
    {                   
        char_x += w + 2;
    } 
    else
    { 
        char_x += hor;
    }
}


void SPI_TFT_ILI9341::set_font(unsigned char* f)
{
    font = f;
}


void SPI_TFT_ILI9341::Bitmap(unsigned int x, unsigned int y, unsigned int w, unsigned int h,unsigned char *bitmap)
{
    unsigned int  j;
    int padd;
    unsigned short *bitmap_ptr = (unsigned short *)bitmap;

    //#ifdef PARA_TFT                                 // 8 bit parallel transfer
        unsigned short pix_temp;
    //#endif
    
    unsigned int i;
    
    padd = -1;                                      // the lines are padded to multiple of 4 bytes in a bitmap
    do 
    {
        padd ++;
    } 
    while (2*(w + padd)%4 != 0);
    window(x, y, w, h);
    bitmap_ptr += ((h - 1)* (w + padd));
    wr_cmd(0x2C);                                   // send pixel

    if (iTypeTFT != PARA_TFT_) //#ifndef PARA_TFT
    {        
        _spi.format(16,0);
    }

    for (j = 0; j < h; j++)                         //Lines
    {         
        for (i = 0; i < w; i++)                     // one line
        {     
            if (iTypeTFT == PARA_TFT_) //PARA_TFT
            {                 
                pix_temp = *bitmap_ptr;
                wr_dat(pix_temp >> 8);
                wr_dat(pix_temp);
                bitmap_ptr++;
            }
            else
            {                                  // 16 bit SPI transfer
                _spi.write(*bitmap_ptr);            // one line
                bitmap_ptr++;
            }
        }
        bitmap_ptr -= 2*w;
        bitmap_ptr -= padd;
    }
    _cs = 1;

    if (iTypeTFT != PARA_TFT_) //#ifndef PARA_TFT
    {
        _spi.format(8,0);                           
    }

    WindowMax();
}

