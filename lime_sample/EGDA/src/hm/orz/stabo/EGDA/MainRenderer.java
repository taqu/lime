package hm.orz.stabo.EGDA;

import hm.orz.stabo.EGDA.CommandManager.Command;
import hm.orz.stabo.EGDA.CommandManager.CommandFileLoad;

import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.content.res.AssetManager;


/**
 * �X�V�����N���X
 * @author taqu
 *
 */
public class MainRenderer implements GLSurfaceView.Renderer
//public class MainRenderer implements android.opengl.GLSurfaceView.Renderer
{
    //private static final String Encode = "EUC-JP";
    private static final String Encode = "UTF-8";
    private static final String TextTextureFilename = "M_plus_1m_regular10.png";

    private static final int State_Play = 0; /// �ʏ�
    private static final int State_Load = 1; /// ���[�h��
    
    public MainRenderer()
    {
        commandManager_ = new CommandManager();
    }

    void loadTextTexture()
    {
        if(context_ == null){
            return;
        }
      //�A�Z�b�g����e�N�X�`���f�[�^���[�h
        AssetManager assetManager = context_.getResources().getAssets();
        try{
            InputStream inputStream = assetManager.open(TextTextureFilename);

            ByteArrayOutputStream buffer = new ByteArrayOutputStream();
            OutputStream out = new BufferedOutputStream(buffer);
            int c;
            while((c=inputStream.read()) != -1){
                out.write(c);
            }
            out.flush();
            out.close();
            textTexture_ = buffer.toByteArray();
        }catch(IOException e)
        {
        }
    }
    
    /**
     * ������
     */
    public void onCreate(Context context)
    {
        context_ = context;
        loadTextTexture();
        state_ = State_Play;
    }

    /**
     * ���t���[���X�V����
     */
    public void onDrawFrame(GL10 gl)
    {
        Command command = commandManager_.pop();
        if(command != null){
            processCommand(command);
            command = null;
        }

        switch(state_)
        {
        case State_Play:
            EGDALib.update();
            break;

        case State_Load:
            {//���[�h��
                //TODO: �^�C���A�b�v�����
                if(EGDALib.updateLoad()){
                    state_ = State_Play;
                }
            }
            break;
        }
    }

    /**
     * �T�[�t�F�C�X���T�C�Y������
     */
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        //initialize
        EGDALib.setViewport(width, height);
    }

    /**
     * �T�[�t�F�C�X�쐬������
     */
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        if(textTexture_ == null){
            loadTextTexture();
        }
      //���C�u����������
      EGDALib.initialize(textTexture_);

      //���[�h�Z�b�g
      Config appConfig = Config.getInstance();
      EGDALib.setMode(
    		  appConfig.getScreenMode(),
    		  appConfig.getCameraMode(),
    		  appConfig.isAlhpaTest(),
    		  appConfig.isMipmap(),
    		  appConfig.isPhysics(),
    		  appConfig.isTextureCompress() );

      textTexture_ = null;
    }

    /**
     * �`��X���b�h�I��������
     */
    public void onDestroy()
    {
        EGDALib.terminate();
    }

    /// �R�}���h�o�b�t�@�N���X�擾
    public CommandManager getCommandManager()
    {
        return commandManager_;
    }

    /**
     * ���[�U�R�}���h����
     * @param command
     */
    private void processCommand(Command command)
    {
        switch(command.getType())
        {
        case CommandManager.Command_FileLoad:
            fileLoad((CommandFileLoad)command);
            break;

        case CommandManager.Command_ChangeState:
        	CommandManager.CommandChangeState changeState = (CommandManager.CommandChangeState)command;
        	EGDALib.setState( changeState.state_ );
        	break;

        case CommandManager.Command_ChangeMode:
            CommandManager.CommandChangeMode changeMode = (CommandManager.CommandChangeMode)command;
        	EGDALib.setMode(
        			changeMode.screenMode_,
        			changeMode.cameraMode_,
        			changeMode.isAlphaTest_,
        			changeMode.isMipmap_,
        			changeMode.isPhysics_,
        			changeMode.isTextureCompress_ );
        	break;
        }
    }

    /**
     * �t�@�C�����[�h
     * @param command
     */
    private void fileLoad(CommandFileLoad command)
    {
        try{
            //�t�@�C���p�X�A�f�B���N�g���p�X�쐬�B�����R�[�h��UTF8�œ���
            String str = command.getFilename() + "\0";
            byte[] filename = str.getBytes(Encode);

            str = command.getDirectory() + "\0";
            byte[] directory = str.getBytes(Encode);
            
            //�t�@�C�����[�h�v�������ŏ�ԕύX
            if(EGDALib.load(filename, directory, command.getResourceType())){
                state_ = State_Load;
            }

        }catch(UnsupportedEncodingException e){
        }
    }

    private int state_ = State_Play; /// �������
    private Context context_ = null;
    private CommandManager commandManager_ = null;
    private byte[] textTexture_ = null;
}

