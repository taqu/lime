package hm.orz.stabo.EGDA;

public class EGDALib
{
    static
    {
        System.loadLibrary("egda");
    }
    
    public static final int State_Stop = 0;
    public static final int State_Play = 1;
    
    public static native boolean initialize(byte[] textTexture);
    public static native void terminate();
    public static native void setViewport(int width, int height);
    public static native void update();
    
    public static native void updateOrientation(float rotx, float roty, float rotz);
    public static native void updateTouch(boolean on, float x, float y);
    
    public static native boolean load(byte[] filename, byte[] directory, int resourceType);
    
    public static native void setState(int state);
    public static native void setMode(int screenMode, int cameraMode);
}
