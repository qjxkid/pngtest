#include <iostream>
#include "png.h"
#include "pngconf.h"
#include "pnglibconf.h"
using namespace std;
void myreadpng(char* name)
{
	// 前边几句是扯淡，初始化各种结构
	FILE* file = fopen(name, "rb");
	/*
	png_structp变量是在libpng初始化的时候创建，
	由libpng库内部使用，代表libpng的是调用上下文，
	库的使用者不应该对这个变量进行访问。
	调用libpng的API的时候，
	需要把这个参数作为第一个参数传入。
	*/
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	/*
	png_infop变量，
	初始化完成libpng之后，
	可以从libpng中获得该类型变量指针。
	这个变量保存了png图片数据的信息，
	库的使用者可以修改和查阅该变量，
	比如：查阅图片信息，修改图片解码参数。
	在早期的版本中直接访问该变量的成员，
	最新的版本建议是通过API来访问这些成员。
	*/
	png_infop info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));
	// 这句很重要
	png_init_io(png_ptr, file);
	// 读文件了
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
	// 得到文件的宽高色深
	int m_width = png_get_image_width(png_ptr, info_ptr);
	int m_height = png_get_image_height(png_ptr, info_ptr);
	
	//int color_type = png_get_color_type(png_ptr, info_ptr);
	//cout<<color_type<<endl;

	int channels = png_get_channels(png_ptr, info_ptr);	/*获取通道数*/
	//cout <<channels;
	// 申请个内存玩玩，这里用的是c++语法，甭想再c上编过
	int size = m_height * m_width * 4;
	unsigned char* bgra = new unsigned char[size];
	int pos = 0;
	// row_pointers里边就是传说中的rgba数据了
	png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
	// 拷贝！！注意，如果你读取的png没有A通道，就要3位3位的读。还有就是注意字节对其的问题，最简单的就是别用不能被4整除的宽度就行了。读过你实在想用，就要在这里加上相关的对齐处理。
	for(int i = 0; i < m_height; i++)
	{
		for(int j = 0; j < (4 * m_width); j += 4)
		{
			bgra[pos++] = row_pointers[i][j + 2]; // blue
			bgra[pos++] = row_pointers[i][j + 1]; // green
			bgra[pos++] = row_pointers[i][j];	  // red
			bgra[pos++] = row_pointers[i][j + 3]; // alpha
		}
	}
	// 好了，你可以用这个数据作任何的事情了。。。把它显示出来或者打印出来都行。
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(file);
	delete bgra;
	return;
}
