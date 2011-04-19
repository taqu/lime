package hm.orz.stabo.EGDA;

import hm.orz.stabo.EGDA.CommandManager.Command;
import hm.orz.stabo.EGDA.CommandManager.CommandFileLoad;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;


public class MainRenderer implements GLSurfaceView.Renderer
//public class MainRenderer implements android.opengl.GLSurfaceView.Renderer
{
    //private static final String Encode = "EUC-JP";
    private static final String Encode = "UTF-8";
    private static final String TextTextureFilename = "M_plus_1m_regular10.tga";

    public MainRenderer()
    {
        commandManager_ = new CommandManager();
    }

    public void onCreate(Context context)
    {
        //アセットからテクスチャデータロード
        AssetManager assetManager = context.getResources().getAssets();
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

    public void onDrawFrame(GL10 gl)
    {
        Command command = commandManager_.pop();
        if(command != null){
            processCommand(command);
            command = null;
        }

        //update frame
        EGDALib.update();
    }

    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        //initialize
        EGDALib.setViewport(width, height);
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
      //ライブラリ初期化
        {
            EGDALib.initialize(textTexture_);
        }

      //モードセット
      EGDALib.setMode(Config.getInstance().getScreenMode(), Config.getInstance().getCameraMode() );
    }

    public void onDestroy()
    {
        EGDALib.terminate();
    }

    public CommandManager getCommandManager()
    {
        return commandManager_;
    }

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
        	EGDALib.setMode(changeMode.screenMode_, changeMode.cameraMode_ );
        	break;
        }
    }

    private void fileLoad(CommandFileLoad command)
    {
        try{
            String str = command.getFilename() + "\0";
            byte[] filename = str.getBytes(Encode);

            str = command.getDirectory() + "\0";
            byte[] directory = str.getBytes(Encode);
            EGDALib.load(filename, directory, command.getResourceType());

        }catch(UnsupportedEncodingException e){
        }
    }

    private CommandManager commandManager_ = null;
    private byte[] textTexture_ = null;
}

