// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// ZLIB_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的

#ifndef ZLIBH
#define ZLIBH


#ifdef __cplusplus
extern "C" {
#endif

#ifdef ZLIB_EXPORTS
#define ZLIB_API __declspec(dllexport)
#else
#define ZLIB_API __declspec(dllimport)
#endif

	//以下是此DLL的导出函数
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	//函数参数：input		待压缩数据指针
	//函数参数：uncomprLen	未被压缩前数据字节数
	//函数参数：output		压缩后数据指针
	//函数参数：comprLen		输入即压缩前是output预分配空间大小 (可以等于uncomprLen) ,压缩后为输出的数据大小
	//函数返回：false值代表执行失败；true值代表执行成功。
	//函数说明：压缩核心
	ZLIB_API size_t HW_Compress(void *input, unsigned long uncomprLen, void *output, unsigned long &comprLen);
	//---------------------------------------------------------------------------
	//函数参数：input		待解压缩数据指针
	//函数参数：comprLen		输入的待解压缩数据字节数
	//函数参数：output		解压缩后数据指针，预分配空间需要根据压缩前的数据长度确定
	//函数参数：uncomprLen	解压缩后的数据长度，无输入作用，仅作为输出参数
	//函数返回：false值代表执行失败；true值代表执行成功。
	//函数说明：压缩核心
	ZLIB_API size_t HW_Decompress(void *input, unsigned long comprLen, void *output, unsigned long &uncomprLen);
	//---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif
