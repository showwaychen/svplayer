#include"GlVideoRender.h"
#include "../src/Log.h"


static JNINativeMethod ls_nm[] = {
	{ "nativeCreateObject", "()J", reinterpret_cast<void*>(
	&CGLVideoRender::newinstance) },
	{ "nativeRenderInit", "()I", reinterpret_cast<void*>(
	&CGLVideoRender::nativeRenderInit) },
	{ "nativeRenderDeinit", "()I", reinterpret_cast<void*>(
	&CGLVideoRender::nativeRenderDeinit) },
	{ "nativeRenderFrame", "()I", reinterpret_cast<void*>(
	&CGLVideoRender::nativeRenderFrame) },
	{ "nativeRenderResize", "(II)I", reinterpret_cast<void*>(
	&CGLVideoRender::nativeRenderResize) },
	{ "nativeSetShowMode", "(I)V", reinterpret_cast<void*>(
	&CGLVideoRender::nativeSetShowMode) },
	{ "nativeDestroy", "()V", reinterpret_cast<void*>(
	&CGLVideoRender::nativeDestroy) }
};
CRegisterNativeM CGLVideoRender::s_registernm("cxw/cn/svplayer/NativeRender", ls_nm, ARRAYSIZE(ls_nm));

const char  CGLVideoRender::g_VertexShaderStr[] =
"attribute vec4 vPosition;    \n"
"attribute vec2 a_texCoord;   \n"
"uniform mat4 vMatrix;\n"
"varying vec2 tc;     \n"
"void main()                  \n"
"{                            \n"
"   gl_Position = vPosition*vMatrix;  \n"
"   tc = a_texCoord;  \n"
"}                            \n";

const char CGLVideoRender::g_FragmentShaderStr[] = 
"precision mediump float;\n"
"uniform sampler2D tex_y;                 \n"
"uniform sampler2D tex_u;                 \n"
"uniform sampler2D tex_v;                 \n"
"varying vec2 tc;                         \n"
"void main()                                  \n"
"{                                            \n"
"float y = texture2D(tex_y, tc).r; \n"
"float u = texture2D( tex_u, tc ).r; \n"
"float v = texture2D( tex_v, tc ).r; \n"
"y = 1.1643 * ( y - 0.0625 ); \n"
"u = u - 0.5; \n"
"v = v - 0.5; \n"
"float r = y + 1.5958 * v; \n"
"float g = y - 0.39173 * u - 0.81290 * v; \n"
"float b = y + 2.017 * u; \n"
"gl_FragColor = vec4(r,g,b, 1.0); \n"
"}                                            \n";


float CGLVideoRender::g_DefaultMatrix[] = { 1.0f, 0.0f, 0.0f, 0.0f,
										0.0f, 1.0f, 0.0f, 0.0f,
										0.0f, 0.0f, 1.0f, 0.0f,
										0.0f, 0.0f, 0.0f, 1.0f };

