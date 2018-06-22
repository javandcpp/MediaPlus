package app.mobile.nativeapp.com.mediaplus.opengl;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import app.mobile.nativeapp.com.libmedia.core.inter.SaveFrameCallBack;
import app.mobile.nativeapp.com.mediaplus.util;

/**
 * Created by chenyan on 2015/10/21.
 */
public class CameraGLSurfaceView extends GLSurfaceView implements SaveFrameCallBack
{
    // 源视频帧宽/高
    private int srcFrameWidth  = 640;
    private int srcFrameHeight = 480;
    private int viewWidth = 0, viewHeight = 0;
    private int frameWidth = 640, frameHeight = 480;


    private ByteBuffer yBuf = null, uBuf = null, vBuf = null;
//    private ByteBuffer mBuf = null;
    private  int yuvFrameSize = 640*480;
    // 纹理id
    private int[] Ytexture = new int[1];
    private int[] Utexture = new int[1];
    private int[] Vtexture = new int[1];
    private int[] Mtexture = new int[1];
    private int aPositionMain = 0, aTexCoordMain = 0,  uYTextureMain = 0, uUTextureMain = 0, uVTextureMain = 0,uMTextureMain = 0;
    private int programHandleMain = 0;
    private static final int FLOAT_SIZE_BYTES = 4;

    private FloatBuffer squareVertices = null;
    private FloatBuffer coordVertices = null;
    private boolean mbpaly = false;

    //**Frame Buffer Object管理----------------------------------------------------------------------------------------/
    private IntBuffer frameBufferHandle = null;
    private IntBuffer textureHandle = null;
    private int fboWidth = 0, fboHeight = 0;
//    private Bitmap baseBmp = null;

    public CameraGLSurfaceView(Context context)
    {
        super(context);
        setEGLContextClientVersion(2);
        //设置Renderer到GLSurfaceView
        setRenderer(new MyGL20Renderer());
        // 只有在绘制数据改变时才绘制view
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        int qtrFrameSize = yuvFrameSize >> 2;
        yBuf = ByteBuffer.allocateDirect(yuvFrameSize);
        yBuf.order(ByteOrder.nativeOrder()).position(0);

        uBuf = ByteBuffer.allocateDirect(qtrFrameSize);
        uBuf.order(ByteOrder.nativeOrder()).position(0);

        vBuf = ByteBuffer.allocateDirect(qtrFrameSize);
        vBuf.order(ByteOrder.nativeOrder()).position(0);

//        mBuf = ByteBuffer.allocateDirect(yuvFrameSize * 4);
//        mBuf.order(ByteOrder.nativeOrder()).position(0);
        // 顶点坐标
        squareVertices = ByteBuffer.allocateDirect(util.squareVertices.length * FLOAT_SIZE_BYTES).order(ByteOrder.nativeOrder()).asFloatBuffer();
        squareVertices.put(util.squareVertices).position(0);
        //纹理坐标
        coordVertices = ByteBuffer.allocateDirect(util.coordVertices.length * FLOAT_SIZE_BYTES).order(ByteOrder.nativeOrder()).asFloatBuffer();
        coordVertices.put(util.coordVertices).position(0);

//        baseBmp = Bitmap.createBitmap(frameWidth, frameHeight, Bitmap.Config.ARGB_8888);// 底图
//        Canvas baseCanvas = new Canvas();
//        baseCanvas.setBitmap(baseBmp);
//        baseBmp.copyPixelsToBuffer(mBuf);
    }

    public class MyGL20Renderer implements Renderer
    {

        @Override
        public void onSurfaceCreated(GL10 unused, EGLConfig config)
        {
            mbpaly = false;
            //设置背景的颜色
            GLES20.glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            //启动纹理
            GLES20.glEnable(GLES20.GL_TEXTURE_2D);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE3);

            changeFilterShader(0);
            //创建yuv纹理
            createTexture(frameWidth, frameHeight, GLES20.GL_LUMINANCE, Ytexture);
            createTexture(frameWidth>>1, frameHeight>>1, GLES20.GL_LUMINANCE, Utexture);
            createTexture(frameWidth >> 1, frameHeight >> 1, GLES20.GL_LUMINANCE, Vtexture);
            createTexture(frameWidth, frameHeight, GLES20.GL_RGBA, Mtexture);


            mbpaly = true;
        }

        private void changeFilterShader( int filterId )
        {
            programHandleMain = util.createShaderProgram();
            if ( programHandleMain != -1 )
            {
                // 获取VertexShader变量
                aPositionMain = getShaderHandle(programHandleMain, "vPosition");
                aTexCoordMain = getShaderHandle(programHandleMain, "a_texCoord");
                // 获取FrameShader变量
                uYTextureMain = getShaderHandle(programHandleMain, "SamplerY");
                uUTextureMain = getShaderHandle(programHandleMain, "SamplerU");
                uVTextureMain = getShaderHandle(programHandleMain, "SamplerV");
                uMTextureMain = getShaderHandle(programHandleMain, "SamplerM");

                // 使用滤镜着色器程序
                GLES20.glUseProgram(programHandleMain);

                //给变量赋值
                GLES20.glUniform1i(uYTextureMain, 0);
                GLES20.glUniform1i(uUTextureMain, 1);
                GLES20.glUniform1i(uVTextureMain, 2);
                GLES20.glUniform1i(uMTextureMain, 3);
                GLES20.glEnableVertexAttribArray(aPositionMain);
                GLES20.glEnableVertexAttribArray(aTexCoordMain);

                // 设置Vertex Shader数据
                squareVertices.position(0);
                GLES20.glVertexAttribPointer(aPositionMain, 2, GLES20.GL_FLOAT, false, 0, squareVertices);
                coordVertices.position(0);
                GLES20.glVertexAttribPointer(aTexCoordMain, 2, GLES20.GL_FLOAT, false, 0, coordVertices);
            }
        }

