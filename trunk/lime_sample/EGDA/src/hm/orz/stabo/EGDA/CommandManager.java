package hm.orz.stabo.EGDA;

import java.io.File;
import java.util.LinkedList;

public class CommandManager
{
    public static final int Command_FileLoad = 0;
    public static final int Command_Num = 1;

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
    
    public static Command createFileLoad(CommandManager parent, File file, int type)
    {
        String directory = Resource.getDirectoryPath(file.getPath());
        
        CommandFileLoad command = parent.new CommandFileLoad();
        command.setFilename(file.getName());
        command.setDirectory(directory);
        command.setResourceType(type);
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
            ret = commandList_.removeLast();
        }
        return ret;
    }
    
    public synchronized int size()
    {
        return commandList_.size();
    }
    
    private LinkedList<Command> commandList_;
}
