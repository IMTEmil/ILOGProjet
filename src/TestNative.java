public class TestNative {

    native static void CopyWAVFileAddEffect(String szFileName, int feffect);

    static {
        System.loadLibrary("AudioEffects");
    }

    public static void JCopyWAVFileAddEffect(String szFileName)
    {

    }

    public static void main(String[] args)
    {
        System.out.println(args[0]);
        CopyWAVFileAddEffect(args[0], 0x01);
    }
}