        // 创建纹理
        private void createTexture(int width, int height, int format, int[] textureId)
        {
            //创建纹理
            GLES20.glGenTextures(1, textureId, 0);
            //绑定纹理
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId[0]);
            //设置纹理属性
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, format, width, height, 0, format, GLES20.GL_UNSIGNED_BYTE, null);
        }
        @Override
        public void onDrawFrame(GL10 unused)
        {
            // 重绘背景色
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

            if ( yBuf != null )
            {
                //y
                GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, Ytexture[0]);
                GLES20.glTexSubImage2D(GLES20.GL_TEXTURE_2D,
                        0,
                        0,
                        0,
                        frameWidth,
                        frameHeight,
                        GLES20.GL_LUMINANCE,
                        GLES20.GL_UNSIGNED_BYTE,
                        yBuf);

                //u
                GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, Utexture[0]);
                GLES20.glTexSubImage2D(GLES20.GL_TEXTURE_2D,
                        0,
                        0,
                        0,
                        frameWidth >> 1,
                        frameHeight >> 1,
                        GLES20.GL_LUMINANCE,
                        GLES20.GL_UNSIGNED_BYTE,
                        uBuf);

                //v
                GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, Vtexture[0]);
                GLES20.glTexSubImage2D(GLES20.GL_TEXTURE_2D,
                        0,
                        0,
                        0,
                        frameWidth >> 1,
                        frameHeight >> 1,
                        GLES20.GL_LUMINANCE,
                        GLES20.GL_UNSIGNED_BYTE,
                        vBuf);

                //mark图层
                GLES20.glActiveTexture(GLES20.GL_TEXTURE3);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, Mtexture[0]);
                GLES20.glTexSubImage2D(GLES20.GL_TEXTURE_2D, 0, 0, 0, frameWidth, frameHeight, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
            }

            //绘制
            GLES20.glViewport(0, 0, viewWidth, viewHeight);


            GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
        }

        @Override
        public void onSurfaceChanged(GL10 unused, int width, int height)
        {
            viewWidth  = width;
            viewHeight = height;
            GLES20.glViewport(0, 0, viewWidth, viewHeight);
        }
    }


    @Override
    public void onSaveFrames(byte[] data, int length)
    {
        if (  length != 0 && mbpaly )
        {
            yBuf.clear();
            uBuf.clear();
            vBuf.clear();
//           rotateYUV(data, srcFrameWidth, srcFrameHeight);
            requestRender();
        }
    }

    public  int  getShaderHandle(int programHandle,String name)
    {
        int handle = GLES20.glGetAttribLocation(programHandle, name);
        if (handle == -1)
        {
            handle = GLES20.glGetUniformLocation(programHandle, name);
        }
        return handle;
    }

    public void rotateYUV(byte[] src,int width,int height)
    {
        byte [] yArray = new  byte[yBuf.limit()];
        byte [] uArray = new  byte[uBuf.limit()];
        byte [] vArray = new  byte[vBuf.limit()];
        int nFrameSize = width * height;
        int k          = 0;
        int uvCount    = nFrameSize>>1;

        //取分量y值
        for(int i = 0;i < height*width;i++ )
        {
            yArray[ k ] = src[ i ];
            k++;
        }

        k = 0;

        //取分量uv值
        for( int i = 0;i < uvCount ;i+=2 )
        {
            vArray[ k ] = src[ nFrameSize +  i ]; //v
            uArray[ k ] = src[ nFrameSize +  i + 1 ];//u
            k++;
        }

        yBuf.put(yArray).position(0);
        uBuf.put(uArray).position(0);
        vBuf.put(vArray).position(0);
    }
    public void initFBO(int nWidth,int nHeight)
    {
        fboWidth           = nWidth;
        fboHeight          = nHeight;
        frameBufferHandle  = IntBuffer.allocate(1);
        textureHandle      = IntBuffer.allocate(1);

        // 生成fbo
        GLES20.glGenFramebuffers(1, frameBufferHandle);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, frameBufferHandle.get(0));

        // 将纹理图像与FBO关联
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER,
                GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D,
                Ytexture[0],
                0);

        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER,
                GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D,
                Utexture[0],
                0);

        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER,
                GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D,
                Utexture[0],
                0);
    }
    public void bindFBO()
    {
        //启动fbo
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, frameBufferHandle.get(0));

    }
    public void useFBOTexture()
    {
        //id置为0时,不再使用FBO
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

    }
    public void deleteFBO()
    {
        GLES20.glDeleteFramebuffers(1, frameBufferHandle);
 //       GLES20.glDeleteTextures(1, textureHandle);
        frameBufferHandle.clear();
//        textureHandle.clear();
        fboWidth = fboHeight = 0;
    }
}
