public class TestNative {

    public static native void CopyWAVFileAddEffect(String szFileName, int feffect, double feedback, int sDelayTime);

    static {
        System.loadLibrary("AudioEffects");
    }

    public static void main(String[] args)
    {
        if (args.length == 4)
        {
            CopyWAVFileAddEffect(args[0], Integer.parseInt(args[1]), Double.parseDouble(args[2]), Integer.parseInt(args[3]));
        }
        if (args.length == 2) 
        {
            CopyWAVFileAddEffect(args[0], Integer.parseInt(args[1]), 0, 0);
        }
    }
}
