package hm.orz.stabo.EGDA;

import java.io.File;
import java.util.LinkedList;

public class CommandManager
{
    public static final int Command_FileLoad = 0;
    public static final int Command_ChangeState = 1;
    public static final int Command_ChangeMode = 2;
    public static final int Command_Num = 3;

    public class Command
    {
        protected Command()
        {
        }

        public int getType()
        {
            return type_;
        }

        protected int type_ = 0;
    }

    public class CommandFileLoad extends Command
    {
        public CommandFileLoad()
        {
            type_ = Command_FileLoad;
        }

        public String getFilename()
        {
            return filename_;
        }

        public void setFilename(String filename)
        {
        	filename_ = filename;
        }

        public String getDirectory()
        {
            return dir_;
        }

        public void setDirectory(String directory)
        {
            dir_ = directory;
        }

        public int getResourceType()
        {
            return resourceType_;
        }

        public void setResourceType(int resourceType)
        {
            resourceType_ = resourceType;
        }

        private int resourceType_ = 0;
        private String filename_ = null;
        private String dir_ = null;
    }

    public class CommandChangeState extends Command
    {
        protected CommandChangeState(int state)
        {
        	type_ = Command_ChangeState;
        	state_ = state;
        }

        public int state_ = 0;
    }

    public class CommandChangeMode extends Command
    {
        protected CommandChangeMode(int screenMode, int cameraMode, boolean isAlphaTest, boolean isMipmap, boolean isPhysics, boolean isTextureCompress)
        {
        	type_ = Command_ChangeMode;
        	screenMode_ = screenMode;
        	cameraMode_ = cameraMode;
        	isAlphaTest_ = isAlphaTest;
        	isMipmap_ = isMipmap;
        	isPhysics_ = isPhysics;
        	isTextureCompress_ = isTextureCompress;
        }

        public int screenMode_ = 0;
        public int cameraMode_ = 0;
        public boolean isAlphaTest_ = false;
        public boolean isMipmap_ = false;
        public boolean isPhysics_ = false;
        public boolean isTextureCompress_ = false;
    }


    public static Command createFileLoad(CommandManager parent, File file, int type)
    {
        String directory = ResourceManager.getDirectoryPath(file.getPath());

        CommandFileLoad command = parent.new CommandFileLoad();
        command.setFilename(file.getName());
        command.setDirectory(directory);
        command.setResourceType(type);
        return command;
    }

    public static Command createChangeState(CommandManager parent, int state)
    {
        CommandChangeState command = parent.new CommandChangeState(state);
        return command;
    }

    public static Command createChangeMode(CommandManager parent, int screenMode, int cameraMode, boolean isAlphaTest, boolean isMipmap, boolean isPhysics, boolean isTextureCompress)
    {
        CommandChangeMode command = parent.new CommandChangeMode(screenMode, cameraMode, isAlphaTest, isMipmap, isPhysics, isTextureCompress);
        return command;
    }

    public CommandManager()
    {
        commandList_ = new LinkedList<Command>();
    }

    public synchronized void push(Command command)
    {
        assert(command != null);
        commandList_.addLast(command);
    }

    public synchronized Command pop()
    {
        Command ret = null;
        if(commandList_.size()>0){
            ret = commandList_.removeFirst(); //FIFO
        }
        return ret;
    }

    public synchronized int size()
    {
        return commandList_.size();
    }

    private LinkedList<Command> commandList_;
}
