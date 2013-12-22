#include <iostream>
#include "png.h"
#include "pngconf.h"
#include "pnglibconf.h"
using namespace std;
void myreadpng(char* name)
{
	// ǰ�߼����ǳ�������ʼ�����ֽṹ
	FILE* file = fopen(name, "rb");
	/*
	png_structp��������libpng��ʼ����ʱ�򴴽���
	��libpng���ڲ�ʹ�ã�����libpng���ǵ��������ģ�
	���ʹ���߲�Ӧ�ö�����������з��ʡ�
	����libpng��API��ʱ��
	��Ҫ�����������Ϊ��һ���������롣
	*/
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	/*
	png_infop������
	��ʼ�����libpng֮��
	���Դ�libpng�л�ø����ͱ���ָ�롣
	�������������pngͼƬ���ݵ���Ϣ��
	���ʹ���߿����޸ĺͲ��ĸñ�����
	���磺����ͼƬ��Ϣ���޸�ͼƬ���������
	�����ڵİ汾��ֱ�ӷ��ʸñ����ĳ�Ա��
	���µİ汾������ͨ��API��������Щ��Ա��
	*/
	png_infop info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));
	// ������Ҫ
	png_init_io(png_ptr, file);
	// ���ļ���
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
	// �õ��ļ��Ŀ��ɫ��
	int m_width = png_get_image_width(png_ptr, info_ptr);
	int m_height = png_get_image_height(png_ptr, info_ptr);
	
	//int color_type = png_get_color_type(png_ptr, info_ptr);
	//cout<<color_type<<endl;

	int channels = png_get_channels(png_ptr, info_ptr);	/*��ȡͨ����*/
	//cout <<channels;
	// ������ڴ����棬�����õ���c++�﷨��������c�ϱ��
	int size = m_height * m_width * 4;
	unsigned char* bgra = new unsigned char[size];
	int pos = 0;
	// row_pointers��߾��Ǵ�˵�е�rgba������
	png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
	// ��������ע�⣬������ȡ��pngû��Aͨ������Ҫ3λ3λ�Ķ������о���ע���ֽڶ�������⣬��򵥵ľ��Ǳ��ò��ܱ�4�����Ŀ�Ⱦ����ˡ�������ʵ�����ã���Ҫ�����������صĶ��봦��
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
	// ���ˣ������������������κε������ˡ�����������ʾ�������ߴ�ӡ�������С�
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(file);
	delete bgra;
	return;
}
