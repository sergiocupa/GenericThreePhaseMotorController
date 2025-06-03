using System.Globalization;
using System.Text;

namespace WinFormsApp1
{
    internal class ValueParser
    {

        public static List<DataLine> Parse(byte[] data)
        {
            var values = new List<DataLine>();

            try
            {
                var cnt = Encoding.UTF8.GetString(data);

                var temp = new List<double>();

                bool detected = false;
                int pos = 0;
                int ix = 1;
                while (ix < data.Length)
                {
                    switch (data[ix])
                    {
                        case (byte)':':
                            {
                                if (ParseValue(data, ix, pos, temp))
                                {
                                    detected = true;
                                }
                                pos = ix + 1;
                            }
                            break;
                        case (byte)'|':
                            {
                                if (detected)
                                {
                                    ParseValue(data, ix, pos, temp);
                                    detected = false;
                                }
                                pos = ix + 1;
                            }
                            break;
                        case (byte)'\n':
                            {
                                if (detected)
                                {
                                    ParseValue(data, ix, pos, temp);
                                    detected = false;
                                }
                                pos = ix + 1;

                                if (temp.Count > 1)
                                {
                                    var line = new DataLine() { Index = (long)temp[1], Values = new List<DataValue>() };

                                    int CNT = temp.Count - 1;
                                    int im = 2;
                                    while (im < CNT)
                                    {
                                        var value = new DataValue() { Address = (int)temp[im], Value = temp[im + 1] };
                                        line.Values.Add(value);

                                        im += 2;
                                    }
                                    values.Add(line);
                                }
                                temp.Clear();
                            }
                            break;
                    }
                    ix++;
                }
            }
            catch (Exception ex)
            {

            }
            return values;
        }


        public static bool ParseValue(byte[] data, int ix, int pos, List<double> temp)
        {
            int leng = ix - pos;
            Span<char> charSpan = stackalloc char[leng];
            Encoding.ASCII.GetChars(data.AsSpan(pos, leng), charSpan);

            if (double.TryParse(charSpan, NumberStyles.Float, CultureInfo.InvariantCulture, out double result))
            {
                temp.Add(result);
                return true;
            }
            return false;
        }

        
    }


    public struct DataLine
    {
        public long Index;
        public List<DataValue> Values;

        public DataLine()
        {
            Values = new List<DataValue>(0);
        }

        public override string ToString()
        {
            return Index.ToString() + " | Values: " + (Values != null ? string.Join(";", Values.Select(s => s.Address + "=" + s.Value)) : "");
        }
    }
    public struct DataValue
    {
        public int Address;
        public double Value;
    }
}
