package hm.orz.stabo.EGDA;

public class Resource
{
    public static final int Type_None = -1;
    //public static final int Type_Model = 0;
    //public static final int Type_Anim = 1;
    public static final int Type_Pmm = 0;
    public static final int Type_Num = 1;
    
    public static final String Type_Ext[] =
    {
        //"pmd",
        //"vmd",
    	"pmm",
    };
    
    public static int getTypeFromExtension(String path)
    {
        //後ろから'.'を探す
        int pos = -1;
        for(pos=path.length()-1; 0<=pos; --pos){
            if(path.charAt(pos) == '.'){
                break;
            }
        }
        
        // 拡張子がなかったら失敗
        if(pos<0 || (path.length()+1)<=pos){
            return Type_None;
        }
        
        String ext = path.substring(pos+1);
        
        for(int i=0; i<Type_Num; ++i){
            if(ext.equals(Type_Ext[i])){
                return i;
            }
        }
        return Type_None;
    }
    
    public static String getDirectoryPath(String path)
    {
      //後ろから'/'を探す
        int pos = -1;
        for(pos=path.length()-1; 0<=pos; --pos){
            if(path.charAt(pos) == '/'){
                break;
            }
        }
        
        // '/'なかったらカレント
        if(pos<0 || (path.length()+1)<=pos){
            return "./";
        }
        
        String ret = path.substring(0, pos+1);
        return ret;
    }
}