void CGLVideoRender::OnFragmentUniform(uint8_t *pSrcBuffer, int nWidth, int nHeight)
{
	if (pSrcBuffer == nullptr)
	{
		return;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &m_yTexture);
	glBindTexture(GL_TEXTURE_2D, m_yTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, nWidth, nHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pSrcBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &m_uTexture);
	glBindTexture(GL_TEXTURE_2D, m_uTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, nWidth / 2, nHeight / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pSrcBuffer + nWidth * nHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &m_vTexture);
	glBindTexture(GL_TEXTURE_2D, m_vTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, nWidth / 2, nHeight / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pSrcBuffer + nWidth * nHeight * 5 / 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_yTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_vTexture);

	glUniform1i(m_yLoc, 0);
	glUniform1i(m_uLoc, 1);
	glUniform1i(m_vLoc, 2);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void CGLVideoRender::OnFragmentDelete()
{
	glDeleteTextures(1, &m_yTexture);
	glDeleteTextures(1, &m_uTexture);
	glDeleteTextures(1, &m_vTexture);
}




void CGLVideoRender::SetPositionMatrix()
{
	float matrix[] = { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
	LOGD << m_SrcWidth << "  " << m_SrcHeight << "  " << m_ShowWidth << "  " << m_ShowHeight;
	if (m_SrcWidth == 0 || m_SrcHeight == 0 || m_nPositionMatrix == -1 || m_ShowWidth == 0 || m_ShowHeight == 0 || m_eShowMode == kShowModeFill)
	{
		glUniformMatrix4fv(m_nPositionMatrix, 1, false, g_DefaultMatrix);
	}
	else
	{
		float xfinalratio = 1.0f;
		float yfinalratio = 1.0f;
		if (m_eShowMode == kShowModeAspectAuto)
		{
			float wratio = (float)m_ShowWidth  / m_SrcWidth ;
			float hratio = (float)m_ShowHeight / m_SrcHeight ;
			LOGD << "wratio = " << wratio << "  hratio = " << hratio;
			if (wratio > hratio)
			{
				xfinalratio = (float)hratio / wratio;
			}
			else
			{
				yfinalratio = (float)wratio / hratio;
			}
		}
		else if (m_eShowMode == kShowModeAspectFill)
		{

		}
		else if (m_eShowMode == kShowModeAspectFit)
		{

		}
		matrix[0] = xfinalratio;
		matrix[5] = yfinalratio;
		glUniformMatrix4fv(m_nPositionMatrix, 1, false, matrix);
	}
}

int CGLVideoRender::renderInit()
{
	glClearColor(0, 0, 0, 1);
	m_cGlShader = new CGlShader(g_VertexShaderStr, g_FragmentShaderStr);
	m_yLoc = m_cGlShader->GetUniformLocation("tex_y");
	m_uLoc = m_cGlShader->GetUniformLocation("tex_u");
	m_vLoc = m_cGlShader->GetUniformLocation("tex_v");
	m_nPositionMatrix = m_cGlShader->GetUniformLocation("vMatrix");
	return 0;
}

int CGLVideoRender::renderUninit()
{
	m_cGlShader->Release();
	delete m_cGlShader;
	m_cGlShader = NULL;
	return 0;
}

int CGLVideoRender::renderResize(int width, int height)
{
	m_ShowWidth = width;
	m_ShowHeight = height;
	glViewport(0, 0, width, height);
	return 0;
}

int CGLVideoRender::renderFrame()
{
	if (m_ImageData == nullptr)
	{
		return -1;
	}
	float vertices[] = { -1, -1, -1, 1, 1, -1, 1, 1 };

	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	m_cGlShader->UseProgram();
	float texs[] = { 0, 1, 0, 0, 1, 1, 1, 0 };
	m_cGlShader->SetVertexAttribArray("vPosition", 2, vertices);
	SetPositionMatrix();
	m_cGlShader->SetVertexAttribArray("a_texCoord", 2, texs);
	
	pthread_mutex_lock(&m_imgdata_mutex);
	OnFragmentUniform(m_ImageData, m_SrcWidth, m_SrcHeight);
	pthread_mutex_unlock(&m_imgdata_mutex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	OnFragmentDelete();
	return 0;

}

CGLVideoRender* CGLVideoRender::GetInst(JNIEnv* jni, jobject j_player)
{

	LOG(LS_INFO) << "CGLVideoRender::GetInst start";
	jclass j_class = jni->GetObjectClass(j_player);
	jfieldID nativeobject_id = jni->GetFieldID(j_class, "mNativeObject", "J");
	MYCHECK_EXCEPTION(jni, "GetInst failed");
	jlong j_p = jni->GetLongField(j_player, nativeobject_id);
	MYCHECK_EXCEPTION(jni, "GetInst failed");

	return reinterpret_cast<CGLVideoRender*>(j_p);
}

jlong JNICALL CGLVideoRender::newinstance(JNIEnv *env, jobject thiz)
{
	CGLVideoRender* instance = new CGLVideoRender(env, thiz);
	return jlongFromPointer((void*)instance);
}

jint JNICALL CGLVideoRender::nativeRenderInit(JNIEnv *env, jobject thiz)
{

	CGLVideoRender *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->renderInit();
	}

	return -1;
}

jint JNICALL CGLVideoRender::nativeRenderDeinit(JNIEnv *env, jobject thiz)
{

	CGLVideoRender *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->renderUninit();
	}

	return -1;
}

jint JNICALL CGLVideoRender::nativeRenderResize(JNIEnv *env, jobject thiz, jint width, jint height)
{
	LOG(LS_INFO) << "nativeRenderResize start";
	CGLVideoRender *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->renderResize(width, height);
	}

	return -1;
}

