package hm.orz.stabo.EGDA;

//import hm.orz.stabo.EGDA.io.SensorSystem;

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
import android.util.FloatMath;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class GLSurfaceView extends SurfaceView implements
        SurfaceHolder.Callback, Runnable
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

    public interface Renderer
    {
        public void onCreate(Context context);

        public void onSurfaceChanged(GL10 gl, int width, int height);

        public void onSurfaceCreated(GL10 gl, EGLConfig config);

        public void onDrawFrame(GL10 gl);

        public void onDestroy();
    }

    public GLSurfaceView(Context context, Renderer renderer)
    {
        super(context);
        initialize(context, renderer);
    }

    public GLSurfaceView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        initialize(context, null);
    }

    private void initialize(Context context, Renderer renderer)
    {
        surfaceHolder_ = getHolder();
        surfaceHolder_.addCallback(this);
        surfaceHolder_.setType(SurfaceHolder.SURFACE_TYPE_GPU);

        renderer_ = renderer;
        if(renderer_ != null){
            renderer_.onCreate(context);
        }

        //sensorSys_ = new SensorSystem(context);
    }

    /**
     * SurfaceHolder.Callback
     */
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
            int height)
    {
        synchronized(this){
            width_ = width;
            height_ = height;
            isResized_ = true;
        }
    }

    /**
     * SurfaceHolder.Callback
     */
    public void surfaceCreated(SurfaceHolder arg0)
    {
        if(thread_ != null){
            stop();
        }
        thread_ = new Thread(this);
        suspended_ = false;
        thread_.start();
    }

    /**
     * SurfaceHolder.Callback
     */
    public void surfaceDestroyed(SurfaceHolder arg0)
    {
        stop();
    }

    private synchronized void stop()
    {
        thread_ = null;
        notify();
    }

    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
    {
        int[] initAttribs =
        {
            EGL10.EGL_RED_SIZE, 4,
            EGL10.EGL_GREEN_SIZE, 4,
            EGL10.EGL_BLUE_SIZE, 4,
            EGL10.EGL_DEPTH_SIZE, 8,
            EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL10.EGL_NONE
        };
        
        // Get the number of minimally matching EGL configurations
        int[] numConfigs = new int[1];
        egl.eglChooseConfig(display, initAttribs, null, 0, numConfigs);

        if (numConfigs[0] <= 0) {
            throw new IllegalArgumentException("No configs match config attributes");
        }
        
        EGLConfig[] configs = new EGLConfig[numConfigs[0]];
        egl.eglChooseConfig(display, initAttribs, configs, numConfigs[0], numConfigs);


        EGLConfig  config = chooseConfig(egl, display, configs,
                5,6,5,0, 16, 0);
        
        return config;
    }

    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs,
            int red, int green, int blue, int alpha, int depth, int stencil)
    {
        int[] value = new int[1];
        
        for(EGLConfig config : configs){
            int d = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, value);
            int s = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, value);

            if(d < depth || s < stencil){
                continue;
            }

            // We want an *exact* match for red/green/blue/alpha
            int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, value);
            int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, value);
            int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, value);
            int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, value);

            if(r == red && g == green && b == blue && a == alpha)
            {
                LogOut.d("rgba:" 
                        + Integer.toString(r)
                        + Integer.toString(g)
                        + Integer.toString(b)
                        + Integer.toString(a)
                        + ", depth:"
                        + d
                        + ", stencil"
                        + s);
                return config;
            }
        }
        return null;
    }
    
    private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config,
            int attribute, int[] value)
    {

        if (egl.eglGetConfigAttrib(display, config, attribute, value)) {
            return value[0];
        }
        return 0;
    }
    
    public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig config)
    {
        int[] attributes = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE }; // gles2
        return egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT,
                attributes);
        // return egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT,
        // null);
    }

    public EGLSurface createWindowSurface(EGL10 egl, EGLDisplay display,
            EGLConfig config, SurfaceHolder window)
    {
        int[] attributes = { EGL10.EGL_RENDER_BUFFER, EGL_BACK_BUFFER, EGL10.EGL_NONE };
        return egl.eglCreateWindowSurface(display, config, window, attributes);
    }

    private void terminate(EGL10 egl)
    {
        if(display_ != EGL10.EGL_NO_DISPLAY){
            egl.eglMakeCurrent(display_, EGL10.EGL_NO_SURFACE,
                    EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);

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
        EGL10 egl = (EGL10) EGLContext.getEGL();

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

        // create context
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

        GL10 gl = (GL10) context_.getGL();

        //sensorSys_.initialize();

        renderer_.onSurfaceCreated(gl, config);
        config = null;
        
        Thread thisThread = Thread.currentThread();
        boolean resized = false;
        int width, height;
        long prevTime = System.nanoTime();
        long currentTime, diffTime;
        long t, milli;
        int nano;
        
        while(thread_ == thisThread){
            synchronized(this){
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

            EGDALib.updateTouch(touchOn_, multiTouch_, touchX_, touchY_, touchZ_);

            //EGDALib.updateOrientation( sensorSys_.rots_[1], sensorSys_.rots_[2], sensorSys_.rots_[0]); //渡す時にマッピングしなおす

            renderer_.onDrawFrame(gl);

            egl.eglSwapBuffers(display_, surface_);
            if(egl.eglGetError() == EGL11.EGL_CONTEXT_LOST){
                Context c = getContext();
                if(c instanceof Activity){
                    Activity activity = (Activity) c;
                    activity.finish();
                }
            }
            
            currentTime = System.nanoTime();
            diffTime = currentTime-prevTime;
            prevTime = currentTime;
            if(diffTime<33000000){
                if(diffTime < 0) diffTime = 0;
                diffTime = 33000000 - diffTime;

                try{
                    while(diffTime>500000){
                        milli = diffTime/1000000;
                        nano = (int)(diffTime - 1000000*milli);
                        
                        Thread.sleep(milli, nano);
                        t = System.nanoTime();
                        diffTime -= t-prevTime;
                        prevTime = t;
                    }
                }catch(InterruptedException e){
                    //e.printStackTrace();
                }
            }

            //ポーズ処理
            try{
                synchronized(this){
                    while(suspended_ && thread_ == thisThread){
                        wait();
                    }
                }
            }catch(InterruptedException e){

            }
        }

        renderer_.onDestroy();
        terminate(egl);
    }

    public boolean onTouchEvent(MotionEvent event)
    {
    	int action = event.getAction() & MotionEvent.ACTION_MASK;
        switch(action)
        {
        case MotionEvent.ACTION_MOVE:
        	if(multiTouch_){
        		float dx = event.getX(0) - event.getX(1);
        		float dy = event.getY(0) - event.getY(1);
        		touchZ_ = FloatMath.sqrt(dx*dx + dy*dy);

        	}else{
        	    touchX_ = event.getX();
                touchY_ = event.getY();
        	}
            break;

        case MotionEvent.ACTION_DOWN:
            touchX_ = event.getX();
            touchY_ = event.getY();
            touchOn_ = true;
            break;

        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_UP:
            touchX_ = event.getX();
            touchY_ = event.getY();
            touchOn_ = false;
            multiTouch_ = false;
            break;

        case MotionEvent.ACTION_POINTER_DOWN: //複数が押された
        	multiTouch_ = true;
        	touchOn_ = true;
        	float dx = event.getX(0) - event.getX(1);
    		float dy = event.getY(0) - event.getY(1);
    		touchZ_ = FloatMath.sqrt(dx*dx + dy*dy);
        	break;

        }

        return true;
    }


    public synchronized void onResume()
    {
    	//sensorSys_.onResume();
    	suspended_ = false;
    	notify();
    }

    public synchronized void onPause()
    {
    	//sensorSys_.onPause();
    	suspended_ = true;
    }

    private SurfaceHolder surfaceHolder_ = null;
    private Renderer renderer_ = null;

    private boolean isResized_ = false;
    private int width_ = 1;
    private int height_ = 1;

    private Thread thread_ = null;
    private volatile boolean suspended_ = false;

    private EGLDisplay display_ = EGL10.EGL_NO_DISPLAY;
    private EGLContext context_ = EGL10.EGL_NO_CONTEXT;
    private EGLSurface surface_ = EGL10.EGL_NO_SURFACE;

    private float touchX_ = 0.0f;
    private float touchY_ = 0.0f;
    private float touchZ_ = 0.0f;
    private boolean touchOn_ = false;
    private boolean multiTouch_ = false;

    //private SensorSystem sensorSys_ = null;
}
