package hm.orz.stabo.EGDA;

public class EGDALib
{
    static
    {
        System.loadLibrary("egda");
    }
    
    public static native boolean initialize();
    public static native void terminate();
    public static native void setViewport(int width, int height);
    public static native void update();
    
    public static native boolean load(byte[] path, byte[] directory, int resourceType);
}
