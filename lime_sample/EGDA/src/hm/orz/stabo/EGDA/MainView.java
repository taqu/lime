package hm.orz.stabo.EGDA;

import hm.orz.stabo.EGDA.io.SensorSystem;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;

public class MainView extends android.opengl.GLSurfaceView 
        implements
        android.opengl.GLSurfaceView.EGLConfigChooser,
        android.opengl.GLSurfaceView.EGLContextFactory
{
    public static final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
    
    public static final int EGL_PRESERVED_RESOURCES = 0x3030; // EGL10.EGL_PRESERVED_RESOURCES,
    public static final int EGL_BIND_TO_TEXTURE_RGB = 0x3039; // EGL10.EGL_BIND_TO_TEXTURE_RGB,
    public static final int EGL_BIND_TO_TEXTURE_RGBA = 0x303A; // EGL10.EGL_BIND_TO_TEXTURE_RGBA,
    public static final int EGL_MIN_SWAP_INTERVAL = 0x303B; // EGL10.EGL_MIN_SWAP_INTERVAL,
    public static final int EGL_MAX_SWAP_INTERVAL = 0x303C; // EGL10.EGL_MAX_SWAP_INTERVAL,
    
    public static final int EGL_BACK_BUFFER = 12420;
    
    public static final int EGL_OPENGL_ES_BIT = 0x0001;
    public static final int EGL_OPENVG_BIT = 0x0002;
    public static final int EGL_OPENGL_ES2_BIT = 0x0004;
    public static final int EGL_OPENGL_BIT = 0x0008;
    

    public MainView(Context context, MainRenderer renderer)
    {
        super(context);
        initialize(renderer);
    }

    public MainView(Context context, AttributeSet attrs, MainRenderer renderer)
    {
        super(context, attrs);
        initialize(renderer);
    }
    
    private void initialize(MainRenderer renderer)
    {
        renderer_ = renderer;
        setEGLConfigChooser(this);
        setEGLContextFactory(this);
        setKeepScreenOn(true);
        setRenderer(renderer);
    }
    
    @Override
    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
    {
      //choose default configure
        int[] attribs24 =
        {
                EGL10.EGL_RED_SIZE, 8,
                EGL10.EGL_GREEN_SIZE, 8,
                EGL10.EGL_BLUE_SIZE, 8,
                EGL10.EGL_DEPTH_SIZE, 24,
                EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                //EGL10.EGL_SURFACE_TYPE, EGL10.EGL_WINDOW_BIT,
                EGL10.EGL_NONE,
        };
        
        
        int[] attribs16 =
        {
                EGL10.EGL_RED_SIZE, 5,
                EGL10.EGL_GREEN_SIZE, 6,
                EGL10.EGL_BLUE_SIZE, 5,
                EGL10.EGL_DEPTH_SIZE, 24,
                EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                //EGL10.EGL_SURFACE_TYPE, EGL10.EGL_WINDOW_BIT,
                EGL10.EGL_NONE,
        };
        
        EGLConfig[] configs = new EGLConfig[1];
        int[] numConfigs = new int[1];
        int[] value = new int[1];

        boolean ret = false;
        
//        for(int i=0; i<3; ++i){
//            attribs24[7] = 8*(4-i);
//            ret = egl.eglChooseConfig(display, attribs24, configs, 4, numConfigs);
//            if(ret && numConfigs[0]>0){
//                egl.eglGetConfigAttrib(display, configs[0], EGL10.EGL_DEPTH_SIZE, value);
//                Log.d(Tag, "depth " + value[0]);
//                return configs[0];
//            }
//        }
        
        for(int i=0; i<2; ++i){
            attribs16[7] = 8*(3-i);
            ret = egl.eglChooseConfig(display, attribs16, configs, 1, numConfigs);
            if(ret && numConfigs[0]>0){
                egl.eglGetConfigAttrib(display, configs[0], EGL10.EGL_DEPTH_SIZE, value);
                return configs[0];
            }
        }
        return null;
    }

    @Override
    public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig config)
    {
        int[] attributes = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE }; // gles2
        return egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, attributes);
        //return egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, null);
    }

    @Override
    public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context)
    {
        renderer_.onDestroy();
        egl.eglDestroyContext(display, context);
    }
    
    public MainRenderer getRenderer()
    {
        return renderer_;
    }
    
    EGLDisplay display_ = EGL10.EGL_NO_DISPLAY;
    EGLContext context_ = EGL10.EGL_NO_CONTEXT;
    EGLSurface surface_ = EGL10.EGL_NO_SURFACE;

    private SensorSystem sensorSys_ = null;
    private MainRenderer renderer_ = null;
}
