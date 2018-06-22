package app.mobile.nativeapp.com.mediaplus;

import android.opengl.GLES20;

import java.nio.IntBuffer;

/**
 * Created by Administrator on 2015/10/26.
 */
public class util
{
    public static String  VERTEX_SHADER =
        "attribute vec4 vPosition;    \n"+
        "attribute vec2 a_texCoord;   \n"+
        "varying vec2 tc;             \n"+
        "void main()                  \n"+
        "{                            \n"+
        "   gl_Position = vPosition;  \n"+
        "   tc = a_texCoord;          \n"+
        "}                            \n";
//    public static String FRAG_SHADER =
//        "varying lowp vec2 tc;      \n"+
//        "uniform sampler2D SamplerY;\n"+
//        "uniform sampler2D SamplerU;\n"+
//        "uniform sampler2D SamplerV;\n"+
//        "void main(void)            \n"+
//        "{                          \n"+
//        "mediump vec3 yuv;          \n"+
//        "lowp vec3 rgb;             \n"+
//        "yuv.x = texture2D(SamplerY, tc).r;     \n"+
//        "yuv.y = texture2D(SamplerU, tc).r - 0.5;\n"+
//        "yuv.z = texture2D(SamplerV, tc).r - 0.5;\n"+
//        "rgb = mat3( 1,1,1,0,-0.39465,2.03211,1.13983,-0.58060,0) * yuv;\n"+
//        "gl_FragColor = vec4(rgb, 1);\n"+
//        "}                          \n";
public static String FRAG_SHADER =
        "precision mediump float;\n"+
        "varying  vec2 tc;                      \n"+
        "uniform sampler2D SamplerY;            \n"+
        "uniform sampler2D SamplerU;            \n"+
        "uniform sampler2D SamplerV;            \n"+
        "uniform sampler2D SamplerM;            \n"+
        "const float PI = 3.14159265;           \n"+
        "const mat3 convertMat = mat3( 1.0, 1.0, 1.0, 0.0, -0.39465, 2.03211, 1.13983, -0.58060, 0.0 );\n"+
        "void main(void)                            \n"+
        "{                                          \n"+
        "vec3 yuv;                                  \n"+
        "yuv.x = texture2D(SamplerY, tc).r;         \n"+
        "yuv.y = texture2D(SamplerU, tc).r - 0.5;   \n"+
        "yuv.z = texture2D(SamplerV, tc).r - 0.5;   \n"+
        "vec3 color = convertMat * yuv;             \n"+
        "// 亮度/对比度调节                         \n"+
        "color += (1.0 - color) * color*0.2362;     \n"+
        "color = min(vec3(1.0), ((color - 0.5) * tan(0.328725 * PI) + 0.5));\n"+
        "// 调色                                    \n"+
        "color.g = color.g * 0.87 + 0.13;           \n"+
        "color.b = color.b * 0.439 + 0.561;         \n"+
        "color *= vec3(0.981, 0.862, 0.686);        \n"+
        "// 混合                                    \n"+
        "vec4 mainColor = vec4(color, 1.0);         \n"+
        "vec4 maskColor = texture2D(SamplerM, tc);  \n"+
        "mainColor.r = maskColor.r + mainColor.r * mainColor.a * (1.0 - maskColor.a);\n"+
        "mainColor.g = maskColor.g + mainColor.g * mainColor.a * (1.0 - maskColor.a);\n"+
        "mainColor.b = maskColor.b + mainColor.b * mainColor.a * (1.0 - maskColor.a);\n"+
        "mainColor.a = maskColor.a + mainColor.a * (1.0 - maskColor.a); \n"+
        "gl_FragColor =mainColor;                                       \n"+
        "}                                                              \n";
    public static float squareVertices[] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f,  1.0f,
            1.0f,  1.0f,
    };
    public static float coordVertices[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f,  1.0f,
            1.0f,  1.0f,
    };
    public static int createShaderProgram()
    {
        int vertexShader    = util.loadShader(GLES20.GL_VERTEX_SHADER, util.VERTEX_SHADER);
        int fragmentShader  = util.loadShader(GLES20.GL_FRAGMENT_SHADER, util.FRAG_SHADER);


        int mProgram        = GLES20.glCreateProgram();             // 创建一个空的OpenGL ES Program

        GLES20.glAttachShader(mProgram, vertexShader);   // 将vertex shader添加到program
        GLES20.glAttachShader(mProgram, fragmentShader); // 将fragment shader添加到program
        GLES20.glLinkProgram(mProgram);                  // 创建可执行的 OpenGL ES program

        // Check the link status
        IntBuffer linked = IntBuffer.allocate(1);
        GLES20.glGetProgramiv(mProgram, GLES20.GL_LINK_STATUS, linked );

        if ( linked.get(0) == 0 )
            return -1;

        return mProgram;
    }
    public static int loadShader(int type, String shaderCode)
    {
        // 创建一个vertex shader类型(GLES20.GL_VERTEX_SHADER)
        // 或fragment shader类型(GLES20.GL_FRAGMENT_SHADER)
        int shader = GLES20.glCreateShader(type);

        // 将源码添加到shader并编译之
        GLES20.glShaderSource(shader, shaderCode);
        GLES20.glCompileShader(shader);

        // Check the compile status
        IntBuffer compiled = IntBuffer.allocate(1);
        GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS,compiled );
        if (compiled.get(0) == 0)
            return  0;

        return shader;
    }
}
