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
 * �ݒ�f�[�^�N���X�B�V���O���g�� 
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
	 * @return ����
	 */
	static public Config getInstance()
	{
		return instance_;
	}

	/**
	 * ������
	 */
	static public void initialize()
	{
		if(instance_ == null){
			instance_ = new Config();
		}
	}
	
	/**
	 * �I��
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
	 * �ݒ�t�@�C������l���[�h�B�t�@�C�����Ȃ��A���͊O���X�g���[�W���Ȃ��ꍇ�́A�f�t�H���g�l��ݒ�B
	 * @param appName ... �A�v���P�[�V������
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
				config.delete(); //���s�����̂ō폜����
			}
        }else{
        	setDefault();
        }
	}
	
	/**
	 * �ݒ�t�@�C�����Z�[�u�B
	 * @param appName ... �A�v���P�[�V������
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

	private static Config instance_ = null; /** ���� */
	private String lastDirectory_; /** �Ō�ɑI�������f�B���N�g�� */
}