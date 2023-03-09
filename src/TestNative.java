public class TestNative {

    public static native void CopyWAVFileAddEffect(String szFileName, int feffect, double feedback, int sDelayTime);

    static {
        System.loadLibrary("AudioEffects");
    }

    public static void main(String[] args)
    {
        System.out.println(args[0]);
        CopyWAVFileAddEffect(args[0], 3, 0.5, 1);
    }
}
