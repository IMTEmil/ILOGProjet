public class TestNative {

    native static void CopyWAVFileAddEffect(String szFileName, int feffect);

    static {
        System.loadLibrary("AudioEffects");
    }
    
    public static void main(String[] args)
    {
        CopyWAVFileAddEffect("audio.wav", 0x01);
    }
}
