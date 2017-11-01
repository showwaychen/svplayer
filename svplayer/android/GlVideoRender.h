#ifndef GL_VIDEO_RENDER_H_
#define GL_VIDEO_RENDER_H_

#include "android/jnihelper/jni_classloader.h"
#include "../src/VideoRenderBase.h"
#include "GLES2/gl2.h"
#include <pthread.h>
#include "GlShader.h"
class CGLVideoRender : public VideoRenderBase
{
	static CRegisterNativeM s_registernm;

	JNIEnv* jni_;
	//jclass j_class_;
	jobject m_javaObject;
	jmethodID m_javaRequestRender;
	//opengl es 

	CGlShader* m_cGlShader;
	GLuint m_VertexShader;
	GLuint m_FragmentShader;
	GLuint m_Program;
	GLuint m_nPositionMatrix;
	GLint m_yLoc, m_uLoc, m_vLoc;
	GLuint m_yTexture, m_uTexture, m_vTexture;

	static const char g_VertexShaderStr[];

	static const char g_FragmentShaderStr[];
	static float g_DefaultMatrix[];


	pthread_mutex_t m_imgdata_mutex;
	uint8_t* m_ImageData = nullptr;
	int m_nSize = 0;

	void OnFragmentUniform(uint8_t *pSrcBuffer, int nWidth, int nHeight);
	void OnFragmentDelete();


	void SetPositionMatrix();
protected:
	int renderInit();
	int renderUninit();
	int renderResize(int width, int height);
	int renderFrame();
public:
	static CGLVideoRender* GetInst(JNIEnv* jni, jobject j_player);
	static jlong JNICALL newinstance(JNIEnv *env, jobject thiz);
	static jint JNICALL nativeRenderInit(JNIEnv *env, jobject thiz);
	static jint JNICALL nativeRenderDeinit(JNIEnv *env, jobject thiz);
	static jint JNICALL nativeRenderResize(JNIEnv *env, jobject thiz, jint width, jint height);
	static jint JNICALL nativeRenderFrame(JNIEnv *env, jobject thiz);
	static void JNICALL nativeSetShowMode(JNIEnv *env, jobject thiz, jint mode);
	static void JNICALL nativeDestroy(JNIEnv *env, jobject thiz);

	CGLVideoRender(JNIEnv* env, jobject thiz);
	~CGLVideoRender();

	virtual int renderImage(uint8_t *pimgdata, int nsize);

	virtual void setSrcImageSize(int width, int height);

};
#endif 