jint JNICALL CGLVideoRender::nativeRenderFrame(JNIEnv *env, jobject thiz)
{

	LOG(LS_INFO) << "nativeRenderFrame start";
	CGLVideoRender *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->renderFrame();
	}

	LOG(LS_INFO) << "nativeRenderFrame end";

	return -1;
}

void JNICALL CGLVideoRender::nativeSetShowMode(JNIEnv *env, jobject thiz, jint mode)
{
	CGLVideoRender *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		instance->SetShowMode((ShowMode)mode);
	}
}

void JNICALL CGLVideoRender::nativeDestroy(JNIEnv *env, jobject thiz)
{
	CGLVideoRender *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		delete instance;
	}

	LOG(LS_INFO) << "nativeRender destroy";
}

CGLVideoRender::CGLVideoRender(JNIEnv* env, jobject thiz) :jni_(env)
{
	m_javaObject = jni_->NewGlobalRef(thiz);
	pthread_mutex_init(&m_imgdata_mutex, NULL);
	m_ImageData = NULL;
	jclass j_class = jni_->GetObjectClass(thiz);
	m_javaRequestRender = jni_->GetMethodID(j_class, "requestRender", "()V");

}

CGLVideoRender::~CGLVideoRender()
{
	AttachThreadScoped attachthread(GetJavaVM());
	JNIEnv* jnienv = attachthread.env();
	jnienv->DeleteGlobalRef(m_javaObject);
	MYCHECK_EXCEPTION(jnienv, "DeleteGlobalRef error");
	jni_ = NULL;
	pthread_mutex_destroy(&m_imgdata_mutex);
	if (m_ImageData != NULL)
	{
		delete m_ImageData;
	}
	m_ImageData = NULL;
}

int CGLVideoRender::renderImage(uint8_t *pimgdata, int nsize)
{
	LOGD << "renderImage start";
	if (nsize > m_nSize)
	{
		return -1;
	}
	LOGD << "renderImage mid";

	pthread_mutex_lock(&m_imgdata_mutex);
	memcpy(m_ImageData, pimgdata, nsize);
	pthread_mutex_unlock(&m_imgdata_mutex);
	AttachThreadScoped attachthread(GetJavaVM());
	JNIEnv *jni = attachthread.env();
	jclass j_class = jni->GetObjectClass(m_javaObject);
	jni->CallVoidMethod(m_javaObject, jni->GetMethodID(j_class, "requestRender", "()V"));
	LOGD << "renderImage end";

	return 0;

}

void CGLVideoRender::setSrcImageSize(int width, int height)
{
	if (m_SrcHeight != height || m_SrcWidth != width)
	{
		VideoRenderBase::setSrcImageSize(width, height);
		m_nSize = m_SrcHeight * m_SrcWidth * 3 / 2;
		pthread_mutex_lock(&m_imgdata_mutex);
		if (m_ImageData != nullptr)
		{
			delete m_ImageData;
			m_ImageData = nullptr;
		}
		m_ImageData = new uint8_t[m_nSize];
		pthread_mutex_unlock(&m_imgdata_mutex);

	}
	
}
