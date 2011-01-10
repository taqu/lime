package hm.orz.stabo.EGDA;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.os.SystemClock;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.widget.TextView;

/**
 * 
 * @author taqu_2
 * 
 */
public class MainView extends GLSurfaceView
{
	
	static final int EGL_PRESERVED_RESOURCES = 0x3030; // EGL10.EGL_PRESERVED_RESOURCES,
    static final int EGL_BIND_TO_TEXTURE_RGB = 0x3039; // EGL10.EGL_BIND_TO_TEXTURE_RGB,
    static final int EGL_BIND_TO_TEXTURE_RGBA = 0x303A; // EGL10.EGL_BIND_TO_TEXTURE_RGBA,
    static final int EGL_MIN_SWAP_INTERVAL = 0x303B; // EGL10.EGL_MIN_SWAP_INTERVAL,
    static final int EGL_MAX_SWAP_INTERVAL = 0x303C; // EGL10.EGL_MAX_SWAP_INTERVAL,

	/**
	 * 
	 * @author taqu_2 GLES2.0のためのコンテキストファクトリ
	 */
	private static class EGLContextFactory2 implements GLSurfaceView.EGLContextFactory
    {
		public EGLContext createContext(EGL10 egl, EGLDisplay display,
				EGLConfig eglConfig) {
			int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

			// コンテキスト作成
			int[] attributes = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE }; // 2.0指定
			EGLContext context = egl.eglCreateContext(display, eglConfig,
					EGL10.EGL_NO_CONTEXT, attributes);

			return context;
		}

		public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context)
		{
			//EGLSurface surface = egl.eglGetCurrentSurface(EGL10.EGL_DRAW);
			
			//egl.eglMakeCurrent(display, draw, read, context);
		    //ライブラリ終了
	        EGDALib.terminate();
			egl.eglDestroyContext(display, context);
		}
	}

	/**
	 * 
	 * @author taqu_2
	 * 
	 */
	private static class EGLConfigChooser2 implements GLSurfaceView.EGLConfigChooser
	{
		public EGLConfigChooser2(
        		int redSize,
        		int greenSize,
        		int blueSize,
        		int alphaSize,
        		int depthSize,
        		int stencilSize)
        {
        	redSize_ = redSize;
            greenSize_ = greenSize;
            blueSize_ = blueSize;
            alphaSize_ = alphaSize;
            depthSize_ = depthSize;
            stencilSize_ = stencilSize;
        }

		private static int EGL_OPENGL_ES_BIT = 0x0001;
		private static int EGL_OPENVG_BIT = 0x0002;
		private static int EGL_OPENGL_ES2_BIT = 0x0004;
		private static int EGL_OPENGL_BIT = 0x0008;

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
			int[] numConfigs = new int[1];

			int[] attributes = new int[9];
			attributes[0] = EGL10.EGL_RED_SIZE;
			attributes[1] = 4;
			attributes[2] = EGL10.EGL_GREEN_SIZE;
			attributes[3] = 4;
			attributes[4] = EGL10.EGL_BLUE_SIZE;
			attributes[5] = 4;
			attributes[6] = EGL10.EGL_RENDERABLE_TYPE;
			attributes[7] = EGL_OPENGL_ES2_BIT;
			attributes[8] = EGL10.EGL_NONE;

			egl.eglChooseConfig(display, attributes, null, 0, numConfigs);

			if (numConfigs[0] <= 0) {
				return null;
			}

			EGLConfig[] configs = new EGLConfig[numConfigs[0]];
			egl.eglChooseConfig(display, attributes, configs, numConfigs[0],
					numConfigs);

			int[] value = new int[1];
			boolean ret = false;
			for (int i = 0; i < numConfigs[0]; ++i) {
				EGLConfig config = configs[i];
				ret = egl.eglGetConfigAttrib(display, config, EGL10.EGL_DEPTH_SIZE, value);
				if(!ret || value[0] < depthSize_){
					continue;
				}
				
				ret = egl.eglGetConfigAttrib(display, config, EGL10.EGL_STENCIL_SIZE, value);
				if(!ret || value[0] < stencilSize_){
					continue;
				}
				
				ret = egl.eglGetConfigAttrib(display, config, EGL10.EGL_RED_SIZE, value);
				if(!ret || value[0] != redSize_){
					continue;
				}
				
				ret = egl.eglGetConfigAttrib(display, config, EGL10.EGL_GREEN_SIZE, value);
				if(!ret || value[0] != greenSize_){
					continue;
				}
				
				ret = egl.eglGetConfigAttrib(display, config, EGL10.EGL_BLUE_SIZE, value);
				if(!ret || value[0] != blueSize_){
					continue;
				}
				
				ret = egl.eglGetConfigAttrib(display, config, EGL10.EGL_ALPHA_SIZE, value);
				if(!ret || value[0] != alphaSize_){
					continue;
				}
				return config;
			}
			return null;
		}

		int redSize_;
		int greenSize_;
		int blueSize_;
		int alphaSize_;
		int depthSize_;
		int stencilSize_;
	}

	/**
	 * 
	 * @param context
	 */
	public MainView(Context context, boolean translucent, int redSize, int greenSize, int blueSize, int alphaSize, int depthSize, int stencilSize)
	{
		super(context);
		
		if (translucent) {
			this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
		}
		
		setEGLContextFactory(new EGLContextFactory2());
		EGLConfigChooser2 chooser = new EGLConfigChooser2(redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize);
		setEGLConfigChooser(chooser);

		renderer_ = new MainRenderer();
		setRenderer(renderer_);
	}
	
	/**
	 * タッチパネルイベント
	 * @return
	 */
	public boolean onTouchEvent(final MotionEvent event)
	{
		queueEvent(new Runnable(){
			public void run()
			{
				renderer_.onTouchEvent(event);
			}
		});
		return true;
	}
	
	public CommandManager getCommandManager()
	{
	    return renderer_.getCommandManager();
	}
	
	private MainRenderer renderer_;
}
