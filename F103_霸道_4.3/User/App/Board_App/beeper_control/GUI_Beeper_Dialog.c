#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"

#define GUI_BEEPER_BACKGROUNG_PIC   "0:/beeper_desktop.jpg"
#define GUI_BEEPER_HORN_PIC         "0:/beeper_horn.png"
#define GUI_BEEPER_HORN1_PIC        "0:/beeper_horn1.png"
#define GUI_BEEPER_HORN2_PIC        "0:/beeper_horn2.png"
#define GUI_BEEPER_HORN3_PIC        "0:/beeper_horn3.png"
#define GUI_BEEPER_BUTTON_PIC       "0:/beeper_button.png"

typedef enum
{
  hdc_beeper_horn = 0,
	hdc_beeper_horn1,
	hdc_beeper_horn2,
	hdc_beeper_horn3,
	hdc_beeper_button,
  
	hdc_beeper_end,
}hdc_beep_png_t;

typedef struct{
	char *pic_name;      // ͼƬ��
	int w;               // ͼƬ��
	int h;               // ͼƬ��
	hdc_beep_png_t id;   // hdc ���
}beeper_hdc_t;

static HDC hdc_beeper_bk;
static HDC hdc_beeper_png[hdc_beeper_end];

const beeper_hdc_t beeper_png_info[hdc_beeper_end] = 
{
  {GUI_BEEPER_HORN_PIC,     59, 101,   hdc_beeper_horn},
  {GUI_BEEPER_HORN1_PIC,    13,  40,   hdc_beeper_horn1},
  {GUI_BEEPER_HORN2_PIC,    21,  79,   hdc_beeper_horn2},
  {GUI_BEEPER_HORN3_PIC,    29, 116,   hdc_beeper_horn3},
  {GUI_BEEPER_BUTTON_PIC,   30,  30,   hdc_beeper_button},
};

/* ��ť ID */
enum 
{
  ID_BEEPER_TICK = 0x1000,    // �������δ�ť
  ID_BEEPER_SW,               // ���������س�����ť
  ID_BEEPER_PRESS,            // ��������ס�찴ť
  ID_BEEPER_EXIT,             // �������˳���ť
};

static uint8_t sw_flag    = 0;
static uint8_t tick_flag  = 0;
static uint8_t press_flag = 3;

//�˳���ť�ػ���
static void ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;

  hdc = ds->hDC;   
  rc = ds->rc; 

  if (ds->State & BST_PUSHED)
  { //��ť�ǰ���״̬
    SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));      //��������ɫ
  }
  else
  { //��ť�ǵ���״̬
    SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
  }

  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 5;
  }
}

/*
 * @brief  ѡ��ť
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void sw_button_OwnerDraw(DRAWITEM_HDR *ds)
{
  HWND hwnd;
	HDC hdc;
  RECT rc, rc_tmp;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  rc = ds->rc;

	/* ���� */
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_beeper_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  rc.h = 83;
  if (ds->ID == ID_BEEPER_SW)
  {
    if (sw_flag == 0)
    {
      BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_beeper_png[hdc_beeper_button], 0, 0, SRCCOPY);
      InflateRect(&rc, 0, -6);
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
    }
    else 
    {
      rc.y = ds->rc.h - rc.h;
      BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_beeper_png[hdc_beeper_button], 0, 0, SRCCOPY);
      InflateRect(&rc, 0, -6);
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
    }
  }
  else if (ds->ID == ID_BEEPER_TICK)
  {
    if (tick_flag == 0)
    {
      BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_beeper_png[hdc_beeper_button], 0, 0, SRCCOPY);
      InflateRect(&rc, 0, -6);
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
    }
    else 
    {
      rc.y = ds->rc.h - rc.h;
      BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_beeper_png[hdc_beeper_button], 0, 0, SRCCOPY);
      InflateRect(&rc, 0, -6);
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
    }
  }
}

