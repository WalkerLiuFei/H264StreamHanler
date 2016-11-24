package walker.person.mediahandlerlib.example;

import android.util.Log;

import java.awt.font.TextAttribute;
import java.lang.reflect.Type;
import java.util.Arrays;

import walker.person.mediahandlerlib.MainActivity;

/**
 * Created by Administrator on 2016/11/22 0022.
 */
public class Demux {
    private static final String TAG = "Demux";
    private byte[] extraDatas;
    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("MyLibrary");
    }

    private MainActivity demuxCallbackInterface;

    public native boolean initNativa();
    public native void demux(String path);
    //save sps pps data
    public void getExtraDatacallBack(byte[] bytes,int extraDataLength){
        extraDatas =  Arrays.copyOfRange(bytes,0, extraDataLength);
    }
    //call back of IDR frame....
    public void getIDRDataCallBack(byte[] bytes,int length){
        byte[] newArray = new byte[length + extraDatas.length];
        System.arraycopy(extraDatas,0,newArray,0,extraDatas.length);
        System.arraycopy(bytes,0,newArray,extraDatas.length,length);
        if(this.demuxCallbackInterface != null)
            this.demuxCallbackInterface.callback(newArray);
    }
    public void callback(byte[] bytes,int length){
        if(this.demuxCallbackInterface != null)
            this.demuxCallbackInterface.callback(Arrays.copyOfRange(bytes,0,length));
        //System.out.println("call back is success!");
    }

    public void setDemuxCallbackInterface(MainActivity demuxCallbackInterface) {
        this.demuxCallbackInterface = demuxCallbackInterface;
    }
}
