#include <iostream>
#include "png.h"
#include "pngconf.h"
#include "pnglibconf.h"
using namespace std;
BOOL SaveToPNG(LPSTR szPath)
{
    PTSTR pstrFileName = szPath;
    png_byte *pDiData = NULL;
    int iWidth = m_dwWidth;
    int iHeight = m_dwHeight;
    png_color bkgColor;
    int red = 255;
    int blue = 255;
    int green = 255;
 
    png_bytep pRGBA = new png_byte[iHeight * iWidth * 4];
    memset(pRGBA, 255, iHeight * iWidth * 4);
    /*cout << iWidth << " " << iHeight << endl;*/
//     for (int i =0; i < dwHeight; i++)
//     {
//         for (int j = 0; j < dwWidth; j++)
//         {
//             RGBA pixel = GetRGBAAt(j, i);
//             pRGBA[(j + i * dwWidth) * 4] = pixel.B;                 // B
//             pRGBA[(j + i * dwWidth) * 4 + 1] = pixel.G;             // G
//             pRGBA[(j + i * dwWidth) * 4 + 2] = pixel.R;             // R
//             pRGBA[(j + i * dwWidth) * 4 + 3] = pixel.A;             // A
//         }
//     }
 
    for (int i = 0; i < iWidth; i++)
    {
        for (int j = 0; j < iHeight; j++)
        {
            RGBA pixel = GetRGBAAt(i, j);
            pRGBA[(i + j * iWidth) * 4] = pixel.B;                 // B
            pRGBA[(i + j * iWidth) * 4 + 1] = pixel.G;             // G
            pRGBA[(i + j * iWidth) * 4 + 2] = pixel.R;             // R
            pRGBA[(i + j * iWidth) * 4 + 3] = pixel.A;             // A
        }
    }
 
    pDiData = pRGBA;
     
 
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    const int           ciBitDepth = 8;
    const int           ciChannels = 4;
     
    static FILE        *pfFile;
    png_uint_32         ulRowBytes;
    static png_byte   **ppbRowPointers = NULL;
     
    // 打开输出文件
     
    if (!pstrFileName)
        return FALSE;
     
    if (!(pfFile = fopen(pstrFileName, "wb")))
        return FALSE;
     
    // 设置标准PNG结构
     
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
        (png_error_ptr)NULL, (png_error_ptr)NULL);
    if (!png_ptr)
    {
        fclose(pfFile);
        return FALSE;
    }
     
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) 
    {
        fclose(pfFile);
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
        return FALSE;
    }
     
     
    // 设置调色板
     
    png_set_IHDR(png_ptr, info_ptr, iWidth, iHeight, ciBitDepth,
        6, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);
     
    // 初始化IO，写文件头
    png_init_io(png_ptr, pfFile);
    png_write_info(png_ptr, info_ptr);
     
    // 将BGRA变成ARGB
     
    png_set_bgr(png_ptr);
     
    // 一行的字节数
    ulRowBytes = iWidth * ciChannels;
     
    // 为指针分配内存
     
    if ((ppbRowPointers = (png_bytepp) malloc(iHeight * sizeof(png_bytep))) == NULL)
        MessageBeep(MB_ICONERROR);
     
    // 设置指针
    for (i = 0; i < iHeight; i++)
        ppbRowPointers[i] = pDiData + i * iWidth * 4;
     
    // 一次性写入
    png_write_image (png_ptr, ppbRowPointers);
 
    // 填写附加块，不是必须
    png_write_end(png_ptr, info_ptr);
     
    // 全部完成
     
    free (ppbRowPointers);
    ppbRowPointers = NULL;
     
    // 扫尾
     
    png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
     
    fclose (pfFile);
     
    return TRUE;
}