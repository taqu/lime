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
	private static final String FileName = "config.xml";
	private static final String XmlEncoding = "UTF-8";
	private static final String Empty = "";

	private static final String TagConfig = "config";
	private static final String TagLastDirectory = "last_directory";
	private static final String TagCameraMode = "camera_mode";
	private static final String TagScreenMode = "screen_mode";
	private static final String TagAlphaTest = "alpha_test";
	private static final String TagMipmap = "mipmap";
	private static final String TagTextureCompress = "tex_compress";

	public static final int Camera_Manual = 0;
	public static final int Camera_Animation = 1;

	public static final int Screen_Rot0 = 0;
	public static final int Screen_Rot90 = 1;
	public static final int Screen_Rot180 = 2;
	public static final int Screen_Rot270 = 3;


	//320�~240 //QVGA 4:3
	//400�~240 //WQVGA 16:9
	//800�~480 //WVGA 15:9
	//854�~480 //FWVGA 16:9
	//800�~600 //SVGA 4:3
	//1024�~576 //WSVGA 16:9

	private int clamp(int v, int min, int max)
	{
	    if(v<min){
	        return min;
	    }else if(v>max){
	        return max;
	    }
	    return v;
	}

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

	public int getCameraMode()
	{
	    return cameraMode_;
	}

	public void setCameraMode(int mode)
	{
	    cameraMode_ = mode;
	}

	public int getScreenMode()
	{
	    return screenMode_;
	}

	public void setScreenMode(int mode)
	{
	    screenMode_ = mode;
	}
	
	public boolean isAlhpaTest()
	{
	    return alphaTest_;
	}
	
	public void setAlphaTest(boolean alphaTest)
	{
	    alphaTest_ = alphaTest;
	}
	
	public boolean isMipmap()
	{
		return mipmap_;
	}
	
	public void setMipmap(boolean mipmap)
	{
		mipmap_ = mipmap;
	}

	public boolean isTextureCompress()
    {
        return textureCompress_;
    }

    public void setTextureCompress(boolean textureCompress)
    {
        textureCompress_ = textureCompress;
    }


	private String createPath(String appName)
	{
		File root = Environment.getExternalStorageDirectory();

		//�f�B���N�g�����Ȃ�������쐬
		String appDirPath = root.getAbsolutePath() + "/" + appName + "/";
		File appDirectory = new File(appDirPath);
		if(appDirectory.exists()){
		    return appDirPath + FileName;
		}

		if(appDirectory.mkdir()){
		    return appDirPath + FileName;
		}
		return null;
	}

	/**
	 * �ݒ�t�@�C������l���[�h�B�t�@�C�����Ȃ��A���͊O���X�g���[�W���Ȃ��ꍇ�́A�f�t�H���g�l��ݒ�B
	 * @param appName ... �A�v���P�[�V������
	 */
	public void load(String appName)
	{
	    setDefault();
		if(false == Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
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

        				}else if(xmlPullParser.getName().equals(TagCameraMode)){
        				    cameraMode_ = Integer.parseInt( xmlPullParser.nextText() );
        				    cameraMode_ = clamp(cameraMode_, 0, 1);

                        }else if(xmlPullParser.getName().equals(TagScreenMode)){
                            screenMode_ = Integer.parseInt( xmlPullParser.nextText() );
                            screenMode_ = clamp(screenMode_, 0, 3);
                            
                        }else if(xmlPullParser.getName().equals(TagAlphaTest)){
                            alphaTest_ = Boolean.parseBoolean(xmlPullParser.nextText() );
                            
                        } else if(xmlPullParser.getName().equals(TagMipmap)){
                        	mipmap_ = Boolean.parseBoolean(xmlPullParser.nextText());
                        	
                        }else if(xmlPullParser.getName().equals(TagTextureCompress)){
                            textureCompress_ = Boolean.parseBoolean( xmlPullParser.nextText() );
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
        	if(path == null){
        	    return;
        	}
        	FileOutputStream os = new FileOutputStream(path, false);

        	XmlSerializer xmlSerializer = Xml.newSerializer();
        	xmlSerializer.setOutput(os, XmlEncoding);

        	xmlSerializer.startDocument(XmlEncoding, true);

        	xmlSerializer.startTag(Empty, TagConfig);

        	xmlSerializer.startTag(Empty, TagLastDirectory);
        	xmlSerializer.text(lastDirectory_);
        	xmlSerializer.endTag(Empty, TagLastDirectory);

        	xmlSerializer.startTag(Empty, TagCameraMode);
            xmlSerializer.text( Integer.toString(cameraMode_) );
            xmlSerializer.endTag(Empty, TagCameraMode);

            xmlSerializer.startTag(Empty, TagScreenMode);
            xmlSerializer.text( Integer.toString(screenMode_) );
            xmlSerializer.endTag(Empty, TagScreenMode);

            xmlSerializer.startTag(Empty, TagAlphaTest);
            xmlSerializer.text( Boolean.toString(alphaTest_) );
            xmlSerializer.endTag(Empty, TagAlphaTest);

            xmlSerializer.startTag(Empty, TagMipmap);
            xmlSerializer.text( Boolean.toString(mipmap_) );
            xmlSerializer.endTag(Empty, TagMipmap);
            
            xmlSerializer.startTag(Empty, TagTextureCompress);
            xmlSerializer.text( Boolean.toString(textureCompress_) );
            xmlSerializer.endTag(Empty, TagTextureCompress);
            
            xmlSerializer.endTag(Empty, TagConfig);

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
		cameraMode_ = Camera_Animation;
	    screenMode_ = Screen_Rot0;
	    alphaTest_ = false;
	    mipmap_ = false;
	    textureCompress_ = false;
	}

	private Config(){}

	private static Config instance_ = null; /** ���� */

	private String lastDirectory_; /** �Ō�ɑI�������f�B���N�g�� */
	private int cameraMode_ = Camera_Animation;
	private int screenMode_ = Screen_Rot0;
	private boolean alphaTest_ = false;
	private boolean mipmap_ = false;
	private boolean textureCompress_ = false;
}