/*
 * @brief  �м䰴ס��ť
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void press_button_OwnerDraw(DRAWITEM_HDR *ds)
{
  HWND hwnd;
	HDC hdc;
  RECT rc, rc_tmp;

  hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  rc = ds->rc;

	/* ���� */
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_beeper_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  BitBlt(hdc, 54, 77, 103, 190, hdc_beeper_png[hdc_beeper_horn], 0, 0, SRCCOPY);
  
  switch (press_flag)
  {
    case 3:
      BitBlt(hdc, 200, 62, 74, 220, hdc_beeper_png[hdc_beeper_horn3], 0, 0, SRCCOPY);
    
    case 2:
      BitBlt(hdc, 187, 98, 53, 148, hdc_beeper_png[hdc_beeper_horn2], 0, 0, SRCCOPY);

    case 1:
      BitBlt(hdc, 174, 135, 33, 74, hdc_beeper_png[hdc_beeper_horn1], 0, 0, SRCCOPY);
  
  default:
    break;
  }
}

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  
  static uint8_t Load_Flag = 1;
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
            
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                  286, 2, 23, 23, hwnd, ID_BEEPER_EXIT, NULL, NULL); 

      CreateWindow(BUTTON, L"�δ�", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                  15, 95, 32, 77, hwnd, ID_BEEPER_TICK, NULL, NULL); 

      CreateWindow(BUTTON, L"��ס��", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                  69, 42, 182, 182, hwnd, ID_BEEPER_PRESS, NULL, NULL); 

      CreateWindow(BUTTON, L"����", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                  269, 95, 32, 77, hwnd, ID_BEEPER_SW, NULL, NULL); 
      
      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;
      BOOL res = NULL;

//      res = RES_Load_Content(GUI_BEEPER_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      res = FS_Load_Content(GUI_BEEPER_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      hdc_beeper_bk = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
      if(res)
      {
        /* ����ͼƬ���ݴ���JPG_DEC��� */
        dec = JPG_Open(jpeg_buf, jpeg_size);

        /* �������ڴ���� */
        JPG_Draw(hdc_beeper_bk, 0, 0, dec);

        /* �ر�JPG_DEC��� */
        JPG_Close(dec);
      }
      /* �ͷ�ͼƬ���ݿռ� */
      RES_Release_Content((char **)&jpeg_buf);

      u8 *pic_buf;
      u32 pic_size;
      PNG_DEC *png_dec;
      BITMAP png_bm;
      
      for (uint8_t xC=0; xC<hdc_beeper_end; xC++)
      {
        /* ���� HDC */
        hdc_beeper_png[beeper_png_info[xC].id] = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, beeper_png_info[xC].w, beeper_png_info[xC].h);
        ClrDisplay(hdc_beeper_png[beeper_png_info[xC].id], NULL, 0);
//          res = RES_Load_Content(clock_png_info[xC].pic_name, (char**)&pic_buf, &pic_size);
        res = FS_Load_Content(beeper_png_info[xC].pic_name, (char**)&pic_buf, &pic_size);
        if(res)
        {
          png_dec = PNG_Open(pic_buf, pic_size);
          PNG_GetBitmap(png_dec, &png_bm);
          DrawBitmap(hdc_beeper_png[beeper_png_info[xC].id], 0, 0, &png_bm, NULL);
          PNG_Close(png_dec);
        }
        /* �ͷ�ͼƬ���ݿռ� */
        RES_Release_Content((char **)&pic_buf);
      }
      
