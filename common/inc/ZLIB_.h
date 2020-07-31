// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// ZLIB_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ�������

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

	//�����Ǵ�DLL�ĵ�������
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	//����������input		��ѹ������ָ��
	//����������uncomprLen	δ��ѹ��ǰ�����ֽ���
	//����������output		ѹ��������ָ��
	//����������comprLen		���뼴ѹ��ǰ��outputԤ����ռ��С (���Ե���uncomprLen) ,ѹ����Ϊ��������ݴ�С
	//�������أ�falseֵ����ִ��ʧ�ܣ�trueֵ����ִ�гɹ���
	//����˵����ѹ������
	ZLIB_API size_t HW_Compress(void *input, unsigned long uncomprLen, void *output, unsigned long &comprLen);
	//---------------------------------------------------------------------------
	//����������input		����ѹ������ָ��
	//����������comprLen		����Ĵ���ѹ�������ֽ���
	//����������output		��ѹ��������ָ�룬Ԥ����ռ���Ҫ����ѹ��ǰ�����ݳ���ȷ��
	//����������uncomprLen	��ѹ��������ݳ��ȣ����������ã�����Ϊ�������
	//�������أ�falseֵ����ִ��ʧ�ܣ�trueֵ����ִ�гɹ���
	//����˵����ѹ������
	ZLIB_API size_t HW_Decompress(void *input, unsigned long comprLen, void *output, unsigned long &uncomprLen);
	//---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif
