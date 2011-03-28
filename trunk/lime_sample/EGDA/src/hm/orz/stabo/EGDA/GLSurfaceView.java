package hm.orz.stabo.EGDA;

import hm.orz.stabo.EGDA.io.SensorSystem;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGL11;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class GLSurfaceView extends SurfaceView implements SurfaceHolder.Callback, Runnable
{
    public static final String Tag = "LIME";
    
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
    
    
    private static final int WaitCountForThreadTermination = 5;
    private static final int WaitTimeForThreadTermination = 1000;
    
    public interface Renderer
    {
        public void onSurfaceChanged(GL10 gl, int width, int height);
        public void onSurfaceCreated(GL10 gl, EGLConfig config);
        
        public void onDrawFrame(GL10 gl);
        public void onDestroy();
    }
    
    public GLSurfaceView(Context context)
    {
        super(context);
        initialize(context);
    }
    
    public GLSurfaceView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        initialize(context);
    }

    private void initialize(Context context)
    {
        surfaceHolder_ = getHolder();
        surfaceHolder_.addCallback(this);
        surfaceHolder_.setType(SurfaceHolder.SURFACE_TYPE_GPU);
        
        //sensorSys_ = new SensorSystem(context);
    }
    
    public void setRenderer(Renderer renderer)
    {
        renderer_ = renderer;
    }

    
    /**
     *  SurfaceHolder.Callback
     */
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        synchronized(this)
        {
            width_ = width;
            height_ = height;
            isResized_ = true;
        }
    }

    /**
     *  SurfaceHolder.Callback
     */
    public void surfaceCreated(SurfaceHolder arg0)
    {
        thread_ = new Thread(this);
        thread_.start();
    }

    /**
     *  SurfaceHolder.Callback
     */
    public void surfaceDestroyed(SurfaceHolder arg0)
    {
        synchronized(this)
        {
            isRunnging_ = false;
        }
        
        try{
            if(thread_ != null){
                int count = 0;
                do{
                    thread_.join(WaitTimeForThreadTermination);
                    if(thread_.isAlive() == false){
                        break;
                    }
                }while(++count < WaitCountForThreadTermination);
            }
        }catch(InterruptedException ex)
        {
        }
        thread_ = null;
    }

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
                EGL10.EGL_SURFACE_TYPE, EGL10.EGL_WINDOW_BIT,
                EGL10.EGL_NONE,
        };
        
        
        int[] attribs16 =
        {
                EGL10.EGL_RED_SIZE, 5,
                EGL10.EGL_GREEN_SIZE, 6,
                EGL10.EGL_BLUE_SIZE, 5,
                EGL10.EGL_DEPTH_SIZE, 24,
                EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL10.EGL_SURFACE_TYPE, EGL10.EGL_WINDOW_BIT,
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
            attribs16[7] = 8*(2-i);
            ret = egl.eglChooseConfig(display, attribs16, configs, 1, numConfigs);
            if(ret && numConfigs[0]>0){
                egl.eglGetConfigAttrib(display, configs[0], EGL10.EGL_DEPTH_SIZE, value);
                LogOut.d("depth " + value[0]);
                return configs[0];
            }
        }
        return null;
    }
    
    public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig config)
    {
        int[] attributes = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE }; // gles2
        return egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, attributes);
        //return egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, null);
    }
    
    public EGLSurface createWindowSurface(EGL10 egl, EGLDisplay display, EGLConfig config, SurfaceHolder window)
    {
        int[] attributes = { EGL10.EGL_RENDER_BUFFER, EGL_BACK_BUFFER, EGL10.EGL_NONE };
        return egl.eglCreateWindowSurface(display, config, window, attributes);
    }
    
    private void terminate(EGL10 egl)
    {
        if(display_ != EGL10.EGL_NO_DISPLAY){
            egl.eglMakeCurrent(display_, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
            
            if(surface_ != EGL10.EGL_NO_SURFACE){
                egl.eglDestroySurface(display_, surface_);
            }

            if(context_ != EGL10.EGL_NO_CONTEXT){
                egl.eglDestroyContext(display_, context_);
            }
            
            egl.eglTerminate(display_);
        }
        display_ = EGL10.EGL_NO_DISPLAY;
        context_ = EGL10.EGL_NO_CONTEXT;
        surface_ = EGL10.EGL_NO_SURFACE;
        
        //sensorSys_.terminate();
    }

    public void run()
    {        
        EGL10 egl = (EGL10)EGLContext.getEGL();
        
        terminate(egl);
        
        display_ = egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
        if(EGL10.EGL_NO_DISPLAY == display_){
            terminate(egl);
            return;
        }

        if(false == egl.eglInitialize(display_, null)){
            terminate(egl);
            return;
        }
        
        EGLConfig config = chooseConfig(egl, display_);
        if(null == config){
            terminate(egl);
            return;
        }
        
        //create context
        context_ = createContext(egl, display_, config);
        if(EGL10.EGL_NO_CONTEXT == context_){
            terminate(egl);
            return;
        }

        surface_ = createWindowSurface(egl, display_, config, surfaceHolder_);
        if(EGL10.EGL_NO_SURFACE == surface_){
            terminate(egl);
            return;
        }
        
        if(false == egl.eglMakeCurrent(display_, surface_, surface_, context_)){
            terminate(egl);
        }
        
        GL10 gl = (GL10)context_.getGL();

        //sensorSys_.initialize();
        
        renderer_.onSurfaceCreated(gl, config);

        config = null;
        isRunnging_ = true;
        boolean resized = false;
        int width, height;
        while(isRunnging_)
        {
            synchronized(this)
            {
                resized = isResized_;
            }

            if(resized){
                synchronized(this){
                    isResized_ = false;
                    width = width_;
                    height = height_;
                }
                renderer_.onSurfaceChanged(gl, width, height);
            }
            renderer_.onDrawFrame(gl);
;
            egl.eglSwapBuffers(display_, surface_);
            if(egl.eglGetError() == EGL11.EGL_CONTEXT_LOST){
                Context c = getContext();
                if(c instanceof Activity){
                    Activity activity = (Activity)c;
                    activity.finish();
                }
            }
        }

        renderer_.onDestroy();
        terminate(egl);
    }
    
    SurfaceHolder surfaceHolder_ = null;
    Renderer renderer_ = null;
    
    boolean isResized_ = false;
    boolean isRunnging_ = false;
    int width_ = 1;
    int height_ = 1;
    
    Thread thread_ = null;
    
    EGLDisplay display_ = EGL10.EGL_NO_DISPLAY;
    EGLContext context_ = EGL10.EGL_NO_CONTEXT;
    EGLSurface surface_ = EGL10.EGL_NO_SURFACE;
    
    SensorSystem sensorSys_ = null;
}
