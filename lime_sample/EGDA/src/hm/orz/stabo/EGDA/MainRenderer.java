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
 * 更新処理クラス
 * @author taqu
 *
 */
public class MainRenderer implements GLSurfaceView.Renderer
//public class MainRenderer implements android.opengl.GLSurfaceView.Renderer
{
    //private static final String Encode = "EUC-JP";
    private static final String Encode = "UTF-8";
    private static final String TextTextureFilename = "M_plus_1m_regular10.png";

    private static final int State_Play = 0; /// 通常
    private static final int State_Load = 1; /// ロード中
    
    public MainRenderer()
    {
        commandManager_ = new CommandManager();
    }

    void loadTextTexture()
    {
        if(context_ == null){
            return;
        }
      //アセットからテクスチャデータロード
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
     * 初期化
     */
    public void onCreate(Context context)
    {
        context_ = context;
        loadTextTexture();
        state_ = State_Play;
    }

    /**
     * 毎フレーム更新処理
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
            {//ロード中
                //TODO: タイムアップ入れる
                if(EGDALib.updateLoad()){
                    state_ = State_Play;
                }
            }
            break;
        }
    }

    /**
     * サーフェイスリサイズ時処理
     */
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        //initialize
        EGDALib.setViewport(width, height);
    }

    /**
     * サーフェイス作成時処理
     */
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        if(textTexture_ == null){
            loadTextTexture();
        }
      //ライブラリ初期化
      EGDALib.initialize(textTexture_);

      //モードセット
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
     * 描画スレッド終了時処理
     */
    public void onDestroy()
    {
        EGDALib.terminate();
    }

    /// コマンドバッファクラス取得
    public CommandManager getCommandManager()
    {
        return commandManager_;
    }

    /**
     * ユーザコマンド処理
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
     * ファイルロード
     * @param command
     */
    private void fileLoad(CommandFileLoad command)
    {
        try{
            //ファイルパス、ディレクトリパス作成。文字コードはUTF8で統一
            String str = command.getFilename() + "\0";
            byte[] filename = str.getBytes(Encode);

            str = command.getDirectory() + "\0";
            byte[] directory = str.getBytes(Encode);
            
            //ファイルロード要求成功で状態変更
            if(EGDALib.load(filename, directory, command.getResourceType())){
                state_ = State_Load;
            }

        }catch(UnsupportedEncodingException e){
        }
    }

    private int state_ = State_Play; /// 内部状態
    private Context context_ = null;
    private CommandManager commandManager_ = null;
    private byte[] textTexture_ = null;
}

