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
     * ������
     * @return 
     * @param textTexture �A���t�@�x�b�g�e�N�X�`��
     */
    public static native boolean initialize(byte[] textTexture);
    
    /**
     * �I��
     */
    public static native void terminate();
    
    /**
     * ��ʃT�C�Y�ݒ�
     * @param width
     * @param height
     */
    public static native void setViewport(int width, int height);
    
    /**
     * �X�V
     */
    public static native void update();

    /**
     * ��]����
     * @param rotx
     * @param roty
     * @param rotz
     */
    public static native void updateOrientation(float rotx, float roty, float rotz);
    
    /**
     * �^�b�`�C�x���g
     * @param on
     * @param multiOn
     * @param x
     * @param y
     * @param z
     */
    public static native void updateTouch(boolean on, boolean multiOn, float x, float y, float z);

    /**
     * ���[�h�v��
     * @param filename
     * @param directory
     * @param resourceType
     * @return
     */
    public static native boolean load(byte[] filename, byte[] directory, int resourceType);
    
    /**
     * ���[�h�X�V
     * @return ���[�h����������true
     */
    public static native boolean updateLoad();
    
    /**
     * ���[�h�L�����Z��
     */
    public static native void cancelLoad();

    /**
     * ������ԕύX
     * @param state
     */
    public static native void setState(int state);

    /**
     * �X�N���[���A�J�������[�h�A�A���t�@�e�X�g�A�e�N�X�`�����k�ύX
     * @param screenMode
     * @param cameraMode
     * @param alphaTest
     * @param mipmap
     * @param physics
     * @param texComp
     */
    public static native void setMode(int screenMode, int cameraMode, boolean alphaTest, boolean mipmap, boolean physics, boolean texComp);
}
