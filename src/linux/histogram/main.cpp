
#include <cstdio>
#include <string>

#include <gdal_priv.h>
#include <cpl_conv.h>

#include "exception.h"

using namespace std;

int histogram(GDALDataset * src);

int main()
{
	unsigned char * read_buf = NULL, * write_buf = NULL, ch[3] = {'2', '3', '4'};
	int band, ret = 0;
	unsigned v, u, t, height, width, from_x = 2000, from_y = 2000, x_size = 1000, y_size = 1000;
	double transform_coef[6];
	string src_fname, dst_fname = "test/res.tif";
	// string base_src_fname = "/home/amv/RGRTU/evm/data/satellite/Landsat/TM/Мещера/LT51760222010175MOR00/L5176022_02220100624_B";
	string base_src_fname = "/home/amv/trash/sda4/data/satellite/Landsat/TM/Мещера/LT51760222010223KIS01/L5176022_02220100811_B";
	GDALDriver * drv = NULL;
	GDALDataset * src[3] = {NULL, NULL, NULL}, * dst = NULL;

	try
	{
		/* Загрузка драйверов растровых форматов */
		GDALAllRegister();

		/* Получение описателя драйвера формата GeoTIFF */
		throw_null((drv = GetGDALDriverManager()->GetDriverByName("GTiff")));

		for(v = 0; v < 3; v++)
		{
			src_fname = ch[v];
			src_fname = base_src_fname + src_fname + "0.tif";

			/* Открытие на чтение файла формата GeoTIFF с очередным каналом спутникового снимка */
			throw_null((src[v] = (GDALDataset *) GDALOpen(src_fname.c_str(), GA_ReadOnly)));
		}

		/* Получение размеров спутникового снимка */
		height = src[0]->GetRasterYSize();
		width = src[0]->GetRasterXSize();

		/* Выделение памяти под буфер, в который будут считываться каналы спутникового снимка */
		throw_null((read_buf = new unsigned char[height * width]));

		/* Выделение памяти под буфер, в котором будут формироваться каналы результирующего изображения */
		throw_null((write_buf = new unsigned char[x_size * y_size]));

		/* Создание результирующего изображения */
		throw_null((dst = drv->Create(dst_fname.c_str(), x_size, y_size, 3, GDT_Byte, NULL)));

		for(v = 0; v < 3; v++)
		{
			band = v + 1;

			/* Чтение очередного канала спутникового снимка */
			throw_if((src[v]->RasterIO(GF_Read, 0, 0, width, height, read_buf, width, height, GDT_Byte, 1, NULL, 0, 0, 0) != CE_None));

			/* Копирование в канал результирующего изображения части канала исходного спутникового снимка */
			for(u = 0; u < y_size; u++)
				for(t = 0; t < x_size; t++)
					write_buf[u * x_size + t] = read_buf[(u + from_y) * width + (t + from_x)];

			/* Сохранение очередного канала результирующего изображения */
			throw_if((dst->RasterIO(GF_Write, 0, 0, x_size, y_size, write_buf, x_size, y_size, GDT_Byte, 1, & band, 0, 0, 0) != CE_None));
		}
		
		/* Расчет данных о коэффициентах геопривязки результирующего изображения - простое копирование не подходит, так как каналы исходного спутникового снимка копируются не полностью */
		src[0]->GetGeoTransform(transform_coef);

		transform_coef[0] += from_x * transform_coef[1] + from_y * transform_coef[2];
		transform_coef[3] += from_x * transform_coef[4] + from_y * transform_coef[5];

		dst->SetGeoTransform(transform_coef);
		
		/* Копирование сведений о системе координат и проекции результирующего изображения */
		dst->SetProjection(src[0]->GetProjectionRef());

		/* Расчет гистограммы канала */
		throw_if(histogram(src[0]));
	}
	catch(...)
	{
		ret = -1;
	}

	if(read_buf != NULL)
		delete read_buf;

	if(write_buf != NULL)
		delete write_buf;

	/* Закрытие файлов каналов исходного спутникового снимка */
	for(v = 0; v < 3; v++)
		if(src[v] != NULL)
			GDALClose(src[v]);

	/* Закрытие файла результирующего изображения */
	if(dst != NULL)
		GDALClose(dst);

	return ret;
}

int histogram(GDALDataset * src)
{
	int ret = 0;
	unsigned char * buf = NULL;
	unsigned v, u, height = src->GetRasterYSize(), width = src->GetRasterXSize();
	unsigned height_width = height * width;
	double hist[256];

	try
	{
		throw_null((buf = new unsigned char[height_width]));
		throw_if((src->RasterIO(GF_Read, 0, 0, width, height, buf, width, height, GDT_Byte, 1, NULL, 0, 0, 0) != CE_None));

		for(v = 0; v < 256; v++)
			hist[v] = 0;

		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
				hist[buf[v * width + u]]++;

		for(v = 0; v < 256; v++)
		{
			hist[v] /= height_width;
			printf("%u = %lf\n", v, hist[v]);
		}
	}
	catch(...)
	{
		ret = -1;
	}

	if(buf != NULL)
		free(buf);

	return ret;
}

