package hm.orz.stabo.EGDA;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlSerializer;

import android.os.Environment;
import android.util.Xml;

/**
 * 設定データクラス。シングルトン 
 * @author taqu
 *
 */
public class Config
{
	private final String FileName = "config.xml";
	private final String XmlEncoding = "UTF-8";
	private final String Empty = "";
	private final String TagLastDirectory = "LastDirectory";
	
	/**
	 * @return 実体
	 */
	static public Config getInstance()
	{
		return instance_;
	}

	/**
	 * 初期化
	 */
	static public void initialize()
	{
		if(instance_ == null){
			instance_ = new Config();
		}
	}
	
	/**
	 * 終了
	 */
	static public void terminate()
	{
		instance_ = null;
	}
	
	public String getLastDirectory()
	{
		if(lastDirectory_.length() == 0){
			if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
				lastDirectory_ = Environment.getExternalStorageDirectory().getAbsolutePath();
			}
        }
		
		return lastDirectory_;
	}
	public void setLastDirectory(String directory)
	{
		lastDirectory_ = directory;
	}
	
	private String createPath(String appName)
	{
		File root = Environment.getExternalStorageDirectory();
        return root.getAbsolutePath() + "/" + appName + "/" + FileName;
	}
	
	/**
	 * 設定ファイルから値ロード。ファイルがない、又は外部ストレージがない場合は、デフォルト値を設定。
	 * @param appName ... アプリケーション名
	 */
	public void load(String appName)
	{
		if(false == Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
            setDefault();
            return;
        }
 
        File config = new File(createPath(appName));
        if(config.exists()){
        	
        	try{
        		FileInputStream is = new FileInputStream(config);
        		XmlPullParser xmlPullParser = Xml.newPullParser();
        		xmlPullParser.setInput(is, XmlEncoding);
        		
        		for(int event = XmlPullParser.START_DOCUMENT;
        			event != XmlPullParser.END_DOCUMENT;
        			event = xmlPullParser.next())
        		{
        			switch(event)
        			{
        			case XmlPullParser.START_TAG:
        				if(xmlPullParser.getName().equals(TagLastDirectory)){
        					lastDirectory_ = xmlPullParser.nextText();
        				}
        				break;
        			}
        		}
        		
        	}catch(FileNotFoundException e)
        	{
        	} catch (XmlPullParserException e) {
			} catch (IOException e) {
				config.delete(); //失敗したので削除する
			}
        }else{
        	setDefault();
        }
	}
	
	/**
	 * 設定ファイルをセーブ。
	 * @param appName ... アプリケーション名
	 */
	public void save(String appName)
	{
		if(false == Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
            return;
        }
		
        try{
        	String path = createPath(appName);
        	FileOutputStream os = new FileOutputStream(path, false);
        	
        	XmlSerializer xmlSerializer = Xml.newSerializer();
        	xmlSerializer.setOutput(os, XmlEncoding);
        	
        	xmlSerializer.startDocument(XmlEncoding, true);
        	xmlSerializer.startTag(Empty, TagLastDirectory);
        	xmlSerializer.text(lastDirectory_);
        	xmlSerializer.endTag(Empty, TagLastDirectory);
        	xmlSerializer.endDocument();
        	
        	os.close();
        	
        }catch(FileNotFoundException e)
        {
        }catch(IOException e)
        {
        }
	}
	
	private void setDefault()
	{
		lastDirectory_ = Empty;
	}
	
	private Config(){}

	private static Config instance_ = null; /** 実体 */
	private String lastDirectory_; /** 最後に選択したディレクトリ */
}