//      SetTimer(hwnd, 2, 10, TMR_START | TMR_SINGLE, NULL);
      SetTimer(hwnd, 5, 10, TMR_START | TMR_SINGLE, NULL);
      SetTimer(hwnd, 6, 1000, TMR_START | TMR_SINGLE, NULL);
      break;
    } 
    case WM_TIMER:
    {
      static uint8_t beep_flag = 0;

      uint16_t timer_id;
      timer_id = wParam;
      if(timer_id == 5)
      {
        BEEP_ON;
      }
      else if(timer_id == 6)
      {
        BEEP_OFF;
        KillTimer(hwnd,55);
        KillTimer(hwnd,66);
      }
      else if (timer_id == 1)
      {
        beep_flag = !beep_flag;

        if (beep_flag)
        {
          BEEP_ON;
        }
        else
        {
          BEEP_OFF;
        }

        press_flag++;
        press_flag %= 4;

        RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_PRESS), NULL, TRUE);
			} 
      else if (timer_id == 2)
      {
        u8 *jpeg_buf;
        u32 jpeg_size;
        JPG_DEC *dec;
        BOOL res = NULL;

        res = RES_Load_Content(GUI_BEEPER_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
//        res = FS_Load_Content(GUI_BEEPER_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
        hdc_beeper_bk = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
        if(res)
        {
          /* ����ͼƬ���ݴ���JPG_DEC��� */
          dec = JPG_Open(jpeg_buf, jpeg_size);

          /* �������ڴ���� */
          JPG_Draw(hdc_beeper_bk, 0, 0, dec);

          /* �ر�JPG_DEC��� */
          JPG_Close(dec);
        }
        /* �ͷ�ͼƬ���ݿռ� */
        RES_Release_Content((char **)&jpeg_buf);

        u8 *pic_buf;
        u32 pic_size;
        PNG_DEC *png_dec;
        BITMAP png_bm;
        
        for (uint8_t xC=0; xC<hdc_beeper_end; xC++)
        {
          /* ���� HDC */
          hdc_beeper_png[beeper_png_info[xC].id] = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, beeper_png_info[xC].w, beeper_png_info[xC].h);
          ClrDisplay(hdc_beeper_png[beeper_png_info[xC].id], NULL, 0);
//          res = RES_Load_Content(clock_png_info[xC].pic_name, (char**)&pic_buf, &pic_size);
          res = FS_Load_Content(beeper_png_info[xC].pic_name, (char**)&pic_buf, &pic_size);
          if(res)
          {
            png_dec = PNG_Open(pic_buf, pic_size);
            PNG_GetBitmap(png_dec, &png_bm);
            DrawBitmap(hdc_beeper_png[beeper_png_info[xC].id], 0, 0, &png_bm, NULL);
            PNG_Close(png_dec);
          }
          /* �ͷ�ͼƬ���ݿռ� */
          RES_Release_Content((char **)&pic_buf);
        }

        Load_Flag = 1;    // ��־��Դ�������
        for (uint32_t xC=ID_BEEPER_TICK; xC<=ID_BEEPER_EXIT; xC++)
        {
          ShowWindow(GetDlgItem(hwnd, xC), SW_SHOW);    // ��Դ������ɣ���ʾ��ҳ��ȫ���ؼ�
        }
        InvalidateRect(hwnd, NULL, TRUE);    // �ػ洰��
      }
        
    }break;

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      hdc = BeginPaint(hwnd, &ps);
      
      RECT rc2 = {0, 0, GUI_XSIZE, GUI_YSIZE};
        
      if (Load_Flag)     // ��Դ�������
      {
        BitBlt(hdc, 0, 0, GUI_XSIZE, GUI_YSIZE, hdc_beeper_bk, 0, 0, SRCCOPY);
      }
      else
      {    /* ��Դ����δ��� */ 
        SetBrushColor(hdc, MapRGB(hdc, 10, 10, 10));
        FillRect(hdc, &rc2);
        SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
        DrawText(hdc, L"��Դ�����У����Ե�...", -1, &rc2, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
      }

      EndPaint(hwnd, &ps);
      break;
    }

    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case ID_BEEPER_EXIT:
          {
            ExitButton_OwnerDraw(ds);
            return TRUE;             
          }  

          case ID_BEEPER_SW:
          case ID_BEEPER_TICK:
          {
            sw_button_OwnerDraw(ds);
            return TRUE;   
          }

          case ID_BEEPER_PRESS:
          {
            press_button_OwnerDraw(ds);
            return TRUE;   
          }
       }

       break;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������    
      if(code == BN_CLICKED)
      {
        if (id == ID_BEEPER_EXIT)
        {
          PostCloseMessage(hwnd);
        }
        else if (id == ID_BEEPER_TICK)
        {
          tick_flag = !tick_flag;    // ������־

          if (tick_flag)
          {
            SetTimer(hwnd, 1, 500, TMR_START, NULL);    // �����δ�ʱ��
          }
          else
          {
            KillTimer(hwnd, 1);    // �رյδ�ʱ��
            BEEP_OFF;              // �رշ�����
            press_flag =3;
          }
          
          sw_flag = 0;    // ��λ��־
          RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_SW), NULL, TRUE);
          RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_PRESS), NULL, TRUE);
        }
        else if (id == ID_BEEPER_SW)
        {
          sw_flag = !sw_flag;    // ������־

          if (sw_flag)
          {
            BEEP_ON;    // ��������
          }
          else
          { 
            BEEP_OFF;    // �ط����� 
          }

          KillTimer(hwnd, 1);
          tick_flag = 0;
          press_flag = 3;

          RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_TICK), NULL, TRUE);
          RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_PRESS), NULL, TRUE);
        }
        else if (id == ID_BEEPER_PRESS)
        {
          KillTimer(hwnd, 1);
          BEEP_OFF;    // �ط����� 
          press_flag = 3;
          RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_PRESS), NULL, TRUE);
        }
      }
      
      if(code == BN_PUSHED && id == ID_BEEPER_PRESS)    // ����
      {
        tick_flag = 0;
        sw_flag = 0;

        RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_TICK), NULL, TRUE);
        RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_SW), NULL, TRUE);
        SetTimer(hwnd, 1, 500, TMR_START, NULL);
      }

      break;
    } 

    case WM_DESTROY:
    {
      tick_flag = 0;
      sw_flag = 0;
      press_flag = 3;
      
      BEEP_OFF;
      
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_Beeper_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;
  HWND MAIN_Handle;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//����������
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI_Beeper_Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);
   //��ʾ������
	ShowWindow(MAIN_Handle, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, MAIN_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}


