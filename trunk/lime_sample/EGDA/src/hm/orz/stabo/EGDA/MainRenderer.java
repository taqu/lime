package hm.orz.stabo.EGDA;

import hm.orz.stabo.EGDA.CommandManager.Command;
import hm.orz.stabo.EGDA.CommandManager.CommandFileLoad;

import java.io.UnsupportedEncodingException;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;


public class MainRenderer implements GLSurfaceView.Renderer
//public class MainRenderer implements android.opengl.GLSurfaceView.Renderer
{
    //private static final String Encode = "EUC-JP";
    private static final String Encode = "UTF-8";
    
    public MainRenderer()
    {
        commandManager_ = new CommandManager();
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
      //ÉâÉCÉuÉâÉäèâä˙âª
      EGDALib.initialize();
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
}

