package walker.person.mediahandlerlib.video.core;

import android.graphics.Bitmap;

/**
 * Created by walker on 2016/11/18 0018.
 */
public interface DecodeCallBack {
    void decodeSuccess(byte[] decodedStream,int width,int height);
    void decodeSuccess(byte[] decodedStream);
}
