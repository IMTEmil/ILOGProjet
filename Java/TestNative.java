public class TestNative {

    public static native void CopyWAVFileAddEffect(String szFileName, int feffect, double feedback, int sDelayTime);

    static {
        System.loadLibrary("AudioEffects");
    }

    public static void main(String[] args)
    {
        CopyWAVFileAddEffect(args[0], 3, 0.5, 1);

        CopyWAVFileAddEffect("o_" + args[0], 1, 0.5, 1);
    }
}
