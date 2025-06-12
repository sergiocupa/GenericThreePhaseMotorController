using System.Runtime.InteropServices;

namespace WinFormsApp1
{

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct LogFrame
    {
        public byte  Start01;
        public byte  Start02;
        public uint  FrameSize;
        public uint  Counter;
        public uint  TicksByIteration;
        public float Adc01;
        public float Adc02;
        public float Adc03;
        public float TotalCurrent;
        public float Frequency;
        public float MotionStep;
        public float Theta;
        public float WaveR;
        public float WaveS;
        public float WaveT;

        // Padding bytes to align the structure size to 64 bytes
        public uint Pad1;
        public uint Pad2;
        public byte Pad3;
        public byte Pad4;

        public override string ToString()
        {
            return "LogFrame: " +
                   $"Start01: {Start01}, Start02: {Start02}, Size: {FrameSize}, " +
                   $"Counter: {Counter}, Ticks: {TicksByIteration}, " +
                   $"A01: {Adc01}, A02: {Adc02}, A03: {Adc03}, " +
                   $"Current: {TotalCurrent}, Freq: {Frequency}, " +
                   $"Step: {MotionStep}, Theta: {Theta}, R: {WaveR}, S: {WaveS}, T: {WaveT}";
        }
    }


    internal class LogFrameParser
    {

        static byte[] Remainder = new byte[0];


        public static unsafe List<LogFrame> Parse(byte[] data)
        {
            var values = new List<LogFrame>();

            if (Remainder.Length > 0)
            {
                data = Remainder.Concat(data).ToArray();
                Remainder = Array.Empty<byte>();
            }

            fixed (byte* pData = data)
            {
                int ix = 0;

                while ((ix + LogFrameSize) <= data.Length)
                {
                    byte a = pData[ix];
                    byte b = pData[ix + 1];
                    byte c = pData[ix + 2];

                    // Checar cabeçalho
                    if ((a == 254 && b == 253) && c == LogFrameSize)
                    {
                        LogFrame* framePtr = (LogFrame*)(pData + ix);
                        LogFrame frame = *framePtr;
                        values.Add(frame);

                        ix += LogFrameSize;
                        continue;
                    }
                    else
                    {
                        // Dados incompletos ou fora de alinhamento
                        int remaining = data.Length - ix;
                        Remainder = new byte[remaining];
                        Buffer.BlockCopy(data, ix, Remainder, 0, remaining);
                        break;
                    }
                }
            }

            return values;
        }


        private static readonly int LogFrameSize;

        static LogFrameParser()
        {
            LogFrameSize = Marshal.SizeOf<LogFrame>();
        }

    }
}
