package walker.person.mediahandlerlib.video.natives;

/**
 * Created by Administrator on 2016/11/18 0018.
 */
public class H264Decoder {
    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("MyLibrary");
    }
    int mirrorData;
    public H264Decoder(){
        initDecoder();
    }

    public native boolean initDecoder();

    /**
     *
     * @param datas H264 stream data you want decode
     * @return byte[] source decoded picture data,could be use to generate bitmap
     */
    public native byte[] consumeStream(byte[] datas);

    /**
     *
     * @return
     */
    public native boolean releaseDecoder();
}
