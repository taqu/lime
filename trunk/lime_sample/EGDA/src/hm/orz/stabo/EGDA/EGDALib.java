package hm.orz.stabo.EGDA;

public class EGDALib
{
    static
    {
        System.loadLibrary("egda");
    }

    public static final int State_Stop = 0;
    public static final int State_Play = 1;

    /**
     * 初期化
     * @return 
     * @param textTexture アルファベットテクスチャ
     */
    public static native boolean initialize(byte[] textTexture);
    
    /**
     * 終了
     */
    public static native void terminate();
    
    /**
     * 画面サイズ設定
     * @param width
     * @param height
     */
    public static native void setViewport(int width, int height);
    
    /**
     * 更新
     */
    public static native void update();

    /**
     * 回転入力
     * @param rotx
     * @param roty
     * @param rotz
     */
    public static native void updateOrientation(float rotx, float roty, float rotz);
    
    /**
     * タッチイベント
     * @param on
     * @param multiOn
     * @param x
     * @param y
     * @param z
     */
    public static native void updateTouch(boolean on, boolean multiOn, float x, float y, float z);

    /**
     * ロード要求
     * @param filename
     * @param directory
     * @param resourceType
     * @return
     */
    public static native boolean load(byte[] filename, byte[] directory, int resourceType);
    
    /**
     * ロード更新
     * @return ロード完了したらtrue
     */
    public static native boolean updateLoad();
    
    /**
     * ロードキャンセル
     */
    public static native void cancelLoad();

    /**
     * 内部状態変更
     * @param state
     */
    public static native void setState(int state);

    /**
     * スクリーン、カメラモード、アルファテスト、テクスチャ圧縮変更
     * @param screenMode
     * @param cameraMode
     * @param alphaTest
     * @param mipmap
     * @param physics
     * @param texComp
     */
    public static native void setMode(int screenMode, int cameraMode, boolean alphaTest, boolean mipmap, boolean physics, boolean texComp);
}
